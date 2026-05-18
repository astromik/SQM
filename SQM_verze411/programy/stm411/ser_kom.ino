// USB seriova komunikace 
//================================================



//----------------------------------------------
// test, jestli pres USB seriovou komunikaci prisel nejaky pozadavek
void test_ser_kom(void)
  {
    if (Serial.available())
      {
        char funkce = Serial.read();
    
        if (funkce == '?')                                                       // napoveda pro uzivatelske (bezne) funkce
          {
            ser_napoveda1();
          }
    
        if (funkce == '#')                                                       // vstup do podmenu servisnich funkci (malo pouzivane)
          {
            servisni_funkce();
          }

        if (funkce == '@')                                                       // vstup do podmenu tajnych funkci (prime rizeni systemu)
          {
            tajne_funkce();
          }

        if (funkce == '*')                                                       // vstup do podmenu funkci pouzitych pro komunikaci s PC programem
          {
            PC_funkce();
          }


        if (funkce == '%')                                                       // vstup do podmenu specialnich funkci (rozsirujici HW) //!! (r30s)
          {
            spec_funkce();
          }
    
        if (senzor_BME == true)
          {
            if (funkce == USB_fce_1_03)                                              //'p'
              {
                Serial.print(lng004);                                                //  Serial.print("Absolutni tlak (bez prepoctu na hladinu more): ");
                Serial.print(citelny_tlak(tlak()));
                Serial.println(" Pa");                             
              }

            if (funkce == USB_fce_1_01)                                              //'v'
              {
                Serial.print(lng001);                                                //  Serial.print("Vlhkost: ");
                
                Serial.print(citelna_vlhkost(vlhkost()));       
                Serial.println(" %");
              }
              
            if (funkce == USB_fce_1_02)                                              //'t'
              {
                uint16_t pom_teplota = teplota(false);
                Serial.print(lng002);                                                //  Serial.print("Teplota bez korekce: ");
                Serial.print(citelna_teplota(pom_teplota));
                Serial.println(" 'C");
    
                Serial.print(lng003);                                                //  Serial.print("Teplota s korekci podle tabulky: ");
                Serial.print(citelna_teplota(korekce_teploty(pom_teplota)));
                Serial.println(" 'C");          
              }
          
          
          
          }    
    
    
        if (funkce == USB_fce_1_05)                                              // 'z' nastaveni zony (z 0 = SEC; z 1 = SELC)
          {
            delay(20);
            uint8_t pomprom = Serial.parseInt();                                    // precte nasledujici cislo (ignoruje pripadnou mezeru)
            Serial.print(lng005);                                                //  Serial.print("Nastavena zona ");
            if (pomprom == 0)
              {
                leto = false;
                Serial.println(char_zima);                                       //  Serial.println("SEC ");
              }
            else
              {
                leto = true;
                Serial.println(char_leto);                                       //  Serial.println("SELC");
              }
            EEPROM_update(eeaddr_leto_zima,leto);
            STM_DS(false);                                                       // serizeni casu v DS3231 podle casu v STM
          }
    
        if (funkce == USB_fce_1_04)                                              //  'j' zobraz svetlo (bez korekce i s korekci)
          {
            uint16_t pom_svetlo = svetlo_1x(false,false);
            Serial.print(lng008);                                                //  Serial.print("Plosny jas bez korekce: ");
            Serial.print(pom_svetlo / 1000.0, 3);
            Serial.println(" mag/arcsec2 ");

            Serial.print(lng009);                                                //  Serial.print("Plosny jas s korekci podle tabulky: ");
            Serial.print(korekce_svetla(pom_svetlo) / 1000.0, 3);
            Serial.println(" mag/arcsec2 ");

          }    


        if (funkce == USB_fce_1_06)                                              // 'a' nastaveni automatickeho mereni
          {
            delay(20);
            long pomprom = Serial.parseInt();
            if (pomprom < 0 or pomprom > 255)
              {
                pomprom = 0;
              }
            EEPROM_update(eeaddr_automat,pomprom);
            automat = pomprom;
            MODdata[46] = automat;                                               // interval automatickeho spousteni
            if (automat == 0) Serial.println(lng010);                            //  Serial.println("Automat vypnuty");
            else
              {
                Serial.print(lng011);                                            //  Serial.print("Automaticky spoustet mereni po ");
                Serial.print(automat);
                Serial.println(" min.");             
                posledni_autospusteni = millis();                                // prvni mereni se spusti okamzite
                mereni(6);         
              }
          }

        if (pouzit_ds3231 == true)
          {
            if (funkce == USB_fce_1_27)                                          // 'b n hh:mm' nastaveni jednoho budiku
              {
                uint8_t budiky_hodiny = 0;
                uint8_t budiky_minuty = 0;
                long pomprom;
                bool budik_zapnuty = true;
                delay(20);
                long cislo_budiku = Serial.parseInt();                           // zjistovani cisla budiku
                if (cislo_budiku > 0 and cislo_budiku < 6)                       // cislo budiku musi byt v rozsahu 1 az 5
                  {
                    if (Serial.available() < 3 )                                 // v nejhorsim pripade ocekavam alespon 3 znaky (H:M). Kdyz nedorazi, vypinam zvoleny budik
                      {
                        budik_zapnuty = false;
                      }
                    
                    pomprom = Serial.parseInt();                                 // zjistovani hodin
                    if (pomprom > 23 or pomprom < 0)                             // hodiny musi byt v rozsahu 0 az 23
                      {
                        budik_zapnuty = false;
                      }
                    else
                      {
                        budiky_hodiny = pomprom;           
                      }
                                    
                    Serial.read();                                               // oddelovac hodin a minut (dvojtecka / mezera ...)
    
                    pomprom = Serial.parseInt();                                 // zjistovani minut
                    if (pomprom > 59 or pomprom < 0)                             // minuty musi byt v rozsahu 0 az 59
                      {
                        budik_zapnuty = false;
                      }
                    else
                      {
                        budiky_minuty = pomprom;
                      }
    
                    if (budik_zapnuty == true)
                      {
                        EEPROM_write_int((((cislo_budiku - 1) * 2) + eeaddr_alarmy), (budiky_hodiny * 60) + budiky_minuty);
                        RTC_alarm[cislo_budiku] = (budiky_hodiny * 60) + budiky_minuty;
                      }
                    else
                      {
                        EEPROM_write_int((((cislo_budiku - 1) * 2) + eeaddr_alarmy), 1440);
                        RTC_alarm[cislo_budiku] = 1440;
                      }
                    dalsi_budik();                                               // vyhledani dalsiho budiku pro oznaceni hvezdickou
                    vypis_budiku(1);                                             // tabulka nastavenych nebo smazanych budiku
                  }
                else
                  {
                    Serial.print(lng006);                                        // "Vstup mimo rozsah "
                    Serial.println("(1 - 5)");
                    vypis_budiku(1);                                             // tabulka nastavenych nebo smazanych budiku
                  }
              
              }

            if (funkce == USB_fce_1_28)                                          // 'o -nnn' nastaveni odpoctu
              {                                                                  // jen samotny znak 'o' (pripadne znaky 'o'<CR> nebo 'o'<LF>) odpocet nemaze, ale zobrazi tabulku alarmu
                                                                                 // znaky 'o'<MEZERA> jsou ale uz vyhodnoceny jako parametr nula a mazou odpocet 
                long zadane_minuty;
                int16_t znak = Serial.peek();                                        // test, jestli za znakem 'o' nasleduje nejaky rozumny parametr, nebo jestli parametr chybi
                if (znak > 13 and znak < 58)   zadane_minuty = Serial.parseInt();   // pokud za znakem '0' neco pokracuje (krome <CR>, <LF>) je to nejaky parametr (treba i mezera), zjsiti se zadany pocet minut
                else                           zadane_minuty = 1000;             // pokud za znakem 'o' nic neni, nebo nasleduje <CR>, nebo <LF>, nastavi se minuty zamerne mimo rozsah
                
                if (zadane_minuty >= -999 and zadane_minuty <= 999)              // zadane cislo je ve spravnem rozsahu
                  {
                    zadane_minuty = abs(zadane_minuty);                          // nezalezi na tom, jestli se zada kladne nebo zaporne cislo
                    
                    if (zadane_minuty == 0)                                      // pri zadani 0 minut na odpoctu se odpoctovy alarm maze
                      {
                        RTC_alarm[6] = 1440;
                      }
                    else                                                         // pri zadani nejakeho nenuloveho poctu minut se vypocte cas alarmu v minutach od pulnoci 
                      {
                        RTC_alarm[6] = pridej_minuty(zadane_minuty);
                      }                 

                    EEPROM_write_int(eeaddr_alarmy + 10 , RTC_alarm[6]);         // zapis odpoctu do tabulky budiku v EEPROM
                    STM_DS(false);                                               // serizeni casu v DS3231 podle STM32 (false = bez 30-sekundoveho testu)
                  }
                else                                                             // zadane cislo je mimo povoleny rozsah
                  {
                    Serial.print(lng006);                                        // "Vstup mimo rozsah "
                    Serial.println("(0 - 999)");
                  }
                vypis_budiku(1);                                             // tabulka nastavenych nebo smazanych budiku
              }
          
          
          }




        if (pocet_cidel_svetla > 1 and funkce == USB_fce_1_07)                   // 'e'
          {
            long pomprom = Serial.parseInt();
            if (pomprom > 0 and pomprom <= pocet_cidel_svetla)
              {
                prepni_cidlo_svetla(pomprom);
                Serial.print(lng012);                                            //  Serial.print("Aktualne je aktivni cidlo: ");
                Serial.print(index_cidla_svetla);
                Serial.print(" / ");
                Serial.println(pocet_cidel_svetla);
              }
            else                                                                 // parametr mimo rozsah
              {
                if (pomprom == 0)
                  {
                    prepni_cidlo_svetla();                                       // jedina povolena moznost je 0, ktera prepina na nasledujici cidlo
                    Serial.print(lng012);                                        //  Serial.print("Aktualne je aktivni cidlo: ");
                    Serial.print(index_cidla_svetla);
                    Serial.print(" / ");
                    Serial.println(pocet_cidel_svetla);
                  }
                else
                  {
                    Serial.print(lng006);                                        // "Vstup mimo rozsah "
                    Serial.print("(1 - ");
                    Serial.print(pocet_cidel_svetla);
                    Serial.println(')');
                  }
              }
          }

        if (funkce == USB_fce_1_12)                                              // 'c'  zobrazi aktualni datum a cas
          {
            zobraz_RTC(true,true);
          }


        if (funkce == USB_fce_1_13)                                              // 'i' informace o systemu (ulozene v EEPROM na adresach 0 az 19 )
          {
            char znak = Serial.peek();
            if (znak == 'x')                                                     // 'ix' specialni funkce pro identifikaci SQM-LU
              {
// #doc#15 
                Serial.println("i,00000002,0000003,00000001,00012345");          // Protocol number, Model number, Feature number,Serial number
              }
            else
              {
                sys_info();            
              }
          }

        if (funkce == USB_fce_1_14)                                              // 'h' vypis hlavicky do terminalu pro pripad, kdy se v terminalu zobrazuji jen hodnoty, ale neni videt, co znamenaji
          {
//            Serial.println(hlavicka);
            print_co_kam(false, false);
          }
    

        if (funkce == USB_fce_1_15)                                              // 'm' kompletni mereni s ulozenim do EEPROM (vysledek se nezobrazuje na displeji, jen se odesle do seriove linky)
          {
            mereni(4);
            
            if(dokoncene_mereni == 2)                                            // mereni dokonceno
              {
                print_co_kam(false, false);                                      //   Serial.println(hlavicka);
                print_co_kam(true, false);                                       //   Serial.println(vystupni_retezec);
              }
          }

        if (modul_LSM303DLHC == true)
          {
            if (funkce == USB_fce_1_20)                                          // 'n' zmereni a zobrazeni aktualniho naklonu krabicky
              {
                float akt_naklon = (uhel() - 10000);                             // zjisteni aktualniho uhlu zvetseneho o 1000 stupnu v desetinach stupne a prevod na desetinne cislo
                Serial.print(lng229);                                            // " Aktualni naklon: "
                Serial.println(akt_naklon/10.0 ,1);
              }
          }

        if (funkce == USB_fce_1_21)                                              // 'x' jedno mereni osvetleni s vystupem v luxech vcetne nastaveni spravneho rozsahu - bez prumerovani
          {
            Serial.print(luxometr_1x());
            Serial.println( "[lx]");
          }


        if (modul_LSM303DLHC == true)
          {
            if (funkce == USB_fce_1_22)                                          // 's' zmeri azimut namireni cidla
              {
                int16_t hodnota = zjisti_azimut();
                Serial.print (lng269);                                           // "Azimut: "
                Serial.println (hodnota);
              }
          }
        if (funkce == USB_fce_1_24)                                              // 'R ....' vypise posledni zaznam ve formatu pro SQM-LU
          {
            vypis_LU();
          }




// ----------------------------------------------------------------------------
//                           S  M  A  Z  A  T     !!

        if (funkce == '=')                                                       // '=' testy
          {
          }

// ----------------------------------------------------------------------------


        if (funkce == USB_fce_1_08)                                              // 'r' cteni hodnot z EEPROM a jejich zobrazeni v terminalu
          {
            vypisove_funkce();
          }

        if (funkce == USB_fce_1_16)                                              // 'k' mereni a zobrazeni hodnot bez korekce
          {
            bezkorekcni_funkce();
          }


        while (Serial.available())  Serial.read();                               // pripadny zbytek dat v prijimacim bufferu se smaze

      }
  }
//----------------------------------------------



//----------------------------------------------
void vypisove_funkce(void)                                                       // funkce zacinala znakem 'r'
  {
    delay(10);
    if (Serial.available())                                                      // za znakem 'r' jeste neco nasleduje
      {
        char podfunkce = Serial.read();
        if (podfunkce == USB_fce_1_09)                                           // 'rp' = vypise posledni zaznam
          {
            vypis_EEPROM(65534,0);
          }
        if (podfunkce == USB_fce_1_10)                                           // 'ra' = vypise vsechny zaznamy
          {
            vypis_EEPROM(65533,0);
          }

        if (podfunkce == USB_fce_1_19)                                           // 'rh' = vypise vsechny zaznamy za poslednich 'cas_hloubka' hodin
          {
            uint16_t cas_hloubka = Serial.parseInt();
            vypis_EEPROM(65530,cas_hloubka);
          }


        if (podfunkce == USB_fce_1_11)                                           // 'rz' = vypise jeden zvoleny zaznam
          {
            uint16_t polozka = Serial.parseInt();
            if (polozka == 0)                                                    // pri zadani 0 vypise posledni zaznam
              {
                polozka = (adr_dalsiho_zaznamu - eeaddr_MIN_EEPROM) / delka_zaznamu;
              }

            vypis_EEPROM(polozka,0);                                             // polozky jsou indexovane od 1 (prvni zaznam na adrese 'eeaddr_MIN_EEPROM' ma index 1)
          }

        if (podfunkce == USB_fce_1_23)                                           // 'rx' = vypise jeden zvoleny zaznam ve formatu pro SQM-LU
          {
            vypis_LU();
          }

        if (podfunkce == USB_fce_1_26)                                           // 'rd' = vypise jeden zvoleny zaznam v integerovem datovem formatu
          {
            uint16_t polozka = Serial.parseInt();
            if (polozka == 0)                                                    // pri zadani 0 vypise posledni zaznam
              {
                polozka = (adr_dalsiho_zaznamu - eeaddr_MIN_EEPROM) / delka_zaznamu;
              }
            vypis_EEPROM(polozka,0,1);                                           // polozky jsou indexovane od 1 (prvni zaznam na adrese 'eeaddr_MIN_EEPROM' ma index 1)
          }

        
      }
    else                                                                         // byla volana pouze funkce 'r' (vypis poslednich 100 zaznamu)
      {
        vypis_EEPROM(65535,0);        
      }
    
  }
//----------------------------------------------



//----------------------------------------------
void bezkorekcni_funkce(void)                                                    // funkce zacinala znakem 'k'
  {
    delay(10);
    if (Serial.available())                                                      // za znakem 'k' jeste neco nasleduje
      {
        char podfunkce = Serial.read();
        if (podfunkce == USB_fce_1_17)                                           // 'ks'   zobrazeni hodnoty svetla primo z cidla (bez kalibracni korekce)
          {
            Serial.print(lng013);                                                //  Serial.print("Plosny jas bez kalibracni korekce: ");
            mereni(8);
            if(dokoncene_mereni == 2)                                            // mereni dokonceno
              {
                uint16_t pom_svetlo = (pole_EEPROM[5] * 256) + pole_EEPROM[6];
                Serial.print(citelne_svetlo(pom_svetlo),3);
                Serial.println(" mag/arcsec2 ");          
              }
            
          }

        if (senzor_BME == true)
          {
            if (podfunkce == USB_fce_1_18)                                       // 'kt'    zobrazeni hodnoty teploty primo z cidla (bez kalibracni korekce)
              {
                Serial.print(lng014);                                            //  Serial.print("Teplota bez kalibracni korekce: ");
                uint16_t pom_teplota = teplota(false);
                Serial.print(pom_teplota);
                Serial.print("  =  ");
                Serial.print(citelna_teplota(pom_teplota));
                Serial.println("'C ");            
              }
          }
      }
  }
//----------------------------------------------

