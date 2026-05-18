// Rozsireni o budik, ktery zapina SQM i pri vypnutem hlavnim vypinaci.
// Vyuziva externi RTC obvod DS3231, u ktereho se da nastavit preklopeni vystupniho /INT pinu do LOW i pri vypnutem napajeni (staci 3V zalohovaci baterie).
// Tento /INT pin pak sepne P-FET, ktery premosti hlavni vypinac.
// V loop() smycce se pak pravidelne testuje stav "Status Registru (0Fh)" a kdyz je bit0 (Alarm Flag 1) v HIGH, spousti se piskak.
// Stiskem tlacitka, nebo po 2 minutach se "Alarm Flag 1" zrusi.
// Vsechny udalosti spojene s alarmem je mozne logovat (spusteni, automaticke vypnuti po 2 minutach, rucni vypnuti).


#define I2C_3231_ADDR      0x68


//--------------------------------------------------------------------------------------------
// smazani Alarm Flagu v DS3231 (zrusi premosteni napajeciho vypinace FET tranzistorem)
void reset_AF(void)
  {
    if (pouzit_ds3231 == true)
      {
        RTC3231_write( 0x0F, 0b00000000);                        // Status registr: bit7 = '0' (Oscilator STOP flag se nastavuje do normalu - vsechno OK)
                                                                 //                 bit6 = '0' povinna 0
                                                                 //                 bit5 = '0' povinna 0
                                                                 //                 bit4 = '0' povinna 0
                                                                 //                 bit3 = '0' (zakazat generovani 32kHz na vystupnim pinu)
                                                                 //                 bit2 = '0' (Bussy me nezajima - prepsat '0')
                                                                 //                 bit1 = '0' (Zrusit nepouzity AlarmFlag2)
                                                                 //                 bit0 = '0' (Zrusit AlarmFlag1 - okamzite vypina premosteni napajeni)
      }
  }
//--------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------
// zarovnany vypis obshu registru v DS3231
void RTC_dump(void)
  {  
    if (pouzit_ds3231 == true)
      {
        Serial.println(lng315);                                  //"Obsah registru DS3231:"
        for (uint8_t i = 0; i < 0x13 ; i++)                         // vypise se vsech 18 registru
          {
            Serial.print("0x");                                  // registry jsou oznacene v HEX formatu, aby byly shodne s datasheetem
            if (i < 16) Serial.print('0');                       // zarovnani cisla registru uvodni mezerou
            Serial.print(i,HEX);
            Serial.print(" ... 0b");
            uint8_t ds_bajt = RTC3231_read(i);
            for (int16_t b = 7; b >= 0 ; b--)                        // rozlozeni hodnoty na jednotive bity kvuli spravnemu zarovnani
              {
                if (bitRead(ds_bajt,b) == false)  Serial.print('0');
                else                              Serial.print('1');
              }
            Serial.print("  (0x");                              // na zaver jeste do zavorky hodnotu v BCD formatu (horni a spodni pulbajt jako HEX)
            Serial.print(ds_bajt >> 4, HEX);                     // horni pulbajt
            Serial.print(ds_bajt & 0b1111 , HEX);                // dolni pulbajt
            Serial.print(")    // ");
            Serial.println(popisy3231[i]);                       // textovy popis registru
          }
      }
  }
//----------------------------------------------


