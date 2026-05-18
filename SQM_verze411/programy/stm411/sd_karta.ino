// podprogramy pro praci s SD kartou
//================================================


char jmeno_souboru[] = "20190125.csv";                                      // retezec pro konstrukci jmena souboru
char jmeno_slozky[] = "2019";                                               // retezec pro konstrukci nazvu slozky


//----------------------------------------------
//  zapis dat na kartu do souboru (\rok\yyyymmdd.csv)
//  vcetne nastaveni datumu a casu posledniho zapisu souboru do FAT
void SD_save(void)
  {
    if (pouzivat_SD_kartu == true)                                                    // kdyz je nastaveno pouzivani SD karty, tak se muze ukladat
      {
        // nejdriv zjistit, jestli je dneska pred polednem, nebo po poledni. Podle toho se urci jmeno souboru.
        //  Kdyz je dnesek jeste pred polednem, uklada se do souboru s predchozim dnem
        uint32_t tt = rtc_korekce();                                       // aktualni cas v RTC v sekundach od 1.1.1970
        sek1970_datcas(tt,false);
    
        // mtt_file se pouziva k vytvoreni jmena souboru a slozky, a proto tam muze byt nekdy i vcerejsek
        if (mtt.hour < 12)                                                      // od pulnoci se cele dopoledne dnesniho dne ulkada jeste do vcerejsiho souboru
          {
            tt_file = tt - 43200UL;                                             // datum je o 12 hodin starsi (vcerejsi)
            breakTimeFCE(tt_file,mtt_file);
          }
        else                                                                    // od poledne az do nasledujici pulnoci se uz vytvari normalni dnesni soubor
          {
            breakTimeFCE(tt,mtt_file);
          }
    
        // den, mesic a rok pro jmeno souboru a slozky
        LOC_den_file = mtt_file.day;
        LOC_mes_file = mtt_file.month;
        LOC_rok_file = mtt_file.year+1970;
    
    
        rokmesden = LOC_rok_file * 10000 + LOC_mes_file*100 + LOC_den_file;     // zacatek obdobi, pro ktere plati zaznamenane hodnoty (napriklad pri aktualnim case "23.5.2019 16:48" se zaznamena 20190523)
    
        
        jmeno_slozky[0]  = 48 + ((LOC_rok_file % 10000) / 1000 );               // tisice let
        jmeno_slozky[1]  = 48 + ((LOC_rok_file % 1000)  /  100 );               // stovky let
        jmeno_slozky[2]  = 48 + ((LOC_rok_file % 100)   /   10 );               // desitky let
        jmeno_slozky[3]  = 48 + ((LOC_rok_file % 10))           ;               // jednotky roku
        
        jmeno_souboru[0]  = 48 + ((rokmesden % 100000000L)   /  10000000L );    // rad tisicu roku
        jmeno_souboru[1]  = 48 + ((rokmesden % 10000000L)    /   1000000L );    // rad stovek roku
        jmeno_souboru[2]  = 48 + ((rokmesden % 1000000L)     /    100000L );    // rad desitek roku
        jmeno_souboru[3]  = 48 + ((rokmesden % 100000L)      /     10000L );    // rad jednotek roku
        jmeno_souboru[4]  = 48 + ((rokmesden % 10000L)       /      1000L );    // rad desitek mesicu
        jmeno_souboru[5]  = 48 + ((rokmesden % 1000L)        /       100L );    // rad jednotek mesicu
        jmeno_souboru[6]  = 48 + ((rokmesden % 100L)         /        10L );    // rad desitek dni
        jmeno_souboru[7]  = 48 + ((rokmesden % 10L)                      );     // rad jednotek dni
    
    //  jmeno_souboru[8]  = '.';      // tyhle znaky se nemeni
    //  jmeno_souboru[9]  = 'c';
    //  jmeno_souboru[10] = 's';
    //  jmeno_souboru[11] = 'v';
    //  jmeno_souboru[12] = '\0';
    
        for ( i = 0; i< 4 ; i++)
          {
            cesta[i] = jmeno_slozky[i];
          }
    
        cesta[4] = '/';
        for ( i = 5; i< 18 ; i++)
          {
            cesta[i] = jmeno_souboru[i-5];
          }
    
    
        if (digitalRead(pin_karta_IN) == LOW)                                   // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                                // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                                  // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            sd.begin(SD_CONFIG);                                                //inicializace SD karty pro pripad, ze by byla predtim vytazena
        
            if (!sd.exists(jmeno_slozky))   sd.mkdir(jmeno_slozky);
    
            bool novy_soubor;
            if (!sd.exists(cesta))   novy_soubor = true;                        // Soubor neexistuje - bude se zapisovat hlavicka
            else                     novy_soubor = false;                       // Soubor uz existuje - hlavicka se zapisovat nebude

            if (novy_soubor == true)                                        // pri zalozeni noveho souboru se jeho jmeno (yyyymmdd) zaznamena do EEPROM
              {                                                             //      pak se muze pouzit pro funkci "vypis POSLEDNIHO souboru se zaznamy"          
                EEPROM_write_long(eeaddr_posledni_den,rokmesden);
              }

        
            SdFile::dateTimeCallback(dateTime);
           
            open_OK = soubor.open(cesta, O_WRITE | O_APPEND | O_CREAT);
    
            if (open_OK)
              {                
                if (vytvaret_hlavicku == true and novy_soubor == true)
                  {
    //              soubor.println(hlavicka);  
                    print_co_kam(false, true);    
                  }
    //          soubor.println(vystupni_retezec); 
                print_co_kam(true, true);

                delay(20);
                soubor.close();
              }    
            else
              {
                chyba(2);                                                       // SD-Err
              }
            
          }
        else                                                                    // karta je vysunuta, nic se nezapisuje
          {
            bitSet(err_bit,2);                                                  // SD karta je vysunuta, nastavuje se bit v promenne 'err_bit'               
            
            if (bitRead(signal_LED,2) == true)                                  // pokud je povoleno bliknuti LED pri chybach
              {
                blik_B();                                                       // pri vysunute karte blikne modrou LED     
              }
          }

      }                                                                         // kdyz neni nastaveno pouzivani SD karty, tak se ukladaci podprogram preskakuje
  }
//----------------------------------------------
    

//----------------------------------------------
// podprogram pro nastavovani datumu a casu u souboru na SD karte:
// zdroj:  https://forum.arduino.cc/index.php?topic=348562.0
void dateTime(uint16_t* date, uint16_t* time)
  {
    if (pouzivat_SD_kartu == true)
      {    
        *date = FAT_DATE(LOC_rok, LOC_mes, LOC_den);
        *time = FAT_TIME(LOC_hod, LOC_min, LOC_sek);
      }
  }
//----------------------------------------------



//----------------------------------------------
// prevede cast pole prijato[] na integer   
//    (reseni s podprogramem je hlavne kvuli uspore mista v PROGMEM - prevzal jsem to z jineho sveho programu a uz se mi to nechtelo predelavat)
// priklad:                            prijato = { 50,  48,  49,  56}
// v ASCII znacich to je               prijato = {'2', '0', '1', '8'}
//   tak zavolani funkce  "prijato_int(0,4)" vrati cislo 2018
uint16_t prijato_int( uint8_t znak_start, uint8_t znak_stop)
  {
    uint16_t soucet = 0;
    uint16_t mocnina = 0;
    for (i = znak_start; i < znak_stop; i++)
      {
    
        switch (znak_stop - i)
          {
            case 1:
              mocnina = 1;
              break;
            case 2:
              mocnina = 10;
              break;
            case 3:
              mocnina = 100;
              break;
            case 4:
              mocnina = 1000;
              break;
          }
      
        soucet = soucet + (prijato[i] - '0') * mocnina ;
      }
    return soucet;
  }
//----------------------------------------------



//----------------------------------------------
//  Pri nastaveni casu pres GPS se zaznamenaji na SD kartu udaje o aktualnim case v RTC aby bylo mozne zjistovat presnost vnitrniho RTC
void RTC_SD(uint32_t novy, uint32_t v_rtc, long odchylka, uint32_t interval, uint32_t log_korekce , uint16_t logznacka)
  {
    if (pouzivat_SD_kartu == true)                                          // kdyz je nastaveno pouzivani SD karty, tak se muze ukladat
      {
        char dbg_cesta[] = "LOGS/RTC_set.csv";
        if (digitalRead(pin_karta_IN) == LOW)                               // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                            // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                              // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            sd.begin(SD_CONFIG);                                            //inicializace SD karty pro pripad, ze by byla predtim vytazena
            bool RTC_hlavicka = false;
            if (!sd.exists(dbg_cesta))    RTC_hlavicka = true;              // Soubor neexistuje - bude se zapisovat hlavicka
       
            SdFile::dateTimeCallback(dateTime);
            
            open_OK = soubor.open(dbg_cesta, O_WRITE | O_APPEND | O_CREAT);
            if (open_OK)
              {
                if (RTC_hlavicka == true)
                  {
                    soubor.println("YYYY/MM/DD;hh:nn:ss;  GPS [s] ;  RTC [s] ;  RTC-GPS  ; int[D];  kor[h]  ;9  Info  0;");
    //              soubor.println("===================================================================================;");
    
                    for (uint8_t i = 0 ; i < 83 ; i++)
                      {
                        soubor.print('=');
                      }
                    soubor.println(';');                
                  }
    
                soubor.print(DATCAS_retezec); 
                soubor.print(';'); 
                soubor.print(novy); 
                soubor.print(';');
                if (v_rtc < 1000000000UL) soubor.print("          ");
                else                      soubor.print(v_rtc); 
                soubor.print(';');
                if (odchylka < 0)   soubor.print('-');
                else                soubor.print('+');
                odchylka = abs(odchylka);
                
                if (odchylka < 1000000000L) soubor.print('0');
                if (odchylka <  100000000L) soubor.print('0');
                if (odchylka <   10000000L) soubor.print('0');
                if (odchylka <    1000000L) soubor.print('0');
                if (odchylka <     100000L) soubor.print('0');
                if (odchylka <      10000L) soubor.print('0');
                if (odchylka <       1000L) soubor.print('0');
                if (odchylka <        100L) soubor.print('0');
                if (odchylka <         10L) soubor.print('0');
                soubor.print(odchylka); 
                soubor.print(';'); 
    
                if (interval > 63115200)
                  {
                    soubor.print("       ");
                  }
                else
                  {
                    if ((interval / 86400) < 100)  soubor.print('0');
                    if ((interval / 86400) <  10)  soubor.print('0');
                    soubor.print(interval / 86400.0,3); 
                  }
                soubor.print(';'); 
                   
                if (log_korekce == 0x7FFFFFFFUL)
                  {
                    soubor.print("          ");
                  }
                else
                  {
                    if (log_korekce >= 0x80000000UL)      soubor.print('-');
                    else                                  soubor.print('+');
    
                    log_korekce = (0x7FFFFFFF & log_korekce);               // odstraneni nejjvyssiho bitu jako znamenka
                    
                    if ((log_korekce / 3600) < 100000UL)  soubor.print('0');
                    if ((log_korekce / 3600) <  10000UL)  soubor.print('0');
                    if ((log_korekce / 3600) <   1000UL)  soubor.print('0');
                    if ((log_korekce / 3600) <    100UL)  soubor.print('0');
                    if ((log_korekce / 3600) <     10UL)  soubor.print('0');
    
                    soubor.print((log_korekce/3600.0),2); 
                  }
                soubor.print(';');
                if (logznacka <  512) soubor.print('0');
                if (logznacka <  256) soubor.print('0');
                if (logznacka <  128) soubor.print('0');
                if (logznacka <   64) soubor.print('0');
                if (logznacka <   32) soubor.print('0');
                if (logznacka <   16) soubor.print('0');
                if (logznacka <    8) soubor.print('0');
                if (logznacka <    4) soubor.print('0');
                if (logznacka <    2) soubor.print('0');
                soubor.print(logznacka,BIN);
                soubor.println(';'); 
                delay(20);
                soubor.close();
    
              }        
          }
        else                                                                // karta neni zasunuta
          {
            bitSet(err_bit,2);                                              // SD karta je vysunuta, nastavuje se bit v promenne 'err_bit'               
          }
      }                                                                     // kdyz neni povolene pouzivani SD karty, nic se nestane      
  }
