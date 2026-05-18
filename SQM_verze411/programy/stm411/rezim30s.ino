// Specialni rezim pro ukladani teploty, tlaku, vlhkosti a osvetleni kazdych 30 sekund na zvlastni pamet, ktera je pripojena na I2C
//================================================
//
// I2C adresa externi EEPROM musi byt 0x57
//  Pro pamet 24LC512 (64k x 8 bitu) jsou vsechny adresovaci vstupy v HIGH
//
// format externi EEPROM:
//  adr. 0  = 100 (spec. rezim 30s)
//  adr. 1  =  30 (pocet sekund mezi vzorky [10 - 250], nebo prednastavene delsi intervaly 5, 7, 10, 20, 30 min)
//  adr. 2  =   0 zachytny bod pro hledani volneho mista. Kazdych 100 zaznamu (1300 bajtu) se zvysi o 1. Rozsah je 0 az 50.
//  adr. 3  =   1 (0 = probiha mereni, 1 = mereni pozastaveno)
//  adr. 4  az  5 (velikost osazene pameti - prvni adresa za koncem pouzite pameti. Na tuto adresu se uz nic nezapisuje)
//  adr. 6  =     posledni vytvareny index souboru "r_30s_01.csv" (pri zapneni pocitadla 0 az 99 se nastavi na 255 a znamena to soubor "r_30s_xx.csv")       
//  adr. 7  az  9          .... REZERVA (3 bajty)
//--------------------
//       kazdy jeden zaznam (13 bajtu)
//  adr. 10, 11, 12, 13  ... cas v sekundach od 1.1.1970
//  adr. 14, 15          ... teplota
//  adr. 16, 17          ... vlhkost
//  adr. 18, 19          ... tlak
//  adr. 20, 21, 22      ... osvetleni v setinach [lx]
//--------------------


char vystupni_retezec30[] = "00044 ; 1631464959 ;12.09.2021;16:42:39;  Ne ;   +26,48  ;   056,21  ;  095953  ;    78123,12    ;\0";

uint8_t posledni_index_souboru;   // zaznamenava si do EEPROM, jaky soubor se naposled vytvarel. Je to pouzito pro vypis posledniho souboru do seriove linky

int16_t velke_sekundy [] = {300, 420, 600, 1200, 1800};           // pri zadani intervalu delsi nez 250 sekund se nastavuji minuty (5, 7, 10, 20, 30)

//----------------------------------------------
void rezim30s(void)
  {
    int16_t zaznamvsekundach;
    if (casovani30 < 251)
      {
        zaznamvsekundach = casovani30;    
      }
    else
      {
        zaznamvsekundach = velke_sekundy[casovani30 - 251];
      }

    
    if ( (millis()- posledni_zaznam30) > (zaznamvsekundach * 1000UL) and pauza30 == false)
      {

        uint8_t pompam4 = D_pamet[4];                                                    // zapamatuje se stav displeje pred interruptem, ve kterem muze dojit k jeho zhasuti
        uint8_t pompam3 = D_pamet[3];                                      
        uint8_t pompam2 = D_pamet[2];
        uint8_t pompam1 = D_pamet[1];
        uint8_t pompam0 = D_pamet[0];

            
        posledni_zaznam30 = millis();
        if (blokuj_znacku30 == false)                                                 // pokud neni zobrazene menu, problikavaji pri zaznamu na prvni sedmisegmentovce 3 vodorovne carky
          {
            D_pamet[4] = 73;                                                          // v okamziku mereni se na leve segmentovce na chvili rozsviti 3 vodorovne carky, ostatni zhasnou
            D_pamet[3] = 0;
            D_pamet[2] = 0;
            D_pamet[1] = 0;
            D_pamet[0] = 0;     
            aktualizuj_displej(0);                                                    // znacka specialniho rezimu se zobrazi nejnizsim moznym jasem (nezavisle na globalni promenne 'jas_displeje')
          }
        int_teplota = teplota(true);
        int_vlhkost = vlhkost();
        int_tlak = tlak();
        float float_luxy = luxometr_1x();

        int_luxy = (uint32_t)((float_luxy * 100.0) + 0.5);
        uint32_t tt = rtc_korekce();                                             // prevod casu z RTC na mistni cas vcetne autokorekce


   // jen debugovaci vypisy pri kazdem mereni
//        Serial.print("Teplota: ");
//        Serial.println(int_teplota);
//        Serial.print("Vlhkost: ");
//        Serial.println(int_vlhkost);
//        Serial.print("Tlak: ");
//        Serial.println(int_tlak);
//        Serial.print("Osvetleni: ");
//        Serial.println(int_luxy);
//        Serial.print("CAS (tt): ");
//        Serial.println(tt);

//        rtclock.breakTime(tt, mtt_EEPROM);                                          // z poctu sekund od 1.1.1970 se ziska citelny datum a cas
    
//        LOC_dvt = mtt_EEPROM.weekday + 1;
//        LOC_den = mtt_EEPROM.day;
//        LOC_mes = mtt_EEPROM.month;
//        LOC_rok = mtt_EEPROM.year+1970;
//        LOC_hod = mtt_EEPROM.hour;
//        LOC_min = mtt_EEPROM.minute;
//        LOC_sek = mtt_EEPROM.second;
//        Serial.print(dny[LOC_dvt]);
//        Serial.print(" - ");
//        Serial.print(LOC_den);
//        Serial.print('.');
//        Serial.print(LOC_mes);
//        Serial.print('.');
//        Serial.print(LOC_rok);
//        Serial.print(' ');
//        Serial.print(LOC_hod);
//        Serial.print(':');
//        Serial.print(LOC_min);
//        Serial.print(':');
//        Serial.println(LOC_sek);


      
        pole_EEPROM[0] =  ((uint32_t)tt >> 24) & 0xFF;                           // 4 bajty pro pocet sekund od 1.1.1970 (s korekci nepresnosti a pripadnym prevodem na letni cas)
        pole_EEPROM[1] =  ((uint32_t)tt >> 16) & 0xFF;
        pole_EEPROM[2] =  ((uint32_t)tt >>  8) & 0xFF;
        pole_EEPROM[3] =  ((uint32_t)tt      ) & 0xFF;  
        pole_EEPROM[4] = (int_teplota >> 8) & 0xFF;
        pole_EEPROM[5] = (int_teplota     ) & 0xFF;
        pole_EEPROM[6] = (int_vlhkost >> 8) & 0xFF;
        pole_EEPROM[7] = (int_vlhkost     ) & 0xFF;
        pole_EEPROM[8] = (int_tlak >> 8) & 0xFF;
        pole_EEPROM[9] = (int_tlak     ) & 0xFF;

        pole_EEPROM[10] = (int_luxy >> 16) & 0xFF;
        pole_EEPROM[11] = (int_luxy >>  8) & 0xFF;
        pole_EEPROM[12] = (int_luxy      ) & 0xFF;

       
        for (uint8_t i = 0 ; i< 13 ; i++)
          {
            EEPROM_write30(adrsave30 + i, pole_EEPROM[i]);
          }

        adrsave30 = adrsave30 + 13;                                                   // adresa pro dalsi zaznam se nastavi o 13 bajtu dal
        if (adrsave30 > max_EEPROM30)
          {
            SD_save30(true , false);                                                  // automaticka zaloha na SD kartu pri zaplneni cele EEPROM (v tomto pripade se neprepisuje 24 hod. soubor) bez vystupu do seriove linky
            adrsave30 = 10;                                                           // pri prekroceni rozsahu se pokracuje od zacatku 
          }
        if (((adrsave30-10) % 1300) == 0)                                             // kazdych 1300 bajtu (100 zaznamu) se aktualizuje zachytny bod pro dalsi hledani volneho mista 
          {
            uint8_t pomprom = ((adrsave30 - 10) / 1300);                                 // z posledni adresy 
            EEPROM_write30(2, pomprom);                                               // zachytny bod pro dalsi hledani volneho mista se nastavi na zacatek EEPROM
          }

      
        // kdyz je dalsi misto v EEPROM obsazene, tak se smaze
        if (EEPROM_read30(adrsave30    ) > 0)  EEPROM_write30(adrsave30    , 0);
        if (EEPROM_read30(adrsave30 + 1) > 0)  EEPROM_write30(adrsave30 + 1, 0);
        if (EEPROM_read30(adrsave30 + 2) > 0)  EEPROM_write30(adrsave30 + 2, 0);
        if (EEPROM_read30(adrsave30 + 3) > 0)  EEPROM_write30(adrsave30 + 3, 0);
        
        if (blokuj_znacku30 == false)
          {
            D_pamet[0] = pompam0;                                                         // po vykonani specialni funkce zobrazi puvodni stav displeje
            D_pamet[1] = pompam1;
            D_pamet[2] = pompam2;
            D_pamet[3] = pompam3;
            D_pamet[4] = pompam4;
            aktualizuj_displej();    
          }
                 
      }
  }