//----------------------------------------------
// nastavi cas v DS3231 podle casu v STM32F411
//  kdyz je nastaven AFtest na HIGH, nastavi se alarmove registry na o 30 sekund vyssi cas, nez je cas v RTC, takze by se po 30 sekundach mel spustit testovaci alarm
void STM_DS(bool AFtest)
  {
    if (pouzit_ds3231 == true)
      {
        zobraz_RTC(false);
        loguj_rozdily_casu();                                               // do sys logu se zapise rozdil casu mesi DS3231 a STM
    
        RTC3231_write(0, (((LOC_sek / 10) << 4) | (LOC_sek % 10)));
        RTC3231_write(1, (((LOC_min / 10) << 4) | (LOC_min % 10)));
        RTC3231_write(2, (((LOC_hod / 10) << 4) | (LOC_hod % 10)));
    
        // tady by bylo pripadne nastaveni dne v tydnu do registru [3], ale zatim se DvT netestuje ani nezadava pri nastaveni casu alarmu

        
        RTC3231_write(4, (((LOC_den / 10) << 4) | (LOC_den % 10)));
        RTC3231_write(5, (((LOC_mes / 10) << 4) | (LOC_mes % 10)));         // nejvyssi bit je century, ale ten vubec neresim (zustava v '0')
        LOC_rok = LOC_rok - 2000;                                           // do DS3231 se zadava jen dvojciferny rok 00 az 99
        RTC3231_write(6, (((LOC_rok / 10) << 4) | (LOC_rok % 10)));
    
        posledni_time_synchro = millis();                                   // znacka, kdy doslo k posledni synchronizaci casu v DS3231

        RTC3231_write( 0x10, AGING);                                        // kalibracni registr (AGING OFFSET) = defaultne 0        
        dalsi_budik();                                                      // aktualizace alarmovych registru v DS3231

    
        if (AFtest == true)
          {
            uint8_t alsek = LOC_sek + 30;                                      // pricteni 30 sekund k aktualnimui casu s pripadnymi prenosy mezi minutami a hodinami
            uint8_t almin = LOC_min;
            uint8_t alhod = LOC_hod;
            if (alsek > 59)
              {
                alsek = alsek - 60;
                almin ++;
                if (almin > 59)
                  {
                    almin = almin - 60;
                    alhod ++;        
                  }
              }
            EEPROM_update(eeaddr_TEST_alarm, 0xCC);                         // znacka, ze pristi probuzeni ma byt testovaci alarm   
    
            RTC3231_write( 0x07, (((alsek / 10) << 4) | (alsek % 10)));     // alarmove sekundy:  bit7 (A1M1 = '0')
                                                                            //                    zbytek se nastavi na takove sekundy, aby odpovidaly casu o 30 sek vyssimu, nez je v RTC
    
            RTC3231_write( 0x08, (((almin / 10) << 4) | (almin % 10)));     // alarmove minuty:   bit7 (A1M2 = '0')
                                                                            //                    zbytek se nastavi na takove minuty, aby odpovidaly casu o 30 sek vyssimu, nez je v RTC
    
            RTC3231_write( 0x09, (((alhod / 10) << 4) | (alhod % 10)));     // alarmove hodiny:   bit7 (A1M3 = '0')
                                                                            //                    zbytek se nastavi na takove hodiny, aby odpovidaly casu o 30 sek vyssimu, nez je v RTC
    
            RTC3231_write( 0x0A, 0b10000010);                               // alarmove dny:      bit7 (A1M4 = '1') porovnava se hodina, minuta a sekunda
                                                                            //                    na zbytku nezalezi, nastavi se datum = 2.     
            reset_AF();  
    
            RTC3231_write( 0x0E, 0b00100101);                               // Control registr: bit7 = '0' = enable oscilator
                                                                            //                  bit6 = '0' = BBSQ zruseno
                                                                            //                  bit5 = '1' = vypocet teplotni kompenzace
                                                                            //                  bit4 = '0' = SQUARE-WAVE OUTPUT FREQUENCY 1Hz, ale stejne je generovani impulzu zakazano
                                                                            //                  bit3 = '0' = SQUARE-WAVE OUTPUT FREQUENCY 1Hz, ale stejne je generovani impulzu zakazano 
                                                                            //                  bit2 = '1' = zruseni SQW generovani impulzu, povoleni INT pro ALARM
                                                                            //                  bit1 = '0' = deaktivace druhe sady alarmovych registru
                                                                            //            !!!   bit0 = '1' = AKTIVACE prvni sady alarmovych registru (A1IE)
    
            Serial.println(lng313);                                         // "Vypni napajeni a odpoj USB kabel. Za 30 sekund by se mel spustit alarm."
    
          }
      }  
  }