//----------------------------------------------


//----------------------------------------------
// Zapis parametru ze systemove oblasti EEPROM na SD kartu jako textovy soubor
//   (zaloha systemoveho nastaveni)
void SD_sysMEM_WRITE(void)
  {
    if (pouzivat_SD_kartu == true)                                          // kdyz je nastaveno pouzivani SD karty, tak se muze ukladat   
      {
        char cesta[] = "LOGS/SYS_MEM.TXT";
        if (digitalRead(pin_karta_IN) == LOW)                               // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                            // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                              // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            zobraz_RTC(false);
            sd.begin(SD_CONFIG);                                            //inicializace SD karty pro pripad, ze by byla predtim vytazena
            SdFile::dateTimeCallback(dateTime);
            
            open_OK = soubor.open(cesta, O_WRITE | O_CREAT);                // pripadny stary soubor se prepisuje
            if (open_OK)
              {

                soubor.print(EEPROM_verze);                                 // na prvni radku v souboru se ulozi verze rozlozeni EEPROM, se kterou je zaloha vytvorena
                                                                            //              "ver:001\r\n"

                bitClear(err_bit,1);                                        // SD karta je v poradku, maze se pripadny bit v promenne 'err_bit'       
                for (uint16_t adr_mem = 0; adr_mem < eeaddr_MIN_EEPROM; adr_mem ++) // zapisuje se cela oblast adres 0 az 699
                  {
                    if (adr_mem < 10 ) soubor.print('0');
                    if (adr_mem < 100) soubor.print('0');
                    soubor.print(adr_mem);
                    soubor.print(';');
                    uint8_t data_mem = EEPROM_read(adr_mem);
    
                    if (data_mem < 10 ) soubor.print('0');
                    if (data_mem < 100) soubor.print('0');
                    soubor.println(data_mem);
                    Serial.print(adr_mem);
                    Serial.print(';');
                    Serial.println(data_mem);
                  }
                soubor.close();
              }
            else
              {
                chyba(2);
              }                                                             // SD-Err
          }
        else
          {
            chyba(3);                                                       // No-SD
          }
      }                                                                     // kdyz neni SD karta povolena, podprogram se nevykona      
  }



//----------------------------------------------
// Obnoveni parametru z SD karty do systemove casti pameti (adresy 8 az 699) s vyjimkou
//     casove kalibrace a zachytneho bodu pro hledani prazdneho mista
void SD_sysMEM_READ(void)
  {
    if (pouzivat_SD_kartu == true)                                          // kdyz je nastaveno pouzivani SD karty, tak se z ni muze cist   
      {
        char cesta[] = "LOGS/SYS_MEM.TXT";
        char buff[15];                                                      // buffer pro ukladani ctenych dat ze souboru
        uint16_t adr_dat;                                               // prectena data pro kazdou adresu
        bool shoda;
        bool vynechat;                                                      // nektere udaje se nenahrazuji starymi hodnotami ze souboru a ponechavaji se v EEPROM beze zmeny
    
        if (digitalRead(pin_karta_IN) == LOW)                               // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                            // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                              // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            zobraz_RTC(false);
            sd.begin(SD_CONFIG);                                            //inicializace SD karty pro pripad, ze by byla predtim vytazena
    
            open_OK = soubor.open(cesta, O_READ);                           // soubor pro cteni
            if (open_OK)
              {
                uint8_t BOM = 0;                                               // testovani delky pripadne znacky kodovani
                soubor.fgets(buff, 9);                                      // nejdriv se zjisti jestli soubor obsahuje BOMznacku
                while (buff[BOM] != ':' and BOM < 9)                        // nalezeni znaku ':' na prvni radce
                  {
                    BOM++;
                  }
                BOM = BOM - 3;                                              // odecteni delky retezce "ver"
                               
                soubor.seek(BOM);                                           // nastaveni prvniho znaku souboru az za BOM znacku
                soubor.fgets(buff, 9);                                      // prvni radka je pro zjisteni rozlozeni dat v EEPROM (verze)
                shoda = true;
                for (uint8_t i = 0; i < 7 ; i++)
                  {
                    if (buff[i] != EEPROM_verze[i]) shoda = false;          // kdyz se verze neshoduji, neni mozne obnoveni
                  }

                if (shoda == false)
                  {
                    Serial.println(lng321);                                 // "Verze zalohovaneho souboru se neshoduje s verzi pozadovanou v tomto programu."
                    Serial.print(buff);
                    Serial.print(EEPROM_verze);
                    return;                                                 // obnovovaci podprogram se ukonci
                  }
                
                bitClear(err_bit,1);                                        // SD karta je v poradku, maze se pripadny bit v promenne 'err_bit'                  
                for (uint16_t adr_mem = 0; adr_mem < eeaddr_MIN_EEPROM; adr_mem ++)// cte se cela oblast adres 0 az do prvniho bajtu prvniho zaznamu (adresa 700)
                  {
                    soubor.fgets(buff, 9);                                  // prvni 3 znaky jsou adresa, dalsi znak je strednik, pak 3 znaky pro data, nakonec ukonceni radky CRLF

                    vynechat = false;                                       // nektere udaje se nenahrazuji starymi hodnotami ze souboru a ponechavaji se v EEPROM beze zmeny
                    if ((adr_mem >= eeaddr_RTC_set)     and (adr_mem <= (eeaddr_RTC_set + 3)))        vynechat = true;          // cas posledni kalibrace
                    if ((adr_mem >= eeaddr_RTC_korekce) and (adr_mem <= (eeaddr_RTC_korekce + 3)))    vynechat = true;          // autokorekce
                    if (adr_mem == eeaddr_10kB_znacka)                                                vynechat = true;          // zachytny bod pro hledani prazdneho mista
    
                    if (vynechat == false)                                  // vsechno ostatni se prepisuje hodnotami ze souboru
                      {
                        adr_dat = (buff[4] - 48) * 100;
                        adr_dat = adr_dat + (buff[5] - 48) * 10;
                        adr_dat = adr_dat + (buff[6] - 48);
        
                        Serial.print(adr_mem);
                        Serial.print(';');
                        Serial.println(adr_dat);    
                        EEPROM_write(adr_mem,adr_dat);
                      }
    
                  }
                soubor.close();
                EEPROM_write(eeaddr_10kB_znacka,0);                         // zachytny bod pro hledani volne pameti se nastavi na 0
                SOFT_RESET();
              }
            else
              {
                chyba(2);
              }                                                             // SD-Err
          }
        else
          {
            chyba(3);                                                       // No-SD
          }
      }                                                                     // kdyz neni SD karta povolena, podprogram se nevykona
  }

//----------------------------------------------


//----------------------------------------------
// Pokud je povolene detailni logovani, uklada se do souboru "SYS_LOG.TXT" na SD kartu kazda zmena obsahu EEPROM na adresach 0 az 699
//   Specialni kody pro dalsi typy logu (999 = bezne zapnuti napajeni)
void SD_log(uint32_t adresa , uint16_t logdata)
  {
    if (pouzivat_SD_kartu == true)                                          // kdyz je nastaveno pouzivani SD karty, tak se muze ukladat   
      {
        char log_cesta[] = "LOGS/SYS_LOG.TXT";
        if (digitalRead(pin_karta_IN) == LOW)                               // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                            // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                              // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            zobraz_RTC(false);
            sd.begin(SD_CONFIG);                                            // inicializace SD karty pro pripad, ze by byla predtim vytazena
            
            if (!sd.exists("LOGS"))   sd.mkdir("LOGS");                     // kdyz slozka "\LOGS\" neexistuje, tak se vytvori
            
            
            SdFile::dateTimeCallback(dateTime);
            
            open_OK = log_soubor.open(log_cesta,  O_WRITE | O_APPEND | O_CREAT);    // dalsi logy se pridavaji na konec logovaciho souboru
            if (open_OK)
              {
                bitClear(err_bit,1);                                        // SD karta je v poradku, maze se pripadny bit v promenne 'err_bit'       
    
                if (LOC_rok < 2000)                                         // jen ochrana pro spatne nastavene RTC
                  {
                    DATCAS_retezec[0] = '1';
                    DATCAS_retezec[1] = '9';
                    DATCAS_retezec[2]  = 48 + ((LOC_rok-1900) / 10);
                    DATCAS_retezec[3]  = 48 + ((LOC_rok-1900) % 10);                                
                  }
                else
                  {
                    DATCAS_retezec[0] = '2';
                    DATCAS_retezec[1] = '0';
                    DATCAS_retezec[2]  = 48 + ((LOC_rok - 2000) / 10);
                    DATCAS_retezec[3]  = 48 + ((LOC_rok - 2000) % 10);                
                  }
    
                DATCAS_retezec[5]  = 48 + (LOC_mes / 10);
                DATCAS_retezec[6]  = 48 + (LOC_mes % 10);
                DATCAS_retezec[8]  = 48 + (LOC_den / 10);
                DATCAS_retezec[9]  = 48 + (LOC_den % 10);
                DATCAS_retezec[11] = 48 + (LOC_hod / 10);
                DATCAS_retezec[12] = 48 + (LOC_hod % 10);
                DATCAS_retezec[14] = 48 + (LOC_min / 10);
                DATCAS_retezec[15] = 48 + (LOC_min % 10);
                DATCAS_retezec[17] = 48 + (LOC_sek / 10);
                DATCAS_retezec[18] = 48 + (LOC_sek % 10);
                
                log_soubor.print(DATCAS_retezec); 
                log_soubor.print(';'); 
                
                if (adresa < 10 ) log_soubor.print('0');
                if (adresa < 100) log_soubor.print('0');
                log_soubor.print(adresa);
                log_soubor.print(';');
                if (logdata > 999)                                          // omezeni cisla na sirku 3 znaky
                  {
                    log_soubor.println("^^^");
                  }
                else
                  {
                    if (logdata < 10 ) log_soubor.print('0');
                    if (logdata < 100) log_soubor.print('0');
                    log_soubor.println(logdata);                
                  }
                log_soubor.close();
              }
            else
              {
                chyba(2);
              }                                                             // SD-Err
          }
        else
          {
            chyba(3);                                                       // No-SD
          }
      }                                                                     // kdyz neni SD karta povolena, nic se neloguje
  }





