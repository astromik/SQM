// Podprogramy pro praci s vnitrnim RTC
//================================================


//----------------------------------------------
void RTC_info(void)
  {
    Serial.println(lng174);                                                             //  Serial.println("Informace o RTC");
    Serial.println("---------------");

    Serial.print(lng175);                                                               //  Serial.print("Aktualne je ");
    if(leto == true)     Serial.println(char_leto);                                     //  Serial.println("LEto");
    else                 Serial.println(char_zima);                                     //  Serial.println("2iMA");

    Serial.print(lng176);                                                               //  Serial.print ("Cas v RTC bez korekci: ");
    uint32_t tt = rtclock.getY2kEpoch();

    sek1970_datcas(tt,true);

    Serial.println(lng177);                                                             //  Serial.println("Cas posledniho serizeni RTC (data z EEPROM): ");
    uint32_t pomprom = EEPROM_read_long(eeaddr_RTC_set);
    Serial.print(lng178);                                                               //  Serial.print("... v sekundach od 1.1.1970: ");
    Serial.println(pomprom);
    Serial.print(lng179);                                                               //  Serial.print("... v citelnem tvaru: ");

    sek1970_datcas(pomprom,true);
    
    
    uint32_t interval;
    if (tt > cas_minuleho_nastaveni)    interval = tt - cas_minuleho_nastaveni;         // interval musi byt vzdycky kladny
    else                                interval = cas_minuleho_nastaveni - tt;    

    Serial.print(lng180);                                                               //  Serial.print("Interval od posledniho nastaveni casu: ");
    Serial.print(interval);
    Serial.print(" [s] (= ");    
    Serial.print(interval / 86400.0);
    Serial.println(" [d])");    
    
    Serial.println(lng181);                                                             //  Serial.println("Korekce casu (data z EEPROM): ");
    pomprom = EEPROM_read_long(eeaddr_RTC_korekce);
    Serial.print(lng182);                                                               //  Serial.print(" ... ulozene cislo v HEX formatu : ");
    Serial.println(pomprom,HEX);

    Serial.print(lng183);                                                               //  Serial.print(" ... sekunda se ");

    
    if (pomprom > 0x7FFFFFFFUL)   Serial.print(lng184);                                 //  Serial.print ("odecita");
    else                          Serial.print(lng185);                                 //  Serial.print ("pricita");
    
    Serial.print(lng186);                                                               //  Serial.print(" kazdych ");

    uint32_t za_kolik_sek = pomprom & 0x7FFFFFFFUL;
    Serial.print(za_kolik_sek);
    Serial.println(lng187);                                                             //  Serial.println(" sekund od posledniho serizeni.");
    Serial.print(lng188);                                                               //  Serial.print("           (to je asi ");  
    
    if(za_kolik_sek < 180000UL )                                                        // mene nez 50 hodin
      {
        Serial.print(za_kolik_sek / 3600.0 ,1);
        Serial.println(lng189);                                                         //  Serial.println(" hodin)");
      }

    if(za_kolik_sek >= 180000UL and za_kolik_sek < 31557600UL)                          // 50 hodin az 1 rok 
      {
        Serial.print(za_kolik_sek / 86400.0 ,1);
        Serial.println(lng190);                                                         //  Serial.println(" dni)");              
      }
     if(za_kolik_sek >= 31557600UL)                                                     // vic nez rok 
      {
        Serial.print(za_kolik_sek / 31557600.0 ,1);
        Serial.println(lng191);                                                         //  Serial.println(" roku)");              
      }

    uint32_t extra_sekundy = interval / (0x7FFFFFFFUL & pomprom);
    Serial.print(lng192);                                                               //  Serial.print("Zatim se takto melo pridat/ubrat: ");
    if (pomprom > 0x7FFFFFFFUL)   Serial.print('-');                                    // zaporna korekce = sekundy se odecitaji
    else                          Serial.print('+');                                    // kladna korekce = sekundy se pricitaji
 
    
    Serial.print(extra_sekundy);
    Serial.println(lng193);                                                             //  Serial.println(" extra sekund.");
    
    Serial.print(lng194);                                                               //  Serial.print("Takze zkorigovany cas pro zobrazeni je: ");
    zobraz_RTC(true,true);

    if (pouzit_ds3231 == true)
      {
        Serial.print(lng314);                                          // "Cas v DS3231: "
        cti_ds3231();                                                  // nacteni casovych registru DS3231 a jejich prevod na globalni promenne 'ds_xxx'
        formatuj_ds_cas();    
      }    
  }
