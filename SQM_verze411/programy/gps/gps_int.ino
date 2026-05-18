// Rozsirujici deska k SQM pro zpracovani GPS signalu
//=====================================================
//                 sqm.astromik.org
//
//     verze 17.5.2026 pro desku SQM-GPS-5(6)
//     ----------------------------------

//  POZOR!
//       V Arduino IDE nastavit hardware (soubor boards.txt) na 4MHz krystal
//       Po nahrani prelozeneho programu pak upravit FUSE:
//                                                       LowFuse   : 0xDC
//                                                       HighFuse  : 0xDE
//                                                       ExtendFuse: 0xF7

// Strucny popis programu
//
// Poloha se zjistuje z GPS modulu (NEO-6M,  ATGM336H ...) pres seriovou linku (Tx, Rx).
// Zpracovavaji se zpravy (vety) GxRMC a GxGGA.
// Druha strana komunikace probiha pres I2C s hlavni SQM deskou.
// Pomoci I2C komunikace je mozne stahnout data (zprumerovane souradnice, cas a dalsi informace o GPS modulu).
// Take je mozne pres I2C ovladat LED - docasne vsechny zhasnout (funkce 20) a pak obnovit do puvodniho stavu (funkce 21),
//   nebo provest HOT restart GPS modulu (funkce 22).
//
// Pri sepnuti servisniho pinu behem zapnuti napajeni se nastavi GPS modul na prijem zprav GxRMC a GxGGA (ostatni zpravy se vypnou).
//   Nastavi se i rychlost odesilani NMEA zprav na 1 sekundu.
// Pri beznem provozu se spojenim servisniho pinu na GND rozblikaji vsechny LED frekvenci 1Hz (strida 50%)
//   Po 5 sekundach spojeni dochazi k prepinani faze blikani LED1. Nektere GPS moduly odesilaji NMEA zpravy v obracenem poradi a to zpusobuje, 
//     ze LED1 blika s vyrazne delsim rozsvicenym stavem. Po prepnuti faze se pak usetri trochu energie a snizi celkove vyzarovani svetla z SQM.
//     Prepnuti faze je signalizovano rychlym zablikanim LED1.
// 3 signalizacni LEDky informuji o stavu GPS modulu:
//   LED1 - prepina se podle prave zpracovavane vety (GxRMC nesviti / GxGGA sviti) -  blikani je spravny stav, modul NEO-6M vysila obe zpravy
//   LED2 - stav prijmu datumu a casu (cas neni k dispozici - sviti / cas je k dispozici - zhasnuto)
//   LED3 - dostupnost souradnic 
//                     sviti = souradnice jeste nejsou k dispozici, neni zafixovano ani na jeden satelit
//                     blika = je zafixovano alespon na 1 satelit, ale jeste nejsou souradnice, nebo pocet prijatych souradnic nestaci na prumerovani (mene nez 10)
//                     nesviti = vsechno je v poradku, souradnice jsou k dispozici (pocet OK souradnic je 10 a vice) 
//   Spravny stav je tedy takovy, ze blika LED1 a ostatni LED jsou zhasnute.
//   V pripade, ze blika LED1 a LED2 je zhasnuta, dojde pri mereni jasu automaticky k serizeni casu v RTC na hlavni desce SQM-BAS. 

// Data se do SQM odesilaji pomoci 29 8-bitovych registru 
// priklad exportnich dat:
//  0 ---  8        prumer LAT (pro severni polokouli zvetseny o 90 stupnu)
//  1 ---  79
//  2 --- 192
//  3 --- 172

//  4 ---  11       prumer LON (pro vychodni polokouli zvetseny o 180 stupnu)
//  5 --- 149
//  6 --- 204
//  7 --- 147

//  8 --- 100       OK cnt (pocet bezchybnych zafixovanych dat v rade)

//  9 ---  21       YY     (datum a cas z GPS - v GMT zone)
// 10 ---   1       MM
// 11 ---   1       DD
// 12 ---  11       HH
// 13 ---  49       NN
// 14 ---  22       SS

// 15 ---   1       Nadmorska vyska zvysena o 500m
// 16 --- 241

// 17 ---   4       Pocet 'viditelnych' satelitu
// 18 ---  23       10x HDoP (Horizontal Dilution of Precision) ... mensi cislo = vetsi presnost
// 19 ---   6       status bajt:
//                    bit 0 = '0' pole klouzaku jeste neni zaplneno
//                    bit 0 = '1' pole klouzaku zaplneno, muzou se stahovat data
//                    bit 1 = '0' cas se do RTC nezapise (v GPS nejsou casove znacky, nebo je v ext. verzi prepinac casu v poloze "nenastavovat cas")
//                    bit 1 = '1' pri stazeni dat se do RTC ulozi cas z GPS (pokud bude k dispozci)
//                    bit 2 = '0' puvodni externi verze GPS
//                    bit 2 = '1' nova interni verze GPS
//
// doplneni registru ve verzi 2026-05-17
// 20 ---  8        aktualni (posledni) LAT bez prumerovani (pro severni polokouli zvetseny o 90 stupnu)
// 21 ---  79
// 22 --- 192
// 23 --- 172
// 24 ---  11       aktualni (posledni) LON bez prumerovani (pro vychodni polokouli zvetseny o 180 stupnu)
// 25 --- 149
// 26 --- 204
// 27 --- 147
// 28 ---   1       aktualni (posledni) nadmorska vyska zvysena o 500m
// 29 --- 241

//
//   Na zvlastni pozadavek odeslany pres I2C sbernici (prikazy 50 az 55 a 60 az 65) je mozne si vyzadat i kompletni
//           obsah NMEA vet GxRMC a GxGGA. Vety se zpatky do SQM odesilaji po 15 znacich.
//   Pri pozadavku s kodem 70 se vrati 15-znakovy text s verzi programu (yyyy-mm-dd..INT)
//======================================================================================================================


char verzeSW[] = "2026-05-17..INT";                        // 15 znaku popisu verze

#define F_CPU 4000000UL

#include <Wire.h>                                          // knihovna pro I2C komunikaci
#include <EEPROM.h>                                        // knihovna pro vnitrni EEPROM

#define pin_LED1            2
#define pin_LED2            3
#define pin_LED3            4

#define pin_setup           6                              // pri prizemneni behem resetu se do GPS modulu odesle prikaz pro povoleni GxGGA a GxRMC vety a toto nastaveni se ulozi do vnitrni EEPROM GPS modulu
                                                           // pri prizemneni za chodu se meni strida blikanio spodni LED1

#define pin_dta_rdy        A1                              // signalizace pro SQM, ze je modul GPS zapnuty

#define I2C_ADDR_GPS     0x17                              // I2C adresa pridavneho modulu pro GPS

// do retezce 'veta' se budou zapisovat vsechna prijata data z GPS
char veta[]     = "                                                                                            @\0";

//                 $GPRMC,110109.00,A,4926.67723,N,01421.99214,E,0.521,,311220,,,A*72
char veta_RMC[] = "                                                                                            @\0";

//                 $GPGGA,103515.00,4926.65079,N,01421.99995,E,1,05,4.92,499.9,M,44.3,M,,0000*5C
char veta_GGA[] = "                                                                                            @\0";

//   POZOR! Nektere prijimace, ktere kombinuji prijem vice druzicovych systemu (GPS, GLONASS, BEIDOU), misto "GPRMC" a "GPGGA" vraci retezce "GNRMC" a "GNGGA"
//                Treti znak ve zprave je tedy nutne pri zpracovani zprav ignorovat


char rozlozeny_pole[18][28];                               // vety se sem do toho pole rozkladaji na jednotlive polozky (puvodne byly ve vete oddelene carkami)

