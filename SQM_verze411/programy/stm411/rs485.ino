// Podprogramy pro komunikaci pres linku RS485
//================================================



//----------------------------------------------
// testovani, jestli se v prijimacim bufferu objevila nejaka zadost
void test_485_kom(void)
  {
    if (millis() - casova_znacka > ignor_cas)                                          // ignoracni prodleva pro komunikaci vyprsela
      {
        LED_B_onoff(false);                                                            // konec ignorance
      }

    if(Serial2.available())                                                            // nejaka data na seriove lince
      {
        char prvni_znak = Serial2.peek();                                              // '*a nnn' test existence SQM na pozadovanem portu 
        if (prvni_znak == '*')
          {
            Serial2.read();
            prvni_znak = Serial2.read();                                                // ve skutecnosti druhy znak by mel byt 'a'
            if (prvni_znak == 'a')
              {
                uint8_t pozadovana_adresa = Serial2.parseInt(); 
                if (pozadovana_adresa == slave_addr)                                       // pozadovana adresa odpovida
                  {
                    vysilani();
                    Serial2.print('2');                                                     // vrati se znacka komunikace RS485
                    prijem();
                  }
              }
          }

        if (millis() - casova_znacka > ignor_cas)                                      // ignoracni prodleva pro komunikaci vyprsela
          {

            bool chyba = false;
            delay(20);                                                                 // pockej na prijem 13 bajtu od MASTERa 

            for (uint8_t i = 1 ; i < 14 ; i++)
              {
                if (Serial2.available())
                  {
                    prijimaci_pole[i] = Serial2.read();                    
                  }
                else
                 {
                   chyba = true;                                                       // v bufferu neni 13 bajtu, takze je neco spatne
                   if (bitRead(signal_LED,2) == true)                                  // je povoleno hlaseni chyb pomoci LED
                     {
                       LED_B_onoff(true);                                              // chyba komunikace                                    
                     }
                   break;                                                              // smycka se predcasne ukonci
                 }
              }
              
            if ((prijimaci_pole[1] == slave_addr or prijimaci_pole[1] == 127) and chyba == false)
              {                              
                 if (bitRead(signal_LED,3) == true)                                  // je povoleno bliknuti LED pri komunikaci
                   {
                     blik_C_kratky();                                                // prisel nejaky pozadavek, blikne svetle modra LED
                   }
                 zpracuj_pozadavek();
              }
            else
              {
                 casova_znacka = millis();
                   if (bitRead(signal_LED,2) == true)                                  // je povoleno hlaseni chyb pomoci LED
                     {
                       LED_B_onoff(true);                                              // 3 sek zacatek ignorance
                     }
              }
      
          }
        else                                                                           // trva cas pro ignorovani komunikace
          {
            while (Serial2.available())  Serial2.read();
          }
      }
  }
//----------------------------------------------



//----------------------------------------------
// prepnuti smeru na prijem
void prijem(void)
  {
    delay(50);
    digitalWrite(pin_DERE,LOW);
    delay(20);
  }
//----------------------------------------------



//----------------------------------------------
// prepnuti smeru na vysilani
void vysilani(void)
  {
    delay(50);
    digitalWrite(pin_DERE,HIGH);
    delay(20);
  }
//----------------------------------------------