//----------------------------------------------
    


//----------------------------------------------
//  Zobrazeni casu v RTC obvodu vcetne korekce
void zobraz_RTC(bool serkom, bool secselc)
  {
    uint32_t tt = rtc_korekce();                                                   // aktualni cas v RTC v sekundach od 1.1.1970 (UTC)
    sek1970_datcas(tt,serkom,secselc);                                                  // v promenne tt je UTC cas s korekci
  }


void zobraz_RTC(bool serkom)
  {
    zobraz_RTC(serkom, false);
  }
//----------------------------------------------



//----------------------------------------------
//  Nastaveni casu v RTC obvodu
//  Po prvnim znaku 'T', ktery zpusobil skok do tohoto podprogramu, ocekava na seriove lince retezec ve tvaru "yyyymmddhhnnss"
void nastav_RTC(void)
  {
    bool chyba = false;                                                                 // bude provadeno testovani na korektni meze vlozenych cisel. Kdyz dojde k prekroceni mezi, nastavi se znacka 'chyba' na true
    long odchylka ;                                                                     // o kolik se lisi zadavany cas a aktualni cas v RTC
    uint32_t interval;                                                             // rozdil casu v sekundach od minuleho nastavovani casu
    uint32_t tt;
    uint32_t aktualni_cas_RTC;                                                     // aktualni cas v RTC v sekundach od 1.1.1970
  
    delay(100);                                                                         // pauza na prijeti vsech znaku ze seriove linky
    if (Serial.available() == 14)                                                       // pro nastaveni casu se ocekava 14 znaku: "YYYYMMDDHHNNSS"
      {
    
        i = 0;
        while (Serial.available() > 0)                                                  // vsechny prijate znaky se ulozi do pole
          {
            prijato[i] = Serial.read();
            i++;
          }

        Serial.print(lng195);                                                           //  Serial.print("Prijato: ");
        Serial.println(prijato);


      
        // rok
        LOC_rok = prijato_int(0, 4);
        if (LOC_rok > 2099 || LOC_rok < 2020) chyba = true;                             // test, ze je rok v rozsahu 2020 az 2099
    
    
        // mesic
        LOC_mes = prijato_int(4, 6);
        if (LOC_mes > 12 || LOC_mes < 1)      chyba = true;                             // test, ze je mesic v rozsahu 1 az 12
    
    
        // den
        LOC_den = prijato_int(6, 8);
        if (LOC_den > 31 || LOC_den < 1)      chyba = true;                             // test, ze je den v rozsahu 1 az 31 (nebere se ohled na to, kolik ma mesic ve skutecnosti dni)
    
        // hodina
        LOC_hod = prijato_int(8, 10);
        if (LOC_hod > 23 || LOC_hod < 0)      chyba = true;                             // test, ze jsou hodiny v rozsahu 0 az 23
    
        // minuta
        LOC_min = prijato_int(10, 12);
        if (LOC_min > 59 || LOC_min < 0)      chyba = true;                             // test, ze jsou minuty v rozsahu 0 az 59
    
    
        // sekunda
        LOC_sek = prijato_int(12, 14);
        if (LOC_sek > 59 || LOC_sek < 0)      chyba = true;                             // test, ze jsou sekundy v rozsahu 0 az 59
        if (chyba == true)                                                              // kdyz je nejaka hodnota mimo povoleny rozsah, ale prisel spravny pocet znaku ...
          {
            Serial.println(lng006);                                                     //  Serial.println ("Vstup mimo rozsah ");          // zahlasi se chyba v zadani
            Serial.print(lng197);                                                       //  Serial.print("Rok: ");                          //  a vypisou se zadane udaje
            Serial.println(LOC_rok);
            Serial.print(lng198);                                                       //  Serial.print("Mesic: ");
            Serial.println(LOC_mes);
            Serial.print(lng199);                                                       //  Serial.print("Den: ");
            Serial.println(LOC_den);
            Serial.print(lng200);                                                       //  Serial.print("Hodina: ");
            Serial.println(LOC_hod);
            Serial.print(lng201);                                                       //  Serial.print("Minuta: ");
            Serial.println(LOC_min);
            Serial.print(lng202);                                                       //  Serial.print("Sekunda: ");
            Serial.println(LOC_sek);
          }
      }
    else
      {
        chyba = true;                                                                   // kdyz po seriove lince dorazil jiny pocet znaku, nez 15, tak se jen vypise chyba
        Serial.println(lng006);                                                         //  Serial.println ("Vstup mimo rozsah ");          // zahlasi se chyba v zadani
      }

    
    if (chyba == false)                                                                 // kdyz se zdaji byt vsechny hodnoty v poradku
      {

        // pri kazdem spravnem zadani se vypocita korekce na zaklade rozdilu casu od minuleho serizeni 
        aktualni_cas_RTC = rtclock.getY2kEpoch();

        mtt.year     = LOC_rok-1970;
        mtt.month    = LOC_mes;
        mtt.day      = LOC_den;
        mtt.hour     = LOC_hod;
        mtt.minute   = LOC_min;
        mtt.second   = LOC_sek;       

        if (leto == false) tt = makeTimeFCE(mtt) - (60*60*zimni_posun);            // v zime se pri ukladani casu uklada do RTC o hodinu mene (prevod SEC na UTC)
        else               tt = makeTimeFCE(mtt) - (60*60*letni_posun);            // kdyz se nastavuje cas v lete (SELC), do RTC se uklada o dalsi hodinu mene (prevod SELC na SEC)
        
        rtclock.setY2kEpoch(tt);                                                   // skutecne prenastaveni casu v RTC na zadanou hodnotu
        RTC->BKP0R = (uint8_t)0xAA;                                                // znacka ze byl cas nastaveny. Po vypadku 3V zalozni baterie se znacka automaticky maze 
        bitClear(err_bit,3);                                                       // RTC je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'

               
        cas_minuleho_nastaveni = EEPROM_read_long(eeaddr_RTC_set);

        if (tt > cas_minuleho_nastaveni)    interval = tt - cas_minuleho_nastaveni;     // interval musi byt vzdycky kladny
        else                                interval = cas_minuleho_nastaveni - tt;

        odchylka = aktualni_cas_RTC - tt;

        if (interval < 86400UL)                                                         // od predchoziho serizeni ubehlo min nez 24 hodin
          {
            Serial.println(lng220);                                                     // ("Kratky interval od posledniho serizeni (< 1 den)")
            if (odchylka > 172800L or odchylka < -172800L )                             // rozdil mezi zadavanym casem a casem v RTC je vic, nez 2 dny. To je CHYBA
              {
                Serial.println(lng219);                                                 // ("Velka odchylka mezi novym casem a casem v RTC")
                Serial.println(lng205);                                                 // ("Automaticka kalibrace RTC zrusena.");
                korekce = 0x7FFFFFFFUL;
              }
            else                                                                        // "prijatelna odchylka, korekce se ale nepocita - prebira se puvodni"
              {
                korekce = EEPROM_read_long(eeaddr_RTC_korekce);
              }
          
          }
        
        if (interval > 15768000UL)                                                      // od predchoziho serizeni ubehlo vic nez pul roku
          {
            Serial.println(lng221);                                                     // Serial.println("Dlouhy interval od posledniho serizeni (> pul roku)");
            Serial.println(lng205);                                                     // Serial.println("Automaticka kalibrace RTC zrusena.");
            korekce = 0x7FFFFFFFUL;                                                     // korekce se rusi
          }
    
        if (interval <= 15768000UL and interval >=86400UL)                              // od predchoziho serizeni ubehlo vic nez den, ale min nez pul roku
          {                                                                             //  "spravny interval od posledniho serizeni"
            if (odchylka > 172800L or odchylka < -172800L )                             // rozdil mezi zadavanym casem a casem v RTC je vic, nez 2 dny. To je CHYBA
              {
                Serial.println(lng219);                                                 // ("Velka odchylka mezi novym casem a casem v RTC")
                Serial.println(lng205);                                                 // ("Automaticka kalibrace RTC zrusena.");
                korekce = 0x7FFFFFFFUL;
              }
            else                                                                        // "prijatelna odchylka, pocita se korekce"
              {
                
                if (odchylka  < 0) korekce = interval / -odchylka;                      // odchylka je zaporna, sekundy se budou pricitat -  (odchylka se prevadi na kladne cislo)
                if (odchylka  > 0) korekce = (0x80000000UL | (interval / odchylka));    // nejvyssi bit je znamenko, korekce se bude odecitat
                if (odchylka == 0) korekce = 0x7FFFFFFFUL;                              // cas v RTC je stejny, jako zadany cas - zadna korekce neni potreba (prvni extra sekunda za 68 let) 
    
                if ((korekce & 0x7FFFFFFFUL) < 300)                                     // sekunda se ma pricitat, nebo odecitat driv, nez za 5 minut - to vypada na chybu RTC
                  {
                    Serial.println(lng204);                                             // "Chyba RTC (sekunda se ma korigovat casteji, nez za 5 minut)."
                    Serial.println(lng205);                                             // "Automaticka kalibrace RTC zrusena."
                    korekce = 0x7FFFFFFFUL;
                  }
              
              }      
          }        

        cas_minuleho_nastaveni = tt;                                                    // nastaveni globalni promenne, aby se nemuselo pri kazdem zjistovani casu sahat do EEPROM
        EEPROM_write_long(eeaddr_RTC_set,tt);                                                        // do EEPROM se v kazdem pripade ulozi cas posledniho nastaveni
    
    
        if (korekce != EEPROM_read_long(eeaddr_RTC_korekce))                            // do EEPROM se nova korekce uklada jen v pripade, ze se zmenila
          {
            EEPROM_write_long(eeaddr_RTC_korekce , korekce);
          }

      }
  }