// funkce pro ovladani SQM programem z PC
void PC_funkce(void)                                                       // funkce zacinala znakem '*', dalsi znaky jsou nastavene na pevno bez moznosti editace v souboru "jazyky.h"
  {
    delay(10);
    if (Serial.available())                                                // za znakem '*' jeste neco nasleduje
      {
        char podfunkce = Serial.read();
        if (podfunkce == 'm')                                              // '*m' mereni spustene pres seriovou linku, ale bez vystupu do terminalu
          {
             mereni(10);
          }

        if (podfunkce == 'n')                                              // '*n' zadost o cislo posledniho zaznamu
          {
            Serial.println(((adr_posl_zaznamu - eeaddr_MIN_EEPROM)/delka_zaznamu)+1);
          }

        if (podfunkce == 'a')                                              // '*a' test na shodu adresy
          {
            uint8_t pozadovana_adresa = Serial.parseInt();
            if (pozadovana_adresa == slave_addr)
             {
               Serial.print('1');
             }
          }

        if (podfunkce == 'r')                                              // '*r' = vypise jeden zvoleny zaznam v integerovem datovem formatu
          {
            uint16_t polozka = Serial.parseInt();
            if (polozka == 0)                                              // pri zadani 0 vypise posledni zaznam
              {
                polozka = (adr_dalsiho_zaznamu - eeaddr_MIN_EEPROM) / delka_zaznamu;
              }
            vypis_EEPROM(polozka,0,1);                                     // polozky jsou indexovane od 1 (prvni zaznam na adrese 'eeaddr_MIN_EEPROM' ma index 1)
          }

        if (podfunkce == 's')                                              // '*s' = simulace slunecni soustavy
          {
            uint16_t vstup_planeta = 1;
            uint16_t vstup_pocet_dni = 1;
            uint16_t vstup_den = 1;
            uint16_t vstup_mes = 1;
            uint16_t vstup_rok = 2000;           
            
            bool stav_zadani = true;                                       // pro testy zadanych hodnot (na zacatku je vsechno v poradku, pri problemu se vypadne z bloku) 
            delay(100);

            vstup_planeta = Serial.parseInt();                             // blok pro zadani cisla planety
            if (vstup_planeta > 8)
              {            
                stav_zadani = false;
                Serial.print(lng006);                                      //  Serial.print("Vstup mimo rozsah ");
                Serial.println("(0 - 8)");
                for (uint8_t i = 0 ; i < 9 ; i++)                             // mini info s oznacenim planet
                  {
                    Serial.print(i);
                    Serial.print(" ... ");
                    Serial.println(planety[i]);

                  }

              } 

            if (stav_zadani == true)                                       // blok pro zadani poctu simulacnich kroku
              {
                vstup_pocet_dni = Serial.parseInt();
                if (vstup_pocet_dni > 20000)
                  {
                    stav_zadani = false;
                    Serial.print(lng006);                                  //  Serial.print("Vstup mimo rozsah ");
                    Serial.println("(0=PC, 1 - 20000)");
                  }
              }

            if (stav_zadani == true)                                       // blok pro zadani tri hodnot datumu
              {
                vstup_den = Serial.parseInt();
                vstup_mes = Serial.parseInt();
                vstup_rok = Serial.parseInt();

                bool OKdatum = test_datumu(vstup_den , vstup_mes , vstup_rok);  // test, ze dny mesice a roky jsou ve spravnych intervalech 
                
                if (OKdatum == false)
                  {
                    stav_zadani = false;
                    Serial.print(lng006);                                  //  Serial.print("Vstup mimo rozsah ");
                    Serial.println("(1.1.2000 - 31.12.2080)");
                  }
              }

            while (Serial.available())                                     // vsechny ostatni znaky se ze seriove linky smazou                                                                  
              {
                delay(10);
                Serial.read();
              }



            if (stav_zadani == true)
              {    
                simulace(vstup_planeta, vstup_pocet_dni , vstup_den, vstup_mes , vstup_rok);
              }
          }



        if (podfunkce == '?')                                              // '*?' napoveda pro PC funkce (vypis seznamu prikazu)
          {
            ser_napoveda5();
          }

        if (podfunkce == 'e')                                              // '*e' = cteni hodnoty z EEPROM
          {
            uint32_t adresa_eeprom = Serial.parseInt();
            if (adresa_eeprom < 200000UL)      Serial.println(EEPROM_read(adresa_eeprom));              // pro adresy 0 az 128k se cte zakladni EEPROM
            else                               Serial.println(EEPROM_read30(adresa_eeprom-200000UL));   // pro adresy nad 200000 se cte rozsirena EEPROM
          }



        if (podfunkce == 'f')                                              // '*f' zadost o seznam znakovych kodu vsech pouzitych funkci 
          {                                                                //              (kvuli zachovani komunikace s PC pri uzivatelske zmene jazyka, nebo znaku)
            index_kodu = 0;                                                // index prvniho ovladaciho kodu se nastavi na 0
            Serial.print("CHAR:008:");                                     // verze datoveho bloku 
            Serial.print(kod_jazyka);                                      //       "CZ" nebo "EN"
            Serial.println(':');
            ovladaci_znak = ' ';                         // cela nasledujici sada je bez ovladaciho znaku
            posli_kod_f(USB_fce_1_01);                   //  [0] ... v = zmer vlhkost - bez ukladani
            posli_kod_f(USB_fce_1_02);                   //  [1] ... t = zmer teplotu - bez ukladani
            posli_kod_f(USB_fce_1_03);                   //  [2] ... p = zmer tlak    - bez ukladani
            posli_kod_f(USB_fce_1_04);                   //  [3] ... j = zmer plosny jas - bez ukladani
            posli_kod_f(USB_fce_1_05);                   //  [4] ... z = prepnout zonu ("z0" pro zimu nebo "z1" pro leto)
            posli_kod_f(USB_fce_1_06);                   //  [5] ... a = nastaveni automatickeho spousteni 0-255 minut
            posli_kod_f(USB_fce_1_07);                   //  [6] ... e = pri pouziti expanzni desky prepne na zvolene cidlo (1-7)
            posli_kod_f(USB_fce_1_08);                   //  [7] ... r = vypis zaznamu z EEPROM (bez dalsiho parametru vypise poslednich 100 zaznamu)
            posli_kod_f(USB_fce_1_09);                   //  [8] ...   rp = vypis posledniho zaznamu
            posli_kod_f(USB_fce_1_10);                   //  [9] ...   ra = vypis vsech zaznamu
            posli_kod_f(USB_fce_1_11);                   //  [10] ...   rz = vypis jednoho vybraneho zaznamu
            posli_kod_f(USB_fce_1_19);                   //  [11] ...   rh = vypis zaznamu za posledni obdobi (treba za posledni den)
            posli_kod_f(USB_fce_1_23);                   //  [12] ...   rx = vypis posledniho zaznamu ve formatu pro software od Unihedronu
            posli_kod_f(USB_fce_1_26);                   //  [13] ...   rd = vypis jednoho vybraneho zaznamu v datovem formatu
            posli_kod_f(USB_fce_1_12);                   //  [14] ... c = aktualni datum a cas
            posli_kod_f(USB_fce_1_13);                   //  [15] ... i = informace o aktualnim nastaveni systemu
            posli_kod_f(USB_fce_1_25);                   //  [16] ...   ix = informace o zarizeni ve formatu pro software od Unihedronu
            posli_kod_f(USB_fce_1_14);                   //  [17] ... h = zobrazeni hlavicky pro formatovany vystup
            posli_kod_f(USB_fce_1_15);                   //  [18] ... m = spusteni vsech mereni se zapisem do EEPROM
            posli_kod_f(USB_fce_1_16);                   //  [19] ... k = kalibrace
            posli_kod_f(USB_fce_1_17);                   //  [20] ...   kj = kalibrace jasu
            posli_kod_f(USB_fce_1_18);                   //  [21] ...   kt = kalibrace teploty
            posli_kod_f(USB_fce_1_20);                   //  [22] ... n = aktualni naklon
            posli_kod_f(USB_fce_1_21);                   //  [23] ... x = zmer osvetleni (lux)
            posli_kod_f(USB_fce_1_22);                   //  [24] ... s = zjisti smer natoceni (azimut)
            posli_kod_f(USB_fce_1_24);                   //  [25] ... R = vypis posledniho zaznamu ve formatu pro software od Unihedronu
            posli_kod_f(USB_fce_1_27);                   //  [26] ... b = nastaveni jednoho z 5 budiku  "b 2 15:48"
            posli_kod_f(USB_fce_1_28);                   //  [27] ... o = nastaveni odpoctu

            delay(200);

            ovladaci_znak = '#';                         // cela nasledujici sada je s ovladacim kodem '#'
            posli_kod_f(USB_fce_2_01);                   //  [28] ... #T = nastaveni datumu a casu
            posli_kod_f(USB_fce_2_33);                   //  [29] ...   #TG = nastaveni datumu a casu z GPS
            posli_kod_f(USB_fce_2_02);                   //  [30] ... #P = nastaveni poctu prumerovani
            posli_kod_f(USB_fce_2_03);                   //  [31] ... #S = nastaveni urovne stability (0-255)
            posli_kod_f(USB_fce_2_04);                   //  [32] ... #I = detailni informace o kalibraci vnitrnich hodin
            posli_kod_f(USB_fce_2_05);                   //  [33] ... #A = nastaveni SLAVE adresy pro komunikaci (1-15)
            posli_kod_f(USB_fce_2_06);                   //  [34] ... #C = zapnuti, nebo vypnuti kontroly CRC pro seriovou komunikaci (#C0=CRC se nekontroluje; #C1=CRC se kontroluje; #C2=jako #C1 ale s vypisem chyb do USB)
            posli_kod_f(USB_fce_2_07);                   //  [35] ... #O = nastaveni oddelovacu
            posli_kod_f(USB_fce_2_08);                   //  [36] ...   #Oo = oddelovac polozek (nasleduje jeste jeden znak, ktery urcuje oddelovac: #Oo, ; #Oo; ;  #Oo_ ; #Oot)
            posli_kod_f(USB_fce_2_09);                   //  [37] ...   #O, = carka jako desetinny oddelovac  (#O,0 = vypnout carku, zapnout tecku   #O,1 = zapnout carku, vypnout tecku)
            posli_kod_f(USB_fce_2_10);                   //  [38] ...   #O" = uzavirat nebo neuzavirat polozky do uvozovek   (#O"1 = uzavirat #O"0 = neuzavirat)
            posli_kod_f(USB_fce_2_11);                   //  [39] ...   #Oh = zapisovat nebo nezapisovat hlavicku do souboru (#Oh0 = nezapisovat #Oh1 = zapisovat)
            posli_kod_f(USB_fce_2_12);                   //  [40] ... #F = formatovani EEPROM (nasleduje jeste jeden znak pro urceni zpusobu mazani dat)
            posli_kod_f(USB_fce_2_13);                   //  [41] ...   #FS = SOFT format
            posli_kod_f(USB_fce_2_14);                   //  [42] ...   #FH = HARD format
            posli_kod_f(USB_fce_2_15);                   //  [43] ... #U = hranice jasu pro ukladani zaznamu do EEPROM
            posli_kod_f(USB_fce_2_16);                   //  [44] ... #K = zapis bodu kalibracni tabulky (nasleduje jeden znak pro urceni tabulky)
            posli_kod_f(USB_fce_2_17);                   //  [45] ...   #KJ = kalibracni tabulka pro svetlo
            posli_kod_f(USB_fce_2_18);                   //  [46] ...   #KT = kalibracni tabulka pro teplotu
            posli_kod_f(USB_fce_2_19);                   //  [47] ... #E = editace pozorovacich stanovist
            posli_kod_f(USB_fce_2_20);                   //  [48] ...   #ET = textove popisy
            posli_kod_f(USB_fce_2_21);                   //  [49] ...   #EG = graficke popisy na displeje
            posli_kod_f(USB_fce_2_22);                   //  [50] ... #N = nastaveni naklonomeru
            posli_kod_f(USB_fce_2_24);                   //  [51] ...   #Ni = informace o naklonomeru
            posli_kod_f(USB_fce_2_23);                   //  [52] ...   #Nk = kalibrace naklonomeru
            posli_kod_f(USB_fce_2_27);                   //  [53] ... #X = kalibrace luxmetru
            posli_kod_f(USB_fce_2_28);                   //  [54] ...   #Xa = parametr 'a'
            posli_kod_f(USB_fce_2_29);                   //  [55] ...   #Xb = parametr 'b'
            posli_kod_f(USB_fce_2_30);                   //  [56] ...   #Xc = parametr 'c'
            posli_kod_f(USB_fce_2_31);                   //  [57] ... #M = Magnetometr (kompas)
            posli_kod_f(USB_fce_2_32);                   //  [58] ...   #Mk = kalibrace
            posli_kod_f(USB_fce_2_35);                   //  [59] ...   #Mo = offset kompasu (doladeni spatne nalepeneho cidla)
            posli_kod_f(USB_fce_2_36);                   //  [60] ...   #Mi = informace o magnetometru
            posli_kod_f(USB_fce_2_34);                   //  [61] ... #D = nastaveni AfD

            delay(200);

            ovladaci_znak = '@';                         // cela nasledujici sada je s ovladacim kodem '@'
            posli_kod_f(USB_fce_3_01);                   //  [62] ... @E = test postupneho prepinani cidel na expanzni desce
            posli_kod_f(USB_fce_3_02);                   //  [63] ... @X = nastaveni poctu vstupu na expanzni desce (1-7)
            posli_kod_f(USB_fce_3_03);                   //  [64] ... @R = prime cteni dat z EEPROM
            posli_kod_f(USB_fce_3_04);                   //  [65] ... @W = primy zapis hodnoty do EEPROM
            posli_kod_f(USB_fce_3_05);                   //  [66] ... @D = obnoveni defaultnich hodnot (nasleduje 1 znak pro urceni toho, co se ma obnovit)
            posli_kod_f(USB_fce_3_06);                   //  [67] ...   @DS = obnoveni kalibracnich hodnot pro svetlo
            posli_kod_f(USB_fce_3_07);                   //  [68] ...   @DT = obnoveni kalibracnich hodnot pro teplotu
            posli_kod_f(USB_fce_3_08);                   //  [69] ...   @DP = obnoveni parametru systemu
            posli_kod_f(USB_fce_3_10);                   //  [70] ... @@ = reset
            posli_kod_f(USB_fce_3_11);                   //  [71] ... @G = vypis dat z GPS modulu
            posli_kod_f(USB_fce_3_19);                   //  [72] ...   @Gs = nastaveni domaci zemepisne sirky (latitude) pro pozorovaci stanoviste (pouziva se pro vypocet elevace Slunce a Mesice)
            posli_kod_f(USB_fce_3_20);                   //  [73] ...   @Gd = nastaveni domaci zemepisne delky (longitude) pro pozorovaci stanoviste (pouziva se pro vypocet elevace Slunce a Mesice)
            posli_kod_f(USB_fce_3_21);                   //  [74] ...   @Gz = nastaveni domaci casove zony pro "zimni" cas
            posli_kod_f(USB_fce_3_22);                   //  [75] ...   @Gl = nastaveni domaci casove zony pro "letni" cas
            posli_kod_f(USB_fce_3_23);                   //  [76] ...   @Gp = nastaveni textovych popisku pro zimni a letni cas
            posli_kod_f(USB_fce_3_12);                   //  [77] ... @S = nastaveni rychlosti seriove komunikace
            posli_kod_f(USB_fce_3_15);                   //  [78] ... @> = vypis souboru "RTC_set.csv" do seriove linky
            posli_kod_f(USB_fce_3_16);                   //  [79] ... @h = vypnuti zapisu informaci o nastaveni RTC obvodu do souboru "RTC_set.csv
            posli_kod_f(USB_fce_3_63);                   //  [80] ... @H = zapnuti zapisu informaci o nastaveni RTC obvodu do souboru "RTC_set.csv
            posli_kod_f(USB_fce_3_18);                   //  [81] ... @* = nezdokumentovana servisni funkce pro vypis jednoho zaznamu z EEPROM v HEX formatu
            posli_kod_f(USB_fce_3_24);                   //  [82] ... @Z = zapsat systemove parametry na SD kartu
            posli_kod_f(USB_fce_3_25);                   //  [83] ... @O = obnovit systemove parametry z SD karty
            posli_kod_f(USB_fce_3_26);                   //  [84] ... @T = Funkce pro otestovani HW
            posli_kod_f(USB_fce_3_27);                   //  [85] ... @m = vypnout modbus
            posli_kod_f(USB_fce_3_28);                   //  [86] ... @M = zapnout modbus
            posli_kod_f(USB_fce_3_33);                   //  [87] ... @V = verze SQM
            posli_kod_f(USB_fce_3_34);                   //  [88] ...   @Vd = displejova verze
            posli_kod_f(USB_fce_3_35);                   //  [89] ...   @Vl = LED verze
            posli_kod_f(USB_fce_3_36);                   //  [90] ... @L = blokovani/povolovani RGB LED
            posli_kod_f(USB_fce_3_37);                   //  [91] ...   @Lb = blokovani  RGB LED (blikani pri testu baterie)
            posli_kod_f(USB_fce_3_38);                   //  [92] ...   @LB = povolovani RGB LED (blikani pri testu baterie)
            posli_kod_f(USB_fce_3_39);                   //  [93] ...   @Lm = blokovani  RGB LED (blikani pri mereni)
            posli_kod_f(USB_fce_3_40);                   //  [94] ...   @LM = povolovani RGB LED (blikani pri mereni)
            posli_kod_f(USB_fce_3_41);                   //  [95] ...   @Le = blokovani  RGB LED (blikani pri chybe)
            posli_kod_f(USB_fce_3_42);                   //  [96] ...   @LE = povolovani RGB LED (blikani pri chybe)
            posli_kod_f(USB_fce_3_43);                   //  [97] ...   @Lk = blokovani  RGB LED (blikani pri komunikaci pres RS485)
            posli_kod_f(USB_fce_3_44);                   //  [98] ...   @LK = povolovani RGB LED (blikani pri komunikaci pres RS485)
            posli_kod_f(USB_fce_3_45);                   //  [99] ...   @La = blokovani  RGB LED (blikani pri AfD)
            posli_kod_f(USB_fce_3_46);                   // [100] ...   @LA = povolovani RGB LED (blikani pri AfD)
            posli_kod_f(USB_fce_3_67);                   // [101] ...   @Lt = blokovani  RGB LED (blikani pri GPS trasovani)
            posli_kod_f(USB_fce_3_68);                   // [102] ...   @LT = povolovani RGB LED (blikani pri GPS trasovani)
            posli_kod_f(USB_fce_3_17);                   // [103] ... @P = povolit/zakazat pipani
            posli_kod_f(USB_fce_3_47);                   // [104] ...   @Pb = vypnout pipani pri zmene urovne napeti baterie
            posli_kod_f(USB_fce_3_48);                   // [105] ...   @PB = zapnout pipani pri zmene urovne napeti baterie
            posli_kod_f(USB_fce_3_49);                   // [106] ...   @Pb = vypnout pipani pri mereni
            posli_kod_f(USB_fce_3_50);                   // [107] ...   @PB = zapnout pipani pri mereni
            posli_kod_f(USB_fce_3_51);                   // [108] ...   @Pa = vypnout pipani pri alarmu (budiku) / odpoctu
            posli_kod_f(USB_fce_3_52);                   // [109] ...   @PA = zapnout pipani pri alarmu (budiku) / odpoctu
            posli_kod_f(USB_fce_3_53);                   // [110] ...   @Pe = vypnout pipani pri HW chybe
            posli_kod_f(USB_fce_3_54);                   // [111] ...   @PE = zapnout pipani pri HW chybe
            posli_kod_f(USB_fce_3_55);                   // [112] ...   @Pt = vypnout pipani pri operacich s bocnim tlacitkem
            posli_kod_f(USB_fce_3_56);                   // [113] ...   @PT = zapnout pipani pri operacich s bocnim tlacitkem
            posli_kod_f(USB_fce_3_57);                   // [114] ...   @Po = vypnout pipani pri mireni na objekt (RA-DEC)
            posli_kod_f(USB_fce_3_58);                   // [115] ...   @PO = zapnout pipani pri mireni na objekt (RA-DEC)
            posli_kod_f(USB_fce_3_59);                   // [116] ...   @Pv = vypnout pipani pri vystahach
            posli_kod_f(USB_fce_3_60);                   // [117] ...   @PV = zapnout pipani pri vystrahach
            posli_kod_f(USB_fce_3_61);                   // [118] ...   @Ps = vypnout pipani pri stopkach
            posli_kod_f(USB_fce_3_62);                   // [119] ...   @PS = zapnout pipani pri stopkach
            posli_kod_f(USB_fce_3_65);                   // [120] ...   @Pz = vypnout pipnuti pri zapnuti napajeni
            posli_kod_f(USB_fce_3_66);                   // [121] ...   @PZ = zapnout pipnuti pri zapnuti napajeni
            posli_kod_f(USB_fce_3_64);                   // [122] ...   @N = nastaveni elevace Slunce pro uzivatelsky definovany soumrak
            posli_kod_f(USB_fce_3_69);                   // [123] ...   @A = automaticke odesilani dat do seriove linky

            delay(200);

            ovladaci_znak = '%';                         // cela nasledujici sada je s ovladacim kodem '%'
            posli_kod_f(USB_fce_4_01);                   // [124] ... %I = detailni informace
            posli_kod_f(USB_fce_4_02);                   // [125] ... %L = vypis poslednich 24 hod. do seriove linky
            posli_kod_f(USB_fce_4_03);                   // [126] ... %Lh = poslednich 'n' hod. do seriove linky
            posli_kod_f(USB_fce_4_04);                   // [127] ...   %La = kompletni vypis do seriove linky
            posli_kod_f(USB_fce_4_05);                   // [128] ...   %Lz = poslednich 'n' zaznamu do seriove linky
            posli_kod_f(USB_fce_4_07);                   // [129] ...   %Lb = bajtovy vypis
            posli_kod_f(USB_fce_4_06);                   // [130] ... %S = ulozeni na SD kartu
            posli_kod_f(USB_fce_4_08);                   // [131] ... %P = pozastavit mereni
            posli_kod_f(USB_fce_4_09);                   // [132] ... %R = pokracovat v mereni
            posli_kod_f(USB_fce_4_10);                   // [133] ... %C = nastaveni mericiho intervalu (10-255)
            posli_kod_f(USB_fce_4_11);                   // [134] ... %F = formatovani pridavne EEPROM
            posli_kod_f(USB_fce_4_12);                   // [135] ... %N = nastaveni na nulovou pozici
            posli_kod_f(USB_fce_4_13);                   // [136] ... %W = primy zapis do pridavne EEPROM
            posli_kod_f(USB_fce_4_14);                   // [137] ... %X = cteni dat z pridavne EEPROM

            ovladaci_znak = '*';                         // cela nasledujici sada je s ovladacim kodem '*'
            posli_kod_f('m');                            // [138] ...     "*m" mereni spustene pres seriovou linku, ale bez vystupu do terminalu
            posli_kod_f('n');                            // [139] ...     "*n" zadost o cislo posledniho zaznamu
            posli_kod_f('a');                            // [140] ...     "*a" test na shodu adresy
            posli_kod_f('r');                            // [141] ...     "*r" jeden zaznam v datovem formatu
            posli_kod_f('f');                            // [142] ...     "*f" zadost o seznam znakovych kodu vsech pouzitych funkci 
            posli_kod_f('s');                            // [143] ...     "*s" simulace planet slunecni soustavy
            posli_kod_f('e');                            // [144] ...     "*e" cteni jedne hodnoty z EEPROM

        //--------------------------------------------------------------------------
        //    Pokud by bylo nutne zmenit v predchozich blocich i ovladaci znak, je to mozne.
        //    Jen se pred prikaz umisti spravne nastaveni ovladaciho znaku a po prikazu se zase vrati ovladaci znak na puvodni hodnotu.
        //                     Hlavne neposouvat indexy!
        //
        //    Pripadne rozsirujici funkce v dalsich verzich programu se musi uvadet az pod toto misto
        //    Nevkladat do predchoziich bloku, aby se neposunuly existujici indexy.
        //    PC program pocita s tim, ze  napriklad index [136] bude vzdycky prikaz pro zapis do pridavne EEPROM
        //    Nezapomenout na nastaveni spravneho ovladaciho znaku '#', '%', '@', ' ', '*'.
        //    Maximalni povoleny index je 255 (mximalni pocet ovladacich prikazu)
        // 
        //  PRIKLAD:
        //
        //  ovladaci_znak = '#';                         // 
        //  posli_kod_f(USB_fce_2_34);                   // [145] ...     "#X" nejaka dalsi funkce
        // 
        //  ovladaci_znak = '@';                         // 
        //  posli_kod_f(USB_fce_3_70);                   // [146] ...     "@Q" nejaka dalsi funkce
        //--------------------------------------------------------------------------

            Serial.println(":END");                      // zakoncovaci znacka + <CR><LF>

          }

      
      }
    while (Serial.available())  Serial.read();                                   // pripadny zbytek dat v prijimacim bufferu se smaze
  } 


