// menu ovladane 3 tlacitky se zobrazovanim na displeji
//======================================================


uint32_t trvani_stisku;          // doba drzeni tlacitka OK v milisekundach v hlavnim menu
uint32_t trvani_stisku2;         // doba drzeni tlacitka OK v milisekundach v podmenu

uint32_t start_UD;               // promenne pro automaticke listovani pri dlouhem stisku tlacitek nahoru a dolu
uint16_t cas_UD;
bool auto_UD;

uint8_t pocet_polozek_menu;

int8_t pozice_menu1 = 0;       // nalistovana pozice v hlavnim menu
                        // 0 = LIST           (listovani v poslednich namerenych hodnotach)
                        // 1 = Auto:          (nastaveni automatickeho zaznamu)
                        // 2 = ZonA:          (prepinani LETO/ZIMA - SEC/SELC)
                        // 3 = CALIb:         (zobrazeni a ulozeni jasu bez prepoctu)
                        // 4 = dAtuM:         (zobrazeni a moznost nastaveni datumu)
                        // 5 = CAS:           (zobrazeni a moznost nastaveni casu)
                        // 6 = tEPL:          (prubezne zobrazovani aktualni teploty)
                        // 7 = VLH:           (prubezne zobrazovani aktualni vlhkosti)
                        // 8 = JAS:           (prubezne zobrazovani aktualniho jasu)
                        // 9 = Ho-Mi:         (prubezne zobrazovani aktualniho casu)
                        //10 = PruM:          (nastaveni prumerovani)
                        //11 = LEd:           (regulovatelna cervena svitilna)
                        //12 = StoP:          (Stopky)
                        //13 = GPS:           (zobrazeni zemepisnych souradnic) - jen kdyz je pripojeny GPS modul a jsou k dispozici data
                        //14 = Stan           (prednastavene pozorovaci stanoviste misto GPS)
                        //15 = Spec           (specialni funkce)
                        //16 = int            (specialni funkce)
                        //21 = CoPy           (specialni funkce)
                        //22 = uHEL           (naklonomer)         
                        //23 = Astro          Astronomicke vypocty
                        //24 = COMPA          Kompas
                        //25 = rA-dE          prepocet Rektascenze a Deklinace na Azimut a Elevaci 
                        //26 = ALArM          petinasobny budik a jeden odpocet, ktery se spousti i pri vypnutem vypinaci
                        //27 = VoLtY          Mereni napeti zdroje
                        //28 = AFd            Alarm for darkness (upozorneni, ze jas oblohy poklesl pod nastavenou hodnotu)
                        //29 = DSO            viditelnost DSO objektu

//----------------------------------------------
// listovani v hlavnim menu a volba polozky
void menu_1(void)
  {
    blokuj_znacku30 = true;                                                  // blokovani blikani prvni segmentovky v rezimu R30s 
    pocet_polozek_menu = pocet_polozek_v_menu + 1;  // maximalni pocet pocet polozek (+1) pres ktere se listuje (nezalezi na tom, jestli jsou pouzite)



// moznost rucne omezit nektere polozky v menu
    pole_zobr_menu[ 0] = true;   // LIST
    pole_zobr_menu[ 1] = true;   // Auto
    pole_zobr_menu[ 2] = true;   // ZonA
    pole_zobr_menu[ 3] = true;   // CALIb
    pole_zobr_menu[ 4] = true;   // dAtuM
    pole_zobr_menu[ 5] = true;   // CAS
    pole_zobr_menu[ 6] = true;   // tEPL
    pole_zobr_menu[ 7] = true;   // VLH
    pole_zobr_menu[ 8] = true;   // JAS
    pole_zobr_menu[ 9] = true;   // Ho-Mi
    pole_zobr_menu[10] = true;   // PruM
    pole_zobr_menu[11] = true;   // LEd
    pole_zobr_menu[12] = true;   // StoP
    pole_zobr_menu[13] = false;  // GPS        polozka se povoluje automaticky podle toho, jestli je GPS zasunute a povolene
    pole_zobr_menu[14] = true;   // 5tAn       (Stan) Prednastavene pozorovaci stanoviste kdyz neni zapnuty GPS modul
    pole_zobr_menu[15] = false;  // #SPEC      polozka se povoluje automaticky podle typu zasunuteho specialniho HW
    pole_zobr_menu[16] = false;  // # int      polozka se povoluje automaticky podle typu zasunuteho specialniho HW
    pole_zobr_menu[17] = false;  // #PAr2      polozka se povoluje automaticky podle typu zasunuteho specialniho HW
    pole_zobr_menu[18] = false;  // #PAr3      polozka se povoluje automaticky podle typu zasunuteho specialniho HW
    pole_zobr_menu[19] = false;  // #FCE1      polozka se povoluje automaticky podle typu zasunuteho specialniho HW
    pole_zobr_menu[20] = false;  // #FCE2      polozka se povoluje automaticky podle typu zasunuteho specialniho HW
    pole_zobr_menu[21] = false;  // #CoPy      polozka se povoluje automaticky podle typu zasunuteho specialniho HW
    pole_zobr_menu[22] = false;  // uHEL       polozka se povoluje automaticky podle typu zasunuteho specialniho HW
    pole_zobr_menu[23] = true;   // AStro      Astronomicke vypocty      
    pole_zobr_menu[24] = false;  // COMPA      Kompas               polozka se povoluje automaticky pokud je pouzito cidlo LSM303DLHC
    pole_zobr_menu[25] = false;  // rA-dE      Ra-Dec na Az-El      polozka se povoluje automaticky pokud je pouzito cidlo LSM303DLHC
    pole_zobr_menu[26] = false;  // ALArM      vicenasobny budik    polozka se povoluje automaticky pokud je povolena periferie "RTC" (DS3231)
    pole_zobr_menu[27] = true;   // VoLtY      mereni napeti zdroje
    pole_zobr_menu[28] = true;   // AFd        Alarm for Darkness
    pole_zobr_menu[29] = true;   // DSO        viditelnost DSO objektu
    


    if (spec_rezim == 100)                                                   //!! pro spec rezim (r30s) se pouzivaji jen polozky #SPEC, #Par1 a #CoPy
      {
        pole_zobr_menu[15] = true;                                           // #SPEC
        pole_zobr_menu[16] = true;                                           // # int
        pole_zobr_menu[21] = true;                                           // #CoPy
      }

    pole_zobr_menu[26] = pouzit_ds3231;                                      // ALArM      vicenasobny budik nebo odpocet
    
    pole_zobr_menu[22] = modul_LSM303DLHC;                                   // uHEL
    pole_zobr_menu[24] = modul_LSM303DLHC;                                   // CoMPA
    pole_zobr_menu[25] = modul_LSM303DLHC;                                   // rA-dE

    pole_zobr_menu[ 6] = senzor_BME;                                         // teplota
    pole_zobr_menu[ 7] = senzor_BME;                                         // vlhkost
    

#ifdef ukladat_GPS
        if (digitalRead(pin_DATA_RDY) == osazeno_gps)                        // kdyz je GPS povolene a zasunute, tak se polozka bude zobrazovat
          {
            pole_zobr_menu[13] = true;                                       // polozku GPS zobrazit
            pole_zobr_menu[14] = false;                                      // polozku Stan nezobrazit
            if (pozice_menu1 == 14) pozice_menu1 = 13;                       // uprava aktualni pozice menu pro pripad, ze dojde k zablokovani aktualni "STAN" polozky
          }
        else
          {
            pole_zobr_menu[13] = false;                                      // polozku GPS nezobrazit
            pole_zobr_menu[14] = true;                                       // polozku Stan zobrazit            
            if (pozice_menu1 == 13) pozice_menu1 = 14;                       // uprava aktualni pozice menu pro pripad, ze dojde k zablokovani aktualni "GPS" polozky
          }
#endif


    auto_exit_casovani = millis();                                           // vstup do menu spousti casovac automatickeho vypadnuti z menu
    if (prvni_vstup_do_menu == true)                                         // pri prvnim vstupu do menu po zapnuti napajeni najde prvni zobrazitelnou polozku
      {
        for (uint8_t po = 0; po < pocet_polozek_menu ; po++)                    // pri vypnuti nejake polozky menu v uvodnich definicich programu se tady tato polozka skutecne zakaze
          {
            if (menu_enable[po] == false) pole_zobr_menu[po] = false;
          }

        
        // pro pripad, ze by nekdo zakazal vsechny polozky krome tech, ktere se stejne nezobrazuji bylo nutne porovnat dve pole
        bool menu_dostupne = false;
        for (uint8_t po = 0; po <= pocet_polozek_v_menu ; po ++)
          {
            if (menu_enable[po] == true and pole_zobr_menu[po] == true)      // byla nalezena alespon jedna polozka k zobrazeni
              {
                menu_dostupne = true;
                pozice_menu1 = po;
                break;                                                       // vypadnuti ze smycky for
              }
          }
        if (menu_dostupne == false)                                          // zadna polozka se nema zobrazovat
          {
            while (digitalRead(pin_tl_ok) == LOW) delay(100);                // cekani na uvolneni tlacitka OK s odrusenim zakmitu
            delay(100);
            return;                                                          // zadne menu se nezobrazi a po uvolneni tlacitka OK se vrati program zpatky do hlavni smycky
          }
      }
              
    prvni_vstup_do_menu = false;
    zobraz_text(pozice_menu1);



    while (digitalRead(pin_tl_ok) == LOW)                                    // do hlavniho menu se vstoupilo dlouhym stiskem tlacitka OK
      {                                                                      //  ted je nutne pockat na jeho uvolneni
        delay(20);
        if (digitalRead(pin_tl_ok) == HIGH) delay(50);                       // kdyz se behem drzeni tlacitka OK objevi kratky vypadek kontaktu, tak se odignoruje        
      }

    delay(50);
    
    bude_save = true;                                                        // prvni vstup do menu musi umoznit pruchod while smyckou alespon 1x
    while (bude_save == true)
      {
        pozice_menu1 = plusminus('#', pozice_menu1, 0, pocet_polozek_menu);  // tady na teto radce se spousti listovani v hlavnim menu a dal se program dostane az po stisku OK

        
        if (bude_save == true)                                               // ukonceni listovani v polozkach bylo provedeno dlouhym stiskem OK
          {
            podmenu(pozice_menu1);                                           // skoci se do podmenu (kratk stisk OK v podmenu by ale zpuspobil i ukonceni menu)
            bude_save = true;                                                // aby se pri navratu z podmenu kratkym stiskem OK z menu nevyskocilo, prepise se vzdycky znacka 'bude_save' na true
          }
        else                                                                 // kratky stisk OK v hlavnim menu zpusobi vypadnuti z podprogramu 'menu_1'
          {
            break;
          }
        
      }

    blokuj_znacku30 = false;                                                 // povoleni blikani prvni segmentovky v rezimu R30s 
    zobraz_text(66);                                                         // pri opusteni menu se zhasne displej 
  
  }
//------------------------------------------



