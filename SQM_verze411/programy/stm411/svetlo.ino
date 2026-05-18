// Podprogramy pro praci s cidlem svetla
//================================================

#define TSL2591_COMMAND_BIT       (0xA0)                                                 // 1010 0000: bits 7 and 5 for 'command normal'

uint16_t ir, full;
float lux ;
uint32_t ulux;
double mag_arcsec2;
uint8_t TSL_cas = 5;                                                                        // cas snimani  (0=100ms, 1=200ms, 2=300ms .... 5=600ms)
uint8_t cnf;                                                                                // konfiguracni registr (slouceny cas mereni a citlivost)


//-----------------------------------------------------------------------------------------------------------------------------
// Pred kazdym zaznamenavanym merenim svetla se provadi 2 az 5 pokusu ze kterych se zjistuje, jestli je namerena hodnota svetla stabilni
//  Kdyz jsou dva po sobe jdouci vzorky podobne, vraci se 'true' a podprogram okamzite konci.
//  Kdyz ani po 5 pokusech neni 4. a 5. vzorek podobny vraci se 'false'.
//  Pri prumerovani se test provede jen na zacatku. Vlastni prumerovani uz je potom bez testu stability.
//  Pri prubeznem mereni z menu se test stability take neprovadi.
//  Test neni mozne prerusit. Pri kazdem z 5 mereni se ale testuji seriove linky, takze je mozne reagovat na prikazy zvenku
//  Tento podprogram slouzi i k tomu, aby se pripadne pri startu mereni ustalila hodnota
//  Prvni parametr urcuje, jestli se ma pri kazdem mereni bzuknout
//  druhy parametr udava, na ktere sedmisegmentovce ma pred kazdym merenim probliknout tecka
//----------------------------------------------
bool test_stability(bool bzuk, uint8_t bliktecka)
  {
    uint16_t posledni_mereni = 0;
    uint16_t pomprom;
    bool stabilni = false;
   
    for (uint8_t teststab = 1; teststab < 6 ; teststab ++)                                  // maximalne 5 pokusu o nalezeni dvou stabilnich hodnot
      {
        if (bzuk == true)                                                                // bzukani je aktivni jen v pripade spusteni tlacitkem, nebo pres kalibraci z menu
          {
            zobraz_cislo(70 - (teststab * 10), 3);                                       // ... se pred kazdym merenim blikne zbyvajici pocet vzorku pro prumerovani ve stylu "-nn-"
          }

        if (bliktecka == 4) zobraz_text(48);                                             // blikne tecka na leve sedmisegmentovce
        if (bliktecka == 3) zobraz_text(52);                                             // blikne tecka na druhe sedmisegmentovce zleva
        delay(100);             
        MODdata[31] = ((status_afd << 8) | (err_bit << 7) | (70 - (teststab * 10)));
        pomprom = svetlo_1x(false, bzuk);                                                // jedno mereni svetla s vyhledanim maximalni citlivosti snimani - nelze predcasne ukoncit
        uint16_t aktualni_mereni = korekce_svetla(pomprom); 

        if (prubeh_mereni > 0)  prubeh_mereni --;                                        // kvuli odhadu zbyvajiciho casu do linky RS485 se odesila prubeh mereni (kolik mereni jeste zbyva do konce)
        if ((rozdil_jasu(aktualni_mereni, posledni_mereni) / (float)aktualni_mereni) < (rozhod_stab / 1000.0))    // odchylka mezi soucasnym  predchozim merenim uz je v toleranci
          {
             stabilni = true;                                                            // dalsi testovani se prerusuje a vraci se znacka stabilni hodnoty
             break;
          }
         posledni_mereni = korekce_svetla(pomprom);
      }
    return stabilni;        
  }
//----------------------------------------------