//----------------------------------------------



//----------------------------------------------
// Prevod poctu sekund od 1.1.1970 na citelny datum a cas.
//  Vystup datumu a casu se muze smerovat do seriove linky a zaroven nastavuje globalni promenne LOC_xxx
void sek1970_datcas(uint32_t parametr, bool seriovy_vystup)                       // pretizeni funkce (kdyz chybi parametr 'secselc')
  {    
    sek1970_datcas(parametr, seriovy_vystup , false);
  }



void sek1970_datcas(uint32_t parametr, bool seriovy_vystup, bool secselc)
  {    
    breakTimeFCE(parametr, mtt);
    LOC_dvt = mtt.weekday + 1;                                                          // primo v RTC se den v tydnu uklada v rozsahu 0(Po) az 6(Ne), proto se pro spravne zobrazeni pricita +1 (1=Po....7=Ne)
    LOC_den = mtt.day;
    LOC_mes = mtt.month;
    LOC_rok = mtt.year+1970;
    LOC_hod = mtt.hour;
    LOC_min = mtt.minute;
    LOC_sek = mtt.second;

    if (seriovy_vystup == true)                                                         // vypis zadanych hodnot v citelnem formatu:  d.m.yyyy h:nn:ss DVT SELC
      {
        Serial.print(LOC_den);
        Serial.print('.');    
        Serial.print(LOC_mes);
        Serial.print('.');
        Serial.print(LOC_rok);
        Serial.print(' ');    
        Serial.print(LOC_hod);
        Serial.print(':');    
        if (LOC_min < 10)     Serial.print('0');
        Serial.print(LOC_min);
        Serial.print(':');    
        if (LOC_sek < 10)     Serial.print('0');
        Serial.print(LOC_sek);
        Serial.print(' ');    
        Serial.print(dny[LOC_dvt]);
        Serial.print(' ');    
        if (secselc == true)
          {
            if (leto == true) Serial.print(char_leto);
            else              Serial.print(char_zima);        

            Serial.print(" (UTC ");
            uint16_t pomprom = EEPROM_read(eeaddr_leto_zima);

            if (pomprom == 0)
              {
                if (zimni_posun >= 0)  Serial.print('+');
                Serial.print(zimni_posun);
              }
            else
              {
                if (letni_posun >= 0)  Serial.print('+');
                Serial.print(letni_posun);
              }
            Serial.println(')');
          
          }
        else
          {
            Serial.print("\r\n");            
          }
      }
  }