//------------------------------------------
void podmenu(uint8_t polozka)
  {
    int16_t zadano;
    if (polozka == 14)                                                       // polozka 14 ("Stan") je trochu specialni - tam se misto pomlcek zhasne displej
      {                                                                      //    pomlcky znamenaji nenastavene pozorovaci stanoviste, proto se tady nezobrazuji
        zobraz_text(66);                                                     // "     "
      }
    else                                                                     // pro vsechny ostatni polozky se dlouhym stiskem zobrazi pomlcky
      {
        zobraz_text(68);                                                     // vsechny jednotky zobrazi "-----" a ceka se na uvolneni tlacitka (v podprogramu plusminus)    
      }

    while (digitalRead(pin_tl_ok) == LOW)                                    // do podmenu se vstoupilo dlouhym stiskem tlacitka OK
      {                                                                      //  ted je nutne pockat na jeho uvolneni
        delay(20);
        if (digitalRead(pin_tl_ok) == HIGH) delay(50);                       // kdyz se behem drzeni tlacitka ok objevi kratky vypadek kontaktu, tak se odignoruje        
      } 
    delay(50);


    if (polozka == 0)                                                        // LIST
      {
        zadano = plusminus('L', list,0,99);

        if (bude_save == true)                                               // kdyz se z podprogramu 'plusminus()' vyskocilo dlouhym stiskem [OK], zobrazi se jedna polozka v EEPROM
          {
            list = zadano & 255;
            vypis_EEPROM(65531,0);
          }                                                                  // pri navratu z podporgramu 'plusminus()' kratkym stiskem [OK], se EEPROM nevypise a vrati se na polozku 'LIST'
      }

    
    
    if (polozka == 1)                                                        // pocet minut mezi automatickym merenim
      {
        zadano = plusminus('A', automat,0,255);
        EEPROM_update(eeaddr_automat,zadano & 255);
        automat = zadano & 255;
        MODdata[46] = automat;                                               // interval automatickeho spousteni
        if (automat > 0)                                                     // pokud byl zadany nejaky casovy interval,
          {
            posledni_autospusteni = millis();                                // spusti se prvni mereni okamzite
            mereni(6);         
          }
      }



    if (polozka == 2)                                                        // SEC/SELC
      {
        zadano = plusminus('S', leto,0,1);
        EEPROM_update(eeaddr_leto_zima,zadano & 255);

        if ((zadano & 255) == 0) leto = false;
        else                     leto = true;
        STM_DS(false);                                                       // serizeni casu v DS3231 podle casu v STM        
      }


    if (polozka == 3)                                                        // svetlo bez korekce pro kalibraci
      {
        delay(1000);
        int_po_kalibraci = true;                                             // odskocenim do interruptu se po navratu obnovi hodnota na displeji
        mereni(7);                                                           //  prumeruje 10 vzorku, ale vystup vraci bez korekce. Na zaver vystledek zobrazi na displeji
        
        while (digitalRead(pin_tl_ok) == HIGH)                               // dokud je tlacitko OK uvolnene, je videt hodnota svetla bez korekce
          {
            for (uint8_t pametdis = 0; pametdis < 5 ; pametdis ++)              // pred interruptem se zapamatuje obsah displeje
              {
                D_pam_pred_int[pametdis] = D_pamet[pametdis];
              }

            
            interrupty();                                                    // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni            
            delay(20);

            bool zmena_na_displeji = false;                                  // kdyz behem interruptu doslo k nejake zmene na displeji, tak se na displej vrati puvodni kalibracni hodnota
            for (uint8_t pametdis = 0; pametdis < 5 ; pametdis ++)
              {
                if (D_pamet[pametdis] != D_pam_pred_int[pametdis])
                  {
                    zmena_na_displeji = true;
                    D_pamet[pametdis] = D_pam_pred_int[pametdis];                
                  }
              }
            if (zmena_na_displeji == true)   aktualizuj_displej();

            
            if (digitalRead(pin_tl_up) == LOW)                               // prepnuti na zobrazeni posledni nezkorigovane teploty
              { 
                pozice_tecky = 2;                                            // teplota na 2 destinna mista
                zobraz_cislo((int_teplota  - 5000 ), 1);                          
                D_pamet[4] = znak_stupen; 
              }

            if (digitalRead(pin_tl_dn) == LOW)                               // prepnuti na zobrazeni posledniho nezkorigovaneho jasu
              {
                pozice_tecky = 3;                                            // svetlo na 3 desetinna mista   
                zobraz_cislo(int_svetlo , 0);
              }          
          } 
        delay(20);
        int_po_kalibraci = false;
      }



    if (polozka == 4)                                                        // datum
      {
        bool pomprom_save = false;
        zobraz_RTC(false);                                                   // cas z RTC rozlozi do promennych "LOC_xxx" (vcetne korekci)
        
        // uprava promennych pomoci tlacitek       
        uint8_t zadany_den = plusminus('d', LOC_den,1,31);
        if (bude_save == true) pomprom_save = true;

        uint8_t zadany_mesic = plusminus('M', LOC_mes,1,12);
        if (bude_save == true) pomprom_save = true;

        if (LOC_rok < 2000) LOC_rok = 2000;                                  // kdyz jeste neni nastaveny cas v RTC, tak je rok nesmyslny, proto se upravi na '2000'
      
        uint8_t zadany_rok = plusminus('r', LOC_rok - 2000,0,99);
        if (bude_save == true) pomprom_save = true;

                                                                             // v promennych 'zadany_den' (/ mesic / rok) jsou upravene hodnoty

        if (pomprom_save == true)
          {

            mtt.year     = zadany_rok + 2000 - 1970;                         // knihovna je napsana tak, ze se cas pocita od 1.1.1970
            mtt.month    = zadany_mesic;
            mtt.day      = zadany_den;
            mtt.hour     = LOC_hod;
            mtt.minute   = LOC_min;
            mtt.second   = LOC_sek;

                                          
            if (leto == false) tt = makeTimeFCE(mtt) - (60*60*zimni_posun);    // v zime se pri ukladani casu uklada do RTC o hodinu mene (prevod SEC na UTC)
            else               tt = makeTimeFCE(mtt) - (60*60*letni_posun);    // kdyz se nastavuje cas v lete (SELC), do RTC se uklada o dalsi hodinu mene (prevod SELC na SEC)

            rtclock.setY2kEpoch(tt);
            RTC->BKP0R = (uint8_t)0xAA;                                        // znacka ze byl cas nastaveny. Po vypadku 3V zalozni baterie se znacka automaticky maze
            bitClear(err_bit,3);                                               // RTC je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'

            STM_DS(false);                                                     // serizeni casu v DS3231 podle STM32 (false = bez 30-sekundoveho testu)

            cas_minuleho_nastaveni = 0;                                        // zruseni automatickych korekci casu
            korekce = 0x7FFFFFFFUL;
            EEPROM_write_long(eeaddr_RTC_set,cas_minuleho_nastaveni);                       // do EEPROM se ulozi cas posledniho nastaveni (aktualni cas) a vypoctena korekce
            EEPROM_write_long(eeaddr_RTC_korekce,korekce);            
          }
         
      }


    if (polozka == 5)                                                        // cas
      {
        bool pomprom_save = false;
        zobraz_RTC(false);                                                   // cas z RTC rozlozi do promennych "LOC_xxx" (vcetne korekci)
        
        uint8_t zadane_hodiny = plusminus('H', LOC_hod,0,23);
        if (bude_save == true) pomprom_save = true;
        

        uint8_t zadane_minuty = plusminus('M', LOC_min,0,59);
        if (bude_save == true) pomprom_save = true;

        if (pomprom_save == true)
          {
            mtt.year     = LOC_rok - 1970;                                     // knihovna je napsana tak, ze se cas pocita od 1.1.1970
            mtt.month    = LOC_mes;
            mtt.day      = LOC_den;
            mtt.hour     = zadane_hodiny;
            mtt.minute   = zadane_minuty;
            mtt.second   = 0;

            if (leto == false) tt = makeTimeFCE(mtt) - (60*60*zimni_posun);    // v zime se pri ukladani casu uklada do RTC o hodinu mene (prevod SEC na UTC)
            else               tt = makeTimeFCE(mtt) - (60*60*letni_posun);    // kdyz se nastavuje cas v lete (SELC), do RTC se uklada o dalsi hodinu mene (prevod SELC na SEC)

            rtclock.setY2kEpoch(tt);
            RTC->BKP0R = (uint8_t)0xAA;                                        // znacka ze byl cas nastaveny. Po vypadku 3V zalozni baterie se znacka automaticky maze
            bitClear(err_bit,3);                                               // RTC je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'           

            STM_DS(false);                                                     // serizeni casu v DS3231 podle STM32 (false = bez 30-sekundoveho testu)
            
            cas_minuleho_nastaveni = 0;                                        // zruseni automatickych korekci casu
            korekce = 0x7FFFFFFFUL;
            EEPROM_write_long(eeaddr_RTC_set,cas_minuleho_nastaveni);          // do EEPROM se ulozi cas posledniho nastaveni (aktualni cas) a vypoctena korekce
            EEPROM_write_long(eeaddr_RTC_korekce,korekce);  
          }
      }


    if (polozka == 6)                                                        // teplota
      {
        delay(500);
        uint8_t pauza = 0;

        bool s_korekci = true;                                               // pri vstupu do polozky se vzdycky nejdriv zobrazuje teplota s korekci podle kalibracni tabulky

        while (digitalRead(pin_tl_ok) == HIGH)                               // Dokud neni stisknuto tlacitko OK
          {
            interrupty();                                                    // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni
            
            pauza ++;
            if (pauza == 200)                                                // teplota se testuje kazde 2 sekundy (200 x 0.01s)
              {
                int16_t pomprom = teplota(s_korekci);

                pauza = 0;
                D_pamet[4] = znak_stupen;                                    // znacka "stupen" pro mereni teploty na prvni zobrazovaci jednotce (rad desetitisicu)
                pozice_tecky = 2;
                zobraz_cislo((pomprom  - 5000 ), 1);

                
              }
            delay(10);  

            if (digitalRead(pin_tl_up) == LOW and digitalRead(pin_tl_dn) == LOW)             // funkce pro prepinani zobrazeni s korekci, nebo bez korekce
              {
                if (s_korekci == true)
                  {
                    s_korekci = false;
                    zobraz_text(32);                                         // "noCor"  (zobrazeni prubezne teploty bez korekce )
                  }
                else
                  {
                    s_korekci = true;
                    zobraz_text(33);                                         // " Corr"  (zobrazeni prubezne teploty s korekci )                    
                  }
                delay(50);
                while (digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW) ;     // ceka na uvolneni obou tlacitek
                delay(50);
              }
          }
      }


    if (polozka == 7)                                                        // vlhkost
      {
        uint8_t pauza = 0;
        while (digitalRead(pin_tl_ok) == HIGH)                               // Dokud neni stisknuto tlacitko OK
          {
            interrupty();                                                    // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni
            
            pauza ++;
            if (pauza == 200)                                                // vlhkost se testuje kazde 2 sekundy (200 x 0.01s)
              {
                uint16_t pomprom = vlhkost(); 
                pauza = 0;

                D_pamet[4] = znak_V;                                         // znacka "V" pro mereni vlhkosti na prvni zobrazovaci jednotce
                pozice_tecky = 2;
                zobraz_cislo(pomprom , 1);
              }
            delay(10);  
          }
      }
        

    if (polozka == 8)                                                        // svetlo
      {
        uint8_t  pauza = 75;
        int16_t rezim_mereni = 0;                                            // pri vstupu do teto polozky se zacina vzdycky rezimem SQM
        bool s_korekci = true;                                               // pri vstupu do polozky se vzdycky nejdriv zobrazuje jas s korekci podle kalibracni tabulky

        while (digitalRead(pin_tl_ok) == HIGH)                               // Dokud neni stisknuto OK
          {
            probiha_mereni_svetla = true;                                    // v pripade ze se v interruptech objevi pozadavek o mereni v rezimu r30s touto znackou se provede reset cidla svetla
            interrupty();                                                    // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni

            if (rezim_mereni == 0)                                           // je prepnuto na mereni mag/arcsec2 (SQM)
              {
                if (pauza == 75)                                             // svetlo se meri po 0,75 sekunde zobrazeni predchoziho mereni (75 x 0.01s)
                  {                
                    pauza = 0;
                    uint16_t pomprom = svetlo_1x(true,false);                // meri jen 1 vzorek - bez bargrafu, je mozne prerusit stiskem tlacitka OK
                    if (pomprom == 65535)  break;                            // mereni bylo predcasne ukonceno tlacitkem, proto se ukonci nadrazena smycka while

                    if (s_korekci == true)                                   // pokud je zapnuty korekcni prepocet ...
                      {
                        pomprom = korekce_svetla(pomprom);                   // ... proved pred zobrazenim korekci podle kalibracni tabulky                
                      }
                    else                                                     // kdyz NENI zapnuta kalibrani korekce (napis "noCor")
                      {
                        logjas(pomprom);                                     //   loguje se jas do specialniho souboru "logjas.txt" ktery pak muze slouzit ke kalibraci porovnanim s originalnim SQM
                      }
    
                    pozice_tecky = 3;
                    zobraz_cislo(pomprom , 0);
                  }
                pauza ++;
                delay(10);            

              }
            
            if (rezim_mereni == 1)                                           // je prepnuto na luxmetr
              {
                zmer_lux(s_korekci,false);                                   // zmeri a zobrazi aktualni hodnotu osvetleni v luxech. V pripade potreby si upravi citlivost. A podle parametru vrati cislo s korekci nebo bez korekce
              }


            if (rezim_mereni == 2)                                           // je prepnuto na mereni INFRA slozky
              {
                zmer_lux(false,true);                                        // zmeri a zobrazi aktualni hodnotu infra slozky, zadne korekce se neprepocitavaji.
              }

            if (rezim_mereni == 3)                                           // je prepnuto na mereni Bortle stupne
              {
                uint16_t pomprom = svetlo_1x(true,false);                    // meri jen 1 vzorek - bez bargrafu, je mozne prerusit stiskem tlacitka OK
                if (pomprom == 65535)  break;                                // mereni bylo predcasne ukonceno tlacitkem, proto se ukonci nadrazena smycka while
                pomprom = korekce_svetla(pomprom);                           // pred zobrazenim proved korekci podle kalibracni tabulky (pro rezim Bortle vzdycky s korekci)            
                pozice_tecky = 0;
                zobraz_cislo(bortle(pomprom) , 23);                          // zobrazeni roztridenim MSA na pasmo Bortle  "bor.=4"
                delay(300);
              }



            if (digitalRead(pin_tl_up) == LOW)                               // tlacitko "nahoru" prepina rezim mereni nahoru (SQM -> lux -> infra -> (Bortle))
              {
                rezim_mereni ++;
                if (rezim_mereni > 2 + bortle_povoleno) rezim_mereni = 2 + bortle_povoleno;    // polozka bortle je normalne vypnuta a musi se zapnout az po precteni navodu (vysvetleni nepresnosti)

                if (rezim_mereni == 1)                                           // rezim luxmetru
                  {
                    zobraz_text(50);                                             // "LuXM "  ("LuHM")
                    delay(50);
                    while (digitalRead(pin_tl_up) == LOW)                        // ceka na uvolneni tlacitka "Nahoru"
                      {
                        delay(50);
                        if (digitalRead(pin_tl_dn) == LOW)                       // kdyz se pri napisu "LUXM" jeste stiskne tlacitko dolu, 
                          {
                            s_korekci = ! s_korekci;                             // zinvertuje se stav korekce
                            zobraz_text(32 + s_korekci);                         // a stav se zobrazi na dispeji ("Corr" / "noCor")
                            while (digitalRead(pin_tl_dn) == LOW) delay(50);     // a ceka se na uvolneni tlacitka
                          }
                      }
                    delay(50);
                  }
                if (rezim_mereni == 2)                                           // rezim INFRA metru
                  {
                    zobraz_text(79);                                             // "inFrA"
                    delay(50);
                    while (digitalRead(pin_tl_up) == LOW)                        // ceka na uvolneni tlacitka "Nahoru"
                      {
                        delay(50);                                               // pri tomto rezimu se korekce nemeni stiskem tlacitka dolu
                      }
                    delay(50);
                  }

                if (rezim_mereni == 3)                                           // rezim zobrazeni Bortleho stupne (defaultne zablokovano)
                  {
                    zobraz_text(103);                                             // "bortL"
                    delay(50);
                    while (digitalRead(pin_tl_up) == LOW)                        // ceka na uvolneni tlacitka "Nahoru"
                      {
                        delay(50);                                               // pri tomto rezimu se korekce nemeni stiskem tlacitka dolu
                      }
                    delay(50);
                  }


              }


            if (digitalRead(pin_tl_dn) == LOW)                               // tlacitko "dolu" prepina rezim mereni dolu ((Bortle) -> infra -> lux ->SQM)
              {
                rezim_mereni --;
                if (rezim_mereni < 0) rezim_mereni = 0;

                if (rezim_mereni == 2)                                           // rezim INFRA metru
                  {
                    zobraz_text(79);                                             // "inFrA"
                    delay(50);
                    while (digitalRead(pin_tl_dn) == LOW)                        // ceka na uvolneni tlacitka "Dolu"
                      {
                        delay(50);                                               // pri tomto rezimu se Cor/noCor nemeni
                      }
                    delay(50);
                  }


                if (rezim_mereni == 1)                                           // rezim luxmetru
                  {
                    zobraz_text(50);                                             // "LuXM "  ("LuHM")
                    delay(50);
                    while (digitalRead(pin_tl_dn) == LOW)                        // ceka na uvolneni tlacitka "Nahoru"
                      {
                        delay(50);
                        if (digitalRead(pin_tl_up) == LOW)                       // kdyz se pri napisu "LUXM" jeste stiskne tlacitko nahoru, 
                          {
                            s_korekci = ! s_korekci;                             // zinvertuje se stav korekce
                            zobraz_text(32 + s_korekci);                         // a stav se zobrazi na dispeji ("Corr" / "noCor")
                            while (digitalRead(pin_tl_up) == LOW) delay(50);     // a ceka se na uvolneni tlacitka
                          }
                      }
                    delay(50);
                  }

                if (rezim_mereni == 0)                                           // rezim SQM
                  {
                    zobraz_text(51);                                             // "SQM "  ("5qM  ")
                    delay(50);
                    while (digitalRead(pin_tl_dn) == LOW)                        // ceka na uvolneni tlacitka "dolu"
                      {
                        delay(50);
                        if (digitalRead(pin_tl_up) == LOW)                       // kdyz se pri napisu "SQM" jeste stiskne tlacitko nahoru, 
                          {
                            s_korekci = ! s_korekci;                             // zinvertuje se stav korekce
                            zobraz_text(32 + s_korekci);                         // a stav se zobrazi na dispeji ("Corr" / "noCor")
                            while (digitalRead(pin_tl_up) == LOW) delay(50);     // a ceka se na uvolneni tlacitka
                          }
                      }
                    delay(50);
                  }
              }            
          }
        probiha_mereni_svetla = false;                                            // navrat do menu bez mereni svetla (rezim 30s muze pracovat bez omezeni)
      }


    if (polozka == 9)                                                        // Ho-Mi
      {
        bool oddelovac_hodin = false;                                        // pomocna promenna pro zjistovani stavu blikajici tecky mezi hodinama a minutama
        bool ho_mi_styl = true;                                              // zacina se vzdycky stylem Ho-Mi
        while (digitalRead(pin_tl_ok) == HIGH)                               // Dokud neni stisknuto tlacitko OK
          {
            if (digitalRead(pin_tl_up) == LOW )                              // funkce pro prepinani zobrazeni Ho-Mi
              {
                ho_mi_styl = true;
                zobraz_text(9);                                              // "Ho-Mi"
                delay(50);
              }

            if (digitalRead(pin_tl_dn) == LOW )                              // funkce pro prepinani zobrazeni Mi-Se
              {
                ho_mi_styl = false;
                zobraz_text(56);                                             // "Mi-Se"
                delay(50);
              }

            while (digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW) ;     // ceka na uvolneni obou tlacitek
            delay(50);


            interrupty();                                                    // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni
            
            if (millis() % 1000 > 500 and oddelovac_hodin == true)
              {
                oddelovac_hodin = false;
                D_pamet[2] = 0;                                              // pro prostredni displej se zrusi pomlcka mezi hodinami a minutami
                aktualizuj_displej();
              }
            
            if (millis() % 1000 < 500 and oddelovac_hodin == false)
              {
                oddelovac_hodin = true;
                if (ho_mi_styl == true)  ho_mi();                            // zobrazeni udaje "HH-MM" na displeji (hodiny nezobrazuji uvodni nuly)
                else                     mi_se();                            // zobrazeni udaje "MM-SS" na displeji (minuty zobrazuji uvodni nuly)
              }
          }
      }


    if (polozka == 10)                                                       // pocet vzorku svetla pro prumerovani svetla
      {
        zadano = plusminus('P', prumery,1,20);
        EEPROM_update(eeaddr_prumerovani,zadano & 255);
        prumery = zadano & 255;
        MODdata[41] = prumery;                                               // pocet prumerovani
      }


    if (polozka == 11)                                                       // funkce LED svitilna
      {
        svitilna();
      }


    if (polozka == 12)                                                       // funkce STOPKY
      {
        stopky();
      }


    if (polozka == 13)                                                       // GPS
      {
        uint8_t blokace_rotace = 1;                                             // blokovani rotace pro pripad, ze jeste nejsou aktualizovane souradnice
        lat_alt_lon = 0;
        delay(200);
        gps(0);                                                              // pres I2C si stahne posledni souradnice (zprumerovane), s casem se nic nedeje - RTC se nenastavuje
        lat_alt_lon = 0;                                                     // znacka, ze se jako prvni po vstupu do polozky GPS zobrazuje LAT
        bool ExitFlag = false;
        uint16_t pocitadlo_smycek = 0;                                   // pocitadlo smycek se pouziva k casovani nekterych udalosti (0.5 sek na rotaci, 2 sek na aktualizaci GPS)
        uint16_t index_rotace = 0;                                       // index prvniho zobrazeneho znaku na displeji z promenne 'pom_disp_scroll[]'
        uint16_t maxrotace = 9;                                          // kolik se bude pro delkovou a sirkovou polozku provadet rotacnich kroku
        bool povol_prepnuti = true;                                          // pro povoleni dalsiho stisku musi dojit k uvolneni tlacitek
        while (digitalRead(pin_tl_ok) == HIGH and ExitFlag == false)         // dokud je tlacitko OK uvolnene, jsou zobrazeny souradnice. Vypadnou se da i nastavenim ExitFlagu na true
          {
            if (digitalRead(pin_tl_up) == LOW and povol_prepnuti == true)    // prepnuti na nasledujici polozku LAT -> ALT -> LON -> trASA
              { 
                lat_alt_lon ++;
                if (lat_alt_lon > 3) lat_alt_lon = 3;                        // zarazka na nejvyssi polozce ("trASA")
                if (GPS_alt >= 9500) lat_alt_lon = 3;                        // kdyz jeste nejsou dostupna data, tak se stiskem [UP] prepne rovnou na "trASA"
                index_rotace = 0;                                            // dalsi zobrazeni cisla na displeji bude od nulove pozice "pom_disp_scroll[0]"
                povol_prepnuti = false;                                      // az do uvolneni obou tlacitek je zakazono prepnuti na dalsi polozku
                pocitadlo_smycek = 0;
                blokace_rotace = 1;                                          // nez se aktualizuji souradnice, nebude nic rotovat
                zobraz_text(66);                                             // hned pri stisku tlacitka zhasnout displej (pet zhasnutych sedmisegmentovek)
                delay(100);
              }
              
            if (digitalRead(pin_tl_dn) == LOW and povol_prepnuti == true)    // prepnuti na predchozi polozku trASA -> LON -> ALT -> LAT
              {
                lat_alt_lon --;
                if (lat_alt_lon < 0) lat_alt_lon = 0;                        // zarazka na nejnizsi polozce (GPS_lat)
                if (GPS_alt >= 9500) lat_alt_lon = 0;                        // kdyz jeste nejsou dostupna data, tak se stiskem [DN] prepne rovnou na spodni 'GPS_lat' (budou to pomlcky)
                index_rotace = 0;                                            // dalsi zobrazeni cisla na displeji bude od nulove pozice "pom_disp_scroll[0]"
                povol_prepnuti = false;                                      // az do uvolneni obou tlacitek je zakazono prepnuti na dalsi polozku
                pocitadlo_smycek = 0;
                blokace_rotace = 1;                                          // nez se aktualizuji souradnice, nebude nic rotovat
                zobraz_text(66);                                             // hned pri stisku tlacitka zhasnout displej (pet zhasnutych sedmisegmentovek)
                delay(100);
              }            

            if (digitalRead(pin_tl_up) == HIGH and digitalRead(pin_tl_dn) == HIGH )
              {
                delay(50);
                povol_prepnuti = true;
              }

            if (GPS_alt >= 9500 and lat_alt_lon == 0)                        // nejsou k dispozici souradnice, na LAT se zobrazuji pomlcky
              {
                zobraz_text(68);                                             // "-----"
              }

            if ((pocitadlo_smycek % 5) == 0)                                 // kazdou pul sekundu  (5 x 100ms)...
              {               
                if (blokace_rotace == 0)
                  {
                    if ((lat_alt_lon == 0 or lat_alt_lon == 2) and GPS_alt < 9500)    // zobrazeni zemepisne sirky a delky se rotuje (jen kdyz nesviti pomlcky)
                      {
                        D_pamet[4] =  pom_disp_scroll[index_rotace + 0];
                        D_pamet[3] =  pom_disp_scroll[index_rotace + 1];
                        D_pamet[2] =  pom_disp_scroll[index_rotace + 2];
                        D_pamet[1] =  pom_disp_scroll[index_rotace + 3];
                        D_pamet[0] =  pom_disp_scroll[index_rotace + 4];
                        aktualizuj_displej();
                      }
                    
                  }
    
                if (lat_alt_lon == 3)
                  {
                    if (trackuj == true)  zobraz_text(97);                   // polozka "tr. on"
                    else                  zobraz_text(96);                   // polozka "tr.oFF"
                  }
    
                index_rotace ++;                                             // ... se zvysi rotacni index o +1
                if (index_rotace > maxrotace) index_rotace = maxrotace;      // po dorotovani na posledni pozici uz se ale index nezvysuje
              }

            if ((pocitadlo_smycek % 20) == 0 or blokace_rotace == 1)         // kazde dve sekundy  (20 x 100ms) nebo ihned po stisku tlacitka ...
              {
                gps(0);                                                      // ... se pres I2C si stahnou z GPS posledni souradnice (zprumerovane)
                                                                             // a zvolene hodnoty se pripravi k zobrazeni 
                if (lat_alt_lon == 0)  priprava_rotujiciho_retezce(GPS_lat,0);  // sirka
                if (lat_alt_lon == 1)  priprava_rotujiciho_retezce(GPS_alt,1);  // pro nadmorskou vysku se vyska rovnou i zobrazi (zadna rotace neprobiha)
                if (lat_alt_lon == 2)  priprava_rotujiciho_retezce(GPS_lon,2);  // delka
                blokace_rotace = 0;                                          // po aktualizaci souradnic, uz se muze rotovat
              }
              


            if (digitalRead(pin_tl_dn) == LOW and digitalRead(pin_tl_up) == LOW and GPS_alt < 9500)  // dlouhym soucasnym stiskem tlacitek nahoru a dolu se aktualizuji domaci souradnice
              {                                                                                      //    (ale jen v pripade, ze je zafixovano - vraci se nejaka normalni nadmorska vyska)
                zobraz_text(66);                                             // zhasnuti displeje
                delay(100);
                uint32_t zacatek_stisku_DN = millis();
                delay(50);

                while (digitalRead(pin_tl_dn) == LOW and digitalRead(pin_tl_up) == LOW) 
                  {
                    pocitadlo_smycek = 38;                                   // aby se pri preruseni pokusu o ulozeni HC temer okamzite aktualizoval displej
                    if (millis() - zacatek_stisku_DN > 2000)                 // po dvou sekundach drzeni tlacitka se domaci souradnice prepisou
                      {
                        zobraz_text(38);                                     // napis "SAVE" na displeji
                        nastav_HC();                                         // z globalnich promennych 'GPS_lat' a 'GPS_lon' nastavi domaci souradnice
                        zobraz_text(68);                                     // napis "-----" na displeji
                        ExitFlag = true;                                     // po ulozeni novych HC souradnic se vyskoci ze smycky do hlavniho menu do polozky s napisem "GPS"
                      }
                    else
                      {
                        graf_natazeni(millis() - zacatek_stisku_DN , 133);   // dvousekundovy natahovaci graf  (133ms na jeden z 15 segmentu = 1995ms na zaplneni displeje )                    
                      }
                  }
              }

            pocitadlo_smycek++;
            interrupty();
            delay(100);                                                      // hlavni casovani pocitadla smycek
          } // konec smycky pro uvolnene tlacitko [OK]

        if (lat_alt_lon == 3)                                                // zvolena polozka pro zapinani trasovani
          {
            uint32_t start_stisku = millis();
            bool budesave = false;                                        
            while (digitalRead(pin_tl_ok) == LOW)
              {
                delay(50);                                                   // do polozky se vstoupilo stiskem OK, musi se pockat na uvolneni
                if (millis()-start_stisku > 1000)                            // dlouhy stisk
                  {
                    budesave = true;
                    if (trackuj == true)
                      {
                        trackuj = false;
                        zobraz_text(96);                                     // "tr.oFF"
                      }
                    else
                      {
                        trackuj = true;
                        zobraz_text(97);                                     // "tr. on"   
                      }                    
                    break;                                                   // vypadnuti ze smycky WHILE                     
                  }
              }           
            
            while (digitalRead(pin_tl_ok) == LOW) delay(50);
            delay(50);

            if (budesave == true)                                            // stisk tlacitka OK byl dlouhy, doslo ke zmene stavu, bude se ukladat
              {
                zobraz_text(38);                                             // "SAVE"
                delay(500);
                if (trackuj == true)  zaloz_GPX_soubor();                    // do GPX souboru se zapisou uvodni znacky, trasovani se spusti a do EEPROM se zapise znacka TRACKUJ
                else                  ukonci_GPX_soubor();                   // do GPX souboru se zapisou zaverecne znacky, trasovani se ukonci a do EEPROM se zapise znacka STOP              
              }
          }                                                                  // navrat do hlavniho menu se zobrazenou polozkou "GPS"
      }



    if (polozka == 14)                                                       // prednastavene pozorovaci stanoviste
      {
        menu_stanoviste();
      }



    if (polozka > 14 and polozka <= 21)                                      //!!(r30s)  spusteni podprogramu pro polozky v menu pro specialni funkce
      {
        spec_dis_menu(polozka);
      }


    if (modul_LSM303DLHC == true)
      {
        if (polozka == 22)                                                       // naklonomer
          {
            float akt_naklon;
            styl_vodovahy = 0;
            while (digitalRead(pin_tl_ok) == HIGH)                               // Dokud neni stisknuto tlacitko OK
              {
                interrupty();                                                    // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni
                if (digitalRead(pin_tl_up) == LOW)                               // tlacitkem nahoru se meni styl zobrazeni vodovahy
                  {
                    styl_vodovahy ++;
                   
                    zobraz_text(68);                                             // pri stisku tlacitka nahoru se zobrazi "-----"
                    if (styl_vodovahy == 2) styl_vodovahy = 0;
                    delay(100);
                    while (digitalRead(pin_tl_up) == LOW and digitalRead(pin_tl_dn) == HIGH) delay(100);
                    delay(100);
                  }
                
                if (digitalRead(pin_tl_dn) == LOW and digitalRead(pin_tl_up) == LOW)  // dlouhym soucasnym stiskem tlacitek nahoru a dolu se vstupuje do kalibrace vodovahy
                  {
                    zobraz_text(66);                                             // zhasnuti displeje
                    delay(100);
                    uint32_t zacatek_stisku_DN = millis();
                    delay(50);
    
                    while (digitalRead(pin_tl_dn) == LOW and digitalRead(pin_tl_up) == LOW) 
                      {
                        if (millis() - zacatek_stisku_DN > 2000)                 // po dvou sekundach drzeni tlacitka se spousti kalibrace
                          {
                            kalibrace_naklon();
                          }
                        else
                          {
                            graf_natazeni(millis() - zacatek_stisku_DN , 133);   // dvousekundovy natahovaci graf  (133ms na jeden z 15 segmentu = 1995ms na zaplneni displeje )                    
                          }
                      }
                        
                  }
    
    
                akt_naklon = (uhel() - 10000);                                   // zjisteni aktualniho uhlu zvetseneho o 1000 stupnu v desetinach stupne a prevod na desetinne cislo
    
                switch (styl_vodovahy)
                  {
                    case 0:                                                      // jednoduche zobrazeni elevace          
                      if (digitalRead(pin_tl_dn) == HIGH)                        // zobrazovani probiha jen kdyz je spodni tlacitko uvolnene. Jeho drzeni funguje jako "Data-Hold" a displej se neprekreluje
                        {
                          zobraz_cislo(akt_naklon, 9);                           // zobrazi na displeji uhel na 1 desetinne misto s pripadnym znamenkem na prvni segmentovce a znackou uhlu
                        }
                    break;
    
                    default:
                      if (akt_naklon < 450)  vodovaha_hor();                     // pod 45 stupnu se zobrazi horizontalni dvouosa vodovaha
                      else                   vodovaha_zen();                     // nad 45 stupnu se zobrazi vertikalni dvouosa vodovaha
                  }
    
              }
          }
      }


    if (polozka == 23)                                                       // Astro
      {
                              
        bool k_zobrazeni[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};          // pro ruzne typy objektu se zobrazuji ruzne vysledky (napriklad astronomicka noc nema vzdalenost)
        int16_t typ_zobrazeni = 0;
        uint32_t odpocet_autovypadnuti = millis();                      // po minute necinnosti se vypadne ze zobrazeni vypoctenych hodnot
        bool vypadnout_z_planet = false;                                     // specialni polozka menu, ktera se chova tak, ze po potvrzeni dlouhym stiskem [OK] 
        while(vypadnout_z_planet == false)                                   //             NEVYPADNE o uroven vys do hlavniho menu
          {
            index_planety = plusminus('p', index_planety , 1 , 12);
            if (bude_save == true)                                           // potvrzeni nalistovane planety dlouhym siskem [OK] provede vypocty zvolene planety
              {
                vypadnout_z_planet = false;
                zobraz_RTC(false);                                           // aktualizace promennych s lokalnim casem 'LOC_...'
                simulace_planet(index_planety);                              // 1=Merkur, 2=Venuse, 3=Mars, 4=Jupite,.... 8=Pluto, 9=Mesic, 10=Slunce, 11=noc, 12=volit_soumrak
               
                if (index_planety == 1 or index_planety == 2)                // jen u Merkuru nebo Venuse ...
                  {
                    k_zobrazeni[13] = true;                                  // zobrazuje se elongace
                    k_zobrazeni[14] = true;                                  // zobrazuje se faze planety
                  }
                else                                                         // vsechno ostatni krome Merkuru a Venuse ...
                  {
                    k_zobrazeni[13] = false;                                 // nezobrazuje se elongace
                    k_zobrazeni[14] = false;                                 // nezobrazuje se faze planety                  
                  }


                if (index_planety < 9)                                       // Planety 1=Merkur az 8=Pluto (Zeme se preskakuje)
                  {
                    k_zobrazeni[2] = true;                                   // zobrazuje se azimut vychodu / zapadu
                    k_zobrazeni[3] = true;                                   // zobrazuje se aktualni azimut
                    k_zobrazeni[4] = true;                                   // zobrazuje se aktualni elevace
                    k_zobrazeni[5] = true;                                   // zobrazuje se aktualni vzdalenost v AU
                    k_zobrazeni[6] = false;                                  // nezobrazuje se aktualni vzdalenost v tisicich km
                    k_zobrazeni[7] = false;                                  // nezobrazuje se vzdalenost Mesice v procentech
                    k_zobrazeni[8] = false;                                  // nezobrazuje se vzdalenost Slunce v procentech
                    k_zobrazeni[9] = false;                                  // nezobrazuje se osvetleni
                    k_zobrazeni[10] = false;                                 // nezobrazuje se trvani astronomicke noci
                    k_zobrazeni[11] = false;                                 // nezobrazuje se zacatek intervalu astronomicke noci (cas vecer)
                    k_zobrazeni[12] = false;                                 // nezobrazuje se konec intervalu astronomicke noci (cas rano)
                    k_zobrazeni[15] = true;                                  // zobrazuje se aktualni magnituda
                  }
                else
                  {
                    k_zobrazeni[15] =false;                                  // pro NEplanety se nezobrazuje aktualni magnituda
                  }

                if (index_planety == 9)                                      // Mesic
                  {
                    k_zobrazeni[2] = true;                                   // zobrazuje se azimut vychodu / zapadu
                    k_zobrazeni[3] = true;                                   // zobrazuje se aktualni azimut
                    k_zobrazeni[4] = true;                                   // zobrazuje se aktualni elevace
                    k_zobrazeni[5] = false;                                  // nezobrazuje se aktualni vzdalenost v AU
                    k_zobrazeni[6] = true;                                   // zobrazuje se aktualni vzdalenost v tisicich km
                    k_zobrazeni[7] = true;                                   // zobrazuje se vzdalenost Mesice v procentech
                    k_zobrazeni[8] = false;                                  // nezobrazuje se vzdalenost Slunce v procentech
                    k_zobrazeni[9] = true;                                   // zobrazuje se osvetleni
                    k_zobrazeni[10] = false;                                 // nezobrazuje se trvani astronomicke noci
                    k_zobrazeni[11] = false;                                 // nezobrazuje se zacatek intervalu astronomicke noci (cas vecer)
                    k_zobrazeni[12] = false;                                 // nezobrazuje se konec intervalu astronomicke noci (cas rano)
                    k_zobrazeni[16] = true;                                  // zobrazuje se colongitudo Mesice
                    k_zobrazeni[17] = true;                                  // zobrazuje se terminator Mesice
                    if (SIM_osvit < 30)                                      // kdyz je osvetlena jen mala cast Mesice (Mesic je pobliz Slunce)
                      {
                         k_zobrazeni[13] = true;                             // zobrazuje se elongace
                      }
                    else                                                     //  kdyz uz je Mesic od Slunce daleko, nema elongace vyznam (navic by mohlo dojit k prekroceni 99 stupnu a tim k problemum zobrazeni)
                      {
                         k_zobrazeni[13] = false;                            // nezobrazuje se elongace
                      }
                  }
                else                                                         // u cehokoliv jineho krome Mesice se colongitudo nezobrazuje
                  {
                    k_zobrazeni[16] = false;                                 // nezobrazuje se colongitudo Mesice
                    k_zobrazeni[17] = false;                                 // nezobrazuje se terminator Mesice
                  }
                if (index_planety == 10)                                     // Slunce
                  {
                    k_zobrazeni[2] = true;                                   // zobrazuje se azimut vychodu / zapadu
                    k_zobrazeni[3] = true;                                   // zobrazuje se aktualni azimut
                    k_zobrazeni[4] = true;                                   // zobrazuje se aktualni elevace
                    k_zobrazeni[5] = true;                                   // zobrazuje se aktualni vzdalenost v AU
                    k_zobrazeni[6] = false;                                  // nezobrazuje se aktualni vzdalenost v tisicich km
                    k_zobrazeni[7] = false;                                  // nezobrazuje se vzdalenost Mesice v procentech
                    k_zobrazeni[8] = true;                                   // zobrazuje se vzdalenost Slunce v procentech
                    k_zobrazeni[9] = false;                                  // nezobrazuje se osvetleni
                    k_zobrazeni[10] = false;                                 // nezobrazuje se trvani astronomicke noci
                    k_zobrazeni[11] = false;                                 // nezobrazuje se zacatek intervalu astronomicke noci (cas vecer)
                    k_zobrazeni[12] = false;                                 // nezobrazuje se konec intervalu astronomicke noci (cas rano)                    
                  }
                if (index_planety == 11 or index_planety == 12)              // Astronomicka noc nebo volitelny soumrak
                  {
                    k_zobrazeni[2] = false;                                  // nezobrazuje se azimut vychodu / zapadu
                    k_zobrazeni[3] = false;                                  // nezobrazuje se aktualni azimut
                    k_zobrazeni[4] = false;                                  // nezobrazuje se aktualni elevace
                    k_zobrazeni[5] = false;                                  // nezobrazuje se aktualni vzdalenost v AU
                    k_zobrazeni[6] = false;                                  // nezobrazuje se aktualni vzdalenost v tisicich km
                    k_zobrazeni[7] = false;                                  // nezobrazuje se vzdalenost Mesice v procentech
                    k_zobrazeni[8] = false;                                  // nezobrazuje se vzdalenost Slunce v procentech
                    k_zobrazeni[9] = false;                                  // nezobrazuje se osvetleni
                    k_zobrazeni[10] = true;                                  // zobrazuje se trvani astronomicke noci
                    k_zobrazeni[11] = true;                                  // zobrazuje se zacatek intervalu astronomicke noci (cas vecer)
                    k_zobrazeni[12] = true;                                  // zobrazuje se konec intervalu astronomicke noci (cas rano)          
                  }



                typ_zobrazeni = 0;
                zobraz_vyzapla(typ_zobrazeni);                               // hned po nekolikasekundovem vypoctu se zobrazi cas vychodu / zapadu ve formatu HH.MM
                odpocet_autovypadnuti = millis();
                while (digitalRead(pin_tl_ok) == HIGH and millis()-odpocet_autovypadnuti < 60000)    // listovani mezi jednotlivymi typy zobrazeni vypoctenych parametru zvolene planety
                  {
                    
                    if (digitalRead(pin_tl_up) == LOW)                       // tlacitko nahoru
                      {
                        odpocet_autovypadnuti = millis();                    // stisk tlacitka oddali automaticke vypadnuti o dalsi minutu
                        typ_zobrazeni ++;
                        if (typ_zobrazeni > 17) typ_zobrazeni = 0;
                        while (k_zobrazeni[typ_zobrazeni] == false)          // zobrazeni, ktera nejsou pro 'index_planety' dostupna, se preskoci
                          {
                            typ_zobrazeni ++;
                            if (typ_zobrazeni > 17) typ_zobrazeni = 0;
                          }
                        zobraz_vyzapla(typ_zobrazeni);
                        delay(100);
                        while (digitalRead(pin_tl_up) == LOW) delay(100);
                        delay(100);
                      }
                    if (digitalRead(pin_tl_dn) == LOW)                       // tlacitko dolu
                      {
                        odpocet_autovypadnuti = millis();                    // stisk tlacitka oddali automaticke vypadnuti o dalsi minutu
                        typ_zobrazeni --;
                        if (typ_zobrazeni < 0) typ_zobrazeni = 17;
                        while (k_zobrazeni[typ_zobrazeni] == false)          // zobrazeni, ktera nejsou pro 'index_planety' dostupna, se preskoci
                          {
                            typ_zobrazeni --;
                            if (typ_zobrazeni < 0) typ_zobrazeni = 17;
                          }
                        
                        zobraz_vyzapla(typ_zobrazeni);
                        delay(100);
                        while (digitalRead(pin_tl_dn) == LOW) delay(100);
                        delay(100);
                      }

                    interrupty();                                            // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni    
                    delay(300);
                  }
                while (digitalRead(pin_tl_ok) == LOW) delay(50);             // cekani na uvolneni tlacitka OK, ktere vrati zobrazeni na posledni zvolenou planetu
                delay(50);                                                   //   vcetne odruseni
              }
            else                                                             // kratky stisk umozni vypadnuti z podmenu "planety" do hlavniho menu
              {
                vypadnout_z_planet = true;
                odpocet_autovypadnuti = millis(); 
              }            
          }
        
      }






    if (modul_LSM303DLHC == true)
      {
        if (polozka == 24)                                                       // azimut
          {
            int16_t akt_azimut;
            int16_t posledni_azimut;
            
            uint8_t kompas_fce = 0;
            velikost_pole_azimutu = 5;
            bool zacni_prumerobvat = true;                                            // prvnich 5 vzorku se neprumeruje (pole klouzaku pro hodnoty azimutu jeste neni naplnene)
            uint8_t ukazatel_azimpole = 0;                                          // kvuli prumerovani azimutu se pouziva klouzave pole 5 poslednich hodnot
            while (digitalRead(pin_tl_ok) == HIGH)                               // Dokud neni stisknuto tlacitko OK
              {
                interrupty();                                                    // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni
                if (digitalRead(pin_tl_up) == LOW)                               // tlacitkem nahoru se prepne na kalibraci
                  {
                    zobraz_text(66);                                             // zhasnuti displeje
                    delay(100);
                    uint32_t zacatek_stisku_UP = millis();
                    while (digitalRead(pin_tl_up) == LOW)                        // pred kalibraci kompasu se musi tlacitko [nahoru] drzet alespon 2 sekundy
                      {
                        delay(50);
                        
                        if (millis() - zacatek_stisku_UP > 2000)                 // po dvou sekundach drzeni tlacitka se spousti kalibrace
                          {
                            kompas_fce = 1;
                            zobraz_text(59);                                     // "C-CAL"
                          }
                        else
                          {
                            graf_natazeni(millis() - zacatek_stisku_UP , 133);   // dvousekundovy natahovaci graf  (133ms na jeden z 15 segmentu = 1995ms na zaplneni displeje )                    
                          }
                        
                      }
                    delay(100);
                  }
    
                if (kompas_fce == 0)
                  {
                    uhel();                                                      // uhel nakloneni se pouziva pro urceni, ktere osy magnetometru zvyhodnit ve vypoctech
                    akt_azimut = zjisti_azimut();
                    posledni_azimut = akt_azimut;                                // pro pripad nestabilniho mereni (azimuty se v poli lisi o vic nez 10 stupnu) ae ignalizuje problem a posledni hodnota
                    pole_azimutu[ukazatel_azimpole] = akt_azimut;
                    ukazatel_azimpole ++;
                    if (ukazatel_azimpole > velikost_pole_azimutu - 1)
                      {
                        ukazatel_azimpole = 0;
                        zacni_prumerobvat = true;                                // probehlo alespon 1 kompletni zaplneni pole, zacina se prumerobvat
                      }
                    if (zacni_prumerobvat == true)
                      {
                        akt_azimut = prumeruj_azimuty();
                        if (akt_azimut >= 500)                                   // v pripade problemu s prumerovanim ...
                          { 
                            akt_azimut = posledni_azimut;                        // ... se nebere posledni hodnota v poli, ale posledni zmerena hodnota
                            problem_azimutu = true;                              // ale pri zobrazeni se bude signalizovat problem
                          }
                        else
                          {
                            problem_azimutu = false;                             //  \zadna problem s prumerovanim azimutu
                          }
                      }
    
                    if (digitalRead(pin_tl_dn) == HIGH)                          // displej se aktualizuje jen v pripade, ze je tlacitko [dolu] uvolnene
                      {                                                          //  (pokud je stisknute, meri se normalne dal, ale displej zustava beze zmeny - DataHold)
                        zobraz_cislo(constrain(akt_azimut, 0, 359), 15);         // constrain() vyresi problem obcasneho zaokrouhlovani na 360 stupnu
                      }              
                  }
    
                if (kompas_fce == 1)
                  {
                    kalibrace_magnet(false);
                    kompas_fce = 0;
                  }
                delay(100);
              }
          }
    
        if (polozka == 25)                                                       // Prepocet Ra-Dec na azimut a elevaci
          {
            bool pomprom_save = false;
            
            zadana_rektascenze = plusminus('R', zadana_rektascenze,0,239);
            if (bude_save == true) pomprom_save = true;
            
            while (digitalRead(pin_tl_ok) == LOW)    delay(50);              // cekani na uvolneni tlacitka OK s odrusenim zakmitu
            delay(100);
    
            zadana_deklinace = plusminus('D', zadana_deklinace,-90,90);
            if (bude_save == true) pomprom_save = true;
    
            if (pomprom_save == true)
              {
                if (leto == true) casova_zona = letni_posun;
                else              casova_zona = zimni_posun;
                z_LOC_na_Astro_UTC(casova_zona);                                 // z globalnich promennych pro casove udaje v mistni casove zone (LOC_xxx) vypocte UTC datum a cas
                RaToAzim ( 3 ,    zadana_rektascenze / 10.0   ,  zadana_deklinace , LOC_rok , LOC_mes , LOC_den , LOC_hod , LOC_min , casova_zona , GeoLon , GeoLat );       // obecne teleso
    
                float angle_dist;                                                // vysledek funkce pro mereni uhlove vzdalenosti
                
                
                uint16_t azimut_test;
                zobraz_text(62);                                                 // "HLEdA"
                while (digitalRead(pin_tl_ok) == LOW)    delay(50);              // cekani na uvolneni tlacitka OK s odrusenim zakmitu
                delay(100);
                 
                while (digitalRead(pin_tl_ok) == HIGH)                           // hledani trva, nez se stiskne tlacitko OK
                  {
                    uhel();
                    azimut_test = zjisti_azimut();
      
                    angle_dist = uhlova_vzdalenost(obecny_azimut, obecna_elevace, azimut_test, posledni_uhel);
    
                    if (digitalRead(pin_tl_dn) == HIGH and digitalRead(pin_tl_up) == HIGH)
                      {
                        D_pamet[4] = znak_u;                                     // prvni dve segmentovky zobrazuji jen male "u" a mezeru
                        D_pamet[3] = 0;
                        zobraz_cislo(angle_dist , 8);                
                      }
    
                    if (digitalRead(pin_tl_dn) == LOW) zobraz_cislo(obecny_azimut , 15);
                    if (digitalRead(pin_tl_up) == LOW) zobraz_cislo((int)(obecna_elevace * 10.0) + 0.5 , 9);
    
    
                    if (angle_dist > 1.5)                                        // akusticky vystup je aktivni v okruhu pod 1,5 stupne, pri vetsim rozdilu je piskak uplne vypnuty
                      {
                        noTone(pin_bzuk);
                      }
                    else                                           
                      {
                        tone_X(pin_bzuk, 600 , 20 , 5);                          // piska se jen v pripade, ze je piskani pri priblizeni povolene (prikaz "@P")
                      }
    
    
                  }
                while (digitalRead(pin_tl_ok) == LOW)                            // cekani na uvolneni OK
                  {
                    zobraz_text(68);                                             // "-----"
                    delay(100);
                  }
                delay(100);
              }                     
          }
      }

    if (polozka == 26)                                                           // Budik
      {
        uint16_t zadane_minuty;
        uint16_t zadane_hodiny;
        uint16_t ala_homin;
        bool pomprom_save = false;
        uint8_t zvoleny_budik = plusminus('b', 1,1,6);
        if (bude_save == true)                                                   // potvrzeni nalistovaneho budiku dlouhym siskem [OK]
          {
            uint8_t ALA_hod = RTC_alarm[zvoleny_budik] / 60;
            uint8_t ALA_min = RTC_alarm[zvoleny_budik] % 60;
    
            if (zvoleny_budik < 6)                                               // prvnich 5 budiku se zadava jako cas ve formatu HH:MM
              {
                if (ALA_hod == 24)                                               // kdyz ma zvoleny budik nastavenou hodnotu 1440 minut, znamena to, ze je vypnuty
                  {
                    zobraz_text(67);                                             // "H  --"
                  }
    
    
                zadane_hodiny = plusminus('H', ALA_hod,0,24);
                if (bude_save == true) pomprom_save = true;
        
                if (zadane_hodiny < 24)                                         // budik se vypina zadanim 24hodin. V tom pripade se minuty uz nezadavaji a nastavi se na 0
                  {
                    if (pomprom_save == true)                                   // zadavani hodin bylo potvrzeno dlouhym stiskem, pokracuje se v zadavani minut
                      {
                        zadane_minuty = plusminus('M', ALA_min,0,59);           // kdyz je nastaveny cas mene nez 24 hodin, zadavaji se jeste minuty
                        if (bude_save == true) pomprom_save = true;
                      }
                  }
                else                                                            // pri zadani 24H ... 
                  {
                    zadane_minuty = 0;                                          //    ... se minuty nastavi na 0
                  }
                ala_homin = (zadane_hodiny * 60) + zadane_minuty;
              
              }
            else                                                                // posledni budik (s indexem 6) je funkce "odpocet" a cas se zadava v (minutach 0 az 999)
              {
                uint16_t odpocet_zbyva;
                if (RTC_alarm[6] < 1440)
                  {
                    odpocet_zbyva = zbyvajici_minuty(RTC_alarm[6]);
                  }
                else
                  {
                    odpocet_zbyva = 0;
                  }
                zadane_minuty = plusminus('M', odpocet_zbyva,0,999);            // pri odpoctu se zadavaji jen minuty
                if (bude_save == true) pomprom_save = true;
    
                if (zadane_minuty == 0)                                         // pri zadani 0 minut na odpoctu se odpoctovy alarm maze
                  {
                    ala_homin = 1440;
                  }
                else                                                            // pri zadani nejakeho nenuloveho poctu minut se vypocte cas alarmu v minutach od pulnoci 
                  {
                    ala_homin = pridej_minuty(zadane_minuty);
                  }
                  
              }
              
            if (pomprom_save == true)                                       // pro jakykoliv zvoleny budik nebo odpocet, kdyz se ma hodnota zapsat ...
              {
                RTC_alarm[zvoleny_budik] = ala_homin;                       // podle nastavenych hodin a minut se upravi hodnota v prislune polozce alarmoveho pole
                EEPROM_write_int(eeaddr_alarmy + ((zvoleny_budik - 1) * 2),RTC_alarm[zvoleny_budik]);    // a zaroven se zapise do EEPROM
    
                STM_DS(false);                                              // serizeni casu v DS3231 podle STM32 (false = bez 30-sekundoveho testu)
              }
            else                                                            // kdyz k zadnemu ulozeni dojit nema (kratky stisk pri zadavani hodin nebo minut)
              {
                RTC_alarm[zvoleny_budik] = EEPROM_read_int(eeaddr_alarmy + ((zvoleny_budik - 1) * 2));       // obnovi se puvodni hodnota pole z EEPROM
              }
          }                                                                 // kratky stisk nalistovaneho budiku (nebo odpoctu) nic neudela a vypadne zpatky do menu "Alarm"


      }

    if (polozka == 27)                                                       // Mereni napeti zdroje
      {
        int8_t typ_vstupu = 0;                                               // prepinani mereni napeti 9V baterie, nebo 3V zalozniho knoflikoveho clanku
        
        while (digitalRead(pin_tl_ok) == HIGH and auto_exit_menu == false)   // dokud neni stisknuto tlacitko OK nebo nevyprsel cas pro automaticke vypadnuti z menu
          {

            if (millis() - auto_exit_casovani > AUTOEXITMENU)     auto_exit_menu = true;                 // cas bez stisku tlacitka vyprsel... provede se odchod z menu, jakoby bylo stisknuto kratce OK

            if (digitalRead(pin_tl_up) == LOW)                               // tlacitkem nahoru se meni parametr k zobrazeni
              {
                typ_vstupu ++;
                if (typ_vstupu > 3) typ_vstupu = 3;
                delay(100);
                while (digitalRead(pin_tl_up) == LOW) delay(100);
                delay(100);
                auto_exit_casovani = millis();                               // stisk tlacitka oddali automaticke vypadnuti z menu
              }
            if (digitalRead(pin_tl_dn) == LOW)                               // tlacitkem nahoru se meni parametr k zobrazeni (zdroj, reference, procesor, zalohovaci clanek)
              {
                typ_vstupu --;
                if (typ_vstupu < 0) typ_vstupu = 0;
                delay(100);
                while (digitalRead(pin_tl_dn) == LOW) delay(100);
                delay(100);
                auto_exit_casovani = millis();                               // stisk tlacitka oddali automaticke vypadnuti z menu
              }

            interrupty();                                                    // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni    
            cti_napajeni();


            if (typ_vstupu == 0)                                                 // zobrazeni hlavniho napajeciho napeti (9V)
              {
                pozice_tecky = 1;                                                // tecka na druhe sedmisegmentovce zprava
                D_pamet[4] = 190;                                                // znak "V." (hlavni napajeci napeti)
                if (osazeno_uref == true)
                  {
                    zobraz_cislo((long)napeti_9V, 1);
                  }
                else                                                             // kdyz neni osazena reference, napeti se nezobrazi
                  {
                    zobraz_text(93);                                             // "U. ---"
                  }
              }

            if (typ_vstupu == 1)                                                 // zobrazeni procent zalohovaci baterie [0 az 99%]
              {
                pozice_tecky = 0;                                                //  tecka se v zobrazeni procent nezobrazuje
                D_pamet[4] = 252;                                                // znak "b." (napeti zalozni baterie v %) - parametr 20 prepisue posledni segmentovku procentama
                zobraz_cislo((long)bat_proc * 10, 20);                           //                                             (proto se hodnota jeste nasobi 10)
              }
            
            if (typ_vstupu == 2)                                                 // zobrazeni reference (1,80V)
              {
                pozice_tecky = 2;                                                //  tecka na treti sedmisegmentovce zprava
                D_pamet[4] = 208;                                                // znak "r." (reference)
                if (osazeno_uref == true)
                  {
                    zobraz_cislo((long)(ref_in * 100.0), 1);
                  }
                else                                                             // kdyz neni osazena reference, napeti se nezobrazi
                  {
                    zobraz_text(94);                                             // "r. ---"
                  }
              }

            if (typ_vstupu == 3)                                                 // zobrazeni napajeni procesoru (3,30V)
              {
                pozice_tecky = 2;                                                //  tecka na treti sedmisegmentovce zprava
                D_pamet[4] = 243;                                                // znak "P." (napajeni procesoru)
                zobraz_cislo((long)(Vcc * 100.0), 1);                
              }            
            delay(300);
          }
      }


    if (polozka == 28)                                                       // Alarm for Darkness
      {
        bool pomprom_save = false;
        afd_bzuk = true;                                                     // po otevreni polozky AfD v menu se povoli napis  "-AFd-" na displeji a bzukani pri AfD
        afd_log = true;                                                      // povoli se i logovani do syslogu (zaznam prvniho prekroceni nastavene hodnoty)
        afd = plusminus('a', afd,130,255);
        if (bude_save == true) pomprom_save = true;
        if (pomprom_save == true)
          {
            EEPROM_update(eeaddr_AfD,afd);
            digitalWrite(pin_bok_out, LOW);
            predchozi_cas_afd = millis();
            afd_nabito = false;        
            afd_desetiny = 0;                                                // okamzite po zmene urovne se maze predchozi hodnota, se kterou se porovnava aktualni jas
            status_afd = false;
            digitalWrite(pin_bok_out, LOW);
          }
        afd = EEPROM_read(eeaddr_AfD);
        if (afd == 255) afd_bzuk = false;                                   // zakazani zobrazeni informace o prekroceni hranice na displeji
        else            afd_bzuk = true;                                    // povoleni zobrazeni informace o prekroceni hranice na displeji

        if (afd_styl == 2 or afd_styl == 3)     afd_bzuk = false;           // styly 2 a 3 rusi zobrazeni informace na displeji nezavisle na tom, jak je hranice nastavena
        
      }

//-----------------------------------------------------------------------------------------------------------
    if (polozka == 29)                                                       // DSO
      {

        zobraz_text(105 + DSO_index_podmenu);                                // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112
                                                                             //    zobrazi se okamzite po dlouhem stisku OK v hlavnim menu na polozce "DSO"

        while (digitalRead(pin_tl_ok) == LOW) delay(50);                     // cekani na uvolneni tlacitka [OK]
        delay(50);                                                           // a odruseni zakmitu

        bool predchozi_stav_ok = HIGH;                                       // tlacitko OK je uvolneno
        bool ukoncit_DSO = false;                                            // znacka pro ukonceni DSO funkci a navrat do hlavniho menu (zobrazeno "DSO")
        uint32_t casovani_dlouheho_stisku = 0;                               // pomocna znacka pro vyhodnocovani dlouheho stisku [OK]

        // listovani v prvni urovni podmenu (0=M; 1=NGC; 2=IC; 3=okular; 4=schopnost; 5=prumer; 6=delka dalekohledu)
        while (ukoncit_DSO == false and auto_exit_menu == false)             // dokud nevyprsel cas pro automaticke vypadnuti z menu nebo nedoslo k ukonceni DSO funkci
          {

            if (millis() - auto_exit_casovani > AUTOEXITMENU)     auto_exit_menu = true;     // cas bez stisku tlacitka vyprsel... provede se odchod z menu, jakoby bylo stisknuto kratce OK

            if (digitalRead(pin_tl_up) == LOW)                               // tlacitkem nahoru se meni parametr k zobrazeni
              {
                DSO_index_podmenu ++;
                if (DSO_index_podmenu > 7) DSO_index_podmenu = 7;
                zobraz_text(105 + DSO_index_podmenu);                        // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112
                delay(100);
                while (digitalRead(pin_tl_up) == LOW) delay(100);
                delay(100);
                auto_exit_casovani = millis();                               // stisk tlacitka oddali automaticke vypadnuti z menu
              }

            if (digitalRead(pin_tl_dn) == LOW)                               // tlacitkem nahoru se meni parametr k zobrazeni (zdroj, reference, procesor, zalohovaci clanek)
              {
                DSO_index_podmenu --;
                if (DSO_index_podmenu < 0) DSO_index_podmenu = 0;
                zobraz_text(105 + DSO_index_podmenu);                        // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 111
                delay(100);
                while (digitalRead(pin_tl_dn) == LOW) delay(100);
                delay(100);
                auto_exit_casovani = millis();                               // stisk tlacitka oddali automaticke vypadnuti z menu
              }

            if (digitalRead(pin_tl_ok) == LOW and predchozi_stav_ok == HIGH) // prave bylo stisknuto OK
              {
                predchozi_stav_ok = LOW;
                casovani_dlouheho_stisku = millis();
                delay(50);                                                   // odruseni zakmitu
              }

            if (millis() - casovani_dlouheho_stisku > 1000  and digitalRead(pin_tl_ok) == LOW)             // tlacitko je stisknute dostatecne dlouho na vstup do vybrane polozky
              {
                zobraz_text(68);                                                                           // jeste pred uvolnenim dlouho drzeneho tlacitka okamzite zobrazit  "-----"
              }


            if (digitalRead(pin_tl_ok) == HIGH and predchozi_stav_ok == LOW) // prave bylo uvolneno OK
              {
                auto_exit_casovani = millis();                                                              // stisk tlacitka oddaluje automaticke vypadnuti z menu
                predchozi_stav_ok = HIGH;
                delay(50);
                if (millis() - casovani_dlouheho_stisku > 1000)
                  {
                    while (digitalRead(pin_tl_ok) == LOW) delay(50);         // cekani na uvolneni tlacitka [OK]
                    delay(50);                                               // a odruseni zamitu


                    // podle polozky v podmenu listovat hodnotami
                    if (DSO_index_podmenu == 0)                              // listovani v Messierovkach
                      {
                        zadano = plusminus('0', DSO_posledni_messier,1,110); // rozsah Messierovek je obycejne 1 az 110 a zacina se poslednim zvolenym z minule
                        while (digitalRead(pin_tl_ok) == LOW)    delay(50);  // cekani na uvolneni tlacitka OK s odrusenim zakmitu
                        delay(100);
                        DSO_posledni_messier = zadano;                       // pri volbe DSO se zapamatuje posledni nalistovana hodnota (nenusi byt potvrzena)
                        if (bude_save == true)                               // nastavovani Messierovky skoncilo dlouhym stiskem [OK]
                          {          
                            zobraz_zvolene_dso(DSO_index_podmenu,zadano);
                          }
                        zobraz_text(105 + 0);                                // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112
                      }
                  
                    if (DSO_index_podmenu == 1)                              // listovani v NGC
                      {
                        zadano = plusminus('1', DSO_posledni_ngc,0,NGC_pocet-1);    // rozsah NGC je 0 az NGC_pocet a zacina se poslednim zvolenym NGC z minule
                                    
                        while (digitalRead(pin_tl_ok) == LOW)    delay(50);  // cekani na uvolneni tlacitka OK s odrusenim zakmitu
                        delay(100);
                        DSO_posledni_ngc = zadano;                           // pri volbe DSO se zapamatuje posledni nalistovana hodnota (nenusi byt potvrzena)
                        if (bude_save == true)                               // nastavovani IC objektu skoncilo dlouhym stiskem [OK]
                          {            
                            zobraz_zvolene_dso(DSO_index_podmenu,zadano);
                          }
                        zobraz_text(105 + 1);                                // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112
                      }
            
                    if (DSO_index_podmenu == 2)                              // listovani v IC
                      {
                        zadano = plusminus('2', DSO_posledni_ic,0,IC_pocet-1);      // rozsah IC je 0 az IC_pocet (defaultne 8) a zacina se poslednim zvolenym IC z minule
                                    
                        while (digitalRead(pin_tl_ok) == LOW)    delay(50);  // cekani na uvolneni tlacitka OK s odrusenim zakmitu
                        delay(100);
                        DSO_posledni_ic = zadano;                            // pri volbe DSO se zapamatuje posledni nalistovana hodnota (nenusi byt potvrzena)

                        if (bude_save == true)                               // nastavovani IC objektu skoncilo dlouhym stiskem [OK]
                          {
                            zobraz_zvolene_dso(DSO_index_podmenu,zadano);
                          }
                        zobraz_text(105 + 2);                                // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112
                      }
            
            
                    if (DSO_index_podmenu == 3)                              // listovani v ohniskovych delkach okularu
                      {
                        zadano = plusminus('3', DSO_index_ohniska_okularu,0,4);     // je preddefinovano 5 okularu s indexy 0 az 4
                                    
                        while (digitalRead(pin_tl_ok) == LOW)    delay(50);  // cekani na uvolneni tlacitka OK s odrusenim zakmitu
                        delay(100);
                        if (bude_save == true)                               // nastavovani IC objektu skoncilo dlouhym stiskem [OK]
                          {
                            DSO_index_ohniska_okularu = zadano;
                            DSO_delka_okularu = DSO_pole_ohniska_okularu[DSO_index_ohniska_okularu];  // ohniskova delka okularu v mm  (neuklada se do EEPROM)
                          }
                        zobraz_text(105 + 3);                                // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112
                      }
            
                    if (DSO_index_podmenu == 4)                              // volba schopnosti pozorovatele
                      {
                        zadano = plusminus('4', DSO_schopnosti,1,5);         // rozsah schopnosti pozorovatele je 1 az 5 a zacina se poslednim zvolenym stupnem ulozenym v EEPROM
                                    
                        while (digitalRead(pin_tl_ok) == LOW)    delay(50);  // cekani na uvolneni tlacitka OK s odrusenim zakmitu
                        delay(100);
                        if (bude_save == true)                               // nastavovani schopnosti pozorovatele skoncilo dlouhym stiskem [OK]
                          {
                            DSO_schopnosti = zadano;
                            uint16_t pomprom_dso = EEPROM_read_int(eeaddr_DSO_setup);
            
                            if (zadano != (pomprom_dso & 0b0000000000001111))
                              {
                                pomprom_dso = (pomprom_dso & 0b1111111111110000) | zadano;
                                EEPROM_write_int(eeaddr_DSO_setup,  pomprom_dso);              
                              }
                          }
                        zobraz_text(105 + 4);                                // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112
                      }
            
                    if (DSO_index_podmenu == 5)                              // volba referencniho jasu oblohy pro OC
                      {
                        zadano = plusminus('5', DSO_normal,190,255);         // rozsah jasu je 19,0 az 25,5 a zacina se posledni zvolenou hodnotou ulozenou v EEPROM

                        while (digitalRead(pin_tl_ok) == LOW)    delay(50);  // cekani na uvolneni tlacitka OK s odrusenim zakmitu
                        delay(100);
                        if (bude_save == true)                               // nastavovani referencniho jasu skoncilo dlouhym stiskem [OK]
                          {
                            DSO_normal = zadano;                            
                            uint16_t pomprom_dso = EEPROM_read(eeaddr_DSO_normal_MSA);
                            
                            if (zadano != pomprom_dso)
                              {
                                EEPROM_write(eeaddr_DSO_normal_MSA,  DSO_normal);              
                              }
                          }
                        zobraz_text(105 + 5);                                // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112
                      }
            
            
                    if (DSO_index_podmenu == 6)                              // listovani v prumerech dalekohledu
                      {
                        zadano = plusminus('6', DSO_index_prumeru,0,2);      // jsou preddefinovany 3 prumery dalekohledu s indexy 0 az 2
                                    
                        while (digitalRead(pin_tl_ok) == LOW)    delay(50);  // cekani na uvolneni tlacitka OK s odrusenim zakmitu
                        delay(100);
                        if (bude_save == true)                               // nastavovani prumeru dalekohledu skoncilo dlouhym stiskem [OK]
                          {
                            DSO_index_prumeru = zadano;
                            DSO_prumer_dalekohledu = DSO_pole_prumery_dalekohledu[zadano];    // prumer dalekohledu v mm
                            uint16_t pomprom_dso = EEPROM_read_int(eeaddr_DSO_setup);
            
                            if (zadano != ((pomprom_dso & 0b0000000011110000) >> 4))          // index prumeru dalekohledu je v druhem nibbllu (skutecne vyuzite jsou jen bity 4 a 5)
                              {
                                pomprom_dso = (pomprom_dso & 0b1111111100001111) | (zadano<<4);
                                EEPROM_write_int(eeaddr_DSO_setup,  pomprom_dso);              
                              }
                            
                          }
                        zobraz_text(105 + 6);                                // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112
                      }
            
                    if (DSO_index_podmenu == 7)                              // listovani v hlavnich ohniskovych delkach dalekohledu
                      {
                        zadano = plusminus('7', DSO_index_ohniska_dalekohledu,0,2);           // jsou preddefinovany 3 delky dalekohledu s indexy 0 az 2
                                    
                        while (digitalRead(pin_tl_ok) == LOW)    delay(50);  // cekani na uvolneni tlacitka OK s odrusenim zakmitu
                        delay(100);
                        if (bude_save == true)                               // nastavovani IC objektu skoncilo dlouhym stiskem [OK]
                          {
                            DSO_index_ohniska_dalekohledu = zadano;
                            DSO_delka_dalekohledu = DSO_pole_delky_dalekohledu[zadano];       // ohniskova delka dalekohledu v mm
                            uint16_t pomprom_dso = EEPROM_read_int(eeaddr_DSO_setup);
            
                            if (zadano != ((pomprom_dso & 0b0000111100000000) >> 8))          // index prumeru dalekohledu je ve tretim nibbllu (skutecne vyuzite jsou jen bity 9 a 10)
                              {
                                pomprom_dso = (pomprom_dso & 0b1111000011111111) | (zadano<<8);
                                EEPROM_write_int(eeaddr_DSO_setup,  pomprom_dso);              
                              }
                            
                          }
                        zobraz_text(105 + 7);                                // napisy do podmenu DSO jsou definovany na textovych indexech 105 az 112

                      }
                    
                  }
                else
                  {
                    ukoncit_DSO = true;                    
                  }
              }
            

            interrupty();                                                    // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni    
          }
      
      }

//-----------------------------------------------------------------------------------------------------------


    zobraz_text(pozice_menu1);                                               // po navratu z podmenu se zobrazi aktualne vybrana polozka
    auto_exit_casovani = millis();
    
    delay(50);
    while (digitalRead(pin_tl_ok) == LOW)                                    // ze zmeny hodnoty polozky se vraci dlouhym stiskem tlacitka OK
      {                                                                      //  ted je nutne pockat na jeho uvolneni
        delay(20);
      }
    
    delay(100);
  }