//----------------------------------------------
// Jedno mereni svetla.
// Zakladni vystup je bez korekce, ale kvuli nastaveni jasu displeje se uvnitr provadi vnitrni korekce podle kalibracni tabulky.
// vystupem je 1000x mag_arcsec2, ktera se pak muze dal zpracovavat jako uint16_t
// dalsimi vystupy jsou globalni promenne 'int_infra', 'int_full' a 'byte_cnf'
// parametr 'mozno prerusit' umoznuje pri mereni svetla bez zapisu do EEPROM (jen pri zobrazeni na displeji) prerusit nedokoncene mereni pomoci tlacitka [OK]
//----------------------------------------------
uint16_t svetlo_1x(bool mozno_prerusit, bool bzuk)
  {
    afd_nabito = false;                                                                  // blokovani testu Alarm for Darkness
    uint16_t vystup_svetla;
    bool bylo_stisknuto_tlacitko = false;
    zobraz_text(66);                                                                     // pred zacatkem mereni se displej zhasina
    if (bitRead(signal_LED,1) == true)  blik_Y_kratky();                                 // kdyz je povoleno blikani RGB LED pri mereni, blikne se kratce zlute

    // cekani na uvolneni vsech tlacitek
    while (digitalRead(pin_tl_ok) == LOW or digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW)
      {
        delay(10);
        if (kalibruje_se == true) break;
      }


    while (true)                                                                         // hledani takove citlivosti, ktera mereni neprebudi (vypadne se v okamziku, kdy uz neni mereni prebuzene, nebo kdyz je minimalni citlivost)
      {
        if (bzuk == true)    tone_X(pin_bzuk,450, 10 , 1);                               // pokud je pipani pro mereni povolene, tak se pipne
        // ---- mereni svetla ---
        cnf = pole_citlivosti[posledni_citlivost] + TSL_cas;                             // zacina se posledni znamou citlivosti
        TSL_config(cnf); 
        TSL_enable();

        //----
        if (mozno_prerusit == true)                                                      // pri prubeznem mereni z menu je mozne okamzite ukonceni stiskem tlacitka OK
          {
            uint8_t pauza = 60;                                                             // obvod musi mit dostatek casu na mereni pri libovolnem casu mereni (nastaveno 125 x 10ms = 1,25 sek)
            while (pauza > 0 and digitalRead(pin_tl_ok) == HIGH)                         // moznost predcasneho ukonceni pauzy tlacitkem OK
              {
                delay(22);             
                if (digitalRead(pin_tl_ok) == LOW or digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW) bylo_stisknuto_tlacitko = true; 
                pauza --;
              }
            if (pauza > 0) return 65535;                                                 // mereni bylo predcasne ukoncene tlacitkem OK (pauza nedobehla az do konce)
          }
        else                                                                             // pri beznem mereni se zaznamem je nutne celou pauzu dodrzet
          {
            for (uint8_t i = 0 ; i< 60 ; i++)
              {
                delay(22);             
                if (digitalRead(pin_tl_ok) == LOW or digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW) bylo_stisknuto_tlacitko = true; 
              }
          }

        delay(10);
        full = TSL_read_int(TSL2591_COMMAND_BIT | 0x14);                                 // CH_0
        ir  =  TSL_read_int(TSL2591_COMMAND_BIT | 0x16);                                 // CH_1
        TSL_disable();

         
        if (pouzit_modbus == true) test_485_mod();                                       // testovani, jestli neprislo neco po seriove lince RS485
        else                       test_485_kom();

        test_ser_kom();
      
        if (full == 0xFFFF or ir == 0xFFFF)                                              // je prebuzeno ...
          {
            posledni_citlivost --;                                                       //   ... snizuje se citlivost
          }
        else                                                                             // neni prebuzeno ...
          {
            break;                                                                       //   ... vypadne se z teto smycky 
          }

        if (posledni_citlivost < 0)                                                      // posledni mereni probehlo s nejnizsi citlivosti, takze dalsi snizovani uz neni mozne
          {
            posledni_citlivost = 0;                                                      // vratit zpatky nejnizsi citlivost 
            break;                                                                       //   a vypadnout
          } 
      }                                                                                  // konec prvni faze hledani citlivosti


    while (posledni_citlivost < 3)                                                       // dokud neni dosazena maximalni mozna citlivost, nebo dokud nedojde k prebuzeni
      {
        uint16_t pamet_full = full;                                                      // nejdriv se zapamatuje posledni neprebuzena hodnota
        uint16_t pamet_ir = ir;

        posledni_citlivost ++;                                                           //   ...  zvysi se citlivost


        // ---- mereni svetla ---
        cnf = pole_citlivosti[posledni_citlivost] + TSL_cas;
        TSL_config(cnf); 
        TSL_enable();
        //----
        if (mozno_prerusit == true)                                                      // pri prubeznem mereni z menu je mozne okamzite ukonceni stiskem tlacitka OK
          {
            uint8_t pauza = 60;                                                            // obvod musi mit dostatek casu na mereni pri libovolnem casu mereni (nastaveno 125 x 10ms = 1,25 sek)
            while (pauza > 0 and digitalRead(pin_tl_ok) == HIGH)                         // moznost predcasneho ukonceni pauzy tlacitkem OK
              {
                delay(22);             
                pauza --;
                if (digitalRead(pin_tl_ok) == LOW or digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW) bylo_stisknuto_tlacitko = true; 
              }
            if (pauza > 0) return 65535;                                                 // mereni bylo predcasne ukoncene tlacitkem OK (pauza nedobehla az do konce)
          }
        else                                                                             // pri beznem mereni se zaznamem je nutne celou pauzu dodrzet
          {
            for (uint8_t i = 0 ; i< 60 ; i++)
              {
                delay(22);             
                if (digitalRead(pin_tl_ok) == LOW or digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW) bylo_stisknuto_tlacitko = true; 
              }
          }

        full = TSL_read_int(TSL2591_COMMAND_BIT | 0x14);                                 // CH_0
        ir  =  TSL_read_int(TSL2591_COMMAND_BIT | 0x16);                                 // CH_1   
        TSL_disable();


        if (pouzit_modbus == true) test_485_mod();                                       // testovani, jestli neprislo neco po seriove lince RS485
        else                       test_485_kom();

        test_ser_kom();                                                                  // stejne tak se otestuje, jestli neprislo neco pres USB linku
        
                                                                                         // stisk libovolneho tlacitka v prubehu mereni vyvola hlaseni "BuSSy"
        if (bylo_stisknuto_tlacitko == true and mozno_prerusit == false)                 //    ale tyka se to jen bezneho mereni (pri prubeznem mereni se napis bussy nezobrazi)
          {
            tone_X(pin_bzuk,2500, 10 , 6);           
            zobraz_text(39);                                                             // na displeji zobrazi napis "BUSSY"
            SD_log(998 , 12); 
            delay(500);
            bylo_stisknuto_tlacitko = false;
          }




        if (full == 0xFFFF or ir == 0xFFFF)                                              // je prebuzeno ...
          {
            posledni_citlivost --;                                                       //  ... citlivost se vrati o uroven zpet
            cnf = pole_citlivosti[posledni_citlivost] + TSL_cas;                         //  ... o uroven zpet se vrati i registr 'cnf' (neni nutne ho zapisovat do cipu, pouziva se jen k vypoctum nize)
            full = pamet_full;                                                           //  ... obnovi se posledni neprebuzene hodnoty
            ir = pamet_ir;
            break;                                                                       //  ... a okamzite se vypadne ze smycky
          }
                                                                                         // kdyz neni prebuzeno, pokracuje se ve smycce, se zvysenim citlivosti

      }


    // promenne full a ir obsahuji zmerenou hodnotu
    // promenna posledni_citlivost obasuhuje posledni citlivost, od ktere se bude pode pokracovat v pristim mereni
    // bylo nalezeno takove zesileni, ktere uz nezpusobuje prebuzeni

    JAS_log_full = full;                                                                // zapamatovani neupravenych hodnot primo z registru TSL
    JAS_log_ir = ir;
    JAS_log_cnf = cnf;

    
    if (full == 0) full = 1;                                                             // tady se to musi trochu osidit, protoze pri hodne tmave obloze (kdyby se vratilo full = 0)
                                                                                         //  by to v nasledujicich vzorcich znamenalo deleni nulou
    if (full == ir) full ++;                                                             // podobny problem nastava,kdyz jsou oba kanaly stejne, proto se pri shode prida k full kanalu jednicka
                                                                                         // (problem je s funkci:  log10(0) )
  
    lux = TSL_calculateLux(cnf, full, ir);
//    ulux= lux * 1000000UL;
   
    mag_arcsec2 = log10(lux/108000UL)/-0.4;
  
    int_infra = ir;                                                                      // do globalni promenne se odkladaji mezivysledky kvuli pripadnemu zaznamu do EEPROM a na kartu
    int_full  = full;                                                                    // do globalni promenne se odkladaji mezivysledky kvuli pripadnemu zaznamu do EEPROM a na kartu
    byte_cnf  = cnf;                                                                     // do globalni promenne se odkladaji mezivysledky kvuli pripadnemu zaznamu do EEPROM a na kartu

  
    vystup_svetla = (mag_arcsec2 + 0.0005) * 1000;                                       // vysledek se zaokrouhli a prevede na uint16_t (maximalni hodnota by nemela byt vyssi, nez nejakych 24 x 1000 = 24000)
  
    // podle aktualne zjisteneho svetla nastavi jas displeje
    uint16_t korigovane_svetlo = korekce_svetla(vystup_svetla);
    if (korigovane_svetlo <= 10000)  jas_displeje = 7;                                   // nejvyssi jas
    if (korigovane_svetlo >  10000)  jas_displeje = 6;
    if (korigovane_svetlo >  15000)  jas_displeje = 5;
    if (korigovane_svetlo >  18000)  jas_displeje = 4;
    if (korigovane_svetlo >  19000)  jas_displeje = 3;
    if (korigovane_svetlo >  19500)  jas_displeje = 2;
    if (korigovane_svetlo >  20000)  jas_displeje = 1;                                   // nejnizsi jas    
    
    return vystup_svetla;        
  }