//----------------------------------------------



//----------------------------------------------
// Seriova komunikace s pridavnou pameti v rezimu 30s
// Do tohoto podprogramu se vstoupi v okamziku, kdy je prijat prvni znak '%'
// Dalsi nasledujici znaky jsou osetreny zde
void ser_kom30(void)
  {
    delay(10);
    char podfunkce = Serial.read();                                                   // nacteni druheho znaku za '%'
    uint16_t zadane_cislo = 0;

    if (podfunkce == USB_fce_4_01)                                                    // "%I" ... detailni informace
      {
        Serial.print(lng30_001);                                                      // Serial.print("Stav mereni: ");
        if (pauza30 == true)   Serial.println(lng30_004);                             // Serial.println("Pozastaveno");
        else                   Serial.println(lng30_005);                             // Serial.println("Spusteno");

        Serial.print(lng30_002);                                                      // Serial.print("Interval mezi merenim: ");
        if (casovani30 < 251)
          {
            Serial.print(casovani30);
            Serial.println(" [s]");        
          }
        else
          {
            Serial.print(velke_sekundy[casovani30 - 251] / 60);
            Serial.println(" [min]");        
          }
          
        Serial.print(lng30_006);                                                      // Serial.print("Zachytny bod pro hledani volneho mista: ");
        Serial.print(EEPROM_read30(2));
        Serial.println(" (x1300 [B])");

        Serial.print(lng30_007);                                                      // Serial.print("Adresa pro ulozeni dalsiho zaznamu: ");
        Serial.println(adrsave30);

        Serial.print(lng30_008);                                                      // Serial.print("Velikost pridavne EEPROM2: ");
        Serial.println(EEPROM_read_int30(4));           
      }

    if (podfunkce == USB_fce_4_02)                                                    // "%L" ... vypis do seriove linky
      {
        delay(50);
        if (Serial.available())                                                       // za %L je nejaky dalsi parametr
          {
            podfunkce = Serial.read();
          }
        else                                                                          // za %L uz zadny dalsi parametr neni, proto se vyhodnoti jako %Lh 24 (vypis za poslednich 24 hodin)
          {
            podfunkce = USB_fce_4_03;
            zadane_cislo = 24;
          }

        if (podfunkce == USB_fce_4_03)                                                // %Lh n .... vypis zaznamu za poslednich 'n' hodin
          {
            if (Serial.available())                                                   // za %Lh je nejaky dalsi parametr
              {
                zadane_cislo = Serial.parseInt();
              }
            uint32_t aktualni_sekundy = rtclock.getY2kEpoch();

            if (zadane_cislo > 0)
              {
                seriovy_vypis(aktualni_sekundy - (zadane_cislo * 3600UL));            // projde celou EEPROM a vypise vsechny zaznamy mladsi, nez zadany cas v hodinach            
              }
          }
          
        if (podfunkce == USB_fce_4_04)                                                // %La   .... vypis vsech zaznamu        
          {
            seriovy_vypis(0);                                                         // projde celou EEPROM a vypise vsechny zaznamy (krome tech, ktere jeste nejsou obsazeny)
          }

        if (podfunkce == USB_fce_4_05)                                                // %Lz n .... vypis poslednich 'n' zaznamu
          {
            if (Serial.available())                                                   // za %Lz je nejaky dalsi parametr
              {
                zadane_cislo = Serial.parseInt();
                if (zadane_cislo > ((max_EEPROM30 - 10)/13))                          // omezeni nesmyslne zadanych velkych cisel
                  {
                    zadane_cislo = ((max_EEPROM30 - 10)/13)+1;                
                  }
              }
            else                                                                      // %Lz bez dalsiho parametru vypise jen 1 posledni zaznam
              {
                zadane_cislo = 1;
              }

            uint16_t adr_eeprom = adrsave30;
            Serial.println(hlavicka30);
            while (zadane_cislo > 0)
              {
                zadane_cislo --;
                if (adr_eeprom > 22)
                  {
                    adr_eeprom = adr_eeprom - 13;                    
                  }
                else                                                                  // pri podlezeni pod nejnizsi zaznam se pokracuje od horniho konce EEPROM
                  {
                    adr_eeprom = max_EEPROM30-13;                                      
                  }

                if (((adr_eeprom - 10) % 13) == 0)                                    // kazdy desaty zaznam se aktualizuje displej
                  {
                    zobraz_cislo((zadane_cislo) / 13, 6 );                            // na displeji se bude postupne zobrazovat odpocet: "rd.655" az "rd.000"
                  }
    
                uint32_t read_cas = EEPROM_read_long30(adr_eeprom);

                if (read_cas > 0 and read_cas < 0xFFFFFFFFUL)                         // vypisuji se jen ty zaznamy, ktere maji spravny cas
                  {                    
                    priprav_radku(adr_eeprom);
                    Serial.println(vystupni_retezec30); 
                  }
                if (digitalRead(pin_tl_ok) == LOW) break;
              }

            zobraz_text(66);                                                          // smazani displeje
    
            while (digitalRead(pin_tl_ok) == LOW)                                     // cekani na uvolneni tlacitka
              {
                delay(50);
              }
            delay(50);
    
            Serial.println(lng30_003);                                                // "Konec vypisu"
          }

        if (podfunkce == USB_fce_4_07)                                                // "%Lb" ... bajtovy vypis cele pameti po 13 bajtech na radku (na zacatku je 5-mistna adresa)
          {
            Serial.println(hlavicka_E0);                                              // systemova hlavicka pro bajty 0 az 9
            Serial.print("     0 ; ");            
    
            for (uint8_t subbajt = 0 ; subbajt < 10; subbajt ++)
              {
                uint8_t hodnota = EEPROM_read30(subbajt);
                if (hodnota < 100) Serial.print(' ');
                if (hodnota <  10) Serial.print(' ');
                Serial.print(hodnota);
                Serial.print(" ; ");
              }
            Serial.println(" \r\n");
    
            Serial.println(hlavicka_E10);                                             // datova hlavicka pro bajty 10 az .....
            
            for (uint16_t adr_eeprom = 10; adr_eeprom < max_EEPROM30 ; adr_eeprom = adr_eeprom + 13)
              {
                if (((adr_eeprom - 10) % 130) == 0)                                   // kazdou stou adresu se aktualizuje displej
                  {
                    zobraz_cislo((max_EEPROM30 - adr_eeprom) / 130, 6);               // na displeji se bude postupne zobrazovat odpocet: "rd.655" az "rd.000"
                  }
    
                uint32_t read_cas = EEPROM_read_long30(adr_eeprom);
                if (read_cas > 0 and read_cas < 0xFFFFFFFFUL)                         // vypisuji se jen ty zaznamy, ktere maji spravny cas (preskakuji se zaznamy s nulovym casem a nezformatovana pamet)
                  {
                    if (adr_eeprom < 10000) Serial.print(' ');
                    if (adr_eeprom <  1000) Serial.print(' ');
                    if (adr_eeprom <   100) Serial.print(' ');
                    if (adr_eeprom <    10) Serial.print(' ');
                    Serial.print(" ");            
                    Serial.print(adr_eeprom);
                    Serial.print(" ; ");            
        
                    for (uint8_t subbajt = 0 ; subbajt < 13; subbajt ++)
                      {
                        uint8_t hodnota = EEPROM_read30(adr_eeprom + subbajt);
                        if (hodnota < 100) Serial.print(' ');
                        if (hodnota <  10) Serial.print(' ');
                        Serial.print(hodnota);
                        Serial.print(" ; ");
                      }
                    Serial.println(' ');
                  }
                if (digitalRead(pin_tl_ok) == LOW or Serial.available()) break;
              }
            while (digitalRead(pin_tl_ok) == LOW)
              {
                delay(50);
              }
            delay(50);
            while (Serial.available()) Serial.read();
            zobraz_text(66);                                                          // smazani displeje
          
          
          }
      
      }

    if (podfunkce == USB_fce_4_06)                                                    // "%S" ... ulozeni na SD kartu
      {

        SD_save30(false , true);                                                      // bargraf a infotexty se vypisuji do seriove linky
        Serial.println(lng30_003);                                                    // "Konec vypisu"
      }

    if (podfunkce == USB_fce_4_08)                                                    // "%P" ... Pozastavit mereni
      {
        pauza30 = true;
        EEPROM_write30(3, 1);
        Serial.println(lng30_009);                                                    // Serial.println("Mereni pozastaveno");
      }

    if (podfunkce == USB_fce_4_09)                                                    // "%R" ... Opetovne spusteni po zastaveni
      {
        pauza30 = false;
        EEPROM_write30(3, 0);
        Serial.println(lng30_010);                                                    // Serial.println("Mereni spusteno");
      }


    if (podfunkce == USB_fce_4_10)                                                    // "%C" ... Zmena nastaveni mericiho intervalu
      {
        int16_t pomprom;
        uint16_t zadany_cas = Serial.parseInt();
        if (zadany_cas < 10 or zadany_cas > 255)
          {
            Serial.print(lng006);                                                     // Serial.print("Mimo rozsah ");
            Serial.println(lng007);                                                   // Serial.println("(10 az 255)");            
          }
        else
          {
            Serial.print(lng30_011);                                                  // Serial.print("puvodni casovani: "); 
            pomprom = EEPROM_read30(1);
            if (pomprom < 251)
              {
                Serial.print(pomprom); 
                Serial.println(" [s]"); 
              }
            else
              {
                Serial.print(velke_sekundy[pomprom - 251] / 60); 
                Serial.println(" [min]"); 
              }

            EEPROM_write30(1, zadany_cas);
            casovani30 = zadany_cas;


            Serial.print(lng30_012);                                                  //  Serial.print("nove casovani: "); 
            pomprom = EEPROM_read30(1);
            if (pomprom < 251)
              {
                Serial.print(pomprom); 
                Serial.println(" [s]"); 
              }
            else
              {
                Serial.print(velke_sekundy[pomprom - 251] / 60); 
                Serial.println(" [min]"); 
              }

          }
      
      
      }


    if (podfunkce == USB_fce_4_11)                                                    // "%F" ...  FORMAT pridavne EEPROM
      {
        Serial.print("FORMAT ");
        bool errf = false;                                                            // chyba predcasne ukonceneho formatovani
        // test velikosti pameti
        EEPROM_write30(0, 100);                                                       // na adresu 0 se zapise typ rezimu (100 = r30s)
        EEPROM_write30(32768, 0);                                                     // pokud je osazena pamet jen 32kB, dojde k preteceni adresy 32768 na 0 a prepise se hodnota 100 na adrese nula nulou

        uint8_t pomprom = EEPROM_read30(0);                                              // pokud ale zustala na adrese 0 hodnota 100, jedna se o 64kB pamet
        if (pomprom == 100)
          {
            Serial.println("64kB");
            max_EEPROM30 = 65504;                                                     // po odecteni 10 (systemove parametry) musi byt delitelne delkou zaznamu (13)
//            Serial.println(lng30_001);                                                // "|---------------------------------------------------|"
            pomlcky(51);
          }
        else
          {
            Serial.println("32kB");
            max_EEPROM30 = 32705;                                                     // po odecteni 10 (systemove parametry) musi byt delitelne delkou zaznamu (13)
//            Serial.println(lng30_002);                                                // "|--------------------------|"
            pomlcky(26);
          }
        Serial.print('|');

        for (uint16_t adr_eeprom = 10; adr_eeprom < max_EEPROM30 ; adr_eeprom = adr_eeprom + 13)
          {
            EEPROM_write30(adr_eeprom    , 0);
            EEPROM_write30(adr_eeprom + 1, 0);
            EEPROM_write30(adr_eeprom + 2, 0);
            EEPROM_write30(adr_eeprom + 3, 0);

            if (((adr_eeprom-10) % 130) == 0)                                         // kazdou stou adresu se aktualizuje displej
              {
                zobraz_cislo((max_EEPROM30 - adr_eeprom) / 130, 5);                   // na displeji se bude postupne zobrazovat odpocet: "H.F.504" az "H.F.000"
              }
                          
            if ((adr_eeprom-10) % 1300 == 0)
              {
                Serial.print('*');
              }
            if (digitalRead(pin_tl_ok) == LOW)                                        // moznost preruseni formatovani tlacitkem OK
              {
                delay(50);
                uint32_t exit_znacka = millis();
                while (digitalRead(pin_tl_ok) == LOW)                                 // ceka se na uvolneni tlacitka OK
                  {
                    graf_natazeni(millis() - exit_znacka , 113);                      // zaroven se na displeji zobrazuje pomlckovy bargraf
                    delay(20);
                  }
                if (millis() - exit_znacka > 2000)                                    // kdyz stisk trval alespon 2 sekundy, smycka for se prerusi 
                  {
                    chyba(10);                                                        // "Err-F"
                    delay(1000);
                    errf = true;
                    break;              
                  }
              }
          }

        Serial.println('|');   
        if (errf == false)                                                            // kdyz nedoslo k predcasnemu ukonceni formatovani tlacitkem [OK]
          {
            EEPROM_write30(0, 100);                                                   // typ modulu (100 = r30s)
            EEPROM_write30(1,  30);                                                   // interval spousteni mereni [s]
            EEPROM_write30(2,   0);                                                   // zachytny bod pro hledani volneho mista se nastavi na zacatek EEPROM
            EEPROM_write30(3,   1);                                                   // po zformatovani je zaznam zastaveny
    
            EEPROM_write30(4,  (max_EEPROM30 >> 8));                                  // maximalni pouzitelna kapacita se zapisuje do EEPROM
            EEPROM_write30(5,  (max_EEPROM30 & 0xFF));                                // maximalni pouzitelna kapacita se zapisuje do EEPROM
    
            EEPROM_write30(6,   0);                                                   // index posledniho souboru
            
            EEPROM_write30(7, 255);                                                   // rezerva
            EEPROM_write30(8, 255);                                                   // rezerva
            EEPROM_write30(9, 255);                                                   // rezerva        
            adrsave30 = 10; 
            casovani30 = 30;        
            
            Serial.println(lng166);                                                   //  "EEPROM byla zformatovana."
            delay(1000);
            SOFT_RESET();            
          }
        else
          {
            Serial.println(lng30_013);                                                // Serial.println("chyba formatovani"); 
          }


      }

    if (podfunkce == USB_fce_4_12)                                                    // "%N" ... Nastaveni zaznamu na nultou pozici
      {
        EEPROM_write30(2,  0);                                                        // zachytny bod pro hledani volneho mista nastavi na zacatek EEPROM
        
        EEPROM_write30(10, 0);
        EEPROM_write30(11, 0);
        EEPROM_write30(12, 0);
        EEPROM_write30(13, 0);
        adrsave30 = 10;
        Serial.println(lng30_014);                                                    // Serial.println("Dalsi zaznam na adresu 10.");
      }  


    if (podfunkce == USB_fce_4_13)                                                    // "%W" ... primy zapis do pridavne EEPROM
      {
        uint32_t adresa = Serial.parseInt();
        long wdata = Serial.parseInt();
        adresa &= 0xFFFF;                                                             // ochrana pred prekrocenim adresy (max = 64kB)       
        Serial.print(lng359);                                                         //  Serial.print(" puvodni: EEPROM2[");
        Serial.print(adresa);
        Serial.print("] = ");
        Serial.print(EEPROM_read30(adresa));
        wdata &= 0xFF;                                                                // ochrana pred prekrocenim rozsahu dat (0 az 255)             
        EEPROM_write30(adresa, wdata);            

        Serial.print("   <-  ");
        Serial.println(EEPROM_read30(adresa));
      }  


    if (podfunkce == USB_fce_4_14)                                                    // "%X" ... prime cteni z pridavne EEPROM
      {
        uint32_t adresa = Serial.parseInt();
        adresa &= 0xFFFF;                                                             // ochrana pred prekrocenim adresy (max = 64kB)       
        uint8_t readbajt = EEPROM_read30(adresa);
        Serial.print("EEPROM2[");
        Serial.print(adresa);
        Serial.print("] = 0b");
        if (readbajt <  128) Serial.print('0');
        if (readbajt <   64) Serial.print('0');
        if (readbajt <   32) Serial.print('0');
        if (readbajt <   16) Serial.print('0');
        if (readbajt <    8) Serial.print('0');
        if (readbajt <    4) Serial.print('0');
        if (readbajt <    2) Serial.print('0');
        Serial.print(readbajt,BIN);
        Serial.print(" = ");
        Serial.println(readbajt);

        if (adresa < 131071)
          {
            Serial.print("MSB[");
            Serial.print(adresa);
            Serial.print("] + LSB[");
            Serial.print(adresa + 1);
            Serial.print("] = (uint16_t) ");
            Serial.println(EEPROM_read_int30(adresa));

          }

        
      }



    if (podfunkce == '?')                                                             // "%?" ... Napoveda pro specialni funkce
      {
        ser_napoveda4();
      }  
        
   while (Serial.available()) Serial.read();

  
  }