//------------------------------------------



//------------------------------------------
//  listovani v parametrech zvolene polozky o +/- 1 pomoci tlacitek nahoru adolu
int16_t plusminus(char znak1, int16_t start_hodnota, int16_t minimalni_hodnota, int16_t maximalni_hodnota)
  {
    int16_t vystup = start_hodnota;
    int16_t nova_hodnota = start_hodnota;

    uint8_t stav_casovani = 0;                                      // casovani zmeny hodnot se meni v zavislosti na delce drzeni tlacitka 
    uint32_t casova_znacka = millis();
    uint32_t casova_znacka2 = 0;
    

    //--------------------------  zobrazeni puvodni hodnoty na displeji -------------------

    switch (znak1)
      {
        case 'A':                                                // (automat)
          D_pamet[4] =  znak_A;                                  // velke A
          D_pamet[3] = 0b00000000;                               // druha sedmisegmentovka zleva (rad tisicu) je prazdna
          pozice_tecky = 0;
          zobraz_cislo(vystup,1);
          break;
 
        case 'P':                                                // (prumerovani)
          D_pamet[4] =  znak_P;                                  // velke P
          D_pamet[3] = 0b00000000;                               // druha sedmisegmentovka zleva (rad tisicu) je prazdna
          pozice_tecky = 0;
          zobraz_cislo(vystup,1);
          break;

        case 'd':                                                // (den)
          D_pamet[4] =  znak_d;                                  // male d
          D_pamet[3] = 0b00000000;                               // druha sedmisegmentovka zleva (rad tisicu) je prazdna
          pozice_tecky = 0;
          zobraz_cislo(vystup,1);
          break;

        case 'M':                                                // (mesic nebo minuta)
          D_pamet[4] =  znak_M;                                  // velke M
          D_pamet[3] = 0b00000000;                               // druha sedmisegmentovka zleva (rad tisicu) je prazdna
          pozice_tecky = 0;
          zobraz_cislo(vystup,1);
          break;

        case 'r':                                                // (rok)
          D_pamet[4] =  znak_r;                                  // male r
          D_pamet[3] = 0b00000000;                               // druha sedmisegmentovka zleva (rad tisicu) je prazdna
          pozice_tecky = 0;
          zobraz_cislo(vystup,1);
          break;

        case 'H':                                                // (hodina)
          D_pamet[4] =  znak_H;                                  // velke H
          D_pamet[3] = 0b00000000;                               // druha sedmisegmentovka zleva (rad tisicu) je prazdna
          pozice_tecky = 0;
          if (vystup < 24)   zobraz_cislo(vystup,1);             // v pripade nastaveni budiku je mozne prelistovat az na 24 hodin, to ale znamena vypnuti budiku
          else
            {
              zobraz_text(67);                                   // "H  --"
            }
          break;

        case 'L':                                                // (list)
          D_pamet[4] =  znak_L;                                  // velke L
          D_pamet[3] = 0b00000000;                               // druha sedmisegmentovka zleva (rad tisicu) je prazdna
          pozice_tecky = 0;
          zobraz_cislo(vystup,2);                                // speciani typ vypisu cisla            
          break;

        case 'S':                                                 // (zona)
          zobraz_text(vystup + 57);                               //  "2iMA" / "LEto"
          break;

        case 'T':                                                 // (trasovani)
          zobraz_text(vystup + 96);                               //  "tr.oFF" / "tr. on"
          break;


        case 'R':                                                // (rektascenze)
          D_pamet[4] =  znak_R1;                                 // male r   (v napisu 'rA.')
          D_pamet[3] =  znak_R2;                                 // velke A. (v napisu 'rA.')
          pozice_tecky = 1;                                      // jedno desetinne misto (0.0 az 23.9)
          zobraz_cislo(vystup,17);
          break;

        case 'D':                                                // (deklinace)
          D_pamet[4] =  znak_D1;                                 // male d   (v napisu 'dE.')
          D_pamet[3] =  znak_D2;                                 // velke E. (v napisu 'dE.')
          pozice_tecky = 0;                                      // bez desetinnych mist (-90 az +90)
          zobraz_cislo(vystup,18);
          break;

        case 'X':                                                // (interval pro rezim R30s)
          D_pamet[4] =  134;                                     //  "I."
          D_pamet[3] =  72;                                      //  "="
          pozice_tecky = 0;                                      // bez desetinnych mist (10 az 250, 5M, 7M, 10M, 20M, 30M)
          zobraz_cislo(vystup,19);
          break;

        case 'b':                                                // nastaveni budiku
          if (vystup < 6)
            {
              zobraz_ho_mi(RTC_alarm[vystup],vystup);            // zobrazeni alarmu ve formatu "b.20.45" (pro aktivni budik) nebo "b.---2" (pro neaktivni budik)
            }
          else
            {
              zobraz_odpocmi(RTC_alarm[vystup]);                 // zobrazeni odpoctu ve formatu "o.-457" (pro aktivni odpocet) nebo "o.----" (pro neaktivni odpocet)
            }
          break;

        case 'a':                                                // (Alarm for Darkness)
          D_pamet[4] =  znak_AD1;                                // velke "A." (v napisu "A.d.")
          D_pamet[3] =  znak_AD2;                                // male  "d." (v napisu "A.d.")
          pozice_tecky = 1;                                      // jedno desetinne misto (13.0 az 25.5)
          if (vystup == 255)                                     // hodnota 255 vypina funkci
            {
              zobraz_text(69);                                   // "A.d.---"  (funkce vypnuta)
              status_afd = false;
              digitalWrite(pin_bok_out, LOW);
              MODdata[31] = MODdata[31] & 0b0111111111111111;    // modbus status
              afd_bzuk = false;
              afd_log = false;                                   // pri vypnutem AfD se zakaze i logovani AFD do syslogu
            }
          else
            {
              zobraz_cislo(vystup,21);                           // zobrazeni hodnoty ve formatu "A.d.15.4"          
            }

          break;

       
        case '#':                                                // (hlavni menu)
          if (digitalRead(pin_DATA_RDY) == osazeno_gps)          // pri zasunuti GPS (nebo jine externi desky) se zrusi prednastavene pozorovaci stanoviste
            {
              if ((EEPROM_read(eeaddr_TS_stanoviste) & 0b11100000) > 0)                // zrusi se ale jen v pripade, ze je nejake nastavene (aby se porad dokola neprepisovala jedna bunka EEPROM)
                {
                  EEPROM_update(eeaddr_TS_stanoviste,(EEPROM_read(eeaddr_TS_stanoviste) & 0b00011111));        // smazat nejvyssi 3 bity z adresy 48
                }
            }
          if (nova_hodnota == 13 and pole_zobr_menu[13] == false ) nova_hodnota = 14;  // okamzita aktualizace napisu pro pripad, ze dojde k vytazeni nebo zasunuti GPS modulu
          if (nova_hodnota == 14 and pole_zobr_menu[14] == false ) nova_hodnota = 13;
          zobraz_text(nova_hodnota);                                                   // polozky menu jsou seskladany na indexech  [0] az [27]
          break;

        case 'p':                                                // listovani v planetach
          zobraz_text(vystup + 79);                              // zobrazeni zkratky planety (indexy 80 az 90) pri vstupu do podmenu
          break;


        case '0':                                                // listovani v Messierovkach
          D_pamet[4] =  55;                                      // velke M
          pozice_tecky = 0;
          zobraz_cislo(vystup,1);
          break;
          
        case '1':                                                // listovani v NGC
          D_pamet[4] =  84;                                      // male 'n'
          pozice_tecky = 0;
          zobraz_cislo(ngc_odkaz[vystup][0],1);
          break;

        case '2':                                                // listovani v IC
          D_pamet[4] =  4;                                       // male 'i'
          pozice_tecky = 0;
          zobraz_cislo(ic_odkaz[vystup][0],1);
          break;


        case '3':                                                // listovani v okularech "o.o." je zkratka pro "Ohnisko Okularu"
          D_pamet[4] =  znak_dso_okular_1;                       // "o."
          D_pamet[3] =  znak_dso_okular_2;                       // "o."
          pozice_tecky = 0;
          zobraz_cislo(DSO_pole_ohniska_okularu[vystup],8);              // styl 8 zachovava prvni dve pozice na displeji 
          break;


        case '4':                                                // nastaveni schopnosti pozorovatele
          D_pamet[4] =  znak_dso_schop_1;                        // "S."
          D_pamet[3] =  znak_dso_schop_2;                        // "P."
          pozice_tecky = 0;
          
          zobraz_cislo(vystup,8);                                // styl 8 zachovava prvni dva znaky na sedmisegmentovkach
          break;

        case '5':                                                // nastaveni referencniho jasu
          D_pamet[4] =  znak_dso_ref_1;                          // "r."
          D_pamet[3] =  znak_dso_ref_2;                          // "J."
          pozice_tecky = 1;          
          zobraz_cislo(vystup,25);                                // styl 25 zachovava prvni dva znaky na sedmisegmentovkach a cislo je na 1 desetinne misto
          break;


        case '6':                                                // listovani v prumerech dalekohledu
          D_pamet[4] =  znak_dso_prumer_1;                       // 'd.'
          pozice_tecky = 0;
          zobraz_cislo(DSO_pole_prumery_dalekohledu[vystup],1);              // styl 1 zachovava prvni znak na displeji
          break;

        case '7':                                                // listovani v hlavnich ohniskovych delkach dalekohledu
          D_pamet[4] =  znak_dso_ohnisko_1;                      // 'L.'
          pozice_tecky = 0;
          zobraz_cislo(DSO_pole_delky_dalekohledu[vystup],1);                // styl 1 zachovava prvni znak na displeji
          break;

        case '@':                                                // (editace viditelnych polozek v menu)
          break;

        case '&':                                                // (vypinani porouchanych periferii)
          break;

      
      }
    
    while (digitalRead(pin_tl_ok) == HIGH and auto_exit_menu == false)     // z teto smycky se vypadne jen tlacitkem OK (kratkym, nebo dlouhym) nebo vyprsenim casovace automatickeho opusteni menu
      {
        if (znak1 != '@' and znak1 != '&')                                 // pri editaci viditelnych polozek menu nebo vypinani periferii se interrupty neprovadi
          {
             for (uint8_t dpam = 0 ; dpam < 5 ; dpam ++)
               {
                 D_pam_pred_int[dpam] = D_pamet[dpam];
               }
             interrupty();
             for (uint8_t dpam = 0 ; dpam < 5 ; dpam ++)
               {
                 D_pamet[dpam] = D_pam_pred_int[dpam];
               }
             aktualizuj_displej();
          }

        if (znak1 == '#' and nova_hodnota == 13 and digitalRead(pin_DATA_RDY) != osazeno_gps)     // (jen pro pohyb v hlavnim menu) je zvolena polozka 13(GPS), ale GPS se vypnula
          {
            pole_zobr_menu[13] = false;
            pole_zobr_menu[14] = true; 
            nova_hodnota = 14;
            zobraz_text(nova_hodnota);
          }

        if (znak1 == '#' and nova_hodnota == 14 and digitalRead(pin_DATA_RDY) == osazeno_gps)     // (jen pro pohyb v hlavnim menu) je zvolena polozka 14(STAN), ale GPS se zapnula
          {
            pole_zobr_menu[13] = true;
            pole_zobr_menu[14] = false; 
            nova_hodnota = 13;
            zobraz_text(nova_hodnota);
          }



        if (digitalRead(pin_tl_up) == LOW  or digitalRead(pin_tl_dn) == LOW)  auto_exit_casovani = millis(); // kazdy stisk tlacitka nahoru nebo dolu spousti casovac automatickeho vypadnuti z menu

        if (millis() - auto_exit_casovani > AUTOEXITMENU)     auto_exit_menu = true;                     // cas bez stisku tlacitka vyprsel... provede se odchod z menu, jakoby bylo stisknuto kratce OK
        


        // --------------  pocitani po jednotkach na kratky klik ---------------
        if (digitalRead(pin_tl_up) == LOW and stav_casovani == 0 and millis() > (casova_znacka + 100))   // prvni stisk tlacitka nahoru s odrusenim zakmitu na 0,1sekundy
          {
            stav_casovani = 1;
            casova_znacka = millis();
            nova_hodnota = plus_hodnota(znak1 , nova_hodnota, minimalni_hodnota , maximalni_hodnota);    // okamzite pri stisku (po desetine sekundy) pricte jednicku
            casova_znacka2 = millis();        
          }

        if (digitalRead(pin_tl_dn) == LOW and stav_casovani == 0 and millis() > (casova_znacka + 100))   // prvni stisk tlacitka doluru s odrusenim zakmitu na 0,1sekundy
          {
            stav_casovani = 1;
            casova_znacka = millis();
            nova_hodnota = minus_hodnota(znak1 , nova_hodnota, minimalni_hodnota, maximalni_hodnota);    // okamzite pri stisku (po desetine sekundy) odecte jednicku
            casova_znacka2 = millis();        
          }
        // --------------


        // --------------  prepnuti z klikani na pomaly automat po jedne sekunde drzeni tlacitka ---------------        
        if ( (digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW ) and stav_casovani == 1 and millis() > (casova_znacka2 + 1000))   // nektere tlacitko je drzeno alespon sekundu
          {
            stav_casovani = 2;
            casova_znacka2 = millis();
          }
        // --------------

       

        // --------------  pocitani po jednotkach automaticky po sekundovem drzeni tlacitka ---------------
        if (digitalRead(pin_tl_up) == LOW and stav_casovani == 2 and  millis() > (casova_znacka + 200))  // je drzeno tlacitko nahoru, kazdou 0,2 sekundy se pricita jednicka
          {
            casova_znacka = millis();
            nova_hodnota = plus_hodnota(znak1 , nova_hodnota, minimalni_hodnota, maximalni_hodnota);
            if (millis() > (casova_znacka2 + 5000)) stav_casovani = 3;                                   // po trech sekundach drzeni se prechazi na vysokou rychlost pricitani
          }

        if (digitalRead(pin_tl_dn) == LOW and stav_casovani == 2 and  millis() > (casova_znacka + 200))  // je drzeno tlacitko nahoru, kazdou 0,2 sekundy se odecita jednicka
          {
            casova_znacka = millis();
            nova_hodnota = minus_hodnota(znak1 , nova_hodnota, minimalni_hodnota , maximalni_hodnota);
            if (millis() > (casova_znacka2 + 5000)) stav_casovani = 3;                                   // po trech sekundach drzeni se prechazi na vysokou rychlost odecitani
          }
        
        // --------------




        // --------------  rychle pocitani automaticky po 3-sekundovem drzeni ---------------        
        if (digitalRead(pin_tl_up) == LOW and stav_casovani == 3 and  millis() > (casova_znacka + 50))   // po trech sekundach drzeni tlacitka nahoru, se kazdych 50 ms pricita jednicka
          {
            casova_znacka = millis();
            nova_hodnota = plus_hodnota(znak1 , nova_hodnota, minimalni_hodnota , maximalni_hodnota);
          }      

        if (digitalRead(pin_tl_dn) == LOW and stav_casovani == 3 and  millis() > (casova_znacka + 50))   // po trech sekundach drzeni tlacitka dolu, se kazdych 50 ms odecita jednicka
          {
            casova_znacka = millis();
            nova_hodnota = minus_hodnota(znak1 , nova_hodnota, minimalni_hodnota , maximalni_hodnota);
          }      

        // --------------



        // --------------  uvolneni tlacitek  ---------------        
        if (digitalRead(pin_tl_up) == HIGH and digitalRead(pin_tl_dn) == HIGH and  millis() > (casova_znacka + 100))  // obe tlacitka uvolnena alespon na desetinu sekundy
          {
            stav_casovani = 0;
            casova_znacka = millis(); 
            casova_znacka2 = millis();        
          }
        // --------------

      
        if (znak1 == 'b' and nova_hodnota == 6)                  // kdyz je zobrazene menu "Alarm" a polozka "Odpocet", provadi se kazdou sekundu aktualizace 
          {          
            if ((millis() % 1000) == 0 and aktual_odp == true)   // nekdy v prvni desetine kazde sekundy se displej 1x aktualizuje
              {
                zobraz_odpocmi(RTC_alarm[6]);                    // zobrazeni odpoctu ve formatu "o.-457" (pro aktivni odpocet) nebo "o.----" (pro neaktivni odpocet)
                aktual_odp = false;
                auto_exit_casovani = millis();                   // jedina polozka, pri ktere nedochazi k automatickemu vypadnuti z menu 
              }
            if ((millis() % 1000) > 100 and aktual_odp == false)   aktual_odp = true;     // po prvni desetine sekundy od aktualizace displeje se provede priprava na dalsi aktualizaci

          }


      
      }        // konec listovaci smycky, ze ktere se vypadne jen tlacitkem OK
    


    if (digitalRead(pin_tl_ok) == LOW)                           // potvrzovaci tlacitko v polozce
      {
        auto_exit_casovani = millis();                           // kazdy stisk tlacitka ok spousti casovac automatickeho vypadnuti z menu
        delay(50);                                               // odruseni zakmitu
        vystup = nova_hodnota;
        uint32_t startTime = millis();
        while (digitalRead(pin_tl_ok) == LOW)                    // dokud je tlacitko stisknute, nic se nedeje
          {
            delay(50);
            trvani_stisku2 = millis() - startTime;
            if (trvani_stisku2 > 500)                            // stisk byl delsi nez 0,5 sekundy
              {
                bude_save = true;

                switch(znak1)
                  {
                    case 'L':
                      zobraz_text(68);                           // dlouhy stisk OK pri prohlizeni starych zaznamu (funkce LIST) zpusobi napis "-----"
                      break;

                    case 'A':                                    // (automat)
                    case 'P':                                    // (prumerovani)
                    case 'd':                                    // (den)
                    case 'M':                                    // (mesic nebo minuta)
                    case 'r':                                    // (rok)
                    case 'H':                                    // (hodina)
                    case 'S':                                    // (casova zona)
                    case 'T':                                    // (trasovani)
                    case 'R':                                    // (rektascenze)
                    case 'D':                                    // (deklinace)
                    case 'a':                                    // (Alarm for Darkness)
                    case '@':                                    // (editace viditelnych polozek v menu)
                    case '&':                                    // (vypinani poskozenych periferii)
                    case 'X':                                    // (interval pro rezim 30s)
                    case '4':                                    // (nastaveni schopnosti pozorovatele v menu DSO se uklada do EEPROM)
                    case '5':                                    // (nastaveni referencniho jasu v menu DSO se uklada do EEPROM)
                    case '6':                                    // (nastaveni prumeru dalekohledu v menu DSO se uklada do EEPROM)
                    case '7':                                    // (nastaveni ohniska dalekohledu v menu DSO se uklada do EEPROM)
                      zobraz_text(38);                           // dlouhy stisk OK pri zadavani nejakych parametru zpusobi napis "SAVE"
                      break;                     

                    case 'p':                                    // (planety)
                    case 'b':                                    // (budik)
                    case '0':                                    // listovani v Messierovkach
                    case '1':                                    // listovani v NGC
                    case '2':                                    // listovani v IC
                    case '3':                                    // listovani v okularech
                      zobraz_text(68);                           // dlouhy stisk OK pri nalistovani alarmu, planety, nebo DSO objektu zobrazi napis "-----"
                      break;                    
                    
                    case '#':                                    // (menu)
                      break;                                     // dlouhy stisk OK pri listovani v menu tady nezobrazi nic, ale v nasledujicich prikazech zobrazi vybrane podmenu
                 
                  }
                
                delay(500);
                break;
              }
            if (digitalRead(pin_tl_ok) == HIGH) delay(50);       // kdyz se behem drzeni tlacitka OK objevi kratky vypadek kontaktu, tak se odignoruje         
          }
        if (trvani_stisku2 <= 500)                               // stisk byl kratsi nez 0,5 sekundy
          {
            switch(znak1)
              {
                case '#':                                        // pri kratkem stisku OK pri listovani v hlavnim menu se NENASTAVI puvodni polozka, ale zustane ulozena aktualni
                  break;

                case '@':                                        // menu editace viditelnych a neviditelnych polozek je trochu specialni, protoze se kratkym stiskem OK meni jas
                  if (menu_enable[vystup] == true)   menu_enable[vystup] = false;   // ... prepina stav mezi "viditelna" / "neviditelna" polozka
                  else                               menu_enable[vystup] = true;      
  
                  if (menu_enable[vystup] == true)   jas_displeje = 7;              // a zaroven se jeste aktualizuje jas prave editovane polozky
                  else                               jas_displeje = 1;
                  zobraz_text(vystup);                           // polozky menu jsou seskladany na indexech  [0] az [29]
                  break;


                case '&':                                        // editace zapnutych a vypnutych periferii je trochu specialni, protoze se kratkym stiskem OK meni jas
                  if (perif_enable[vystup] == true)  perif_enable[vystup] = false;   // ... prepina stav mezi "zapnutou" / "vypnutou" periferii
                  else                               perif_enable[vystup] = true;      
  
                  if (perif_enable[vystup] == true)  jas_displeje = 7;              // a zaroven se jeste aktualizuje jas prave editovane polozky
                  else                               jas_displeje = 1;
                  zobraz_text(vystup + 73);                      // polozky menu jsou seskladany na indexech  [73] az [77]
                  break;

               
               
               default:                                          // u vsech ostatnich listovani a nastavovani se vrati puvodni vstupni hodnota
                vystup = start_hodnota;                          // nastavene cislo se rusi a zustava puvodni                  
              }

            delay(200);                                          // pauza pred opustenim menu (nebo po prepnuti viditelnosti polozky)
            bude_save = false;
          }
      }      

    if (auto_exit_menu == true)                                  // nekde v predchozi smycce doslo k vyprseni casu pro autoexit
      {
        vystup = start_hodnota;                                  // vypadne se jakoby bylo kratce stisknuto OK
        bude_save = false;
      }

    return vystup;
  }