//----------------------------------------------



//----------------------------------------------
// do config registru TSL cidla ulozi hodnotu (zkombinovany cas a citlivost)
void TSL_config(uint8_t confdata)
  {
    TSL_enable();
    TSL_write(TSL2591_COMMAND_BIT | 0x01,confdata);
    TSL_disable();
  }
//----------------------------------------------



//----------------------------------------------
void TSL_enable(void)
  {
    TSL_write(TSL2591_COMMAND_BIT | 0x00, 147);                                          // power_on bit se nastavi, ostatni bity (sleep a interrupty se zakazou)    
  }
//----------------------------------------------



//----------------------------------------------
void TSL_disable(void)
  {
    TSL_write(TSL2591_COMMAND_BIT | 0x00, 0x00);                                         // power_on bit se zrusi, ostatni bity (sleep a interrupty se taky zakazou)
  }
//----------------------------------------------



//----------------------------------------------
//Zkopirovano z knihovny pro TSL2591 a zjednoduseno
// vypocet luxu z citlivosti, casu a namerenych hodnot v obou kanalech
float TSL_calculateLux(uint8_t conf, uint16_t ch0, uint16_t ch1)
  {
    float    atime;
    float    again = 1;
    float    cpl, lux;
    uint32_t citlivost;
  
    if ((ch0 == 0xFFFF) or (ch1 == 0xFFFF))                                              // Check for overflow conditions first
      {      
        return -1;                                                                       // Signal an overflow
      }

  
    atime = ((conf & 0b111) + 1) * 100.0;                                                // prevede cas mereni z command registru na [ms]   (5 = 600ms)
  
    citlivost = (conf & 0b110000);
    switch(citlivost)                                                                    // prevede jednu ze 4 urovni citlivosti na nasobek citlivosti
      {
        case 0b000000:
          again = 1.0;
          break;
        case 0b010000:
          again = 25.0;
          break;
        case 0b100000:
          again = 428.0;
          break;
        case 0b110000:
          again = 9876.0;
          break;
      }
   
    cpl = (atime * again) / 408.0;                                                       // cpl nebude nikdy 0 (atime je na pevno nastaveno na 600 a again je v nejhorsim pripade 1)

    
    lux = ( ((float)ch0 - (float)ch1 )) * (1.0F - ((float)ch1/(float)ch0) ) / cpl;
    return lux;  
  }