//----------------------------------------------
// zapis jednoho bajtu do externi RTC DS3231
void dalsi_budik(void)
  {
    if (pouzit_ds3231 == true)
      {
        uint8_t index_dalsiho = 0;                     // ve vysledku tady bude nejblizsi vyssi budik po aktualnim case
        uint8_t index_minima = 0;                      // pro pripad, ze je aktualni cas vyssi nez vsechny budiky, zapamatuje se jeste nejnizsi budik pro spusteni pristi den
        uint16_t pole_budiku[7];                // pole, do ktereho se nactou nastavene budiky z EEPROM
        pole_budiku[0] = 1440;                      // polozka pole mimo pouzivane indexy (1 az 6)
        
        for (uint8_t bud = 1; bud < 7 ; bud ++)        // nejdriv se prenesou nastavene hodnoty z EEPROM do pole_budiku[] pro lepsi pristup, aby se nemuselo porad sahat do EEPROM
          {
            pole_budiku[bud] =  EEPROM_read_int(((bud - 1) * 2) + eeaddr_alarmy);
          }
    
        uint8_t ds_min = RTC3231_read(1);                                        // z DS3231 precte aktualni hodiny a minuty a prevede je na aktualni minuty od pulnoci
        uint8_t ds_hod = RTC3231_read(2);
        ds_hod = (((ds_hod >> 4) &   0b11) * 10) + (ds_hod & 0b1111);         // prevod z BCD na skutecne minuty a hodiny
        ds_min = (((ds_min >> 4) &  0b111) * 10) + (ds_min & 0b1111);
        uint16_t ds_aktual  = (ds_hod * 60) + ds_min;                     // minuty od pulnoci
    
        for (uint8_t bud = 1; bud < 7 ; bud ++)
          {
            if (pole_budiku[bud] != 1440)                                     // hodnota 1440 znamena deaktivovany budik, proto se preskakuje
              {
                if ((pole_budiku[bud] > ds_aktual) and (pole_budiku[index_dalsiho] > pole_budiku[bud]))     // nalezeni nejblizsiho vyssiho budiku
                  {
                    index_dalsiho = bud;
                  }
    
                if (pole_budiku[bud] < pole_budiku[index_minima])     // nalezeni minimalniho budiku (pristi den po pulnoci)
                  {
                    index_minima = bud;
                  }
              }
          }
    
        if (index_dalsiho == 0)                                       // kdyz nebyl nalezen nejblizsi vyssi budik, pouzije se ten nejnizsi (z dalsiho dne)
          {
            index_dalsiho = index_minima;
          }
    
        if (pole_budiku[index_dalsiho] < 1440)                        // kdyz je v EEPROM nejaky budik zadany, zapise se jeho hodnota do alarmovych registru a aktivuji se alarmove funkce
          {
            uint8_t DS_alarm_hod = pole_budiku[index_dalsiho] / 60;
            uint8_t DS_alarm_min = pole_budiku[index_dalsiho] % 60;
        
            // nastaveni alarmovych registru v DS3231
            RTC3231_write( 0x07, 0);                                                   // alarmove sekundy:  bit7 (A1M1 = '0')
                                                                                       //                    zbytek sekund se nastavi na 0 (testuje se jen na minuty a hodiny, alarm se spousti v 0 sekundach)
        
            RTC3231_write( 0x08, (((DS_alarm_min / 10) << 4) | (DS_alarm_min % 10)));  // alarmove minuty:   bit7 (A1M2 = '0')
                                                                                       //                    zbytek se nastavi na takove minuty, aby odpovidaly casu dalsiho budiku v EEPROM
        
            RTC3231_write( 0x09, (((DS_alarm_hod / 10) << 4) | (DS_alarm_hod % 10)));  // alarmove hodiny:   bit7 (A1M3 = '0')
                                                                                       //                    zbytek se nastavi na takove hodiny, aby odpovidaly casu dalsiho budiku v EEPROM
        
            RTC3231_write( 0x0A, 0b10000010);                                          // alarmove dny:      bit7 (A1M4 = '1') porovnava se hodina, minuta a sekunda
                                                                                       //                    na zbytku nezalezi, nastavi se datum = 2.     
            
            RTC3231_write( 0x0E, 0b00100101);                                          // Control registr: bit7 = '0' = enable oscilator
                                                                                       //                  bit6 = '0' = BBSQ zruseno
                                                                                       //                  bit5 = '1' = vypocet teplotni kompenzace
                                                                                       //                  bit4 = '0' = SQUARE-WAVE OUTPUT FREQUENCY 1Hz, ale stejne je generovani impulzu zakazano
                                                                                       //                  bit3 = '0' = SQUARE-WAVE OUTPUT FREQUENCY 1Hz, ale stejne je generovani impulzu zakazano 
                                                                                       //                  bit2 = '1' = zruseni SQW generovani impulzu, povoleni INT pro ALARM
                                                                                       //                  bit1 = '0' = deaktivace druhe sady alarmovych registru
                                                                                       //            !!!   bit0 = '1' = AKTIVACE prvni sady alarmovych registru (A1IE)    
          }
        else                                          // kdyz neni ani jeden budik aktivni, vraci se index [0] s hodnotou 1440. V tom pripade se alarmove funkce deaktivuji
          {
            RTC3231_write( 0x0E, 0b00100100);                                          // Control registr: bit7 = '0' = enable oscilator
                                                                                       //                  bit6 = '0' = BBSQ zruseno
                                                                                       //                  bit5 = '1' = vypocet teplotni kompenzace
                                                                                       //                  bit4 = '0' = SQUARE-WAVE OUTPUT FREQUENCY 1Hz, ale stejne je generovani impulzu zakazano
                                                                                       //                  bit3 = '0' = SQUARE-WAVE OUTPUT FREQUENCY 1Hz, ale stejne je generovani impulzu zakazano 
                                                                                       //                  bit2 = '1' = zruseni SQW generovani impulzu, povoleni INT pro ALARM
                                                                                       //                  bit1 = '0' = deaktivace druhe sady alarmovych registru
                                                                                       //            !!!   bit0 = '0' = DEAKTIVACE prvni sady alarmovych registru  (A1IE)       
            
          }
      }    
  }