//------------------------------------------



//==============================================================================================
// pricitani a odecitani jednicky s kontrolou minimalni a maximalni hodnoty
int16_t plus_hodnota(char typ_pricitani , int16_t hodnota, int16_t minimum , int16_t maximum)
  {
    if (hodnota > maximum) hodnota = maximum;
    if (hodnota < minimum) hodnota = minimum;
    pozice_tecky = 0;
    switch (typ_pricitani)
      {
        case 'A':                                                // (automat)
        case 'P':                                                // (prumerovani)
        case 'd':                                                // (den)
        case 'M':                                                // (mesic nebo minuta)
        case 'r':                                                // (rok)
          if (hodnota + 1 <= maximum )  hodnota ++;              // bezne pricteni hodnoty s kontrolou na maximum
          zobraz_cislo(hodnota,1);                               // (styl 1 zachovava prvni znak na prvni jednotce, ostatni prepisuje cislem 0 az 9999 )
          break;


        case '0':                                                // listovani v Messierovkach
          hodnota ++;
          if (hodnota > maximum) hodnota = minimum;              // pri prekroceni maxima se pokracuje od minima
          zobraz_cislo(hodnota,1);                               // (styl 1 zachovava prvni znak na prvni jednotce, ostatni prepisuje cislem 0 az 9999 )
          break;

        case '1':                                                // listovani v NGC nezobrazuje index, ale prvni sloupec z prevodni tabulky z pole 'ngc_odkaz[]'
          hodnota ++;
          if (hodnota > maximum) hodnota = minimum;              // pri prekroceni maxima se pokracuje od minima
          zobraz_cislo(ngc_odkaz[hodnota][0],1);                 // (styl 1 zachovava prvni znak "N" na prvni jednotce, ostatni prepisuje cislem 0 az 9999 )
          break;

        
        case '2':                                                // listovani v IC nezobrazuje index, ale prvni sloupec z prevodni tabulky z pole 'ic_odkaz[]'
          hodnota ++;
          if (hodnota > maximum) hodnota = minimum;              // pri prekroceni maxima se pokracuje od minima
          zobraz_cislo(ic_odkaz[hodnota][0],1);                  // (styl 1 zachovava prvni znak na prvni jednotce "i", ostatni prepisuje cislem 0 az 9999 )
          break;


        case '3':                                                // nastavovani ohniskove vzdalenosti okularu
          if (hodnota + 1 <= maximum )  hodnota ++;              // zarazka na maximu
          zobraz_cislo(DSO_pole_ohniska_okularu[hodnota],8);             // (styl 8 zachovava prvni dva znaky na displeji)
          break;

        case '4':                                                // nastavovani schopnosti pozorovatele
          if (hodnota + 1 <= maximum )  hodnota ++;              // zarazka na maximu
          zobraz_cislo(hodnota,8);                              // (styl 8 zachovava prvni dva znaky na displeji, bez desetnnych mist)
          break;

        case '5':                                                // nastavovani referencniho jasu
          if (hodnota + 1 <= maximum )  hodnota ++;              // zarazka na maximu
          zobraz_cislo(hodnota,25);                              // (styl 25 zachovava prvni dva znaky na displeji a zobrazene cislo je na 1 desetinne misto)
          break;

        case '6':                                                // nastavovani prumeru dalekohledu
          if (hodnota + 1 <= maximum )  hodnota ++;              // zarazka na maximu
          zobraz_cislo(DSO_pole_prumery_dalekohledu[hodnota],1);             // (styl 1 zachovava prvni znak na displeji)
          break;


        case '7':                                                // nastavovani hlavni ohniskove vzdalenosti dalekohledu
          if (hodnota + 1 <= maximum )  hodnota ++;              // zarazka na maximu
          zobraz_cislo(DSO_pole_delky_dalekohledu[hodnota],1);               // (styl 1 zachovava prvni znak na displeji)
          break;



        case 'H':                                                // (hodina)
          hodnota ++;
          if(hodnota > maximum)    hodnota = minimum;         
          if (hodnota < 24)  zobraz_cislo(hodnota,1);
          else               zobraz_text(67);                    // "H  --" pri nastaveni budiku je mozne volbou 24H budik zrusit
          break;

        case 'b':                                                // (budik)
          if (hodnota + 1 <= maximum )  hodnota ++;              // bezne pricteni hodnoty s kontrolou na maximum
          if (hodnota < 6)
            {
              zobraz_ho_mi(RTC_alarm[hodnota],hodnota);          // zobrazeni alarmu ve formatu "b.20.45" (pro aktivni budik) nebo "b.---2" (pro neaktivni budik)
            }
          else
            {
              zobraz_odpocmi(RTC_alarm[hodnota]);                // zobrazeni odpoctu ve formatu "o.-457" (pro aktivni odpocet) nebo "o.----" (pro neaktivni odpocet)
            }
          break;

        case 'a':                                                // (Alarm for Darkness)
          hodnota ++;
          if (hodnota > maximum )  hodnota = minimum;            // pretoceni pri prekroceni maxima
          if (hodnota == 255)                                    // hodnota 255 vypina funkci
            {
              zobraz_text(69);                                   // "A.d.---"  (funkce vypnuta)
            }
          else
            {
              zobraz_cislo(hodnota,21);                          // zobrazeni hodnoty ve formatu "A.d.15.4"          
            }
          break;


        case 'L':                                                // (list)
          if (hodnota - 1 >= minimum )  hodnota --;              // list funguje opacne (misto pricitani se odecita)
          zobraz_cislo(hodnota,2);
          break;

        case 'S':                                                // (zona)
          hodnota ++;
          if (hodnota > maximum) hodnota = maximum;              // maximum nelze prekrocit
          zobraz_text(hodnota + 57);
          break;

        case 'T':                                                // (trasovani)
          hodnota ++;
          if (hodnota > maximum) hodnota = maximum;              // maximum nelze prekrocit
          zobraz_text(hodnota + 96);
          break;


        case 'R':                                                // (rektascenze)
          hodnota ++;
          if (hodnota > maximum) hodnota = minimum;              // pri prekroceni maxima se pokracuje od minima
          zobraz_cislo(hodnota,17);
          break;

        case 'D':                                                // (deklinace)
          hodnota ++;
          if (hodnota > maximum) hodnota = minimum;              // pri prekroceni maxima se pokracuje od minima
          zobraz_cislo(hodnota,18);
          break;

        case 'X':                                                // (interval R30s)
          hodnota ++;
          if (hodnota > maximum) hodnota = maximum;              // pri prekroceni maxima se na maximu zarazi
          zobraz_cislo(hodnota,19);
          break;

        case 'p':                                                // (planety)
          hodnota ++;
          if (hodnota > maximum) hodnota = maximum;              // pri prekroceni maxima se na maximu zarazi
          zobraz_text(hodnota + 79);
          break;

       
        case '#':                                                // (hlavni menu) - pohyb k nizsim indexum polozek (tlacitko nahoru)
          hodnota --;
          if (hodnota < 0)   hodnota = 0;

          while (pole_zobr_menu[hodnota] == false)               // kdyz je zvolena polozka nezobrazitelna, prepne se na dalsi nizsi
            {
              hodnota --;
              if (hodnota < 0)
                {
                  hodnota = 0;
                  break;                                         // pri podteceni pod nultou polozku se vypadne ze smycky while
                }
            }
          while (pole_zobr_menu[hodnota] == false)               // pokud se z predchozi smycky vystoupi pres podteceni nulove polozky
            {
              hodnota ++;                                        // postupnym zvysovanim indexu se hleda nejnizsi povolena polozka
            }          
          zobraz_text(hodnota);                                  // polozky menu jsou seskladany na indexech  [0] az [27]
          break;                                                 // tenhle break ukoncuje case '#'


        case '@':                                                // (editace viditelnychj polozek menu) - pohyb k nizsim indexum polozek (tlacitko nahoru)
          hodnota --;
          while (spec_rezim == 0 and hodnota > 14 and hodnota < 22)  hodnota --;     // polozky menu pro specialni funkce se bez zasunuteho HW vubec nezobrazuji
          if (hodnota < 0)        hodnota = 0;                   // zastaveni na polozce "List"

          if (menu_enable[hodnota] == true) jas_displeje = 7;    // povolene polozky menu se zobrazi vysokym jasem
          else                              jas_displeje = 1;    // zakazane polozky menu se zobrazi nizkym jasem
                   
          zobraz_text(hodnota);                                  // polozky menu jsou seskladany na indexech  [0] az [28]
          break;                                                 // tenhle break ukoncuje case '@'


        case '&':                                                // (zapinani / vypinani periferii) - pohyb k nizsim indexum (tlacitko nahoru)
          hodnota --;
          if (hodnota < 0)        hodnota = 0;                   // zastaveni na periferii "BME  "

          if (perif_enable[hodnota] == true)jas_displeje = 7;    // povolene periferie se zobrazi vysokym jasem
          else                              jas_displeje = 1;    // zakazane periferie se zobrazi nizkym jasem
                   
          zobraz_text(hodnota + 73);                             // periferie jsou seskladany na indexech  [73] az [77]
          break;                                                 // tenhle break ukoncuje case '&'



      } 

    return hodnota;
  }