//----------------------------------------------
// v rezimu prubezneho mereni jasu BEZ KOREKCE se merene hodnoty ukladaji do souboru "logjas.txt"
void logjas(uint16_t logovane_cislo)
  {
    if (pouzivat_SD_kartu == true)                                              // kdyz je nastaveno pouzivani SD karty, tak se muze ukladat   
      {
        char cesta[] = "LOGS/logjas.txt";
        if (digitalRead(pin_karta_IN) == LOW)                                   // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                                // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                                  // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            zobraz_RTC(false);
            sd.begin(SD_CONFIG);                                                //inicializace SD karty pro pripad, ze by byla predtim vytazena

            if (!sd.exists("LOGS"))   sd.mkdir("LOGS");                     // kdyz slozka "\LOGS\" neexistuje, tak se vytvori
                        
            SdFile::dateTimeCallback(dateTime);
            
            open_OK = soubor.open(cesta,  O_WRITE | O_APPEND | O_CREAT);        // dalsi logy se pridavaji na konec logovaciho souboru
            if (open_OK)
              {
                bitClear(err_bit,1);                                            // SD karta je v poradku, maze se pripadny bit v promenne 'err_bit'       
    
                if (LOC_rok < 2000)                                             // jen ochrana pro spatne nastavene RTC
                  {
                    DATCAS_retezec[0] = '1';
                    DATCAS_retezec[1] = '9';
                    DATCAS_retezec[2]  = 48 + ((LOC_rok-1900) / 10);
                    DATCAS_retezec[3]  = 48 + ((LOC_rok-1900) % 10);                                
                  }
                else
                  {
                    DATCAS_retezec[0] = '2';
                    DATCAS_retezec[1] = '0';
                    DATCAS_retezec[2]  = 48 + ((LOC_rok - 2000) / 10);
                    DATCAS_retezec[3]  = 48 + ((LOC_rok - 2000) % 10);                
                  }
    
                DATCAS_retezec[5]  = 48 + (LOC_mes / 10);
                DATCAS_retezec[6]  = 48 + (LOC_mes % 10);
                DATCAS_retezec[8]  = 48 + (LOC_den / 10);
                DATCAS_retezec[9]  = 48 + (LOC_den % 10);
                DATCAS_retezec[11] = 48 + (LOC_hod / 10);
                DATCAS_retezec[12] = 48 + (LOC_hod % 10);
                DATCAS_retezec[14] = 48 + (LOC_min / 10);
                DATCAS_retezec[15] = 48 + (LOC_min % 10);
                DATCAS_retezec[17] = 48 + (LOC_sek / 10);
                DATCAS_retezec[18] = 48 + (LOC_sek % 10);
                
                soubor.print(DATCAS_retezec); 
                soubor.print(';'); 
                
                if (logovane_cislo < 10000)  soubor.print('0');
                if (logovane_cislo <  1000)  soubor.print('0');
                if (logovane_cislo <   100)  soubor.print('0');
                if (logovane_cislo <    10)  soubor.print('0');
                soubor.print(logovane_cislo);
                soubor.print(';'); 
                if (JAS_log_full < 10000)             soubor.print('0');        // ke kazdemu zaznamu se jeste pro kontrolu pridavaji zmerene (a neupravene) hodnoty svetelnych registru a konfigurace cidla TSL
                if (JAS_log_full < 1000)              soubor.print('0');
                if (JAS_log_full < 100)               soubor.print('0');
                if (JAS_log_full < 10)                soubor.print('0');
                soubor.print(JAS_log_full);
                soubor.print(';'); 
                if (JAS_log_ir < 10000)               soubor.print('0');
                if (JAS_log_ir < 1000)                soubor.print('0');
                if (JAS_log_ir < 100)                 soubor.print('0');
                if (JAS_log_ir < 10)                  soubor.print('0');
                soubor.print(JAS_log_ir);
                soubor.print(';'); 
                if (JAS_log_cnf < 100)                soubor.print('0');
                if (JAS_log_cnf < 10)                 soubor.print('0');
                soubor.println(JAS_log_cnf);
                soubor.close();
              }
            else
              {
                chyba(2);
              }                                                                 // SD-Err
          }
        else
          {
            chyba(3);                                                           // No-SD
          }
      }                                                                         // kdyz neni SD karta povolena, nic se neloguje
  }


//----------------------------------------------



//----------------------------------------------
// docasne ukladani merene hodnoty do souboru "logafd.txt"
void logafd(uint32_t logovane_cislo)
  {
    if (pouzivat_SD_kartu == true)                                              // kdyz je nastaveno pouzivani SD karty, tak se muze ukladat   
      {
        char cesta[] = "LOGS/logafd.txt";
        if (digitalRead(pin_karta_IN) == LOW)                                   // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                                // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                                  // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            zobraz_RTC(false);
            sd.begin(SD_CONFIG);                                                //inicializace SD karty pro pripad, ze by byla predtim vytazena
            if (!sd.exists("LOGS"))   sd.mkdir("LOGS");                         // kdyz slozka "\LOGS\" neexistuje, tak se vytvori
            SdFile::dateTimeCallback(dateTime);
            
            open_OK = soubor.open(cesta,  O_WRITE | O_APPEND | O_CREAT);        // dalsi logy se pridavaji na konec logovaciho souboru
            if (open_OK)
              {
                bitClear(err_bit,1);                                            // SD karta je v poradku, maze se pripadny bit v promenne 'err_bit'       
    
                if (LOC_rok < 2000)                                             // jen ochrana pro spatne nastavene RTC
                  {
                    DATCAS_retezec[0] = '1';
                    DATCAS_retezec[1] = '9';
                    DATCAS_retezec[2]  = 48 + ((LOC_rok-1900) / 10);
                    DATCAS_retezec[3]  = 48 + ((LOC_rok-1900) % 10);                                
                  }
                else
                  {
                    DATCAS_retezec[0] = '2';
                    DATCAS_retezec[1] = '0';
                    DATCAS_retezec[2]  = 48 + ((LOC_rok - 2000) / 10);
                    DATCAS_retezec[3]  = 48 + ((LOC_rok - 2000) % 10);                
                  }
    
                DATCAS_retezec[5]  = 48 + (LOC_mes / 10);
                DATCAS_retezec[6]  = 48 + (LOC_mes % 10);
                DATCAS_retezec[8]  = 48 + (LOC_den / 10);
                DATCAS_retezec[9]  = 48 + (LOC_den % 10);
                DATCAS_retezec[11] = 48 + (LOC_hod / 10);
                DATCAS_retezec[12] = 48 + (LOC_hod % 10);
                DATCAS_retezec[14] = 48 + (LOC_min / 10);
                DATCAS_retezec[15] = 48 + (LOC_min % 10);
                DATCAS_retezec[17] = 48 + (LOC_sek / 10);
                DATCAS_retezec[18] = 48 + (LOC_sek % 10);
                
                soubor.print(DATCAS_retezec); 
                soubor.print(';'); 
                if (logovane_cislo < 10000)           soubor.print('0');
                if (logovane_cislo < 1000)            soubor.print('0');
                if (logovane_cislo < 100)             soubor.print('0');
                if (logovane_cislo < 10)              soubor.print('0');                
                soubor.println(logovane_cislo);
                soubor.close();
              }
            else
              {
                chyba(2);
              }                                                                 // SD-Err
          }
        else
          {
            chyba(3);                                                           // No-SD
          }
      }                                                                         // kdyz neni SD karta povolena, nic se neloguje
  }


//----------------------------------------------




//----------------------------------------------
// Obsah vybraneho souboru vypise do seriove linky.
// Podle parametru 2 muze vypsat jen cast souboru (poslednich nekolik radek).
// Parametr 3 u nekterych typu souboru vybira konkretni jmeno souboru (typy 'B' a 'C'), nebo umoznuje filtraci vypisovanych adres (typ 'A').

// Funkce ma nasledujici parametry
//     @> [t][,par2][,par3]
//
// t ... typ souboru 
//                           A  "/LOGS/SYS_LOG.TXT" 
//                           B  "/yyyy/########.csv"
//                           C  "/R30S/r_30s_##.csv"
//                           D  "/LOGS/RTC_set.csv"
//                           E  "/LOGS/SYS_MEM.TXT"
//                           F  "/LOGS/logafd.txt"
//                           G  "/LOGS/logjas.txt"
//                           H  "/TRCK/trck####.gpx"
//                           I  "/CALB/nnn-mmdd.csv"
//                           T  "tele_par.txt"
//                           ?  "/HELP/2_[.txt"      (napoveda - bez zadani typu je to default)
//
// param2 ...  maximalni pocet zaznamu (radek) pro vypis
//               (minimum je 1; bez parametru = 50) radky se pocitaji od konce souboru
//                Pri typu '?' se vypisuje cela napoveda, nezavisle na zadanem poctu radek.
//                Pri typu 'H' se vypisuje cely GPX soubor, nezavisle na zadanem poctu radek.
//                Pri typu 'I' se vypisuje cely CSV soubor, nezavisle na zadanem poctu radek.
//                Typ 'T' je special pro nasatveni dalekohledu v menu DSO a zadne dalsi parametry nema
//
// param3 ...  pro typ A   volba vypisovane adresy 
//                  (bez parametru = vsechno)
//             pro typ B   datum v CSV souboru 
//                  (bez parametru = datum posledniho zaznamu)
//             pro typ C   poradove cislo ## ve jmene "r_30s_##.csv"
//                  (bez parametru = posledni pouzite cislo)
//             pro typ H   poradove cislo #### ve jmene "trck####.gpx"
//                  (bez parametru = posledni pouzite cislo)
//             pro typ I   stari souboru (1=posledni ukladany, 2=predposleni ukladany (druhy od konce), ... 5=paty od konce ....)
//                  (bez parametru (nebo s parametrem 9999) vypis vsech ukladanych CSV souboru (obsah souboru "/CALB/seznam.txt")
//             pro ostatni typy souboru je parametr 3 vypusten.                                           