byte    ukazatel_klouzaku;
long    klouzak_LAT[10];                                   // pole pro prumerovani zjistenych souradnic (znamenkove)
long    klouzak_LON[10];
float   klouzak_ALT[10];
boolean pripraveno = false;                                // je zaznamenany alespon 1 blok klouzaku (10 vzorku), tak se da pocitat prumer 

byte volatile prikaz_I2C;                                  // z SQM (MASTER) se da do modulu (SLAVE) poslat prikaz:
                                                           //   0 = bez prikazu (predchozi prikaz byl vykonan)
                                                           //  10 = zadost o zpacovana data
                                                           //  20 = zhasnout docasne vsechny LED (pri mereni jasu)
                                                           //  21 = obnovit puvodni svit LED
                                                           // 50 az 55 = stahnout 15 bajtu dlouhy blok textu ze zpravy GxRMC (dohromady az 90 znaku)
                                                           // 60 az 65 = stahnout 15 bajtu dlouhy blok textu ze zpravy GxGGA (dohromady az 90 znaku)
                                                           // 70       = stahnout 15 bajtu dlouhy blok textu s verzi programu
                                                           

boolean  stav_LED_1;                                       // kvuli zhasinani a obnovovavni LED pre I2C prikaz se musi zapamatovat aktualni stav LED
boolean  stav_LED_2;
boolean  stav_LED_3;

boolean  pamet_LED_1;                                      // kvuli zhasinani a obnovovavni LED pre I2C prikaz se musi zapamatovat stav LED pred zhasnutim
boolean  pamet_LED_2;
boolean  pamet_LED_3;

boolean ignoruj_vetu;                                      // kdyz se prijme veta, o kterou nemam zajem

byte OK_byte;                                              // pocitadlo nepretrzitych prijatych dat


byte CRC_end;                                              // na ktere pozici ve vete se objevila '*' (pouzito pro vypocet CRC)
byte ukazatel;
byte EXPORT_pole[35];                                      // sem se ukladaji zpracovana data z GPS a odsud se pak preposilaji pres I2C

char rd;

byte aktivace_dta_rdy = 0;                                 // "pin_dta_rdy" se prepina do HIGH az kdyz se podari alespon 1x nacist z modulu GPS obe NMEA vety

unsigned long starttime;
char typ_vety;                                             // aktualni typ prijate vety ('R' nebo 'G')
char ocekavana_veta = 'R';                                 // zacina se s prijmem vety GxRMC


byte veta1[]     = { 'G', 'G', 'A'};                       // prvni povolena veta je GxGGA
byte veta2[]     = { 'R', 'M', 'C'};                       // druha povolena veta je GxRMC

byte prijate_CRC [] = { 'A', 'B'};                         // posledni dva znaky za hvezdickou z kazde vety se ukladaji sem a pak se porovnavaji s vypoctenym CRC

char EW_polokoule[10] ;                                    // reseni problemu s prumerovanim delky podel 180. poledniku (Tichy ocean) 
                                                           //  V tech mistech se muze stridave prijimat zemepisna delka +180 a -180 stupnu. To se neda zprumerovat


byte statusy_LED;                                          // promenna, do ktere se prubezne uklada pozadovany stav 3 LED (bit0=LED1; bit1=LED2; bit2 = LED3; 
                                                           //                                                              bit5=blikani LED3 nadrazene nad bit2 )
boolean zhasnuto;                                          // pri docasnem zhasnuti blokuje dalsi rozsveceni LED

boolean blik3;                                             // pouzito pro blikani 3. LED

boolean faze_LED1;                                         // u nekterych modulu prichazi nejdriv veta GGA a az potom RMC. To zpusobuje, ze LED1 blika s opacnou stridou.
                                                           //  zmenou teto promenne se stav LED prohodi 
unsigned long cas_prvniho_cyklu;                           // pro podprogram na zmenu faze LED1 (5-sekundovy odpocet drzeni servisniho pinu v LOW)
boolean prvni_cyklus;

unsigned long blik500;                                     // casovani 500ms pro servisni blikani vsech LED
boolean stav500;                                           // pro prepinani stavu LED pri servisnim blikani

// prikazy pro GPS modul (UBX protokol)
                        //  sync  sync class   id    delka (LittleEndian)
byte zacatek_prikazu[]  = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00};                                 // zacatek vypinaciho i zapinaciho prikazu je vzdycky stejny

                    //   msgclass  id   rate  CK_A  CK_B
byte zapni_GxGGA[]      = { 0xF0, 0x00, 0x01, 0xFB, 0x10};
byte zapni_GxRMC[]      = { 0xF0, 0x04, 0x01, 0xFF, 0x18};

byte vypni_GxGLL[]      = { 0xF0, 0x01, 0x00, 0xFB, 0x11};
byte vypni_GxGSA[]      = { 0xF0, 0x02, 0x00, 0xFC, 0x13};
byte vypni_GxGSV[]      = { 0xF0, 0x03, 0x00, 0xFD, 0x15};
byte vypni_GxVTG[]      = { 0xF0, 0x05, 0x00, 0xFF, 0x19};
byte vypni_GxGRS[]      = { 0xF0, 0x06, 0x00, 0x00, 0x1B};
byte vypni_GxGST[]      = { 0xF0, 0x07, 0x00, 0x01, 0x1D};
byte vypni_GxZDA[]      = { 0xF0, 0x08, 0x00, 0x02, 0x1F};
byte vypni_GxGBS[]      = { 0xF0, 0x09, 0x00, 0x03, 0x21};
byte vypni_GxDTM[]      = { 0xF0, 0x0A, 0x00, 0x04, 0x23};
byte vypni_GxGNS[]      = { 0xF0, 0x0D, 0x00, 0x07, 0x29};
byte vypni_GxVLW[]      = { 0xF0, 0x0F, 0x00, 0x09, 0x2D};
byte vypni_GPTXT[]      = { 0xF0, 0x41, 0x00, 0x3B, 0x91};
byte vypni_PUBX1[]      = { 0xF1, 0x00, 0x00, 0xFB, 0x12};
byte vypni_PUBX2[]      = { 0xF1, 0x03, 0x00, 0xFE, 0x18};
byte vypni_PUBX3[]      = { 0xF1, 0x04, 0x00, 0xFF, 0x1A};
byte vypni_XXXX1[]      = { 0xF0, 0x0E, 0x00, 0x08, 0x2B};
byte vypni_XXXX2[]      = { 0xF1, 0x01, 0x00, 0xFC, 0x14};
byte vypni_XXXX3[]      = { 0xF1, 0x05, 0x00, 0x00, 0x1C};
byte vypni_XXXX4[]      = { 0xF1, 0x06, 0x00, 0x01, 0x1E};

byte jeden_Hz[]         = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xE8, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x39, 0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30};

byte SAVE_EEPROM[]      = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x31, 0xBF};

byte hot_start[]        = {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x10, 0x68};


//======================================================================================================================