//----------------------------------------------
void servisni_funkce(void)                                                       // funkce zacinala znakem '#'
  {
    delay(10);
    if (Serial.available())                                                      // za znakem '#' jeste neco nasleduje
      {
        char podfunkce = Serial.read();

        if (podfunkce == '?')                                                    // "#?" = napoveda pro servisni funkce
          {
            ser_napoveda2();
          }

        if (podfunkce == USB_fce_2_01)                                           // "#T" = nastaveni casu
          {
            podfunkce = Serial.peek();
            if (podfunkce == USB_fce_2_33)                                       // "#TG" = nastaveni casu pres GPS
              {
                gps(3);                                                          // kdyz je v GPS signalu dostupny cas, nastavi se vnitrni hodiny
              }
            else
              {
                nastav_RTC();                                                    // vnitrni hodiny se nastavi podle nasledujiciho retezce cisel "yyyymmddhhnnss"
                STM_DS(false);                                                   // serizeni casu v DS3231 podle STM32 (false = bez 30-sekundoveho testu)             
              }
            zobraz_RTC(true);                                                    // po nastaveni casu se ulozeny cas hned zobrazi            
          }

      
        if (podfunkce == USB_fce_2_02)                                           // "#P" = nastaveni poctu prumerovani
          {
            delay(20);
            int16_t pomprom = Serial.parseInt();
            if (pomprom >0 and pomprom < 21)
              {
                EEPROM_update(eeaddr_prumerovani,pomprom);
                prumery = pomprom;
                MODdata[41] = prumery;                  // pocet prumerovani
                Serial.print(lng015);                                            //  Serial.print("Prumerovat ");
                Serial.print(prumery);
                Serial.println(lng016);                                          //  Serial.println(" mereni");
              }
            else
              {
                Serial.print(lng006);                                            //  Serial.print("Vstup mimo rozsah ");
                Serial.println(lng017);                                          //  Serial.println("(1-20)");
              }
          }

        if (podfunkce == USB_fce_2_03)                                           // "#S" = nastaveni urovne pro vyhodnoceni stabilniho jasu
          {
            delay(20);
            long pomprom;
            pomprom = Serial.parseInt();
            if (pomprom >= 0 and pomprom <= 255)                                     // kdyz je zadani mimo rozsah, jen se zobrazi puvodni hodnota
              {
                rozhod_stab = pomprom;
                EEPROM_update(eeaddr_stab,rozhod_stab);
              }
            MODdata[44] = rozhod_stab;                                           // rozhodovaci uroven  pro stabilni jas
            Serial.print(lng018);                                                //  Serial.print("Rozhodovaci uroven pro stabilni jas je nastavena na ");
            Serial.print(rozhod_stab / 10.0,1);
            Serial.println(" %");
          }

        if (podfunkce == USB_fce_2_04)                                           // "#I" = informace o poslednim serizeni casu a o automaticke casove korekci
          {
            RTC_info();
          }

        if (podfunkce == USB_fce_2_05)                                           // "#A" = nastaveni SLAVE adresy
          {
            delay(20);
            long pomprom = Serial.parseInt();
            if (pomprom > 0 and pomprom < 16)
              {
                uint8_t puvodni_baudrate = (EEPROM_read(eeaddr_SLAVE_baud) & 0b00110000);
                slave_addr = pomprom;
                pomprom = pomprom | puvodni_baudrate;                
                EEPROM_update(eeaddr_SLAVE_baud,pomprom);
                Serial.print(lng019);                                            //  Serial.print("SLAVE addr: ");
                Serial.println(pomprom & 0b00001111);
                MBslave.setID(slave_addr);                                       // zmena ID pro modbusovou komunikaci
                MBslave.start();
              }
            else
              {
                Serial.print(lng006);                                            //  Serial.print("Vstup mimo rozsah");
                Serial.println(lng020);                                          //  Serial.println("(1-15)");
              }

          }

        if (podfunkce == USB_fce_2_06)                                           // "#C" = zapnuti, nebo vypnuti kontroly prijimaneho CRC bajtu
          {
            delay(20);
            long param = Serial.parseInt();
            uint8_t adresa15 = EEPROM_read(eeaddr_CRC_tset_lock);                   // na adrese 15 v EEPROM je krome typu testu CRC ulozena i informace o zamknuti bocniho tlacitka 
            if (param == 0) 
              {
                pouzij_CRC = 0;
                Serial.println(lng021);                                          //  Serial.println("CRC se nekontroluje");
                EEPROM_update(eeaddr_CRC_tset_lock, (adresa15 & 0b11111100));    //  zachovaji se nejvyssi bity a bity 1 a 0 se nastavi na 0b00
              }
            if (param == 1)
              {
                pouzij_CRC = 1;
                Serial.println(lng022);                                          //  Serial.println("CRC se testuje");
                EEPROM_update(eeaddr_CRC_tset_lock, ((adresa15 & 0b11111100) | 0b00000001));       //  zachovaji se nejvyssi bity a bity 1 a 0 se nastavi na 0b01
              }            
            if (param == 2)
              {
                pouzij_CRC = 2;
                Serial.println(lng023);                                          //  Serial.println("CRC se testuje, chyby se hlasi do USB seriove linky");
                EEPROM_update(eeaddr_CRC_tset_lock, ((adresa15 & 0b11111100) | 0b00000010));       //  zachovaji se nejvyssi bity a bity 1 a 0 se nastavi na 0b10
              }            
          
          }

        if (podfunkce == USB_fce_2_07)                                           // "#O" = nastaveni oddelovacu pri vystupu zaznamu do terminalu, nebo na SD kartu
          {
            nastaveni_oddelovacu();
          }

        if (podfunkce == USB_fce_2_12)                                           // "#F" = format EEPROM
          {
            delay(20);
            if (Serial.available())
              {
                char param = Serial.read();
                if (param == USB_fce_2_13)                                       // "#FS"      SOFT format EEPROM
                  {
                    soft_format_EEPROM();
                  }

                if (param == USB_fce_2_14)                                       // "#FH"      HARD format EEPROM
                  {
                    hard_format_EEPROM();
                  }
              }
          }


        if (podfunkce == USB_fce_2_15)                                           // "#U" = hranice svetla pro ukladani zaznamu
          {
            delay(20);
            long pomprom = Serial.parseInt();
            if (pomprom >= 0 and pomprom <= 255)
              {
                EEPROM_update(eeaddr_uroven_EEPROM,pomprom);
                hranice_ukladani = pomprom;
                MODdata[45] = hranice_ukladani;                                  // hranice jasu pro ukladani do EEPROM
                Serial.print(lng024);                                            //  Serial.print("Do EEPROM se budou ukladat zaznamy s hodnotou plosneho jasu vetsi (tmavsi) nez ");
                Serial.print(hranice_ukladani / 10.0);
                Serial.println(" mag/arcsec2");
              }
            else
              {                
                Serial.print(lng006);                                            //  Serial.print("Vstup mimo rozsah ");
                Serial.println(lng025);                                          //  Serial.println("(0-255)");
              }
          }


        if (podfunkce == USB_fce_2_34)                                           // "#D" = hranice svetla pro AfD
          {
            delay(20);
            float pomprom = Serial.parseFloat();
            if (pomprom <= 25.4 and pomprom > 13.0)
              {
                afd_bzuk = true;                                                 // po nastaveni AfD pres seriovou linku se povoli napis  "-AFd-" na displeji a bzukani pri AfD
                afd_log = true;                                                  // povoli se i logovani do syslogu
                afd = (pomprom + 0.05) * 10;
                EEPROM_update(eeaddr_AfD,afd);
                Serial.print(lng303);                                            //  Serial.print("AfD nastaven na ");
                Serial.print(afd / 10.0);
                Serial.println(" mag/arcsec2");
                digitalWrite(pin_bok_out, LOW);
                predchozi_cas_afd = millis();
              }
            else
              {
                afd = 255;     
                afd_bzuk = false;
                afd_log = false;                                                 // pri vypnutem AfD se zakaze i logovani AFD do syslogu
                EEPROM_update(eeaddr_AfD,afd);
                digitalWrite(pin_bok_out, LOW);                                  // vypnuti bocniho signalu
                Serial.print(lng006);                                            //  Serial.print("Vstup mimo rozsah ");
                Serial.println("(13.0 - 25.4)");
                Serial.println(lng304);                                          //  Serial.println("AfD vypnuty");
              }
            status_afd = false;
            digitalWrite(pin_bok_out, LOW);
            afd_desetiny = 0;                
            afd_nabito = false;

          }




        if (podfunkce == USB_fce_2_16)                                           // "#K" = zadavani polozek kalibracnich tabulek
          {
            delay(20);
            if (Serial.available())
              {
                char param = Serial.read();
                if (param == USB_fce_2_17)                                       // "#KJ"      Kalibrace svetla
                  {
                    vstup_kalibrace_svetla();
                  }

                if (param == USB_fce_2_18)                                       // "#KT"      Kalibrace teploty
                  {
                    vstup_kalibrace_teploty();
                  }
              }
          }      

        if (podfunkce == USB_fce_2_19)                                           // "#E" = Vypis nebo editace prednastavenych pozorovacich stanovist
          {
            delay(50);
            if (Serial.available())
              {
                char param = Serial.read();
                if (param == USB_fce_2_20 or param == USB_fce_2_21)              // po "#ET" nebo "#EG" nasleduje index pozorovaciho stanoviste
                  {
                    long pomprom = Serial.parseInt();                            // ocekava se znak '1' az '5'
                    if (pomprom > 0 and pomprom < 6)                             // index je v povolenem intervalu 1 az 5 cokoliv jineho rusi funkci a pouze vypise aktualni nastaveni
                      {
                        
                        if (param == USB_fce_2_20)                               // "#ETn "      editace textu
                          {
                            if (Serial.available()) Serial.read();               // prvni znak (mezera) se zahodi

                            uint8_t ukazatel = 0;                                   // ostatni se ulozi do EEPROM
                            while (Serial.available() and ukazatel < 10)         // nikdy se ale neprekroci 10 znaku
                              {
                                char param = Serial.read();
                                if (param < 32 or param > 126 ) param = 32;      // mimo ASCII rozsah se nahrazuje znak mezerou
                                EEPROM_write(ukazatel + eeaddr_stanoviste + ((pomprom-1) * 15) , param );                      
                                ukazatel++;
                              }

                            while (ukazatel < 10)                                // doplneni zbytku textu mezerami
                              {
                                EEPROM_write(ukazatel + eeaddr_stanoviste + ((pomprom-1) * 15) , 32 );                      
                                ukazatel++;
                              }
                          
                          
                          }

                        if (param == USB_fce_2_21)                               // "#EGn "      editace grafiky
                          {                           
                            uint8_t ukazatel = 0;                                     
                            while (Serial.available() and ukazatel < 5 )         // uklada se maximalne 5 cisel
                              {
                                long param = Serial.parseInt();
                                if (param >= 0 and param <=255)
                                  {
                                    EEPROM_write(ukazatel + eeaddr_stanoviste + 10 + ((pomprom-1) * 15) , param );                      
                                  }
                                ukazatel++;
                              }

                            while (ukazatel < 5)                                 // doplneni zbytku znaku na displeji prazdnymi sedmisegmentovkami
                              {
                                EEPROM_write(ukazatel + eeaddr_stanoviste + 10 + ((pomprom-1) * 15) , 0 );                      
                                ukazatel++;
                              }
                            

                          }
                      }
                  }

              }

            for (uint8_t i = 1; i < 6 ; i++)                                        // na zaver jakekoliv funkce #E se provede vypis
              {
                Serial.print (i);
                Serial.print (" - ");
                for (uint8_t j = 0; j < 10 ; j++)
                  {
                    Serial.write (EEPROM_read(j + eeaddr_stanoviste + ((i-1) * 15)));
                  }

                Serial.print (" - ");
                for (uint8_t j = 0; j < 5 ; j++)
                  {
                    srovnej_pod_sebe(EEPROM_read(j + eeaddr_stanoviste + 10 + ((i-1) * 15)), 4);
                    Serial.print (" , ");
                    D_pamet[4-j] = EEPROM_read(j + eeaddr_stanoviste + 10 + ((i-1) * 15));
                  }
                Serial.println(' ');
                aktualizuj_displej();
                delay(1000);                    
              }
          }            


        if (modul_LSM303DLHC == true)
          {
            if (podfunkce == USB_fce_2_22)                                       // "#N" = Naklonomer (s kompasem)
              {
                delay(20);
                if (Serial.available())
                  {
                    char param = Serial.read();
                    if (param == USB_fce_2_23)                                   // "#Nk" Kalibrace naklonomeru
                      {
                        kalibrace_naklon();
                      }
                    if (param == USB_fce_2_24)                                   // "#Ni" informace o nastavenych parametrech (offsety, krajni polohy)
                      {
                        naklonomer_info(true);
                      }
                  }
              } 
    
            
            if (podfunkce == USB_fce_2_31)                                       // "#M" = magnetometr (kompas)
              {
                delay(20);
                if (Serial.available())
                  {
                    char param = Serial.read();
                    if (param == USB_fce_2_32)                                   // "#Mk" Kalibrace kompasu
                      {
                        kalibrace_magnet(true);                                  // true = vystup do seriove linky
                      }
                      
                    if (param == USB_fce_2_35)                                   // "#Mo" nastaveni offsetu kompasu
                      {
                        float pomprom = Serial.parseFloat();                     // zadani cisla -12.7 az + 12.7 stupnu
                        if (pomprom <= 12.7 and pomprom >= -12.7)                // v povolenem rozsahu
                          {
                            pomprom = pomprom * 10;                              //  -127 ... +127
                            pomprom = pomprom  + 128;                            //     1 ... +255
                            EEPROM_update(eeaddr_kompas_offset,(uint8_t)pomprom);
                            offset_kompasu = (pomprom - 128) / 10.0;             // -12.7 ... +12.7
                          }
                        else
                          {
                            Serial.print(lng006);                                //  Serial.println("Vstup mimo rozsah ");
                            Serial.println("(-12.7 ... +12.7)");
                          }
    
                        Serial.print(lng317);                                    //  Serial.print("Kompas pootocen o [stupne]: ");
                        Serial.println(offset_kompasu,1);                        // -12.7 ... +12.7
                      }
                    if (param == USB_fce_2_36)                                   // "#Mi" informace o magnetometru
                      {
                        magnetometr_info(true);
                      }
                  
                  }
              } 
          }

        if (podfunkce == USB_fce_2_27)                                           // "#X" = Kalibrace luxmetru
          {
            delay(20);
            if (Serial.available())
              {
                char param = Serial.read();
                int16_t luxkal = (Serial.parseFloat() + 0.005) * 100.0;          // do EEPROM se bude ukladat cislo v rozsahu -32768 az +32767, takze maximalni mozne zadane cislo je +/- 327.67
                
                if (param == USB_fce_2_28)                                       // "#Xa" Kalibrace luxmetru - parametr 'a'
                  {
                    EEPROM_write_int(eeaddr_luxmetr,luxkal);
                  }

                if (param == USB_fce_2_29)                                       // "#Xb" Kalibrace luxmetru - parametr 'b'
                  {
                    EEPROM_write_int(eeaddr_luxmetr + 2 ,luxkal);
                  }

                if (param == USB_fce_2_30)                                       // "#Xc" Kalibrace luxmetru - parametr 'c'
                  {
                    EEPROM_write_int(eeaddr_luxmetr + 4 ,luxkal);
                  }
              }
            lux_kal_a = ((int16_t)EEPROM_read_int(eeaddr_luxmetr)) / 100.0;                 // nacteni kalibracnich konstant pro luxmetr (v EEPROM mohou byt ulozena i zaporna cisla -32768 az +32767)
            lux_kal_b = ((int16_t)EEPROM_read_int(eeaddr_luxmetr + 2 )) / 100.0;
            lux_kal_c = ((int16_t)EEPROM_read_int(eeaddr_luxmetr + 4)) / 10000000.0;

            Serial.print("lux_kal_a = ");
            Serial.println(lux_kal_a,2); 
            Serial.print("lux_kal_b = "); 
            Serial.println(lux_kal_b,2); 
            Serial.print("lux_kal_c = "); 
            Serial.println(lux_kal_c,7); 

            
          }


      
      }
    while (Serial.available())  Serial.read();                                   // pripadny zbytek dat v prijimacim bufferu se smaze
  }
//----------------------------------------------