void file_to_serkom(char typ, uint16_t param2, uint16_t param3)
  {
    uint16_t read_adresa;                                                   // adresa, ktera se precte z radky sys_logu
    uint16_t pocet_radek = 0;                                               // pocet zjistenych radek v souboru
    uint16_t delka_radky;                                                   // kazdy typ souboru ma jiny pocet znaku
    uint16_t posledni_znak = 0;                                                 // jen pro graficke zakonceni vypisu (kdyz soubor nekonci odradkovanim, jedna prazna radka se prida)
    char cesta[] = "XXXX/12345678.XYZ";                                         // retezec pro konstrukci cele cesty k souboru pro pripad, ze je soubor v nejake slozce
    
    char f_name_A[] = "LOGS/SYS_LOG.TXT ";                                      // definice konkretnich jmen souboru
    char f_name_B[] = "2024/20240825.csv";                                      // misto cisel bude rok jako slozka a datum ve formatu YYYYMMDD jako jmeno souboru
    char f_name_C[] = "R30S/r_30s_##.csv";                                      // cislo ## se bude jeste upravovat podle 3. parametru
    char f_name_D[] = "LOGS/RTC_set.csv ";
    char f_name_E[] = "LOGS/SYS_MEM.TXT ";
    char f_name_F[] = "LOGS/logafd.txt  ";
    char f_name_G[] = "LOGS/logjas.txt  ";
    char f_name_H[] = "TRCK/trck####.gpx";                                      // cislo #### se bude upravovat podle 3. parametru 
    char f_name_I[] = "CALB/seznam.txt  ";                                      // soubor obsahuje seznam vsech kalibracnich CSV souboru a nacita se vzdycky (nezavisle na parametrech)
    char f_name_Z[] = "HELP/2_[.txt     ";                                      // pozustatek z doby, kdy se napoveda pro tuto funkci vypisovala jako default prikazu '@>' nebo "@>?"
    
    if (pouzivat_SD_kartu == true)
      {
        if (typ == 'A')                       // priklad souboru: "LOGS/SYS_LOG.TXT "
          {
            for (uint8_t fn_znak = 0; fn_znak < 16 ; fn_znak ++) { cesta[fn_znak] = f_name_A[fn_znak]; }
            cesta[16] = '\0';
            delka_radky = 29; 
          }

        if (typ == 'B')                      // priklad souboru: "2024/20240420.csv"
          {
            f_name_B[0]  = 48 + ((param3 % 100000000UL)   /  10000000UL );    // rad tisicu roku ve jmenu slozky
            f_name_B[1]  = 48 + ((param3 % 10000000UL)    /   1000000UL );    // rad stovek roku ve jmenu slozky
            f_name_B[2]  = 48 + ((param3 % 1000000UL)     /    100000UL );    // rad desitek roku ve jmenu slozky
            f_name_B[3]  = 48 + ((param3 % 100000UL)      /     10000UL );    // rad jednotek roku ve jmenu slozky
            f_name_B[4]  = '/';
            f_name_B[5]  = f_name_B[0];                                       // rad tisicu roku ve jmenu souboru
            f_name_B[6]  = f_name_B[1];                                       // rad stovek roku ve jmenu souboru
            f_name_B[7]  = f_name_B[2];                                       // rad desitek roku ve jmenu souboru
            f_name_B[8]  = f_name_B[3];                                       // rad jednotek roku ve jmenu souboru
            f_name_B[9]  = 48 + ((param3 % 10000UL)       /      1000UL );    // rad desitek mesicu ve jmenu souboru
            f_name_B[10] = 48 + ((param3 % 1000UL)        /       100UL );    // rad jednotek mesicu ve jmenu souboru
            f_name_B[11] = 48 + ((param3 % 100UL)         /        10UL );    // rad desitek dni ve jmenu souboru
            f_name_B[12] = 48 + ((param3 % 10UL)                        );    // rad jednotek dni ve jmenu souboru
            f_name_B[13] = '.';
            f_name_B[14] = 'c';
            f_name_B[15] = 's';
            f_name_B[16] = 'v';

            for (uint8_t fn_znak = 0; fn_znak < 17 ; fn_znak ++) { cesta[fn_znak] = f_name_B[fn_znak]; }
            cesta[17] = '\0';
            delka_radky = velikost_hlavicky + 2;                                   // delka radky je zavisla na poctu pouzitych rezervnich cidel
          }

        if (typ == 'C')                      // priklad souboru: "R30S/r_30s_25.csv"
          {
           
            for (uint8_t fn_znak = 0; fn_znak < 17 ; fn_znak ++) { cesta[fn_znak] = f_name_C[fn_znak]; }
            if (param3 < 100)
              {
                cesta[11]  = 48 + ((param3 % 100UL)         /        10UL );    // rad desitek v poradovem cisle
                cesta[12]  = 48 + ((param3 % 10UL)                        );    // rad jednotek v poradovem cisle            
              }
            else                                                                  // pri parametru 100 a vice je ve jmene souboru misto cisla dvojite 'x' ("r_30s_xx.csv")
              {
                cesta[11]  = 'x';
                cesta[12]  = 'x';                
              }

            cesta[17] = '\0';
            delka_radky = 100;
          }


        
        if (typ == 'D')                      // priklad souboru: "LOGS/RTC_set.csv ";
          {
            for (uint8_t fn_znak = 0; fn_znak < 16 ; fn_znak ++) { cesta[fn_znak] = f_name_D[fn_znak]; }
            cesta[16] = '\0';
            delka_radky = 86;

          }

        if (typ == 'E')                      // priklad souboru: "LOGS/SYS_MEM.TXT "
          {
            for (uint8_t fn_znak = 0; fn_znak < 16 ; fn_znak ++) { cesta[fn_znak] = f_name_E[fn_znak]; }
            cesta[16] = '\0';
            delka_radky = 9;
          }

        if (typ == 'F')                      // priklad souboru: "LOGS/logafd.txt  "
          {
            for (uint8_t fn_znak = 0; fn_znak < 15 ; fn_znak ++) { cesta[fn_znak] = f_name_F[fn_znak]; }
            cesta[15] = '\0';
            cesta[16] = '\0';
            delka_radky = 27;
          }

        if (typ == 'G')                      // priklad souboru: "LOGS/logjas.txt  "
          {
            for (uint8_t fn_znak = 0; fn_znak < 15 ; fn_znak ++) { cesta[fn_znak] = f_name_G[fn_znak]; }
            cesta[15] = '\0';
            cesta[16] = '\0';
            delka_radky = 27 + 16;           // (nove se do logu ukladaji i hodnoty svetelnych registru, proto se delka radky rozsirila o 2x5 znaku, 1x3 znaky a 3 stredniky)
          }

        if (typ == 'H')                          // priklad souboru "TRCK/trck1234.gpx";
          {
            f_name_H[9]   = 48 + ((param3 % 10000)  /      1000 );    // rad tisicu
            f_name_H[10]  = 48 + ((param3 %  1000)  /       100 );    // rad stovek
            f_name_H[11]  = 48 + ((param3 %   100)  /        10 );    // rad desitek
            f_name_H[12]  = 48 + ((param3 %    10)              );    // rad jednotek


            for (uint8_t fn_znak = 0; fn_znak < 17 ; fn_znak ++) { cesta[fn_znak] = f_name_H[fn_znak]; }
            cesta[17] = '\0';
            delka_radky = 109;
          }

        if (typ == 'I')                          // soubor "CALB/seznam.txt"
          {            
            for (uint8_t fn_znak = 0; fn_znak < 17 ; fn_znak ++) { cesta[fn_znak] = f_name_I[fn_znak]; }
            cesta[15] = '\0';
            cesta[16] = '\0';
            cesta[17] = '\0';
          }

        if (typ == 'T')                          // zpracovani parametru dalekohledu ze souboru "tele_par.txt"
          {            
            teleread();                          // odskok do samostatneho podprogramu pro nastaveni menu DSO (vyber z parametru dalekohledu)
            return;                              //  vypadnuti z podporgramu "file_to_serkom()"
          }


        if (typ == '?')                          // priklad souboru "HELP/2_[.txt";
          {
            for (uint8_t fn_znak = 0; fn_znak < 12 ; fn_znak ++) { cesta[fn_znak] = f_name_Z[fn_znak]; }
            cesta[12] = '\0';
            cesta[13] = '\0';
            cesta[14] = '\0';
            cesta[15] = '\0';
            cesta[16] = '\0';
            delka_radky = 100;                                                  // pri vypisu obycejneho textoveho souboru, ktery se dal nezpracovava, na delce radky nezalezi
          }

        
        if (digitalRead(pin_karta_IN) == LOW)                                   // LOW = karta zasunuta, muze se provest pokus o cteni
          {
            bitClear(err_bit,2);                                                // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            if (typ != 'I')                                                     // pro typ 'I' se v prvnim kroku nacte obsah souboru "seznam.txt", ale ten se obvykle nemusi vypisovat
              {
                Serial.print(cesta);
              }

            //----------------------------------------------------------------------------------------------
            // prvni otevreni souboru je jen pro zjisteni poctu radek - je to rychle, nic se nevypisuje
            sd.begin(SD_CONFIG);                                                //inicializace SD karty pro pripad, ze by byla predtim vytazena

            open_OK = soubor.open(cesta, O_READ);                               // otevreni souboru vcetne slozky
              
            if (open_OK)
              {                
                pocet_radek = 0;
                while ((soubor.fgets(read_buffer, delka_radky)) > 0)            // cist znak po znaku, dokud soubor neco obsahuje
                  {
                    if (typ == 'A')                                             // pri vypisu syslogu se nepocitaji vsechny radky, ale jen ty, ktere odpovidaji zadane adrese
                      {
                        read_adresa = ((read_buffer[20] - 48) * 100) + ((read_buffer[21] - 48) * 10) + (read_buffer[22] - 48);
                        if (param3 == read_adresa or param3 == 1000)
                          {
                            pocet_radek ++;
                          }
                      }
                    else                                                        // pri ostatnich typech souboru se pocitaji vsechny radky
                      {
                        pocet_radek ++;  
                      }
                  
                  }

                if (typ == 'H' or typ == '?' or typ == 'T' )                        // u trasovaciho souboru, souboru pro nastaveni parametru dalekohledu a u napovedy se vypisuje vzdycky vsechno
                  {
                    param2 = pocet_radek;                                           // proto se parametr 2 nastavi na celkove zjisteny pocet radek
                  } 


                if (typ == 'H')                                                     // u trasovaciho souboru a u napovedy se misto poctu radek vypisuje pocet zaznamenanych bodu
                  {
                    param2 = pocet_radek;                                           // u trasovaciho souboru, se vypisuje vzdycky vsechno proto se parametr 2 nastavi na celkove zjisteny pocet radek
                    Serial.print("     (");
                    Serial.print(lng383);                                           // "Celkovy pocet GPS bodu v souboru: "
                    if (trackuj == true and  param3 == (EEPROM_read_int(eeaddr_GPS_track_id) & 0x3FFF))  // kdyz se vypisuje prave bezici soubor ...
                      {
                        pocet_radek = pocet_radek - 5;                              //  ... z celkoveho poctu se odecita 6 radek ze zacatku souboru
                      }
                    else                                   
                      {
                        pocet_radek = pocet_radek - 11;                             // ve vsech ostatnich pripadech se odecita 9 radek (5 na zacatku souboru, 6 na konci)                                
                      }
                    
                    Serial.print(pocet_radek);
                    Serial.println(')');
                  } 

                if (typ == '?' )                                                    // u napovedy se vypisuje vzdycky vsechno
                  {
                    param2 = pocet_radek;                                           // proto se parametr 2 nastavi na celkove zjisteny pocet radek
                    Serial.println(' ');                                            //   a pred vypisem se zadny pocet radek nezobrazuje (jen se odradkuje za jmenem souboru)
                  } 

                if (typ == 'B' )                                                    // soubory se zaznamy maji moc dlouhe radky (delsi nez 255 znaku) a to zpusobuje, ze se kazda nacita na 2x
                  {                                                                 //  tim se spocteny pocet radek zdvojnasobuje
                    pocet_radek = pocet_radek / 2;                                  //  Tak se tady natvrdo zase vydeli 2 a tim se ziska spravny pocet radek
                  }


            
                if (typ >= 'A' and typ <= 'G' )                                     // u ostatnich souboru se vypisuje pocet vsech radek v souboru
                  {
                    Serial.print(lng342);                                           //"     (pocet radek: "
                    Serial.print(pocet_radek);
                    Serial.println(')');            
                  }



                if (typ == 'I')
                  {
                    if (param3 != 9999)                                             // vypis seznamu kalibracnich souboru
                      {
                        char jmeno_CSV[18];                                         // jmeno CSV souboru vcetne cesty ma pevne danou delku
                        uint32_t fileSize = soubor.size();
                        uint16_t pocet_radek = fileSize / 19;                       // pocet radek 
                        soubor.seek((pocet_radek - param3) * 19);                   // nastaveni cteni na zacatek vypoctene radky
                        soubor.fgets(jmeno_CSV, 18);                                // nacist 18 znaku ze souboru do pole (cesta a jmeno pozadovaneho souboru)

                        for (uint8_t fn_znak = 0; fn_znak < 17 ; fn_znak ++) { cesta[fn_znak] = jmeno_CSV[fn_znak]; }
                      }
                    Serial.println(cesta);                                        // tady se vypise cesta (bud na soubor "seznam.txt" nebo na konkretni CSV soubor)

                  }

                soubor.close();

                Serial.println("-------------");                                    // oznaceni zacatku souboru


                //----------------------------------------------------------------------------------------------
                // druhe otevreni souboru uz zobrazuje radky v seriove lince, ale nejdriv si spocita, kolik radek se ma ignorovat

                sd.begin(SD_CONFIG);                                                // inicializace SD karty pro pripad, ze by byla predtim vytazena
        
                open_OK = soubor.open(cesta, O_READ);                               // otevreni souboru vcetne slozky
    
                if (open_OK)
                  {
                    uint32_t fileSize = soubor.size();
                    soubor.seek(fileSize-1);
                    posledni_znak = soubor.read();
                    soubor.seek(0);
                    
                    while ((soubor.fgets(read_buffer, delka_radky)) > 0)            // cist znak po znaku, dokud soubor neco obsahuje
                      {
                        if (digitalRead(pin_tl_ok) == false)                        // predcasne ukonceni dlouheho vypisu stiskem tlacitka [OK],
                          {
                            break;
                          }
        
                        if (typ == 'A')                                             // pri vypisu logovaciho souboru se porovnava adresa v bufferu [20] az [22] s pozadovanou adresou
                          {
                            read_adresa = ((read_buffer[20] - 48) * 100) + ((read_buffer[21] - 48) * 10) + (read_buffer[22] - 48);
                            if (param3 == read_adresa or param3 == 1000)    // vypisuji se jen shodne adresy, nebo pri adrese 1000 se vypise vsechno
                              {
                                pocet_radek --;                                     // celkovy pocet radek se v tomto cyklu zase odecita
                                if (pocet_radek < param2)                           // logy se tisknou jen v pripade, ze jich od konce souboru zbyva mene, nez pozadovany pocet logu
                                  {
                                    Serial.print(read_buffer);
                                    delay(1);                                       // pri vypisech se mezi radky dela kratka pauza
                                  }                         
                              }
                          }
    
                        if (typ == 'H' or typ == '?')                               // pri vypisu trackovaciho souboru se vypisuji vsechny radky, a muze se provadet i matematicka analyza souradnic
                          {                                                         //   (plati i pro vypis napovedy)
                            Serial.print(read_buffer);
                            delay(2);
                          }
    
                        if (typ == 'I' or typ == 'T')                               // pri vypisu kalibracnich souboru a nastavovacich parametru dalekohledu se vypisuje cely obsah
                          {
                            Serial.print(read_buffer);
                            delay(1);
                          }
    
    
                        if (typ > 'A' and typ < 'H')                                // u vsech ostatnich typu souboru ('B' az 'G') se jen vypisuji posledni radky 
                          {
                            pocet_radek --;                                         // celkovy pocet radek se v tomto cyklu zase odecita
                            if (pocet_radek < param2)                               // logy se tisknou jen v pripade, ze jich od konce souboru zbyva mene, nez pozadovany pocet logu
                              {
                                Serial.print(read_buffer);
                                delay(1 + (delka_radky / 100));                     // pri vypisech se mezi radky dela kratka pauza
                              }                         
    
                          }
                      }
                    soubor.close();
                  }
                else
                  {
                     Serial.println(lng320);                                         // "chyba pri otevirani souboru / soubor neexistuje"
                  }
              }
            else
              {
                Serial.println(lng320);                                              // "chyba pri otevirani souboru / soubor neexistuje"
              }

            if (posledni_znak != 10)     Serial.print('\n');                         // oznaceni konce vypisu - kdyz soubor nekonci odradkovanim, prida se pred podtrzeni jedno odradkovani
            Serial.println("-------------");
          }
        else                                                                         // neni zasunuta karta
          {
            Serial.println(lng320);                                                  // "chyba pri otevirani souboru / soubor neexistuje"
          }
      }
  }