//----------------------------------------------



//----------------------------------------------
int16_t minus_hodnota(char typ_odecitani , int16_t hodnota, int16_t minimum, int16_t maximum)
  {
    if (hodnota > maximum) hodnota = maximum;
    if (hodnota < minimum) hodnota = minimum;

    pozice_tecky = 0;
    switch (typ_odecitani)
      {
        case 'A':                                                // (automat)
        case 'P':                                                // (prumerovani)
        case 'd':                                                // (den)
        case 'M':                                                // (mesic nebo minuta)
        case 'r':                                                // (rok)
          if (hodnota - 1 >= minimum )  hodnota --;              // bezne odecteni hodnoty s kontrolou na minimum
          zobraz_cislo(hodnota,1);                               // (styl 1 zachovava prvni znak na prvni jednotce, ostatni prepisuje cislem 0 az 9999 )
          break;

        case '0':                                                // listovani v Messierovkach
          hodnota --;
          if (hodnota < minimum) hodnota = maximum;              // pri podlezeni pod minimum se nastavi maximum
          zobraz_cislo(hodnota,1);                               // (styl 1 zachovava prvni znak na prvni jednotce "M", ostatni prepisuje cislem 0 az 9999 )
          break;


        case '1':                                                // listovani v NGC nezobrazuje index, ale prvni sloupec z prevodni tabulky z pole 'ngc_odkaz[]'
          hodnota --;
          if (hodnota < minimum) hodnota = maximum;              // pri podlezeni pod minimum se nastavi maximum
          zobraz_cislo(ngc_odkaz[hodnota][0],1);                 // (styl 1 zachovava prvni znak na prvni jednotce "n", ostatni prepisuje cislem 0 az 9999 )
          break;

        
        case '2':                                                // listovani v IC nezobrazuje index, ale prvni sloupec z prevodni tabulky z pole 'ic_odkaz[]'
          hodnota --;
          if (hodnota < minimum) hodnota = maximum;              // pri podlezeni pod minimum se nastavi maximum
          zobraz_cislo(ic_odkaz[hodnota][0],1);                  // (styl 1 zachovava prvni znak na prvni jednotce "i", ostatni prepisuje cislem 0 az 9999 )
          break;

        case '3':                                                // listivani v okularech
          if (hodnota - 1 >= minimum )  hodnota --;              // bezne odecteni hodnoty s kontrolou na minimum
          zobraz_cislo(DSO_pole_ohniska_okularu[hodnota],8);     // (styl 8 zachovava prvni znaky na prvnich dvou jednotkach)
          break;


        case '4':                                                // nastaveni schopnosti pozorovatele
          if (hodnota - 1 >= minimum )  hodnota --;              // bezne odecteni hodnoty s kontrolou na minimum
          zobraz_cislo(hodnota,8);                               // (styl 8 zachovava prvni dva znaky na displeji, bez desetinnych mist)
          break;
        
        case '5':                                                // nastaveni referencniho jasu
          if (hodnota - 1 >= minimum )  hodnota --;              // bezne odecteni hodnoty s kontrolou na minimum
          zobraz_cislo(hodnota,25);                              // (styl 25 zachovava prvni dva znaky na displeji a zobrazene cislo je na 1 desetinne misto)
          break;


        case '6':                                                // nastaveni prumeru dalekohledu
          if (hodnota - 1 >= minimum )  hodnota --;              // bezne odecteni hodnoty s kontrolou na minimum
          zobraz_cislo(DSO_pole_prumery_dalekohledu[hodnota],1);             // (styl 1 zachovava prvni znak na displeji)
          break;


        case '7':                                                // nastaveni hlavniho ohniska dalekohledu
          if (hodnota - 1 >= minimum )  hodnota --;              // bezne odecteni hodnoty s kontrolou na minimum
          zobraz_cislo(DSO_pole_delky_dalekohledu[hodnota],1);               // (styl 1 zachovava prvni znak na displeji)
          break;

        case 'H':                                                // (hodina)
          hodnota --;
          if(hodnota < minimum)    hodnota = maximum;         
          if (hodnota < 24)  zobraz_cislo(hodnota,1);
          else               zobraz_text(67);                    // "H  --" pri nastaveni budiku je mozne volbou 24H budik zrusit
          break;

        case 'b':                                                // (budik)
          if (hodnota - 1 >= minimum )  hodnota --;              // bezne odecteni hodnoty s kontrolou na minimum
          if (hodnota < 6)
            {
              zobraz_ho_mi(RTC_alarm[hodnota],hodnota);          // zobrazeni alarmu ve formatu "b.20.45" (pro aktivni budik) nebo "b.---2" (pro neaktivni budik)
            }
          else
            {
              zobraz_odpocmi(RTC_alarm[hodnota]);                // zobrazeni odpoctu ve formatu "o.-457" (pro aktivni odpocet) nebo "o.----" (pro neaktivni odpocet)
            }
          break;

        case 'a':                                                // (Alarm for Darkness)
          hodnota --;
          if (hodnota < minimum )  hodnota = maximum ;           // pretoceni pocitadla pri podkroceni minima
          if (hodnota == 255)                                    // hodnota 255 (maximum) vypina funkci
            {
              zobraz_text(69);                                   // "A.d.---"  (funkce vypnuta)
            }
          else
            {
              zobraz_cislo(hodnota,21);                          // zobrazeni hodnoty ve formatu "A.d.15.4"          
            }
          break;


        case 'L':                                                // (list)
          if (hodnota + 1 <= maximum )                           // list funguje opacne (misto odecitani se pricita)
            {
              hodnota ++;
            }        
          zobraz_cislo(hodnota,2);
          break;

        case 'S':                                                // (zona)
          hodnota --;
          if (hodnota < minimum) hodnota = minimum;              // minimum nelze podlezt
          zobraz_text(hodnota + 57);
          break;

        case 'T':                                                // (trasovani)
          hodnota --;
          if (hodnota < minimum) hodnota = minimum;              // minimum nelze podlezt
          zobraz_text(hodnota + 96);
          break;

        
        case 'R':                                                // (rektascenze)
          hodnota --;
          if (hodnota < minimum) hodnota = maximum;              // pri podlezeni pod minimum se nastavi maximum
          zobraz_cislo(hodnota,17);
          break;

        case 'D':                                                // (deklinace)
          hodnota --;
          if (hodnota < minimum) hodnota = maximum;              // pri podlezeni pod minimum se nastavi maximum
          zobraz_cislo(hodnota,18);
          break;

        case 'X':                                                // (interval R30s)
          hodnota --;
          if (hodnota < minimum) hodnota = minimum;              // pri podlezeni pod minimum se nastavi minimum
          zobraz_cislo(hodnota,19);
          break;

        case 'p':                                                // (planety)
          hodnota --;
          if (hodnota < minimum) hodnota = minimum;              // pri podlezeni pod minimum se nastavi minimum
          zobraz_text(hodnota + 79);
          break;



        case '#':                                                // (hlavni menu) - pohyb k vyssim indexum polozek (tlacitko dolu)
          hodnota ++;
          while (pole_zobr_menu[hodnota] == false)               // kdyz je zvolena polozka neaktivni, prepne se na dalsi vyssi
            {
              hodnota ++;
              if (hodnota > maximum)
                {
                  hodnota = maximum;
                  break;                                         // pri preteceni maximalniho poctu polozek se ze smycky while vypadne
                }
            }
          while (pole_zobr_menu[hodnota] == false)               // pokud se z predchozi smycky vystoupi pres prekroceni horni hranice poctu polozek
            {
              hodnota --;                                        // postupnym snizovanim indexu se hleda nejvyssi povolena polozka
            }                    
          
          zobraz_text(hodnota);                                  // polozky menu jsou seskladany na indexech  [0] az [27]
          break;                                                 // tenhle brak ukoncuje case '@'

        case '@':                                                // (editace viditelnych polozek menu) - pohyb k nizsim indexum polozek (tlacitko nahoru)
          hodnota ++;
          while (spec_rezim == 0 and hodnota > 14 and hodnota < 22)  hodnota ++;     // polozky menu pro specialni funkce se bez zasunuteho HW vubec nezobrazuji
          if (hodnota >= maximum)        hodnota = maximum;      // zastaveni na posledni polozce ("DSO  ")
                   
          if (menu_enable[hodnota] == true) jas_displeje = 7;    // povolene polozky menu se zobrazi vysokym jasem
          else                              jas_displeje = 1;    // zakazane polozky menu se zobrazi nizkym jasem

          zobraz_text(hodnota);                                  // polozky menu jsou seskladany na indexech  [0] az [28]
          break;                                                 // tenhle break ukoncuje case '@'


          
        case '&':                                                // (zapinani / vypinani periferii) - pohyb k nizsim indexum (tlacitko nahoru)
          hodnota ++;
          if (hodnota >= maximum)        hodnota = maximum;      // zastaveni na posledni periferii (" r30S")
                   
          if (perif_enable[hodnota] == true) jas_displeje = 7;   // zapnute periferie se zobrazi vysokym jasem
          else                               jas_displeje = 1;   // vypnute periferie se zobrazi nizkym jasem

          zobraz_text(hodnota + 73);                             // periferie jsou seskladany na indexech  [73] az [77]
          break;                                                 // tenhle break ukoncuje case '&'



      
      }

    return hodnota;
  }