//----------------------------------------------
void tajne_funkce(void)                                                          // funkce zacinala znakem '@'
  {
    delay(10);
    if (Serial.available())                                                      // za znakem '@' jeste neco nasleduje
      {
        char podfunkce = Serial.read();
        if (podfunkce == '?')                                                    // "@?" = napoveda pro tajne funkce
          {
            ser_napoveda3();
          }

        if (podfunkce == USB_fce_3_01)                                           // "@E" = test postupneho prepinani cidel na expanzni desce
          {
            prepni_cidlo_svetla();         
            Serial.print(lng012);                                                //  Serial.print("Aktualne je aktivni cidlo: ");
            Serial.print(index_cidla_svetla);
            Serial.print(" / ");
            Serial.println(pocet_cidel_svetla);
          }


        if (podfunkce == USB_fce_3_02)                                           // "@X" = nastaveni poctu vstupu na expanzni desce (1-7)
          {
            long pomprom = Serial.parseInt();
            if (pomprom <= 7 and pomprom >= 0)                                   // je umozneno zadat i 0 (nebo jen @X), ale v tom pripade se nastavi 1 cidlo
              {
                if (pomprom == 0)   pomprom = 1;                                 // nula se automaticky prepise na 1
                EEPROM_update(eeaddr_pocet_cidel,pomprom);
                pocet_cidel_svetla = pomprom;
                Serial.print(lng026);                                            //  Serial.print("Zadany pocet vstupu pro cidla na expanzni desce: ");
                Serial.println(pocet_cidel_svetla);
                
              }
            else
              {
                Serial.print(lng006);                                            //  Serial.print("Vstup mimo rozsah ");
                Serial.println(lng027);                                          //  Serial.println("(1 az 7)");
              }
          }

      
        if (podfunkce == USB_fce_3_03)                                           // "@R" = cteni z EEPROM
          {
            uint32_t adresa = Serial.parseInt();
            adresa &= 0x1FFFF;                                                   // ochrana pred prekrocenim adresy (max = 128kB)       
            uint8_t readbajt = EEPROM_read(adresa);
            Serial.print("EEPROM[");
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
                Serial.println(EEPROM_read_int(adresa));

              }

          
          }
      
        if (podfunkce == USB_fce_3_04)                                           // "@W" = zapis do EEPROM
          {
            uint32_t adresa = Serial.parseInt();
            long wdata = Serial.parseInt();
            adresa &= 0x1FFFF;                                                   // ochrana pred prekrocenim adresy (max = 128kB)       
            Serial.print(lng028);                                                //  Serial.print(" puvodni: EEPROM[");
            Serial.print(adresa);
            Serial.print("] = ");
            Serial.print(EEPROM_read(adresa));
            wdata &= 0xFF;                                                       // ochrana pred prekrocenim rozsahu dat (0 az 255)             
            EEPROM_write(adresa, wdata);            

            Serial.print("   <-  ");
            Serial.println(EEPROM_read(adresa));
            // v nekterych specialnich pripadech je po zapisu do EEPROM nutny restart
            switch (adresa)
              {
                case eeaddr_MOD_autoupdate:                                       // zmena automatickeho odesilani dat do seriove linky
                case eeaddr_bortle:                                               // povoleni nebo zakazani polozky "bortle" v menu "JAS"
                SOFT_RESET();
                break;
              }
            
          }

        if (podfunkce == USB_fce_3_05)                                           // "@D" = obnoveni defaultnich hodnot
          {
            delay(20);
            if (Serial.available())
              {
                char param = Serial.read();
                if (param == USB_fce_3_06)                                       // "@DS"      obnoveni defaultnich kalibracnich hodnot svetla
                  {
                    default_kalibrace_svetla();
                    Serial.println(lng029);                                      //  Serial.println("Nastaveny defaultni kalibracni hodnoty pro cidlo jasu");
                  }

                if (param == USB_fce_3_07)                                       // "@DT"      obnoveni defaultnich kalibracnich hodnot teploty
                  {
                    default_kalibrace_teploty();
                    Serial.println(lng030);                                      //  Serial.println("Nastaveny defaultni kalibracni hodnoty pro teplotu");
                  }

                if (param == USB_fce_3_08)                                       // "@DP"      obnoveni defaultnich programovych konstant
                  {
                    default_parametry();
                    Serial.println(lng031);                                      //  Serial.println("Nastaveny defaultni parametry");
                  }
                
                if (param == USB_fce_3_06 or param == USB_fce_3_07 or param == USB_fce_3_08)       // automaticky reset po prenastaveni kalibrace
                  {
                    podfunkce = USB_fce_3_10;                                         
                  } 

              }

          }

        if (podfunkce == USB_fce_3_10)                                           // "@@" = softwarovy RESET pres vyprseni WD timeru
          {
            SOFT_RESET();
          }

        if (podfunkce == USB_fce_3_27)                                           // "@m" = vypnuti modbusu
          {
            pouzit_modbus = false;
            EEPROM_write(eeaddr_modbus_LED,(menu_dis_led << 1));
            Serial.println(lng280);                                              // Serial.println("MODBUS vypnuty");                            
          }

        if (podfunkce == USB_fce_3_28)                                           // "@M" = zapnuti modbusu
          {
            pouzit_modbus = true;
            if (AUTOkom_linka == 1)                                              // kdyz je zapnute automaticke odesilani dat do linky RS485 ...
              {
                AUTOkom = AUTOkom & 0b00111111;                                  // ... automaticke odesilani se vypne
                EEPROM_write(eeaddr_AUTOkom, AUTOkom);
                Serial.println(lng382);                                          // Serial.println("Zapnutim modbusu bylo vypnuto automaticke odesilani zaznamu pres RS485");
              }
            

            EEPROM_write(eeaddr_modbus_LED,(menu_dis_led << 1) | pouzit_modbus);
            Serial.println(lng279);                                              // Serial.println("MODBUS zapnuty");
          }




        if (podfunkce == USB_fce_3_11)                                           // "@G" = vypis dat z GPS modulu
          {
            delay(20);
            if (Serial.available())
              {
                char podpodfunkce = Serial.read();
                if (podpodfunkce == USB_fce_3_19)                                // "@Gs nnn" = nastaveni domaci zemepisne sirky pro astro vypocty
                  {
                    float zaokrouhleni;
                    float pomprom = Serial.parseFloat();
                    if (pomprom <= 90 and pomprom >= -90)
                      {
                        if (pomprom < 0) zaokrouhleni = -0.05;
                        else             zaokrouhleni =  0.05;
                        int16_t zapis = ((pomprom + zaokrouhleni) * 10);

                        EEPROM_write_int(   eeaddr_GEO_lat   ,  zapis);   // zemepisna sirka pro astro vypocty
                        uint16_t obsah_eeprom = EEPROM_read_int(eeaddr_GEO_lat);

                        if (obsah_eeprom > 32767)
                          {
                           obsah_eeprom = 65536 - obsah_eeprom ; 
                           GeoLat = obsah_eeprom / -10.0;
                          }
                        else
                          {
                            GeoLat = obsah_eeprom / 10.0;
                          }
                        Serial.print(lng238);
                        Serial.println(GeoLat,1);
                      }
                    else
                      {
                        Serial.print(lng006);                                    // "Vstup mimo rozsah "
                        Serial.println("(-90.0 ... +90.0)");
                      }                      
                  }
                if (podpodfunkce == USB_fce_3_20)                                // "@Gd nnn" = nastaveni domaci zemepisne delky pro astro vypocty
                  {
                    float zaokrouhleni;
                    float pomprom = Serial.parseFloat();
                    if (pomprom <= 180 and pomprom >= -180)
                      {
                        if (pomprom < 0) zaokrouhleni = -0.05;
                        else             zaokrouhleni =  0.05;
                        int16_t zapis = ((pomprom + zaokrouhleni) * 10);

                        EEPROM_write_int(   eeaddr_GEO_lon   ,  zapis);   // zemepisna delka pro astro vypocty
                        uint16_t obsah_eeprom = EEPROM_read_int(eeaddr_GEO_lon);

                        if (obsah_eeprom > 32767)
                          {
                           obsah_eeprom = 65536 - obsah_eeprom ; 
                           GeoLon = obsah_eeprom / -10.0;
                          }
                        else
                          {
                            GeoLon = obsah_eeprom / 10.0;
                          }
                        Serial.print(lng239);
                        Serial.println(GeoLon,1);
                      }
                    else
                      {
                        Serial.print(lng006);                                    // "Vstup mimo rozsah "
                        Serial.println("(-180.0 ... +180.0)");
                      }                      
                  }

                if (podpodfunkce == USB_fce_3_21)                                // "@Gz nn" = nastaveni domaci casove zony pro "zimni" cas
                  {
                    long pomprom = Serial.parseInt();
                    if (pomprom <= 12 and pomprom >= -12)
                      {
                        EEPROM_write(   eeaddr_zima_hod   ,  pomprom + 100);     // prepocte se tak, aby se vzdycky ukladalo kladne cislo (pri cteni se 100 hned odecita)
                        zimni_posun = pomprom;
                        MODdata[42] = zimni_posun;                               // zimni posun casu
                        Serial.print(lng244);                                    // "Casovy posun zima/leto: "
                        Serial.print(zimni_posun);
                        Serial.print(" / ");
                        Serial.println(letni_posun);
                        STM_DS(false);                                           // serizeni casu v DS3231 podle casu v STM                        
                      }
                    else
                      {
                        Serial.print(lng006);                                    // "Vstup mimo rozsah "                        
                        Serial.println("(-12 ... +12)");
                      }                      
                  }

                if (podpodfunkce == USB_fce_3_22)                                // "@Gl nn" = nastaveni domaci casove zony pro letni cas
                  {
                    int16_t pomprom = Serial.parseInt();
                    if (pomprom <= 12 and pomprom >= -12)
                      {
                        EEPROM_write(   eeaddr_leto_hod   ,  pomprom + 100);     // prepocte se tak, aby se vzdycky ukladalo kladne cislo (pri cteni se 100 hned odecita)
                        letni_posun = pomprom;
                        MODdata[43] = letni_posun;                               // letni posun casu
                        Serial.print(lng244);                                    // "Casovy posun zima/leto: "
                        Serial.print(zimni_posun);
                        Serial.print(" / ");
                        Serial.println(letni_posun);
                        STM_DS(false);                                           // serizeni casu v DS3231 podle casu v STM                        
                      }
                    else
                      {
                        Serial.print(lng006);                                    // "Vstup mimo rozsah "
                        Serial.println("(-12 ... +12)");
                      }                      
                  }

              if (podpodfunkce == USB_fce_3_23)                                  // "@Gp SEC SELC" = nastaveni popisku pro zimni a letni casovou zonu
                {
                  delay(10);
                  uint8_t pozice = 0;                                               // ulozi prvni 4 znaky (popis zimni zony)
                  if (Serial.read() == ' ')
                    {
                      while (Serial.available() and pozice < 4)
                        {
                          char znak = Serial.read();
                          char_zima[pozice] = znak;
                          char_Xzima[pozice] = znak;
                          EEPROM_write(eeaddr_txt_zony + pozice, znak);
                          pozice ++;
                        }
                      char_Xzima[0] = '_';

                      uint8_t pozice = 0;                                           // ulozi zbyle 4 znaky (popis letni zony)
                      while (Serial.available() and pozice < 4)
                        {
                          char znak = Serial.read();
                          char_leto[pozice] = znak;
                          char_Xleto[pozice] = znak;
                          EEPROM_write(eeaddr_txt_zony + 4 + pozice, znak);
                          pozice ++;
                        }
                      char_Xleto[0] = '_';
                      Serial.println(char_zima);
                      Serial.println(char_leto);                  
                    }
                  else
                    {
                      Serial.print("Err (\"@Gp SEC SELC\")");
                    }
                }


              }


            else                                                                   // za prikazem "@G" uz nic nenasledovalo
              {
                if (digitalRead(pin_DATA_RDY) == osazeno_gps)                      // testuje se jen v pripade, je je GPS zasunuta (/zapnuta)
                  {
                    gps(0);                                                        // jen zpbrazeni stavu GPS bez jakychkoliv operaci s RTC
                    Serial.print("GPS LAT: ");
                    Serial.print(GPS_lat);
                    Serial.print(" = ");
                    uint32_t pom_prom_gps = GPS_lat;
                    
                    if (pom_prom_gps > 180000000UL)
                      {
                        Serial.println("Err");
                      }
                    else
                      {
                        if (pom_prom_gps > 90000000UL)
                          {
                            Serial.print("[N] ");
                            pom_prom_gps = pom_prom_gps - 90000000UL;
                          }
                        else
                          {
                            Serial.print("[S] ");
                          }
                        Serial.print(pom_prom_gps/1000000);                                // cela cast stupnu
                        pom_prom_gps = pom_prom_gps -(1000000 * (pom_prom_gps/1000000));   // vypocet desetinne casti
                        Serial.print('.');                                                 // desetinny oddelovac
                        if (pom_prom_gps < 100000)   Serial.print('0');                    // doplneni pripadnych nul za desetinnou teckou
                        if (pom_prom_gps <  10000)   Serial.print('0');
                        if (pom_prom_gps <   1000)   Serial.print('0');
                        if (pom_prom_gps <    100)   Serial.print('0');
                        if (pom_prom_gps <     10)   Serial.print('0');

                        Serial.println(pom_prom_gps);                                      // desetinna cast
                      }
        
        
                    
                    Serial.print("GPS LON: ");
                    Serial.print(GPS_lon);
                    Serial.print(" = ");
                    pom_prom_gps = GPS_lon;
                    if (pom_prom_gps > 360000000UL)
                      {
                        Serial.println("Err");
                      }
                    else
                      {
                        if (pom_prom_gps > 180000000UL)
                          {
                            Serial.print("[E] ");
                            pom_prom_gps = pom_prom_gps - 180000000UL;   
                          }
                        else
                          {
                            Serial.print("[W] ");
                          }
                        Serial.print(pom_prom_gps/1000000);                                // cela cast stupnu
                        pom_prom_gps = pom_prom_gps -(1000000 * (pom_prom_gps/1000000));   // vypocet desetinne casti
                        Serial.print('.');                                                 // desetinny oddelovac
                        if (pom_prom_gps < 100000)   Serial.print('0');                    // doplneni pripadnych nul za desetinnou teckou
                        if (pom_prom_gps <  10000)   Serial.print('0');
                        if (pom_prom_gps <   1000)   Serial.print('0');
                        if (pom_prom_gps <    100)   Serial.print('0');
                        if (pom_prom_gps <     10)   Serial.print('0');
                        Serial.println(pom_prom_gps);                                      // desetinna cast
                      }
        
                    
                    Serial.print("GPS ALT: ");
                    Serial.print(GPS_alt);
                    Serial.print(" = ");
                    if (GPS_alt > 10000)
                      {
                        Serial.println("Err");                
                      }
                    else
                      {
                        Serial.print(GPS_alt-500);
                        Serial.println(" [m]");
                      }
        
        
                    Serial.print("GPS satelites: ");
                    Serial.println(pole_GPS_I2C[17]);
                    
                    Serial.print("GPS HDoP: ");
                    if (pole_GPS_I2C[18] == 255 or GPS_alt > 10000)
                      {
                        Serial.println("99.99 (Err)");
                      }
                    else
                      {
                        Serial.println(pole_GPS_I2C[18] / 10.0);
                      }
        
                    Serial.print("GPS date time (UTC): ");
                    if (pole_GPS_I2C[11] < 10)  Serial.print('0');
                    Serial.print(pole_GPS_I2C[11]);
                    Serial.print('.');
                    if (pole_GPS_I2C[10] < 10)  Serial.print('0');
                    Serial.print(pole_GPS_I2C[10]);
                    Serial.print('.');
                    Serial.print(pole_GPS_I2C[9] + 2000);
                    Serial.print(' ');
                    if (pole_GPS_I2C[12] < 10)  Serial.print('0');
                    Serial.print(pole_GPS_I2C[12]);
                    Serial.print(':');
                    if (pole_GPS_I2C[13] < 10)  Serial.print('0');
                    Serial.print(pole_GPS_I2C[13]);
                    Serial.print(':');
                    if (pole_GPS_I2C[14] < 10)  Serial.print('0');
                    Serial.println(pole_GPS_I2C[14]);
        
        
                    Serial.print("RTC date time (UTC): ");
    
                    if (leto == true) casova_zona = letni_posun;
                    else              casova_zona = zimni_posun;
                    zobraz_RTC(false);
                    z_LOC_na_Astro_UTC(casova_zona);                                 // z mistnich casovych udaju (LOC_den; LOC_hod ;.....) vypocte UTC datum a cas
        
                    if (astro_UTC_den < 10)  Serial.print('0');
                    Serial.print(astro_UTC_den);
                    Serial.print('.');
                    if (astro_UTC_mes < 10)  Serial.print('0');
                    Serial.print(astro_UTC_mes);
                    Serial.print('.');
                    Serial.print(astro_UTC_rok);
                    Serial.print(' ');
                    if (astro_UTC_hod < 10)  Serial.print('0');
                    Serial.print(astro_UTC_hod);
                    Serial.print(':');
                    if (astro_UTC_min < 10)  Serial.print('0');
                    Serial.print(astro_UTC_min);
                    Serial.print(':');
                    if (LOC_sek < 10)  Serial.print('0');                
                    Serial.println(LOC_sek);
        
                    
        
                    Serial.print("GPS OK cnt: ");
                    Serial.println(pole_GPS_I2C[8]);
        
                    Serial.print("GPS status: ");
                    Serial.print(pole_GPS_I2C[19]);
                    Serial.print("  (data: ");
                    if ((pole_GPS_I2C[19] & 0b00000001) == 1) Serial.print("OK");
                    else                                      Serial.print("--");
                    Serial.print(" ; Time_SET: ");
                    if ((pole_GPS_I2C[19] & 0b00000010) == 2) Serial.print("ON");
                    else                                      Serial.print("OFF");
    
                    if ((pole_GPS_I2C[19] & 0b00000100) == 4) Serial.print(" ; Int.");
                    else                                      Serial.print(" ; Ext.");
    
                    
                    Serial.println(')');
        
                    Serial.print("ver_SW:");
                    gps_NMEA(2);

                    Serial.println("NMEA:");
                    Serial.print(' ');
                    gps_NMEA(0);
                    Serial.print(' ');
                    gps_NMEA(1);
                    

                  }
                else
                  {
                    Serial.println("GPS-OFF");
                  }
              }


          }

        if (podfunkce == USB_fce_3_12)                                           // "@S n" = nastaveni rychlosti komunikace
          {                                                                      //       (aaaa = SLAVE adresa)
                                                                                 // (0bxx00aaaa =   9600)
                                                                                 // (0bxx01aaaa =  19200)
                                                                                 // (0bxx10aaaa =  38400)
                                                                                 // (0bxx11aaaa = 115200)
            delay(20);
            uint32_t baudrate = Serial.parseInt(); 
            switch (baudrate)
              {
                case 9600:
                  baudrate = 0;
                  break;
                case 19200:
                  baudrate = 1;
                  break;
                case 38400:
                  baudrate = 2;
                  break;
                case 11520:
                  baudrate = 3;
                  break;                   
              }

            if (baudrate < 4)
              {
                uint8_t pomprom = EEPROM_read(eeaddr_SLAVE_baud) & 0b00001111;      // z EEPROM 14 se zachova pouze SLAVE adresa zarizeni
                baudrate = (baudrate << 4);                                      // zadane baudrate se posune na bity 4 a 5
                EEPROM_write(eeaddr_SLAVE_baud, (pomprom | baudrate));           // spoji se s puvodni adresou a zapise se zpatky do EEPROM
                Serial.print("Baudrate: ");
                Serial.println(pole_baudrate[(baudrate >> 4)]);
                SOFT_RESET();
              }
            else
              {
                Serial.print(lng006);                                            // "Vstup mimo rozsah "
                Serial.println("(0 ... 3)");
              }
          }


      if (podfunkce == USB_fce_3_63)                                             //  "@H" zapnuti zapisu do souboru "RTC_set.csv"
        {
          uint8_t hodnota = EEPROM_read(eeaddr_CRC_tset_lock);
          hodnota = hodnota | 0b01000000;
          RTC_SD_DEBUG = true;
          Serial.println(lng215);                                                // Zapis do 'RTC_set.csv' POVOLEN"
          EEPROM_write(eeaddr_CRC_tset_lock,hodnota);
        }
        
            
      if (podfunkce == USB_fce_3_16)                                             //  "@h" vypnuti zapisu do souboru "RTC_set.csv"
        {
          uint8_t hodnota = EEPROM_read(eeaddr_CRC_tset_lock);
          hodnota = hodnota & 0b10111111;
          RTC_SD_DEBUG = false;
          Serial.println(lng216);                                                // Zapis do 'RTC_set.csv' ZAKAZAN"
          EEPROM_write(eeaddr_CRC_tset_lock,hodnota);
        } 

     
      if (podfunkce == USB_fce_3_15)                                             //  "@>" vypis souboru z SD karty do seriove linky
        {
            char typ_souboru = ' ';
            uint32_t par2;
            uint32_t par3;
            char znak;
            uint32_t H_par2;                                                      // pro vypis trasovaciho souboru je umoznen i jednodussi format zapisu "@> H ####"
            uint32_t H_par3;                                                      // pro vypis trasovaciho souboru je umoznen i jednodussi format zapisu "@> H ####"

            uint32_t I_par2;                                                      // pro vypis kalibracnich souboru je umoznen i jednodussi format zapisu "@> I ####"
            uint32_t I_par3;                                                      // pro vypis kalibracnich souboru je umoznen i jednodussi format zapisu "@> I ####"

            delay(30);                                                           // chvili pauza na prijem vsech zadanych znaku ze seriove linky
            
            
            while (Serial.available() and typ_souboru == ' ')                    // mezi funkci a typem souboru jsou tolerovany mezery
              {
                typ_souboru =  Serial.read();                                    //      ... ktere se mazou
              }            

                                                                                 // promenna 'typ_souboru' by tady mela obsahovat znak 'A' z 'H'

            if (typ_souboru < 'A' or typ_souboru > 'I')                          // kdyz obsahuje cokoliv jineho (cislo, carku, pismeno mimo povoleny interval, mezeru)
              {                                                                  //    nove je povoleny i typ 'T' pro parametry dalekohledu
                if (typ_souboru != 'T')     typ_souboru = '?';                   //      ... nastavi se defaultni typ souboru '?' (vypis napovedy)
              }


            do                                                                   // ve zbytku serioveho bufferu hleda dalsi vyskyt nejakeho cisla
              {
                znak =  Serial.peek();
                if (znak < '0' or znak > '9') Serial.read();                    // neciselne znaky se preskakuji a mazou z bufferu
              }
            while (Serial.available() and (znak < '0' or znak > '9'));          // konec smycky DO-WHILE
            

            par2 = Serial.parseInt();                                           // pokus o rozeznani cisla od aktualni pozice v bufferu (parametr 2: zadany pocet radek)
            H_par2 = par2;                                                      // docasne se poznamena prvni parametr pro typ funkce 'H', ktera se da spoustet jednodussim prikazem 
            I_par2 = par2;                                                      // docasne se poznamena prvni parametr pro typ funkce 'I', ktera se da spoustet jednodussim prikazem 

            
            if (par2 == 0) par2 = 50;                                            // povolene minimum poctu radek je 1. Kdyz je zadana nula nebo nic, nastavi se defaultne 50 radek
                                                                                 // pro typy napoveda '?', vypis trasovaciho souboru 'H' a kalibrace 'I' se vypisuje vzdycky cely obsah souboru
                                                                                 //  je to ale osetreno az ve vypisove funkci "file_to_serkom()"
            
            do                                                                   // ve zbytku serioveho bufferu hleda dalsi vyskyt nejakeho cisla
              {
                znak =  Serial.peek();
                if (znak < '0' and znak > '9') Serial.read();                    // neciselne znaky se preskakuji a mazou z bufferu
              }
            while (Serial.available() and (znak < '0' and znak > '9'));          // konec smycky DO-WHILE


            if ((typ_souboru >= 'A' and typ_souboru <= 'C') or typ_souboru == 'H' or typ_souboru == 'I') // pro typy souboru 'A' az 'C', nebo 'H', nebo 'I' ...
              {
                par3 = Serial.parseInt();                                        //    ... se cte jeste treti parametr 
              }
            else                                                                 // pro ostatni typy souboru se treti parametr nastavi na 0
              {
                par3 = 0;
              }

            H_par3 = par3;                                                      // docasne se poznamena druhy parametr pro typ funkce 'H', ktera se da spoustet jednodussim prikazem 
            I_par3 = par3;                                                      // docasne se poznamena druhy parametr pro typ funkce 'I', ktera se da spoustet jednodussim prikazem 

            
            switch(typ_souboru)       
              {
                case 'A':                                                        // pro vypis "SYS_LOG.TXT"
                  if (par3 == 0)                                                 //     pokud parametr 3 chybi, nebo je zadany jako 0, nastavi se defaultne filtr na 'vsechno'                   
                    {
                      par3 = 1000;                        
                    }
                break;

                case 'B':                                                        // pro vypis zaznamu (napriklad "20240225.csv")
                  if (par3 == 0)                                                 //     pokud parametr 3 chybi, nebo je zadany jako 0, nastavi se datum posledniho zaznamu
                    {
                       par3 = EEPROM_read_long(eeaddr_posledni_den);             // v EEPROM je ulozeny datum posledniho zapsaneho souboru ve formatu "yyyymmdd"
                    }
                break;
                
                case 'C':                                                        // pro vypis "r_30s_##.csv"
                  if (par3 == 0 or par3 > 99)                                    //     pokud parametr 3 chybi, je nesmyslne vysoky, nebo je zadany jako 0, nastavi se defaultne posledni index
                    {
                      par3 = EEPROM_read30(6);                                   // posledni index je dostupny v EEPROM, kam se uklada pri kazdem exportu baliku dat na SD kartu
                    }
                break;

                case 'H':                                                       // pro vypis "trck####.gpx" je umozneno zadani indexu ve zjednodusenem formatu
                  if (H_par2 > 0 and H_par3 == 0)                               // "@> H 25"        je zadany jen druhy parametr
                    {
                      par3 = H_par2;                                            // v tom pripade druhy parametr oznacuje index a presune se na pozici 3. parametru
                    }
                  if (H_par2 == 0 and H_par3 == 0)                              // "@> H"    zadani bez parametru zobrazi posledni GPX soubor
                    {
                      par3 = EEPROM_read_int(eeaddr_GPS_track_id) & 0x3FFF;     // posledni index je dostupny v EEPROM (musi se ale smazat nejvyssi dva bity)
                    }
                  // ve vsech ostatnich pripadech se predpoklada format zadani: "@> H 123, 25" na druhem parametru (pocet radek) nezalezi treti parametr je vetsi nez 0
                break;

                case 'I':                                                        // pro vypis "CALB/nnn-mmdd.csv"
                  if (I_par2 > 0 and I_par3 == 0)                                // "@> I 25"        je zadany jen druhy parametr
                    {
                      par3 = I_par2;                                             // v tom pripade druhy parametr oznacuje index a presune se na pozici 3. parametru
                    }
                  if (I_par2 == 0 and I_par3 == 0)                               // "@> I"    zadani bez parametru zobrazi seznam souboru
                    {
                      par3 = 9999;
                    }
                  if (par3 > -50)                                                // pro pripad, ze by se jako index stari souboru zadalo zaporne cislo (-1 az - 49), tak se prevede na kladne 
                    {                                                            //    zadana hodnota -50 se v uint32_t uklada jako 4294967246
                      par3 = -par3;
                    }
                
                
                break;
                
                
                default:                                                         // pro ostatni typy souboru se pripadny dalsi parametr ignoruje a nastavi se na defaulni 999999
                  while (Serial.available()) Serial.read();
                  par3 = 999999;
                break;             
              }

            file_to_serkom(typ_souboru, par2, par3);         
        } 

      if (podfunkce == USB_fce_3_18)                                             //  "@*" servisni vypis jednoho zaznamu v hexa formatu
        {
          uint32_t hodnota = Serial.parseInt();
          Serial.print("adr. [");
          if (hodnota < 100000UL) Serial.print('0');
          if (hodnota <  10000)   Serial.print('0');
          if (hodnota <   1000)   Serial.print('0');
          if (hodnota <    100)   Serial.print('0');
          if (hodnota <     10)   Serial.print('0');         
          Serial.print(hodnota);
          Serial.print("]: ");

          for (uint32_t eeaddr = hodnota; eeaddr < (hodnota + delka_zaznamu) ; eeaddr ++)
            {
              uint8_t rd_hex = EEPROM_read(eeaddr);
              Serial.print("0x");
              if (rd_hex < 16)      Serial.print('0');
              Serial.print(rd_hex,HEX);
              if (eeaddr < (hodnota + delka_zaznamu - 1) )  Serial.print(", ");
            }
          Serial.println(' ');
        } 


      if (podfunkce == USB_fce_3_24)                                             //  "@Z" Zapis systemovych parametru na kartu
        {
          SD_sysMEM_WRITE();
        } 

      if (podfunkce == USB_fce_3_25)                                             //  "@O" Obnoveni systemovych parametru z karty
        {
          SD_sysMEM_READ();
        } 


      if (podfunkce == USB_fce_3_26)                                             // "@T" Funkce pro test HW
        {
          HW_test(false);
        } 

      if (podfunkce == USB_fce_3_36)                                             // "@L" = ovladani RGB LED
        {
          char param = Serial.read(); 
          if (param == USB_fce_3_37)  signal_LED = signal_LED & 0b11111110;      // @Lb - vypnout blikani pri testu baterie
          if (param == USB_fce_3_38)  signal_LED = signal_LED | 0b00000001;      // @LB - zapnout blikani pri testu baterie

          if (param == USB_fce_3_39)  signal_LED = signal_LED & 0b11111101;      // @Lm - vypnout blikani pri mereni
          if (param == USB_fce_3_40)  signal_LED = signal_LED | 0b00000010;      // @LM - zapnout blikani pri mereni

          if (param == USB_fce_3_41)  signal_LED = signal_LED & 0b11111011;      // @Le - vypnout blikani pri chybe
          if (param == USB_fce_3_42)  signal_LED = signal_LED | 0b00000100;      // @LE - zapnout blikani pri chybe

          if (param == USB_fce_3_43)  signal_LED = signal_LED & 0b11110111;      // @Lk - vypnout blikani pri komunikaci pres RS485
          if (param == USB_fce_3_44)  signal_LED = signal_LED | 0b00001000;      // @LK - zapnout blikani pri komunikaci pres RS485

          if (param == USB_fce_3_45)  signal_LED = signal_LED & 0b11101111;      // @La - vypnout blikani pri funkci AfD
          if (param == USB_fce_3_46)  signal_LED = signal_LED | 0b00010000;      // @LA - zapnout blikani pri funkci AfD

          if (param == USB_fce_3_67)  signal_LED = signal_LED & 0b11011111;      // @Lt - vypnout blikani pri funkci GPS trasovani
          if (param == USB_fce_3_68)  signal_LED = signal_LED | 0b00100000;      // @LT - zapnout blikani pri funkci GPS trasovani


          if (param == '?')
            {
              Serial.println(lng339);                                                      //  Serial.println("@L           ... vypnout / zapnout RGB LED");            
              Serial.println(lng282);                                                      //  Serial.println("     @Lb (B) ... vypnout (zapnout) LED pro test baterie");
              Serial.println(lng283);                                                      //  Serial.println("     @Lm (M) ... vypnout (zapnout) LED mereni");
              Serial.println(lng284);                                                      //  Serial.println("     @Le (E) ... vypnout (zapnout) LED pri chybe");
              Serial.println(lng285);                                                      //  Serial.println("     @Lk (K) ... vypnout (zapnout) LED pri komunikaci");
              Serial.println(lng301);                                                      //  Serial.println("     @La (A) ... vypnout (zapnout) LED pri funkci AfD");
              Serial.println(lng354);                                                      //  Serial.println("     @Lt (T) ... vypnout (zapnout) LED pri GPS trasovani"
            }
          else
            {
              RGB_LED_info();
              EEPROM_update(eeaddr_LED,signal_LED);          
            }

        }

      if (podfunkce == USB_fce_3_17)                                             // "@P" povolit/zakazat pipani
        {
          char param = Serial.read(); 
          if (param == USB_fce_3_47)  signal_BEEP = signal_BEEP & 0b1111111111111110;      // @Pb vypnout pipani pri zmene urovne napeti baterie
          if (param == USB_fce_3_48)  signal_BEEP = signal_BEEP | 0b0000000000000001;      // @PB zapnout pipani pri zmene urovne napeti baterie

          if (param == USB_fce_3_49)  signal_BEEP = signal_BEEP & 0b1111111111111101;      // @Pm vypnout pipani pri pri mereni
          if (param == USB_fce_3_50)  signal_BEEP = signal_BEEP | 0b0000000000000010;      // @PM zapnout pipani pri pri mereni

          if (param == USB_fce_3_51)  signal_BEEP = signal_BEEP & 0b1111111111111011;      // @Pa vypnout pipani pri alarmu (budiku) / odpoctu
          if (param == USB_fce_3_52)  signal_BEEP = signal_BEEP | 0b0000000000000100;      // @PA zapnout pipani pri alarmu (budiku) / odpoctu
        
          if (param == USB_fce_3_53)  signal_BEEP = signal_BEEP & 0b1111111111110111;      // @Pa vypnout pipani pri HW chybe
          if (param == USB_fce_3_54)  signal_BEEP = signal_BEEP | 0b0000000000001000;      // @PA zapnout pipani pri HW chybe

          if (param == USB_fce_3_55)  signal_BEEP = signal_BEEP & 0b1111111111101111;      // @Pt vypnout pipani pri operacich s bocnim tlacitkem
          if (param == USB_fce_3_56)  signal_BEEP = signal_BEEP | 0b0000000000010000;      // @PT zapnout pipani pri operacich s bocnim tlacitkem

          if (param == USB_fce_3_57)  signal_BEEP = signal_BEEP & 0b1111111111011111;      // @Po vypnout pipani pri mireni na objekt (RA-DEC)
          if (param == USB_fce_3_58)  signal_BEEP = signal_BEEP | 0b0000000000100000;      // @PO zapnout pipani pri mireni na objekt (RA-DEC)

          if (param == USB_fce_3_59)  signal_BEEP = signal_BEEP & 0b1111111110111111;      // @Pv vypnout pipani pri vystrahach
          if (param == USB_fce_3_60)  signal_BEEP = signal_BEEP | 0b0000000001000000;      // @PV zapnout pipani pri vystrahach
        
          if (param == USB_fce_3_61)  signal_BEEP = signal_BEEP & 0b1111111101111111;      // @Ps vypnout pipani pri stopkach
          if (param == USB_fce_3_62)  signal_BEEP = signal_BEEP | 0b0000000010000000;      // @PS zapnout pipani pri stopkach

          if (param == USB_fce_3_65)  signal_BEEP = signal_BEEP & 0b1111111011111111;      // @Pz vypnout pipnuti pri zapnuti napajeni
          if (param == USB_fce_3_66)  signal_BEEP = signal_BEEP | 0b0000000100000000;      // @PZ zapnout pipnuti pri zapnuti napajeni

          if (param == '?')
            {
              Serial.println(lng222);                                                      //  Serial.println("@P           ... povolit/zakazat pipani");            
              Serial.println(lng322);                                                      //  Serial.println("     @Pb (B) ... zakazat (povolit) pipani pro signalizaci baterie");
              Serial.println(lng323);                                                      //  Serial.println("     @Pm (M) ... zakazat (povolit) pipani pro mereni");
              Serial.println(lng324);                                                      //  Serial.println("     @Pa (A) ... zakazat (povolit) pipani pro budiky a odpocet");
              Serial.println(lng325);                                                      //  Serial.println("     @Pe (E) ... zakazat (povolit) pipani pro HW chyby");
              Serial.println(lng326);                                                      //  Serial.println("     @Pt (T) ... zakazat (povolit) pipani pro operace s TS tlacitkem");
              Serial.println(lng327);                                                      //  Serial.println("     @Po (O) ... zakazat (povolit) pipani pro priblizovani k objektu (Ra/Dec)");
              Serial.println(lng328);                                                      //  Serial.println("     @Pv (V) ... zakazat (povolit) pipani pro dalsi vystrahy");
              Serial.println(lng329);                                                      //  Serial.println("     @Ps (S) ... zakazat (povolit) pipani pro stopky");              
              Serial.println(lng352);                                                      //  Serial.println("     @Pz (Z) ... zakazat (povolit) pipnuti pri zapnuti");              

            }
          else
            {
              BEEP_info();
              EEPROM_update(eeaddr_beep_bity     , signal_BEEP / 256);
              EEPROM_update(eeaddr_beep_bity + 1 , signal_BEEP % 256);          
            }

        }


      if (podfunkce == USB_fce_3_33)                                             // "@V" = nastaveni verze SQM (rucni s displejem / trvale nainstalovana s LED)
        {
          char param = Serial.read(); 
          if (param == USB_fce_3_34)                                             // @Vd - verze s displejem
            {
              menu_dis_led = true;
              EEPROM_write(eeaddr_modbus_LED,(menu_dis_led << 1) | pouzit_modbus); 
              Serial.println(lng296);                                            // Serial.println("Displejova verze SQM");  
            }
          if (param == USB_fce_3_35)                                             // @Vl - verze s LED
            {
              menu_dis_led = false;
              EEPROM_write(eeaddr_modbus_LED, pouzit_modbus);
              Serial.println(lng297);                                            // Serial.println("LED verze SQM");  
            }

        }

      if (podfunkce == USB_fce_3_64)                                             // "@N" = nastaveni elevace Slunce pro uzivatelsky definovany soumrak
        {
          int16_t hodnota = Serial.parseInt();
          hodnota = abs(hodnota);
          if (hodnota > 90)
            {
              Serial.print(lng006);                                              //  Serial.print("Vstup mimo rozsah ");
              Serial.println("(0 - 90)");
              hodnota = EEPROM_read(eeaddr_elevace_soumrak);                     // pri spatne zadane hodnote se pouzije puvodni hodnota
            }
          else
            {
              EEPROM_update(eeaddr_elevace_soumrak , hodnota);
              elevace_soumrak = hodnota;   
            }
                                                                                 //  V kazdem pripade se hodnota zobrazi
          Serial.print(lng349);                                                  //  Serial.print("Uzivatelsky definovany soumrak je pocitan pri elevaci Slunce -");
          Serial.print(hodnota);
          Serial.println(lng350);                                                //  Serial.print(" stupnu.");
        }

      if (podfunkce == USB_fce_3_69)                                             // "@A" = nastaveni automatickeho odesilani dat do seriove linky
        {
          uint8_t slozeno = 0;
          uint8_t hodnota = Serial.parseInt();                                      //  zadani linky
          if (hodnota > 2)
            {
              Serial.print(lng006);                                              // "Vstup mimo rozsah "
              Serial.println(" (l = [0 ... 2])");
            }
          else
            {
              if (hodnota == 2) slozeno = 128;                                   // USB
              if (hodnota == 1) slozeno =  64;                                   // RS485
              hodnota = Serial.parseInt();                                       // zadani formatu
              if (hodnota > 2)
                {
                  Serial.print(lng006);                                          // "Vstup mimo rozsah "
                  Serial.println(" (f = [0 ... 2])");
                }
              else
                {
                  if (hodnota == 2)                                              // specialni funkce pro vypis hlavicky (parametry se nikam neukladaji)
                    {
                      AUTO_hlavicka();
                    }
                  else
                    {
                      if (hodnota == 1) slozeno =  slozeno + 16;
                      hodnota = Serial.parseInt();                               // zadani vypisovanych objektu
                      if (hodnota > 15)
                        {
                          Serial.print(lng006);                                  // "Vstup mimo rozsah "
                          Serial.println(" (o = [0 ... 15])");
                        }
                      else
                        {
                          slozeno =  slozeno + hodnota;
                          EEPROM_write(eeaddr_AUTOkom, slozeno);
                          
                          AUTOkom_linka    = (slozeno >> 6);                     // (bity 7 a 6) linka 1 (RS485), nebo 2 (USB) , nebo 0 (vypnuto)
                                                                                 // (bit 5 nevyuzity)
                          AUTOkom_format   = ((slozeno & 0b00010000) >> 4);      // (bit 4) format odesilanych dat (0= pole dat; 1= text)
                          AUTOkom_obsah    = (slozeno & 0b00001111);             // (bity 3 az 0) obsah odesilanych dat   
                          AUTOdata_tabulka();
                          if (AUTOkom_linka == 1 and pouzit_modbus == true)
                            {
                              Serial.println(lng366);                            // "Automatickym odesilanim zaznamu pres RS485 byl vypnut modbus"
                              pouzit_modbus = false;                             // automaticke odesilani zaznamu do linky RS485 vypina MODBUS
                            }
                                          
                          if (AUTOkom_posledni == 0)                             // kdyz jeste nejsou zadna data (pocitano od zapnuti napajeni) dojde k upozorneni
                            {
                              Serial.print('\n');
                              Serial.println(lng381);                            // "Nejsou data k odeslani"
                            }             
                        }
                    }
                }
            }
        }
      }
      
    while (Serial.available())  Serial.read();                                   // pripadny zbytek dat v prijimacim bufferu se smaze  
  }