//----------------------------------------------


//----------------------------------------------
// Obsah vybraneho HELP souboru vypise do seriove linky.
//   jmeno souboru vcetne cesty je v globalnim znakovem poli 'helpfile_name[]'  ("HELP/#_#.txt")
void helpfile(void)
  {    
    char read_buffer[300];                                                      // sem se bude nacitat obsah souboru a prubezne zpracovavat
    
    if (pouzivat_SD_kartu == true)
      {
        if (digitalRead(pin_karta_IN) == LOW)                                   // LOW = karta zasunuta, muze se provest pokus o cteni
          {
            bitClear(err_bit,2);                                                // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       

            Serial.println(helpfile_name);            
            Serial.println("-------------");                                    // oznaceni zacatku souboru

            sd.begin(SD_CONFIG);                                                // inicializace SD karty pro pripad, ze by byla predtim vytazena
            open_OK = soubor.open(helpfile_name, O_READ);                       //   otevreni souboru vcetne slozky

            if (open_OK)
              {
                while ((soubor.fgets(read_buffer, 100)) > 0)                    // cist znak po znaku, dokud soubor neco obsahuje
                  {
                    if (digitalRead(pin_tl_ok) == false)                        // predcasne ukonceni dlouheho vypisu stiskem tlacitka [OK],
                      {
                        break;
                      }
                    else                                                         
                      {
                        Serial.print(read_buffer);
                        delay(2);                                               // pri vypisech se mezi radky dela kratka pauza
                      }
                  }
                soubor.close();
              }
            else
              {
                if (helpfile_name[5] == 'm')                                    // na prikaz ?? se nejdriv pokusi vypsat soubor "/HELP/mmm.txt"
                  {                                                             //      kdyz neexistuje, nebo nejde otevrit vypisou se vsechny bezene napovedy najednou
                    ser_napoveda0();
                    ser_napoveda1();
                    ser_napoveda2();
                    ser_napoveda3();
                    ser_napoveda4();
                    ser_napoveda5();
                  }
                else
                  {
                    Serial.println(lng320);                                     // "chyba pri otevirani souboru / soubor neexistuje"                
                  }
              }
 
            Serial.println("-------------");                                    // oznaceni konce vypisu          
          }
        else
          {
            if (helpfile_name[5] == 'm')                                        // na prikaz ?? se nejdriv pokusi vypsat soubor "/HELP/mmm.txt"
              {                                                                 //      kdyz ale neni karta zasunuta, vypisou se vsechny bezene napovedy najednou
                ser_napoveda0();
                ser_napoveda1();
                ser_napoveda2();
                ser_napoveda3();
                ser_napoveda4();
                ser_napoveda5();
              }
            else
              {            
                Serial.print(lng136);                                           //    " SD karta: "
                Serial.println(lng138);                                         //    "vysunuta"
              }
          }
      }
  }
//----------------------------------------------