//----------------------------------------------



//----------------------------------------------
// Zjisti aktualni cas v RTC (v UTC), pripadne k nemu prida/ubere nejake sekundy z automaticke korekce.
// Kdyz je aktualne SEC prida hodinu, kdyz je SELC, prida 2 hodiny
// vysledek je zkorigovany pocet sekund od 1.1.1970
uint32_t rtc_korekce(void)
  {
    uint32_t rtc_tt = rtclock.getY2kEpoch();                        // zjisteni aktualniho casu v RTC (v UTC)
    uint32_t interval;
    if (rtc_tt > cas_minuleho_nastaveni) interval = rtc_tt - cas_minuleho_nastaveni;    // interval musi byt vzdycky kladny
    else                                 interval = cas_minuleho_nastaveni - rtc_tt;        
       
    uint32_t extra_sekundy = interval / (0x7FFFFFFFUL & korekce);                  // vypocet, kolik sekund ma pricist, nebo odecist od casu v RTC

    if (korekce > 0x7FFFFFFFUL) rtc_tt = rtc_tt - extra_sekundy;
    else                        rtc_tt = rtc_tt + extra_sekundy;
    

    if (leto == false) rtc_tt = rtc_tt + (60*60*zimni_posun);                           // v zime se ke UTC pridava hodina
    else               rtc_tt = rtc_tt + (60*60*letni_posun);                           // v lete se pridava jeste hodina navic
    
    return rtc_tt;
  }