//----------------------------------------------



//----------------------------------------------
//  podprogramy pro komunikaci s cidlem TSL2591 pres I2C
//----------------------------------------------
//  zapis jedne hodnoty do zadaneho registru
void TSL_write(uint8_t registr, uint8_t tsldata)
  {
    Wire.beginTransmission(0x29);                                                        // zacatek komunikace s cidlem (I2C adresa 0x29 se neda menit)
    Wire.write(registr);                                                                 // cislo registru
    Wire.write(tsldata);                                                                 // ulozeni hodnoty
    delay(1);
    uint8_t err = Wire.endTransmission();                                                   // konec komunikace
    if (err > 0) chyba(1);
    else         bitClear(err_bit,0);                                                            // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
  }
//----------------------------------------------



//----------------------------------------------
// cteni jedne hodnoty ze zadaneho registru
uint8_t TSL_read(uint8_t registr)
  {
    Wire.beginTransmission(0x29);                                                        // zacatek komunikace s cidlem (I2C adresa 0x29 se neda menit)
    Wire.write(registr);                                                                 // LSB z adresy
    uint8_t err = Wire.endTransmission();
    if (err > 0) chyba(1);
    else         bitClear(err_bit,0);                                                            // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
    delay(1);
    Wire.requestFrom(0x29, 1);                                                           //  (I2C adresa 0x29 se neda menit)
    uint8_t x = Wire.read();   
    return x;
  }
//----------------------------------------------



//----------------------------------------------
// cteni INT hodnoty z registru cidla TSL
uint16_t TSL_read_int(uint8_t registr)
  {
    Wire.beginTransmission(0x29);                                                        // zacatek komunikace s cidlem (I2C adresa 0x29 se neda menit)
    Wire.write(registr);                                                                 // LSB z adresy
    delay(1);
    uint8_t err = Wire.endTransmission();
    if (err > 0) chyba(1);
    else         bitClear(err_bit,0);                                                            // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
    delay(1);
    Wire.requestFrom(0x29, 2);                                                           //  (I2C adresa 0x29 se neda menit)
    uint8_t lsb = Wire.read();                                                              //   pozor, nejdriv je LSB a az pak MSB
    uint8_t msb = Wire.read();

    return ((msb << 8) | lsb);
  }