//----------------------------------------------
// do globalni promenne "GPS_track_cesta[]" se pripravi nazev souboru pro ukladani trasy
void zaloz_GPX_soubor(void)
  {   
    if (pouzivat_SD_kartu == true)                                              // kdyz je nastaveno pouzivani SD karty, tak se muze ukladat   
      {    
        zobraz_RTC(false); 
        uint16_t track_cislo = EEPROM_read_int(eeaddr_GPS_track_id) & 0x7FFF;   // precte posledni pouzity index GPX souboru
        track_cislo ++;                                                             // zvetsi ho o jednicku
        if (track_cislo > 9999)  track_cislo = 1;                                   //    pri prekroceni povoleneho rozsahu se priste bude zacinat od 1
        EEPROM_write_int(eeaddr_GPS_track_id , track_cislo);                        // a zapise ho zpatky (trasovani je i v EEPROM porad zastavene)

        nastav_cestu_tracku();                                                  // pripravi jmeno GPX souboru s novym indexem

        if (digitalRead(pin_karta_IN) == LOW)                                   // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                                // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                                  // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            sd.begin(SD_CONFIG);                                                //inicializace SD karty pro pripad, ze by byla predtim vytazena
            if (!sd.exists("TRCK"))   sd.mkdir("TRCK");                         // kdyz slozka "\TRCK\" neexistuje, tak se vytvori
            SdFile::dateTimeCallback(dateTime);
            open_OK = soubor.open(GPS_track_cesta,  O_WRITE | O_APPEND | O_CREAT); 

            if (open_OK)
              {
                bitClear(err_bit,1);                                            // SD karta je v poradku, maze se pripadny bit v promenne 'err_bit'       

                //     zapsat GPX hlavicku na zacatek souboru       
                soubor.println("<?xml version=\"1.0\" encoding=\"utf-8\"?>                                                                     ");
                soubor.println("<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" version=\"1.1\" creator=\"http://sqm.astromik.org/\">           ");
                soubor.println("<trk>                                                                                                      ");
                soubor.print("<name>SQM - ");
                for (uint8_t zn = 5 ; zn <= 16 ; zn++)
                  {
                    soubor.print(GPS_track_cesta[zn]);
                  }
                soubor.print(" - Local DateTime: ");
                soubor.print(LOC_rok);
                soubor.print('-');
                if(LOC_mes < 10)     soubor.print('0');
                soubor.print(LOC_mes);
                soubor.print('-');
                if(LOC_den < 10)     soubor.print('0');
                soubor.print(LOC_den);
                soubor.print(' ');
                if(LOC_hod < 10)     soubor.print('0');
                soubor.print(LOC_hod);
                soubor.print(':');
                if(LOC_min < 10)     soubor.print('0');
                soubor.print(LOC_min);
                soubor.print(':');
                if(LOC_sek < 10)     soubor.print('0');
                soubor.print(LOC_sek);              
                soubor.println("</name>                                      ");
                soubor.println("<trkseg>                                                                                                   ");

                soubor.close();
                uint16_t cislo_tracku = EEPROM_read_int(eeaddr_GPS_track_id) | 0x8000;   // nastavi nejvyssi bit (trasovani na spusteno)
                EEPROM_write_int(eeaddr_GPS_track_id , cislo_tracku);
                trackuj = true;
                posledni_zaznam_tracku = millis();
              }
            else
              {
                chyba(2);
              }                                                                 // SD-Err
          }
        else
          {
            chyba(3);                                                           // No-SD
          }
      }
    
  }
//----------------------------------------------



//----------------------------------------------
// do posledniho vytvoreneho trasovaciho GPX souboru prida jeden zaznam nezprumerovanych (aktualnich) souradnic
void track_GPS(void)
  {
    float sourad_print;
    
    if (pouzivat_SD_kartu == true)                                              // kdyz je nastaveno pouzivani SD karty, tak se muze ukladat   
      {
        if (digitalRead(pin_karta_IN) == LOW)                                   // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                                // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                                  // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            zobraz_RTC(false);
            gps(0);                                                             // pres I2C se pokusi stahnout aktualni souradnice z desky SQM-GPS
            if (GPS_alt_akt < 9500 and GPS_alt_akt > 0 )                        // k zapisu bodu dojde jen v pripade, ze jsou vyskova data v normalnich mezich. Kdyz jeste neni zafixovano, odesila se ve vysce hodnota 65535m n.m.
              {
                sd.begin(SD_CONFIG);                                            //inicializace SD karty pro pripad, ze by byla predtim vytazena
                SdFile::dateTimeCallback(dateTime);
                
                open_OK = soubor.open(GPS_track_cesta,  O_WRITE | O_APPEND | O_CREAT);        // dalsi souradnice se pridavaji na konec souboru
                if (open_OK)
                  {
                    bitClear(err_bit,1);                                        // SD karta je v poradku, maze se pripadny bit v promenne 'err_bit'       

                    // zapsat bod do GPX souboru ve spravnem formatu (cas je v UTC formatu).  Priklad: 
                    //              <trkpt lat="+49.444309" lon="+014.366644"><ele>+0495.000000</ele><time>2024-11-22T19:24:46Z</time></trkpt>

                    if (pole_GPS_I2C[9] < 99 and pole_GPS_I2C[9] > 0)           // jen test spravneho roku (test funkcni komunikace I2C) (roky 2001 az 2099)
                      {                                                         // pri nejake necekane chybe komunikace se mi stalo, ze se vratil triznakovy rok
                        soubor.print("<trkpt lat=\"");

                        if (GPS_lat_akt > 90000000UL)                           // severni sirka
                          {
                            sourad_print = (GPS_lat_akt-90000000UL) / 1000000.0;
                            soubor.print('+');
                          }
                        else                                                     // jizni sirka
                          {                            
                            sourad_print = GPS_lat_akt / 1000000.0;
                            soubor.print('-');
                          }

                        
                        if (sourad_print < 10)        soubor.print('0');        // uvodni nula pro sirku
                        soubor.print( sourad_print, 6);
                        soubor.print("\" lon=\"");

                        if (GPS_lon_akt > 180000000UL)                          // vychodni delka
                          {
                            sourad_print = (GPS_lon_akt - 180000000UL) / 1000000.0;
                            soubor.print('+');
                          }
                        else                                                    // zapadni delka
                          {
                            sourad_print = GPS_lon_akt / 1000000.0;
                            soubor.print('-');
                          }
                        
                        
                        if (sourad_print < 100)       soubor.print('0');        // uvodni nula pro delku
                        if (sourad_print <  10)       soubor.print('0');        // uvodni nula pro delku
                        soubor.print(sourad_print , 6);
                        soubor.print("\"><ele>");
                        sourad_print = GPS_alt_akt - 500;
                        if (sourad_print > 0)         soubor.print('+');
                        else                          soubor.print('-');
                        sourad_print = abs(sourad_print);
                        if (sourad_print < 1000)      soubor.print('0');        // uvodni nula pro vysku
                        if (sourad_print <  100)      soubor.print('0');        // uvodni nula pro vysku
                        if (sourad_print <   10)      soubor.print('0');        // uvodni nula pro vysku
    
                        soubor.print(sourad_print , 6);
                        soubor.print("</ele><time>20");
                        soubor.print(pole_GPS_I2C[9]);                          // UT rok
                        soubor.print('-');
                        if (pole_GPS_I2C[10] < 10) soubor.print('0');           // UT mesic
                        soubor.print(pole_GPS_I2C[10]);
                        soubor.print('-');
                        if (pole_GPS_I2C[11] < 10) soubor.print('0');           // UT den
                        soubor.print(pole_GPS_I2C[11]);
                        soubor.print('T');
                        if (pole_GPS_I2C[12] < 10) soubor.print('0');           // UT hodina
                        soubor.print(pole_GPS_I2C[12]);
                        soubor.print(':');
                        if (pole_GPS_I2C[13] < 10) soubor.print('0');           // UT minuta
                        soubor.print(pole_GPS_I2C[13]);
                        soubor.print(':');
                        if (pole_GPS_I2C[14] < 10) soubor.print('0');           // UT sekunda
                        soubor.print(pole_GPS_I2C[14]);
                        soubor.println("Z</time></trkpt>");                
                      }

                    soubor.close();
                    if (bitRead(signal_LED,5) == true)                          // kdyz je blikani pri trasovani povolene 
                      {
                        blik_G_fast();                                          //    1x se kratce blikne zelene
                      }
                    posledni_zaznam_tracku = millis();
                    
                  }
                else
                  {
                    chyba(2);
                  }                                                             // SD-Err
              }
            else                                                                // souradnice nejsou k dispozici
              {
                posledni_zaznam_tracku = millis() - (track_interval * 500);     // dalsi pokus o zapis prijde za polovicni dobu, nez je nastaven interval
              }
          }
        else
          {
            chyba(3);                                                           // No-SD
          }
      }                                                                         // kdyz neni SD karta povolena, nic se neloguje
  }
//----------------------------------------------



