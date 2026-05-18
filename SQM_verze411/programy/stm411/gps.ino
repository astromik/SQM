// Stazeni GPS souradnic pres I2C z pridavneho modulu s ATmega328 a s GPS modulem
//================================================

// priklad exportnich dat v registrech:
// 0 ---   8       prumer 10 poslednich LAT
// 1 ---  79
// 2 --- 192
// 3 ---  53

// 4 ---  11       prumer 10 poslednich LON
// 5 --- 149
// 6 --- 204
// 7 --- 156

// 8 --- 100       OK cnt

// 9 ---  23       YY
//10 ---  10       MM
//11 ---  25       DD
//12 ---  17       HH
//13 ---  33       NN
//14 ---  36       SS

//15 ---   3       prumer 10 poslednich ALT
//16 --- 229

//17 ---   7       Pocet satelitu
//18 ---  20       10x HDoP
//19 ---   7       pole klouzaku zaplneno, muzou se stahovat data, bit 2= int/ext


//GPS LAT: 139444277 = [N] 49.444278
//GPS LON: 194366620 = [E] 14.366621
//GPS ALT: 997 = 497 [m]
//GPS satelites: 7
//GPS HDoP: 2.00
//GPS date time (UTC): 25.10.2023 17:33:36
//GPS OK cnt: 100
//GPS status: 7  (data: OK ; Time_SET: ON ; Int.)
//NMEA:
// $GPRMC,173336.00,A,4926.65677,N,01421.99742,E,0.091,,251023,,,A*78
// $GPGGA,173337.00,4926.65683,N,01421.99756,E,1,08,1.13,495.3,M,44.3,M,,*58
//----------------------------------------------
// stazeni dat z GPS rozsirujici desky
void gps(uint8_t RTC_param)
  {

    if (digitalRead(pin_DATA_RDY) == osazeno_gps)                             // modul pripojeny (interni v HIGH, externi v LOW)
      {

        uint8_t i;
    
        for (i = 0 ; i < 30 ; i++)                                            // nejdriv smazat vsechna predchozi data
          {
            pole_GPS_I2C[i] = 0;
          }
    
        if ((EEPROM_read(eeaddr_TS_stanoviste) & 0b11100000) > 0)                                 // zrusi se prednastavene pozrovaci stanoviste (v pripade, ze je nejake nastavene - aby se porad dokola neprepisovala jedna bunka EEPROM)
          {
            EEPROM_write(eeaddr_TS_stanoviste,(EEPROM_read(eeaddr_TS_stanoviste) & 0b00011111));  // smazat nejvyssi 3 bity z adresy 48
          }

         
       
    
        Wire.beginTransmission(I2C_ADDR_GPS);                                 // tady nacitat data z I2C adresy 0x17 (modul GPS) do promennych pole_GPS_I2C[] 
        delay(12);
        Wire.write(10);                                                       // 10 = zadost o data   (50 az 70 by byly zadosti o NMEA zpravy)
        delay(12);
        Wire.endTransmission();    
        
        i = 0;
        delay(20);
        Wire.requestFrom(I2C_ADDR_GPS,30);                                    // zadost o 30 bajtu (registru)
        delay(20);
        while (Wire.available())                                              // postupne ulozeni prijatych hodnot do pole
          {
            pole_GPS_I2C[i] = Wire.read();
            i++;
          }    

//  moznost podvrhnout GPS data v pripade, ze nebude pro testy dostupny GPS signal
/*
        pole_GPS_I2C[0]  =   8;       // LAT
        pole_GPS_I2C[1]  =  79;       // LAT
        pole_GPS_I2C[2]  = 192;       // LAT
        pole_GPS_I2C[3]  =  53;       // LAT
        pole_GPS_I2C[4]  =  11;       // LON
        pole_GPS_I2C[5]  = 149;       // LON
        pole_GPS_I2C[6]  = 204;       // LON
        pole_GPS_I2C[7]  = 156;       // LON
        pole_GPS_I2C[8]  = 100;       // cnt
        pole_GPS_I2C[9]  =  23;       // yy
        pole_GPS_I2C[10] =  10;       // mm
        pole_GPS_I2C[11] =  18;       // dd
        pole_GPS_I2C[12] =  15;       // hh
        pole_GPS_I2C[13] =  45;       // nn
        pole_GPS_I2C[14] =   0;       // ss
        pole_GPS_I2C[15] =   3;       // ALT
        pole_GPS_I2C[16] = 229;       // ALT
        pole_GPS_I2C[17] =   8;       // sat
        pole_GPS_I2C[18] =  22;       // 10x HDoP
        pole_GPS_I2C[19] =   7;       // info
        pole_GPS_I2C[20]  =   8;      // aktual LAT
        pole_GPS_I2C[21]  =  79;      // aktual LAT
        pole_GPS_I2C[22]  = 192;      // aktual LAT
        pole_GPS_I2C[23]  =  53;      // aktual LAT
        pole_GPS_I2C[24]  =  11;      // aktual LON
        pole_GPS_I2C[25]  = 149;      // aktual LON
        pole_GPS_I2C[26]  = 204;      // aktual LON
        pole_GPS_I2C[27]  = 156;      // aktual LON
        pole_GPS_I2C[28] =   3;       // aktual ALT
        pole_GPS_I2C[29] = 229;       // aktual ALT
        


*/        


        bool problem = false;
        if (i < 29)                                                           // nebyl prijaty prislusny pocet bajtu - NECO JE SPATNE
          {
            zobraz_text(41);                                                  // "Err-G"
            SD_log(998 , 11);
            delay(1500);
            zobraz_text(66);                                                  // "     "
            problem = true;
          }
        else                                                                  // pres I2C dorazil ocekavany pocet bajtu
          {
            if (bitRead(pole_GPS_I2C[19],2) == true) vnitrniGPS = true;       // podle bitu 2 ve statusovem bajtu se urcuje, jestli je GPS deska interni nebo externi
            else                                     vnitrniGPS = false;
            
            if (bitRead(pole_GPS_I2C[19],1) == true and pole_GPS_I2C[9] > 20) // prisel spravny pocet bajtu, je vyzadan zapis casu do RTC a rok se zda byt v poradku
              {
                GPS_nastav_cas(RTC_param);                                    // parametr: 0 ... cas v RTC se neupravuje, casove promenne zustavaji beze zmeny - nezavisle na casu v GPS
                                                                              //           1 ... cas v RTC se neupravuje, casove promenne se docasne nastavi na presny cas v GPS
                                                                              //           2 ... cas se zapise do RTC, provede se vypocet autokorekce (pokud je dostatecny interval od posledniho serizeni)
                                                                              //           3 ... stejne jako parametr 2, akorat se seriovym vystupem pri chybe (na konci tohoto podprogramu)
              }
            else
              {
                if (RTC_param == 3)   Serial.println(lng278);                 // "Cas neni v signalu GPS dostupny" (prs I2C nedorazil spravny pocet bajtu, nebo je rok v GPS datech mensi nez 2020)
              }
    
            
            if (bitRead(pole_GPS_I2C[19],0) == true)                          // klouzak uz je pripraveny
              {
                GPS_lat = (pole_GPS_I2C[0] << 24) | (pole_GPS_I2C[1] << 16) | (pole_GPS_I2C[2] << 8) | (pole_GPS_I2C[3]); 
                GPS_lon = (pole_GPS_I2C[4] << 24) | (pole_GPS_I2C[5] << 16) | (pole_GPS_I2C[6] << 8) | (pole_GPS_I2C[7]); 
                GPS_alt = (pole_GPS_I2C[15] << 8) | (pole_GPS_I2C[16]);           

                GPS_lat_akt = (pole_GPS_I2C[20] << 24) | (pole_GPS_I2C[21] << 16) | (pole_GPS_I2C[22] << 8) | (pole_GPS_I2C[23]);  // do trackovaciho souboru se zapisuji jen aktualni hodnoty bez prumerovani
                GPS_lon_akt = (pole_GPS_I2C[24] << 24) | (pole_GPS_I2C[25] << 16) | (pole_GPS_I2C[26] << 8) | (pole_GPS_I2C[27]); 
                GPS_alt_akt = (pole_GPS_I2C[28] <<  8) | (pole_GPS_I2C[29]);           

              }
            else
              {
                problem = true;                                               // neni dostatek dat pro klouzak, nebo je signal casto prerusovany
              }
          }
    
    
        if (problem == true)                                                  // pri problemu se prenastavi hodnoty zamerne mimo rozsah, aby se ve vypisu zobrazily pomlcky
          {
            GPS_lat = 0x1FFFFFFFUL;                                           // hodnoty mimo rozsah se ve vypisu zobrazi jako pomlcky (nejvyssi 3 bity slouzi jako znacka pro prednastavene stanoviste)
            GPS_lon = 0xFFFFFFFFUL;
            GPS_alt = 0xFFFF;        

            GPS_lat_akt = 0x1FFFFFFFUL;                                       // aktualni hodnoty mimo rozsah se nezapisuji do tracku
            GPS_lon_akt = 0xFFFFFFFFUL;
            GPS_alt_akt = 0xFFFF;        
          }
      }
    else                                                                      // modul neni zasunuty (zapnuty)
      {
        GPS_lat = 0x1FFFFFFFUL;                                               // hodnoty mimo rozsah se ve vypisu zobrazi jako pomlcky
        GPS_lon = 0xFFFFFFFFUL;
        GPS_alt = 0xFFFF;

        GPS_lat_akt = 0x1FFFFFFFUL;                                           // aktualni hodnoty mimo rozsah se nezapisuji do tracku
        GPS_lon_akt = 0xFFFFFFFFUL;
        GPS_alt_akt = 0xFFFF;        


        if (RTC_param == 3)   Serial.println(lng278);                         // "Cas neni v signalu GPS dostupny"
      }
   


#ifdef ukladat_GPS

    if (posledni_i2C_prikaz != 21)
      {
        posledni_i2C_prikaz = 21;
        if (digitalRead(pin_DATA_RDY) == osazeno_gps)                         // s modulem se komunikuje jen kdyz je pripojeny (vnitrni v HIGH / externi v LOW)
          {
            Wire.beginTransmission(I2C_ADDR_GPS);                             // LED na GPS modulu po skonceni mereni obnovit na hodnoty, ktere byly pred nucenym zhasnutim
            delay(12);
            Wire.write(21);
            delay(12);
            Wire.endTransmission();
          }
      }
#endif
  }