//----------------------------------------------


//----------------------------------------------
// projde vsechny zaznamy v EEPROM30 a vypise ty, u kterych cas prekracuje zadanou 'hranici'
void seriovy_vypis(uint16_t hranice)
  {
    Serial.println(hlavicka30);
    uint16_t celkovy_pocet_zaznamu = ((max_EEPROM30-10) / 13) + 1;

    uint16_t adr_eeprom = adrsave30;

    while (celkovy_pocet_zaznamu > 0)
      {
        if (((adr_eeprom-10) % 13) == 0)                                              // kazdy 13. zaznam se aktualizuje displej
          {
            zobraz_cislo((celkovy_pocet_zaznamu) / 13, 6);                            // na displeji se bude postupne zobrazovat odpocet: "rd.504" az "rd.000"
          }

        uint32_t read_cas = EEPROM_read_long30(adr_eeprom);
        if (read_cas > hranice and read_cas < 0xFFFFFFFFUL)                           // vypisuji se jen ty zaznamy, ktere maji spravny cas
          {                    
            priprav_radku(adr_eeprom);
            Serial.println(vystupni_retezec30); 
          }
        celkovy_pocet_zaznamu --;
        adr_eeprom = adr_eeprom + 13;
        if (adr_eeprom > max_EEPROM30) adr_eeprom = 10;                               // po dosazeni horniho konce EEPROM pokracuje od zacatku
        if (digitalRead(pin_tl_ok) == LOW) break;                                     // tlacitko OK muze okamzite prerusit vypis
      }

    while (digitalRead(pin_tl_ok) == LOW)
      {
        delay(50);
      }
    delay(50);

    zobraz_text(66);                                                                  // smazani displeje
    Serial.println(lng30_003);                                                        // "Konec vypisu"
  }




