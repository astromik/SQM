// Podprogram pro mereni ve vsech moznych variantach spousteni
//  (dalo by se to zjednodusit, ale nechal jsem to prehledne kazde zvlast i kdyz jsou rozdily mezi jednotlivymi typy nekdy minimalni)
//=========================================================================================================================================
// Zmeri vsechny veliciny a ulozi je vcetne casu a znacky do docasneho "pole_EEPROM[]", podle typu mereni pak ulozi hodnoty do externi pameti a na SD kartu)
//  Muze ukladat bud posledni zprumerovanou hodnotu, nebo kazdy vzorek zvlast

// param =  1 ....  - spusteni tlacitkem UP
//                  - pocet mereni podle promenne 'prumery'
//                  - do pameti a na kartu uklada jen zaverecny prumer jasu (Polozky FULL, INFRA a GAIN bere posledni - bez prumeru)
//                  - zobrazuje vysledek na displeji
//                  - bargraf zobrazuje na displeji (pokud je k dispozici)
//                  - pri mereni pipa
//                  - provadi korekci teploty a jasu podle kalibracnich tabulek
//                  - na zaver ceka 8 sekund pred zhasnutim displeje (casovani je mozne prerusit stiskem tlacitka OK, nebo nejakou komunikaci)
//                  - pri pouziti expanzni desky se meri jen aktivnim cidlem (neprobiha automaticke prepinani cidel)
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se zapipa a na displeji se zobrazi napis "BUSSY" - mereni nezacne

// param =  2 ....  - spusteni tlacitkem DOWN (jinak uplne stejne jako param = 1)
//                  - pocet mereni podle promenne 'prumery'
//                  - do pameti a na kartu uklada jen zaverecny prumer jasu (Polozky FULL, INFRA a GAIN bere posledni - bez prumeru)
//                  - zobrazuje vysledek na displeji
//                  - bargraf zobrazuje na displeji (pokud je k dispozici)
//                  - pri mereni pipa
//                  - provadi korekci teploty a jasu podle kalibracnich tabulek
//                  - na zaver ceka 8 sekund pred zhasnutim displeje (casovani je mozne prerusit stiskem tlacitka OK, nebo nejakou komunikaci)
//                  - pri pouziti expanzni desky se meri jen aktivnim cidlem (neprobiha automaticke prepinani cidel)
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se zapipa a na displeji se zobrazi napis "BUSSY" - mereni nezacne

// param =  3 ....  - spusteni tlacitkem OK (jinak uplne stejne jako param = 1)
//                  - pocet mereni podle promenne 'prumery'
//                  - do pameti a na kartu uklada jen zaverecny prumer jasu (Polozky FULL, INFRA a GAIN bere posledni - bez prumeru)
//                  - zobrazuje vysledek na displeji
//                  - bargraf zobrazuje na displeji (pokud je k dispozici)
//                  - pri mereni pipa
//                  - provadi korekci teploty a jasu podle kalibracnich tabulek
//                  - na zaver ceka 8 sekund pred zhasnutim displeje (casovani je mozne prerusit stiskem tlacitka OK, nebo nejakou komunikaci)
//                  - pri pouziti expanzni desky se meri jen aktivnim cidlem (neprobiha automaticke prepinani cidel)
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se zapipa a na displeji se zobrazi napis "BUSSY" - mereni nezacne

// param =  4 ....  - spusteni prikazem 'm' pres USB seriovou linku
//                  - pocet mereni podle promenne 'prumery'
//                  - do pameti a na kartu uklada jen zaverecny prumer jasu (Polozky FULL, INFRA a GAIN bere posledni - bez prumeru)
//                  - nezobrazuje vysledek na displeji
//                  - bargraf zobrazuje v seriovem terminalu
//                  - pri mereni nepipa
//                  - provadi korekci teploty a jasu podle kalibracnich tabulek
//                  - pri pouziti expanzni desky se meri jen aktivnim cidlem (neprobiha automaticke prepinani cidel)
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se sice dalsi mereni nezacne, ale nic se nehlasi a nic nepipne

// param =  5 ....  - spusteni prikazem pres seriovou linku RS485 (prikaz pro adresu 1: 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE)
//                  - pocet mereni podle promenne 'prumery'
//                  - do pameti a na kartu uklada jen zaverecny prumer jasu (Polozky FULL, INFRA a GAIN bere posledni - bez prumeru)
//                  - nezobrazuje vysledek na displeji
//                  - bez bargrafu (stav mereni je mozne vyzadat pres RS485 prikazem: 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFA)
//                  - pri mereni nepipa
//                  - provadi korekci teploty a jasu podle kalibracnich tabulek
//                  - pri pouziti expanzni desky se meri jen aktivnim cidlem (neprobiha automaticke prepinani cidel)
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se sice dalsi mereni nezacne, ale nic se nehlasi a nic nepipne

// param =  6 ....  - automaticke casovane spusteni (je mozne spustit jen v pripade, ze jsou vsechny predchozi mereni dokoncene - nemutze se stat, ze by v pulce jednoho mereni prisel pozadavek na dalsi mereni)
//                  - pocet mereni podle promenne 'prumery'
//                  - do pameti a na kartu uklada jen zaverecny prumer jasu (Polozky FULL, INFRA a GAIN bere posledni - bez prumeru)
//                  - nezobrazuje vysledek na displeji
//                  - bez bargrafu
//                  - pri mereni nepipa
//                  - provadi korekci teploty a jasu podle kalibracnich tabulek
//                  - pokud je zapojena expanzni deska, bude se merit na kazdem cidle zvlast (cyklus od cidla 1 do maximalniho zadaneho poctu vstupu) 
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se sice dalsi mereni nezacne, ale nic se nehlasi a nic nepipne

// param =  7 ....  - kalibracni mereni jasu spustene tlacitkem pres menu
//                  - pocet mereni 10
//                  - do pameti a na kartu uklada kazdy zmereny vzorek i zaverecny prumer jasu (pokud nedoslo v prubehu mereni ke zmene GAIN, tak polozky FULL a INFRA take prumeruje)
//                  - vysledek zobrazuje na displeji (pokud je k dispozici)
//                  - bargraf na displeji (pokud je k dispozici)
//                  - pri mereni pipa
//                  - neprovadi korekci teploty a jasy uklada take bez korekce - tak jak vylezou z cidla
//                  - pri pouziti expanzni desky se meri jen aktivnim cidlem (neprobiha automaticke prepinani cidel)
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se zapipa a na displeji se zobrazi napis "BUSSY" - mereni nezacne

// param =  8 ....  - kalibracni mereni jasu spustene pres USB seriovou linku (prikaz "ks")
//                  - pocet mereni 10
//                  - do pameti a na kartu uklada kazdy zmereny vzorek i zaverecny prumer jasu (pokud nedoslo v prubehu mereni ke zmene GAIN, tak polozky FULL a INFRA take prumeruje)
//                  - vysledek zobrazuje na displeji (pokud je k dispozici)
//                  - bargraf zobrazuje v seriovem terminalu 
//                  - pri mereni nepipa
//                  - neprovadi korekci teploty a jasy uklada take bez korekce - tak jak vylezou z cidla
//                  - pri pouziti expanzni desky se meri jen aktivnim cidlem (neprobiha automaticke prepinani cidel)
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se sice dalsi mereni nezacne, ale nic se nehlasi a nic nepipne