//----------------------------------------------
// zapis jednoho bajtu do externi RTC DS3231
void RTC3231_write(uint8_t registr, uint8_t hodnota)
  {
    if (pouzit_ds3231 == true)
      {
        if (blokuj_3231 == false)
          {
            Wire.beginTransmission(I2C_3231_ADDR);                                                     // zacatek komunikace s DS3231
            Wire.write(registr);                                                                       // cislo registru
            delay(1);
            Wire.write(hodnota);                                                                       // zapis hodnoty
            uint8_t err = Wire.endTransmission();                                                         // konec komunikace
            if (err > 0) chyba(1);
            else         bitClear(err_bit,0);                                                         // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
            delay(5);    
          }
      }
  }
//----------------------------------------------

//----------------------------------------------
// cteni 1 bajtu z RTC DS3231
uint8_t RTC3231_read(uint8_t registr)
  {
    if (pouzit_ds3231 == true)
      {
        if (blokuj_3231 == false)
          {
            Wire.beginTransmission(I2C_3231_ADDR);                                                     // zacatek komunikace s DS3231
            Wire.write(registr);                                                                       // cislo registru
            uint8_t err = Wire.endTransmission();                                                         // konec komunikace
            if (err > 0) chyba(1);
            else         bitClear(err_bit,0);                                                          // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
            delayMicroseconds(2);
            Wire.requestFrom(I2C_3231_ADDR, 1);
            return Wire.read();    
          }
        else                                                                                          // kvuli nejake chybe DS3231 se docasne blokuje i komunikace pres I2C
          {
            return 255;
          }
      }
    else                                                                                              // pri zakazanem DS3231 se vraci cislo 255
      {
        return 255;
      }
  }
//----------------------------------------------

//----------------------------------------------


//----------------------------------------------
// funkce = 0 ... v EEPROM smaze jeden budik, ktery ma stejnou hodnotu, jako budik, ktery je aktualne nastaveny v Alarmovych registrech DS3231
// funkce = 1 ... vypise vsechny budiky z EEPROM vcetne vypnutych (pouzito v podprogramech pro zadavani budiku pres seriovou linku)
// funkce = 2 ... vypise jen aktivni budiky z EEPROM  (pouzito v podprogramu 'sys_info')
// funkce = 3 ... jen zjisti pocet aktivnich budiku v EEPROM
// funkce = 4 ... vrati index prave aktivovaneho budiku (kdyz se shoduji alarmove registry v DS3231 a nejaka polozka v EEPROM)