//======================================================================================================================
void setup(void)
  {
    UBRR0H = 0;                                                                                  // doladeni rychlosti seriove linky na 9600bps pro 4MHz krystal (kat. list ATmega328 str. 201)
    UBRR0L = 25;

    byte pomprom = EEPROM.read(0);                                                               // promenna pro zjisteni predchozi faze blikani LED1
    switch (pomprom)
      {
        case 0:
          faze_LED1 = false;
          break;

        case 1:
          faze_LED1 = true;
          break;
        
        default:
          EEPROM.write(0,0);
          faze_LED1 = false;
      }

    Wire.begin(I2C_ADDR_GPS);                                                                    // rozsirujici deska bude pres I2C komunikovat na adrese 0x17
    Wire.onRequest(requestEvent); 
    Wire.onReceive(receiveEvent);
    Serial.begin(9600);                                                                          // rychlost komunikace s GPS modulem
    
    pinMode(pin_setup ,INPUT_PULLUP);                                                            // jumper pro zakladni nastaveni GPS modulu

    pinMode(pin_LED1  ,OUTPUT);                                                                  // LEDKY
    pinMode(pin_LED2  ,OUTPUT);
    pinMode(pin_LED3  ,OUTPUT);

    pinMode(pin_dta_rdy, OUTPUT);
    digitalWrite(pin_dta_rdy, LOW);                                                              // po spusteni "pin_dta_rdy" zustane jeste chvili v LOW

    if (digitalRead(pin_setup) == LOW)                                                           // kdyz je pro zapnuti prizemneny pin_setup, dojde k prenastaveni NMEA vet z GPS modulu
      {
        for (byte i = 1 ; i < 6 ; i++)                                                           // 5x bliknuti frekvenci 1Hz kvuli kontrole spravne rychlosti krystalu (4MHz)
          {
            LED1(false);                                                                         // strida 50%
            LED2(false);
            LED3(false);
            delay(500);
            LED1(true);                                                                          
            LED2(true);
            LED3(true);
            delay(500);            
          }

        if (digitalRead(pin_setup) == LOW)                                                       // kdyz je i po 5 sekundach sepnuty kontakt, nastavi se modul NEO-6M
          {
            GPS_setup();                                                                         // povoleni GxGGA a GxRMC vet. Ostatni zrusit.

            while (true)                                                                         // Z teto nekonecne smycky se vypadne jen resetem
              {
                LED1(true);                                                                      // nekonecne rychle blikani (1Hz) vsech LED najednou
                LED2(true);                                                                      // strida 10%
                LED3(true);
                delay(100);
                LED1(false);
                LED2(false);
                LED3(false);
                delay(900);
              }


          }

      }

    bitSet(statusy_LED,0);                                                                       // po zapnuti napajeni se vsechny LED rozsviti (znameni vsech chyb: GPS neodesila vety, signal neobsahuje cas, signal je spatny)
    bitSet(statusy_LED,1); 
    bitSet(statusy_LED,2); 

    
    LED1(true);                                                                                  // rozsviti vsechny LED
    LED2(true);
    LED3(true);
    delay(500);


    EXPORT_pole[19] = 0b00000100;                                                                // Interni verze desky SQM-GPS, neni dostupny cas, neni zaplneno pole pro klouzaky
                                                                                                 //             bit2=1                 bit1=0               bit0=0

    // pro novejsi typy modulu NEO8-M se musi provest nastaveni konfigurace pri kazdem zapnuti (nemaji EEPROM)

    setup_neo_8();


  
  }
//======================================================================================================================



//======================================================================================================================
void loop(void)
  {
    ignoruj_vetu = false;
    if(prikaz_I2C == 0)
      {
        if (Serial.available())                                                                  // kdyz se neco objevi na seriove lince z GPS modulu
          {
            CRC_end = 1;
    
            while( rd != '$')   rd = Serial.read();                                              // cekani na zacatek vety
            ukazatel = 0;
            veta[ukazatel] = rd;                                                                 // '$' je vzdycky na zacatku
            starttime = millis();
            while (rd != '\r' and (millis() - starttime) < 700 and ukazatel < 90)                // kdyz uz je znamy zacatek vety, musi se zbytek vety stahnout do 0.7 sekundy nebo do 90 znaku
              {
                if (Serial.available())
                  {
                    rd = Serial.read();
                    if (rd == '*') CRC_end = ukazatel;                                           // znacka, do ktereho znaku se bude pocitat CRC
                    ukazatel++;
                    veta[ukazatel] = rd;                                                         // vsechny prijate znaky se zapisuji do znakoveho pole 'veta[]'
                  }
              }

           // tady v tom miste je poli 'veta[]' nactena cela veta z NEO modulu (nezavisle na typu - treba tam muze byt veta GxVTG)

           ignoruj_vetu = true;
           if (veta[3] == veta1[0] and veta[4] == veta1[1] and veta[5] == veta1[2])              // jedna se o povolenou vetu GxGGA
             {
               ignoruj_vetu = false;                                                             // veta se tedy bude zpracovavat
               typ_vety = 'G';                                                                   //    a jeste znacka, ze je to veta GGA
             }

           if (veta[3] == veta2[0] and veta[4] == veta2[1] and veta[5] == veta2[2])              // jedna se o povolenou vetu GxRMC
             {
               ignoruj_vetu = false;                                                             // veta se tedy bude zpracovavat
               typ_vety = 'R';                                                                   //    a jeste znacka, ze je to veta RMC
             }   
    
            if (ignoruj_vetu == false)                                                           // kdyz je nactena nejaka povolena veta, tak se zpracuje
              {
                byte blok = 0;                                                                   // rozlozeni vety na jednotlive bloky mezi carkami
                byte pozice = 0;
                byte rad = 0;
                char znak = ' ';
                while (znak != '\r' and pozice < 90 )
                  {
                    znak = veta[pozice];
                    if (veta[3] == 'G') veta_GGA[pozice] = znak;
                    if (veta[3] == 'R') veta_RMC[pozice] = znak;
                    
                    if (znak == ',' or znak == '*')
                      {
                        rozlozeny_pole[blok][rad] = '\0';
                        blok ++;
                        rad = 0;
                      }
                    else
                      {
                        rozlozeny_pole[blok][rad] = znak;
                        rad ++;                
                      }
                    pozice ++;
                  }            

                while (pozice < 90)                                                              // kompletni smazani aktualne zpracovavane vety pred jejim naplnenim novymi daty
                  {
                     if (typ_vety == 'G')   veta_GGA[pozice] = ' ';
                     if (typ_vety == 'R')   veta_RMC[pozice] = ' ';
                     pozice ++;
                  }
                  
                if (typ_vety == 'G' and ocekavana_veta == 'G') zpracuj_vetu_GxGGA();                                     // Nektere prijimace vraci retezec GNGGA, ale protoze se druhy znak nevyhodnocuje, tak to nevadi
                if (typ_vety == 'R' and ocekavana_veta == 'R') zpracuj_vetu_GxRMC();                                     // Nektere prijimace vraci retezec GNRMC, ale protoze se druhy znak nevyhodnocuje, tak to nevadi
    
                byte i;
                for (i = 0 ; i < 90 ; i ++)                                                      // po zpracovani prislusne vety se pole veta[] smaze
                  {
                    veta[i] = ' ';
                  }
                veta[i+1] = '\0';
              }
          }

      }

    if (aktivace_dta_rdy == 0b11)                                                                // obe vety byly alespon 1x prijaty
      {
        digitalWrite(pin_dta_rdy, HIGH);                                                         // "pin_dta_rdy" se prepne na HIGH
      }

    obsluha_LED();


  }
//======================================================================================================================