// param =  9 ....  - kalibracni mereni jasu spustene pres seriovou linku RS485 (prikaz 0x01, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF3)
//                  - pocet mereni 10
//                  - do pameti a na kartu uklada kazdy zmereny vzorek i zaverecny prumer jasu (pokud nedoslo v prubehu mereni ke zmene GAIN, tak polozky FULL a INFRA take prumeruje)
//                  - vysledek nezobrazuje
//                  - bargraf nezobrazuje
//                  - pri mereni nepipa
//                  - neprovadi korekci teploty a jasy uklada take bez korekce - tak jak vylezou z cidla
//                  - pri pouziti expanzni desky se meri jen aktivnim cidlem (neprobiha automaticke prepinani cidel)
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se sice dalsi mereni nezacne, ale nic se nehlasi a nic nepipne

// param =  10 .... - stejne jako param = 4, ale nic nevypisuje (zadny bargraf, zadne hlaseni)
//                  - spusteni prikazem '*m' pres USB seriovou linku
//                  - pocet mereni podle promenne 'prumery'
//                  - do pameti a na kartu uklada jen zaverecny prumer jasu (Polozky FULL, INFRA a GAIN bere posledni - bez prumeru)
//                  - nezobrazuje vysledek na displeji
//                  - bargraf zobrazuje v seriovem terminalu
//                  - pri mereni nepipa
//                  - provadi korekci teploty a jasu podle kalibracnich tabulek
//                  - pri pouziti expanzni desky se meri jen aktivnim cidlem (neprobiha automaticke prepinani cidel)
//                  - pri pokusu o spusteni mereni kdyz jeste neni dokoncene predchozi mereni se sice dalsi mereni nezacne, ale nic se nehlasi a nic nepipne


// param =  11 .... - slouzi jen k mereni jasu pozadi pri urcovani viditelnosti DSO
//                  - zmeri nekolik vzorku podle parametru prumerovani
//                  - nemeri teplotu, tlak ani vlhkost. Nezjistuje GPS.
//                  - nic se nikam nezapisuje (ani na kartu ani do EEPROM)
//                  - na displeji se chova jako pri mereni spustenem tlacitkem (faze ustaleni hodnot a faze mereni s pipanim mezi vzorky a zaverecnym pipanim podle stability)
//                  - zadny vystup do seriove linky