uint8_t vypis_budiku(uint8_t funkce)
  {
    if (pouzit_ds3231 == true)
      {
        uint8_t aktivni = 0;
    
        uint8_t ala_min = RTC3231_read(8);                                          // z DS3231 precte hodiny a minuty z alarmovych registru a prevede je na minuty od pulnoci
        uint8_t ala_hod = RTC3231_read(9);
        ala_hod = (((ala_hod >> 4) &   0b11) * 10) + (ala_hod & 0b1111);         // prevod z BCD na skutecne minuty a hodiny
        ala_min = (((ala_min >> 4) &  0b111) * 10) + (ala_min & 0b1111);
        uint16_t ds_ala  = (ala_hod * 60) + ala_min;
    
        for (uint8_t bud = 1; bud < 7 ; bud ++)
          {
            uint16_t budmin = EEPROM_read_int(((bud - 1) * 2) + eeaddr_alarmy);

            if (budmin >= 1440)                                                                   // budik v EEPROM neni nastaveny
              {               
                if (funkce == 1)
                  {
                    if (bud < 6)
                      {
                        Serial.print(lng311);                                                         // "budik c. "
                        Serial.print(bud);
                      }
                    else
                      {
                        Serial.print(lng318);                                                         // "dopocet  "
                      }
                    Serial.println(lng312);                                                           // " vypnuty"                                
                  }
              }
            else                                                                                  // budik v EEPROM je nastaveny na nejaky cas
              {
                if (funkce == 1 or funkce == 2)
                  {
                    if (bud < 6)
                      {
                        Serial.print(lng311);                                                         // "budik c. "
                        Serial.print(bud);
                        Serial.print(' ');
                        if ((budmin / 60) < 10)   Serial.print('0');                                  // pripadna nula pred hodiny
                        Serial.print(budmin / 60);                                                    // prevod minut na cele hodiny
                        Serial.print(':');
                        if ((budmin % 60) < 10)   Serial.print('0');                                  // pripadna nula pred minuty
                        Serial.print(budmin % 60);                                                    // prevod minut na zbytek minut
                      }
                    else
                      {
                        Serial.print(lng318);                                                         // "dopocet -"
                        Serial.print(zbyvajici_minuty(budmin));
                      }
                    
                    if (budmin == ds_ala)  Serial.println(" *");                                  // kdyz se vypisovany budik shoduje s nastavenou hodnotou v alarmovych registrech doplni se hvezdicka
                    else                   Serial.println("  ");                                  // kdyz se neshoduje, tak konec radky bez hvezdicky
                  }
                if (funkce == 0)                                                                  // mazani budiku z EEPROM, pokud ma v EEPROM stejnou hodnotu jako v alarmovych registrech
                  {
                    if (budmin == ds_ala)
                      {
                        EEPROM_write_int(((bud - 1) * 2) + eeaddr_alarmy, 1440);                  // prepsani budiku v EEPROM hodnotou 1440 (= nezadany budik)
                        break;                                                                    //  maze se jen jeden zaznam - i v pripade, ze by bylo v EEPROM nastaveno vice stejnych hodnot
                      }
                  }
                if (funkce == 3)                                                                  // pocitadlo aktivnich budiku v EEPROM
                  {
                    aktivni++;
                  }
                if (funkce == 4)                                                                  // kdyz se hleda shoda v  EEPROM s prave aktivnim budikem
                  {
                    if (budmin == ds_ala)
                      {
                        aktivni = bud;                                                            // vraci se index budiku 
                        break;
                      }
                  }
              }                                    
          }

        
        if (funkce == 3 or funkce == 4)  return aktivni;                                          // pokud se jedna o funkci, ktera jen pocita aktivni budiky v EEPROM, vrati se jejich pocet
        else                             return 255;                                              // vsechny ostatni funkce vraci 255
      }
    else
      {
        return 0;                                                                                 // kdyz je DS3231 zakazane, vraci vzdycky 255
      }
  
  }