//----------------------------------------------




//----------------------------------------------
// podle casovych udaju prijatych z modulu GPS pres I2C se nastavi cas v RTC
void GPS_nastav_cas(uint8_t RTC_param)
  {
    uint8_t UTC_rok = pole_GPS_I2C[9];
    uint8_t UTC_mes = pole_GPS_I2C[10];
    uint8_t UTC_den = pole_GPS_I2C[11];
    uint8_t UTC_hod = pole_GPS_I2C[12];
    uint8_t UTC_min = pole_GPS_I2C[13];
    uint8_t UTC_sek = pole_GPS_I2C[14];

    long odchylka ;                                                           // o kolik se lisi zadavany cas a aktualni cas v RTC
    uint32_t interval;                                                        // rozdil casu v sekundach od minuleho nastavovani casu
    uint32_t aktualni_cas_RTC;                                                // aktualni cas v RTC v sekundach od 1.1.1970
    uint16_t log_znacka = 0;                                                  // pri logovani do souboru se jeste prenasi bitova znacka s ruznymi chybami nebo informacemi o vetveni programu
    
    aktualni_cas_RTC = rtclock.getY2kEpoch();

    mtt.year     = UTC_rok + 2000 - 1970;                                     // rok je dvojciferne cislo, takze se musi pricitat +2000 a RTC pocita sekundy od 1.1.1970
    mtt.month    = UTC_mes;
    mtt.day      = UTC_den;
    mtt.hour     = UTC_hod;
    mtt.minute   = UTC_min;
    mtt.second   = UTC_sek;
    tt = makeTimeFCE(mtt);                                                    // do RTC se uklada vzdycky UTC (nezavisle na SEC nebo SELC)
    if (RTC_param == 1)
      {
        GPS_temp_time = tt  + 3600;                                           // globalni promenna pro docasne ulozeni GPS casu (v zime se ke GPS casu pricita hodina)
        if (selc == 1) GPS_temp_time = GPS_temp_time + 3600;                  //    v lete se pricita jeste jedna hodina navic
        GPS_RTC_flag = true;                                                  // pri naslednem vytvareni zaznamu v EEPROM se pouzije cas z predchozi radky
      }


    if (RTC_param == 2 or RTC_param == 3)                                     // RTC se nastavuje jen v pripade, ze se jedna o rucni spusteni mereni, nebo o specialni zadost "#TG"
      {                                                                       //       pri automatu, nebo pri infovypisech se s RTC nic nedeje
        rtclock.setY2kEpoch(tt);
        RTC->BKP0R = (uint8_t)0xAA;                                           // znacka, ze probehlo nastaveni casu. Pri pristim zapnuti by nemel pristroj hlasit chybu RTC
        bitClear(err_bit,3);                                                  // RTC je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
        STM_DS(false);                                                        // serizeni casu v DS3231 podle STM32 (false = bez 30-sekundoveho testu)
        
        cas_minuleho_nastaveni = EEPROM_read_long(eeaddr_RTC_set);
        interval = tt - cas_minuleho_nastaveni;
        odchylka = aktualni_cas_RTC - tt;
    
    
        if (interval < 86400UL)                                               // od predchoziho serizeni ubehlo min nez 24 hodin
          {
            log_znacka = 1;                                                   // "moc kratky interval mezi serizenimi"
            if (odchylka > 172800L or odchylka < -172800L )                   // rozdil mezi zadavanym casem a casem v RTC je vic, nez 2 dny. To je CHYBA
              {
                log_znacka = log_znacka + 8;                                  // "vic nez 2 dny velka odchylka"
                korekce = 0x7FFFFFFFUL;
              }
            else
              {
                log_znacka = log_znacka + 16;                                 // "prijatelna odchylka, korekce se ale nepocita - prebira se puvodni"
                korekce = EEPROM_read_long(eeaddr_RTC_korekce);
              }
          
          }
        
        if (interval > 15768000UL)                                            // od predchoziho serizeni ubehlo vic nez pul roku
          {
            log_znacka = 2;                                                   // "vic nez pul roku od posledniho serizeni"
            korekce = 0x7FFFFFFFUL;                                           // korekce se rusi
          }
    
        if (interval <= 15768000UL and interval >=86400UL)                    // od predchoziho serizeni ubehlo vic nez den, ale min nez pul roku
          {
            log_znacka = 4;                                                   // "spravny interval od posledniho serizeni"
            if (odchylka > 172800L or odchylka < -172800L )                   // rozdil mezi zadavanym casem a casem v RTC je vic, nez 2 dny. To je CHYBA
              {
                log_znacka = log_znacka + 8;                                  // "vic nez 2 dny velka odchylka"
                korekce = 0x7FFFFFFFUL;
              }
            else
              {
                log_znacka = log_znacka + 32;                                 // "prijatelna odchylka, pocita se korekce"
                
                if (odchylka  < 0) korekce = interval / -odchylka;            // odchylka je zaporna, sekundy se budou pricitat -  (odchylka se prevadi na kladne cislo)
                if (odchylka  > 0) korekce = (0x80000000UL | (interval / odchylka));  // nejvyssi bit je znamenko, korekce se bude odecitat
                if (odchylka == 0) korekce = 0x7FFFFFFFUL;                    // cas v RTC je stejny, jako zadany cas - zadna korekce neni potreba (prvni extra sekunda za 68 let) 
    
                if ((korekce & 0x7FFFFFFFUL) < 300)                           // sekunda se ma pricitat, nebo odecitat driv, nez za 5 minut - to vypada na chybu RTC
                  {
                    log_znacka = log_znacka + 64;                             // "chyba - nezvykle caste pouziti korekcni sekundy"
                    korekce = 0x7FFFFFFFUL;
                  }
              
              }      
          }
    
        cas_minuleho_nastaveni = tt;
        EEPROM_write_long(eeaddr_RTC_set,tt);                                              // do EEPROM se v kazdem pripade ulozi cas posledniho nastaveni (aktualni presny cas z GPS)
    
    
        if (korekce != EEPROM_read_long(eeaddr_RTC_korekce))
          {
            log_znacka = log_znacka + 128;                                    // "zapis zmeny korekce do EEPROM"
            EEPROM_write_long(eeaddr_RTC_korekce , korekce);
          }
        else
          {
            log_znacka = log_znacka + 256;                                    // "korekce beze zmeny - nezapisuje se do EEPROM"
          }
        if ( RTC_param == 3)                                                  // pri spusteni nastaveni casu prikazem #TG se vypise, ze byl cas nastaven
          {
            Serial.println(lng340);                                           // "Cas nastaven "
          }
        
   
        if (RTC_SD_DEBUG == true)                                             // priprava textoveho retezce s datumem a casem pro logovani na SD kartu
          {
            DATCAS_retezec[2]  = 48 + (UTC_rok / 10);
            DATCAS_retezec[3]  = 48 + (UTC_rok % 10);
            DATCAS_retezec[5]  = 48 + (UTC_mes / 10);
            DATCAS_retezec[6]  = 48 + (UTC_mes % 10);
            DATCAS_retezec[8]  = 48 + (UTC_den / 10);
            DATCAS_retezec[9]  = 48 + (UTC_den % 10);
            DATCAS_retezec[11] = 48 + (UTC_hod / 10);
            DATCAS_retezec[12] = 48 + (UTC_hod % 10);
            DATCAS_retezec[14] = 48 + (UTC_min / 10);
            DATCAS_retezec[15] = 48 + (UTC_min % 10);
            DATCAS_retezec[17] = 48 + (UTC_sek / 10);
            DATCAS_retezec[18] = 48 + (UTC_sek % 10);

            RTC_SD( tt , aktualni_cas_RTC, odchylka, interval , korekce , log_znacka);
          }
      }
  }