//----------------------------------------------
// hlavni mereici podprogram
void mereni(uint8_t param)
  {
    bool znacka_stability;

    if (dokoncene_mereni == 0)                                                                        // jeste probiha predchozi mereni, neni mozne spustit mereni nove
      {
        switch (param)
          {
            case 4:                                                                                   // pri techto zpusobech spusteni NEPIPAT "Bussy signal"
            case 5:
            case 6:
            case 8:
            case 9:
            break;

            default:                                                                                  // pri vsech ostatnich typech spousteni zapipat "Bussy signal"
              tone_X(pin_bzuk,2500, 10 , 6);           
              zobraz_text(39);                                                                        // na displeji zobrazi napis "BUSSY"
              SD_log(998 , 12); 
          }        
      }
    else                                                                                              // predchozi mereni bylo ukonceno, muze zacit nove mereni
      {
        dokoncene_mereni = 0;                                                                         // dokud probiha mereni, neni mozne ziskat pres RS485 vysledky

#ifdef ukladat_GPS

        if (posledni_i2C_prikaz != 20)                                                                // kdyz je zapojeny GPS modul, tak se na nem pred merenim jeste zhasinaji LED
          {
            posledni_i2C_prikaz = 20;
            if (digitalRead(pin_DATA_RDY) == osazeno_gps)                                             // s modulem se komunikuje jen kdyz je pripojeny
              {
                if (param < 4)                                                                        // v rezimu automatickeho mereni, nebo spusteni na dalku se nepipa vubec
                  {
                    tone_X(pin_bzuk,450, 10 , 1);                                                     // jedno pipnuti jeste pred zhasnutim LED na GPS modulu
                  }
                Wire.beginTransmission(I2C_ADDR_GPS);                                                 // LED na GPS modulu se pred zacatkem mereni zhasnou
                delay(12);
                Wire.write(20);
                delay(12);
                Wire.endTransmission();
                delay(1500);                                                                          // zhasnuti LED na GPS modulu chvili trva, tak se musi pockat
              }
          }
#endif
        zobraz_RTC(false);
        astro_vypocty();                                                                              // ze zadanych geosouradnic a casu se vypocte elevace Slunce a Mesice a jeho osvetleni. Vysledky jsou v globalnich promennych.


    
        //-----------------------SPUSTENI TLACITKEM---------------------------------
        if (param <  4)                                                                               // normalni mereni spustene nekterym tlacitkem
          {
            znacka = param;                                                                           // znacka se nasledne uklada do zaznamu na SD kartu a EEPROM    
            prubeh_mereni = 5 + prumery;                                                              // tohle cislo se vraci do linky RS485 (5 pokusu o ustaleni + pocet vzorku, ze kterych se dela prumer)
            znacka_stability = test_stability(true,9);                                                // pred hlavnim merenim se provadi test stability mereni (2 az 5 pokusu) - neblika zadna tecka [9] (blika odpocet), pipa
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
    
            uint32_t suma_svetla = 0;                                                            // prumerovani mereni svetla
            uint16_t posledni_jas = 0;
            for (uint8_t vzorek = 1; vzorek <= prumery ; vzorek ++ )
              {

                prubeh_mereni = prumery - vzorek;

                tone_X(pin_bzuk,600, 10 , 1);                                                         // pri spusteni tlacitkem se pipa
                zobraz_cislo(prumery - vzorek, 3);                                                    //  ... pred kazdym merenim blikne zbyvajici pocet vzorku pro prumerovani ve stylu "-nn-"
                MODdata[31] = (status_afd << 8) | (err_bit << 7) | (prumery - vzorek);                // prubeh mereni do modbusu
                delay(100);
                
                int_svetlo = svetlo_1x(false,false);                                                  // pred merenim svetla se vzdycky displej zhasne
       
                if (vzorek == 1)                                                                      // pri prvnim vzorku se jeste nic netestuje - jen se nastavi pocatecni hodnoty
                  {                                                                                   //   se kterymi se bude porovnavat nasledujici vzorek
                    posledni_jas = korekce_svetla(int_svetlo);                                        // porovnavaji se hodnoty jasu zkorigovane pomoci kalibracni tabulky
                  }
                else
                  {                    
                    if ((rozdil_jasu(posledni_jas,korekce_svetla(int_svetlo)) / (float)korekce_svetla(int_svetlo)) > (rozhod_stab / 1000.0))      // porovnani rozdilu mezi sousednimi hodnotami
                      {
                        znacka_stability = false;                                                     //  nestabilni
                        bitClear (znacka,3);
                      }
                    posledni_jas = korekce_svetla(int_svetlo);
                  }

    
   
                suma_svetla = suma_svetla + int_svetlo;

                if (modul_LSM303DLHC == true)                                                         // azimut se prumeruje
                  {
                    int_azimut  = zjisti_azimut();                                                    // kdyz je zapnuty kompas, zmeri se azimut
                    pole_azimutu[vzorek - 1] = int_azimut;                
                  }
              }
            if (znacka_stability == true)                                                             // po poslednim vzorku se pipne jinym tonem
              {
                tone_X(pin_bzuk,1000, 10 , 1);                                                        // kdyz bylo mereni stabilni, bzukne se jednim vysokym tonem
              }
            else
              {
                for (uint8_t bz = 0; bz < 3 ; bz++ )
                  {
                    tone_X(pin_bzuk,600, 10 , 1);                                                     // kdyz nebylo mereni stabilni, bzukne se 3x hlubsim tonem
                    delay(100);
                  }
              }                         
            
            int_svetlo = suma_svetla / prumery;                                                       // a vypocte se prumer v uint16_t formatu
            int_svetlo = korekce_svetla(int_svetlo);                                                  // pro tlacitkove spousteni se provadi i tabulkova korekce
            
            int_teplota = teplota(true);                                                              // teplota s korekci podle tabulky
            int_vlhkost = vlhkost();
            int_tlak    = tlak();

            int_naklon  = 65535;
            int_azimut  = 65535;
            if (modul_LSM303DLHC == true)
              {
                int_naklon  = uhel();                                                                 // kdyz je zapnute cidlo naklonu, tak zmeri i uhel nakloneni krabicky 
                if (int_naklon > 11800 or int_naklon < 9100) chyba(8);                                // naklon se spatne precetl (vic nez 180 stupnu, nebo min nez -90 stupnu), ulozi se sice nesmyslne cislo, ale zahlasi se chyba 
                else                                         bitClear(err_bit,4);                     // naklon je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
                velikost_pole_azimutu = prumery;
                int_azimut = prumeruj_azimuty();
              }              
              
            int_rezerva_1 = rezervni_cidlo_1();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_2 = rezervni_cidlo_2();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_3 = rezervni_cidlo_3();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_4 = rezervni_cidlo_4();                                                       // pripadne spousteni mereni z rezervnich cidel


            gps(2);                                                                                   // pokud je cas z GPS dostupny, provede se serizeni RTC vcetne pripadnych autokorekci
                        
            priprava_pole_EEPROM();
            zaznam_EEPROM();                                                                          //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)
    
            pozice_tecky = 3;                                                                         // svetlo na 3 desetinna mista   
           
            zobraz_cislo(int_svetlo , 0);
            
            bool nic_stisknuto;
            bool seriove_pozdrzeni = false;                                                           //  v pripade preruseni zobrazeni hodnoty z duvodu komunikace blikne hodnota alespon na 0,5 sek
            for (uint16_t i = 0; i < 800 ; i++)                                                   //  po ulozeni zmereneho zaznamu zustava displej svitit 8 sekund (800x 10ms)
              {
                if (menu_dis_led == false) break;                                                     // kdyz neni zapnuta verze s displejem, nema cenu cekat s rozsvicenou hdnotou
                if (Serial.available() or Serial2.available())                                        // pri dlouhem zobrazeni posledni hodnoty se objevila komunikace. Zobrazeni se ukoncuje a provadi se navrat do hlavni smycky
                  {
                    seriove_pozdrzeni = true;
                    break;
                  }
    
                nic_stisknuto = digitalRead(pin_tl_up) and digitalRead(pin_tl_dn)and digitalRead(pin_tl_ok);
                if (nic_stisknuto == false)                                                           // neco bylo stisknuto
                  {
                    delay(50);
                    break;               
                  }
                delay(10);
              }

            if (seriove_pozdrzeni == true)                                                            // v pripade prdcasneho ukonceni z duvodu komunikace se hodnota zobrazi alespon na 0,5 sek
              {
                delay(500);                
              }
            zobraz_text(66);                                                                          // zhasne displej
            delay(100);
            while (nic_stisknuto == false)                                                            // kdyz je neco stisknuto, tak se ceka na uvolneni
              {
                nic_stisknuto = digitalRead(pin_tl_up) and digitalRead(pin_tl_dn)and digitalRead(pin_tl_ok)  ;
                delay(50);
              }
            delay(100);
          }
        //--------------------------------------------------------
    
        
        
        //---------------SPUSTENI PRES USB-----------------------------------------
        if (param ==  4)                                                                              // normalni mereni pres USB seriovou linku
          {
            znacka = 4;                                                                               // znacka "komun" se nasledne uklada do zaznamu na SD kartu a EEPROM
    
            Serial.print("\r\n");                                                                     // horni cast bargrafu do serioveho terminalu
            pomlcky(prumery);
            
            Serial.print('|');


            prubeh_mereni = 5 + prumery;                                                              // tohle cislo se vraci do linky RS485 (5 pokusu o ustaleni + pocet vzorku, ze kterych se dela prumer)
            znacka_stability = test_stability(false, 3);                                              // pred hlavnim merenim se provadi test stability mereni (2 az 5 pokusu) - blika 2. tecka zleva
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
    
            uint32_t suma_svetla = 0;                                                            // prumerovani mereni svetla
            uint16_t posledni_jas = 0;        
            
            for (uint8_t vzorek = 1; vzorek <= prumery ; vzorek ++ )
              {
                prubeh_mereni = prumery - vzorek;

                MODdata[31] = (status_afd << 8) | (err_bit << 7) | (prumery - vzorek);                // prubeh mereni do modbusu
                zobraz_text(52);                                                                      // " .   " (prave bezi mereni spustene seriovou linkou)
                delay(100);
                
                int_svetlo = svetlo_1x(false,false);
                
                if (vzorek == 1)                                                                      // pri prvnim vzorku se jeste nic netestuje - jen se nastavi pocatecni hodnoty
                  {                                                                                   //   se kterymi se bude porovnavat nasledujici vzorek
                    posledni_jas = korekce_svetla(int_svetlo);
                  }
                else
                  {
                    if ((rozdil_jasu(posledni_jas,korekce_svetla(int_svetlo)) / (float)korekce_svetla(int_svetlo)) > (rozhod_stab / 1000.0))      // porovnani rozdilu mezi sousednimi hodnotami
                      {
                        znacka_stability = false;                                                     //  nestabilni
                        bitClear (znacka,3);
                      }
                    posledni_jas = korekce_svetla(int_svetlo);
                  }            
                suma_svetla = suma_svetla + int_svetlo;

                if (modul_LSM303DLHC == true)                                                         // azimut se prumeruje
                  {
                    int_azimut  = zjisti_azimut();                                                    // kdyz je zapnuty kompas, zmeri se azimut
                    pole_azimutu[vzorek - 1] = int_azimut;                
                  }

                
                Serial.print('#');                                                                    // vyplni 1 pole v bargrafu
              }
            
            Serial.println('|');                                                                      // ukonceni bargrafu v seriovem terminalu
            
            int_svetlo = suma_svetla / prumery;                                                       // a vypocte se prumer v uint16_t formatu
            int_svetlo = korekce_svetla(int_svetlo);                                                  // pro tlacitkove spousteni se provadi i tabulkova korekce
            
            int_teplota = teplota(true);                                                              // teplota s korekci podle tabulky
            int_vlhkost = vlhkost();
            int_tlak    = tlak();

            int_naklon = 65535;                                                                       // pro pripad, ze se naklon nepouziva, nastavi se na 65535
            int_azimut  = 65535;
            if (modul_LSM303DLHC == true)
              {
                int_naklon  = uhel();                                                                 // kdyz je zapnute cidlo naklonu, tak zmeri i uhel nakloneni krabicky 
                velikost_pole_azimutu = prumery;
                int_azimut = prumeruj_azimuty();
              }
            int_rezerva_1 = rezervni_cidlo_1();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_2 = rezervni_cidlo_2();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_3 = rezervni_cidlo_3();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_4 = rezervni_cidlo_4();                                                       // pripadne spousteni mereni z rezervnich cidel
            
            gps(2);                                                                                   // pokud je cas z GPS dostupny, provede se serizeni RTC vcetne pripadnych autokorekci
            priprava_pole_EEPROM();

            zaznam_EEPROM();                                                                          //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)

            if (err_bit > 0) hlaseni_chyb();                                                          // kdyz se behem mereni vyskytla nejaka chyba, zahlasi se varovani do seriove linky 
            
          }
        //--------------------------------------------------------
    


        //---------------SPUSTENI PRES USB bez bargrafu -----------------------------------------
        if (param ==  10)                                                                             // normalni mereni pres USB seriovou linku
          {
            znacka = 4;                                                                               // znacka "komun" se nasledne uklada do zaznamu na SD kartu a EEPROM
    
            prubeh_mereni = 5 + prumery;                                                              // tohle cislo se vraci do linky RS485 (5 pokusu o ustaleni + pocet vzorku, ze kterych se dela prumer)
            znacka_stability = test_stability(false, 3);                                              // pred hlavnim merenim se provadi test stability mereni (2 az 5 pokusu) - blika 2. tecka zleva
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
    
            uint32_t suma_svetla = 0;                                                            // prumerovani mereni svetla
            uint16_t posledni_jas = 0;        
            
            for (uint8_t vzorek = 1; vzorek <= prumery ; vzorek ++ )
              {
                prubeh_mereni = prumery - vzorek;

                MODdata[31] = (status_afd << 8) | (err_bit << 7) | (prumery - vzorek);                // prubeh mereni do modbusu
                zobraz_text(52);                                                                      // " .   " (prave bezi mereni spustene seriovou linkou)
                delay(100);
                
                int_svetlo = svetlo_1x(false,false);
                
                if (vzorek == 1)                                                                      // pri prvnim vzorku se jeste nic netestuje - jen se nastavi pocatecni hodnoty
                  {                                                                                   //   se kterymi se bude porovnavat nasledujici vzorek
                    posledni_jas = korekce_svetla(int_svetlo);
                  }
                else
                  {
                    if ((rozdil_jasu(posledni_jas,korekce_svetla(int_svetlo)) / (float)korekce_svetla(int_svetlo)) > (rozhod_stab / 1000.0))      // porovnani rozdilu mezi sousednimi hodnotami
                      {
                        znacka_stability = false;                                                     //  nestabilni
                        bitClear (znacka,3);
                      }
                    posledni_jas = korekce_svetla(int_svetlo);
                  }            
                suma_svetla = suma_svetla + int_svetlo;
                
                if (modul_LSM303DLHC == true)                                                         // azimut se prumeruje
                  {
                    int_azimut  = zjisti_azimut();                                                    // kdyz je zapnuty kompas, zmeri se azimut
                    pole_azimutu[vzorek - 1] = int_azimut;                
                  }

              }
            
            
            int_svetlo = suma_svetla / prumery;                                                       // a vypocte se prumer v uint16_t formatu
            int_svetlo = korekce_svetla(int_svetlo);                                                  // pro tlacitkove spousteni se provadi i tabulkova korekce
            
            int_teplota = teplota(true);                                                              // teplota s korekci podle tabulky
            int_vlhkost = vlhkost();
            int_tlak    = tlak();

            int_naklon = 65535;                                                                       // pro pripad, ze se naklon nepouziva, nastavi se na 65535
            int_azimut  = 65535;
            if (modul_LSM303DLHC == true)
              {
                int_naklon  = uhel();                                                                 // kdyz je zapnute cidlo naklonu, tak zmeri i uhel nakloneni krabicky 
                velikost_pole_azimutu = prumery;                                                      // z jednotlivych vzorku azimutu se spocte prumer
                int_azimut = prumeruj_azimuty();
              }
            int_rezerva_1 = rezervni_cidlo_1();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_2 = rezervni_cidlo_2();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_3 = rezervni_cidlo_3();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_4 = rezervni_cidlo_4();                                                       // pripadne spousteni mereni z rezervnich cidel
            
            gps(2);                                                                                   // pokud je cas z GPS dostupny, provede se serizeni RTC vcetne pripadnych autokorekci
            priprava_pole_EEPROM();

            zaznam_EEPROM();                                                                          //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)

            if (err_bit > 0) hlaseni_chyb();                                                          // kdyz se behem mereni vyskytla nejaka chyba, zahlasi se varovani do seriove linky 
            
          }
        //--------------------------------------------------------

        
        
        //---------------SPUSTENI PRES RS485-----------------------------------------
        if (param ==  5)                                                                              // Spusteni mereni pres RS485
          {
            prubeh_mereni = 5 + prumery;                                                              // tohle cislo se vraci do linky RS485 (5 pokusu o ustaleni + pocet vzorku, ze kterych se dela prumer)
            znacka = 4;                                                                               // znacka "komun" se nasledne uklada do zaznamu na SD kartu a EEPROM
            znacka_stability = test_stability(false, 3);                                              // pred hlavnim merenim se provadi test stability mereni (2 az 5 pokusu) blika druha tecka zleva
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
    
            uint32_t suma_svetla = 0;                                                            // prumerovani mereni svetla
            uint16_t posledni_jas = 0;
            
            for (uint8_t vzorek = 1; vzorek <= prumery ; vzorek ++ )
              {
                zobraz_text(52);                                                                      // " .   " (prave bezi mereni spustene seriovou linkou)
                delay(100);
                
                int_svetlo = svetlo_1x(false,false);
                prubeh_mereni = prumery - vzorek;
                MODdata[31] = (status_afd << 8) | (err_bit << 7) | (prumery - vzorek);                // prubeh mereni do modbusu
                if (vzorek == 1)                                                                      // pri prvnim vzorku se jeste nic netestuje - jen se nastavi pocatecni hodnoty
                  {                                                                                   //   se kterymi se bude porovnavat nasledujici vzorek
                    posledni_jas = korekce_svetla(int_svetlo);
                  }
                else
                  {
                    if ((rozdil_jasu(posledni_jas,korekce_svetla(int_svetlo)) / (float)korekce_svetla(int_svetlo)) > (rozhod_stab / 1000.0))      // porovnani rozdilu mezi sousednimi hodnotami
                      {
                        znacka_stability = false;                                                     //  nestabilni
                        bitClear (znacka,3);
                      }
                    posledni_jas = korekce_svetla(int_svetlo);
                  }            
                suma_svetla = suma_svetla + int_svetlo;

                if (modul_LSM303DLHC == true)                                                         // azimut se prumeruje
                  {
                    int_azimut  = zjisti_azimut();                                                    // kdyz je zapnuty kompas, zmeri se azimut
                    pole_azimutu[vzorek - 1] = int_azimut;                
                  }
                
              }
                    
            int_svetlo = suma_svetla / prumery;                                                       // a vypocte se prumer v uint16_t formatu
            int_svetlo = korekce_svetla(int_svetlo);                                                  // pro tlacitkove spousteni se provadi i tabulkova korekce
            
            int_teplota = teplota(true);                                                              // teplota s korekci podle tabulky
            int_vlhkost = vlhkost();
            int_tlak    = tlak();

            int_naklon  = 65535;                                                                      // pro pripad, ze se naklon nepouziva, nastavi se na 65535
            int_azimut  = 65535;
            if (modul_LSM303DLHC == true)
              {
                int_naklon  = uhel();                                                                 // kdyz je zapnute cidlo naklonu, tak zmeri i uhel nakloneni krabicky 
                velikost_pole_azimutu = prumery;                                                      // z jednotlivych vzorku azimutu se spocte prumer
                int_azimut = prumeruj_azimuty();
              }
            int_rezerva_1 = rezervni_cidlo_1();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_2 = rezervni_cidlo_2();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_3 = rezervni_cidlo_3();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_4 = rezervni_cidlo_4();                                                       // pripadne spousteni mereni z rezervnich cidel

            gps(2);                                                                                   // pokud je cas z GPS dostupny, provede se serizeni RTC vcetne pripadnych autokorekci
            
            priprava_pole_EEPROM();
            zaznam_EEPROM();                                                                          //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)
          }
        //--------------------------------------------------------
    
    
        //---------------AUTOMATICKE SPUSTENI-----------------------------------------
        if (param ==  6)                                                                              // automaticke casovane spousteni
          {
            znacka = 5;                                                                               // znacka "auto" se nasledne uklada do zaznamu na SD kartu a EEPROM
            
            prubeh_mereni = (5 + prumery) * pocet_cidel_svetla;                                       // tohle cislo se vraci do linky RS485 (5 pokusu o ustaleni + pocet vzorku, ze kterych se dela prumer a to cele se opakuje tolikrat, kolik je zapnutych cidel)
            uint8_t pamet_cidla = index_cidla_svetla;                                                    // pamet aktualniho cidla, aby se po navratu z mereni ze vsech cidel zase mohlo nastavit to puvodni
            for (uint8_t cidla = 1; cidla <= pocet_cidel_svetla ; cidla++)
              {
                prepni_cidlo_svetla(cidla);                                                           // prepnuti na pozadovane cidlo
                znacka_stability = test_stability(false,4);                                           // pred hlavnim merenim se provadi test stability mereni (2 az 5 pokusu) blika leva tecka
                if (znacka_stability == true)   bitSet (znacka,3);                                    // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
        
                uint32_t suma_svetla = 0;                                                        // prumerovani mereni svetla
                uint16_t posledni_jas = 0;
                
                for (uint8_t vzorek = 1; vzorek <= prumery ; vzorek ++ )
                  {
                    zobraz_text(48);                                                                  // ".    " (prave bezi automat)
                    MODdata[31] = (status_afd << 8) | (err_bit << 7) | (prumery - vzorek);            // prubeh mereni do modbusu
                    delay(100);

                    if (digitalRead(pin_tl_ok) == LOW)                                                // predcasne ukonceni automatickeho mereni stiskem tlacitka OK
                      {
                        zobraz_text(68);                                                              // "-----"
                        while (digitalRead(pin_tl_ok) == LOW)
                          {
                            delay(10);
                          }
                        zobraz_text(66);                                                              // "     "
                        delay(20);
                        dokoncene_mereni = 2;                                                         // neprovede se zadny zapis - proste se jen vypadne z podprogramu
                        return;
                      }
                    
                    int_svetlo = svetlo_1x(false,false);
                    prubeh_mereni = prumery - vzorek;
                    if (vzorek == 1)                                                                  // pri prvnim vzorku se jeste nic netestuje - jen se nastavi pocatecni hodnoty
                      {                                                                               //   se kterymi se bude porovnavat nasledujici vzorek
                        posledni_jas = korekce_svetla(int_svetlo);
                      }
                    else
                      {
                        if ((rozdil_jasu(posledni_jas,korekce_svetla(int_svetlo)) / (float)korekce_svetla(int_svetlo)) > (rozhod_stab / 1000.0))      // porovnani rozdilu mezi sousednimi hodnotami
                          {
                            znacka_stability = false;                                                 //  nestabilni
                            bitClear (znacka,3);
                          }
                        posledni_jas = korekce_svetla(int_svetlo);
                      } 
                    
                    suma_svetla = suma_svetla + int_svetlo;

                    if (modul_LSM303DLHC == true)                                                     // azimut se prumeruje
                      {
                        int_azimut  = zjisti_azimut();                                                // kdyz je zapnuty kompas, zmeri se azimut
                        pole_azimutu[vzorek - 1] = int_azimut;                
                      }

                  }
                        
                int_svetlo = suma_svetla / prumery;                                                   // vypocte se prumer v uint16_t formatu
                int_svetlo = korekce_svetla(int_svetlo);                                              // a tabulkova korekce
                
                int_teplota = teplota(true);                                                          // teplota s korekci podle tabulky
                int_vlhkost = vlhkost();
                int_tlak    = tlak();

                int_naklon  = 65535;                                                                  // pro pripad, ze se naklon nepouziva, nastavi se na 65535
                int_azimut  = 65535;
                if (modul_LSM303DLHC == true)
                  {
                    int_naklon  = uhel();                                                             // kdyz je zapnute cidlo naklonu, tak zmeri i uhel nakloneni krabicky 
                    velikost_pole_azimutu = prumery;                                                  // z jednotlivych vzorku azimutu se spocte prumer
                    int_azimut = prumeruj_azimuty();
                  }
                int_rezerva_1 = rezervni_cidlo_1();                                                   // pripadne spousteni mereni z rezervnich cidel
                int_rezerva_2 = rezervni_cidlo_2();                                                   // pripadne spousteni mereni z rezervnich cidel
                int_rezerva_3 = rezervni_cidlo_3();                                                   // pripadne spousteni mereni z rezervnich cidel
                int_rezerva_4 = rezervni_cidlo_4();                                                   // pripadne spousteni mereni z rezervnich cidel

                gps(1);                                                                               // presny cas z GPS se docasne ulozi do lokalnich promennych, ale RTC se neserizuje

                priprava_pole_EEPROM();
                zaznam_EEPROM();                                                                      //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)
              }

              prepni_cidlo_svetla(pamet_cidla);                                                       // prepnuti zpatky na cidlo, ktere bylo nastaveno pred vstupem do smycky
          
          }
        //--------------------------------------------------------
    
    
        //----------------------- KALIBRACNI MERENI PRES MENU ---------------------------------
        if (param == 7)                                                                               // kalibracni spusteni pres menu (je jedno, jestli s displejovou, nebo LED verzi signalizace)
          {
            znacka = 6;                                                                               // znacka "kalib" se nasledne uklada do zaznamu na SD kartu a EEPROM
            tone_X(pin_bzuk,600, 10 , 1);                                                             // jedno pipnuti jeste pred testem stability
    
            prubeh_mereni = 5 + 10;                                                                   // tohle cislo se vraci do linky RS485 (5 pokusu o ustaleni + 10 vzorku)
            znacka_stability = test_stability(true,9);                                                // pred hlavnim merenim se provadi test stability mereni (2 az 5 pokusu) - tecka pri testu neblika
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
    
                                                                
            int_teplota = 65535;                                                                      // v jednotlivych zaznamech 10 prumerovacich vzorku svetla se ostatni veliciny nezobrazuji
            int_vlhkost = 65535;
            int_tlak    = 65535;
            int_naklon  = 65535;
            int_azimut  = 65535;
            GPS_lat = 0xFFFFFFFFUL;
            GPS_lon = 0xFFFFFFFFUL;
            GPS_alt = 0xFFFF;
    
    
            uint32_t suma_svetla = 0;                                                            // prumerovani mereni svetla
            uint32_t suma_infra = 0;
            uint32_t suma_full = 0;
            uint32_t suma_teploty = 0;                                                           //   (a teploty)
            uint16_t posledni_jas = 0;
            bool again_zmena = false;
            uint8_t again_hodnota = 0;
            
            for (uint8_t vzorek = 1; vzorek < 11 ; vzorek ++ )                                           // pri kalibraci je prumerovani nastaveno na 10 vzorku
              {
                prubeh_mereni = 10 - vzorek;
                MODdata[31] = (status_afd << 8) | (err_bit << 7) | (10 - vzorek);                     // prubeh mereni do modbusu

                tone_X(pin_bzuk,600, 10 , 1);                                                         // pri spusteni tlacitkem se pipa

                zobraz_cislo(10 - vzorek , 3);                                                        // .. pred kazdym merenim blikne zbyvajici pocet vzorku pro prumerovani ve stylu "-nn-"
                delay(100);
                int_svetlo = svetlo_1x(false,false);                                                  // pred kazdym merenim svetla se displej zhasina
                                                                                                      // testovani stability mezi jednotlivymi vzorky
                if (vzorek == 1)                                                                      // pri prvnim vzorku z 10 se jeste nic netestuje - jen se nastavi pocatecni hodnoty
                  {                                                                                   //   se kterymi se bude porovnavat nasledujici vzorek
                    again_hodnota = byte_cnf;
                    posledni_jas = korekce_svetla(int_svetlo);
                  }
                else
                  {
                    if(again_hodnota != byte_cnf) again_zmena = true;                                 // pri zmene GAIN se neprovadi zapis prumeru FULL, IR, CNF a MS
    
                    if ((rozdil_jasu(posledni_jas,korekce_svetla(int_svetlo)) / (float)korekce_svetla(int_svetlo)) > (rozhod_stab / 1000.0))      // porovnani rozdilu mezi sousednimi hodnotami
                      {
                        znacka_stability = false;                                                     //  nestabilni
                        bitClear (znacka,3);
                      }
                    else
                      {
                        bitSet (znacka,3);                
                      }                
                    posledni_jas = korekce_svetla(int_svetlo);
                  }

                int_teplota = teplota(false);                                                         // teplota bez tabulkove korekce (cista hodnota primo z cidla)

                suma_teploty = suma_teploty + int_teplota;
                
                suma_svetla = suma_svetla + int_svetlo;

                if (modul_LSM303DLHC == true)                                                         // azimut se prumeruje
                  {
                    int_azimut  = zjisti_azimut();                                                    // kdyz je zapnuty kompas, zmeri se azimut
                    pole_azimutu[vzorek - 1] = int_azimut;                
                  }

                
                suma_infra = suma_infra + int_infra;
                suma_full = suma_full + int_full;
                
                priprava_pole_EEPROM();                                                               // pri kalibraci se uklada kazde z 10 mereni
                hranice_ukladani = 0;                                                                 //  (nezavisle na nastavene hranici pro ukladani)                
                zaznam_EEPROM();                                                                      //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)
    
              }
              
            if (znacka_stability == true)                                                             // po poslednim vzorku se pipne jinym tonem
              {
                tone_X(pin_bzuk,1000, 10 , 1);                                                        // kdyz bylo mereni stabilni, bzukne se jednim vysokym tonem
              }
            else
              {
                for (uint8_t bz = 0; bz < 3 ; bz++ )
                  {
                    tone_X(pin_bzuk,600, 10 , 1);                                                     // kdyz nebylo mereni stabilni, bzukne se 3 hlubsim tonem
                    delay(100);
                  }
              }  
            
            int_svetlo = suma_svetla / 10;                                                            // a vypocte se prumer v uint16_t formatu
            int_infra = suma_infra / 10;
            int_full = suma_full / 10;        
                                                                                                      // pred ulozenim prumerne hodnoty se zmeri i zbyle veliciny
            int_teplota = suma_teploty / 10;                                                          // teploty bez korekci se zprumeruji
            int_vlhkost = vlhkost();
            int_tlak    = tlak();
            if (modul_LSM303DLHC == true)
              {
                int_naklon  = uhel();                                                                 // kdyz je zapnute cidlo naklonu, tak zmeri i uhel nakloneni krabicky 
                if (int_naklon > 11800 or int_naklon < 9100) chyba(8);                                // naklon se spatne precetl (vic nez 180 stupnu, nebo min nez -90 stupnu), ulozi se sice nesmyslne cislo, ale zahlasi se chyba 
                else                                         bitClear(err_bit,4);                     // naklon je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
                velikost_pole_azimutu = prumery;                                                      // z jednotlivych vzorku azimutu se spocte prumer
                int_azimut = prumeruj_azimuty();
              }
            int_rezerva_1 = rezervni_cidlo_1();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_2 = rezervni_cidlo_2();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_3 = rezervni_cidlo_3();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_4 = rezervni_cidlo_4();                                                       // pripadne spousteni mereni z rezervnich cidel

            gps(2);                                                                                   // pokud je cas z GPS dostupny, provede se serizeni RTC vcetne pripadnych autokorekci
            
            znacka = 7;                                                                               // znacka "Ka-Pr" - posledni zaznam je prumer predchozich 10 hodnot
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
            if (again_zmena == true) byte_cnf = 255;                                                  // kdyz doslo ke zemne zesileni, ve vypisu budou pro IR, FULL, CNF a MS zobrazeny pomlcky
            priprava_pole_EEPROM();
            hranice_ukladani = 0;                                                                     //  i zprumerovany vysledek se uklada nezavisle na nastavene hranici pro ukladani
            zaznam_EEPROM();                                                                          //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)
    
            pozice_tecky = 3;                                                                         // svetlo na 3 desetinna mista   
            if (menu_dis_led == true)                                                                 // zprumerovana hodnota kalibracniho mereni se zobrazi jen v pripade, ze je pouzita verze s displejem
              {
                zobraz_cislo(int_svetlo , 0);            
              }
          }                                                                                           // kalibrace pomoci tlacitkoveho menu konci zobrazenim cisla - nic se automaticky nezhasina
        //--------------------------------------------------------
    
        
        
    
        //----------------------- KALIBRACNI MERENI PRES SERIOVOU LINKU USB ---------------------------------
        if (param == 8)                                                                               // kalibracni spusteni pres seriovou linku USB 
          {
            znacka = 6;                                                                               // znacka "kalib" se nasledne uklada do zaznamu na SD kartu a EEPROM
    
            Serial.print("\r\n");                                                                       // horni cast bargrafu do serioveho terminalu
            pomlcky(10);
            
            Serial.print('|');
    
            prubeh_mereni = 5 + 10;                                                                   // tohle cislo se vraci do linky RS485 (5 pokusu o ustaleni + 10 vzorku)
            znacka_stability = test_stability(false,3);                                               // pred hlavnim merenim se provadi test stability mereni (2 az 5 pokusu)
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
                                                                
            int_teplota = 65535;                                                                      // v jednotlivych zaznamech 10 prumerovacich vzorku svetla se ostatni veliciny nezobrazuji
            int_vlhkost = 65535;
            int_tlak    = 65535;
            int_naklon  = 65535;
            int_azimut  = 65535;
            GPS_lat = 0xFFFFFFFFUL;
            GPS_lon = 0xFFFFFFFFUL;
            GPS_alt = 0xFFFF;
    
    
            uint32_t suma_svetla = 0;                                                            // prumerovani mereni svetla
            uint32_t suma_infra = 0;
            uint32_t suma_full = 0;
            uint32_t suma_teploty = 0;                                                           //   (a teploty)
            uint16_t posledni_jas = 0;
            bool again_zmena = false;
            uint8_t again_hodnota = 0;
                    
            for (uint8_t vzorek = 1; vzorek < 11 ; vzorek ++ )                                           // pri kalibraci je prumerovani nastaveno na 10 vzorku
              {
                prubeh_mereni = 10 - vzorek;
                Serial.print('#');                                                                    // vyplni 1 pole v bargrafu
                MODdata[31] = (status_afd << 8) | (err_bit << 7) | (10 - vzorek);                     // prubeh mereni do modbusu

                zobraz_text(52);                                                                      // " .   " (prave bezi mereni spustene seriovou linkou)
                delay(100);
                
                int_svetlo = svetlo_1x(false,false);
    
                                                                                                      // testovani stability mezi jednotlivymi vzorky
                if (vzorek == 1)                                                                      // pri prvnim vzorku z 10 se jeste nic netestuje - jen se nastavi pocatecni hodnoty
                  {                                                                                   //   se kterymi se bude porovnavat nasledujici vzorek
                    again_hodnota = byte_cnf;
                    posledni_jas = korekce_svetla(int_svetlo);
                  }
                else
                  {
                    if(again_hodnota != byte_cnf) again_zmena = true;                                 // pri zmene GAIN se neprovadi zapis prumeru FULL a IR
    
                    if ((rozdil_jasu(posledni_jas,korekce_svetla(int_svetlo)) / (float)korekce_svetla(int_svetlo)) > (rozhod_stab / 1000.0))      // porovnani rozdilu mezi sousednimi hodnotami
                      {
                        znacka_stability = false;                                                     //  nestabilni
                        bitClear (znacka,3);
                      }
                    else
                      {
                        bitSet (znacka,3);                
                      }                
                    posledni_jas = korekce_svetla(int_svetlo);
                  }
                 
                int_teplota = teplota(false);                                                         // teplota bez tabulkove korekce (cista hodnota primo z cidla)

                suma_teploty = suma_teploty + int_teplota;    

                suma_svetla = suma_svetla + int_svetlo;

                if (modul_LSM303DLHC == true)                                                         // azimut se prumeruje
                  {
                    int_azimut  = zjisti_azimut();                                                    // kdyz je zapnuty kompas, zmeri se azimut
                    pole_azimutu[vzorek - 1] = int_azimut;                
                  }
                
                suma_infra = suma_infra + int_infra;
                suma_full = suma_full + int_full;
                
                priprava_pole_EEPROM();                                                               // pri kalibraci se uklada kazde z 10 mereni
                hranice_ukladani = 0;                                                                 //  (nezavisle na nastavene hranici pro ukladani)
                zaznam_EEPROM();                                                                      //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)
    
              }
    
            Serial.println('|');                                                                      // ukonceni bargrafu v seriovem terminalu          
            int_svetlo = suma_svetla / 10;                                                            // a vypocte se prumer v uint16_t formatu
            int_infra = suma_infra / 10;
            int_full = suma_full / 10;        
            int_teplota = suma_teploty / 10;                                                          // teploty bez korekci se zprumeruji

                                                                                                      // pred ulozenim prumerne hodnoty se zmeri i zbyle veliciny
            int_vlhkost = vlhkost();
            int_tlak    = tlak();
            if (modul_LSM303DLHC == true)
              {
                int_naklon  = uhel();                                                                 // kdyz je zapnute cidlo naklonu, tak zmeri i uhel nakloneni krabicky 
                velikost_pole_azimutu = prumery;                                                      // z jednotlivych vzorku azimutu se spocte prumer
                int_azimut = prumeruj_azimuty();
              }
            int_rezerva_1 = rezervni_cidlo_1();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_2 = rezervni_cidlo_2();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_3 = rezervni_cidlo_3();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_4 = rezervni_cidlo_4();                                                       // pripadne spousteni mereni z rezervnich cidel

            gps(2);                                                                                   // pokud je cas z GPS dostupny, provede se serizeni RTC vcetne pripadnych autokorekci
            
            znacka = 7;                                                                               // znacka "Ka-Pr" - posledni zaznam je prumer predchozich 10 hodnot
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
            if (again_zmena == true) byte_cnf = 255;                                                  // zajisti, ze ve vypisu budou pro IR, FULL, CNF a MS zobrazeny pomlcky
            priprava_pole_EEPROM();
            hranice_ukladani = 0;                                                                     //  i zprumerovany vysledek se uklada nezavisle na nastavene hranici pro ukladani
            zaznam_EEPROM();                                                                          //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)
            if (err_bit > 0) hlaseni_chyb();                                                          // kdyz se behem mereni vyskytla nejaka chyba, zahlasi se varovani do seriove linky 
    
          }
        //--------------------------------------------------------
    
    
        //----------------------- KALIBRACNI MERENI PRES SERIOVOU LINKU RS485 ---------------------------------
        if (param == 9)                                                                               // kalibracni spusteni pres seriovou linku RS485
          {
            znacka = 6;                                                                               // znacka "kalib" se nasledne uklada do zaznamu na SD kartu a EEPROM
    
            prubeh_mereni = 5 + 10;                                                                   // tohle cislo se vraci do linky RS485 (5 pokusu o ustaleni + 10 vzorku)
            znacka_stability = test_stability(false,3);                                               // pred hlavnim merenim se provadi test stability mereni (2 az 5 pokusu)
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
                                                                
            int_teplota = 65535;                                                                      // v jednotlivych zaznamech 10 prumerovacich vzorku svetla se ostatni veliciny nezobrazuji
            int_vlhkost = 65535;
            int_tlak    = 65535;
            int_naklon  = 65535;
            int_azimut  = 65535;
            GPS_lat = 0xFFFFFFFFUL;
            GPS_lon = 0xFFFFFFFFUL;
            GPS_alt = 0xFFFF;
    
    
            uint32_t suma_svetla = 0;                                                            // prumerovani mereni svetla
            uint32_t suma_infra = 0;
            uint32_t suma_full = 0;
            uint32_t suma_teploty = 0;                                                           //   (a teploty)
            uint16_t posledni_jas = 0;
            bool again_zmena = false;
            uint8_t again_hodnota = 0;
            
            for (uint8_t vzorek = 1; vzorek < 11 ; vzorek ++ )                                           // pri kalibraci je prumerovani nastaveno na 10 vzorku
              {
                prubeh_mereni = 10 - vzorek;
                MODdata[31] = (status_afd << 8) | (err_bit << 7) | (10 - vzorek);                     // prubeh mereni do modbusu

                zobraz_text(52);                                                                      // " .   " (prave bezi mereni spustene seriovou linkou)
                delay(100);

                int_svetlo = svetlo_1x(false,false);
    
                                                                                                      // testovani stability mezi jednotlivymi vzorky
                if (vzorek == 1)                                                                      // pri prvnim vzorku z 10 se jeste nic netestuje - jen se nastavi pocatecni hodnoty
                  {                                                                                   //   se kterymi se bude porovnavat nasledujici vzorek
                    again_hodnota = byte_cnf;
                    posledni_jas = korekce_svetla(int_svetlo);
                  }
                else
                  {
                    if(again_hodnota != byte_cnf) again_zmena = true;                                 // pri zmene GAIN se neprovadi zapis prumeru FULL a IR
    
                    if ((rozdil_jasu(posledni_jas,korekce_svetla(int_svetlo)) / (float)korekce_svetla(int_svetlo)) > (rozhod_stab / 1000.0))      // porovnani rozdilu mezi sousednimi hodnotami
                      {
                        znacka_stability = false;                                                     //  nestabilni
                        bitClear (znacka,3);
                      }
                    else
                      {
                        bitSet (znacka,3);                
                      }                
                    posledni_jas = korekce_svetla(int_svetlo);
                  }          

                int_teplota = teplota(false);                                                         // teplota bez tabulkove korekce (cista hodnota primo z cidla)

                suma_teploty = suma_teploty + int_teplota;
                                
                suma_svetla = suma_svetla + int_svetlo;

                if (modul_LSM303DLHC == true)                                                         // azimut se prumeruje
                  {
                    int_azimut  = zjisti_azimut();                                                    // kdyz je zapnuty kompas, zmeri se azimut
                    pole_azimutu[vzorek - 1] = int_azimut;                
                  }
                
                suma_infra = suma_infra + int_infra;
                suma_full = suma_full + int_full;
                
                priprava_pole_EEPROM();                                                               // pri kalibraci se uklada kazde z 10 mereni
                hranice_ukladani = 0;                                                                 //  (nezavisle na nastavene hranici pro ukladani)
                zaznam_EEPROM();                                                                      //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)
    
              }
    
            int_svetlo = suma_svetla / 10;                                                            // a vypocte se prumer v uint16_t formatu
            int_infra = suma_infra / 10;
            int_full = suma_full / 10;
            int_teplota = suma_teploty / 10;                                                          // teploty bez korekci se zprumeruji
                        
                                                                                                      // pred ulozenim prumerne hodnoty se zmeri i zbyle veliciny
            int_vlhkost = vlhkost();
            int_tlak    = tlak();
            if (modul_LSM303DLHC == true)
              {
                int_naklon  = uhel();                                                                 // kdyz je zapnute cidlo naklonu, tak zmeri i uhel nakloneni krabicky 
                velikost_pole_azimutu = prumery;                                                      // z jednotlivych vzorku azimutu se spocte prumer
                int_azimut = prumeruj_azimuty();
              }
            int_rezerva_1 = rezervni_cidlo_1();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_2 = rezervni_cidlo_2();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_3 = rezervni_cidlo_3();                                                       // pripadne spousteni mereni z rezervnich cidel
            int_rezerva_4 = rezervni_cidlo_4();                                                       // pripadne spousteni mereni z rezervnich cidel

            gps(2);                                                                                   // pokud je cas z GPS dostupny, provede se serizeni RTC vcetne pripadnych autokorekci
            
            znacka = 7;                                                                               // znacka "Ka-Pr" - posledni zaznam je prumer predchozich 10 hodnot
            if (znacka_stability == true)   bitSet (znacka,3);                                        // kdyz je hodnota stabilni, provede se zaznam do poznamkoveho bajtu
            if (again_zmena == true) byte_cnf = 255;                                                  // zajisti, ze ve vypisu budou pro IR, FULL, CNF a MS zobrazeny pomlcky
            priprava_pole_EEPROM();
            hranice_ukladani = 0;                                                                     //  i zprumerovany vysledek se uklada nezavisle na nastavene hranici pro ukladani
            zaznam_EEPROM();                                                                          //  zaznam se uklada do EEPROM a na SD kartu (pokud je zasunuta)
    
          }
        //--------------------------------------------------------

        if (param == 11)                                                                              // pro funkci urcovani viditelnosti DSO meri jas pozadi 
          {
            znacka_stability = test_stability(true,9);                                                // pred hlavnim merenim se provadi test stability mereni (2 az 5 pokusu) - neblika zadna tecka [9] (blika odpocet), pipa
    
            uint32_t suma_svetla = 0;                                                            // prumerovani mereni svetla
            uint16_t posledni_jas = 0;
            for (uint8_t vzorek = 1; vzorek <= prumery ; vzorek ++ )
              {

                tone_X(pin_bzuk,600, 10 , 1);                                                         // pri spusteni tlacitkem se pipa
                zobraz_cislo(prumery - vzorek, 3);                                                    //  ... pred kazdym merenim blikne zbyvajici pocet vzorku pro prumerovani ve stylu "-nn-"
                delay(100);
                
                int_svetlo = svetlo_1x(false,false);                                                  // pred merenim svetla se vzdycky displej zhasne
       
                if (vzorek == 1)                                                                      // pri prvnim vzorku se jeste nic netestuje - jen se nastavi pocatecni hodnoty
                  {                                                                                   //   se kterymi se bude porovnavat nasledujici vzorek
                    posledni_jas = korekce_svetla(int_svetlo);                                        // porovnavaji se hodnoty jasu zkorigovane pomoci kalibracni tabulky
                  }
                else
                  {                    
                    if ((rozdil_jasu(posledni_jas,korekce_svetla(int_svetlo)) / (float)korekce_svetla(int_svetlo)) > (rozhod_stab / 1000.0))      // porovnani rozdilu mezi sousednimi hodnotami
                      {
                        znacka_stability = false;                                                     //  nestabilni
                      }
                    posledni_jas = korekce_svetla(int_svetlo);
                  }

    
                suma_svetla = suma_svetla + int_svetlo;

              }

            if (znacka_stability == true)                                                             // po poslednim vzorku se pipne jinym tonem
              {
                tone_X(pin_bzuk,1000, 10 , 1);                                                        // kdyz bylo mereni stabilni, bzukne se jednim vysokym tonem
              }
            else
              {
                for (uint8_t bz = 0; bz < 3 ; bz++ )
                  {
                    tone_X(pin_bzuk,600, 10 , 1);                                                     // kdyz nebylo mereni stabilni, bzukne se 3x hlubsim tonem
                    delay(100);
                  }
              }                         
            
            int_svetlo = suma_svetla / prumery;                                                       // a vypocte se prumer v uint16_t formatu
            int_svetlo = korekce_svetla(int_svetlo);                                                  // vystupni hodnota je vcetne kalibracni korekce
                        
          }
        //--------------------------------------------------------



        if (param < 4 or param == 6)                                          // spusteni tlacitkem nebo automatem   
          {
            if (AUTOkom_linka > 0)                                            // automaticke odesilani dat do seriove linky po kazdem mereni
              {
                if (AUTOkom_linka == 2)     AUTOdata_USB();                   // odesle data do USB seriove linky               (0b10xxxxxx >> 6)
                if (AUTOkom_linka == 1)     AUTOdata_RS485();                 // odesle data do linky RS485 (pokud je osazena)  (0b01xxxxxx >> 6)
              }
            
          }
        
        dokoncene_mereni = 2;                                                                         // tento bajt se odesila po RS485 a je z nej videt, ze posledni mereni uz skoncilo
      }
     
  }

//----------------------------------------------