//----------------------------------------------
// Pri zjistenem aktivnim AF (testuje se v podprogramu 'interrupty()') se vstoupi do tohoto podprogramu, 
//          ktery se bud ukonci sam po dvou minutach, nebo bude ukoncen stiskem libovolneho tlacitka.
// Jinak je to neprerusitelny podprogram. Nic jineho behem alarmu nebezi.
void alarm_3231(void)
  {
    if (pouzit_ds3231 == true)
      {
        SD_log(991, 0);                                                                 // logovani spusteni alarmu (0 = start)
    
        uint16_t cas_spusteni = millis();
        bool byl_stisk = false;
        uint8_t ton_bzuku = 0;
        
        uint8_t indexbudiku = vypis_budiku(4);                                             // vrati index v EEPROM prave aktivovaneho budiku (1-5) / nebo odpoctu (6)

        if (EEPROM_read(eeaddr_TEST_alarm) == 0xCC)                                     // jedna se o testovaci alarm, ktery nema v EEPROM nastavenou zadnou polozku
          {
            indexbudiku = 7;                                                            // specialni index budiku pro funkci testu (bude blikat ALARM / TEST)
            EEPROM_write(eeaddr_TEST_alarm, 0);                                         // znacka, ze se jedna o testovaci alarm se smaze
          }

        if (indexbudiku != 0)                                                           // nasla se shoda s nejakou polozkou v EEPROM        
          {
            while (((millis() - cas_spusteni) < 120000UL) and (byl_stisk == false))     // alarmovy stav trva maximalne 120 sekund
              {
                
                if ((millis() - cas_spusteni) > 10000UL)                                // po 10 sekundach se pridava i bzukani
                  {
                    tone_X(pin_bzuk , 500+(ton_bzuku*100) , 90 , 2);
                  }   
                if (ton_bzuku == 0)
                  {
                    if (bitRead(signal_LED,4) == true)                                  // kdyz je blikani povolene
                      {
                        LED_W(true);                                                    //   bila LED se rozsviti
                      }

                    if (indexbudiku == 6)                                               // pro funkci "odpocet" ...
                      {
                        zobraz_text(92);                                                //    ... se rozsviti napis "odPoc"
                      }
                    else                                                                // pro funkci "alarm"  ...
                      {
                        zobraz_text(53);                                                //    .. se rozsviti napis "ALArM"
                      }
                    
                  }
                if (ton_bzuku == 3)
                  {
                    LED_W(false);                                                       // bila LED v kazdem pripade zhasne (i kdyz neni povolena)
                    if (indexbudiku == 6)                                               // pro funkci "odpocet" ...
                      {
                       zobraz_text(66);                                                 // "     "                ... na 300ms napis zhasne
                      }

                    if (indexbudiku == 7)                                               // pro funkci testovaciho alarmu
                      {
                       test_text(0);                                                    // "     "                ... na 300ms blikne napist "tESt"
                      }

                    if (indexbudiku < 6)                                                // pro funkci "alarm" problikava index budiku ohraniceny pomlckami
                      {
                        D_pamet[0] = 64;                                                // '-'
                        D_pamet[1] = 64;                                                // '-'
                        D_pamet[2] = graf_def[indexbudiku];                             // uprostred displeje je index aktivniho budiku 1 az 5
                        D_pamet[3] = 64;                                                // '-'
                        D_pamet[4] = 64;                                                // '-'
                        aktualizuj_displej();
                      }
                  }
                ton_bzuku ++;                                                           // ton bzuku se prepina kazdych 100ms
                if(ton_bzuku > 5) ton_bzuku = 0;
        
                delay(100);                                                             // casovani smycek na 0,1 sek.
                noTone(pin_bzuk);          
        
                byl_stisk = test_stisku();                                              // tlacitka se testuji 10x za sekundu        
              }
        
            vypis_budiku(0);                                                            // s parametrem 0 se nic nevypisuje, ale jen se smaze aktivni budik z EEPROM
            
            zobraz_text(68);                                                            // "-----"
            while (digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW or digitalRead(pin_tl_ok) == LOW or digitalRead(pin_TS) == LOW)
              {
                delay(50);                                                              // cekani na uvolneni vsech tlacitek
                if (digitalRead(pin_tl_up) == LOW and digitalRead(pin_tl_dn) == LOW and digitalRead(pin_tl_ok) == LOW)
                  {
                    blokuj_3231 = true;                                                 // stisk vsech tri tlacitek najednou trvale (do dalsiho resetu) blokuje dalsi alarmy i komunikaci s DS3231 (napriklad pri poruse DS3231)
                  }
              }
            delay(50);
          }                                                                             // kdyz nenbyla nalezena shoda alarmovych registru se zaznamy v EEPROM, predchozi blok se preskocil 

    
        SD_log(991, (millis() - cas_spusteni) / 1000);                                  // logovani, po kolika sekundach se alarm vypnul (120 = automaticke vypnuti po dvou minutach)
        delay(100);
        
        dalsi_budik();                                                                  // Ze zbylych zaznamu v EEPROM se vybere nejblizsi dalsi budik.
                                                                                        //    Kdyz uz v EEPROM dalsi budik neni, blokuji se alarmove funkce (bit A1IE = '0')
                                                                                        
                                                                                        // POZOR:
                                                                                        // Pokud doslo k alarmovemu premosteni hlavniho vypinace, tak
                                                                                        //  uz tady se vypina napajeni se stale aktivnim bitem A1F 
                                                                                        //      a "reset_AF()" na dalsi radce se mozna nepovede dokoncit.
                                                                                        //      V sekci "setup()" se proto testuje jeste bit A1IE a pokud je nastaveny na 0, tak se
                                                                                        //        AF bit maze jeste pred spustenim dalsiho alarmu.
    
                                                                                        // Kdyz je hlavni vypinac zapnuty, k zadnemu vypadku napajeni nedojde 
        reset_AF();                                                                     //   a bit A1F se regulerne smaze  
      }
  }