//==============================================================================================



//------------------------------------------
// podprogram pro postupne odrotovani zaznamu stopek na displeji
// vraci true pri predcasnem ukonceni rolovani pomoci tlacitka OK
//   nebo false po dobehnuti rolovani do konce
bool disp_zaznam_scroll(uint8_t pocet_znaku)
  {
    pom_disp_scroll[0] = 0;                                                  // prazdna sedmisegmentovka
    pom_disp_scroll[1] = 0;                                                  // prazdna sedmisegmentovka
    pom_disp_scroll[2] = 0;                                                  // prazdna sedmisegmentovka
    pom_disp_scroll[3] = 0;                                                  // prazdna sedmisegmentovka

    for (uint8_t i = 0; i< pocet_znaku - 4 ; i++)
      {
        D_pamet[4] =  pom_disp_scroll[i];
        D_pamet[3] =  pom_disp_scroll[i+1];
        D_pamet[2] =  pom_disp_scroll[i+2];
        D_pamet[1] =  pom_disp_scroll[i+3];
        D_pamet[0] =  pom_disp_scroll[i+4];
        aktualizuj_displej();

        for (uint8_t pauza = 0 ; pauza < 50 ; pauza++)
          {
            delay(10);
            if (i > 2 and digitalRead(pin_tl_ok) == LOW)
              {
                return true;
              }

            if (i > 2 and (digitalRead(pin_tl_dn) == LOW or digitalRead(pin_tl_up) == LOW) )
              {
                return false;
              }
          }
      }
    return false;
  }