//----------------------------------------------



//----------------------------------------------
// specialni funkce pro pridavny HW
void spec_funkce(void)                                                           //!! (r30s)
  {
    if (pouzit_r30s == true)                                                     // kdyby byla periferie zakazana, nesla by ani zformatovat
      {
        delay(50);
        if (spec_rezim == 100)                                                      
          {
            ser_kom30();    
          }
        else                                                                     // format je mozne provest i v pripade, ze neni na nulte adrese hodnota 100
          {
            if (Serial.available())                                                      
              {
                char funkce = Serial.peek();                                     // %F (zakladni nastaveni a FORMAT specialni pridavne EEPROM)
                if (funkce ==  USB_fce_4_11)                                     //   'F'                             
                  { 
                    ser_kom30();    
                  }
                else
                  {
                    Serial.println("Err - Extra EEPROM");
                  }
              }
          }
        while (Serial.available())  Serial.read();                               // pripadny zbytek dat v prijimacim bufferu se smaze  
      }
  }

//----------------------------------------------
// seznam prikazu pro funkce bez znacky typu funkci  
void ser_napoveda0(void)
  {
    Serial.println(lng036);                                                      //  Serial.println("l        ... zmer plosny jas  - bez ukladani");
    if (senzor_BME == true)
      {
        Serial.println(lng037);                                                  //  Serial.println("v        ... zmer vlhkost     - bez ukladani");
        Serial.println(lng038);                                                  //  Serial.println("t        ... zmer teplotu     - bez ukladani");
        Serial.println(lng039);                                                  //  Serial.println("p        ... zmer tlak        - bez ukladani");
      }

    if (modul_LSM303DLHC == true)
      {
        Serial.println(lng226);                                                  //  Serial.println("n        ... zobrazit aktualni naklon");
        Serial.println(lng268);                                                  //  Serial.println("s        ... smer natoceni (azimut)");
      }

    Serial.println(lng245);                                                      //  Serial.println("x        ... zmer osvetleni (lux)  - bez ukladani");


    Serial.println(lng040);                                                      //  Serial.println("z 0      ... prepnout na SEC");
    Serial.println(lng041);                                                      //  Serial.println("z 1      ... prepnout na SELC");
  
    Serial.println(lng042);                                                      //  Serial.println("a nnn    ... nastaveni automatickeho spousteni 0-255 minut");

    if (pocet_cidel_svetla > 1)
      {
        Serial.println(lng043);                                                  //  Serial.println("e n      ... pri pouziti expanzni desky prepne na zvolene cidlo (1-7)");
      }
    Serial.println(lng044);                                                      //  Serial.println("r        ... vypis poslednich 100 zaznamu z EEPROM");
    Serial.println(lng045);                                                      //  Serial.println("rp       ... vypis posledniho zaznamu z EEPROM");
    Serial.println(lng046);                                                      //  Serial.println("ra       ... vypis vsech zaznamu z EEPROM");
    Serial.println(lng047);                                                      //  Serial.println("rz nnnnn ... vypis jednoho zvoleneho zazamu");
    Serial.println(lng225);                                                      //  Serial.println("rh nnnnn ... vypsat zaznamy z poslednich 'nnnnn' hodin");
    Serial.println(lng309);                                                      //  Serial.println("rt nnnnn ... vypis jednoho zvoleneho zazamu v tabulkovem formatu");

    Serial.println(lng048);                                                      //  Serial.println("c        ... aktualni cas");
    Serial.println(lng049);                                                      //  Serial.println("i        ... informace o aktualnim nastaveni systemu");
    if (pouzit_ds3231 == true)
      {
        Serial.println(lng310);                                                  //  Serial.println("b n hh:mm ... nastaveni jednoho z 5 budiku");
        Serial.println(lng319);                                                  //  Serial.println("o -nnn ... nastaveni odpoctoveho alarmu [nnn = minuty]");        
      }
    Serial.println(lng050);                                                      //  Serial.println("h        ... zobrazeni hlavicky pro formatovany vystup");
    Serial.println(lng051);                                                      //  Serial.println("m        ... spusteni vsech mereni se zapisem do EEPROM");

    Serial.println(lng052);                                                      //  Serial.println("ks       ... kalibracni mereni jasu (vystup bez korekce)");
    Serial.println(lng053);                                                      //  Serial.println("kt       ... kalibracni mereni teploty (vystup bez korekce)");

    Serial.println(lng054);                                                      //  Serial.println("#?       ... napoveda pro servisni funkce");
    
  }