//----------------------------------------------
//  v GxGGA vete se zjistuje nadmorska vyska, pocet satelitu a kvalita zamereni (HDoP)
void zpracuj_vetu_GxGGA(void)
  {
    unsigned int  aktual_geoalt = 0;
    aktivace_dta_rdy = aktivace_dta_rdy | 0b01;                                                  // GxGGA veta byla alespon 1x prijata
    byte CRC=0;                                                                                  // vypocet CRC z prijatych dat
    for (byte i = 1 ; i <= CRC_end ; i++)   CRC = CRC xor veta[i];
    prijate_CRC[0] = veta[CRC_end+2];
    prijate_CRC[1] = veta[CRC_end+3];

    if (crc_check(CRC , prijate_CRC[0],prijate_CRC[1]))                                          // kontrolni soucet souhlasi, takze nejake vety se z GPS modulu ziskavaji
      {
        if (faze_LED1 == true)       bitSet(statusy_LED,0);                                      // LED1 rozsvit   - LED1 bude blikat (v GxGGA vete se rozsveci, v GxRMC vete se zhasina)
        else                         bitClear(statusy_LED,0);                                    // LED1 zhasni    - LED1 bude blikat (v GxGGA vete se zhasina, v GxRMC vete se rozsveci)


        int poc_sat = atof(rozlozeny_pole[7]);                                                   // protoze je v poli HDoP pri ztrate signalu posledni platna hodnota, musi se testovat i pocet fixnutych satelitu
        int HDoP = (atof(rozlozeny_pole[8]) + 0.05) * 10;                                        // Starsi verze modulu hlasily pred zafixovanim HDoP=99.99, novejsi hlasi HDoP=25.5. 
        if (poc_sat < 4) HDoP = 255;                                                             // pri ztrate signalu se HDoP nastavuje na maximum (255 = err), takze LED3 neblika ale rozsviti se
        if (HDoP  > 250)                                                                         // pro obe varianty modulu plati, ze HDoP musi byt mensi nez 25.5, aby LED zacala blikat
          {
            bitClear(statusy_LED,5);                                                             // signal neni vubec (pocet satelitu = 0, HDoP = 99.99 nebo 25.5) takze blikani je zrusene (LED trvale sviti)
          }
        else
          {
            if (OK_byte < 10)                                                                    // signal zafixovany alespon na 1 satelit, ale jeste neni dostatek zaznamu pro klouzak - blikani aktivni
              {
                bitSet(statusy_LED,5);                                                           // doplneni blikani o kratky "predblik"                                                 
              }
            else                                                                                 // uz je dostek zaznamu pro klouzak - blikani vypnute (LED trvale zhasnuta)
              {
                bitClear(statusy_LED,5);
              }
          }
    
    
        if (rozlozeny_pole[6][0] != '0')                                                         // kdyz je zafixovany signal, provede se zpracovani
          {
    
            byte pocet_satelitu;
            pocet_satelitu =                  (rozlozeny_pole[7][0] - 48) * 10;                  // desitky satelitu
            pocet_satelitu = pocet_satelitu + (rozlozeny_pole[7][1] - 48)     ;                  // jednotky satelitu
            EXPORT_pole[17] = pocet_satelitu;                                                    // maximalne by jich melo byt 12 (to se vejde do bajtu)
    
    
            // Horizontal Dilution of Precision 10x
            if(HDoP > 255) HDoP = 255;                                                           // maximalne se musi vejit do jednoho bajtu
            EXPORT_pole[18] = HDoP & 0xFF;
    
            if (HDoP < 100 and OK_byte > 10)                                                     // dobra kvalita, alespon 10 vzorku bez chyb
              {
                bitClear(statusy_LED,2);                                                         // LED3 zhasnout
              }
            else
              {
                bitSet(statusy_LED,2);                                                           // LED3 rozsvitit 
              }

 
            klouzak_ALT[ukazatel_klouzaku] = atof(rozlozeny_pole[9]);                            // klouzavy prumer souradnic z 10 vzorku
            aktual_geoalt = 500 + klouzak_ALT[ukazatel_klouzaku];                                // aktualni hodnota se bude ukladat o 500m zvetsena
            EXPORT_pole[28] = aktual_geoalt >>  8 & 0xFF;
            EXPORT_pole[29] = aktual_geoalt       & 0xFF;
    
            if (pripraveno == true)
              {
                float suma_ALT = 0;
                for (byte i = 0 ; i < 10 ; i ++)
                  {
                    suma_ALT = suma_ALT + klouzak_ALT[i];
                  }
                unsigned int prumer_ALT = 500 +( (suma_ALT + 5) / 10);                           // Vyska se prenasi posunuta o 500m (0 mnm = 500; 380mnm = 880 ; 550mnm = 1050, -200mnm = 300)
                
                EXPORT_pole[15] = prumer_ALT >>  8 & 0xFF;
                EXPORT_pole[16] = prumer_ALT       & 0xFF;
                
                OK_byte ++;

                if (OK_byte > 100) OK_byte = 100; 
                EXPORT_pole[8] = OK_byte ;                                                       // kolik vzorku proslo v rade bez problemu (maximalni hodnota je 100, dal se nepocita)
              }    
          }
        else                                                                                     // neni zafixovano
          {
            bitSet(statusy_LED,2);                                                               // LED3 rozsvitit 

            EXPORT_pole[0]  = 255;                                                               // LAT nastavit mimo rozsah (Err)
            EXPORT_pole[1]  = 255;
            EXPORT_pole[2]  = 255;
            EXPORT_pole[3]  = 255;

            EXPORT_pole[4]  = 255;                                                               // LON nastavit mimo rozsah (Err)
            EXPORT_pole[5]  = 255;
            EXPORT_pole[6]  = 255;
            EXPORT_pole[7]  = 255;

            EXPORT_pole[8] = 0;                                                                  // OK cnt 
   
            EXPORT_pole[15] = 255;                                                               // ALT nastavit mimo rozsah (Err)
            EXPORT_pole[16] = 255;

            EXPORT_pole[17] = 0;                                                                 // pocet satelitu
            EXPORT_pole[18] = 255;                                                               // HDoP (Err)
            
            EXPORT_pole[19] = EXPORT_pole[19] & 0b11111110;                                      // status maze informaci o zaplnenem poli klouzaku

            EXPORT_pole[20]  = 255;                                                              // aktual LAT nastavit mimo rozsah (Err)
            EXPORT_pole[21]  = 255;
            EXPORT_pole[22]  = 255;
            EXPORT_pole[23]  = 255;
            EXPORT_pole[24]  = 255;                                                              // aktual LON nastavit mimo rozsah (Err)
            EXPORT_pole[25]  = 255;
            EXPORT_pole[26]  = 255;
            EXPORT_pole[27]  = 255;
            EXPORT_pole[28]  = 255;                                                              // aktual ALT nastavit mimo rozsah (Err)
            EXPORT_pole[29]  = 255;
          

            
          }    
      }
    else                                                                                         // neco je spatne (nebyly prijate souradnice nebo nesouhlasi CRC, nebo je ve vete vystrazna znacka)
      {
        OK_byte = 0;                                                                             // nulovani poctu byzchybnych prijmu v rade
        EXPORT_pole[8] = 0;
        bitSet(statusy_LED,2);                                                                   // LED3 rozsvitit 
      }  

    ocekavana_veta = 'R';                                                                        // priste zpracuj GxRMC vetu
  
  }
//----------------------------------------------