//----------------------------------------------
// prisla nejaka zadost
void zpracuj_pozadavek(void)
  { 
    uint8_t CRCneg;

//    DEBUG: prijata data
//    for (uint8_t i = 0; i < 14 ; i++)
//      {
//        Serial.print(i);
//        Serial.print(" - ");
//        Serial.println(prijimaci_pole[i]);
//      }
//    Serial.println("----");

    
    if (pouzij_CRC > 0)    CRCneg  = kontrolni_bajt(12,prijimaci_pole);                // kdyz se ma testovat CRC, tak se vypocte
    else                   CRCneg  = prijimaci_pole[13];                               // kdyz se testovat nema, tak se jen zkopiruje z prijateho pole, aby vzdycky souhlasil
    
    if (prijimaci_pole[13] == CRCneg)                                                  // kontrolni soucet souhlasi
      {
        vysilaci_pole[0] = slave_addr;
        vysilaci_pole[1] = prijimaci_pole[2];
        vysilani();
        switch (prijimaci_pole[2])
          {

            case 1:                                                                    // spustit normalni mereni
              {
                if (dokoncene_mereni == 2)                                             //dalsi mereni je mozne spustit az kdyz je predchozi dokoncene
                  {
                    vysilaci_pole[2] = prumery;
                    vysilaci_pole[3] = kontrolni_bajt(3,vysilaci_pole);
                    odesli_pole(4);
                    mereni(5);
                  }
                else                                                                   // kdyz predhcozi mereni jeste neni dokoncene, vraci se misto poctu prumeru znacka BUSSY (0)
                  {
                    vysilaci_pole[2] = 0;                                              // BUSSY 
                    vysilaci_pole[3] = kontrolni_bajt(3,vysilaci_pole);
                    odesli_pole(4);
                  }
                break;
              }



            case 2:                                                                    // pozadavek o posledni zmerene hodnoty (je jedno, jestli byly porizeny tlacitkem, automatem, nebo spustenim na dalku)
              {
                vysilaci_pole[1] = dokoncene_mereni;

                if (dokoncene_mereni == 0)                                             // mereni jeste nebylo dokonceno
                  {
                    switch(prijimaci_pole[3])
                      {
                        case 0:                                                        // bylo pozadano o data v bajtech ...
                          {
                            vysilaci_pole[2] = prubeh_mereni;                          // ... vraci se blok bajtu s informaci o nedokoncenem mereni
                            vysilaci_pole[3] = kontrolni_bajt(3,vysilaci_pole);
                            odesli_pole(4);
                            break;
                          }  

                        case 1:                                                        // bylo pozadano o data v textovem formatu ...
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 7:
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                          {
                            Serial2.flush();
                            Serial2.print("* ");                                       // ... vraci se informace o nedokoncenem mereni v textovem formatu
                            if (prubeh_mereni < 10)    Serial2.print('0');
                            Serial2.print(prubeh_mereni);
                            prijem();
                            break;
                          }
 
                        case 6:                                                        // textova hlavicka se muze zobrazit i kdyz jeste neni dokoncene mereni
                          {
                            Serial2.flush();
                            for (uint16_t i = 0; i < velikost_hlavicky ; i++)
                              {
                                Serial2.write(hlavicka[i]); 
                                delay(1);
                              }
                            prijem();
                            break;                      
                          }
                      }
                  }
                else                                                                   // mereni bylo dokonceno
                  {
                    if (adr_posl_zaznamu > 0)
                      {
                        priprav_1_zaznam(adr_posl_zaznamu);                            // v promenne 'adr_posl_zaznamu' je ulozena adresa zacatku posledniho zaznamu
                        switch(prijimaci_pole[3])
                          {
                            case 1:                                                    // vsechna posledni zmerena data se vypisou jako ASCII citelne hodnoty
                              {
                                Serial2.flush();
                                for (uint16_t i = 0; i < velikost_hlavicky ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                    delay(1);
                                  }
                                prijem();
                                break;
                              }
    
                            case 2:                                                    // svetlo v citelnem tvaru
                              {
                                Serial2.flush();
                                for (uint8_t i = 75; i < 81 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                Serial2.print(" mag/arcsec^2");
                                prijem();
                                break;
                              }
    
                            case 3:                                                    // teplota v citelnem tvaru
                              {
                                Serial2.flush();
                                for (uint8_t i = 151; i < 157 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                Serial2.print(" 'C");
                                prijem();
                                break;
                              }
                            
                            case 4:                                                    // vlhkost v citelnem tvaru
                              {
                                Serial2.flush();
                                for (uint8_t i = 168; i < 174 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                Serial2.print(" %");
                                prijem();
                                break;
                              }
                            
                            case 5:                                                    // Tlak v citelnem tvaru
                              {
                                Serial2.flush();
                                for (uint8_t i = 177; i < 183 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                Serial2.print(" Pa");
                                prijem();
                                break;
                              }
                              
                            case 6:                                                    // Zobrazeni hlavicky
                              {
                                Serial2.flush();
                                for (uint16_t i = 0; i < velikost_hlavicky ; i++)
                                  {
                                    Serial2.write(hlavicka[i]); 
                                    delay(1);
                                  }
                                prijem();
                                break;
                              }
    
                            case 7:                                                    // Zemepisne souradnice v citelnem tvaru (  GEO_lat  ;   GEO_long  ;  ALT  ) oddelene stredniky
                              {
                                Serial2.flush();
                                for (uint8_t i = 185; i < 219 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                prijem();
                                break;
                              }

                            case 8:                                                    // Naklon pristroje pri mereni v citelnem tvaru
                              {
                                Serial2.flush();
                                for (uint8_t i = 221; i < 227 ; i++)
                                  {
                                    Serial2.write(hlavicka[i]); 
                                  }
                                Serial2.print(" = ");  
                                for (uint8_t i = 221; i < 227 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                Serial2.print(" '");                                   // znak stupne neprojde pres seriovou linku, proto je tu apostrof (i kdyz v mereni uhlu muze zpusobit zmateni s uhlovymi minutami)
                                prijem();
                                break;
                              }

                            case 12:                                                   // azimut v citelnem tvaru
                              {
                                Serial2.flush();
                                for (uint8_t i = 230; i < 233 ; i++)
                                  {
                                    Serial2.write(hlavicka[i]); 
                                  }
                                Serial2.print(" = ");  
                                for (uint8_t i = 230; i < 233 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                Serial2.print(" '");                                   // znak stupne neprojde pres seriovou linku, proto je tu apostrof (i kdyz v mereni uhlu muze zpusobit zmateni s uhlovymi minutami)
                                prijem();
                                break;
                              }

                            case 9:                                                    // Elevace Slunce pri mereni v citelnem tvaru
                              {
                                Serial2.flush();
                                for (uint8_t i = 236; i < 240 ; i++)
                                  {
                                    Serial2.write(hlavicka[i]); 
                                  }
                                Serial2.print(" = ");  
                                for (uint8_t i = 237; i < 240 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                Serial2.print(" '");  
                                prijem();
                                break;
                              }

                            case 10:                                                   // Elevace Mesice pri mereni v citelnem tvaru
                              {
                                Serial2.flush();
                                for (uint8_t i = 243; i < 247 ; i++)
                                  {
                                    Serial2.write(hlavicka[i]); 
                                  }
                                Serial2.print(" = ");  
                                for (uint8_t i = 244; i < 247 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                Serial2.print(" '");  
                                prijem();
                                break;
                              }

                            case 11:                                                   // Osvetleni Mesice pri mereni v citelnem tvaru
                              {
                                Serial2.flush();
                                for (uint8_t i = 249; i < 254 ; i++)
                                  {
                                    Serial2.write(hlavicka[i]); 
                                  }
                                Serial2.print(" = ");  
                                for (uint8_t i = 251; i < 253 ; i++)
                                  {
                                    Serial2.write(vystupni_retezec[i]); 
                                  }
                                Serial2.print(" %");  
                                prijem();
                                break;
                              }
                            
                            default: 
                              {                          
                                vysilaci_pole[2] = delka_zaznamu;
                                for (uint8_t i = 0; i < delka_zaznamu ; i++)
                                  {                               
                                    vysilaci_pole[3+i] = EEPROM_read(adr_posl_zaznamu + i);              // bere se posledni ULOZENY zaznam z EEPROM                              
                                  }
                                vysilaci_pole[3 + delka_zaznamu] = kontrolni_bajt(3 + delka_zaznamu,vysilaci_pole);
                                odesli_pole(4 + delka_zaznamu);
                                break;
                              }
                          }                        
                      }
                    else
                      {
                        switch(prijimaci_pole[3])
                          {
                            case 0:                                                    // bylo pozadano o data v bajtech ...
                              {
                                vysilaci_pole[2] = 0;                                  // chyba (zaznam neexistuje)
                                vysilaci_pole[3] = kontrolni_bajt(3,vysilaci_pole);
                                odesli_pole(4);
                                break;
                              }  
                            default:                                                   // bylo pozadano o data textovem formatu ...
                              {
                                Serial2.flush();
                                Serial2.print(lng168);                                 // "Zadny ulozeny zaznam"
                                prijem();
                                break;
                              }
                          }
                      }

                  }
                break;
              }




            case 3:                                                                    // vratit zaznam se zvolenym indexem
              {
                uint16_t index_zaznamu = (prijimaci_pole[3] * 256 + prijimaci_pole[4]) - 1; 
                uint32_t adresa = (index_zaznamu * delka_zaznamu) + eeaddr_MIN_EEPROM;

                if (((EEPROM_read(adresa) & 0b10000000) == 0) or (adresa > max_zaznam_EEPROM))       // pozadovany zaznam neexistuje, nebo je dokonce mimo rozsah EEPROM
                  {
                    if (prijimaci_pole[5] == 0)
                      {
                        vysilaci_pole[2] = 0;                                          // chyba (zaznam neexistuje, nebo je mimo rozsah EEPROM)
                        vysilaci_pole[3] = kontrolni_bajt(3,vysilaci_pole);
                        odesli_pole(4);
                      }

                    if (prijimaci_pole[5] == 1)
                      {
                        Serial2.print("--- ( max: ");                                  // Zvolena polozka neexistuje, nebo je mimo rozsah EEPROM
                        Serial2.print((eeaddr_MAX_EEPROM - eeaddr_MIN_EEPROM) / delka_zaznamu); 
                        Serial2.println(')'); 
                        prijem();
                      }
                  }
                else                                                                   // pozadovany zaznam existuje
                  {
                    switch(prijimaci_pole[5])
                      {
                        case 0:                                                        // bylo pozadano o data v bajtech ...
                          {
                            vysilaci_pole[2] = delka_zaznamu;
                            for (uint8_t i = 0; i< delka_zaznamu ; i++)
                              {
                                vysilaci_pole[3 + i] =  EEPROM_read(adresa + i);
                              }
                            vysilaci_pole[3 + delka_zaznamu] = kontrolni_bajt(3 + delka_zaznamu,vysilaci_pole);
                            odesli_pole(4 + delka_zaznamu);
                            prijem();
                            break;
                          }
    
                        case 1:                                                        // bylo pozadano o data v textovem formatu ...
                          {
                            Serial2.flush();
                            priprav_1_zaznam(adresa);
                            for (uint16_t i = 0; i< velikost_hlavicky ; i++)
                              {
                                Serial2.write(vystupni_retezec[i]); 
                                delay(1);
                              }
                            prijem();
                            break;
                          }
                      }
                  }
                break;
              }


            case 4:      // nastavit datum a cas
              {
                bool chyba= false;
                LOC_rok = prijimaci_pole[3]+2000;
                if (LOC_rok > 2099 || LOC_rok < 2020) chyba = true;                    // test, ze je rok v rozsahu 2020 az 2099
                LOC_mes = prijimaci_pole[4];
                if (LOC_mes > 12 || LOC_mes < 1)      chyba = true;                    // test, ze je mesic v rozsahu 1 az 12
                LOC_den = prijimaci_pole[5];
                if (LOC_den > 31 || LOC_den < 1)      chyba = true;                    // test, ze je den v rozsahu 1 az 31 (nebere se ohled na to, kolik ma mesic ve skutecnosti dni)
                LOC_hod = prijimaci_pole[6];
                if (LOC_hod > 23 || LOC_hod < 0)      chyba = true;                    // test, ze jsou hodiny v rozsahu 0 az 23
                LOC_min = prijimaci_pole[7];
                if (LOC_min > 59 || LOC_min < 0)      chyba = true;                    // test, ze jsou minuty v rozsahu 0 az 59
                LOC_sek = prijimaci_pole[8];
                if (LOC_sek > 59 || LOC_sek < 0)      chyba = true;                    // test, ze jsou sekundy v rozsahu 0 az 60
                if (chyba == true)
                  {
                    vysilaci_pole[1] = 255;                                            // kdyz je hodnota k zapisu mimo rozsah, misto cisla funkce se vrati cislo 255 a nic se neuklada
                    vysilaci_pole[2] = kontrolni_bajt(2,vysilaci_pole);
                    odesli_pole(3);
                  }
                else                                                                   // zadani vsech parametru je v poradku
                  {
                    vysilaci_pole[2] = kontrolni_bajt(2,vysilaci_pole);
                    odesli_pole(3);                    

                    //--------------- zapis casu do RTC vcetne pripadnych korekci do EEPROM ---------
                    
                    uint32_t aktualni_cas_RTC = rtclock.getY2kEpoch();
                    uint32_t tt;
                    mtt.year     = LOC_rok - 1970;                                     // knihovna je napsana tak, ze se cas pocita od 1.1.1970
                    mtt.month    = LOC_mes;
                    mtt.day      = LOC_den;
                    mtt.hour     = LOC_hod;
                    mtt.minute   = LOC_min;
                    mtt.second   = LOC_sek;

                    if (leto == false) tt = makeTimeFCE(mtt) - (60*60*zimni_posun);    // v zime se pri ukladani casu uklada do RTC o hodinu mene (prevod SEC na UTC)
                    else               tt = makeTimeFCE(mtt) - (60*60*letni_posun);    // kdyz se nastavuje cas v lete (SELC), do RTC se uklada o dalsi hodinu mene (prevod SELC na SEC)
                    
                    rtclock.setY2kEpoch(tt);                                           // skutecne prenastaveni casu v RTC na UTC cas

                    cas_minuleho_nastaveni = EEPROM_read_long(eeaddr_RTC_set);
                    uint32_t interval;

                    if (tt > cas_minuleho_nastaveni)    interval = tt - cas_minuleho_nastaveni;     // interval musi byt vzdycky kladny
                    else                                interval = cas_minuleho_nastaveni - tt;
                    
                    long odchylka = aktualni_cas_RTC - tt;
                    
                    if (interval < 86400UL)                                            // od predchoziho serizeni ubehlo min nez 24 hodin
                      {
                        if (odchylka > 172800L or odchylka < -172800L )                // rozdil mezi zadavanym casem a casem v RTC je vic, nez 2 dny. To je CHYBA
                          {
                            korekce = 0x7FFFFFFFUL;
                          }
                        else                                                           // "prijatelna odchylka, korekce se ale nepocita - prebira se puvodni"
                          {
                            korekce = EEPROM_read_long(eeaddr_RTC_korekce);
                          }
                      
                      }
                    
                    if (interval > 15768000UL)                                         // od predchoziho serizeni ubehlo vic nez pul roku
                      {
                        korekce = 0x7FFFFFFFUL;                                        // korekce se rusi
                      }
                
                    if (interval <= 15768000UL and interval >=86400UL)                 // od predchoziho serizeni ubehlo vic nez den, ale min nez pul roku
                      {                                                                //  "spravny interval od posledniho serizeni"
                        if (odchylka > 172800L or odchylka < -172800L )                // rozdil mezi zadavanym casem a casem v RTC je vic, nez 2 dny. To je CHYBA
                          {
                            korekce = 0x7FFFFFFFUL;
                          }
                        else                                                           // "prijatelna odchylka, pocita se korekce"
                          {
                            
                            if (odchylka  < 0) korekce = interval / -odchylka;         // odchylka je zaporna, sekundy se budou pricitat -  (odchylka se prevadi na kladne cislo)
                            if (odchylka  > 0) korekce = (0x80000000UL | (interval / odchylka));  // nejvyssi bit je znamenko, korekce se bude odecitat
                            if (odchylka == 0) korekce = 0x7FFFFFFFUL;                 // cas v RTC je stejny, jako zadany cas - zadna korekce neni potreba (prvni extra sekunda za 68 let) 
                
                            if ((korekce & 0x7FFFFFFFUL) < 300)                        // sekunda se ma pricitat, nebo odecitat driv, nez za 5 minut - to vypada na chybu RTC
                              {
                                korekce = 0x7FFFFFFFUL;
                              }
                          
                          }      
                      }        

                      
                    cas_minuleho_nastaveni = tt;                                       // nastaveni globalni promenne, aby se nemulo pri kazdem zjistovani casu sahat do EEPROM
                    EEPROM_write_long(eeaddr_RTC_set,tt);                              // do EEPROM se v kazdem pripade ulozi cas posledniho nastaveni
                
                
                    if (korekce != EEPROM_read_long(eeaddr_RTC_korekce))               // do EEPROM se nova korekce uklada jen v pripade, ze se zmenila
                      {
                        EEPROM_write_long(eeaddr_RTC_korekce , korekce);
                      }
                    // ---------------------------
                  }
                break;
              }


            case 5:      // precist datum a cas
              {
                vysilaci_pole[2] = prijimaci_pole[3];
              
                if (prijimaci_pole[3] == 0)                                            // cas vcetne korekci
                  {
                    uint32_t tt = rtc_korekce();                                  // aktualni cas v RTC v sekundach od 1.1.1970
                    
                    sek1970_datcas(tt,false);
                    
                    vysilaci_pole[3]  = LOC_rok - 2000;
                    vysilaci_pole[4]  = LOC_mes;
                    vysilaci_pole[5]  = LOC_den;
                    vysilaci_pole[6]  = LOC_hod;
                    vysilaci_pole[7]  = LOC_min;
                    vysilaci_pole[8]  = LOC_sek;
                    vysilaci_pole[9]  = LOC_dvt;
                    vysilaci_pole[10] = leto;

                    vysilaci_pole[11] = (tt >> 24) & 255;                              // pocet sekund od 1.1.1970
                    vysilaci_pole[12] = (tt >> 16) & 255;
                    vysilaci_pole[13] = (tt >>  8) & 255;
                    vysilaci_pole[14] = (tt      ) & 255;

                    
                    vysilaci_pole[15] = kontrolni_bajt(15,vysilaci_pole);
                    odesli_pole(16);
                  }

                if (prijimaci_pole[3] == 1)                                            // cas primo v RTC bez korekci v UTC
                  {
                    uint32_t tt = rtclock.getY2kEpoch();           // aktualni cas v RTC v sekundach od 1.1.1970 (UTC bez autokorekce)
                    sek1970_datcas(tt,false);
                                          
                    vysilaci_pole[3]  = LOC_rok - 2000;
                    vysilaci_pole[4]  = LOC_mes;
                    vysilaci_pole[5]  = LOC_den;
                    vysilaci_pole[6]  = LOC_hod;
                    vysilaci_pole[7]  = LOC_min;
                    vysilaci_pole[8]  = LOC_sek;
                    vysilaci_pole[9]  = LOC_dvt;
                    vysilaci_pole[10] = leto;

                    vysilaci_pole[11] = (tt >> 24) & 255;                              // pocet sekund od 1.1.1970
                    vysilaci_pole[12] = (tt >> 16) & 255;
                    vysilaci_pole[13] = (tt >>  8) & 255;
                    vysilaci_pole[14] = (tt      ) & 255;

                    
                    vysilaci_pole[15] = kontrolni_bajt(15,vysilaci_pole);
                    odesli_pole(16);
                  }

                if (prijimaci_pole[3] == 2)                                            // Datum a cas posledniho serizeni
                  {
                    uint32_t tt = EEPROM_read_long(eeaddr_RTC_set);               // precte z EEPROM cas posledniho serizeni v sekundach od 1.1.1970 
                    sek1970_datcas(tt,false);
                                          
                    vysilaci_pole[3]  = LOC_rok - 2000;
                    vysilaci_pole[4]  = LOC_mes;
                    vysilaci_pole[5]  = LOC_den;
                    vysilaci_pole[6]  = LOC_hod;
                    vysilaci_pole[7]  = LOC_min;
                    vysilaci_pole[8]  = LOC_sek;
                    vysilaci_pole[9] = kontrolni_bajt(9,vysilaci_pole);
                    odesli_pole(10);
                  }

                if (prijimaci_pole[3] == 3)                                            // Korekcni dvojbajt (za jak dlouho se ma k RTC pricist, nebo odecit sekunda)
                  {
                                                              
                    vysilaci_pole[3]  = EEPROM_read(eeaddr_RTC_korekce);
                    vysilaci_pole[4]  = EEPROM_read(eeaddr_RTC_korekce + 1);
                    vysilaci_pole[5]  = EEPROM_read(eeaddr_RTC_korekce + 2);
                    vysilaci_pole[6]  = EEPROM_read(eeaddr_RTC_korekce + 3);
                    vysilaci_pole[7] = kontrolni_bajt(7,vysilaci_pole);
                    odesli_pole(8);
                  }

                if (prijimaci_pole[3] == 4)                                            // Textove zobrazeni datumu a casu z RTC
                  {
                    uint32_t tt = rtc_korekce();                                  // aktualni cas v RTC v sekundach od 1.1.1970
                                      
                    sek1970_datcas(tt,false);

                    vysilaci_pole[0]  = 48 + (LOC_den / 10);
                    vysilaci_pole[1]  = 48 + (LOC_den % 10);
                    vysilaci_pole[2]  = '.';

                    vysilaci_pole[3]  = 48 + (LOC_mes / 10);
                    vysilaci_pole[4]  = 48 + (LOC_mes % 10);
                    vysilaci_pole[5]  = '.';
                    
                    vysilaci_pole[6]  = '2';
                    vysilaci_pole[7]  = '0';
                    vysilaci_pole[8]  = 48 + ((LOC_rok - 2000) / 10);
                    vysilaci_pole[9]  = 48 + ((LOC_rok - 2000) % 10);
                    vysilaci_pole[10] = ' ';
                    
                    vysilaci_pole[11] = 48 + (LOC_hod / 10);
                    vysilaci_pole[12] = 48 + (LOC_hod % 10);
                    vysilaci_pole[13] = ':';

                    vysilaci_pole[14] = 48 + (LOC_min / 10);
                    vysilaci_pole[15] = 48 + (LOC_min % 10);
                    vysilaci_pole[16] = ':';

                    vysilaci_pole[17] = 48 + (LOC_sek / 10);
                    vysilaci_pole[18] = 48 + (LOC_sek % 10);
                    vysilaci_pole[19] = ' ';

                    if (leto == true)
                      {
                        vysilaci_pole[20] = char_leto[0];                              // S
                        vysilaci_pole[21] = char_leto[1];                              // E
                        vysilaci_pole[22] = char_leto[2];                              // L
                        vysilaci_pole[23] = char_leto[3];                              // C
                      }
                    else
                      {
                        vysilaci_pole[20] = char_zima[0];                              // ' '  
                        vysilaci_pole[21] = char_zima[1];                              // S
                        vysilaci_pole[22] = char_zima[2];                              // E
                        vysilaci_pole[23] = char_zima[3];                              // C

                      }

                    vysilaci_pole[24] = ' ';

                    vysilaci_pole[25] = '(';                    
                    vysilaci_pole[26] = dny[LOC_dvt][0];                               // P
                    vysilaci_pole[27] = dny[LOC_dvt][1];                               // o
                    vysilaci_pole[28] = ')';                    
                    vysilaci_pole[29] = ' ';                    
                    
                    odesli_pole(30);

                    vysilani();                                                        // dolepovana informace o poctu sekund od 1.1.1970
                    Serial2.print(tt);                                                 // pocet sekund od 1.1.1970
                    prijem();
                  }
                
                
                
                break;
              }



            case 6:                                                                    // nastaveni SEC
              {
                vysilaci_pole[2]  = kontrolni_bajt(2,vysilaci_pole);
                odesli_pole(3);
                leto = false;
                EEPROM_update(eeaddr_leto_zima,0);
                STM_DS(false);                                                         // serizeni casu v DS3231 podle casu v STM
                break;
              }



            case 7:                                                                    // nastaveni SELC
              {
                vysilaci_pole[2]  = kontrolni_bajt(2,vysilaci_pole);
                odesli_pole(3);
                leto = true;
                EEPROM_update(eeaddr_leto_zima,1);
                STM_DS(false);                                                         // serizeni casu v DS3231 podle casu v STM
                break;
              }

            
            case 8:                                                                    // vraceni informace o poctu prumerovani
              {
                vysilaci_pole[2] = prumery;
                vysilaci_pole[3] = kontrolni_bajt(3,vysilaci_pole);
                odesli_pole(4);
                break;
              }

            case 9:                                                                    // nastaveni poctu prumerovani
              {
                if (prijimaci_pole[3] > 0 and prijimaci_pole[3] < 21)
                  {
                    vysilaci_pole[2] = kontrolni_bajt(2,vysilaci_pole);
                    odesli_pole(3);
                    prumery = prijimaci_pole[3];
                    EEPROM_update(eeaddr_prumerovani,prumery);
                    MODdata[41] = prumery;                                             // pocet prumerovani
                  }
                else                                                                   // mimo rozsah 1 az 20
                  {
                    vysilaci_pole[1] = 255;                                            // kdyz je hodnota k zapisu mimo rozsah, misto cisla funkce se vrati cislo 255 a nic se neuklada
                    vysilaci_pole[2] = kontrolni_bajt(2,vysilaci_pole);
                    odesli_pole(3);
                  }
                break;
              }

            case 10:                                                                   // vraceni informace o automatickem spousteni
              {
                vysilaci_pole[2] = automat;
                vysilaci_pole[3] = kontrolni_bajt(3,vysilaci_pole);
                odesli_pole(4);
                break;
              }
          
            case 11:                                                                   // nastaveni automatickeho spousteni
              {
                vysilaci_pole[2] = kontrolni_bajt(2,vysilaci_pole);
                odesli_pole(3);
                automat = prijimaci_pole[3];
                MODdata[46] = automat;                                                 // interval automatickeho spousteni
                EEPROM_update(eeaddr_automat,automat);
                if (automat > 0)                                                       // pokud byl zadany nejaky casovy interval,
                  {
                    posledni_autospusteni = millis();                                  // spusti se prvni mereni okamzite
                    mereni(6);         
                  }
                break;
              }

            case 12:                                                                   // spustit mereni bez korekce
              {
                if (dokoncene_mereni == 2)                                             //dalsi mereni je mozne spustit az kdyz je predchozi dokoncene
                  {
                    vysilaci_pole[2] = 1;                                              // OK, zahajuji mereni
                    vysilaci_pole[3] = kontrolni_bajt(3,vysilaci_pole);
                    odesli_pole(4);
                    mereni(9);
                  }
                else                                                                   // jinak hlasi "SLAVE BUSSY" (adresa, 12, 0, CRC)
                  {
                    vysilaci_pole[2] = 0;                                              // BUSSY - predchozi mereni jeste neni dokoncene
                    vysilaci_pole[3] = kontrolni_bajt(3,vysilaci_pole);
                    odesli_pole(4);
                  }
                break;
              }


            case 13:                                                                   // index a adresa posledniho ulozeneho zaznamu v EEPROM
              {
                vysilaci_pole[2]  = ((index_posledniho_zaznamu >>  8) & 255);
                vysilaci_pole[3]  = ((index_posledniho_zaznamu)       & 255);
                
                
                vysilaci_pole[4]  = ((adr_posl_zaznamu >> 16) & 255);
                vysilaci_pole[5]  = ((adr_posl_zaznamu >>  8) & 255);
                vysilaci_pole[6]  = ((adr_posl_zaznamu)       & 255);
                vysilaci_pole[7]  = delka_zaznamu;
                vysilaci_pole[8]  = kontrolni_bajt(7,vysilaci_pole);
                odesli_pole(9);
                break;                            
              }

            case 14:                                                                   // prepnuti na jine cidlo na expanzni desce
              {
                if (prijimaci_pole[3] < pocet_cidel_svetla and prijimaci_pole[3] > 0)  // bylo pozadovanio prepnuti na konkretni cislo cidla a zadana hodnota byla ve spravnem rozsahu
                  {
                    prepni_cidlo_svetla(prijimaci_pole[3]);
                    if (EXPA_test() == true)  vysilaci_pole[2]  = index_cidla_svetla ; // expander je pripojeny
                    else                      vysilaci_pole[2]  = 255;                 // expander neni pripojeny - hlasi se chyba
                  }
                 else                                                                  // zadana hodnota mimo povoleny rozsah
                  {
                    switch(prijimaci_pole[3])
                      {
                        case 0:                                                        // cislo 0 slouzi k prepnuti na nasledujici cidlo a ma osetreno, ze pri prekroceni maxima se vraci na cidlo 1
                        case 8:                                                        // cislo 8 slouzi k prepnuti na nasledujici cidlo a ma osetreno, ze pri prekroceni maxima se vraci na cidlo 1
                          prepni_cidlo_svetla();
                          vysilaci_pole[2]  = index_cidla_svetla ;
                          break;                                   
  
                        case 9:                                                        // cislo 9 slouzi jen ke zjisteni aktualniho cisla cidla, ale nic neprepina
                          vysilaci_pole[2]  = index_cidla_svetla ;
                          break;                                   
  
                        default:                                                       // jakekoliv jine cislo je chyba
                          vysilaci_pole[2]  = 255 ;                                    // ERR: zadane cislo cidla mimo povoleny rozsah                                
                          break;
                      }
                  }                    

                if (EXPA_test() == false)     vysilaci_pole[2]  = 255;                 // expander neni pripojeny - hlasi se chyba

  
                vysilaci_pole[3]  = kontrolni_bajt(3,vysilaci_pole);
                odesli_pole(4);
                break;                  
              }
            
            case 15:                                                                   // vypis modbusovych registru v pripade ze modbus neni zapnuty
              {
                for (uint16_t modreg = 0 ; modreg < 32 ; modreg ++)                // smazani dat predchoziho zobrazeneho zaznamu
                  {
                    MODdata[modreg] = 0;
                  }

                zobraz_RTC(false);                                                     // aktualni datum a cas se zapise do modbusovych registru
                MODdata[32] = LOC_rok;                                                  
                MODdata[33] = LOC_mes;
                MODdata[34] = LOC_den;
                MODdata[35] = LOC_hod;
                MODdata[36] = LOC_min;
                MODdata[37] = LOC_sek;
                int8_t pomprom;
                if (leto == false)  pomprom = zimni_posun;
                else                pomprom = letni_posun;
                MODdata[38] = (LOC_dvt << 8) | pomprom;                                // v hornim bajtu je den v tydnu (1 az 7 pro Po az Ne), v dolnim bajtu je aktualni casova zona
                                                                                       //         (zaporna zona je ulozena jako binarni doplnek)

                MODdata[39] = index_posledniho_zaznamu;                                // [40040] prubezne aktualizovany index posledniho zaznamu

                MODdata[40] = 0;
                uint16_t MODzaznam = (adr_dalsiho_zaznamu - eeaddr_MIN_EEPROM) / delka_zaznamu;
                if (MODzaznam <= ((eeaddr_MAX_EEPROM - eeaddr_MIN_EEPROM - delka_zaznamu) / delka_zaznamu))   // zadane cislo je ve spravnem intervalu
                  {
                    uint32_t adresa = ((MODzaznam - 1) * delka_zaznamu) + eeaddr_MIN_EEPROM;             // z nej se vypocte adresa zaznamu v EEPROM
                    for (uint8_t i = 0 ; i < delka_zaznamu ; i++)
                      {
                        pole_EEPROM[i] = EEPROM_read(adresa + i);
                      }
  
                    EEPROM_to_MOD(MODzaznam);                                          // prevede pole EEPROM do modbusoveho pole (prida jeste informaci o indexu zvoleneho zaznamu)
                  }
                
                MODdata[41] = prumery;                                                 // pocet prumerovani
                MODdata[42] = zimni_posun;                                             // zimni posun casu 
                MODdata[43] = letni_posun;                                             // letni posun casu 
                MODdata[44] = rozhod_stab;                                             // rozhodovaci uroven  pro stabilni jas
                MODdata[45] = hranice_ukladani;                                        // hranice jasu pro ukladani do EEPROM
                MODdata[46] = automat;                                                 // interval automatickeho spousteni
                
                MODdata[47] = 0xFFFF;                                                  // rezerva
                MODdata[48] = 0xFFFF;                                                  // rezerva
                MODdata[49] = 0xFFFF;                                                  // rezerva
                MODdata[50] = 0xFFFF;                                                  // rezerva
                
                MODdata[53] = MODdata[32];                                             // aktualni datum a cas se jeste zkopiruje do registru pro nastaveni datumu a casu
                MODdata[54] = MODdata[33];
                MODdata[55] = MODdata[34];
                MODdata[56] = MODdata[35];
                MODdata[57] = MODdata[36];
                MODdata[58] = MODdata[37];
                
                MODdata[59] = 0;                                                       // smaze posledni prikaz
                
                for (uint16_t modreg = 30001 ; modreg < 30060 ; modreg ++)         // vypis modbusovych registru v textovem formatu
                  {
                    Serial2.print('[');
                    Serial2.print(modreg);                                             // cislo registru
                    Serial2.print("] = ");
                    if (MODdata[modreg - 30001] < 10000) Serial2.print('0');           // obsah registru doplneny o uvodni nuly
                    if (MODdata[modreg - 30001] <  1000) Serial2.print('0');
                    if (MODdata[modreg - 30001] <   100) Serial2.print('0');
                    if (MODdata[modreg - 30001] <    10) Serial2.print('0');
                    Serial2.println(MODdata[modreg - 30001]);
                  }
                break;                  
              }          

            
            case 16:                                                                   // vypis verze programu a unikatni identifikace
              {
                Serial2.print("SW ver.: ");                                            //  verze software
                Serial2.println(verzeSW);                                              //  datum nebo jiny text nadefinovany na zacatku programu)
                
                Serial2.print(lng129);                                                 // "Unikatni identifikace: "
                Serial2.print(" 0x");
                Serial2.println(ID_char);
                break;                  
              }          
          
          }
      }
    else
      {
        if (pouzij_CRC == 2) 
          {
            Serial.println(lng169);                                                    //    Serial.println("Kontrolni bajt nesouhlasi:");
            Serial.print(lng170);                                                      //    Serial.print("   ocekavane CRC: ");
            Serial.println( CRCneg , HEX);
            Serial.print(lng171);                                                      //    Serial.print("   prijate CRC: ");
            Serial.println( prijimaci_pole[13] , HEX);
          }
      }
   delay(20);
   prijem();
  
  }
//----------------------------------------------



//----------------------------------------------
// jednoduchy jednobajtovy kontrolni soucet z nekolika prvku zadaneho pole (na zaver prevedeny na zaporny bitovy doplnek)
uint8_t kontrolni_bajt(uint8_t pocet_prvku,uint8_t kontrolni_pole[])
  {
    uint8_t soucet = 0;
    for (uint8_t i = 0; i < pocet_prvku; i++)
      {
        soucet = soucet - kontrolni_pole[i];
      }
    return soucet;
  }
//----------------------------------------------



//----------------------------------------------
// odeslani pole dat pres RS485
void odesli_pole(uint8_t pocet)
  {   
    if (prijimaci_pole[1] == 127)
      {
         switch(prijimaci_pole[2])
           {
             case 2:
             case 3:
             case 5:
             case 8:
             case 10:
             case 13:
               {
                 Serial.println(lng172);                                               //    Serial.println("Pro tuto funkci nelze pouzit univerzalni adresu 127.");
                 break;
               }
             default:
               {
                 Serial.println(lng173);                                               //    Serial.println("univerzalni pozadavek vykonan");
                 break;
               }
           }        
      }
    else
      {
        for (uint8_t i = 0; i < pocet ; i ++)                                             // pole se odesila bajt po bajtu
          {
            Serial2.write(vysilaci_pole[i]);
            delay(2);
          }
      }
   if (bitRead(signal_LED,3) == true)                                  // je povoleno bliknuti LED pri komunikaci
     {
       blik_F_kratky();                                                // data byla odeslana, blikne se fialove
     }

    delay(20);
    prijem();
  }
//----------------------------------------------


//----------------------------------------------
// komunikace pres modbus
void test_485_mod(void)
  {
    zobraz_RTC(false);
    MODdata[32] = LOC_rok;                                                     // aktualni datum a cas se zapise do modbusovych registru
    MODdata[33] = LOC_mes;
    MODdata[34] = LOC_den;
    MODdata[35] = LOC_hod;
    MODdata[36] = LOC_min;
    MODdata[37] = LOC_sek;
    int8_t pomprom;
    if (leto == false)  pomprom = zimni_posun;
    else                pomprom = letni_posun;
    MODdata[38] = (LOC_dvt << 8) | pomprom;                           // v hornim bajtu je den v tydnu (1 az 7 pro Po az Ne), v dolnim bajtu je aktualni casova zona
                                                                      //         (zaporna zona je ulozena jako binarni doplnek)
    MODdata[39] = index_posledniho_zaznamu;                           // aktualizace indexu posledniho zaznamu v registru 40040
                                                                      
    char prvni_znak = Serial2.peek();                                 // test existence SQM na pozadovanem portu 
    if (prvni_znak == '*')
      {
        Serial2.read();
        prvni_znak = Serial2.read();                                  // ve skutecnosti druhy znak by mel byt 'a'
        if (prvni_znak == 'a')
          {
            uint8_t pozadovana_adresa = Serial2.parseInt(); 
            if (pozadovana_adresa == slave_addr)                      // pozadovana adresa odpovida
              {
                vysilani();
                Serial2.print('3');                                   // vrati se znacka komunikace RS485 (modbus)
                prijem();
              }
          }
      }
   
 
    stav_MOD = MBslave.poll(MODdata,110);
    
    if (stav_MOD > 0)                                                 // po lince 485 prisla nejaka modbusova zadost, CRC ani adresa ale nemusi souhlasit
      {
        if (bitRead(signal_LED,3) == true)      blik_C_kratky();      // je povoleno bliknuti LED pri komunikaci, blikne svetle modra LED (prijem)    
      }



                                                           
    if (stav_MOD > 4 and stav_MOD < 255)                              // po lince 485 prisla nejaka modbusova zadost, CRC souhlasi, adresa souhlasi
      {
        if (bitRead(signal_LED,3) == true)      blik_F_kratky();      // je povoleno bliknuti LED pri komunikaci, blikne fialova LED (vysilani)
        posledni_funkce = MBslave.getLastFunction();                  // hlavne kvuli zjisteni, ze doslo k pozadavku o zapis do registru, aby se nemusel testovat zapis pri kazdem obycejnem cteni

        
      
        if (posledni_funkce == 0x04 or posledni_funkce == 0x03) posledni_funkce = 0;      // cteni dat z registru 30001 az 30110 nebo 40001 az 40110 se provadi automaticky v knihovne - neni treba resit

        if (posledni_funkce == 0x06 or posledni_funkce == 0x10)       // zapis do jednoho nebo vice registru
          {

            if (MODdata[60] > 0)                                                       // pro rychle vycteni dat z EEPROM do Modbusu bez nutnosti spoustet prikaz 2 v adrese 40060
              {
                uint16_t MODzaznam = MODdata[60];                                      // index pozadovaneho zaznamu by mel byt v registru 30061 (40061)

                if (MODzaznam <= ((eeaddr_MAX_EEPROM - eeaddr_MIN_EEPROM - delka_zaznamu) / delka_zaznamu))   // zadane cislo je ve spravnem intervalu
                  {
                    uint32_t adresa = ((MODzaznam - 1) * delka_zaznamu) + eeaddr_MIN_EEPROM;                  // z nej se vypocte adresa zaznamu v EEPROM
    
                    for (uint8_t i = 0 ; i < delka_zaznamu ; i++)
                      {
                        pole_EEPROM[i] = EEPROM_read(adresa + i);
                      }
                    EEPROM_to_MOD(MODzaznam);                              // prevede pole EEPROM do modbusoveho pole (prida jeste informaci o indexu zvoleneho zaznamu)
                    MODdata[40] = 0;                                       // smaze index pozadovaneho zaznamu (tim je zajisteno, ze pri pozadavku o posledni zaznamy staci jen spoustet prikaz "2" bez nastavovani posledniho indexu)
                    MODdata[59] = 0;                                       // smaze posledni prikaz (2)
                  }
                else                                                       // pozadovany zaznam je mimo povoleny rozsah
                  {
                    EEPROM_to_MOD((uint16_t)0xFFFE);                       // nastavi bunky ve zvolenem zaznamu na nesmyslne hodnoty 0xFFFF
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (pozadovany zaznam mimo povoleny rozsah)
                  }
                
                MODdata[60] = 0;                                                       // po aktualizaci zaznamenanych dat se index zaznamu z 40061 maze
              }

            if (MODdata[59] == 1)                                                      // prikaz pro nastaveni datumu a casu
              {
                bool INPUTerr = false;                                                 // zakladni kontrola spravne zadanych casovych udaju
                if (MODdata[53] > 2070 or MODdata[53] < 1970 ) INPUTerr = true;        // test zadaneho roku
                if (MODdata[54] >   12 or MODdata[54] <    1 ) INPUTerr = true;        // test zadaneho mesice
                if (MODdata[55] >   31 or MODdata[55] <    1 ) INPUTerr = true;        // test zadaneho dne
                if (MODdata[56] >   23)                        INPUTerr = true;        // test zadane hodiny
                if (MODdata[57] >   59)                        INPUTerr = true;        // test zadane minuty
                if (MODdata[58] >   59)                        INPUTerr = true;        // test zadane sekundy
                
                if (INPUTerr == false)
                  {
                    LOC_rok = MODdata[53];
                    LOC_mes = MODdata[54];
                    LOC_den = MODdata[55];
                    LOC_hod = MODdata[56];
                    LOC_min = MODdata[57];
                    LOC_sek = MODdata[58];
                    
                    //--------------- zapis casu do RTC vcetne pripadnych korekci do EEPROM ---------
                    uint32_t aktualni_cas_RTC = rtclock.getY2kEpoch();
            
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
                    uint32_t interval;
                    if (tt > cas_minuleho_nastaveni)    interval = tt - cas_minuleho_nastaveni;     // interval musi byt vzdycky kladny
                    else                                interval = cas_minuleho_nastaveni - tt;
            
                    long odchylka = aktualni_cas_RTC - tt;
            
                    if (interval < 86400UL)                                                         // od predchoziho serizeni ubehlo min nez 24 hodin
                      {
                        if (odchylka > 172800L or odchylka < -172800L )                             // rozdil mezi zadavanym casem a casem v RTC je vic, nez 2 dny. To je CHYBA
                          {
                            korekce = 0x7FFFFFFFUL;
                          }
                        else                                                                        // "prijatelna odchylka, korekce se ale nepocita - prebira se puvodni"
                          {
                            korekce = EEPROM_read_long(eeaddr_RTC_korekce);
                          }
                      
                      }
                    
                    if (interval > 15768000UL)                                                      // od predchoziho serizeni ubehlo vic nez pul roku
                      {
                        korekce = 0x7FFFFFFFUL;                                                     // korekce se rusi
                      }
                
                    if (interval <= 15768000UL and interval >=86400UL)                              // od predchoziho serizeni ubehlo vic nez den, ale min nez pul roku
                      {                                                                             //  "spravny interval od posledniho serizeni"
                        if (odchylka > 172800L or odchylka < -172800L )                             // rozdil mezi zadavanym casem a casem v RTC je vic, nez 2 dny. To je CHYBA
                          {
                            korekce = 0x7FFFFFFFUL;
                          }
                        else                                                                        // "prijatelna odchylka, pocita se korekce"
                          {
                            
                            if (odchylka  < 0) korekce = interval / -odchylka;                      // odchylka je zaporna, sekundy se budou pricitat -  (odchylka se prevadi na kladne cislo)
                            if (odchylka  > 0) korekce = (0x80000000UL | (interval / odchylka));    // nejvyssi bit je znamenko, korekce se bude odecitat
                            if (odchylka == 0) korekce = 0x7FFFFFFFUL;                              // cas v RTC je stejny, jako zadany cas - zadna korekce neni potreba (prvni extra sekunda za 68 let) 
                
                            if ((korekce & 0x7FFFFFFFUL) < 300)                                     // sekunda se ma pricitat, nebo odecitat driv, nez za 5 minut - to vypada na chybu RTC
                              {
                                korekce = 0x7FFFFFFFUL;
                              }
                          
                          }      
                      }        
            
                    cas_minuleho_nastaveni = tt;                                                    // nastaveni globalni promenne, aby se nemuselo pri kazdem zjistovani casu sahat do EEPROM
                    EEPROM_write_long(eeaddr_RTC_set,tt);                                           // do EEPROM se v kazdem pripade ulozi cas posledniho nastaveni
                
                
                    if (korekce != EEPROM_read_long(eeaddr_RTC_korekce))                            // do EEPROM se nova korekce uklada jen v pripade, ze se zmenila
                      {
                        EEPROM_write_long(eeaddr_RTC_korekce , korekce);
                      }
                    
                    // ---------------------------
                    MODdata[59] = 0;                                                   // v pripade uspechu se vymaze posledni prikaz (1) pro nastaveni casu
                  }
                else
                  {
                    MODdata[59] = 0xFFFF;                                              // nejaka chyba (zadane parametry casu mimo povoleny rozsah)
                  }
              }


            
            if (MODdata[59] == 2)                                          // prikaz pro nastaveni starsiho zaznamu
              {
                uint16_t MODzaznam = MODdata[40];                          // index pozadovaneho zaznamu by mel byt v registru 30041 (40041)

                if (MODzaznam == 0)                                        // cislo 0 znamena posledni zaznam
                  {
                    MODzaznam = (adr_dalsiho_zaznamu - eeaddr_MIN_EEPROM) / delka_zaznamu;
                  }
               
                if (MODzaznam <= ((eeaddr_MAX_EEPROM - eeaddr_MIN_EEPROM - delka_zaznamu) / delka_zaznamu))   // zadane cislo je ve spravnem intervalu
                  {
                    uint32_t adresa = ((MODzaznam - 1) * delka_zaznamu) + eeaddr_MIN_EEPROM;             // z nej se vypocte adresa zaznamu v EEPROM
    
                    for (uint8_t i = 0 ; i < delka_zaznamu ; i++)
                      {
                        pole_EEPROM[i] = EEPROM_read(adresa + i);
                      }
                    EEPROM_to_MOD(MODzaznam);                              // prevede pole EEPROM do modbusoveho pole (prida jeste informaci o indexu zvoleneho zaznamu)
                    MODdata[40] = 0;                                       // smaze index pozadovaneho zaznamu (tim je zajisteno, ze pri pozadavku o posledni zaznamy staci jen spoustet prikaz "2" bez nastavovani posledniho indexu)
                    MODdata[59] = 0;                                       // smaze posledni prikaz (2)
                  }
                else                                                       // pozadovany zaznam je mimo povoleny rozsah
                  {
                    EEPROM_to_MOD((uint16_t)0xFFFE);                       // nastavi bunky ve zvolenem zaznamu na nesmyslne hodnoty 0xFFFF
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (pozadovany zaznam mimo povoleny rozsah)
                  }
              }



            if (MODdata[59] == 3)                                          // prikaz pro nastaveni poctu prumerovani
              {
                if (MODdata[41] > 0 and MODdata[41] < 21)                  // zadane cislo musi byt v rozsahu 1 az 20
                  {
                    EEPROM_update(eeaddr_prumerovani,(uint8_t)MODdata[41]);
                    prumery = (uint8_t)MODdata[41];              
                    MODdata[59] = 0;                                       // smaze posledni prikaz (3)
                  }
                else
                  {
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (mimo povoleny rozsah)
                    MODdata[41] = prumery;                                 // nastaveni registru na puvodni hodnotu
                  }
              }



            if (MODdata[59] == 4)                                          // prikaz pro nastaveni posunu zimni casove zony (pro SEC to je +1)
              {
                int16_t pomprom = (int16_t)MODdata[42];                    // pri zadani zaporneho cisla pres modbus bude 'pomprom' take zaporne
                if (pomprom > -13 and pomprom < 13)                        // zadane cislo musi byt v rozsahu -12 az +12   (zaporne cislo se zadava jako 16-bitovy binarni doplnek)
                  {                                                        //        (priklad: -3 = 0xFFFD)
                    EEPROM_update(eeaddr_zima_hod,pomprom + 100);          // do EEPROM se uklada vzdycky kladne cislo
                    zimni_posun = pomprom;                                 // promenna ale muze byt i zaporna
                    MODdata[59] = 0;                                       // smaze posledni prikaz (4)
                    STM_DS(false);                                         // serizeni casu v DS3231 podle casu v STM
                  }
                else
                  {
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (mimo povoleny rozsah)
                    MODdata[42] = zimni_posun;                             // nastaveni registru na puvodni hodnotu
                  }
              }



            if (MODdata[59] == 5)                                          // prikaz pro nastaveni posunu letni casove zony (pro SELC to je +2)
              {
                int16_t pomprom = (int16_t)MODdata[43];                    // pri zadani zaporneho cisla pres modbus bude 'pomprom' take zaporne
                if (pomprom > -13 and pomprom < 13)                        // zadane cislo musi byt v rozsahu -12 az +12   (zaporne cislo se zadava jako 16-bitovy binarni doplnek)
                  {                                                        //        (priklad: -3 = 0xFFFD)
                    EEPROM_update(eeaddr_leto_hod,pomprom + 100);          // do EEPROM se uklada vzdycky kladne cislo
                    letni_posun = pomprom;                                 // promenna ale muze byt i zaporna
                    MODdata[59] = 0;                                       // smaze posledni prikaz (5)
                    STM_DS(false);                                         // serizeni casu v DS3231 podle casu v STM
                  }
                else
                  {
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (mimo povoleny rozsah)
                    MODdata[43] = letni_posun;                             // nastaveni registru na puvodni hodnotu
                  }
              }



            if (MODdata[59] == 6)                                          // prikaz pro nastaveni rozhodovaci urovne pro urceni stability
              {
                if (MODdata[44] < 256)                                     // maximum, ktere se da zpracovat, je 25,5%
                  {
                    EEPROM_update(eeaddr_stab,(uint8_t)MODdata[44]);       // ulozeni do EEPROM
                    rozhod_stab = (uint8_t)MODdata[44];
                    MODdata[59] = 0;                                       // smaze posledni prikaz (6)
                  }
                else
                  {
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (mimo povoleny rozsah)
                    MODdata[44] = rozhod_stab;                             // nastaveni registru na puvodni hodnotu
                  }
              }



            if (MODdata[59] == 7)                                          // prikaz pro nastaveni hranice jasu pro ukladani do EEPROM
              {
                if (MODdata[45] < 256)                                     // maximum, ktere se da zpracovat, je 25,5 mag/arcsec2
                  {
                    EEPROM_update(eeaddr_uroven_EEPROM,(uint8_t)MODdata[45]);         // ulozeni do EEPROM
                    hranice_ukladani = (uint8_t)MODdata[45];
                    MODdata[59] = 0;                                       // smaze posledni prikaz (7)
                  }
                else
                  {
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (mimo povoleny rozsah)
                    MODdata[45] = hranice_ukladani;                        // nastaveni registru na puvodni hodnotu
                  }
              }



            if (MODdata[59] == 8)                                          // prikaz pro nastaveni intervalu automatickeho mereni
              {
                if (MODdata[46] < 256)                                     // maximum, ktere se da zpracovat, je 255 sekund
                  {
                    EEPROM_update(eeaddr_automat,(uint8_t)MODdata[46]);    // ulozeni do EEPROM
                    automat = (uint8_t)MODdata[46];
                    MODdata[59] = 0;                                       // smaze posledni prikaz (8)
                    posledni_autospusteni = millis();                      // prvni mereni se spusti okamzite
                    mereni(6);                                             //  automaticke mereni
                  }
                else
                  {
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (mimo povoleny rozsah)
                    MODdata[46] = automat;                                 // nastaveni registru na puvodni hodnotu
                  }
              }



            if (MODdata[59] == 9)                                          // prikaz pro spusteni mereni (bez parametru)
              {
                mereni(5);                                                 // jednorazove spusteni pres seriovou linku (v zaznamu se ulozi znacka ze byla pouzita linka RS485)                
                MODdata[59] = 0;                                           // smaze posledni prikaz (9)
              }
               


            if (MODdata[59] == 10)                                         // prikaz pro prepnuti na "zimni" cas (bez parametru)
              {
                leto = false;                                              // znacka pro zimu
                EEPROM_update(eeaddr_leto_zima,leto);                      // ulozeni znacky do EEPROM
                MODdata[59] = 0;                                           // smaze posledni prikaz (10)
                STM_DS(false);                                             // serizeni casu v DS3231 podle casu v STM
              }


            if (MODdata[59] == 11)                                         // prikaz pro prepnuti na letni cas (bez parametru)
              {
                leto = true;                                               // znacka pro leto
                EEPROM_update(eeaddr_leto_zima,leto);                      // ulozeni znacky do EEPROM
                MODdata[59] = 0;                                           // smaze posledni prikaz (11)
                STM_DS(false);                                             // serizeni casu v DS3231 podle casu v STM
              }



            if (MODdata[59] == 12)                                         // prikaz pro prime cteni hodnoty z EEPROM (adresa 0 az 699)
              {
                if (MODdata[51] < eeaddr_MIN_EEPROM)                       // Maximalni povolena adresa je 699
                  {
                    MODdata[52] = EEPROM_read(MODdata[51]);                // cteni jednoho bajtu z EEPROM
                    MODdata[59] = 0;                                       // smaze posledni prikaz (12)
                  }
                else
                  {
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (mimo povoleny rozsah)
                  }                
              }



            if (MODdata[59] == 13)                                         // prikaz pro primy zapis hodnoty do EEPROM na adresu 0 az 699
              {
                if (MODdata[51] < eeaddr_MIN_EEPROM and MODdata[52] < 256) // Maximalni povolena adresa je 699, maximalni povolena data jsou 255
                  {
                    EEPROM_update(MODdata[51],(uint8_t)MODdata[52]);       // zapis jednoho bajtu do EEPROM
                    MODdata[59] = 0;                                       // smaze posledni prikaz (13)
                  }
                else
                  {
                    MODdata[59] = 0xFFFF;                                  // nejaka chyba (mimo povoleny rozsah)
                  }
              }


            if (MODdata[59] == 14)                                         // prikaz pro obnoveni registru 30040 az 30051
              {

                MODdata[39] = index_posledniho_zaznamu;                    // index posledniho zaznamu

                MODdata[40] = 0;                                           // vynulovani pripadne zadosti o nejaky starsi zaznam (0 = posledni zaznam)

                MODdata[41] = prumery;                                     // pocet prumerovani
                MODdata[42] = zimni_posun;                                 // zimni posun casu 
                MODdata[43] = letni_posun;                                 // letni posun casu 
                MODdata[44] = rozhod_stab;                                 // rozhodovaci uroven  pro stabilni jas
                MODdata[45] = hranice_ukladani;                            // hranice jasu pro ukladani do EEPROM
                MODdata[46] = automat;                                     // interval automatickeho spousteni
            
                MODdata[47] = 0xFFFF;                                      // rezerva
                MODdata[48] = 0xFFFF;                                      // rezerva
                MODdata[49] = 0xFFFF;                                      // rezerva
                MODdata[50] = 0xFFFF;                                      // rezerva

                MODdata[53] = MODdata[32];                                 // aktualni datum a cas se jeste zkopiruje do registru pro nastaveni datumu a casu
                MODdata[54] = MODdata[33];
                MODdata[55] = MODdata[34];
                MODdata[56] = MODdata[35];
                MODdata[57] = MODdata[36];
                MODdata[58] = MODdata[37];

                MODdata[59] = 0;                                           // smaze posledni prikaz (14)
              }

          }                 


// #doc#23        
        // Volitelna sada informaci, ktera se prenasi pres modbus v registrech od 30071 do 30110
        // Tyto informace jsou pouze pro cteni (pri kazde zadosti se prepisuji) a je mozne je stahnout jedinou zadosti.
        // Moznost omezit stahovana data a moznost doplneni vlastnich informaci (napriklad unikatni ID procesoru, nebo nejaka globalni promenna).
        // Kazdy registr je 16-bitovy. 
        // Priklady:
        MODdata[70]  = ID1 >> 16;                       // registr 30071 : unikatni ID procesoru v 6 16-bitovych registrech
        MODdata[71]  = ID1 & 0xFFFF;                    // registr 30072
        MODdata[72]  = ID2 >> 16;                       // registr 30073
        MODdata[73]  = ID2 & 0xFFFF;                    // registr 30074
        MODdata[74]  = ID3 >> 16;                       // registr 30075
        MODdata[75]  = ID3 & 0xFFFF;                    // registr 30076
        
        MODdata[76]  = MODdata[1];                      // registr 30077 : typ zvoleneho zaznamu
        MODdata[77]  = MODdata[2];                      // registr 30078 : sek 1970 MSW pro zvoleny zaznam
        MODdata[78]  = MODdata[3];                      // registr 30079 : sek 1970 LSW pro zvoleny zaznam
        MODdata[79]  = MODdata[10];                     // registr 30080 : svetlo v mmag/arcsec2 pro zvoleny zaznam
        MODdata[80]  = MODdata[14];                     // registr 30081 : teplota pro zvoleny zaznam
        
        MODdata[81]  = MODdata[37];                     // registr 30082 : aktualni sekundy v SQM
        MODdata[82]  = MODdata[38];                     // registr 30083 : aktualni den v tydnu a posun casove zony v SQM
        MODdata[83]  = MODdata[46];                     // registr 30084 : aktualni interval spousteni mereni v SQM
        MODdata[84]  = MODdata[41];                     // registr 30085 : aktualni pocet prumerovani v SQM

        uint32_t aktualni_millis = millis();
        MODdata[85]  = aktualni_millis >> 16;           // registr 30086 : jen priklad odesilani MSW z hodnoty millis()
        MODdata[86]  = aktualni_millis & 0xFFFF;        // registr 30087 : jen priklad odesilani LSW z hodnoty millis()
               
        MODdata[87]  = analogRead(pin_AD_BAT);          // registr 30088 : analogova hodnota na pinu pro test napajeciho napeti

        MODdata[88]  = SD_volno;                        // registr 30089 : posledni zjisteny volny prostor v [MB] na SD karte

        MODdata[89]  = EEPROM_read_int(474);            // registr 30090 : priklad cteni EEPROM (BEEP bity)
        
        MODdata[90]  = 30091;                           // registr 30091 : testovaci hodnota        
        // ....
        MODdata[108] = 30109;                           // registr 30109 : testovaci hodnota 
        MODdata[109] = 30110;                           // registr 30110 : testovaci hodnota - nejvyssi mozny registr
        // -----------------------------------------------        
         

      }
    
  }

// zaznam (aktualni, nebo pozadovany), ktery je ulozeny v "pole_EEPROM[]" se prekonvertuje do pole "MODdata[]"
void EEPROM_to_MOD(void)                              // pri spusteni bez parametru se prevadi posledni zmereny zaznam (neni treba zjistovat jeho adresu)
  { 
    EEPROM_to_MOD((uint16_t)0xFFFF);
  }


void EEPROM_to_MOD(uint16_t index_zaznamu)
  {    

    if (index_zaznamu == 0xFFFE)                      // nejaka chyba (cislo zaznamu mimo rozsah) - nastaveni vsech registru na nesmyslnou hodnotu
      {
        for (uint8_t i = 0; i<31 ; i++)
          {
            MODdata[i] = 0xFFFF;
          }  
      }
    else
      {
        uint32_t cas_EEPROM = (pole_EEPROM[1] << 24) + (pole_EEPROM[2] << 16) + (pole_EEPROM[3] << 8) + pole_EEPROM[4];
    
        if (index_zaznamu == 0xFFFF)   MODdata[0]  = (adr_dalsiho_zaznamu - eeaddr_MIN_EEPROM) / delka_zaznamu;   // 30001 index zaznamu (poloha v EEPROM) - NENI TO ADRESA V EEPROM!
        else                           MODdata[0]  = index_zaznamu;
        
        breakTimeFCE(cas_EEPROM, mtt_EEPROM);                                         // z poctu sekund od 1.1.1970 se ziska citelny datum a cas
        
        MODdata[1]  =  (pole_EEPROM[0] << 8) + pole_EEPROM[7];                        // 30002 typ zaznamu (svetlo/stopky/TS...) a specialni znacky k zaznamu (SEC/SELC/stabilita/adresa ...)
    
    
        MODdata[2]  = (pole_EEPROM[1] << 8) + pole_EEPROM[2];                         // 30003 pocet sekund MSW
        MODdata[3]  = (pole_EEPROM[3] << 8) + pole_EEPROM[4];                         // 30004 pocet sekund LSW
    
        MODdata[4]  = mtt_EEPROM.year+1970;                                           // 30005 rok zaznamu v plnem formatu "yyyy"
        MODdata[5]  = mtt_EEPROM.month;                                               // 30006 mesic zaznamu
        MODdata[6]  = mtt_EEPROM.day;                                                 // 30007 den zaznamu
        MODdata[7]  = mtt_EEPROM.hour;                                                // 30008 hodina zaznamu
        MODdata[8]  = mtt_EEPROM.minute;                                              // 30009 minuta zaznamu
        MODdata[9]  = mtt_EEPROM.second;                                              // 30010 sekunda zaznamu
    
        MODdata[10] = (pole_EEPROM[5] << 8) + pole_EEPROM[6];                         // 30011 svetlo v tisicinach mag/arcsec2
    
        uint8_t indexpole = 8;
    
    
        #ifdef ukladat_infra
        MODdata[11] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30012 infra slozka svetla
        indexpole = indexpole + 2;
        #else
        MODdata[11] = 0xFFFF;
        #endif
    
    
        #ifdef ukladat_full       
        MODdata[12] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30013 full slozka svetla
        indexpole = indexpole + 2;
        #else
        MODdata[12] = 0xFFFF;
        #endif
    
    
        #ifdef ukladat_contr_reg
        MODdata[13] = pole_EEPROM[indexpole];                                         // 30014 konfiguracni registr cidla svetla
        indexpole = indexpole + 1;
        #else
        MODdata[13] = 0xFFFF;
        #endif
    
    
        #ifdef ukladat_teplotu
        MODdata[14] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30015 teplota v setinach 'C posunute o +50'C (vzdycky kladne cialo) 
        indexpole = indexpole + 2;
        #else
        MODdata[14] = 0xFFFF;
        #endif
    
    
        #ifdef ukladat_vlhkost    
        MODdata[15] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30016 vlhkost v setinach %
        indexpole = indexpole + 2;
        #else
        MODdata[15] = 0xFFFF;
        #endif
    
    
        #ifdef ukladat_tlak       
        MODdata[16] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30017 absolutni tlak (neprepocitany na hladinu more) snizeny o 60000Pa 
        indexpole = indexpole + 2;
        #else
        MODdata[16] = 0xFFFF;
        #endif
    

        #ifdef ukladat_GPS
        MODdata[17] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30018 zemepisna delka MSW (v miliontinach stupne. Pro vychod se pricita +180 000 000)
        indexpole = indexpole + 2;
        MODdata[18] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30019 zemepisna delka LSW
        indexpole = indexpole + 2;
        MODdata[19] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30020 zemepisna sirka MSW (v miliontinach stupne. Pro sever se pricita +90 000 000)
        indexpole = indexpole + 2;
        MODdata[20] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30021 zemepisna sirka LSW
        indexpole = indexpole + 2;
        MODdata[21] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30022 nadmorska vyska     (v metrech, vzdycky zvysena o +500m)
        indexpole = indexpole + 2;
        #else
        MODdata[16] = 0xFFFF;
        MODdata[17] = 0xFFFF;
        MODdata[18] = 0xFFFF;
        MODdata[19] = 0xFFFF;
        MODdata[20] = 0xFFFF;
        MODdata[21] = 0xFFFF;
        #endif    

    
        #ifdef ukladat_naklon       
        MODdata[22] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30023 naklon v desetinach stupne zvyseny o 90 stupnu
        indexpole = indexpole + 2;
        #else
        MODdata[22] = 0xFFFF;
        #endif
    
    
        #ifdef ukladat_azimut 
        MODdata[23] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30024 azimut ve stupnich
        indexpole = indexpole + 2;
        #else
        MODdata[23] = 0xFFFF;
        #endif
    
    
        #ifdef ukladat_astro       
        MODdata[24] = pole_EEPROM[indexpole];                                         // 30025 elevace Slunce ve stupnich, zvetsena o 90 stupnu
        MODdata[25] = pole_EEPROM[indexpole+1];                                       // 30026 elevace Mesice ve stupnich, zvetsena o 90 stupnu 
        MODdata[26] = pole_EEPROM[indexpole+2];                                       // 30027 osvetleni Mesice v procentech
        indexpole = indexpole + 3;
        #else
        MODdata[24] = 0xFFFF;
        MODdata[25] = 0xFFFF;
        MODdata[26] = 0xFFFF;
        #endif

        #ifdef ukladat_rezerva_1 
        MODdata[27] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30028 rezerva
        indexpole = indexpole + 2;
        #else
        MODdata[27] = 0xFFFF;
        #endif

        #ifdef ukladat_rezerva_2 
        MODdata[28] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30029 rezerva
        indexpole = indexpole + 2;
        #else
        MODdata[28] = 0xFFFF;
        #endif

        #ifdef ukladat_rezerva_3 
        MODdata[29] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30030 rezerva
        indexpole = indexpole + 2;
        #else
        MODdata[29] = 0xFFFF;
        #endif

        #ifdef ukladat_rezerva_4 
        MODdata[30] = (pole_EEPROM[indexpole] << 8) +  pole_EEPROM[indexpole+1];      // 30031 rezerva
        indexpole = indexpole + 2;
        #else
        MODdata[30] = 0xFFFF;
        #endif      
      }
  }


void AUTOdata_RS485(void)
  {
    bool stab_mark;
    if (AUTOkom_posledni == 0) return;                        // zatim jeste neprobehlo zadne mereni svetla od zapnuti napajeni 

    vysilani();                                               // prepne link RS485 na vysilani
                                                              
                                                              //      (nebere se posledni ulozeny zaznam, protoze by to mohly byt treba stopky nebo TS)
    uint32_t AUTOadresa = AUTOkom_posledni;    
    
    if (AUTOkom_format == 0)                                 // pole dat
      {

        if ((AUTOkom_obsah & 0b00001111) == 15)              // odeslat uplne cely zaznam
          {
            for (uint8_t i = 0; i< delka_zaznamu ; i++)
              {
                Serial2.write(EEPROM_read(AUTOadresa + i));
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
                Serial2.write(AUTOkom_export_pole[i]);
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
                    Serial2.write(AUTOkom_export_pole[11]);
                    Serial2.write(AUTOkom_export_pole[12]);
                  }
                #endif
                #ifdef ukladat_vlhkost
                  {
                    Serial2.write(AUTOkom_export_pole[13]);
                    Serial2.write(AUTOkom_export_pole[14]);
                  }
                #endif
                #ifdef ukladat_tlak
                  {
                    Serial2.write(AUTOkom_export_pole[15]);
                    Serial2.write(AUTOkom_export_pole[16]);
                  }
                #endif
    
              }
    
            if ((AUTOkom_obsah & 0b00000010) == 2)           // pridat jeste detailni info o svetle
              {
                #ifdef ukladat_infra
                  {
                    Serial2.write(AUTOkom_export_pole[6]);
                    Serial2.write(AUTOkom_export_pole[7]);
                  }
                #endif
                
                #ifdef ukladat_full
                  {
                    Serial2.write(AUTOkom_export_pole[8]);
                    Serial2.write(AUTOkom_export_pole[9]);
                  }
                #endif    

                #ifdef ukladat_contr_reg
                  {
                    Serial2.write(AUTOkom_export_pole[10]);
                  }
                #endif
                Serial2.write(stab_mark);                     // jeden bajt znacky stability 0 / 1
              }
    
    
            if ((AUTOkom_obsah & 0b00000100) == 4)           // pridat jeste info o naklonu a azimutu
              {
                #ifdef ukladat_naklon
                  {
                    Serial2.write(AUTOkom_export_pole[17]);
                    Serial2.write(AUTOkom_export_pole[18]);
                  }
                #endif
                
                #ifdef ukladat_azimut
                  {
                    Serial2.write(AUTOkom_export_pole[19]);
                    Serial2.write(AUTOkom_export_pole[20]);
                  }          
                #endif
              }
    
            if ((AUTOkom_obsah & 0b00001000) == 8)           // pridat jeste info o Slunci a Mesici
              {
                #ifdef ukladat_astro
                  {
                    Serial2.write(AUTOkom_export_pole[21]);    // elevace Slunce
                    Serial2.write(AUTOkom_export_pole[22]);    // elevace Mesice
                    Serial2.write(AUTOkom_export_pole[23]);    // osvetleni Mesice
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
            Serial2.println(vystupni_retezec);
          }
        else                                                 // odesila se jen omezena cast zaznamu
          {

            for (uint16_t i = 30; i < 54 ; i++)          // " 29.12.2020 ; 08:59:23 ;"    - datum a cas se tiskne vzdycky
              {
                Serial2.print(vystupni_retezec[i]);
              }

            for (uint16_t i = 74; i < 83; i++)           // " 08,881 ;"   - jas se tiskne vzdycky
              {
                Serial2.print(vystupni_retezec[i]);
              }
            
    
    
            if ((AUTOkom_obsah & 0b00000001) == 1)           // pridat jeste info o pocasi
              {
                for (uint16_t i = 150; i < 159; i++)     // " +21,96 ;"   - teplota
                  {
                    Serial2.print(vystupni_retezec[i]);
                  }

                for (uint16_t i = 167; i < 186; i++)     // " 051,51 ; 093410 ;"   - vlhkost ; tlak
                  {
                    Serial2.print(vystupni_retezec[i]);
                  }
              }
    
            if ((AUTOkom_obsah & 0b00000010) == 2)           // pridat jeste detailni info o svetle
              {
                for (uint16_t i = 101; i < 142; i++)     // "  STABIL  ; 17989 ; 48311 ; 600 ;  428x ;"   -stabilita ; infra ; full; ATIME; AGAIN
                  {
                    Serial2.print(vystupni_retezec[i]);
                  }
              }
    
    
            if ((AUTOkom_obsah & 0b00000100) == 4)           // pridat jeste info o naklonu a azimutu
              {
                for (uint16_t i = 220; i < 235; i++)     // " +165.8 ; 123 ;"   - naklon ; azimut
                  {
                    Serial2.print(vystupni_retezec[i]);
                  }
              }

    
            if ((AUTOkom_obsah & 0b00001000) == 8)           // pridat jeste info o Slunci a Mesici
              {
                for (uint16_t i = 235; i < 256; i++)     // "  -45 ;  -27 ;  58  ;"   - elevace Slunce ; elevace Mesice; osvetleni Mesice
                  {
                    Serial2.print(vystupni_retezec[i]);
                  }
              }
            Serial2.print('\n');
          }
      }

    prijem();                                                // uvolnit linku pro prijem
  }
  