//----------------------------------------------
// napoveda pro uzivatelske funkce (casto pouzivane)
//  pokud zacina znakem '?' a pokracuje nejakym kodem funkce, prevede se na jmeno help souboru na SD karte ("HELP/1uu.txt")
void ser_napoveda1(void)
  {
    helpfile_name[6] = '_';                                                      // druhy znak ve jmene souboru obvykle urcuje, jestli je kod funkce velke nebo male pismeno, pripadne nejaky znak
    char druhy_znak = Serial.peek();
    if (druhy_znak > 32 and druhy_znak < 127)                                    // je pozadovana nejaka detailnejsi napoveda pro prislusnou funkci (priklad "?#I")
      {                                                                          //       netisknutelne znaky (CR, LF, mezera ....) se tyto znaky ignoruji
        Serial.read();                                                           // druhy znak za otaznikem uz je peeknuty v promenne 'druhy_znak', tady se jen odebere z bufferu

        if (druhy_znak == '#' or druhy_znak == '@' or druhy_znak == '%' or druhy_znak == '*')         // napoveda pro specialni funkce
          {
            char treti_znak = Serial.read();                                     // pri specialnich funkcich se zadava i treti znak
            switch (druhy_znak)                                                  // podle druheho znaku (typ funkce) se nastavi prvni znak ve jmene souboru ("?@..." se prevede na "HELP/2...")
              {
                case '#':
                  helpfile_name[5] = '1';
                  break;
                case '@':
                  helpfile_name[5] = '2';
                  break;
                case '%':
                  helpfile_name[5] = '3';
                  break;
                case '*':
                  helpfile_name[5] = '4';
                  break;

              }
            helpfile_name[7] = treti_znak;
          }
        else                                                                     // napoveda pro hlavni funkce
          {           
            helpfile_name[5] = '0';                                              // kdyz neni typ funkce zadany, nastavi se prvni znak souboru na '0'  ("?I" se prevede na "HELP/0_I")
            helpfile_name[7] = druhy_znak;
          }


// #doc#17
        // osetreni pripadnych zakazanych znaku ('*', '/', '>', '<', ':', '?', '"', '\' ) ve jmene souboru 
        //           (musi se prepsat rucne, ale tak aby nedoslo ke kolizi s jinou funkci)
        switch (helpfile_name[7])
          {
            case '>':                       // puvodni jmeno souboru "2l>.txt" pro funkci "@>" se nahradi za "2_[.txt"
              helpfile_name[7] = '[';       //       znak '[' ale nesmi byt pouzit pro jinou funkci
              break;       

            case '?':                       // specialni pripad dvou otazniku za sebou "??" se prevede na soubor "mmm.txt"
              helpfile_name[5] = 'm';
              helpfile_name[6] = 'm';
              helpfile_name[7] = 'm';
              break;       

          
          }

        if (helpfile_name[6] != 'm')                                  // ve specialnim pripade pro zobrazeni menu ze souboru (prikaz "??") se druhy znak ve jmene souboru neupravuje
          {
            if (helpfile_name[7] > 90)  helpfile_name[6] = 'l';       // 7. znak ve jmene souboru je  male pismeno (ASCII kod je vetsi nez 90) 'LowerCase'
            else                        helpfile_name[6] = 'u';       // 7. znak ve jmene souboru je velke pismeno (ASCII kod je mensi nez 91) 'UpperCase'
    
            if (helpfile_name[7] >= 65 and helpfile_name[7] <= 90)    // ovladaci znak funkce je velke pismeno
              {
                helpfile_name[6] = 'u';                               // do jmena souboru se doplni znacka UpperCase
                helpfile_name[7] = helpfile_name[7] + 32;             // zmena z velkeho pismena na male (posun ASCII kodu mezi 'A' a 'a' je 32 znaku)
              }
            else                                                      // ovladaci znak funkce NENI velke pismeno
              {
                if (helpfile_name[7] >= 97 and helpfile_name[7] <= 122)   // ovladaci znak funkce je male pismeno
                  {
                    helpfile_name[6] = 'l';                           // do jmena souboru se doplni znacka LowerCase
                  }
                else                                                  // ovladaci znak funkce NENI male ani velke pismeno
                  {
                    helpfile_name[6] = '_';                           // do jmena souboru se doplni znacka "NEpismeno"
                  }
              }
          }
        
        helpfile();
      }
    else                                                                         // pokud za znakem '?' uz neni zadny dalsi tisknutelny znak, znamena to jen vypis funkci
      {
        ser_napoveda0();                                                         // seznam prikazu pro funkce bez znacky typu funkci
      }
  }
//----------------------------------------------



//----------------------------------------------
// napoveda pro servisni funkce
void ser_napoveda2(void)
  {
    Serial.println(lng055);                                                      //  Serial.println("#Tyyyymmddhhnnss ... nastaveni casu");

    Serial.println(lng277);                                                      //  Serial.println("#TG              ... nastaveni casu z GPS");

    Serial.println(lng056);                                                      //  Serial.println("#P nn        ... nastaveni poctu prumerovani jasu (1-20)");
    Serial.println(lng057);                                                      //  Serial.println("#S nnn       ... nastaveni urovne stability (0-255)");
    Serial.println(lng058);                                                      //  Serial.println("#I           ... detailni informace o kalibraci vnitrnich hodin");  
  
    Serial.println(lng059);                                                      //  Serial.println("#A nn        ... nastaveni SLAVE adresy pro komunikaci (1-15)");
    Serial.println(lng060);                                                      //  Serial.println("#C 0         ... vypnuti CRC kontroly pro RS485");
    Serial.println(lng061);                                                      //  Serial.println("#C 1         ... zapnuti CRC kontroly pro RS485");

    Serial.println(lng062);                                                      //  Serial.println("#O?          ... nastaveni oddelovacu");

    Serial.println(lng063);                                                      //  Serial.println("#FS          ... SOFT format EEPROM");
    Serial.println(lng064);                                                      //  Serial.println("#FH          ... HARD format EEPROM");

    Serial.println(lng065);                                                      //  Serial.println("#U           ... hranice jasu pro ukladani - v desetinach mag/arcsec2 (0-255)");

    Serial.println(lng302);                                                      //  Serial.println("#D ff.f      ... nastaveni hranice AfD (13.0 - 25.4) ostatni = vypnuto");
    

    Serial.println(lng066);                                                      //  Serial.println("#KS n;x;y*   ... zapis bodu do kalibracni tabulky jasu");
    if (senzor_BME == true)
      {
        Serial.println(lng067);                                                  //  Serial.println("#KT n;x;y*   ... zapis bodu do kalibracni tabulky teploty");
      }

    if (modul_LSM303DLHC == true)
      {
        Serial.println(lng227);                                                  //  Serial.println("#Nk          ... kalibrace naklonomeru");
        Serial.println(lng230);                                                  //  Serial.println("#Ni          ... Info o naklonomeru");
        Serial.println(lng266);                                                  //  Serial.println("#Mk          ... kalibrace magnetometru (kompasu)");
        Serial.println(lng316);                                                  //  Serial.println("#Mo nn.n     ... nastaveni offsetu kompasu (-12.7 ... +12.7 stupne)");
        Serial.println(lng341);                                                  //  Serial.println("#Mi          ... Info o magnetometru");
      }

    Serial.println(lng212);                                                      //  Serial.println("#E              ... vypis pozorovacich stanovist");
    Serial.println(lng213);                                                      //  Serial.println("#ETn text       ... editace textu u pozorovacich stanovist");
    Serial.println(lng214);                                                      //  Serial.println("#EGn g,g,g,g,g  ... editace grafiky u pozorovacich stanovist");

    Serial.println(lng248);                                                      // "#Xa nn       ... kalibrace luxmetru - parametr 'a'"
    Serial.println(lng249);                                                      // "#Xb nn       ... kalibrace luxmetru - parametr 'b'"
    Serial.println(lng263);                                                      // "#Xc nn       ... kalibrace luxmetru - parametr 'c'"

  }
//----------------------------------------------



//----------------------------------------------
// napoveda pro tajne funkce
void ser_napoveda3(void)
  {
    Serial.println(lng068);                                                      //  Serial.println("@W addr,data ... primy zapis do EEPROM");
    Serial.println(lng069);                                                      //  Serial.println("@R addr      ... prime cteni z EEPROM");

    Serial.println(lng070);                                                      //  Serial.println("@E           ... test postupneho prepinani cidel na expanzni desce");
    Serial.println(lng071);                                                      //  Serial.println("@X n         ... nastaveni poctu vstupu na expanzni desce (1-7)");
    Serial.println(lng072);                                                      //  Serial.println("@DS          ... defaultni kalibracni hodnoty pro jas");
    Serial.println(lng073);                                                      //  Serial.println("@DT          ... defaultni kalibracni hodnoty pro teplotu");
    Serial.println(lng074);                                                      //  Serial.println("@DP          ... defaultni parametry systemu");  
    Serial.println(lng281);                                                      //  Serial.println("@M   (m)     ... zapnout (vypnout) modbus");

    Serial.println(lng076);                                                      //  Serial.println("@@           ... softwarovy RESET");  
    Serial.println(lng208);                                                      //  Serial.println("@G           ... vypis dat z GPS modulu");  

    Serial.println(lng240);                                                      //  Serial.println("@Gs nnn      ... domaci zemepisna sirka pro astro vypocty");  
    Serial.println(lng241);                                                      //  Serial.println("@Gd nnn      ... domaci zemepisna delka pro astro vypocty");  
    Serial.println(lng242);                                                      //  Serial.println("@Gz nn       ... domaci zimni casova zona");
    Serial.println(lng243);                                                      //  Serial.println("@Gl nn       ... domaci letni casova zona");
    Serial.println(lng246);                                                      //  Serial.println("@Gp SEC SELC ... textovy popis pro zimni a letni casovou zonu");

    Serial.println(lng209);                                                      //  Serial.println("@S n         ... rychlost seriove komunikace (0=9600; 1=19200; 2=38400; 3=115200)");  
    Serial.println(lng222);                                                      //  Serial.println("@P           ... povolit/zakazat pipani  
    Serial.println(lng322);                                                      //  Serial.println("     @Pb (B) ... zakazat (povolit) pipani pro signalizaci baterie");
    Serial.println(lng323);                                                      //  Serial.println("     @Pm (M) ... zakazat (povolit) pipani pro mereni");
    Serial.println(lng324);                                                      //  Serial.println("     @Pa (A) ... zakazat (povolit) pipani pro budiky a odpocet");
    Serial.println(lng325);                                                      //  Serial.println("     @Pe (E) ... zakazat (povolit) pipani pro HW chyby");
    Serial.println(lng326);                                                      //  Serial.println("     @Pt (T) ... zakazat (povolit) pipani pro operace s TS tlacitkem");
    Serial.println(lng327);                                                      //  Serial.println("     @Po (O) ... zakazat (povolit) pipani pro priblizovani k objektu (Ra/Dec)");
    Serial.println(lng328);                                                      //  Serial.println("     @Pv (V) ... zakazat (povolit) pipani pro dalsi vystrahy");
    Serial.println(lng329);                                                      //  Serial.println("     @Ps (S) ... zakazat (povolit) pipani pro stopky");
    Serial.println(lng352);                                                      //  Serial.println("     @Pz (Z) ... zakazat (povolit) pipnuti pri zapnuti");

    Serial.println(lng339);                                                      //  Serial,println("@L           ... povolit/zakazat RGB LED");
    Serial.println(lng282);                                                      //  Serial.println("     @Lb (B) ... vypnout (zapnout) LED pro test baterie"
    Serial.println(lng283);                                                      //  Serial.println("     @Lm (M) ... vypnout (zapnout) LED mereni"
    Serial.println(lng284);                                                      //  Serial.println("     @Le (E) ... vypnout (zapnout) LED pri chybe"
    Serial.println(lng285);                                                      //  Serial.println("     @Lk (K) ... vypnout (zapnout) LED pri komunikaci"
    Serial.println(lng301);                                                      //  Serial.println("     @La (A) ... vypnout (zapnout) LED pri funkci AfD"
    Serial.println(lng354);                                                      //  Serial.println("     @Lt (T) ... vypnout (zapnout) LED pri GPS trasovani"
    Serial.println(lng217);                                                      //  Serial.println("@> t,n,par   ... vypis souboru z SD karty do seriove linky");  
    Serial.println(lng218);                                                      //  Serial.println("@h   (H)     ... zakazat (povolit) zapis do souboru RTC_set.csv");  

    Serial.println(lng348);                                                      //  Serial.println("@N nn        ... nastavit elevaci Slunce pro signalizaci noci");  

    Serial.println(lng298);                                                      //  Serial.println("@Vd          ... displejova verze SQM");
    Serial.println(lng299);                                                      //  Serial.println("@Vl          ... LED verze SQM");

    Serial.println(lng270);                                                      //  Serial.println("@Z           ... Zapsat parametry na SD");  
    Serial.println(lng271);                                                      //  Serial.println("@O           ... Obnovit parametry z SD");  
    Serial.println(lng272);                                                      //  Serial.println("@T           ... HW test"

    Serial.println(lng362);                                                      //  Serial.println("@A nnn       ... automaticke odesilani zaznamu do seriove linky (bez vyzadani)"

  }
//----------------------------------------------
  