//----------------------------------------------
// Z vety GxRMC se berou souradnice, datum a cas. Taky se tu sleduji vystrahy, pri kterych se nuluje pocet bezchybnych prijmu
void zpracuj_vetu_GxRMC(void)
  {
    aktivace_dta_rdy = aktivace_dta_rdy | 0b10;                                                  // GxRMC veta byla alespon 1x prijata

    byte CRC=0;                                                                                  // vypocet CRC z prijatych dat
    for (byte i = 1 ; i <= CRC_end ; i++)   CRC = CRC xor veta[i];

    // rozklad casu na hodiny, minuty a sekundy
    byte GMT_hod = 0;
    byte GMT_min = 0;
    byte GMT_sek = 0;
    GMT_hod =           (rozlozeny_pole[1][0] - 48) * 10;                                        // desitky hodin
    GMT_hod = GMT_hod + (rozlozeny_pole[1][1] - 48)     ;                                        // jednotky hodin
    GMT_min =           (rozlozeny_pole[1][2] - 48) * 10;                                        // desitky minut
    GMT_min = GMT_min + (rozlozeny_pole[1][3] - 48)     ;                                        // jednotky minut
    GMT_sek =           (rozlozeny_pole[1][4] - 48) * 10;                                        // desitky sekund
    GMT_sek = GMT_sek + (rozlozeny_pole[1][5] - 48)     ;                                        // jednotky sekund
                     
    // rozklad datumu na dny, mesice a roky
    byte GMT_den = 0;
    byte GMT_mes = 0;
    byte GMT_rok = 0;
    GMT_den =           (rozlozeny_pole[9][0] - 48) * 10;                                        // desitky dni
    GMT_den = GMT_den + (rozlozeny_pole[9][1] - 48)     ;                                        // jednotky dni
    GMT_mes =           (rozlozeny_pole[9][2] - 48) * 10;                                        // desitky mesicu
    GMT_mes = GMT_mes + (rozlozeny_pole[9][3] - 48)     ;                                        // jednotky mesicu
    GMT_rok =           (rozlozeny_pole[9][4] - 48) * 10;                                        // desitky roku
    GMT_rok = GMT_rok + (rozlozeny_pole[9][5] - 48)     ;                                        // jednotky roku
    
    if (GMT_rok > 20 and rozlozeny_pole[9][5] != '\0' )                                          // kdyz je rok vetsi, nez 2020, da se predpokladat, ze alespon datum a cas je prijaty v poradku
      {
        EXPORT_pole[9]  = GMT_rok;
        EXPORT_pole[10] = GMT_mes;
        EXPORT_pole[11] = GMT_den;
        EXPORT_pole[12] = GMT_hod;
        EXPORT_pole[13] = GMT_min;
        EXPORT_pole[14] = GMT_sek;
            
        bitClear(statusy_LED,1);                                                                 // LED2 zhasnout


        EXPORT_pole[19] = EXPORT_pole[19] | 0b00000110;                                          // vzdycky zapnout nastaveni RTC podle GPS a zaroven informace, ze se jedna o interni verzi GPS desky
      }

    else                                                                                         // prijaty rok neni v poradku
      {
        bitSet(statusy_LED,1);                                                                   // LED2 rozsvitit 
      }

    prijate_CRC[0] = veta[CRC_end+2];
    prijate_CRC[1] = veta[CRC_end+3];

    if (crc_check(CRC , prijate_CRC[0],prijate_CRC[1]))                                          // kontrolni soucet souhlasi
      {
        if (faze_LED1 == true)       bitClear(statusy_LED,0);                                    // LED1 zhasni    - LED1 bude blikat (v GxGGA vete se rozsveci, v GxRMC vete se zhasina)
        else                         bitSet(statusy_LED,0);                                      // LED1 rozsvit   - LED1 bude blikat (v GxGGA vete se zhasina, v GxRMC vete se rozsveci)
        
        if (rozlozeny_pole[2][0] == 'A')                                                         // znacky, ze je signal v poradku
          {
            // rozklad casu na hodiny, minuty a sekundy
            byte GMT_hod = 0;
            byte GMT_min = 0;
            byte GMT_sek = 0;
            GMT_hod =           (rozlozeny_pole[1][0] - 48) * 10;                                // desitky hodin
            GMT_hod = GMT_hod + (rozlozeny_pole[1][1] - 48)     ;                                // jednotky hodin
            GMT_min =           (rozlozeny_pole[1][2] - 48) * 10;                                // desitky minut
            GMT_min = GMT_min + (rozlozeny_pole[1][3] - 48)     ;                                // jednotky minut
            GMT_sek =           (rozlozeny_pole[1][4] - 48) * 10;                                // desitky sekund
            GMT_sek = GMT_sek + (rozlozeny_pole[1][5] - 48)     ;                                // jednotky sekund
                             
            // rozklad datumu na dny, mesice a roky
            byte GMT_den = 0;
            byte GMT_mes = 0;
            byte GMT_rok = 0;
            GMT_den =           (rozlozeny_pole[9][0] - 48) * 10;                                // desitky dni
            GMT_den = GMT_den + (rozlozeny_pole[9][1] - 48)     ;                                // jednotky dni
            GMT_mes =           (rozlozeny_pole[9][2] - 48) * 10;                                // desitky mesicu
            GMT_mes = GMT_mes + (rozlozeny_pole[9][3] - 48)     ;                                // jednotky mesicu
            GMT_rok =           (rozlozeny_pole[9][4] - 48) * 10;                                // desitky roku
            GMT_rok = GMT_rok + (rozlozeny_pole[9][5] - 48)     ;                                // jednotky roku
    
    
            // ----------------------
            // prepocet zemepisne sirky
            unsigned long geoLAT = 0;
            unsigned long aktual_geolat = 0;
            unsigned long aktual_geolon = 0;

            unsigned long pomprom_geomin;
            long geolat_sign;                                                                    // pro vypocty a prumerovani se pouzivaji znamenkove souradnice

            geoLAT =          (rozlozeny_pole[3][0] - 48) *    10000000UL;                       // desitky stupnu
            geoLAT = geoLAT + (rozlozeny_pole[3][1] - 48) *     1000000UL;                       // jednotky stupnu
    
            pomprom_geomin =                  (rozlozeny_pole[3][2] - 48)  *     1000000UL;      // desitky minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][3] - 48)  *      100000UL;      // jednotky minut
                                            // rozlozeny_pole[3][4] =  desetinna tecka
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][5] - 48)  *       10000UL;      // desetiny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][6] - 48)  *        1000UL;      // setiny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][7] - 48)  *         100UL;      // tisiciny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][8] - 48)  *          10UL;      // desettisiciny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][9] - 48)  *           1UL;      // stotisiciny minut
            pomprom_geomin = pomprom_geomin / 6UL;
    
            geoLAT = geoLAT + pomprom_geomin;                                                    // tady je zemepisna sirka v rozsahu +90 000 000 az 0 a 0 az +90 000 000 miliontin stupnu 
                                                                                                 //  rozliseni SEVER/JIH je znak v poli "rozlozeny_pole[4][0]"
            if (rozlozeny_pole[4][0] == 'N')            geolat_sign = geoLAT;                    // severni polokoule je kladna
            else if (rozlozeny_pole[4][0] == 'S')       geolat_sign = -geoLAT;                   // jizni polokoule je zaporna
            else                                        geolat_sign = 200000000L;                // neni zafixovano, vraci se kod pro poruchu

            if (geolat_sign >= 0) aktual_geolat = geolat_sign + 90000000;                        // pro severni polokouli se k zmerene hodnote pricita +90 stupnu
            else                  aktual_geolat = abs(geolat_sign);                              // pro jizni polokouli se zmerena hodnota jen prevede na kladne cislo
            EXPORT_pole[20] = (aktual_geolat >> 24) & 0xFF;                                      // do I2C pole se tedy ukladaji kladna cisla a SEVER je odlisen tak, ze ma vic nez 90 stupnu
            EXPORT_pole[21] = (aktual_geolat >> 16) & 0xFF;
            EXPORT_pole[22] = (aktual_geolat >>  8) & 0xFF;
            EXPORT_pole[23] = aktual_geolat         & 0xFF;

            klouzak_LAT[ukazatel_klouzaku] = geolat_sign;

            // ----------------------
            // prepocet zemepisne delky
            unsigned long geoLON = 0;
            long geolon_sign = 0;  

            geoLON =          (rozlozeny_pole[5][0] - 48) *    100000000UL;                      // stovky stupnu
            geoLON = geoLON + (rozlozeny_pole[5][1] - 48) *     10000000UL;                      // desitky stupnu
            geoLON = geoLON + (rozlozeny_pole[5][2] - 48) *      1000000UL;                      // jednotky stupnu
    
            pomprom_geomin =                  (rozlozeny_pole[5][3]  - 48)  *     1000000UL;     // desitky minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][4]  - 48)  *      100000UL;     // jednotky minut
                                            // rozlozeny_pole[5][5] =  desetinna tecka
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][6]  - 48)  *       10000UL;     // desetiny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][7]  - 48)  *        1000UL;     // setiny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][8]  - 48)  *         100UL;     // tisiciny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][9]  - 48)  *          10UL;     // desettisiciny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][10] - 48)  *           1UL;     // stotisiciny minut
            pomprom_geomin = pomprom_geomin / 6UL;
    
            geoLON = geoLON + pomprom_geomin;                                                    // tady je zemepisna delka v rozsahu +180 000 000 az 0 a 0 az +180 000 000 miliontin stupnu
                                                                                                 //  rozliseni VYCHOD/ZAPAD je znak v poli "rozlozeny_pole[6][0]"

            if (rozlozeny_pole[6][0] == 'E')            geolon_sign = geoLON;                    // vychodni polokoule je kladna
            else if (rozlozeny_pole[6][0] == 'W')       geolon_sign = -geoLON;                   // zapadni polokoule je zaporna
            else                                        geolon_sign = 200000000L;                // neni zafixovano, vraci se kod pro poruchu

            if (geolon_sign >= 0) aktual_geolon = geolon_sign + 180000000;                        // pro vychodni polokouli se k zmerene hodnote pricita +180 stupnu
            else                  aktual_geolon = abs(geolat_sign);                              // pro zapadni polokouli se zmerena hodnota jen prevede na kladne cislo
            EXPORT_pole[24] = (aktual_geolon >> 24) & 0xFF;                                      // do I2C pole se tedy ukladaji kladna cisla a VYCHOD je odlisen tak, ze ma vic nez 180 stupnu
            EXPORT_pole[25] = (aktual_geolon >> 16) & 0xFF;
            EXPORT_pole[26] = (aktual_geolon >>  8) & 0xFF;
            EXPORT_pole[27] = aktual_geolon         & 0xFF;



            EW_polokoule[ukazatel_klouzaku] = rozlozeny_pole[6][0];                              // kvuli problemum s prumerovanim v okoli +/- 180 stupnu se do extra pole poznamenava i aktualni znamenko polokoule

            klouzak_LON[ukazatel_klouzaku] = geolon_sign;
    
            ukazatel_klouzaku ++;
            if (ukazatel_klouzaku == 10)
              {
                ukazatel_klouzaku = 0;
                pripraveno = true;                    
                EXPORT_pole[19] = EXPORT_pole[19] | 0b00000001;                                  // pole pro klouzaky bylo zaplneno a muze se stahovat
              }
    
            if (pripraveno == true)
              {
                byte poskozene = 0;                                                              // poskozene souradnice (vypadek signalu)
                long suma_LAT = 0;
                long suma_LON = 0;
                long prumer_LAT = 0;
                long prumer_LON = 0;
                
                for (byte i = 0 ; i < 10 ; i ++)
                  {
                    if (klouzak_LAT[i] == 200000000L)                                            // pri poskozenych souradnicich se nic nepricita
                      {
                        poskozene ++;                                                            // vysledek se bude delit o jedna mensim cislem
                      }
                    else
                      {
                        suma_LAT = suma_LAT + klouzak_LAT[i];                                    // klouzak obsahuje normalni souradnice, tak se prictou pro pocitani prumeru                    
                      }                    
                  }             
                prumer_LAT = suma_LAT / (10-poskozene);
                if (prumer_LAT >= 0) prumer_LAT = prumer_LAT + 90000000;                         // pro severni polokouli se k prumeru pricita +90 stupnu
                else                 prumer_LAT = abs(prumer_LAT);                               // pro jizni polokouli se prumer jen prevede na kladne cislo




                // u zemepisne delky se nejdriv zkontroluje, ze jsou vsechny hodnoty ze stejne polokoule (problemy mohou nastat kolem +/- 180 stupnu)
                bool blokuj_prumer_lon = false;                                                  // prumerovani delky je na zacatku povoleno
                if (abs(geolon_sign) > 170000000L and abs(geolon_sign) <= 180000000L)            // kdyz je delka nekde v Tichem oceanu ...
                  {
                    bool vychod = false;
                    bool zapad = false;
                    for (byte i = 0 ; i < 10 ; i ++)
                      {
                        if (EW_polokoule[i] == 'E') vychod = true;                               // pole obsahuje alespon jednu vychodni souradnici
                        if (EW_polokoule[i] == 'W') zapad  = true;                               // pole obsahuje alespon jednu zapadni souradnici
                      }
                    if (vychod == true and zapad == true) blokuj_prumer_lon = true;              // kdyz pole obsauje obe souradnice, prumerovani se zakaze
                  }

                poskozene = 0;
                if (blokuj_prumer_lon == false)
                  {
                    for (byte i = 0 ; i < 10 ; i ++)
                      {
                        if (klouzak_LON[i] == 200000000L)                                        // pri poskozenych souradnicich se nic nepricita
                          {
                            poskozene ++;                                                        // vysledek se bude delit o jedna mensim cislem
                          }
                        else
                          {
                            suma_LON = suma_LON + klouzak_LON[i];                                // klouzak obsahuje normalni souradnice, tak se prictou pro pocitani prumeru                    
                          }
                      }             
                    
                    prumer_LON = suma_LON / (10-poskozene);
                  }
                else                                                                             // kdyz je bezne prumerovani zemepisne delky blokovano ...
                  {
                    for (byte i = 0; i < 10 ; i++)                                               //  ... provede se prumerovani prevodem souradnic o pul otacky niz (kolem nulteho poledniky)
                      {
                        if (klouzak_LON[i] == 200000000L)                                        // poskozene souradnice se ignoruji
                          {
                            poskozene ++;                                                        // vysledek se bude delit o jedna mensim cislem
                          }
                        else
                          {
                            if (klouzak_LON[i] < 0)                                              // klouzak ukazuje na zapadni polokouli (zaporna delka)
                              {
                                suma_LON = suma_LON + (klouzak_LON[i] + 180000000L);             // k delce se pricita 180 stupnu, takze se souradnice dostavaji k nule na kladnou cast osy 
                              }
                            else                                                                 // klouzak ukazuje na vychodni polokouli (kladna delka)
                              {
                                suma_LON = suma_LON + (klouzak_LON[i] - 180000000L);             // od delky se odecita 180 stupnu, takze se souradnice dostavaji k nule na zapornou cast osy 
                              }
                          }
                      }
                    prumer_LON = suma_LON / (10-poskozene);                                     // prumer upravenych souradnic (kolem nulteho poledniku)

                    if (prumer_LON < 0) prumer_LON = prumer_LON + 180000000L;                   // souradnice prumeru se musi vratit zpatky o pul otacky
                    else                prumer_LON = prumer_LON - 180000000L;                  
                  }

                if (prumer_LON >= 0) prumer_LON = prumer_LON + 180000000;                        // pro vychodni polokouli se k prumeru pricita +180 stupnu
                else                 prumer_LON = abs(prumer_LON);                               // pro zapadni polokouli se prumer jen prevede na kladne cislo



                
                EXPORT_pole[0] = (prumer_LAT >> 24) & 0xFF;                                      // do I2C pole se tedy ukladaji kladna cisla a SEVER je odlisen tak, ze ma vic nez 90 stupnu
                EXPORT_pole[1] = (prumer_LAT >> 16) & 0xFF;
                EXPORT_pole[2] = (prumer_LAT >>  8) & 0xFF;
                EXPORT_pole[3] = prumer_LAT         & 0xFF;
                
                EXPORT_pole[4] = (prumer_LON >> 24) & 0xFF;                                      // do I2C pole se tedy ukladaji kladna cisla a VYCHOD je odlisen tak, ze ma vic nez 180 stupnu
                EXPORT_pole[5] = (prumer_LON >> 16) & 0xFF;
                EXPORT_pole[6] = (prumer_LON >>  8) & 0xFF;
                EXPORT_pole[7] = prumer_LON         & 0xFF;
    
                OK_byte ++;
                if (OK_byte > 100) OK_byte = 100; 
                EXPORT_pole[8] = OK_byte ;                                                       // kolik vzorku proslo v rade bez problemu (maximalni hodnota je 100, dal se nepocita)
             

              }
    
            EXPORT_pole[9]  = GMT_rok;
            EXPORT_pole[10] = GMT_mes;
            EXPORT_pole[11] = GMT_den;
            EXPORT_pole[12] = GMT_hod;
            EXPORT_pole[13] = GMT_min;
            EXPORT_pole[14] = GMT_sek;
          }
        else                                                                                     // spatny signal, nebo nejake varovani ve vete
          {
            OK_byte = 0;                                                                         // nulovani poctu bezchybnych prijmu v rade
            
            for (byte i = 0; i < 10 ; i++)                                                       // vsechny klouzaky se nuluji
              {
                klouzak_LAT[ukazatel_klouzaku] = 200000000L;
                klouzak_LON[ukazatel_klouzaku] = 200000000L;
              } 
            pripraveno = false;

            bitSet(statusy_LED,2);                                                               // LED3 rozsvitit 

            EXPORT_pole[0]  = 255;                                                               // LAT nastavit mimo rozsah (Err)
            EXPORT_pole[1]  = 255;
            EXPORT_pole[2]  = 255;
            EXPORT_pole[3]  = 255;

            EXPORT_pole[4]  = 255;                                                               // LON nastavit mimo rozsah (Err)
            EXPORT_pole[5]  = 255;
            EXPORT_pole[6]  = 255;
            EXPORT_pole[7]  = 255;

            EXPORT_pole[8] = 0;        

          
            EXPORT_pole[15] = 255;                                                               // ALT nastavit mimo rozsah (Err)
            EXPORT_pole[16] = 255;

            EXPORT_pole[17] = 0;                                                                 // pocet satelitu
            EXPORT_pole[18] = 255;                                                               // HDoP  (Err)
            
            EXPORT_pole[19] = EXPORT_pole[19] & 0b11111110;                                      // status maze informaci o zaplnenem poli klouzaku

            EXPORT_pole[20]  = 255;                                                              // aktual LAT nastavit mimo rozsah (Err)
            EXPORT_pole[21]  = 255;
            EXPORT_pole[22]  = 255;
            EXPORT_pole[23]  = 255;
            EXPORT_pole[24]  = 255;                                                              // aktual LON nastavit mimo rozsah (Err)
            EXPORT_pole[25]  = 255;
            EXPORT_pole[26]  = 255;
            EXPORT_pole[27]  = 255;
            EXPORT_pole[28]  = 255;                                                              // aktual ALT nastavit mimo rozsah (Err)
            EXPORT_pole[29]  = 255;
          
          
          }
      }
    else                                                                                         // nesouhlasi CRC
      {
        OK_byte = 0;                                                                             // nulovani poctu bezchybnych prijmu v rade
        EXPORT_pole[8] = 0;
      }
    ocekavana_veta = 'G';                                                                        // priste zpracuj GxGGA vetu

  }