//----------------------------------------------



//----------------------------------------------
//Zobrazi na displeji aktualni hodiny a minuty ve formatu " H-MM" nebo "HH-MM"
void ho_mi(void)
  {
    zobraz_RTC(false);

    if(LOC_hod < 10) D_pamet[4] = 0;                                                    // na pozici desitek hodin zapis mezeru
    else             D_pamet[4] = graf_def[LOC_hod / 10];                               //  nebo desitky hodin
    D_pamet[3] = graf_def[LOC_hod % 10];                                                // na pozici jednotek hodin zapis jednotky hodin
    D_pamet[2] = 64;                                                                    // mezi hodiny a minuty se zapise pomlcka

    D_pamet[1] = graf_def[LOC_min / 10];                                                // desitky minut
    D_pamet[0] = graf_def[LOC_min % 10];                                                // jednotky minut

    aktualizuj_displej();    
  }


//----------------------------------------------
//Zobrazi na displeji aktualni minuty a sekundy ve formatu "MM-SS"
void mi_se(void)
  {
    zobraz_RTC(false);

    D_pamet[4] = graf_def[LOC_min / 10];                                                // desitky minut
    D_pamet[3] = graf_def[LOC_min % 10];                                                // na pozici jednotek minut zapis jednotky minut
    D_pamet[2] = 64;                                                                    // mezi minuty a sekundy se zapise pomlcka

    D_pamet[1] = graf_def[LOC_sek / 10];                                                // desitky sekund
    D_pamet[0] = graf_def[LOC_sek % 10];                                                // jednotky sekund

    aktualizuj_displej();    
  }