//----------------------------------------------



//----------------------------------------------
// cteni hodnoty ze 2 po sobe jdoucich registru a jejich spojeni do jednoho 16-bitoveho cisla
//   pozor, nejdriv je LSB a az pak MSB
uint16_t TSL_read_int_old(uint8_t registr)
  {
    uint8_t lsb = TSL_read(registr);
    uint8_t msb = TSL_read(registr+1);
    return (256 * msb)  +   lsb;
  }
//----------------------------------------------



//----------------------------------------------
// prevede hodnotu svetla, ktera se uklada
float citelne_svetlo(uint16_t necitelne_svetlo)
  {
    return (necitelne_svetlo / 1000.0);
  }
//----------------------------------------------



//----------------------------------------------
// prepocte zmerenou hodnotu svetla podle kalibracni tabulky
int16_t korekce_svetla(int16_t zmerene_svetlo)
  {
    uint8_t index_pod = 15;                                                                 // hodnota 0
    uint8_t index_nad = 16;                                                                 // hodnota 24000
    
    for (uint8_t i = 0; i < 17 ; i++)
      {
        // nalezeni nejblizsiho vyssiho kalibracniho bodu v poli bodu
        if (zmerene_svetlo <= kal_svetlo[i][0] and kal_svetlo[i][0] < kal_svetlo[index_nad][0])
          {
            index_nad = i;
          }
    
        // nalezeni nejblizsiho nizsiho kalibracniho bodu v poli bodu
        if (zmerene_svetlo >= kal_svetlo[i][0] and kal_svetlo[i][0] > kal_svetlo[index_pod][0])
          {
            index_pod = i;
          }
      }
                                 
                                    //       pod_spatna       ,        nad_spatna       ,         pod_spravna      ,        nad_spravna
    int16_t vystup = map(zmerene_svetlo,  kal_svetlo[index_pod][0], kal_svetlo[index_nad][0],  kal_svetlo[index_pod][1], kal_svetlo[index_nad][1]);


    return vystup;
  }    
//----------------------------------------------



//----------------------------------------------
// prepocte zmerenou hodnotu teploty podle kalibracni tabulky
uint16_t korekce_teploty(uint16_t zmerena_teplota)
  {
    uint8_t index_pod = 7;                                                                  // hodnota 0     (-50'C)
    uint8_t index_nad = 8;                                                                  // hodnota 10000 (+50'C)

    for (uint8_t i = 0; i < 9 ; i++)
      {
        // nalezeni nejblizsiho vyssiho kalibracniho bodu v poli bodu
        if (zmerena_teplota <= kal_teplota[i][0] and kal_teplota[i][0] < kal_teplota[index_nad][0])
          {
            index_nad = i;
          }
    
        // nalezeni nejblizsiho nizsiho kalibracniho bodu v poli bodu
        if (zmerena_teplota >= kal_teplota[i][0] and kal_teplota[i][0] > kal_teplota[index_pod][0])
          {
            index_pod = i;
          }
      }

   
                                    //         pod_zmerena       ,         nad_zmerena       ,       pod_spravna         ,     nad_spravna
    int16_t vystup = map(zmerena_teplota,  kal_teplota[index_pod][0], kal_teplota[index_nad][0], kal_teplota[index_pod][1] , kal_teplota[index_nad][1]);

    return vystup;
  }    
//----------------------------------------------



//----------------------------------------------
// odecte od sebe dve hodnoty jasu (pouziva se ke zjistovani stability)
uint16_t rozdil_jasu(uint16_t jas_A, uint16_t jas_B)
  {
    if (jas_A >= jas_B) return jas_A - jas_B;
    else                return jas_B - jas_A;
  }
//----------------------------------------------