//----------------------------------------------        



//----------------------------------------------
// porovnani prijateho hexadecimalniho kodu na konci radky s vypoctem kodem
// 'CRC_bajt' je hodnota, ktera se pocita z cele prijate vety (od znaku s indexem 1 az k hvezdicce)
// 'HigHalf'  je prvni znak za hvezdickou v prijate vete
// 'LoHalf'   je druhy znak za hvezdickou v prijate vete
boolean crc_check(byte CRC_bajt, char HigHalf, char LoHalf)
  {
    byte CRC_pul = CRC_bajt >> 4;                                                                // horni pulka bajtu
    char pomprom;
    if (CRC_pul > 9)  pomprom = 65 + CRC_pul - 10;
    else              pomprom = 48 + CRC_pul;
    if (pomprom !=  HigHalf) return false;                                                       // porovnava se s prvnim prijatym HEXA znakem

    CRC_pul = CRC_bajt & 0b00001111;                                                             // spodni pulka
    if (CRC_pul > 9)  pomprom = 65 + CRC_pul - 10;
    else              pomprom = 48 + CRC_pul;
    if (pomprom !=  LoHalf) return false;                                                        // porovnava se s druhym prijatym HEXA znakem
    else                    return true;
  }
//----------------------------------------------



//----------------------------------------------
void LED1(boolean stav)                                                                          // rozsvecovani a zhasinani LED
  { 
    stav_LED_1 = stav;
    if (stav == true)   digitalWrite(pin_LED1, HIGH);
    else                digitalWrite(pin_LED1, LOW);
  }