//----------------------------------------------
// seriova napoveda pro PC funkce 
void ser_napoveda5(void)
  {
    Serial.println(lng343);                                // Serial.println ("*m           ... spustit mereni bez vystupu do seriove linky");
    Serial.println(lng344);                                // Serial.println ("*n           ... zadost o index posledniho zaznamu");
    Serial.println(lng345);                                // Serial.println ("*a           ... test na shodu SLAVE adresy");
    Serial.println(lng346);                                // Serial.println ("*f           ... vypis kodu vsech funkci");
    Serial.println(lng347);                                // Serial.println ("*r nnn       ... stazeni jednoho vybraneho zaznamu v datovem formatu (alternativa k prikazu 'rd')");
    Serial.println(lng351);                                // Serial.println ("*s p c d.m.r ... simulace planety slunecni soustavy");
    Serial.println(lng385);                                // Serial.println ("*e aaa       ... cteni 1 bajtu z EEPROM");
    

  }
//----------------------------------------------


//----------------------------------------------
void nastaveni_oddelovacu(void)
  {
    delay(20);
    char funkce = Serial.read();
    bool chyba = true;
    
    if (funkce == USB_fce_2_08)                                                  // #Oo
      {
       Serial.print(lng077);                                                     //  Serial.print("Oddelovac polozek: ");
        funkce = Serial.read();
        if(funkce == '_')                                                        // "#Oo_"
          {
            oddelovac_polozek = 0;
            Serial.println(lng078);                                              //  Serial.println("mezera");            
            chyba = false;
          }
        if(funkce == ',')                                                        // "#Oo,"
          {
            oddelovac_polozek = 1;
            Serial.println(lng079);                                              // Serial.println("carka");            
            chyba = false;
          }
        if(funkce == ';')                                                        // "#Oo;"
          {
            oddelovac_polozek = 2;
            Serial.println(lng080);                                              //  Serial.println("strednik");            
            chyba = false;
          }
        if(funkce == 't')                                                        // "#Oot"
          {
            oddelovac_polozek = 3;
            Serial.println(lng081);                                              //  Serial.println("tabulator");            
            chyba = false;
          }
        funkce = 'X';                                                            // to jen aby se nevyhodnotila dalsi podminka
      }
  
    
    
    if (funkce == USB_fce_2_09)                                                  //  "#O,"
      {
        uint8_t pomprom = Serial.parseInt();
        Serial.print(lng082);                                                    //  Serial.print("Oddelovac desetinnych mist: ");
        if(pomprom == 1)                                                            // "#O,1"
          {
            des_carka = true;
            Serial.println(lng079);                                              //  Serial.println("carka");            
          }
        else                                                                     // "#O,0"
          {
            des_carka = false;
            Serial.println(lng083);                                              //  Serial.println("tecka");
          }
        chyba = false;
        funkce = 'X';                                                            // to jen aby se nevyhodnotila dalsi podminka
      }

    if (funkce == USB_fce_2_10)                                                  //  "#O""
      {
        uint8_t pomprom = Serial.parseInt();
        if(pomprom == 1)                                                            // "#O"1"
          {
            uzavirat_uvozovky = true;
            Serial.print(lng084);                                                //  Serial.print("Uzavirat");
          }
        else                                                                     // "#O"0"
          {
            uzavirat_uvozovky = false;
            Serial.print(lng085);                                                //  Serial.print("Neuzavirat");
          }
        Serial.println(lng086);                                                  //  Serial.println(" hodnoty do uvozovek");
        chyba = false;
        funkce = 'X';                                                            // to jen aby se nevyhodnotila dalsi podminka
      }

    if (funkce == USB_fce_2_11)                                                  // "#Oh"
      {
        uint8_t pomprom = Serial.parseInt();
        Serial.print(lng087);                                                    //  Serial.print("Na zacatku souboru se hlavicka vytvaret ");
        if(pomprom == 1)                                                            // "#Oh1"
          {
            vytvaret_hlavicku = true;
            Serial.println(lng088);                                              //  Serial.println("bude.");
          }
        else                                                                     // "#Oh0"
          {
            vytvaret_hlavicku = false;
            Serial.println(lng089);                                              //  Serial.println("nebude.");
          }
        chyba = false;
      }

    if (chyba == true)
      {
        Serial.println(lng090);                                                  //  Serial.println("#O,1 ... des. oddelovac: carka");
        Serial.println(lng091);                                                  //  Serial.println("#O,0 ... des. oddelovac: tecka");
        Serial.println(lng092);                                                  //  Serial.println("#O\"1 ... uzavirat polozky do uvozovek");
        Serial.println(lng093);                                                  //  Serial.println("#O\"0 ... neuzavirat polozky do uvozovek");
        Serial.println(lng094);                                                  //  Serial.println("#Oh1 ... vytvaret hlavicky");
        Serial.println(lng095);                                                  //  Serial.println("#Oh0 ... nevytvaret hlavicky");
        Serial.println(lng096);                                                  //  Serial.println("#Oo_ ... oddelovac polozek: mezera");
        Serial.println(lng097);                                                  //  Serial.println("#Oo, ... oddelovac polozek: carka");
        Serial.println(lng098);                                                  //  Serial.println("#Oo; ... oddelovac polozek: strednik");
        Serial.println(lng099);                                                  //  Serial.println("#Oot ... oddelovac polozek: tabulator");        
      }
    else
      {
        bitWrite(oddelovaci_bajt , 2 , des_carka);
        bitWrite(oddelovaci_bajt , 3 , uzavirat_uvozovky);
        bitWrite(oddelovaci_bajt , 4 , vytvaret_hlavicku);
        oddelovaci_bajt = oddelovaci_bajt & 0b11111100;
        oddelovaci_bajt = oddelovaci_bajt | oddelovac_polozek ;
        
        if (des_carka == true and oddelovac_polozek == 1)  Serial.println(lng100);    //  Serial.println("Varovani: oddelovac polozek i desetinnych mist je carka.");
        
        EEPROM_update(eeaddr_oddelovace , oddelovaci_bajt);        

        if (uzavirat_uvozovky == true)  uvozovky = '"';
        else                            uvozovky = ' ';         

        if (des_carka == true)  desod = ',';
        else                    desod = '.';

        oddeleni_polozek();                                                      // aktualizace hlavicky (a vystupniho retezce) s novymi oddelovaci polozek
      
      }    
  }
//----------------------------------------------



//----------------------------------------------
void vstup_kalibrace_svetla(void)
  {
    delay(50);                                                                   // v jednom bloku kalibracnich dat budou maximalne 3 radky tabulky (to je maximalne 3*15 = 45 bajtu, takze se to cele vejde do bufferu )

    for (uint8_t i = 0 ; i < 15 ; i++)                                              // vsech 15 pomocnych promennych pro kalibracni tabulku se nastavi mimo rozsah 
      {
         kal_serial[i][0] = 65535;
         kal_serial[i][1] = 65535;
      }
    
    while (Serial.available())
      {
        delay(1);
        
        long pozice_v_tabulce = Serial.parseInt(); 
        if (pozice_v_tabulce > 15 or pozice_v_tabulce < 1 )
          {
            Serial.print(lng006);                                                //  Serial.print("Vstup mimo rozsah ");
            Serial.println(lng020);                                              //  Serial.println("(1 az 15)");
            break;
          }
        
        uint16_t kal_mereno =  Serial.parseInt(); 
        if (kal_mereno > 24000)
          {
            Serial.print(lng006);                                                //  Serial.print("Vstup mimo rozsah ");
            Serial.println(lng102);                                              //  Serial.println("(merena hodnota max 24 mag/arcsec2 = 24000)");
            break;
          }
        
        uint16_t kal_skutecne =  Serial.parseInt(); 
        if (kal_skutecne > 24000)
          {
            Serial.print(lng006);                                                //  Serial.print("Vstup mimo rozsah ");
            Serial.println(lng103);                                              //  Serial.println("(skutecna hodnota max 24 mag/arcsec2 = 24000)");
            break;
          }

        
        kal_serial[pozice_v_tabulce-1][0] = kal_mereno;                          // polozky, ktere byly zadane, se prepisi z 65535 na zadane hodnoty (muzou to byt i nuly)
        kal_serial[pozice_v_tabulce-1][1] = kal_skutecne;



        if (Serial.available())
          {
            char konec = Serial.read();                                          // precteni oddelovaciho stredniku nebo hvezdicky
            if (konec == '*')
              {
                Serial.println(lng104);                                          //  Serial.println("zadane kalibracni hodnoty pro plosny jas:");
                Serial.println(lng105);                                          //  Serial.println("pozice - merena hodnota - (mag/arcsec2) - spravna hodnota - (mag/arcsec2)");
                uint16_t posun = ((index_cidla_svetla-1) * 60) + eeaddr_svetelna_kalibrace;         // podle aktualniho indexu cidla svetla se zvoli spravna oblast v EEPROM pro kalibracni tabulku
                for (uint8_t i = 0 ; i < 15 ; i++)
                  {
                    if (kal_serial[i][0] != 65535 )                              // ty polozky, ktere byly prepsane z 65535 na neco jineho se zapisi do EEPROM
                      {
                        kal_svetlo[i][0] = kal_serial[i][0];
                        kal_svetlo[i][1] = kal_serial[i][1];
    
                        kal_serial[i][0] = 65535;
                        kal_serial[i][1] = 65535;

                        Serial.print("  ");
                        if ((i+1) < 10)    Serial.print(' ');                   // doplneni uvodni mezery pro indexy 1 az 9
                        Serial.print(i+1);
                        Serial.print("   -      ");

                        if (kal_svetlo[i][0] * kal_svetlo[i][1] > 0)             // obe zadane hodnoty do kalibracni tabulky byly nenulove
                          {
                            EEPROM_write_int(posun   +   (i * 4)   ,  kal_svetlo[i][0]); 
                            EEPROM_write_int(posun + 2 + (i * 4)   ,  kal_svetlo[i][1]); 

                            Serial.print( kal_svetlo[i][0]);
                            Serial.print("     -    (");
                            Serial.print( citelne_svetlo(kal_svetlo[i][0]),3);
                            Serial.print(")   -      ");
                            Serial.print( kal_svetlo[i][1]);
                            Serial.print("      -    (");
                            Serial.print( citelne_svetlo(kal_svetlo[i][1]),3);
                            Serial.println(')');                        
                          }
                        else                                                     // alespon 1 hodnota v kalibracnim bodu byla nulova, cely kalibracni bod se maze
                          {
                            EEPROM_write_int(posun   +   (i * 4)   ,  0); 
                            EEPROM_write_int(posun + 2 + (i * 4)   ,  0); 
                            kal_svetlo[i][0] = 0;
                            kal_svetlo[i][1] = 0;
                            Serial.println(lng210);                              //  "kalibracni bod smazan"
                          }
                      }
                  
                  }
                while (Serial.available()) Serial.read();                        // po hvezdicce se vsechno ostatni maze
              }
          }
      }
    test_korekcni_tabulky_svetla();                                              // zkontroluje spravne zadane udaje
  }
//----------------------------------------------



//----------------------------------------------
void vstup_kalibrace_teploty(void)
  {
    delay(50);                                                                   // v jednom bloku kalibracnich dat budou maximalne 4 radky tabulky (max 55 bajtu)

    for (uint8_t i = 0 ; i < 15 ; i++)                                              // vsech 15 pomocnych promennych pro kalibracni tabulku se nastavi mimo rozsah 
      {
         kal_serial[i][0] = 65535;
         kal_serial[i][1] = 65535;
      }
    
    while (Serial.available())
      {
        delay(1);
        long pozice_v_tabulce = Serial.parseInt();
        if (pozice_v_tabulce > 7 or pozice_v_tabulce < 1 )
          {
            Serial.print(lng006);                                                //  Serial.print("Vstup mimo rozsah ");
            Serial.println(lng106);                                              //  Serial.println("(pozice 1 az 7)");
            break;
          }

        uint16_t kal_mereno =  Serial.parseInt(); 
        if (kal_mereno > 10000)
          {
            Serial.print(lng006);                                                //  Serial.print("Vstup mimo rozsah ");
            Serial.println(lng107);                                              //  Serial.println("(merena hodnota max. 50'C = 10000)");
            break;
          }

        uint16_t kal_skutecne =  Serial.parseInt();
        if (kal_skutecne > 10000 )
          {
            Serial.print(lng006);                                                //  Serial.print("Vstup mimo rozsah ");
            Serial.println(lng108);                                              //  Serial.println("(zobrazena hodnota max. 50'C = 10000)");
            break;
          }

        kal_serial[pozice_v_tabulce-1][0] = kal_mereno;                          // polozky, ktere byly zadane, se prepisi z 65535 na zadane hodnoty (muzou to byt i nuly)
        kal_serial[pozice_v_tabulce-1][1] = kal_skutecne;


        if (Serial.available())
          {
            char konec = Serial.read();                                          // precteni oddelovaciho stredniku nebo hvezdicky
            if (konec == '*')
              {
                Serial.println(lng109);                                          //  Serial.println("zadane kalibracni hodnoty pro teplotu:");
                Serial.println(lng110);                                          //  Serial.println("pozice / merena hodnota ('C) / zobrazena hodnota ('C)");
                for (uint8_t i = 0 ; i < 7 ; i++)
                  {
                    if (kal_serial[i][0] != 65535 )
                      {
                        kal_teplota[i][0] = kal_serial[i][0];
                        kal_teplota[i][1] = kal_serial[i][1];

                        kal_serial[i][0] = 65535;
                        kal_serial[i][1] = 65535;
                        
                        Serial.print("  ");
                        Serial.print(i+1);
                        Serial.print("    -    ");
                        
                        if (kal_teplota[i][0] * kal_teplota[i][1] > 0)           // obe zadane hodnoty do kalibracni tabulky byly nenulove    
                          {
                            EEPROM_write_int(eeaddr_teplotni_kalibrace   + (i * 4)     ,  kal_teplota[i][0]); 
                            EEPROM_write_int(eeaddr_teplotni_kalibrace + 2 + (i * 4)   ,  kal_teplota[i][1]); 
                            
                            Serial.print( kal_teplota[i][0]);                    // merena teplota
                            Serial.print(" (");
                            Serial.print( citelna_teplota(kal_teplota[i][0]));   // (citelna merena teplota)
                            Serial.print(" 'C)  -    ");
                            Serial.print( kal_teplota[i][1]);                    // korekce
                            Serial.print(" (");
                            Serial.print( citelna_teplota(kal_teplota[i][1]));   // (zkorigovana (spravna) citelna teplota)
                            Serial.println(" 'C)");
                          }
                        else
                          {
                            EEPROM_write_int(eeaddr_teplotni_kalibrace     + (i * 4)   ,  0); 
                            EEPROM_write_int(eeaddr_teplotni_kalibrace + 2 + (i * 4)   ,  0); 
                            kal_teplota[i][0] = 0;
                            kal_teplota[i][1] = 0;
                            Serial.println(lng210);                              //  "kalibracni bod smazan"
                          }
                      }
                  }
                while (Serial.available()) Serial.read();                        // po hvezdicce se vsechno ostatni maze
              }
          }
      }
    test_korekcni_tabulky_teploty();                                             // zkontroluje spravne zadane udaje
  }
//----------------------------------------------





// zaznam (aktualni, nebo pozadovany), ktery je ulozeny v "pole_EEPROM[]" se prekonvertuje do pole "ser_out_data[]"
// prevod na citelne datum je zajisten spustenim funkce "priprav_1_zaznam(adresa);"
void EEPROM_to_ser_out(uint16_t index_zaznamu)
  {    

    if (index_zaznamu == 0)                                                                // 0 ... posledni zaznam
      {
        index_zaznamu = (adr_dalsiho_zaznamu - eeaddr_MIN_EEPROM) / delka_zaznamu;
      }

    uint32_t adresa = ((index_zaznamu - 1) * delka_zaznamu) + eeaddr_MIN_EEPROM;      // z nej se vypocte adresa zaznamu v EEPROM
    for (uint8_t i = 0 ; i < delka_zaznamu ; i++)
      {
        pole_EEPROM[i] = EEPROM_read(adresa + i);
      }

    
    uint32_t cas_EEPROM = (pole_EEPROM[1] << 24) + (pole_EEPROM[2] << 16) + (pole_EEPROM[3] << 8) + pole_EEPROM[4];
        
    breakTimeFCE(cas_EEPROM, mtt_EEPROM);                                              // z poctu sekund od 1.1.1970 se ziska citelny datum a cas

    ser_out_data[0]  =  index_zaznamu;                                                 // index zaznamu pocitano od 1 (na adrese 700 je index 1)

    ser_out_data[1]  =  (pole_EEPROM[0] << 8) + pole_EEPROM[7];                        // typ zaznamu (svetlo/stopky/TS...) a specialni znacky k zaznamu (SEC/SELC/stabilita/adresa ...)


    ser_out_data[2]  = (pole_EEPROM[1] << 8) + pole_EEPROM[2];                         // pocet sekund MSW
    ser_out_data[3]  = (pole_EEPROM[3] << 8) + pole_EEPROM[4];                         // pocet sekund LSW

    ser_out_data[4]  = mtt_EEPROM.year+1970;                                           // rok zaznamu v plnem formatu "yyyy"
    ser_out_data[5]  = mtt_EEPROM.month;                                               // mesic zaznamu
    ser_out_data[6]  = mtt_EEPROM.day;                                                 // den zaznamu
    ser_out_data[7]  = mtt_EEPROM.hour;                                                // hodina zaznamu
    ser_out_data[8]  = mtt_EEPROM.minute;                                              // minuta zaznamu
    ser_out_data[9]  = mtt_EEPROM.second;                                              // sekunda zaznamu

    ser_out_data[10] = (pole_EEPROM[5] << 8) + pole_EEPROM[6];                         // svetlo v tisicinach mag/arcsec2

    uint8_t indexpole = 8;


    #ifdef ukladat_infra
    ser_out_data[11] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // infra slozka svetla
    indexpole = indexpole + 2;
    #else
    ser_out_data[11] = 0xFFFF;
    #endif


    #ifdef ukladat_full       
    ser_out_data[12] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // full slozka svetla
    indexpole = indexpole + 2;
    #else
    ser_out_data[12] = 0xFFFF;
    #endif


    #ifdef ukladat_contr_reg
    ser_out_data[13] = pole_EEPROM[indexpole];                                         // konfiguracni registr cidla svetla
    indexpole = indexpole + 1;
    #else
    ser_out_data[13] = 0xFFFF;
    #endif


    #ifdef ukladat_teplotu
    ser_out_data[14] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // teplota v setinach 'C posunute o +50'C (vzdycky kladne cialo) 
    indexpole = indexpole + 2;
    #else
    ser_out_data[14] = 0xFFFF;
    #endif


    #ifdef ukladat_vlhkost    
    ser_out_data[15] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // vlhkost v setinach %
    indexpole = indexpole + 2;
    #else
    ser_out_data[15] = 0xFFFF;
    #endif


    #ifdef ukladat_tlak       
    ser_out_data[16] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // absolutni tlak (neprepocitany na hladinu more) snizeny o 60000Pa 
    indexpole = indexpole + 2;
    #else
    ser_out_data[16] = 0xFFFF;
    #endif


    #ifdef ukladat_GPS
    ser_out_data[17] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // zemepisna delka MSW (v miliontinach stupne. Pro vychod se pricita +180 000 000)
    indexpole = indexpole + 2;
    ser_out_data[18] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // zemepisna delka LSW
    indexpole = indexpole + 2;
    ser_out_data[19] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // zemepisna sirka MSW (v miliontinach stupne. Pro sever se pricita +90 000 000)
    indexpole = indexpole + 2;
    ser_out_data[20] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // zemepisna sirka LSW
    indexpole = indexpole + 2;
    ser_out_data[21] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // nadmorska vyska     (v metrech, vzdycky zvysena o +500m)
    indexpole = indexpole + 2;
    #else
    ser_out_data[16] = 0xFFFF;
    ser_out_data[17] = 0xFFFF;
    ser_out_data[18] = 0xFFFF;
    ser_out_data[19] = 0xFFFF;
    ser_out_data[20] = 0xFFFF;
    ser_out_data[21] = 0xFFFF;
    #endif    


    #ifdef ukladat_naklon       
    ser_out_data[22] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // naklon v desetinach stupne zvyseny o 90 stupnu
    indexpole = indexpole + 2;
    #else
    ser_out_data[22] = 0xFFFF;
    #endif


    #ifdef ukladat_azimut 
    ser_out_data[23] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // azimut ve stupnich
    indexpole = indexpole + 2;
    #else
    ser_out_data[23] = 0xFFFF;
    #endif


    #ifdef ukladat_astro       
    ser_out_data[24] = pole_EEPROM[indexpole];                                         // elevace Slunce ve stupnich, zvetsena o 90 stupnu
    ser_out_data[25] = pole_EEPROM[indexpole+1];                                       // elevace Mesice ve stupnich, zvetsena o 90 stupnu 
    ser_out_data[26] = pole_EEPROM[indexpole+2];                                       // osvetleni Mesice v procentech
    indexpole = indexpole + 3;
    #else
    ser_out_data[24] = 0xFFFF;
    ser_out_data[25] = 0xFFFF;
    ser_out_data[26] = 0xFFFF;
    #endif

    #ifdef ukladat_rezerva_1 
    ser_out_data[27] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // rezerva
    indexpole = indexpole + 2;
    #else
    ser_out_data[27] = 0xFFFF;
    #endif

    #ifdef ukladat_rezerva_2 
    ser_out_data[28] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // rezerva
    indexpole = indexpole + 2;
    #else
    ser_out_data[28] = 0xFFFF;
    #endif

    #ifdef ukladat_rezerva_3 
    ser_out_data[29] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // rezerva
    indexpole = indexpole + 2;
    #else
    ser_out_data[29] = 0xFFFF;
    #endif

    #ifdef ukladat_rezerva_4 
    ser_out_data[30] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // rezerva
    indexpole = indexpole + 2;
    #else
    ser_out_data[30] = 0xFFFF;
    #endif      
  }