//-----------------------------------------------------------------------------------
// Zkontroluje, jestli zadane hodnoty v korekcni tabulce SVETLA neprekracuji meze a jestli ve sloupci "zmerene hodnoty" (primo z cidla) nejsou stejna cisla.
// V pripade, ze je neco spatne, zobrazi se na displeji chybove hlaseni "Err-S" a vystrazne pipne.
// Radka s hodnotou pres rozsah se automaticky smaze.
// Pokud je problem se stejnymi hodnotami, smazou se obe radky zarven.
//   (smazani znamena nastaveni obou sloupcu na hodnoty 0)
// !!! MAZANI SE PROVADI POUZE V NACTENEM POLI PROMENNYCH. DATA V EEPROM ZUSTAVAJI BEZE ZMENY !!!
// Radky s hodnotami [0,0] se ignoruji
void test_korekcni_tabulky_svetla(void)
  {
    bool stav_OK = true;
    for (uint8_t i = 0; i<15 ; i++)
      {
        if (kal_svetlo[i][0] >= 24000  or kal_svetlo[i][1] >= 24000)                     // nejaka hodnota v kalibracni tabulce prekracuje povolenou mez
          {
            stav_OK = false;   
            kal_svetlo[i][0] = 0 ;                                                       // radka v kalibracni tabulce se smaze
            kal_svetlo[i][1] = 0 ;                                                       // radka v kalibracni tabulce se smaze
          }

        
        if (kal_svetlo[i][0] > 0  and kal_svetlo[i][1] > 0)                              // kalibracni bod je zadany
          {
            for (uint8_t j = i+1; j < 15 ; j++)                                             // hledaji se shody v nasledujicich radkach
              {
                if (kal_svetlo[i][0] == kal_svetlo[j][0])                                // byly nalezeny shodne zmerene polozky
                  {
                    stav_OK = false;   
                    kal_svetlo[i][0] = 0 ;                                               // obe radky v kalibracni tabulce se smazou
                    kal_svetlo[i][1] = 0 ;                                               // obe radky v kalibracni tabulce se smazou
                    kal_svetlo[j][0] = 0 ;                                               // obe radky v kalibracni tabulce se smazou
                    kal_svetlo[j][1] = 0 ;                                               // obe radky v kalibracni tabulce se smazou
                  }
              }
          }
      }

    if (stav_OK == false)
      {
        chyba(5);                                                                        // "Err-S" chyba kalibrace svetla
      }

  }
//----------------------------------------------



//-----------------------------------------------------------------------------------
// Zkontroluje, jestli zadane hodnoty v korekcni tabulce TEPLOTY neprekracuji meze a jestli ve sloupci "zmerene hodnoty" (primo z cidla) nejsou stejna cisla.
// V pripade, ze je neco spatne, zobrazi se na displeji chybove hlaseni "Err-t" a vystrazne pipne.
// Radka s hodnotou pres rozsah se automaticky smaze.
// Pokud je problem se stejnymi hodnotami, smazou se obe radky zarven.
//   (smazani znamena nastaveni obou sloupcu na hodnoty 0)
// !!! MAZANI SE PROVADI POUZE V NACTENEM POLI PROMENNYCH. DATA V EEPROM ZUSTAVAJI BEZE ZMENY !!!
// Radky s hodnotami [0,0] se ignoruji
void test_korekcni_tabulky_teploty(void)
  {
    bool stav_OK = true;
    for (uint8_t i = 0; i<7 ; i++)
      {
        if (kal_teplota[i][0] >= 10000  or kal_teplota[i][1] >= 10000)                   // nejaka hodnota v kalibracni tabulce prekracuje povolenou mez
          {
            stav_OK = false;   
            kal_teplota[i][0] = 0 ;                                                      // radka v kalibracni tabulce se smaze
            kal_teplota[i][1] = 0 ;                                                      // radka v kalibracni tabulce se smaze
          }

        
        if (kal_teplota[i][0] > 0  and kal_teplota[i][1] > 0)                            // kalibracni bod je zadany
          {
            for (uint8_t j = i+1; j < 7 ; j++)                                              // hledaji se shody v nasledujicich radkach
              {
                if (kal_teplota[i][0] == kal_teplota[j][0])                              // byly nalezeny shodne zmerene polozky
                  {
                    stav_OK = false;   
                    kal_teplota[i][0] = 0 ;                                              // obe radky v kalibracni tabulce se smazou
                    kal_teplota[i][1] = 0 ;                                              // obe radky v kalibracni tabulce se smazou
                    kal_teplota[j][0] = 0 ;                                              // obe radky v kalibracni tabulce se smazou
                    kal_teplota[j][1] = 0 ;                                              // obe radky v kalibracni tabulce se smazou
                  }
              }
          }
      }

    if (stav_OK == false)
      {
        chyba(6);                                                                        // "Err-5"
      }
  }
//----------------------------------------------