//----------------------------------------------
// nalezeni prvniho volneho mista v pridavne EEPROM
uint16_t najdi_volne_misto30(void)
  {
    uint16_t adr_eeprom;
    uint8_t zachytny_bod = EEPROM_read30(2);

    for (adr_eeprom = (zachytny_bod * 1300)+10 ; adr_eeprom < max_EEPROM30 ; adr_eeprom = adr_eeprom + 13)
      {
        if (EEPROM_read_long30(adr_eeprom) == 0) break;
      }

    if (adr_eeprom == max_EEPROM30)                                                   // cela pamet je zaplnena
      {
        adr_eeprom = 10;                                                              // adresa se nastavi na zacatek
        EEPROM_write30(2,0);                                                          // zachytny bod pro dalsi hledani se nastavi na zacatek
      }
      
    return adr_eeprom;
  }
//----------------------------------------------



//----------------------------------------------
// cteni 1 bajtu ze zasuvne EEPROM
uint8_t EEPROM_read30(uint16_t addr)
  {
    delayMicroseconds(7);
    Wire.beginTransmission(87);                                                       // zacatek komunikace s EEPROM
    Wire.write(uint8_t(addr >> 8));                                                      // MSB z adresy
    Wire.write(uint8_t(addr % 256));                                                     // LSB z adresy
    uint8_t err = Wire.endTransmission();                                                // konec komunikace
    if (err > 0) chyba(1);
    else         bitClear(err_bit,0);                                                         // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
    delayMicroseconds(2);
    Wire.requestFrom(87, 1);
    return Wire.read();
  }