void menu_stanoviste(void)
  {
    uint32_t trvani_stisku_stan = 0;
    uint16_t timeout_stan = 600;                                         // timeout se nastavi na  30 sekund (600 x 0,05 sek)

    while (digitalRead(pin_tl_ok) == LOW)                                    // cekani na uvolneni tlacitka OK (displej je od minule zhasnuty)
      {
        delay(50);
      }
    delay(50);                                                               // odruseni zakmitu pri uvolnovani

    int16_t index_pozst = ((EEPROM_read(eeaddr_TS_stanoviste) & 0b11100000) >> 5);   // osekat nejvyssich 5 bitu (aktualne nastavene preddefinovane pozorovaci stanoviste)
    uint8_t pomprom = index_pozst;                                              // do pomocne promenne se ulozi index stanoviste pri vstupu do podmenu
    for (uint8_t j = 0; j < 5 ; j++)                                            // zobrazeni aktualne nastaveneho stanoviste
      {
        D_pamet[4-j] = EEPROM_read(j + eeaddr_stanoviste + 10 + ((index_pozst-1) * 15));
      }
    aktualizuj_displej();

    while (digitalRead(pin_tl_ok) == HIGH and timeout_stan > 0)              // nez dojde k potvrzeni stanoviste tlacitkem OK nebo nez vyprsi timeout, visi program v teto smycce
      {
        timeout_stan --;
        interrupty();                                                        // odskoceni na test zadosti o komunikaci, nebo automaticke mereni
        
        if (digitalRead(pin_tl_up) == LOW and index_pozst < 6 )
          {
            timeout_stan = 600;                                              // obcerstveni timeoutu
            index_pozst ++;
          }

        if (digitalRead(pin_tl_dn) == LOW and index_pozst >= 0 )
          {
            timeout_stan = 600;                                              // obcerstveni timeoutu
            index_pozst --;            
          }
        
        if (index_pozst > 5) index_pozst = 5;                                // zarazka na krajnich indexech 0 az 5
        if (index_pozst < 0) index_pozst = 0;

        if (index_pozst == 0)                                                // index 0 znamena zrusene stanoviste a zobrazi se pomlcky
          {
            zobraz_text(68);                                                 // "-----"
          }
        else                                                                 // pro vyssi indexy se na displeji zobrazi 5-znakove popisky aktualne zvoleneho stanoviste
          {
            for (uint8_t j = 0; j < 5 ; j++)
              {
                D_pamet[4-j] = EEPROM_read(j + eeaddr_stanoviste + 10 + ((index_pozst-1) * 15));
              }
            aktualizuj_displej();
          }


        while (digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW)     // cekani na uvolneni obou listovacich tlacitek
          {
            delay(50);
          }
        delay(50);                                                           // hlavni casovani pro timeout (600 x 0,05 sek = 30 sekund) 

      }

    if (digitalRead(pin_tl_ok) == LOW)                                       // ukonceni podmenu bylo zpusobeno stiskem tlacitka OK
      {
        delay(50);                                                           // odruseni zakmitu
        uint32_t startTime = millis();
        while (digitalRead(pin_tl_ok) == LOW)                                // dokud je tlacitko stisknute, nic se nedeje
          {
            delay(20);
            trvani_stisku_stan = millis() - startTime;
            if (trvani_stisku_stan > 500)                                    // stisk trval dele nez 0,5 sekundy .... potvrzeni nastaveneho stanoviste
              {
                zobraz_text(38);                                             // napis "Save" se zobrazi vzdycky
                if (index_pozst != pomprom)                                  // ke skutecnemu zapisu ale dochazi jen v pripade, pokud se index pozorovaciho stanoviste zmenil
                  {
                    uint8_t data48 = (EEPROM_read(eeaddr_TS_stanoviste) & 0b00011111);
                    data48 = (data48 | (index_pozst << 5) );
                    EEPROM_update(eeaddr_TS_stanoviste,data48);
                    delay(500);
                  }
                
              }

            if (digitalRead(pin_tl_ok) == HIGH) delay(50);                   // kdyz se behem drzeni tlacitka OK objevi kratky vypadek kontaktu, tak se odignoruje

          }
        if (trvani_stisku_stan <= 500)                                       // stisk tlacitka OK byl kratsi nez 0,5 sekundy
          {
            zobraz_text(66);                                                 // nic se neuklada, jen se smaze displej a vrati se do hlavniho menu
            delay(100);                                                      // pauza pred opustenim podmenu
          }
        
      }
    
    if (timeout_stan == 0) auto_exit_menu = true;                            // kdyz vyprsi timeout v tomto podmenu, zrusi se zaroven timeout i v hlavnim menu, takze se provede navrat do hlavni vyckavaci smycky
  
  }