//----------------------------------------------
// stisk libovolneho tlacitka vraci true
bool test_stisku(void)
  {
    bool ret_stav = false;
    if (digitalRead(pin_tl_up) == LOW) ret_stav = true;
    if (digitalRead(pin_tl_dn) == LOW) ret_stav = true;
    if (digitalRead(pin_tl_ok) == LOW) ret_stav = true;
    if (digitalRead(pin_TS) == LOW) ret_stav = true;
    return ret_stav;
  }
//----------------------------------------------



//----------------------------------------------
// logovani rozdilu casu mezi DS3231 a STM32 pri serizeni casu
void loguj_rozdily_casu(void)
  {
    if (pouzit_ds3231 == true)
      {
        uint32_t ds_hms;
        uint32_t stm_hms;
        cti_ds3231();                                                  // nacteni casovych registru DS3231 a jejich prevod na globalni promenne 'ds_xxx'
    
        ds_hms  = (ds_hod  * 3600UL) + (ds_min  * 60UL) + ds_sek;      // vypocet sekund z hodin, minut a sekund v DS3231
        stm_hms = (LOC_hod * 3600UL) + (LOC_min * 60UL) + LOC_sek;     // vypocet sekund z hodin, minut a sekund v STM32
    
        if (stm_hms >= ds_hms) SD_log (989, stm_hms - ds_hms);         // kod 989 = DS3231 se zpozduje (nebo je stejny jako STM32)
        else                   SD_log (990, ds_hms - stm_hms);         // kod 990 = DS3231 se predchazi
      }    
  }
//----------------------------------------------



//----------------------------------------------
// posle cas v DS3231 do seriove linky ve format "d.m.20yy h:nn:ss"
//   vyzaduje, aby byly casove hodnoty aktualizovane podprogramem 'cti_ds3231()'
void formatuj_ds_cas(void)
  {
    Serial.print(ds_den);
    Serial.print('.');
    Serial.print(ds_mes);
    Serial.print(".20");
    if (ds_rok<10) Serial.print('0');
    Serial.print(ds_rok);
    Serial.print(' ');
    Serial.print(ds_hod);
    Serial.print(':');
    if (ds_min<10) Serial.print('0');
    Serial.print(ds_min);
    Serial.print(':');
    if (ds_sek<10) Serial.print('0');
    Serial.println(ds_sek);    
  }
//----------------------------------------------