//----------------------------------------------
//provede ukonceni GPX souboru
void ukonci_GPX_soubor(void)
  {

    long read_akt = 0;                                                          // pro moznost prumerovani souradnic v GPX souboru se v prvnim pruchodu hledaji minima a maxima vsech tri souradnic
    long track_off_lon = 0;                                                     // z minima a maxima se spocte offset pro prumerovani
    long track_off_lat = 0; 
    long track_off_alt = 0; 
    
    long track_suma_lat = 0;                                                    // soucty upravenych souradnic
    long track_suma_lon = 0;
    long track_suma_alt = 0;
    
    long track_min_lon =  180000000;
    long track_max_lon =          0;
    long track_min_lat =   90000000;
    long track_max_lat =          0;
    long track_min_alt =       9999;
    long track_max_alt =       -500;

    float prum_lat;                                                             // finalni hodnoty prumeru, ktere se budou zapisovat do prumerovaciho waypoint
    float prum_lon;
    float prum_alt;

    uint16_t delka_radky = 0;
    long pocet_bodu = 0;
    
    if (pouzivat_SD_kartu == true)                                              // kdyz je nastaveno pouzivani SD karty, tak se muze ukladat   
      {    
        if (digitalRead(pin_karta_IN) == LOW)                                   // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                                // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                                  // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            zobraz_RTC(false);


// ------- pred ukoncenim GPS souboru se provede analyza GPS souradnic ------------------

//  ---  prvni otevreni souboru je jen pro zjisteni poctu radek a minimalnich a maximalnich hodnot v souradnicich ---
            sd.begin(SD_CONFIG);                                                //inicializace SD karty pro pripad, ze by byla predtim vytazena

            delka_radky = 109;
            open_OK = soubor.open(GPS_track_cesta, O_READ);                     // otevreni souboru vcetne slozky
                           
            if (open_OK)
              {
                pocet_bodu = 0;
                while ((soubor.fgets(read_buffer, delka_radky)) > 0)            // cist znak po znaku, dokud soubor neco obsahuje
                  {
                    if (read_buffer[12] == '+' or read_buffer[12] == '-')       // pracuje se jen s radky, ktere obsahuji souradnice
                      {
                        pocet_bodu ++;
                        read_akt = buf_to_long(12,15);                          // hodnota zemepisne sirky
                        if (read_akt > track_max_lat) track_max_lat = read_akt; 
                        if (read_akt < track_min_lat) track_min_lat = read_akt; 
                        read_akt = buf_to_long(29,33);                          // hodnota zemepisne delky
                        if (read_akt > track_max_lon) track_max_lon = read_akt; 
                        if (read_akt < track_min_lon) track_min_lon = read_akt; 
                        read_akt = buf_to_long(47,52)/1000000;                  // hodnota nadmorske vysky (tam se pocita v celych metrech bez desetinne casti)
                        if (read_akt > track_max_alt) track_max_alt = read_akt; 
                        if (read_akt < track_min_alt) track_min_alt = read_akt; 
                      }

                  
                  }
                soubor.close();

                track_off_lat = (track_max_lat + track_min_lat) / 2;            // tyhle hodnoty se budou v druhem pruchodu odecitat od kazde souradnice prd jejim scitanim do prumeru
                track_off_lon = (track_max_lon + track_min_lon) / 2;
                track_off_alt = (track_max_alt + track_min_alt) / 2;
              
              }
            else                                                                // chyba otevreni souboru ... 
              {
                chyba(2);                                                       // SD-Err
                return;                                                         // predcasny navrat z podprogramu bez ukonceni souboru a zastaveni trasovani
              }

//  ---  druhe otevreni souboru projde znova vsechny souradnice a pomoci odectu vypoctenych offsetu spocte prumery souradnic ---
            
            sd.begin(SD_CONFIG);                                                //inicializace SD karty pro pripad, ze by byla predtim vytazena
            open_OK = soubor.open(GPS_track_cesta, O_READ);                     // otevreni souboru vcetne slozky

            if (open_OK)
              {
                while ((soubor.fgets(read_buffer, delka_radky)) > 0)            // cist znak po znaku, dokud soubor neco obsahuje
                  {
                    if (read_buffer[12] == '+' or read_buffer[12] == '-')       // pracuje se jen s radky, ktere obsahuji souradnice
                      {
                        read_akt = buf_to_long(12,15) - track_off_lat;          // hodnota zemepisne sirky posunuta o offset
                        track_suma_lat = track_suma_lat + read_akt;
                        read_akt = buf_to_long(29,33) - track_off_lon;          // hodnota zemepisne delky posunuta o offset
                        track_suma_lon = track_suma_lon + read_akt;
                        read_akt = (buf_to_long(47,52)/1000000) - track_off_alt;   // hodnota nadmorske vysky posunuta o offset (tady se pocita s celymi metry bez desetinne casti)
                        track_suma_alt = track_suma_alt + read_akt;
                      }
                  }
                if (pocet_bodu > 0)
                  {
                    prum_lat = (track_off_lat + (track_suma_lat / (float)pocet_bodu)) / 1000000.0;
                    prum_lon = (track_off_lon + (track_suma_lon / (float)pocet_bodu)) / 1000000.0;
                    prum_alt = (track_off_alt + (track_suma_alt / (float)pocet_bodu));                
                  }
                else                     // kdyz neni ani jeden zapsany bod, neda se prumerovat (deleni nulou)
                  {
                    prum_lat = 0;
                    prum_lon = 0;
                    prum_alt = 0;                    
                  }

                soubor.close();
              }
            else                                                                // chyba otevreni souboru ... 
              {
                chyba(2);                                                       // SD-Err
                return;                                                         // predcasny navrat z podprogramu bez ukonceni souboru a zastaveni trasovani
              }
            
            
// ----------- treti otevreni GPX souboru uz jen zapise statistiky a zaverecne znacky

            sd.begin(SD_CONFIG);                                                //inicializace SD karty pro pripad, ze by byla predtim vytazena
            SdFile::dateTimeCallback(dateTime);
            
            open_OK = soubor.open(GPS_track_cesta,  O_WRITE | O_APPEND | O_CREAT);   // pridani dat na konec souboru
            if (open_OK)
              {
                bitClear(err_bit,1);                                            // SD karta je v poradku, maze se pripadny bit v promenne 'err_bit'       

                soubor.println("</trkseg>                                                                                                  ");
                soubor.println("</trk>                                                                                                     ");

//             zapis prumernych souradnic do GPX souboru jako waypoint (zapisuje az za blok <trk>....</trk>)
//             "<wpt   lat="+46.576388" lon="-008.892638"><ele>+0372.000000</ele><name> AvgPoint </name></wpt>                    "

                soubor.print("<wpt   lat=\""); 
                if (prum_lat < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_lat = abs(prum_lat);
                if (prum_lat < 10)    soubor.print('0');
                soubor.print(prum_lat,6); 

                soubor.print("\" lon=\""); 
                if (prum_lon < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_lon = abs(prum_lon);
                if (prum_lon < 100)   soubor.print('0');
                if (prum_lon <  10)   soubor.print('0');
                soubor.print(prum_lon,6); 
                
                
                soubor.print("\"><ele>"); 

                if (prum_alt < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_alt = abs(prum_alt);
                if (prum_alt < 1000)  soubor.print('0');
                if (prum_alt <  100)  soubor.print('0');
                if (prum_alt <   10)  soubor.print('0');
                soubor.print(prum_alt,6); 

                soubor.print("</ele><name>");
                soubor.print(" AvgPoint ");
                soubor.println("</name></wpt>             "); 
                

//             zapis strednich souradnic do GPX souboru jako waypoint (zapisuje az za blok <trk>....</trk>)
//             "<wpt   lat="+46.576388" lon="-008.892638"><ele>+0372.000000</ele><name> MidPoint </name></wpt>                    "

                prum_lat = track_off_lat / 1000000.0;           // jen vypujceny nazev promenne
                soubor.print("<wpt   lat=\""); 
                if (prum_lat < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_lat = abs(prum_lat);
                if (prum_lat < 10)    soubor.print('0');
                soubor.print(prum_lat,6); 

                prum_lon = track_off_lon / 1000000.0;           // jen vypujceny nazev promenne
                soubor.print("\" lon=\""); 
                if (prum_lon < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_lon = abs(prum_lon);
                if (prum_lon < 100)   soubor.print('0');
                if (prum_lon <  10)   soubor.print('0');
                soubor.print(prum_lon,6); 
                
                
                soubor.print("\"><ele>"); 
                prum_alt = track_off_alt / 1000000.0;           // jen vypujceny nazev promenne
                if (prum_alt < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_alt = abs(track_off_alt);
                if (prum_alt < 1000)  soubor.print('0');
                if (prum_alt <  100)  soubor.print('0');
                if (prum_alt <   10)  soubor.print('0');
                soubor.print(prum_alt,6); 

                soubor.print("</ele><name>");
                soubor.print(" MidPoint ");
                soubor.println("</name></wpt>             "); 
                

//zapis znacky <bounds> s minimalnimi a maximalnimi souradnicemi 
//               "<bounds minLat="+49.510000" maxLat="+49.610000" minLon="+014.000000" maxLon="+014.500000" />               "

                soubor.print("<bounds minLat=\""); 
                prum_lat = track_min_lat / 1000000.0;           // jen vypujceny nazev promenne
                if (prum_lat < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_lat = abs(prum_lat);
                if (prum_lat < 10)    soubor.print('0');
                soubor.print(prum_lat,6); 
                
                soubor.print("\" maxLat=\""); 
                prum_lat = track_max_lat / 1000000.0;           // jen vypujceny nazev promenne
                if (prum_lat < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_lat = abs(prum_lat);
                if (prum_lat <  10)   soubor.print('0');
                soubor.print(prum_lat,6); 

                soubor.print("\" minLon=\""); 
                prum_lon = track_min_lon / 1000000.0;           // jen vypujceny nazev promenne
                if (prum_lon < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_lon = abs(prum_lon);
                if (prum_lon < 100)   soubor.print('0');
                if (prum_lon <  10)   soubor.print('0');
                soubor.print(prum_lon,6); 
                
                soubor.print("\" maxLon=\""); 
                prum_lon = track_max_lon / 1000000.0;           // jen vypujceny nazev promenne
                if (prum_lon < 0)     soubor.print('-');
                else                  soubor.print('+');
                prum_lon = abs(prum_lon);
                if (prum_lon <  100)  soubor.print('0');
                if (prum_lon <   10)  soubor.print('0');
                soubor.print(prum_lon,6); 
                soubor.println("\" />               "); 


                soubor.println("</gpx>                                                                                                     "); 


//             zapis poznamky s poctem bodu
//             "<!--  pocet GPS bodu: 12345)     -->                                                              "

                soubor.print("<!--  ");
                soubor.print(lng360);                                       // "pocet GPS bodu: "                
                if (pocet_bodu < 10000) soubor.print(' ');
                if (pocet_bodu <  1000) soubor.print(' ');
                if (pocet_bodu <   100) soubor.print(' ');
                if (pocet_bodu <    10) soubor.print(' ');
                soubor.print(pocet_bodu);
                soubor.println("              -->                                                               ");
                
                soubor.print("<!-- Local DateTime: ");

                soubor.print(LOC_rok);
                soubor.print('-');
                if(LOC_mes < 10)     soubor.print('0');
                soubor.print(LOC_mes);
                soubor.print('-');
                if(LOC_den < 10)     soubor.print('0');
                soubor.print(LOC_den);
                soubor.print(' ');
                if(LOC_hod < 10)     soubor.print('0');
                soubor.print(LOC_hod);
                soubor.print(':');
                if(LOC_min < 10)     soubor.print('0');
                soubor.print(LOC_min);
                soubor.print(':');
                if(LOC_sek < 10)     soubor.print('0');
                soubor.print(LOC_sek);              
                soubor.println(" -->                                                               ");

                soubor.close();
              }
            else
              {
                chyba(2);                                                       // SD-Err
              }          
          }
        else
          {
            chyba(3);                                                           // No-SD
          }
      }

    uint16_t track_cislo = EEPROM_read_int(eeaddr_GPS_track_id) & 0x7FFF;   // smaze nejvyssi bit (trasovani zastaveno)
    EEPROM_write_int(eeaddr_GPS_track_id , track_cislo);                        // a ulozi nezmenene cislo ulozi zpatky do EEPROM
    trackuj = false;
  }
//----------------------------------------------



//----------------------------------------------
// podle cisla tracku v EEPROM pripravi jmeno GPX souboru
void nastav_cestu_tracku(void)
  {
    uint16_t cislo_tracku = EEPROM_read_int(eeaddr_GPS_track_id) & 0x7FFF;   // nejvyssi bit udava stav trasovani

     //  GPS_track_cesta[] = "TRCK/trck1234.gpx";

        GPS_track_cesta[9]    =  48 + ((cislo_tracku % 10000) / 1000);       // tisice
        GPS_track_cesta[10]   =  48 + ((cislo_tracku %  1000) /  100);       // stovky
        GPS_track_cesta[11]   =  48 + ((cislo_tracku %   100) /   10);       // desitky
        GPS_track_cesta[12]   =  48 + ((cislo_tracku %    10)       );       // jednotky
  }
//----------------------------------------------



//----------------------------------------------
// prevede textovou hodnotu ze zadane pozice v "read_buffer[]" na long hodnotu bez desetinne tecky
//   priklad "+49.444332" prevede na 49444332
//   pozor na prekroceni rozsahu ukazatele (read_buffer je dlouhy az 300 znaku, ale ukazatel na zacatek cisla muze byt maximalne 255 - 13)
//       Pro podprogramy pro vyhodnocovani GPS zaznamu nic nehrozi, tam je delka radky maximalne 108 znaku (bajtu)
long buf_to_long(uint8_t zacatek_cisla,uint8_t tecka)
  {
    char temp_buf[13];
    uint8_t cil_ukazatel = 0;
    for (uint8_t pozice = zacatek_cisla; pozice < zacatek_cisla + 12 ; pozice++)  // od zadane pozice se do 'temp_buf[]' prekopiruje 12 znaku vcetne uvodniho znamenka '+' nebo '-'
      {
        if (pozice != tecka)                                                   // pripadna desetinna tecka se preskoci
          {
            temp_buf[cil_ukazatel] = read_buffer[pozice];                      // ale vsechno ostatni (i s neplatnymi znaky na konci cisel) se kopiruje do 'temp_buf[]'
            cil_ukazatel ++;
          }
      }
     temp_buf[12] = 'X';                                                       // pro jistotu se posledni znak prepise necim, co neni mozne identifikovat jako cislo
    return atol(temp_buf);                                                     // na zaver se buffer, ktery obsahuje cislo vcetne znamenka ale bez tecky prevede na long
  }
//----------------------------------------------




//----------------------------------------------
// nacte soubor "tele_par.txt", vybere z nej vsechny hodnoty za rovnitkem a ulozi je do docasneho uint16_t pole
void teleread(void)
  {
    uint16_t param_pole[12];
    uint8_t par_ukazatel = 0;

    if (digitalRead(pin_karta_IN) == LOW)                                   // LOW = karta zasunuta, muze se provest pokus o cteni
      {
        bitClear(err_bit,2);                                                // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       

        sd.begin(SD_CONFIG);                                                //inicializace SD karty pro pripad, ze by byla predtim vytazena

        open_OK = soubor.open("tele_par.txt", O_READ);                      // otevreni souboru v korenovem adresari SD karty
          
        if (open_OK)                                                        // otevreni souboru probehlo v poradku
          {
            Serial.println("SD:\\tele_par.txt");
            Serial.println("-------------");                                // oznaceni zacatku souboru
            while (soubor.available() && par_ukazatel < 12)                 // az na ukazatel = 11 by se program dostat nemel, znamenalo by to nadbytecne rovnitko v souboru
              {
                char c = soubor.read();
                Serial.print(c);                                            // kazdy nacteny znak ze souboru se zaroven odesila do seriove linky
                if (c == '=')                                               // byl nalezn  znak '='
                  {
                    uint16_t hodnota = 0;
                    while (soubor.available())                              // preskocit nasledujici znaky, nez se vyskytne nejake cislo
                      {
                        c = soubor.read();
                        Serial.print(c);
                        if (c != ' ' && c != '\t') break;
                      }
          
                    while (c >= '0' && c <= '9')                            // nacist postupne cislo znak po znaku a poskladat z nej uint16_t vysledek jakykoliv jiny znak nez cislo ukoncuje skladani
                      {
                        hodnota = hodnota * 10 + (c - '0');
                        if (!soubor.available())   break; 
                        c = soubor.read();
                        Serial.print(c);
                      }
                    param_pole[par_ukazatel++] = hodnota;                   // ukazatel se posouva az po ulozeni hodnoty do pole
                  }
              }
            soubor.close();
            Serial.println("\n-------------");                              // oznaceni konce souboru

            bool nastav_defaulty = false;
        
            if (par_ukazatel != 11) nastav_defaulty = true;                 // kdyz nejaky parametr chybi nebo prebyva nastavi se defaultni parametry (po ulozeni na posledni index 10 se do ukazatele pricita jeste +1)
        
            for (uint8_t i=0;i<3;i++)                                          // testovani zadanych prumeru dalekohledu
              {
                if (param_pole[i] > 500  or param_pole[i] == 0)   nastav_defaulty = true;
              }
            for (uint8_t i=3;i<6;i++)                                          // testovani hlavnich ohniskovych delek dalekohledu
              {
                if (param_pole[i] > 5000 or param_pole[i] == 0)   nastav_defaulty = true;
              }
            for (uint8_t i=6;i<11;i++)                                         // testovani ohniskovych delek okularu
              {
                if (param_pole[i] > 255  or param_pole[i] == 0)   nastav_defaulty = true;
              }
        
            if (nastav_defaulty == true)                                    // kdyz je nejaka zjistena hodnota mimo povolene meze, nastavi se defaultni parametry
              {
                Serial.println(lng006);                                     // "Vstup mimo rozsah "
                Serial.println(lng031);                                     // "Nastaveny defaultni parametry"
                
// #doc#22
                param_pole[0] = 200;
                param_pole[1] = 50;
                param_pole[2] = 150;
                param_pole[3] = 1000;
                param_pole[4] = 400;
                param_pole[5] = 700;
                param_pole[6] = 24;
                param_pole[7] = 100;
                param_pole[8] = 16;
                param_pole[9] = 8;
                param_pole[10] = 5;
              }
        
            // zapis novych hodnot do EEPROM
                                                                                               //  defaultni parametry v pripade problemu se souborem "tele_par.txt"
            EEPROM_write_int(     eeaddr_DSO_prumery     ,     param_pole[0]);                 // 200 mm
            EEPROM_write_int(     eeaddr_DSO_prumery + 2 ,     param_pole[1]);                 //  50 mm
            EEPROM_write_int(     eeaddr_DSO_prumery + 4 ,     param_pole[2]);                 // 150 mm
        
            EEPROM_write_int(     eeaddr_DSO_ohniska     ,     param_pole[3]);                 // 1000 mm
            EEPROM_write_int(     eeaddr_DSO_ohniska + 2 ,     param_pole[4]);                 //  400 mm
            EEPROM_write_int(     eeaddr_DSO_ohniska + 4 ,     param_pole[5]);                 //  700 mm
        
            EEPROM_write(         eeaddr_DSO_okulary     ,     (uint8_t) param_pole[6]);       //  24 mm   - po resetu se tento okular navoli automaticky
            EEPROM_write(         eeaddr_DSO_okulary + 1 ,     (uint8_t) param_pole[7]);       // 100 mm
            EEPROM_write(         eeaddr_DSO_okulary + 2 ,     (uint8_t) param_pole[8]);       //  16 mm
            EEPROM_write(         eeaddr_DSO_okulary + 3 ,     (uint8_t) param_pole[9]);       //   8 mm
            EEPROM_write(         eeaddr_DSO_okulary + 4 ,     (uint8_t) param_pole[10]);      //   5 mm
            
            SOFT_RESET();                                                   // po zapisu do EEPROM se provede softwarovy reset, takze se do menu nactou upravene hodnoty
                    
          }
        else
          {
            Serial.println(lng292);                                         // "Problem s SD kartou"
            chyba(3);                                                       // napis "no-SD" na displeji (v tomto pripade to ale znamena problem s oteviranim souboru)
          }
      }
    else
      {
        bitSet(err_bit,2);                                                  // SD karta je vysunuta, nastavuje se bit v promenne 'err_bit' 
        Serial.println(lng293);                                             // "SD karta vysunuta"
        chyba(3);                                                           // napis "no-SD" na displeji
      }    
  }
//--------------------------------------------------------------


//--------------------------------------------------------------
// Rychle zjisteni volneho mista na SD karte v bajtech (rozeznava karty s MBR i bez MBR)
// zdroj: GEMINI AI
uint64_t SDfree(SdFat& sd)
  {
    if (digitalRead(pin_karta_IN) == LOW)                                          // LOW = karta zasunuta, muze se provest pokus o zjisteni prazdneho mista
      {
        bitClear(err_bit,2);                                                       // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
        uint8_t  buf[512];
        uint32_t freeCount   = 0xFFFFFFFF;
        uint32_t partStart   = 0;                                                  // vychozi hodnota pro pripad bez MBR
        uint16_t fsinfoOffset;
        uint32_t fsinfoSector;
        uint32_t totSec;
    
        // Krok 1: peecist sektor 0
        if (!sd.card()->readSector(0, buf))
          {
            return 0;                                                              // chyba cteni sektoru 0
          }
    
        // Detekce: je sektor 0 primo boot sektor FAT32?
        // Boot sektor FAT32 ma signaturu 0x55AA na offsetu 510 a retezec "FAT32" na offsetu 82
        bool isSuperFloppy = (buf[510] == 0x55 && buf[511] == 0xAA && buf[82]  == 'F'  && buf[83]  == 'A'  && buf[84]  == 'T');
    
        if (isSuperFloppy)                                                         // SuperFloppy format bez MBR
          {
            partStart = 0;                                                         // buf[] jiz obsahuje boot sektor, neni treba cist znovu
          }
        else                                                                       // format: MBR
          {
            partStart = (uint32_t)buf[454]
                      | (uint32_t)buf[455] << 8
                      | (uint32_t)buf[456] << 16
                      | (uint32_t)buf[457] << 24;
    
            // Krok 2: precist boot sektor oddilu
            if (!sd.card()->readSector(partStart, buf))
              {
                return 0;                                                          // chyba cteni boot sektoru
              }
          }

        totSec = (uint32_t)buf[32]                                                 // celkovy pocet sektoru na karte (vcetne FAT, Boot, FS....)
            | (uint32_t)buf[33] << 8
            | (uint32_t)buf[34] << 16
            | (uint32_t)buf[35] << 24;

        SDkapacita = (totSec >> 11);                                               // prevod na MB (predpoklada se nasobeni 512 bajtu na sektor (posun o 9 bitu vlevo) a nasledny bitovy posun vpravo o 20 bitu jako prevod z bajtu na MB, takze 20-9=11)

    
        // Krok 3: z boot sektoru zjistit polohu FSInfo
        fsinfoOffset = (uint16_t)buf[48] | (uint16_t)buf[49] << 8;
        fsinfoSector = partStart + fsinfoOffset;
    
        // Krok 4: precist FSInfo sektor
        if (!sd.card()->readSector(fsinfoSector, buf))
          {
            return 0;                                                              // chyba cteni FSInfo sektoru
          }
    
        // Overeni signatury FSInfo "RRaA"
        if (buf[0] == 0x52 && buf[1] == 0x52 && buf[2] == 0x61 && buf[3] == 0x41)
          {
            freeCount = (uint32_t)buf[488]                                        // pocet volnych sektoru na karte
                      | (uint32_t)buf[489] << 8
                      | (uint32_t)buf[490] << 16
                      | (uint32_t)buf[491] << 24;
          }
    
        if (freeCount == 0xFFFFFFFF)                                               // neplatny FSinfo sektor
          {
            return 0;                                                              // chyba FSinfo
          }
    
        uint32_t sektory = sd.vol()->sectorsPerCluster();
        return (uint64_t)freeCount * sektory * 512;
      }
    else
      {
        bitSet(err_bit,2);                                                  // SD karta je vysunuta, nastavuje se bit v promenne 'err_bit' 
      }         
  }
//--------------------------------------------------------------