//----------------------------------------------
// zmeri hodnotu svetla v luxech (nastavi si spravnou citlivost a v pripade true parametru se provede jeste kalibracni prepocet z konstant 'lux_kal_a', 'lux_kal_b' a 'lux_kal_c')
//     druhy parametr pri false zobrazuje vypoctene Luxy z obou slozek svetla, pri true zobrazuje pouze hodnotu INFRA registru, pred kterou probliknou na displeji apostrofy podle aktualniho zesileni
void zmer_lux(bool prepocitavat , bool infra_par)
  {
    uint8_t pocet_lx_klouzaku = 3;                                                          // pocet hodnot, ze kterych se bude pocitat klouzavy prumer
    pozice_tecky = 0;
    uint8_t cnflux = pole_citlivosti[lux_citlivost] + 2;                                       // atime:  2=300ms  (stredne dlouha doba mereni)
    TSL_config(cnflux);
    TSL_enable();

    msleep(490);                                                                         // dostatecna pauza na zpracovani
    delay(10);

    uint16_t full = TSL_read_int(0xA0 | 0x14);                                       // CH_0
    uint16_t ir  =  TSL_read_int(0xA0 | 0x16);                                       // CH_1   
    TSL_disable();
    if (full < 20)                                                                       // kdyz jsou zmerene hodnoty moc male
      {
        lux_citlivost ++;                                                                // zvysi se citlivost
        if (lux_citlivost == 4) lux_citlivost = 3;                                       // nikdy ale nesmi prekrocit index 3 (again = 9876x)
      }

    
    if (full == 0) full = 1;                                                             // kvuli deleni nulou nesmi byt nikdy full=0
    if (full == ir) full ++;                                                             // kvuli deleni nulou se nesmi nikdy full=ir
   
    float vypocteny_lux = TSL_calculateLux(cnflux, full, ir);
    if (vypocteny_lux == -1)                                                             // citlivost je moc velka, nektery z kanalu dosahl hodnoty 65535
      {
        lux_citlivost --;                                                                // snizuje se citlivost
        if (lux_citlivost == -1) lux_citlivost = 0;                                      // citlivost ale nikdy nesmi podlezt index 0 (again = 1x)
        zobraz_text(49);                                                                 // na displeji se rozsviti horni segmenty (overload)
      }
    else
      {
        if (prepocitavat == true)
          {
            vypocteny_lux = lux_kalibrace(vypocteny_lux);                                // prepocet podle nejake kalibracni krivky
            
            lux_klouzak[lux_index_klouzak] = vypocteny_lux;                              // klouzave prumerovani poslednich 5 hodnot
            lux_index_klouzak ++;
            if (lux_index_klouzak >= pocet_lx_klouzaku) lux_index_klouzak = 0;
    
            float sumaluxu = 0;
            for(uint8_t lux_pocet = 0; lux_pocet < pocet_lx_klouzaku ; lux_pocet ++)
              {
                sumaluxu = sumaluxu + lux_klouzak[lux_pocet];
              }
             vypocteny_lux = sumaluxu / pocet_lx_klouzaku;
          
          
          }

        if (infra_par == false)
          {
            zobraz_cislo((uint32_t)vypocteny_lux + 0.5, 0);                         // zobrazuje se cele zaokrouhlene cislo v rozsahu 0 az asi 88000        
          }
        else
          {
            D_pamet[0] =  0;                                                             // cely displej se prednastavi do zhasnuteho stavu (krome posledniho znaku, na kterem vzdycky blikne tecka)
            D_pamet[1] =  0;
            D_pamet[2] =  0;
            D_pamet[3] =  0;
            D_pamet[4] =  128;

            if (lux_citlivost >=  1)  D_pamet[3] =  128;
            if (lux_citlivost >=  2)  D_pamet[2] =  128;
            if (lux_citlivost >=  3)  D_pamet[1] =  128;
            aktualizuj_displej();                                                        // pred zobrazenim zjisteneho INFRA cisla probliknou na displeji apostrofy podle aktualniho zesileni
            delay(100);
            zobraz_cislo(ir, 0);                                                         // zobrazuje se cele cislo z infra registru
          }
      }
  }
//----------------------------------------------