//-----------------------------------------------------------------------------
//  podprogramy prevzate z puvodni knihovny RTClock.cpp pro STM32F103
//      (novou knihovnu pro STM32F4x1 se mi nepodarilo nainstalovat)
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Prevede cas v sekundach od 1.1.1970 do strukturovane casove promenne
void breakTimeFCE(time_t timeInput, tm_t & tmm)
  {
  // break the given time_t into time components
  // this is a more compact version of the C library localtime function
  // note that year is offset from 1970 !!!
  
    uint8_t year;
    uint8_t month, monthLength;
    uint32_t time;
    uint32_t days;
  
    time = (uint32_t)timeInput;
    tmm.second = time % 60;
    time /= 60; // now it is minutes
    tmm.minute = time % 60;
    time /= 60; // now it is hours
    tmm.hour = time % 24;
    time /= 24; // now it is days
    tmm.weekday = ((time + 3) % 7); // Monday is day 1
  
    year = 0;
    days = 0;
    while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
      year++;
    }
    tmm.year = year; // year is offset from 1970 
  
    days -= LEAP_YEAR(year) ? 366 : 365;
    time -= days; // now it is days in this year, starting at 0
  
    days = 0;
    month = 0;
    monthLength = 0;
    for (month=0; month<12; month++) {
      if (month==1) { // february
        if (LEAP_YEAR(year)) {
          monthLength=29;
        } else {
          monthLength=28;
        }
      } else {
        monthLength = monthDays[month];
      }
  
      if (time >= monthLength) {
        time -= monthLength;
      } else {
        break;
      }
    }
    tmm.month = month + 1;  // jan is month 1  
    tmm.day = time + 1;     // day of month
  }
//-----------------------------------------------------------------------------








//-----------------------------------------------------------------------------
// Ze strukturovane promenne, ktera obsahuje datum a cas, vypocte pocet sekund od 1.1.1970
time_t makeTimeFCE(tm_t & tmm)
  {
  // assemble time elements into time_t 
  // note year argument is offset from 1970 (see macros in time.h to convert to other formats)
  // previous version used full four digit year (or digits since 2000),i.e. 2009 was 2009 or 9
    
    int16_t i;
    uint32_t seconds;
  
    // seconds from 1970 till 1 jan 00:00:00 of the given year
    seconds = tmm.year*(SECS_PER_DAY * 365);
    for (i = 0; i < tmm.year; i++) {
      if (LEAP_YEAR(i)) {
        seconds +=  SECS_PER_DAY;   // add extra days for leap years
      }
    }
  
    // add days for this year, months start from 1
    for (i = 1; i < tmm.month; i++) {
      if ( (i == 2) && LEAP_YEAR(tmm.year)) { 
        seconds += SECS_PER_DAY * 29;
      } else {
        seconds += SECS_PER_DAY * monthDays[i-1];  //monthDay array starts from 0
      }
    }
    seconds+= (tmm.day-1) * SECS_PER_DAY;
    seconds+= tmm.hour * SECS_PER_HOUR;
    seconds+= tmm.minute * SECS_PER_MIN;
    seconds+= tmm.second;
    return (time_t)seconds; 
  }

//-----------------------------------------------------------------------------


// otestuje, jestli je datum v rozsahu 1.1.2000 az 31.12.2080
bool test_datumu(uint16_t test_den , uint16_t test_mes , uint16_t test_rok)
  {
    
    bool vysledek = true;
    uint16_t  pomdny_d[] = { 0,   31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };
  
    if ((test_rok % 4) == 0)     pomdny_d[2] = 29;             // pri prestupnem roku se nastavuje unor na 29 dni
       
    if (test_rok < 2000 or test_rok > 2080)  vysledek = false;
    if (vysledek == true and (test_mes <    1 or test_mes >   12))  vysledek = false;
    if (vysledek == true and  test_den <    1)                      vysledek = false;    
    if (vysledek == true and  test_den >  pomdny_d[test_mes])       vysledek = false;
    return vysledek;
  }
  