//----------------------------------------------



//----------------------------------------------
void LED2(boolean stav)
  { 
    stav_LED_2 = stav;
    if (stav == true)   digitalWrite(pin_LED2, HIGH);
    else                digitalWrite(pin_LED2, LOW);
  }
//----------------------------------------------



//----------------------------------------------
void LED3(boolean stav)
  { 
    stav_LED_3 = stav;
    if (stav == true)   digitalWrite(pin_LED3, HIGH);
    else                digitalWrite(pin_LED3, LOW);
  }
//----------------------------------------------



//----------------------------------------------
void obsluha_LED(void)
  {
    while (digitalRead(pin_setup) == LOW)                                                        // servisni funkce, ktera by mela kdykoliv rozblikat vsechny LED frekvenci 1Hz
      {
        
        if ((millis() % 1000) < 500)                                                             // pravidelne blikani vsech LED s periodou 1s
          {
            digitalWrite(pin_LED1, LOW);
            digitalWrite(pin_LED2, LOW);
            digitalWrite(pin_LED3, LOW);            
          }
        else
          {
            digitalWrite(pin_LED1, HIGH);
            digitalWrite(pin_LED2, HIGH);
            digitalWrite(pin_LED3, HIGH);                        
          }

        
        if ((millis() - cas_prvniho_cyklu) > 5000)                                               // po 5 sekundach spojeni servisniho pinu do LOW se memi faze LED1
          {
            if (faze_LED1 == true)                                                               // zmena faze LED1 se zapisem noveho stavu do EEPROM
              {
                faze_LED1 = false;
                EEPROM.write(0,0);
              }
            else
              {
                faze_LED1 = true;
                EEPROM.write(0,1);                
              }

            for (byte i = 0; i < 10 ; i++)                                                       // rychle zablikani LEDl jako informace o zmene stavu
              {
                 digitalWrite(pin_LED1, LOW);
                 delay(50);
                 digitalWrite(pin_LED1, HIGH);
                 delay(50);
              }
            cas_prvniho_cyklu = millis();                                                        // dalsi prepnuti stavu je mozne az po 5 sekundach
          }
      }
   
    cas_prvniho_cyklu = millis();                                                                //  pri rozepnutem servisnim pinu se zaznamenava cas, ze ktereho se pak urcuje doba trvani sepnuti pinu 
    
    if (zhasnuto == false)                                                                       // obsluha LED je povolena
      {
        LED1(bitRead(statusy_LED,0));        
        LED2(bitRead(statusy_LED,1));        

        if (bitRead(statusy_LED,5) == 0)                                                         // bit5 obsahuje informaci o blikani 3. LED
          {
            LED3(bitRead(statusy_LED,2));                                                        // kdyz blikat nema, tak je trvale rozsvicena, nebo zhasnuta podle stavu bitu 2
          }
        else                                                                                     // kdyz blikat ma, prepina se s globalni promennou 'blik3'
          {
            if (blik3 == true)
              {
                digitalWrite(pin_LED3, LOW);
                blik3 = false;            
              }
            else
              {
                digitalWrite(pin_LED3, HIGH);
                blik3 = true;                            
              }
              
          }
      }
  
  }