//----------------------------------------------



//------------------------------------------
// moznost volby viditelnych polozek v menu
void editace_polozek_menu(void)
  {
    int16_t polozka = 0;
    uint32_t menubity;
    zobraz_text(54);                                                         // na displeji se zobrazi napis "MEnu "

    if (pouzit_r30s == true)                                                 // jen kdyz je R30s povolena periferie
      {
        spec_rezim = test_zasunuti();                                        // test zasunuti pridavneho HW //!! (r30s)
      }
    else
      {
        spec_rezim = 0;
      }
    while (digitalRead(pin_tl_up) == LOW or digitalRead(pin_TS) == LOW)      // do teto funkce se vstoupilo stiskem tlacitka UP a bocniho tlacitka pri zapnuti napajeni
      {
        delay(100);                                                          // musi se pockat, dokud se obe tlacitka neuvolni (vcetne odruseni zakmitu)
      }
    delay(100);
  


    if (menu_enable[polozka] == true) jas_displeje = 7;                      // pokud je prvni (nulta) polozka ("List") povolena, zobrazi se vysokym jasem
    else                              jas_displeje = 1;                      // pokud je prvni (nulta) polozka ("List") zakazana, zobrazi se nizkym jasem
    zobraz_text(polozka);                                                    // na displeji se zobrazi prvni (nulta) polozka "List "

    while (true)                                                             // z tohoto podprogramu se vypadne dlouhym stiskem OK
      {       
        polozka = plusminus('@', polozka, 0, pocet_polozek_v_menu);          // '@' = editace viditelnosti polozek v menu
        
        if (bude_save == true)                                               // dlouhy stisk = ukonceni editace viditelnych polozek
          {
            jas_displeje = 3;                                                // stredni jas displeje
            break;                                                           // vypadnuti ze smycky while(true)
          }
        
        if (auto_exit_menu == true) break;                                   // v predchozich smyckach doslo k vyprseni casu pro autoexit

      }
    
    
    while (digitalRead(pin_tl_ok) == LOW)   delay(100);                      // cekani na uvolneni tlacitka OK s odrusenim zakmitu
    delay(100);

    if (auto_exit_menu == false)                                             // k zapisu upravenych polozek dojde jen pro korektnim odklepnuti dlouhym stiskem OK
      {
        menubity = 0;                                                        // ulozeni nastavenych hodnot do EEPROM (4 bajty po 8 bitech - kazdy bit jedna polozka)
        for (polozka = 0; polozka <= pocet_polozek_v_menu ; polozka ++)
          {
            if (menu_enable[polozka] == true)   bitSet(menubity,polozka);    // priprava 32-bitove promenne
          }
          
        EEPROM_write_long(eeaddr_menu_bity , menubity);                      // zapis promenne do EEPROM
      }
  }
//----------------------------------------------

//------------------------------------------
// moznost vypinani poskozenych periferii v terenu
void editace_periferii(void)
  {
    int16_t polozka = 0;
    uint16_t peribity;
    zobraz_text(72);                                                         // na displeji se zobrazi napis "PEriF"
    
    while (digitalRead(pin_tl_ok) == LOW or digitalRead(pin_TS) == LOW)      // do teto funkce se vstoupilo stiskem tlacitka OK a bocniho tlacitka pri zapnuti napajeni
      {
        delay(100);                                                          // musi se pockat, dokud se obe tlacitka neuvolni (vcetne odruseni zakmitu)
      }
    delay(100);
  


    if (perif_enable[polozka] == true) jas_displeje = 7;                     // pokud je prvni (nulta) polozka ("  bME") povolena, zobrazi se vysokym jasem
    else                               jas_displeje = 1;                     // pokud je prvni (nulta) polozka ("  bME") zakazana, zobrazi se nizkym jasem
    zobraz_text(polozka + 73);                                               // na displeji se zobrazi prvni (nulta) polozka "  bME"

    while (true)                                                             // z tohoto podprogramu se vypadne dlouhym stiskem OK
      {       
        polozka = plusminus('&', polozka, 0, pocet_periferii);               // '&' = editace zapnutych a vypnutych periferii
        
        if (bude_save == true)                                               // dlouhy stisk = ukonceni editace zapnutych a vypnutych periferii
          {
            jas_displeje = 3;                                                // stredni jas displeje
            break;                                                           // vypadnuti ze smycky while(true)
          }
        
        if (auto_exit_menu == true) break;                                   // v predchozich smyckach doslo k vyprseni casu pro autoexit

      }
    
    
    while (digitalRead(pin_tl_ok) == LOW)   delay(100);                      // cekani na uvolneni tlacitka OK s odrusenim zakmitu
    delay(100);

    if (auto_exit_menu == false)                                             // k zapisu upravenych polozek dojde jen pro korektnim odklepnuti dlouhym stiskem OK
      {
        peribity = 0;                                                        // ulozeni nastavenych hodnot do EEPROM (2 bajt =  16 bitu,  kazdy bit jedna periferie)
        for (polozka = 0; polozka <= pocet_periferii ; polozka ++)
          {
            if (perif_enable[polozka] == true)   bitSet(peribity,polozka);   // priprava 8-bitove promenne
          }
          
        EEPROM_write_int(eeaddr_perif_bity , peribity);                      // zapis promenne do EEPROM

        SOFT_RESET();
      }
    
  }