//-------------------------------------------------------------------------------------
void AUTOdata_tabulka(void)
  {
    Serial.print(lng379);                                               // "Automaticke odesilani dat "
    if (AUTOkom_linka == 0)
      {
        Serial.println(lng380);                                         // "vypnuto"
      }
    else
      {
        Serial.print('\n');
        Serial.print(lng363);                                           // "   Linka:    "
        if (AUTOkom_linka == 1)                Serial.println(lng365);  // " RS485"
        if (AUTOkom_linka == 2)                Serial.println(lng364);  // " USB"

        Serial.print(lng369);                                           // "   Format:   "
        if (AUTOkom_format == 0)               Serial.println(lng370);  // " data"
        if (AUTOkom_format == 1)               Serial.println(lng371);  // " text"

        Serial.print(lng372);                                           // "   Obsah:    "
        if (AUTOkom_obsah == 15)
          {
            Serial.println(lng378);                                     // " kompletni zaznam "
          }
        else
          {
            Serial.print(lng373);                                       // " datum + jas "
            if ((AUTOkom_obsah  & 0b0001)== 1) Serial.print(lng374);    // "+ pocasi "    
            if ((AUTOkom_obsah  & 0b0010)== 2) Serial.print(lng375);    // "+ detaily jasu "    
            if ((AUTOkom_obsah  & 0b0100)== 4) Serial.print(lng376);    // "+ naklon + azimut "    
            if ((AUTOkom_obsah  & 0b1000)== 8) Serial.print(lng377);    // "+ poloha Slunce a Mesice "    
            Serial.print('\n');
          }
      }
  }
//-------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
// odesle posledni svetelny zaznam do USB seriove linky
void AUTOdata_USB(void)
  {

    bool stab_mark;
    if (AUTOkom_posledni == 0) return;                        // zatim jeste neprobehlo zadne mereni svetla od zapnuti napajeni 
                                                              //      (nebere se posledni ulozeny zaznam, protoze by to mohly byt treba stopky nebo TS)
    uint32_t AUTOadresa = AUTOkom_posledni;    
        
    if (AUTOkom_format == 0)                                 // pole dat
      {

        if ((AUTOkom_obsah & 0b00001111) == 15)              // odeslat uplne cely zaznam
          {
            for (uint8_t i = 0; i< delka_zaznamu ; i++)
              {
                Serial.write(EEPROM_read(AUTOadresa + i));
              }
          }
        else                                                 // odesila se jen omezena cast zaznamu
          {

            AUTOkom_export_pole[0] = (EEPROM_read(AUTOadresa + 1));       // pocet sekund od 1.1.1970 (4 bajty) - odesila se vzdycky a vzdycky se i uklada do EEPROM
            AUTOkom_export_pole[1] = (EEPROM_read(AUTOadresa + 2));
            AUTOkom_export_pole[2] = (EEPROM_read(AUTOadresa + 3));
            AUTOkom_export_pole[3] = (EEPROM_read(AUTOadresa + 4));

            AUTOkom_export_pole[4] = (EEPROM_read(AUTOadresa + 5));       // jas v mmag/arcsec2 - odesila se vzdycky a vzdycky se i uklada do EEPROM
            AUTOkom_export_pole[5] = (EEPROM_read(AUTOadresa + 6));
            for (uint8_t i = 0; i< 6 ; i++)
              {
                Serial.write(AUTOkom_export_pole[i]);
              }
    
            stab_mark  = ((EEPROM_read(AUTOadresa + 7) & 0b00001000) >> 3);   // znacka stability

            AUTOadresa = AUTOadresa + 8;

            #ifdef ukladat_infra
              {
                AUTOkom_export_pole[6] = (EEPROM_read(AUTOadresa));        // infra
                AUTOkom_export_pole[7] = (EEPROM_read(AUTOadresa + 1));
                AUTOadresa = AUTOadresa + 2;                
              }
            #endif

            #ifdef ukladat_full
              {
                AUTOkom_export_pole[8] = (EEPROM_read(AUTOadresa));        // full
                AUTOkom_export_pole[9] = (EEPROM_read(AUTOadresa + 1));
                AUTOadresa = AUTOadresa + 2;                
              }
            #endif
            #ifdef ukladat_contr_reg
              {
                AUTOkom_export_pole[10] = (EEPROM_read(AUTOadresa));        // ctrl reg svetla
                AUTOadresa = AUTOadresa + 1;                
              }
            #endif            
            #ifdef ukladat_teplotu
              {
                AUTOkom_export_pole[11] = (EEPROM_read(AUTOadresa));         // teplota
                AUTOkom_export_pole[12] = (EEPROM_read(AUTOadresa + 1));
                AUTOadresa = AUTOadresa + 2;
              }
            #endif
            #ifdef ukladat_vlhkost
              {
                AUTOkom_export_pole[13] = (EEPROM_read(AUTOadresa));        // vlhkost
                AUTOkom_export_pole[14] = (EEPROM_read(AUTOadresa + 1));
                AUTOadresa = AUTOadresa + 2;
              }
            #ifdef ukladat_tlak
              {
                AUTOkom_export_pole[15] = (EEPROM_read(AUTOadresa));         // tlak
                AUTOkom_export_pole[16] = (EEPROM_read(AUTOadresa + 1));
                AUTOadresa = AUTOadresa + 2;
              }
            #endif
            #endif
            #ifdef ukladat_GPS
              AUTOadresa = AUTOadresa + 10;                                // pri ukladani GPS se 10 bajtu preskakuje
            #endif 
            #ifdef ukladat_naklon
              {
                AUTOkom_export_pole[17] = (EEPROM_read(AUTOadresa));        // naklon
                AUTOkom_export_pole[18] = (EEPROM_read(AUTOadresa+1));
                AUTOadresa = AUTOadresa + 2;                
              }
            #endif
            
            #ifdef ukladat_azimut
              {
                AUTOkom_export_pole[19] = (EEPROM_read(AUTOadresa));        // azimut
                AUTOkom_export_pole[20] = (EEPROM_read(AUTOadresa+1));
                AUTOadresa = AUTOadresa + 2;                
              }          
            #endif
            #ifdef ukladat_astro
              {
                AUTOkom_export_pole[21] = (EEPROM_read(AUTOadresa));        // elevace Slunce
                AUTOkom_export_pole[22] = (EEPROM_read(AUTOadresa + 1));    // elevace Mesice
                AUTOkom_export_pole[23] = (EEPROM_read(AUTOadresa + 2));    // osvetleni Mesice
              }
            #endif

    
            if ((AUTOkom_obsah & 0b00000001) == 1)           // pridat jeste info o pocasi
              {
                #ifdef ukladat_teplotu
                  {
                    Serial.write(AUTOkom_export_pole[11]);
                    Serial.write(AUTOkom_export_pole[12]);
                  }
                #endif
                #ifdef ukladat_vlhkost
                  {
                    Serial.write(AUTOkom_export_pole[13]);
                    Serial.write(AUTOkom_export_pole[14]);
                  }
                #endif
                #ifdef ukladat_tlak
                  {
                    Serial.write(AUTOkom_export_pole[15]);
                    Serial.write(AUTOkom_export_pole[16]);
                  }
                #endif
    
              }
    
            if ((AUTOkom_obsah & 0b00000010) == 2)           // pridat jeste detailni info o svetle
              {
                #ifdef ukladat_infra
                  {
                    Serial.write(AUTOkom_export_pole[6]);
                    Serial.write(AUTOkom_export_pole[7]);
                  }
                #endif
                
                #ifdef ukladat_full
                  {
                    Serial.write(AUTOkom_export_pole[8]);
                    Serial.write(AUTOkom_export_pole[9]);
                  }
                #endif    

                #ifdef ukladat_contr_reg
                  {
                    Serial.write(AUTOkom_export_pole[10]);
                  }
                #endif
                Serial.write(stab_mark);                     // jeden bajt znacky stability 0 / 1
              }
    
    
            if ((AUTOkom_obsah & 0b00000100) == 4)           // pridat jeste info o naklonu a azimutu
              {
                #ifdef ukladat_naklon
                  {
                    Serial.write(AUTOkom_export_pole[17]);
                    Serial.write(AUTOkom_export_pole[18]);
                  }
                #endif
                
                #ifdef ukladat_azimut
                  {
                    Serial.write(AUTOkom_export_pole[19]);
                    Serial.write(AUTOkom_export_pole[20]);
                  }          
                #endif
              }
    
            if ((AUTOkom_obsah & 0b00001000) == 8)           // pridat jeste info o Slunci a Mesici
              {
                #ifdef ukladat_astro
                  {
                    Serial.write(AUTOkom_export_pole[21]);    // elevace Slunce
                    Serial.write(AUTOkom_export_pole[22]);    // elevace Mesice
                    Serial.write(AUTOkom_export_pole[23]);    // osvetleni Mesice
                  }
                #endif
              }
          }
      }
    else                                                     // odesilat jako citelny text
      {

        priprav_1_zaznam(AUTOadresa);                        // do globalni promenne 'vystupni retezec' citelny text s jednotlivymi polozkami
        if ((AUTOkom_obsah & 0b00001111) == 15)              // odeslat uplne cely zaznam
          {
            print_co_kam(true, false);                       // Serial.println(vystupni_retezec);
          }
        else                                                 // odesila se jen omezena cast zaznamu
          {

            for (uint16_t i = 30; i < 54 ; i++)          // " 29.12.2020 ; 08:59:23 ;"    - datum a cas se tiskne vzdycky
              {
                Serial.print(vystupni_retezec[i]);
              }

            for (uint16_t i = 74; i < 83; i++)           // " 08,881 ;"   - jas se tiskne vzdycky
              {
                Serial.print(vystupni_retezec[i]);
              }
            
    
    
            if ((AUTOkom_obsah & 0b00000001) == 1)           // pridat jeste info o pocasi
              {
                for (uint16_t i = 150; i < 159; i++)     // " +21,96 ;"   - teplota
                  {
                    Serial.print(vystupni_retezec[i]);
                  }

                for (uint16_t i = 167; i < 186; i++)     // " 051,51 ; 093410 ;"   - vlhkost ; tlak
                  {
                    Serial.print(vystupni_retezec[i]);
                  }
              }
    
            if ((AUTOkom_obsah & 0b00000010) == 2)           // pridat jeste detailni info o svetle
              {
                for (uint16_t i = 101; i < 142; i++)     // "  STABIL  ; 17989 ; 48311 ; 600 ;  428x ;"   -stabilita ; infra ; full; ATIME; AGAIN
                  {
                    Serial.print(vystupni_retezec[i]);
                  }
              }
    
    
            if ((AUTOkom_obsah & 0b00000100) == 4)           // pridat jeste info o naklonu a azimutu
              {
                for (uint16_t i = 220; i < 235; i++)     // " +165.8 ; 123 ;"   - naklon ; azimut
                  {
                    Serial.print(vystupni_retezec[i]);
                  }
              }

    
            if ((AUTOkom_obsah & 0b00001000) == 8)           // pridat jeste info o Slunci a Mesici
              {
                for (uint16_t i = 235; i < 256; i++)     // "  -45 ;  -27 ;  58  ;"   - elevace Slunce ; elevace Mesice; osvetleni Mesice
                  {
                    Serial.print(vystupni_retezec[i]);
                  }
              }
            Serial.print('\n');
          }
      }
  }
//--------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------
// pri zadani parametr f=2 ve funkci @A x,x,2,x se nic neuklada, ale jen se vypise hlavicka aktualne vypisovanych parametru
void AUTO_hlavicka(void)
  {
    if (AUTOkom_format == 0)                                 // pole dat
      {
        if ((AUTOkom_obsah & 0b00001111) == 15)              // odeslat uplne cely zaznam
          {
            for (uint8_t i = 0 ; i < 18 ; i++)
              {
                Serial.println(head_byte[i]);
              }
          }
        else                                                 // odesila se jen cast zaznamu
          {
            Serial.println(head_byte[1]);                    // "4x sek1970"
            Serial.println(head_byte[2]);                    // "2x jas"
            if ((AUTOkom_obsah & 0b00000001) == 1)           // pridat jeste info o pocasi
              {
                #ifdef ukladat_teplotu
                Serial.println(head_byte[7]);                // "2x teplota"
                #endif
                #ifdef ukladat_vlhkost
                Serial.println(head_byte[8]);                // "2x vlhkost"
                #endif
                #ifdef ukladat_tlak
                Serial.println(head_byte[9]);                // "2x tlak"
                #endif
              }

            if ((AUTOkom_obsah & 0b00000010) == 2)           // pridat jeste detailni info o svetle
              {
                #ifdef ukladat_infra
                Serial.println(head_byte[4]);                // "2x infra"
                #endif
                #ifdef ukladat_full
                Serial.println(head_byte[5]);                // "2x full"
                #endif
                #ifdef ukladat_contr_reg
                Serial.println(head_byte[6]);                // "1x config"
                #endif
                Serial.println(head_byte[22]);               // "1x stabilita"
              }

            if ((AUTOkom_obsah & 0b00000100) == 4)           // pridat jeste info o naklonu a azimutu
              {
                #ifdef ukladat_naklon
                Serial.println(head_byte[13]);               // "2x naklon" 
                #endif
                #ifdef ukladat_azimut
                Serial.println(head_byte[14]);               // "2x azimut"
                #endif
              }
            if ((AUTOkom_obsah & 0b00001000) == 8)           // pridat jeste info o Slunci a Mesici
              {
                #ifdef ukladat_astro
                Serial.println(head_byte[15]);               // "1x elevace Slunce"
                Serial.println(head_byte[16]);               // "1x elevace Mesic"
                Serial.println(head_byte[17]);               // "1x osvetleni Mesice"
                #endif
              }              
           
          }

      }
    else                                                     // textovy vypis
      {
        if ((AUTOkom_obsah & 0b00001111) == 15)              // odeslat uplne cely zaznam
          {
            print_co_kam(false, false);                      // Serial.println(hlavicka);
          }
        else                                                 // odesila se jen omezena cast zaznamu
          {
            for (uint16_t i = 30; i < 54 ; i++)          // "    datum   ;    cas   ;"    - datum a cas se tiskne vzdycky
              {
                Serial.print(hlavicka[i]);
              }

            for (uint16_t i = 74; i < 83; i++)           // "   jas  ;"   - jas se tiskne vzdycky
              {
                Serial.print(hlavicka[i]);
              }
            
    
    
            if ((AUTOkom_obsah & 0b00000001) == 1)           // pridat jeste info o pocasi
              {
                for (uint16_t i = 150; i < 159; i++)     // " tep.'C ;"   - teplota
                  {
                    Serial.print(hlavicka[i]);
                  }

                for (uint16_t i = 167; i < 186; i++)     // " vlh. % ;  tlak  ;"   - vlhkost ; tlak
                  {
                    Serial.print(hlavicka[i]);
                  }
              }
    
            if ((AUTOkom_obsah & 0b00000010) == 2)           // pridat jeste detailni info o svetle
              {
                for (uint16_t i = 101; i < 142; i++)     // " stab_jas ; infra ;  full ; [ms]; citl. ;"   -stabilita ; infra ; full; ATIME; AGAIN
                  {
                    Serial.print(hlavicka[i]);
                  }
              }
    
    
            if ((AUTOkom_obsah & 0b00000100) == 4)           // pridat jeste info o naklonu a azimutu
              {
                for (uint16_t i = 220; i < 235; i++)     // " naklon ; azm ;"   - naklon ; azimut
                  {
                    Serial.print(hlavicka[i]);
                  }
              }

    
            if ((AUTOkom_obsah & 0b00001000) == 8)           // pridat jeste info o Slunci a Mesici
              {
                for (uint16_t i = 235; i < 256; i++)     // " S_el ; M_el ; M_sv ;"   - elevace Slunce ; elevace Mesice; osvetleni Mesice
                  {
                    Serial.print(hlavicka[i]);
                  }
              }
            Serial.print('\n');
          }
      }       
  }
//--------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------
uint16_t DSO_rd_data(uint8_t start_pozice)
  {
    uint16_t vysledek = 0;
    uint8_t i;

    for (i = start_pozice; i < 255 ; i++)    // v prvnim kroku prohleda buffer od zadane pozice a najde prvni vyskyt nejakeho cisla (preskakuji se tim pripadne mezery )
      {
        if(read_buffer[i] >= '0' and read_buffer[i] <= '9') break;
      }

    // v promenne 'i' je ted index prvniho vyskytu nejakeho ciselneho znaku ('0' az '9')

    if (i == 255) return 65535;       // od zadaneho indexu nebyl objeven zadny ciselny znak, vraci se chyba (65535)
    
    while ( read_buffer[i] >= '0' and read_buffer[i] <= '9')   // dokud buffer obsahuje ciselne znaky, sklada se z nich vysledek
      {
        vysledek = vysledek * 10;                             // predchozi hodnota se vynasobi 10x a na pozici jednotek se pricte aktualni cifra
        vysledek = vysledek + (read_buffer[i] - '0');
        i++;                                                  // ukazatel do bufferu se zvysi
      }
    return vysledek;  
  }


//--------------------------------------------------------------------------------------------
// Odesle do seriove linky 1 kod znaku a jeho index a ovladaci znakovy kod
//    zvysi globalni promennou index_kodu o 1
// priklad:
//        043;#;U
void posli_kod_f(char kod_znaku)
  {
    if (index_kodu < 100) Serial.print('0');
    if (index_kodu <  10) Serial.print('0');
    
    Serial.print(index_kodu);
    Serial.print(';');
    Serial.print(ovladaci_znak);    
    Serial.print(';');
    Serial.println(kod_znaku);
    delay(5);
    index_kodu ++;
  }

//--------------------------------------------------------------------------------------------