//----------------------------------------------



//----------------------------------------------
// cteni 4 bajtu ze zasuvne EEPROM
uint32_t EEPROM_read_long30(uint16_t adresa)
  {
    uint8_t bajt3 = EEPROM_read30(adresa);                                               // MSB
    uint8_t bajt2 = EEPROM_read30(adresa + 1);
    uint8_t bajt1 = EEPROM_read30(adresa + 2);
    uint8_t bajt0 = EEPROM_read30(adresa + 3);                                           // LSB

    return   (((uint32_t)bajt3 << 24))  | (((uint32_t)bajt2 << 16)) | (((uint32_t)bajt1 << 8)) | ((uint32_t)bajt0);
  }


//----------------------------------------------
// cteni 3 bajtu ze zasuvne EEPROM
uint32_t EEPROM_read_halflong30(uint16_t adresa)
  {
    uint8_t bajt2 = EEPROM_read30(adresa);                                               // MSB
    uint8_t bajt1 = EEPROM_read30(adresa + 1);
    uint8_t bajt0 = EEPROM_read30(adresa + 2);                                           // LSB

    return   (((uint32_t)bajt2 << 16) | ((uint32_t)bajt1 << 8) | ((uint32_t)bajt0));
  }



//----------------------------------------------------------
// cteni 2 bajtu ze zasuvne EEPROM
uint16_t EEPROM_read_int30(uint16_t adresa)
  {
    uint8_t bajt1 = EEPROM_read30(adresa);                                               // MSB
    uint8_t bajt0 = EEPROM_read30(adresa + 1);                                           // LSB

    return   ((((uint32_t)bajt1 << 8)) | ((uint32_t)bajt0));
  }
//----------------------------------------------



//----------------------------------------------
// zapis jednoho bajtu do externi EEPROM
void EEPROM_write30(uint16_t adresa, uint8_t wr30data)
  {
    delay(5);
    Wire.beginTransmission(87);                                                       // zacatek komunikace s EEPROM
    Wire.write(uint8_t(adresa >> 8));                                                    // MSB z adresy
    Wire.write(uint8_t(adresa % 256));                                                   // LSB z adresy
    delay(1);
    Wire.write(wr30data);                                                             // ulozeni hodnoty
    uint8_t err = Wire.endTransmission();                                                // konec komunikace
    if (err > 0) chyba(1);
    else         bitClear(err_bit,0);                                                 // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
    delay(5);
  }
//----------------------------------------------



//----------------------------------
//  zapis dat na kartu do souboru (\rok\yyyymmdd.csv)
//  vcetne nastaveni datumu a casu posledniho zapisu souboru do FAT
//  pri "autozaloha" = true se vytvari jen jeden soubor se zalohou EEPROM
//  pri "autozaloha" = false se vytvari navic jeste soubor s daty za poslednich 24 hodin
//  pri ser_print = true se do USB seriove linky vypisuje jeste bargraf a nejake informacni texty
//                           pri spusteni funkce pres menu tlacitky se nic do seriove linky neodesila