//----------------------------------------------



//----------------------------------------------
// do pole NMEA se ulozi cela neupravena NMEA veta (0 = GxRMC; 1 = GxGGA; 2 = verze programu; 3 = typ NEO modulu)
void gps_NMEA(uint8_t typ_zpravy)
  {
    if (digitalRead(pin_DATA_RDY) == osazeno_gps)                              // modul pripojeny (interni v HIGH / externi v LOW)
      {
        uint8_t i;
        for (i = 0 ; i < 90 ; i++)                                            // na zacatku smazat predchozi data z pole (naplnit mezerami)
          {
            pole_GPS_NMEA[i] = ' ';
          }
          
        if (typ_zpravy == 0)
          {
            GET_I2C_NMEA(50);
            GET_I2C_NMEA(51);
            GET_I2C_NMEA(52);
            GET_I2C_NMEA(53);
            GET_I2C_NMEA(54);
            GET_I2C_NMEA(55);
          }

        if (typ_zpravy == 1)
          {
            GET_I2C_NMEA(60);
            GET_I2C_NMEA(61);
            GET_I2C_NMEA(62);
            GET_I2C_NMEA(63);
            GET_I2C_NMEA(64);
            GET_I2C_NMEA(65);
          }

        if (typ_zpravy == 2)
          {
            GET_I2C_NMEA(70);
          }

    
        for (i = 0; i < 90 ; i++)                                             // vypsat kompletni stazenou zpravu
          {
            Serial.print(pole_GPS_NMEA[i]);
          }
        Serial.print("\r\n");
            
      }

      
  }