//----------------------------------------------
// jen jedno mereni luxu bez prumerovani, ale s nastavenim spravneho rozsahu
float luxometr_1x(void)
  {
    uint8_t cnflx;
    uint16_t fulllx;
    uint16_t irlx;
    msleep(100);
    if (probiha_mereni_svetla == true)                                                  // kdyz probiha jeste nejake jine mereni svetla (napriklad prubezne mereni pres menu)
      {
        TSL_disable();                                                                  // a predchozi mereni se prerusi
        msleep(50);                                                                     // chvilku se pocka 
      }
   
    afd_nabito = false; 
    lux_citlivost = 0;                                                                   // zacne se od nejnizsi citlivosti
    do                                                                                   // smycka musi probehnout alespon 1x
      {
        cnflx = pole_citlivosti[lux_citlivost] + 2;                                        // atime:  2=300ms  (stredne dlouha doba mereni)
//        TSL_disable();                                                                  // a predchozi mereni se prerusi
//        delay(50);
        TSL_config(cnflx);
//        delay(50);
        TSL_enable();
        delay(50);
    
        msleep(490);                                                                     // dostatecna pauza na zpracovani
        delay(10);
        fulllx = TSL_read_int(0xA0 | 0x14);                                                // CH_0
        irlx  =  TSL_read_int(0xA0 | 0x16);                                                // CH_1   
        TSL_disable();
        delay(50);
        lux_citlivost ++;                                                                // po mereni se vzdycky zvysi citlivost
      }
    while (fulllx < 20 and lux_citlivost < 4);      
      
    
    if (fulllx == 0) fulllx = 1;                                                             // kvuli deleni nulou nesmi byt nikdy full=0
    if (fulllx == ir) fulllx ++;                                                             // kvuli deleni nulou se nesmi nikdy full=ir
   
    float vypocteny_lux = TSL_calculateLux(cnflx, fulllx, irlx);

    vypocteny_lux = lux_kalibrace(vypocteny_lux);                                        // prepocet podle nejake kalibracni krivky

/*                  
    Serial.print("lux_citlivost: ");
    Serial.print(lux_citlivost);
    Serial.print("     FULL: ");
    Serial.print(fulllx);
    Serial.print("     IR: ");
    Serial.print(irlx);

    Serial.print("     osvetleni: ");
    Serial.println(vypocteny_lux);
*/    
    msleep(100); 
    return vypocteny_lux;
  }
//----------------------------------------------



//----------------------------------------------
float lux_kalibrace(float vstup)
  {
    float vysledek;
    //  vysledkem je "vypocteny_lux" ve float.
    //   linearni prevod z x na y podle vzorce: y = ax + b
    //  v pripade nejakeho prohnuti kalibracni krivky je mozne pouzit i treti parametr 'lux_kal_c' (y = x*x*c + ax + b)
    if (lux_kal_c == 0)      // kdyz neni konstanta 'lux_kal_c' pouzita, provadi se jednodussi vypocet
      {
        vysledek = (vstup * lux_kal_a) + lux_kal_b;
      }
    else                     // v pripade pouziti konstanty 'lux_kal_c' hrozi nejake pretekani velkych mocnin a zaokrouhlovani, tak je vypocet radeji oddeleny
      {
        vysledek = ((double)vstup * (double)vstup * (double)lux_kal_c) + (vstup * lux_kal_a) + lux_kal_b;
      }
    return vysledek;
  }
//----------------------------------------------



//----------------------------------------------
// rychle otestovani svetla s nastavenym maximalnim again, ktere se provadi kazdych 30 sekund (pokud je funkce zapnuta)
//  Je to navenek "skryta" funkce, ktera neovlivnuje jakekoliv dalsi operace s SQM (pokud sviti displej, tak zustava rozsviceny)
// existuji 2 faze mereni: false = priprava a spusteni mereni, true = nacteni vysledku z mereni. Mezi pripravou a nactenim vysledku musi byt dlouha pauza (2 sekundy)
uint16_t test_AFD(bool faze)
  {
    if (faze == false)                                                        // priprava mereni a spusteni
      {
        TSL_disable();
        cnf = 0b00110101;                                                     // maximalni citlivost a cas (AGAIN = 9876x, ATIME = 600ms)
        TSL_config(cnf); 
        TSL_enable();
        return 0;
      }
    else                                                                      // precteni vysledku a zpracovani pres korekcni tabulku
      {
        full = TSL_read_int(TSL2591_COMMAND_BIT | 0x14);                      // CH_0
        ir  =  TSL_read_int(TSL2591_COMMAND_BIT | 0x16);                      // CH_1
        TSL_disable();
        if (full == 65535 or ir == 65535) return 0;                           // pri prebuzeni nektereho kanalu vraci 0 
        if (full == 0) full = 1;                                              // tady se to musi trochu osidit, protoze pri hodne tmave obloze (kdyby se vratilo full = 0)
                                                                              //  by to v nasledujicich vzorcich znamenalo deleni nulou
        if (full == ir) full ++;                                              // podobny problem nastava,kdyz jsou oba kanaly stejne, proto se pri shode prida k full kanalu jednicka
                                                                              // (problem je s funkci:  log10(0) )
      
        lux = TSL_calculateLux(cnf, full, ir);
        mag_arcsec2 = log10(lux/108000UL)/-0.4;
        return korekce_svetla((mag_arcsec2 + 0.0005) * 1000);                  // zadny kanal neni prebuzeny, vraci 1000x zaokrouhlenou hodnotu mag_arcsec2 (15124 = 15,1237 mag/arcsec)
      }
    
  }



//----------------------------------------------

 
 