void SD_save30(bool autozaloha , bool ser_print)
  {
    if (pouzivat_SD_kartu == true)
      {
        SdFile soubor_den;                                                                // zaznamy z posledniho dne se ukladaji do extra souboru
        bool open_OK1;
        bool open_OK2;
        
    
        uint32_t tt = rtc_korekce();                                                 // prevod casu z RTC na mistni cas vcetne autokorekce
     
        sek1970_datcas(tt,false);
    
    
    
        if (digitalRead(pin_karta_IN) == LOW)                                             // LOW = karta zasunuta, muze se provest pokus o zapis
          {
            bitClear(err_bit,2);                                                          // SD karta je zasunuta, maze se pripadny bit v promenne 'err_bit'       
            sd_out_err = true;                                                            // pri vytazeni karty a pokusu o pristi zapis se 1x zahlasi "no-Sd"
            sd.begin(SD_CONFIG);                                                          //inicializace SD karty pro pripad, ze by byla predtim vytazena
       
            char jmeno_souboru30[]    = "R30S/r_30s_00.csv";                              // retezec pro konstrukci jmena souboru (soubor "r_30_nn.csv" obsahuje vsechny data z EEPROM)
            char jmeno_souboru30den[] = "R30S/r_30_den.csv";                              // retezec pro konstrukci jmena souboru (soubor "r_30_den.csv" obsahuje jen data za poslednich 24 hodin)
            char slozka_r30s[] = "R30S";                                                  // retezec s nazvem slozky, do ktere se ukladaji zaznamy v rezimu 30s
        
            
            if (!sd.exists(slozka_r30s))   sd.mkdir(slozka_r30s);                         // kdyz slozka "\R30S\" neexistuje, tak se vytvori
           
    
            for (uint8_t i = 0 ; i <= 100 ; i++)                                             // pri kazdem spusteni funkce se pokousi vytvorit novy soubor se znackou 00 az 99
              {
                if (i < 100)
                  {
                    jmeno_souboru30[11] = (i / 10) + 48;
                    jmeno_souboru30[12] = (i % 10) + 48;
                    posledni_index_souboru = i;
                    if (!sd.exists(jmeno_souboru30)) break;                      
                  }
                else                                                                      // kdyz je ale vsech 99 souboru obsazenych, zapisuje vsechno do souboru "r_30s_xx.csv"
                  {
                    jmeno_souboru30[11] = 'x';
                    jmeno_souboru30[12] = 'x';
                    posledni_index_souboru = 255;
                  }
              }

            EEPROM_write30(6,   posledni_index_souboru);                                  // odlozi si do EEPROM index posledniho vytvoreneho souboru na SD karte                                                                                        
                                                                                          //          vyuziva se pri zobrazeni obsahu posledniho souboru pres seriovou linku
            SdFile::dateTimeCallback(dateTime);
            open_OK1 = soubor.open(jmeno_souboru30, O_WRITE| O_APPEND  | O_CREAT);
    
            if (autozaloha == false)
              {            
                open_OK2 = soubor_den.open(jmeno_souboru30den, O_WRITE | O_CREAT | O_TRUNC);       // stary soubor se v tomto pripade prepisuje (smaze a zapisuje se do prazdneho souboru)
              }
            else                                                                          // kdyz se provadi pouze automaticka zaloha EEPROM, 24hod soubor se nevytvari, ale musi se nastavit, ze vytvoreni probehlo v poradku
              {
                open_OK2 = true;
              }
    
            if (ser_print == true)
              {
                Serial.println(lng30_015);                                                // Serial.println("Ukladam zaznamy na SD kartu ... ");
                Serial.println(jmeno_souboru30);
        
                if (max_EEPROM30 == 32705)                                                // 32kB pamet
                  {
                    pomlcky(26);
                  }
                else                                                                      // 64kB pamet
                  {
                    pomlcky(51);         
                  }
              
                Serial.print('|');                                                        // zacatek bargrafu
              }
              
            if ((open_OK1 == true) and (open_OK2 == true))
              {
                bitClear(err_bit,1);                                                      // SD karta je v poradku, maze se pripadny bit v promenne 'err_bit'       
                soubor.println(hlavicka30);
    
                if (autozaloha == false)      soubor_den.println(hlavicka30);             // pri automaticke zaloze se 24hod soubor neprepisuje
    
                for (uint16_t adr_eeprom = 10; adr_eeprom < max_EEPROM30 ; adr_eeprom = adr_eeprom + 13)
                  {
    
                    if (((adr_eeprom-10) % 130) == 0)                                     // kazdou stou adresu se aktualizuje displej
                      {
                        if (autozaloha == false)                                          // pri automaticke zaloze po zaplneni cele EEPROM zustava displej beze zmeny
                          {
                            zobraz_cislo((max_EEPROM30 - adr_eeprom) / 130, 6);           // pri vyzadane zaloze se na na displeji bude postupne zobrazovat odpocet: "rd.504" az "rd.000"                    
                          }
                      }

                    if (ser_print == true)
                      {
                        if ((adr_eeprom-10) % 1300 == 0)        Serial.print('*');        // hvezdicka do bargrafu
                      }
    
                    uint32_t read_cas = EEPROM_read_long30(adr_eeprom);
                    if (read_cas > 0 and read_cas < 0xFFFFFFFFUL)                         // na kartu se vypisuji jen ty zaznamy, ktere maji spravny cas
                      {
                        priprav_radku(adr_eeprom);
                        soubor.println(vystupni_retezec30); 
                        if (autozaloha == false)                                          // pri automaticke zaloze se 24hod soubor neprepisuje
                          {
                            if ((tt - read_cas) < 86400)                                  // do denniho souboru se ukladaji jen hodnoty, ktere nemaji cas starsi nez 24 hodin
                              {
                                soubor_den.println(vystupni_retezec30); 
                              }                    
                          }
                      
                      }
                    if (digitalRead(pin_tl_ok) == LOW) break;
    
                  }
                
                delay(20);
                soubor.close();
                if (autozaloha == false)     soubor_den.close();                          // pri automaticke zaloze se 24hod soubor nevytvoril, proto se ani nezavira
                if (ser_print == true)
                  {
                    Serial.println('|');
                    Serial.println(lng30_016);                                            // Serial.println("ULOZENO");
                  }
                if (autozaloha == false)     zobraz_text(66);                             // smazani displeje se provadi jen pri vynucene zaloze (pri autozaloze zustava displej beze zmeny)
              }    
            else
              {
                chyba(2);
                if (ser_print == true)
                  {
                    Serial.println(lng30_017);                                            // Serial.println("funkce 'soubor.open()' skoncila chybou");
                  }
              }        
          }
        else                                                                              // karta je vysunuta, nic se nezapisuje
          {
            chyba(3);
            if (ser_print == true)
              {
                Serial.print(lng136);                                                     //  " SD karta: "
                Serial.println(lng138);                                                   //  "vysunuta"
              }
          }
      }
    else
      {    
        chyba(3);                                                                         // "no SD"
        if (ser_print == true)
          {
            Serial.print(lng136);                                                         //  " SD karta: "
            Serial.println(lng139);                                                       //  "nepouziva se"
          }
      }
  }
    
//----------------------------------------------

void priprav_radku(uint16_t adr_eeprom)
  {
      int16_t LOC_hod_30, LOC_den_30, LOC_mes_30, LOC_min_30, LOC_sek_30;                 // promenne, ktere obsahuji aktualni cas prepocteny z ulozenych sekund v EEPROM
      uint16_t LOC_rok_30 ;                                                       // rok (od 1970)
      uint8_t LOC_dvt_30;                                                                // den v tydnu (po ... ne)

      uint32_t read_cas = EEPROM_read_long30(adr_eeprom);

      int_teplota = EEPROM_read_int30(adr_eeprom+4);
      int_vlhkost = EEPROM_read_int30(adr_eeprom+6);
      int_tlak    = EEPROM_read_int30(adr_eeprom+8);
      int_luxy    = EEPROM_read_halflong30(adr_eeprom+10);
    
      breakTimeFCE(read_cas, mtt_EEPROM);                                             // z poctu sekund od 1.1.1970 se ziska citelny datum a cas
  
      LOC_dvt_30 = mtt_EEPROM.weekday + 1;
      LOC_den_30 = mtt_EEPROM.day;
      LOC_mes_30 = mtt_EEPROM.month;
      LOC_rok_30 = mtt_EEPROM.year+1970;
      LOC_hod_30 = mtt_EEPROM.hour;
      LOC_min_30 = mtt_EEPROM.minute;
      LOC_sek_30 = mtt_EEPROM.second;

      formatuj_ulong30((adr_eeprom /13)  ,  4,    5);                                 // index polozky v EEPROM
      formatuj_ulong30(read_cas          ,  17,  10);                                 // cas v sekundach od 1.1.1970

      formatuj_ulong30(LOC_den_30      ,  21,  2);                                    // citelny den
      formatuj_ulong30(LOC_mes_30      ,  24,  2);                                    // citelny mesic
      formatuj_ulong30(LOC_rok_30      ,  29,  4);                                    // citelny rok
  
      formatuj_ulong30(LOC_hod_30      ,  32,  2);                                    // citelna hodina
      formatuj_ulong30(LOC_min_30      ,  35,  2);                                    // citelna minuta
      formatuj_ulong30(LOC_sek_30      ,  38,  2);                                    // citelna sekunda
       
      formatuj_string30(dny[LOC_dvt_30],  42,  2);                                    // dva znaky pro den v tydnu

      formatuj_float30(citelna_teplota(int_teplota)  ,  49, 51, 2 , 2);               // teplota v citelnem desetinnem float formatu
      formatuj_float30(citelna_vlhkost(int_vlhkost)  , 255, 63, 3 , 2);               // vlhkost v citelnem desetinnem float formatu
      formatuj_ulong30(citelny_tlak(int_tlak)        ,  77,  6);                      // citelny tlak

      int16_t cely_luxy = int_luxy / 100;                                                 // desetinna cisla v luxech se nezobrazuji jako floaty, ale jako cela cast a desetinna cast zvlast
      int16_t zbyly_setiny = int_luxy - (cely_luxy * 100);
      formatuj_ulong30(cely_luxy        , 89, 6 );                                    // cela cast luxu
      formatuj_ulong30(zbyly_setiny     , 92, 2 );                                    // desetinna cast luxu


      
      vystupni_retezec30[22] = '.';                                                   // tecka za dnem
      vystupni_retezec30[25] = '.';                                                   // tecka za mesicem
      vystupni_retezec30[33] = ':';                                                   // dvojtecka za hodinou
      vystupni_retezec30[36] = ':';                                                   // dvojtecka za minutou

      vystupni_retezec30[6]  = ';'; 
      vystupni_retezec30[19] = ';'; 
      vystupni_retezec30[30] = ';'; 
      vystupni_retezec30[39] = ';'; 
      vystupni_retezec30[45] = ';'; 
      vystupni_retezec30[57] = ';'; 
      vystupni_retezec30[69] = ';'; 
      vystupni_retezec30[80] = ';'; 
      vystupni_retezec30[97] = ';'; 

// #doc#18
      vystupni_retezec30[52] = ',';                                                   // desetinny oddelovac v teplote
      vystupni_retezec30[64] = ',';                                                   // desetinny oddelovac ve vlhkosti
      vystupni_retezec30[90] = ',';                                                   // desetinny oddelovac v osvetleni

      vystupni_retezec30[5]  = ' ';                                                   // vypisy cisel konci znakem '\0' a proto se musi prepsat na mezery
      vystupni_retezec30[18] = ' ';
      vystupni_retezec30[44] = ' ';
      vystupni_retezec30[55] = ' '; 
      vystupni_retezec30[67] = ' ';                                                    // pro vlhkost se musi prepisovat obe zaverecne pozice
      vystupni_retezec30[68] = ' ';                                                    //       (problem s formatovanim cisla 0)
      vystupni_retezec30[78] = ' '; 
      vystupni_retezec30[93] = ' '; 
  }