//----------------------------------------------
// prevede zadany pocet minut na budouci cas v minutach od pulnoci
// prebira aktualni cas z globalnich promennych 'ds_hod' a 'ds_min'
// resi i pripadne preteceni do dalsiho dne
//      priklad:    aktualni cas je 23:30 v promennych 'ds_hod' a 'ds_min'
//                  zadany pocet minut je 100.
//                  vysledek je 1:10, ktera se prevede na minuty od pulnoci = 70
uint16_t pridej_minuty(uint16_t plusminuty)
  {
    cti_ds3231();                                                                       // z pridavneho obvodu DS3231 nacte aktualni cas a ulozi ho do globalnich promennych "ds_xxx"
    uint16_t aktualni_minuty_od_pulnoci = (ds_hod * 60) + ds_min;

    return (aktualni_minuty_od_pulnoci + plusminuty) % 1440;                            // pokud dojde k preteceni pres pulnoc do dalsiho dne, vraci se jen zbytek minut od posledni pulnoci       
  }
//----------------------------------------------



//----------------------------------------------
// vypocte, kolik minut zbyva od aktualniho casu z globalnich promennych 'ds_hod' a 'ds_min'
//        do zadaneho casu v minutach od pulnoci
// resi i pripadne preteceni do dalsiho dne
//      priklad:    aktualni cas je 23:30 v promennych 'ds_hod' a 'ds_min'
//                  zadany pocet minut od pulnoci je 70
//                  vysledek by mel by 100 (30 minut do pristi pulnoci a 70 minut do zadaneho casu)
uint16_t zbyvajici_minuty(uint16_t cilove_minuty)
  {
    cti_ds3231();                                                                       // z pridavneho obvodu DS3231 nacte aktualni cas a ulozi ho do globalnich promennych "ds_xxx"
    uint16_t aktualni_minuty_od_pulnoci = (ds_hod * 60) + ds_min;

    if (cilove_minuty >= aktualni_minuty_od_pulnoci)   return cilove_minuty - aktualni_minuty_od_pulnoci;             // cilovy cas je jeste dnes
    else                                               return (1440 - aktualni_minuty_od_pulnoci) + cilove_minuty;    // cilovy cas bude zitra
  }
//----------------------------------------------



//----------------------------------------------
// Precte registry z DS3231 a prevede je na globalni promenne ds_sek, ds_min, ds_hod....
// Cteni je provadeno s kontrolou preteceni sekund
void cti_ds3231(void)
  {
    if (pouzit_ds3231 == true)
      {
        uint8_t ds_sek0;                                   // jen pomocna promenna pro zaznamenani sekund na zacatku cteni registru. Podle ni se pak vyhodnocuje pripadne nechtene preteceni
        do 
          {                                             // ochrana proti preteceni citacu behem postupneho cteni registru z DS3231
            ds_sek0 = RTC3231_read(0);                  // prvni cteni sekund pro kontrolu, ze pri dalsim cteni nedojde k preteceni
            ds_min  = RTC3231_read(1);
            ds_hod  = RTC3231_read(2);
            ds_den  = RTC3231_read(4);
            ds_mes  = RTC3231_read(5);
            ds_rok  = RTC3231_read(6);
            ds_sek  = RTC3231_read(0);                  // opakovane cteni sekund. Pokud je vsechno spravne, nemelo by dojit k preteceni sekund z 59 na 00
            ds_sek0 =(((ds_sek0 >> 4) &  0b111) * 10) + (ds_sek0 & 0b1111);   // prevod z rozdelelneho BCD na normalni sekundy 0 az 59
            ds_sek  =(((ds_sek  >> 4) &  0b111) * 10) + (ds_sek  & 0b1111);
          }
        while (ds_sek0 == 59 and ds_sek == 0);          // kdyz behem cteni doslo k preteceni sekund z 59 na 00, provede se opakovane cteni vsech registru
    
        ds_min =(((ds_min >> 4) &  0b111) * 10) + (ds_min & 0b1111);
        ds_hod =(((ds_hod >> 4) &   0b11) * 10) + (ds_hod & 0b1111);
        ds_den =(((ds_den >> 4) &   0b11) * 10) + (ds_den & 0b1111);
        ds_mes =(((ds_mes >> 4) &    0b1) * 10) + (ds_mes & 0b1111);
        ds_rok =(((ds_rok >> 4) & 0b1111) * 10) + (ds_rok & 0b1111);           // rok se vraci jen jako dvojciferny
      }
  }
//----------------------------------------------