//----------------------------------------------


//----------------------------------------------
// zadost o posledni NMEA zpravy (prenasi se po blocich 15 znaku dlouhych)
void GET_I2C_NMEA(uint8_t blok)
  {
    
    Wire.beginTransmission(I2C_ADDR_GPS);
    delay(12);
    Wire.write(blok);
    delay(12);
    Wire.endTransmission();    
    delay(12);
    
    uint8_t i=0;
    Wire.requestFrom(I2C_ADDR_GPS,15);                                        // zpravy se stahuji po 15 znacich
    delay(12);
    while (Wire.available())                                                  // prijaty znak se uklada na presne stanovenou pozici v poli
      {
        if (blok < 60 and blok >=50)
          {
            pole_GPS_NMEA[((blok-50)*15) + i] = Wire.read();
          }

        if (blok < 70 and blok >=60)
          {
            pole_GPS_NMEA[((blok-60)*15) + i] = Wire.read();
          }

        if (blok == 70)
          {
            pole_GPS_NMEA[((blok-70)*15) + i] = Wire.read();
          }


        delay(5);
        i++;
      }
  }



//--------------------------
//  podle GPS souradnic (GPS_lat a GPS_lon) nastavi domaci souradnice pro Astro vypocty
//        Ve stupnich se zaokrouhlenim na 1 des misto.
//        S kontrolou nesmyslnych souradnic (v tom pripade nic neuklada) 
void nastav_HC(void)
  {
    double HC_lat;
    double HC_lon;
    int16_t zapis;
    int16_t lat_znamenko, lon_znamenko;

//  priklady pro ruzne polokoule
//  NW (Severni Amerika)         NE (Evropa)      SW (Jizni Amerika)         SE (Afrika)
//  GPS_lat=141527871        GPS_lat=139561204     GPS_lat=47511204       GPS_lat= 47511204
//  GPS_lon= 15366598        GPS_lon=194433265     GPS_lon=11418265       GPS_lon=193418265


    while (digitalRead(pin_tl_dn) == LOW or digitalRead(pin_tl_up) == LOW) 
       {
        delay(50);
       }
    delay(50);
    
    if (GPS_lat > 90000000UL)
      {
        HC_lat = GPS_lat - 90000000UL;                 // pro severni polokouli se odecita 90 stupnu (vysledejk je kladny)
        lat_znamenko = 1;                              // ve vysledku zustane kladne znamenko
      }
    else
      {
        HC_lat = GPS_lat;                              // na jizni polokouli se nic neodecita
        lat_znamenko = -1;                             // ale ve vysledku se musi nastavit zaporne znamenko
      }
    
    if (GPS_lon > 180000000UL)
      {
        HC_lon = GPS_lon - 180000000UL;                // pro vychodni polokouli se odecita 180 stupnu (vysledek je kladny)
        lon_znamenko = 1;                              // ve vysledku zustane kladne znamenko
      }
    else
      {
        HC_lon = GPS_lon ;                             // na zapadni polokouli se nic neodecita
        lon_znamenko = -1;                             // ale ve vysledku se musi nastavit zaporne znamenko
      }

    
    GeoLat = (HC_lat / 1000000.0) * lat_znamenko ;
    GeoLon = (HC_lon / 1000000.0) * lon_znamenko;
    
    if (GeoLat > 90 or GeoLat < -90 or GeoLon > 180 or GeoLon < -180)           // souradnice z GPS jsou uplne mimo, nactou se puvodni hodnoty
      {
        float geopomprom = EEPROM_read_int(eeaddr_GEO_lat);
        if (geopomprom > 32767) geopomprom = geopomprom - 65536  ; 
        GeoLat = geopomprom / 10.0;
        
        geopomprom = EEPROM_read_int(eeaddr_GEO_lon);
        if (geopomprom > 32767) geopomprom = geopomprom - 65536  ; 
        GeoLon = geopomprom / 10.0;
    
    
        if (GeoLat > 90 or GeoLat < -90 or GeoLon > 180 or GeoLon < -180)       // puvodni souradnice jsou taky uplne mimo, provede se default na 50/15
          {
            EEPROM_write_int(   eeaddr_GEO_lat   ,         500);                // zemepisna sirka pro astro vypocty na 50.0 stupnu
            EEPROM_write_int(   eeaddr_GEO_lon   ,         150);                // zemepisna delka pro astro vypocty na 15.0 stupnu
          }


      }
    else                                                                        // nove souradnice se zdaji byt v poradku, tak se ulozi do EEPROM
      {
        
        float zaokrouhleni;
        uint16_t obsah_eeprom;
        if (GeoLat < 0)  zaokrouhleni = -0.05;
        else             zaokrouhleni =  0.05;
        zapis = ((GeoLat + zaokrouhleni) * 10);
  
        EEPROM_write_int(   eeaddr_GEO_lat   ,  zapis);   // zemepisna sirka pro astro vypocty
        obsah_eeprom = EEPROM_read_int(eeaddr_GEO_lat);
  
        if (obsah_eeprom > 32767)
          {
           obsah_eeprom = 65536 - obsah_eeprom ; 
           GeoLat = obsah_eeprom / -10.0;
          }
        else
          {
            GeoLat = obsah_eeprom / 10.0;
          }


        if (GeoLon < 0)  zaokrouhleni = -0.05;
        else             zaokrouhleni =  0.05;
        zapis = ((GeoLon + zaokrouhleni) * 10);

        EEPROM_write_int(   eeaddr_GEO_lon   ,  zapis);   // zemepisna delka pro astro vypocty
        obsah_eeprom = EEPROM_read_int(eeaddr_GEO_lon);

        if (obsah_eeprom > 32767)
          {
           obsah_eeprom = 65536 - obsah_eeprom ; 
           GeoLon = obsah_eeprom / -10.0;
          }
        else
          {
            GeoLon = obsah_eeprom / 10.0;
          }

      }

  }