//----------------------------------------------



//----------------------------------------------
// formatovani FLOAT hodnoty
// cislo                ... cislo ke zkopirovani do vystupniho retezce
// index_znamenka       ... index znaku ve vystupnim retezci, kam se bude zapisovat znamenko (pri 255 se znamenko nezobrazuje - bude to vzdycky kladne cislo)
// index_jednotek       ... index znaku ve vystupnim retezci, kam se budou zapisovat jednotky (nasledovane desetinnym oddelovacem)
// pocet_celych_mist    ... do kolika radu se budou pripadne doplnovat nuly (pripraveno az na 3 rady, ale ve skutecnosti se budou pouzivat maximalne 2)
// pocet_desetin        ... kolik bude mist za desetinnym oddelovacem
void formatuj_float30(float cislo , uint8_t index_znamenka,  uint8_t index_jednotek, uint8_t pocet_celych_mist , uint8_t pocet_desetin)
  {
    if (cislo < 0)
      {
        vystupni_retezec30[index_znamenka] = '-';
        cislo = -cislo;
      }
    else
      {
        if (index_znamenka < 255)   vystupni_retezec30[index_znamenka] = '+';
      }

    uint8_t zacatek = index_jednotek - pocet_celych_mist + 1;


    if (pocet_celych_mist > 5)
      {
        if (cislo < 100000UL)
          {
            vystupni_retezec30[index_jednotek - 5] = '0';
            zacatek ++;       
          }
      }


    if (pocet_celych_mist > 4)
      {
        if (cislo < 10000)
          {
            vystupni_retezec30[index_jednotek - 4] = '0';
            zacatek ++;       
          }
      }


    if (pocet_celych_mist > 3)
      {
        if (cislo < 1000)
          {
            vystupni_retezec30[index_jednotek - 3] = '0';
            zacatek ++;       
          }
      }

    if (pocet_celych_mist > 2)
      {
        if (cislo < 100)
          {
            vystupni_retezec30[index_jednotek - 2] = '0';
            zacatek ++;       
          }
      }

    if (pocet_celych_mist > 1)
      {
        if (cislo < 10)
          {
            vystupni_retezec30[index_jednotek - 1] = '0';
            zacatek ++;       
          }
      }

    
    String pomprom = String(cislo , pocet_desetin);

    for (uint8_t i = zacatek ; i < zacatek + pocet_celych_mist + pocet_desetin + 1 ; i++)
      {
        vystupni_retezec30[i] = pomprom.charAt(i-zacatek);
      }
  }
//----------------------------------------------



//----------------------------------------------
// formatovani STRING textu
// retezec        ... String, ktery se ma nakopirovat od prislusne pozice
// index_zacatek  ... index znaku ve vystupnim retezci, kam se nakopiruje prvni znak Stingu
// pocet_znaku    ... kolik znaku ze Stringu se ma zkopirovat do vystupniho retezce
//                       (bez kontroly, ze vstupni String obsahuje potrebny pocet znaku)
void formatuj_string30(String retezec , uint8_t index_zacatek, uint8_t pocet_znaku)
  {
    for (uint8_t i = index_zacatek ; i < index_zacatek + pocet_znaku + 1 ; i++)
      {
        vystupni_retezec30[i] = retezec.charAt(i - index_zacatek);
      }    
  }
//----------------------------------------------



//----------------------------------------------
// formatovani uint32_t hodnoty
// cislo          ... cislo, ktere se ma zformatovat  (Nebude nikdy zaporne, rozsah je 0 az 4,2 miliardy)
// start_jednotek ... index znaku ve vystupnim retezci, kam se budou zapisovat jednotky (nejnizsi rad)
// pocet_mist     ... kolik znaku se zobrazi (priklad: pro cislo 0 az 9999 je pocet mist 4) kdyz je cislo mensi, nez pocet_mist, doplni se zleva nulami
//                     (bez ochrany, ze je cislo vetsi, nez pocet mist - to se proste nestane)
void formatuj_ulong30(uint32_t cislo , uint8_t start_jednotek, uint8_t pocet_mist)
  {
    if (pocet_mist > 9)
      {
        if (cislo < 1000000000UL) vystupni_retezec30[start_jednotek - 9]   = '0';
        else
          {
            vystupni_retezec30[start_jednotek - 9] = (cislo / 1000000000UL) + 48;
            cislo = cislo % 1000000000UL;
          }
      }

    if (pocet_mist > 8)
      {
        if (cislo < 100000000UL) vystupni_retezec30[start_jednotek - 8]   = '0';
        else
          {
            vystupni_retezec30[start_jednotek - 8] = (cislo / 100000000UL) + 48;
            cislo = cislo % 100000000UL;
          }
      }

    if (pocet_mist > 7)
      {
        if (cislo < 10000000UL) vystupni_retezec30[start_jednotek - 7]   = '0';
        else
          {
            vystupni_retezec30[start_jednotek - 7] = (cislo / 10000000UL) + 48;
            cislo = cislo % 10000000UL;
          }
      }

    if (pocet_mist > 6)
      {
        if (cislo < 1000000UL) vystupni_retezec30[start_jednotek - 6]   = '0';
        else
          {
            vystupni_retezec30[start_jednotek - 6] = (cislo / 1000000UL) + 48;
            cislo = cislo % 1000000UL;
          }
      }

    if (pocet_mist > 5)
      {
        if (cislo < 100000UL) vystupni_retezec30[start_jednotek - 5]   = '0';
        else
          {
            vystupni_retezec30[start_jednotek - 5] = (cislo / 100000UL) + 48;
            cislo = cislo % 100000UL;
          }
      }
    
    if (pocet_mist > 4)
      {
        if (cislo < 10000) vystupni_retezec30[start_jednotek - 4]   = '0';
        else
          {
            vystupni_retezec30[start_jednotek - 4] = (cislo / 10000) + 48;
            cislo = cislo % 10000;
          }
      }

    if (pocet_mist > 3)
      {
        if (cislo < 1000) vystupni_retezec30[start_jednotek - 3]   = '0';
        else
          {
            vystupni_retezec30[start_jednotek - 3] = (cislo / 1000) + 48;
            cislo = cislo % 1000;
          }
      }

    if (pocet_mist > 2)
      {
        if (cislo <  100) vystupni_retezec30[start_jednotek - 2]   = '0';
        else
          {
            vystupni_retezec30[start_jednotek - 2] = (cislo / 100) + 48;
            cislo = cislo % 100;
          }
      }

    if (pocet_mist > 1)
      {    
        if (cislo <   10) vystupni_retezec30[start_jednotek - 1]  = '0';
        else
          {
            vystupni_retezec30[start_jednotek - 1] = (cislo / 10) + 48;
            cislo = cislo % 10;
          }
      }

    vystupni_retezec30[start_jednotek] = cislo + 48;
  }