//----------------------------------------------







//----------------------------------------------
// Prisla zadost o data pres I2C, podle kodu prikazu se odeslou pozadovane bloky dat (POZOR! buffer I2C zvlada maximalne 32 Bajtu)
void requestEvent(void)
  {
    if (prikaz_I2C >= 50 and prikaz_I2C < 60)                                                    // pro prikazy 50 az 55 se odesilaji 15 znaku dlouhe casti vety GxRMC
      {
        for (byte snd = (prikaz_I2C-50) * 15 ; snd < ((prikaz_I2C-50) * 15)+15 ; snd ++)
          {
            Wire.write(veta_RMC[snd]);
          }
          
        prikaz_I2C = 0;
      }

    if (prikaz_I2C >= 60 and prikaz_I2C < 70)                                                    // pro prikazy 60 az 65 se odesilaji 15 znaku dlouhe casti vety GxGGA
      {
        for (byte snd = (prikaz_I2C-60) * 15 ; snd < ((prikaz_I2C-60) * 15)+15 ; snd ++)
          {
            Wire.write(veta_GGA[snd]);
          }
        prikaz_I2C = 0;
      }


    if (prikaz_I2C == 70)                                                                        // pro prikaz 70 vrati v textovem formatu verzi programu ("yyyy-mm-dd..INT")
      {
        for (byte snd = 0 ; snd < 15 ; snd ++)
          {
            Wire.write(verzeSW[snd]);
          }
        prikaz_I2C = 0;
      }

    
    
    if (prikaz_I2C == 10)                                                                        // prikaz 10 odesle 30 bajtu velke pole zpracovanych dat
      {
         Wire.write(EXPORT_pole, 30);
         prikaz_I2C = 0;
      }

    if (prikaz_I2C == 20 )                                                                       // prikaz 20 se zpracovava v RecieveEventu.
      {
        prikaz_I2C = 0;                                                                          // tady se jen nastavi znacka, ze byl vykonan
      }

    if (prikaz_I2C == 21 )                                                                       // prikaz 21 se zpracovava v RecieveEventu.
      {
        prikaz_I2C = 0;                                                                          // tady se jen nastavi znacka, ze byl vykonan
      }


  }
//----------------------------------------------



//----------------------------------------------
// zjisteni kodu prikazu ktery prisel pres I2C
void receiveEvent(int prijato)
  {
    prikaz_I2C = prijato;                                                                        // uplne k nicemu, protoze na dalsich radkach se 'prikaz_I2C' stejne hned prepise. 
                                                                                                 //    zabrani ale kompilatoru ve varovani pro nepouzity parametr 'prijato'
    if (Wire.available())
      {
        prikaz_I2C = Wire.read();         
      }
    else
      {
        prikaz_I2C = 0;
      }


    if (prikaz_I2C == 20 )                                                                       // zhasnuti vsech LED pred merenim jasu
      {
        pamet_LED_1 = stav_LED_1;
        pamet_LED_2 = stav_LED_2;
        pamet_LED_3 = stav_LED_3;
        LED1(false);
        LED2(false);
        LED3(false);
        zhasnuto = true;
        prikaz_I2C = 0;
      }

    if (prikaz_I2C == 21 )                                                                       // obnoveni stavu vsech LED po mereni jasu
      {
        LED1(pamet_LED_1);
        LED2(pamet_LED_2);
        LED3(pamet_LED_3);
        zhasnuto = false;
        prikaz_I2C = 0;
      }

    if (prikaz_I2C == 22 )                                                                       // HOT restart GPS modulu
      {
        LED1(false);
        LED2(false);
        LED3(false);
        delay(1500);
        LED1(true);
        LED2(true);
        LED3(true);

        Serial.write(hot_start , 12);
        delay(1500);
      }

  }
//----------------------------------------------



//----------------------------------------------
//  POZOR! funguje jen pro verzi GPS modulu: NEO6-M
//        Pri pouziti verze NEO8-M se kofigurace do EEPROM neuklada a musi se pouzit prodprogram "setup_neo_8()"
// vsechny nepotrebne NMEA zpravy se vypnou, zustanou jen GxGGA a GxRMC
void GPS_setup(void)
  {
    Serial.flush();

    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGLL, 5);
    LED1(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGSA, 5);
    LED2(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGSV, 5);
    LED3(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxVTG, 5);
    LED1(true);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGRS, 5);
    LED2(true);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGST, 5);
    LED3(true);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxZDA, 5);
    LED1(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGBS, 5);
    LED2(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxDTM, 5);
    LED3(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGNS, 5);
    LED1(true);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxVLW, 5);
    LED2(true);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_PUBX1, 5);
    LED3(true);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_PUBX2, 5);
    LED1(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_PUBX3, 5);
    LED2(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_XXXX1, 5);
    LED3(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_XXXX2, 5);
    LED1(true);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_XXXX3, 5);
    LED2(true);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_XXXX4, 5);
    LED3(true);
    delay(1500);
  
    Serial.write(zacatek_prikazu, 6);
    Serial.write(zapni_GxGGA, 5);
    LED1(false);
    delay(1500);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(zapni_GxRMC, 5);
    LED2(false);
    delay(1500);

    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GPTXT, 5);
    LED3(false);
    delay(1500);

    Serial.write(jeden_Hz , 22);
    LED1(true);
    delay(1500);
    
    Serial.write(SAVE_EEPROM , 21);
    LED2(true);
    delay(1500);
    LED3(true);

    Serial.write(hot_start , 12);

    
  }
//----------------------------------------------



//----------------------------------------------
// nastaveni konfigurace pro novejsi moduly NEO
//  protoze nemaji EEPROM, musi se nastavovat kofigurace pri kazdem zapnuti, je ale mozne pouzit vetsi rychlost nastavovani
// podobne jako v podprogramu "GPS_setup(void)" se vypnou vsechny NMEA zpravy a zustanou jen GxGGA a GxRMC 
void setup_neo_8(void)
  {
    delay(50);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGLL, 5);
    LED1(false);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGSA, 5);
    LED2(false);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGSV, 5);
    LED3(false);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxVTG, 5);
    LED1(true);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGRS, 5);
    LED2(true);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGST, 5);
    LED3(true);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxZDA, 5);
    LED1(false);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGBS, 5);
    LED2(false);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxDTM, 5);
    LED3(false);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxGNS, 5);
    LED1(true);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GxVLW, 5);
    LED2(true);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_PUBX1, 5);
    LED3(true);
    delay(20);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_PUBX2, 5);
    LED1(false);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_PUBX3, 5);
    LED2(false);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_XXXX1, 5);
    LED3(false);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_XXXX2, 5);
    LED1(true);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_XXXX3, 5);
    LED2(true);
    delay(40);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_XXXX4, 5);
    LED3(true);
    delay(40);

    Serial.write(zacatek_prikazu, 6);
    Serial.write(vypni_GPTXT, 5);
    LED1(false);
    LED2(false);
    LED3(false);
    delay(400);

    
    Serial.write(zacatek_prikazu, 6);
    Serial.write(zapni_GxGGA, 5);
    LED1(true);
    delay(400);
    Serial.write(zacatek_prikazu, 6);
    Serial.write(zapni_GxRMC, 5);
    LED2(true);
    delay(400);
    Serial.write(jeden_Hz , 22);
    LED3(true);
    delay(400);

  }