//----------------------------------------------



//----------------------------------------------
// 'souradnice' jsou posunute souradnice primo z I2C komunikace (uprava na citelnou hodnotu se provadi uvnitr tohoto podprogramu)
// priklady                                                                                                                   0 1 2 3 4 5 6  7 8 9 0 1 2 3
// GPS LAT: 139444319 = [N] 49.444319                    to je (139444319 - 90000000) / 1000000 = 49.444319 ..... na displeji x x x x ' 4 9. 4 4 4 4 3 1 9
// GPS ALT: 998 = 498 [m]
// GPS LON: 194366552 = [E] 14.366552
// 'smer_osy' je 0 (zemepisna sirka), 1 (nadmorska vyska)  nebo 2 (zemepisna delka)
// vystupem je retezec "pom_disp_scroll[]", ktery obsahuje cely text k zobrazeni (u delky a sirky presahuje displej)
void priprava_rotujiciho_retezce(uint32_t souradnice, uint8_t smer_osy)
  {
    uint8_t vyskove_znamenko;
    if (souradnice == 0x1FFFFFFFUL or souradnice == 0xFFFFFFFFUL)             // pri chybe GPS (jeste nejsou k dispozici spravne souradnice, nebo neni vubec pripojeny modul)
      {
        pom_disp_scroll[0] = 64;                                              // na displeji se zobrazi pomlcky bez scrolovani "-----"
        pom_disp_scroll[1] = 64;
        pom_disp_scroll[2] = 64;
        pom_disp_scroll[3] = 64;
        pom_disp_scroll[4] = 64;
      }
    else                                                                      // souradnice v poradku
      {  
        pom_disp_scroll[0] = 0;                                               // prazdna sedmisegmentovka
        pom_disp_scroll[1] = 0;                                               // prazdna sedmisegmentovka
        pom_disp_scroll[2] = 0;                                               // prazdna sedmisegmentovka
        pom_disp_scroll[3] = 0;                                               // prazdna sedmisegmentovka
        
        if(smer_osy == 1)                                                     // nadmorska vyska
          {
            if (souradnice > 500)                                             // nad morem
              {
                souradnice = souradnice - 500;                                // odecita se 500 metru
                vyskove_znamenko = 0;                                         // prazdna sedmisegmentovka               
              }
            else                                                              // pod morem
              {
                souradnice = 500 - souradnice ;
                vyskove_znamenko = 64;                                        // pomlcka na prvni sedmisegmentovce
              }       
          }

        
        
        if(smer_osy == 0)                                                     // zemepisna sirka (LAT)
          {
            if (souradnice > 90000000UL)                                      // severni polokoule
              {
                souradnice = souradnice - 90000000UL;                         // pro sever se odecita 90 stupnu
                pom_disp_scroll[4] = 1;                                       // horni pomlcka na sedmisegmentovce
                
              }
            else
              {
                pom_disp_scroll[4] = 64 + 8;                                  // pro jih se zobrazuje pomlcka a podtrzitko na prvni sedmisegmentovce 
              }     
          }
          
        if (smer_osy == 2)                                                    // zemepisna delka (LON)
          {
            if (souradnice > 180000000UL)                                     // vychodni polokoule
              {
                souradnice = souradnice - 180000000UL;                        // pro vychod se odecita 180 stupnu
                pom_disp_scroll[4] = 2;                                       // pravy (vychodni) apostrof na prvni sedmisegmentovce
                
              }
            else
              {
                pom_disp_scroll[4] = 64 + 32;                                 // pro zapad se zobrazi pomlcka a levy (zapadni) apostrof na sedmisegmentovce
              }     
          }
      
        if (smer_osy == 0 or smer_osy == 2 )                                  // pro LAT nebo LON se provadi uvodni scroll
          {
            uint32_t pom_souradnice = souradnice;
    
            uint32_t mocnina = 100000000UL;
            for (uint8_t rad = 5; rad < 14 ; rad ++)
              {
    
                if (pom_souradnice < mocnina)
                  {
                    pom_disp_scroll[rad] = graf_def[0];
                  }
                else
                  {
                    pom_disp_scroll[rad] = graf_def[pom_souradnice / mocnina];
                    pom_souradnice       =  pom_souradnice % mocnina;
                  }
    
                if (rad == 7) pom_disp_scroll[rad] = pom_disp_scroll[rad] + 128;      // desetinna tecka na displej
    
                mocnina = mocnina / 10UL;                                     // prechod na nizsi dekadu
              }
          }
        else                                                                  // nadmorska vyska se zobrazuje primo
          {
            if (souradnice < 9500)                                            // maximalni zobrazitelna vyska je 9500m cokoliv nad tuto hranici je nejaka porucha
              {
                pozice_tecky = 0;
                zobraz_cislo(souradnice, 0);            
                D_pamet[4] = vyskove_znamenko;
                aktualizuj_displej();
              }
            else
              {
                zobraz_text(68);                                              // na displeji se zobrazi pomlcky bez scrolovani "-----"
              }
          }
      }        
  }

  