//----------------------------------------------



//----------------------------------------------
// seriova napoveda pro specialni rezimy (r30s)
void ser_napoveda4(void)
  {
    Serial.println(lng30_018);                             // Serial.println ("%I     ... detailni informace");
    Serial.println(lng30_019);                             // Serial.println ("%L     ... vypis poslednich 24 hod. do seriove linky");
    Serial.println(lng30_020);                             // Serial.println ("%Lh n  ... poslednich 'n' hod. do seriove linky");
    Serial.println(lng30_021);                             // Serial.println ("%La    ... kompletni vypis do seriove linky");
    Serial.println(lng30_022);                             // Serial.println ("%Lz n  ... poslednich 'n' zaznamu do seriove linky");
    Serial.println(lng30_024);                             // Serial.println ("%Lb    ... bajtovy vypis");
    Serial.println(lng30_023);                             // Serial.println ("%S     ... ulozeni na SD kartu");
    Serial.println(lng30_025);                             // Serial.println ("%P     ... pozastavit mereni");
    Serial.println(lng30_026);                             // Serial.println ("%R     ... pokracovat v mereni");
    Serial.println(lng30_027);                             // Serial.println ("%C n   ... nastaveni mericiho intervalu (10-255)");
    Serial.println(lng30_028);                             // Serial.println ("%F     ... formatovani pridavne EEPROM");
    Serial.println(lng30_029);                             // Serial.println ("%N     ... nastaveni na nulovou pozici");
    Serial.println(lng30_031);                             // Serial.println ("%W a,d ... zapis hodnoty [d] na adresu [a] v EEPROM2");
    Serial.println(lng30_032);                             // Serial.println ("%X a   ... cteni hodnoty z adresy [a] z EEPROM2");
    Serial.println(lng30_030);                             // Serial.println ("%?     ... tato napoveda");

  
  }
//----------------------------------------------



//----------------------------------------------
// podprogramy ktere se vykonaji vstupem do prislusne polozky menu
void spec_dis_menu(uint8_t polozka)
  {
    int16_t pomprom = 0;

    zobraz_text(68);                                                                  // "-----"

    while (digitalRead(pin_tl_ok) == LOW)                                             // do podmenu se vstoupilo dlouhym stiskem tlacitka OK, ted je nutne pockat na jeho uvolneni
      {
        delay(50);                                  
      }
    delay(100);

    if (polozka == 15)                                                                // "#SPEC" zapinani a vypinani funkce mereni
      {
        if (pauza30 == true)   zobraz_text(44);                                       // aktualne je mereni pozastaveno  "#-oFF"
        else                   zobraz_text(43);                                       // aktualne je mereni povoleno     "#-on "
        while (digitalRead(pin_tl_ok) == HIGH)                                        // dokud se nestiskne tlacitko OK muze se hodnota prepinat tlacitky nahoru a dolu
          {
            if (digitalRead(pin_tl_up) == LOW)
              {
                zobraz_text(43);                                                      // mereni povolit     "#-on "
                pomprom = 1;
              }

            if (digitalRead(pin_tl_dn) == LOW)
              {
                zobraz_text(44);                                                      // mereni zastavit     "#-oFF"
                pomprom = 0;
              }
            delay(50);
            odskok_int();                                                             // otestovani pripadnych interruptu (komunikace, tlacitko TS ....)
          }

        zobraz_text(38);                                                              // "5AVE"
        while (digitalRead(pin_tl_ok) == LOW) delay(50);                              // ukonceni nastavovani stiskem OK. Ceka se na uvolneni tlacitka OK
        delay(150);

        if (pomprom == 1 and pauza30 == true)                                         // puvodne bylo mereni pozastaveno, ale nove se ma spustit
          {
            pauza30 = false;
            EEPROM_write30(3, 0);
          }

        if (pomprom == 0 and pauza30 == false)                                        // puvodne bylo mereni spusteno, ale nove se ma pozastavit
          {
            pauza30 = true;
            EEPROM_write30(3, 1);
          }
        
      }

    if (polozka == 16)                                                                // "# int" nastaveni intervalu mezi merenim
      {
        pomprom = casovani30;                                                         // protoze se do EEPROM bude ukladat jen pripadna zmena hodnoty, musi se puvodni hodnota zapamatovat
        casovani30 = plusminus('X', casovani30 , 10 , 255);
        

        while (digitalRead(pin_tl_ok) == LOW) delay(50);                              // ukonceni nastavovani stiskem OK. Ceka se na uvolneni tlacitka OK 
        delay(100);

        if (casovani30 != pomprom)                                                    // nova hodnota se uklada jen v pripade, ze je rozdilna od te puvodni
          {
            zobraz_text(38);                                                          // "5AVE"
            EEPROM_write30(1, casovani30);
            delay(200);
          }
      }
  

    if (polozka == 21)                                                                // "#CoPy" kopirovani EEPROM na SD kartu
      {
        SD_save30(false , false);                                                     // zadny vystup do seriove linky
      }  


  
  }



//----------------------------------------------
// zapamatuje si stav displeje, odskoci do interruptu a vrati se zpatky s obnovenim puvodniho stavu displeje
void odskok_int(void)
  {
    uint8_t pompam4 = D_pamet[4];                                                        // zapamatuje se stav displeje pred interruptem, ve kterem muze dojit k jeho zhasuti
    uint8_t pompam3 = D_pamet[3];                                      
    uint8_t pompam2 = D_pamet[2];
    uint8_t pompam1 = D_pamet[1];
    uint8_t pompam0 = D_pamet[0];
    interrupty();                                                                     // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni

    D_pamet[0] = pompam0;                                                             // po navratu z interruptu zobrazi puvodni stav displeje
    D_pamet[1] = pompam1;
    D_pamet[2] = pompam2;
    D_pamet[3] = pompam3;
    D_pamet[4] = pompam4;
    aktualizuj_displej();    
  }
//----------------------------------------------
