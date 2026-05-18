// Zjednodusene menu ovladane 3 tlacitky se signalizaci pomoci RGB LED
//================================================

#define AUTOEXITMENULED        30000                                   //  po jake dobe necinnosti se menu automaticky opusti (cislo udava pocet smycek, jedna smycka trva 10ms) 

uint16_t auto_exit_LED_menu;                                       // v teto promenne se bude odecitat doba necinnosti
uint32_t trvani_LED_stisku;                                       // doba drzeni tlacitka OK v milisekundach v hlavnim menu
uint32_t trvani_LED_stisku2;                                      // doba drzeni tlacitka OK v milisekundach v podmenu


int16_t pozice_LED_menu1 = 1;                                              // nalistovana pozice v hlavnim menu



//----------------------------------------------
// listovani v hlavnim menu a volba polozky
void menu_LED(void)
  {
    LED_R_onoff(true);                                                 // cervena LED signalizuje vstup do menu (dlouhym stiskem tlacitka OK)
    uint8_t pamet_signal_LED = signal_LED;                                // v menu se vypinaji ostatni signalizace LED (baterie, chyby, komunikace)
    signal_LED = 0;

    
    auto_exit_LED_menu = AUTOEXITMENULED;                              // po 5 minutach bez stisku tlacitka se menu automaticky ukonci

    while (digitalRead(pin_tl_ok) == LOW)                              // do hlavniho menu se vstoupilo dlouhym stiskem tlacitka OK
      {                                                                //  ted je nutne pockat na jeho uvolneni
        delay(20);
      }

    LED_R_onoff(false);
    
    delay(50);

    uint32_t casovani_menuLED_1 = millis();                       // zacatek dalsiho vyblikani zvolene polozky (okamzite)
    bool blikej = true;
    uint8_t pomprom_menu_LED = pozice_LED_menu1;                          // pomocna promenna kvuli vyblikavani polozek v menu

    while (auto_exit_LED_menu)                                         // z menu se vypadne vyprsenim casovace (beznym, nebo vynucenym)
      {
        interrupty();                                                  // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni

        if (blikej == true)
          {
            if (pomprom_menu_LED > 0)
              {
                blik_F_fast();
                pomprom_menu_LED --;
              }
            else
              {
                blikej = false;
                pomprom_menu_LED = pozice_LED_menu1;
                casovani_menuLED_1 = millis() + 1000;                 // dalsi blikani polozky zacne za sekundu
              }            
          }
          
        if (millis() > casovani_menuLED_1)                            // vyprsela pauza pred dalsim vyblikanim polozky menu (obvykle sekundu po ukonceni predchoziho blikani)
          {
            blikej = true;
          }


        
        if (digitalRead(pin_tl_dn) == LOW)                             // listovani dolu v hlavnim menu
          {
            delay(50);                                                 // odruseni zakmitu
            pozice_LED_menu1 --;                                       // listovani v menu dolu
            if (pozice_LED_menu1 < 1) pozice_LED_menu1 = 1;            //   se zarazkou na prvni polozce
            pomprom_menu_LED = pozice_LED_menu1;
            while (digitalRead(pin_tl_dn) == LOW)                      // dokud je tlacitko stisknute, nic se nedeje (jen sviti cervena LED)
              {
                LED_R_onoff(true);
                delay(10);                           
              }
            delay(50);                                                 // odruseni zakmitu pri uvolneni tlacitka
            LED_R_onoff(false);                                        // zhasnuti cervene LED po uvolneni tlacitka
            delay(300);
            auto_exit_LED_menu = AUTOEXITMENULED;                      // obcerstveni automatickeho vypadnuti z menu
          }


        if (digitalRead(pin_tl_up) == LOW)                             // listovani nahoru v hlavnim menu
          {
            delay(50);                                                 // odruseni zakmitu
            pozice_LED_menu1 ++;                                       // listovani v menu dolu
            if (pozice_LED_menu1 > 14) pozice_LED_menu1 = 14;          //   se zarazkou na posledni polozce
            pomprom_menu_LED = pozice_LED_menu1;
            while (digitalRead(pin_tl_up) == LOW)                      // dokud je tlacitko stisknute, nic se nedeje (jen sviti cervena LED)
              {
                LED_R_onoff(true);
                delay(10);                           
              }
            delay(50);                                                 // odruseni zakmitu pri uvolneni tlacitka
            LED_R_onoff(false);                                        // zhasnuti cervene LED po uvolneni tlacitka
            delay(300);
            auto_exit_LED_menu = AUTOEXITMENULED;                      // obcerstveni automatickeho vypadnuti z menu
          }


 
      
    
        if (digitalRead(pin_tl_ok) == LOW)                             // potvrzovaci tlacitko
          {
            LED_R_onoff(true);
            delay(10);                                                 // odruseni zakmitu
            uint32_t startTime = millis();
            while (digitalRead(pin_tl_ok) == LOW)                      // dokud je tlacitko stisknute, nic se nedeje
              {
                delay(50);
                trvani_LED_stisku = millis() - startTime;
                if (trvani_LED_stisku > 500)                           // stisk trval dele nez 0,5 sekundy
                  {
                    LED_R_onoff(false);                                // po pul sekunde se zhasne cervena LED
                    podmenu_LED(pozice_LED_menu1);                     // skoci se do podmenu - pri navratu z menu zustava nastavena promenna 'trvani_stisku' na vysokou hodnotu, takze se hlavni menu neopusti
                  }
              }

            if (trvani_LED_stisku <= 500)                              // stisk tlacitka OK byl kratsi, nez 0,5 sekundy
              {
                LED_R_onoff(false);                                    // zhasnuti cervene LED
                auto_exit_LED_menu = 1;                                // pri kratkem stisku se opusti hlavni menu    
                delay(200);                                            // pauza pred opustenim menu
              }
            
          }


        
        auto_exit_LED_menu --;
        delay(10);
      
      }                                                                // kdyz je auto_exit_LED_menu = 0, menu se opousti

    signal_LED = pamet_signal_LED;                                     // signalizacni LED se vraci do signalizace puvodnich funkci (baterie, komunikace, chyby)
  }
//----------------------------------------------



//----------------------------------------------
// Jedna z polozek ve zkracenem menu
void podmenu_LED(uint8_t pozice)
  {

    // do podmenu se vstoupilo dlouhym stiskem tlacitka OK, okamzite jeste pred uvolnenim tlacitka OK se zacne vykonavat podmenu

    if (pozice == 1)                                                   // posledni zmerena hodnota
      {
        int16_t pomprom = (pole_EEPROM[5] * 256) + pole_EEPROM[6];
        vyblikej_cislo(pomprom);
      }


    if (pozice == 2)                                                   // kalibracni mereni
      {
        while (digitalRead(pin_tl_ok) == LOW)                          // cekani na uvolneni OK tlacitka
          { 
            LED_R_onoff(true);
            delay(20);
          }
        LED_W(false);
        mereni(7);                                                     // prumeruje 10 vzorku, ale vystup vraci bez korekce
      }


    if (pozice == 3)                                                   // vyblikani aktualniho casu
      {
        zobraz_RTC(false);
        int16_t pomprom = (LOC_hod * 100) + LOC_min;
        vyblikej_cislo(pomprom);        
      }

      
    if (pozice == 4)                                                   // zmena casove zony SEC/SELC
      {

        while (digitalRead(pin_tl_ok) == LOW)                          // cekani na uvolneni OK tlacitka
          { 
            LED_R_onoff(true);
            delay(20);
          }
        delay(50);
        LED_R_onoff(false);                                            // zhasnuti cervene LED
        
        int16_t pomprom = EEPROM_read(eeaddr_leto_zima);
        while (digitalRead(pin_tl_ok) == HIGH)                         // z podmenu zmeny zony se vraci stiskem tlacitka OK - dokud neni stisknute, beha tato smycka
          {
            delay(50);
            if (digitalRead(pin_tl_up) == LOW)   pomprom = 1;          // tlacitko nahoru nastavuje SELC
            if (digitalRead(pin_tl_dn) == LOW)   pomprom = 0;          // tlacitko dolu nastavuje SEC

            if (millis()/100 % 30 == 1)
              {
                if (pomprom == 0)  blik_C_kratky();                    //  zima (svetle modre problikavani)
                else               blik_Y_kratky();                    //  leto (zlute problikavani)
              }            
          }
                                                                       //  [OK] bylo stisknuto
        EEPROM_update(eeaddr_leto_zima,pomprom);                       // posledni zvolena hodnota se ulozi do EEPROM
                                                                       // a nastavi se promenna 'leto'
          
        if (pomprom == 1)         leto = true;
        else                      leto = false;

        
        while (digitalRead(pin_tl_ok) == LOW)                          // cekani na uvolneni OK tlacitka
          { 
            LED_R_onoff(true);
            delay(20);
          }
        delay(20);
        LED_R_onoff(false);      
      }

    if (pozice == 5)                                                   // vyblikani prumerovani
      {
        vyblikej_cislo(prumery);        
      }

    if (pozice == 6)                                                   // vyblikani intervalu automatickeho mereni
      {
        vyblikej_cislo(automat);        
      }

    if (pozice == 7)                                                   // vyblikani teploty zaokrouhlene na cele stupne Celsia
      {
        if (senzor_BME == true)
          {
            vyblikej_cislo(round(citelna_teplota(teplota(true))));        
          }
        else
          {
            vyblikej_cislo(99);                                        // kdyz neni cidlo povolene, vyblika se 99        
          }
      }

    if (pozice == 8)                                                   // vyblikani tlaku v Pa primo z cidla BME
      {
        if (senzor_BME == true)
          {
            vyblikej_cislo(cidlo_BME.readPressure());                  // tlak NENI upravovan, aby se vesel do dvou bajtu - klidne se vyblika 100kPa
          }
        else
          {
            vyblikej_cislo(0);                                         // kdyz neni cidlo povolene, vyblika se 0        
          }
      }

    if (pozice == 9)                                                   // vyblikani vlhkosti v [%] primo z cidla BME
      {
        if (senzor_BME == true)
          {
            vyblikej_cislo(round(cidlo_BME.readHumidity()));               // zaokrouhleni na cela [%]
          }
        else
          {
            vyblikej_cislo(0);                                             // kdyz neni cidlo povolene, vyblika se 0
          }
      }


    if (pozice == 10)                                                  // vyblikani uhlu naklonu
      {
        if (modul_LSM303DLHC == true)
          {
            vyblikej_cislo(uhel());                                    // vyblikani uhlu (muze byt i zaporny)        
          }
        else
          {
            vyblikej_cislo(0);                                         // kdyz neni cidlo povolene, vyblika se 0
          }
      }


    if (pozice == 11)                                                  // vyblikani azimutu
      {
        if (modul_LSM303DLHC == true)
          {
            vyblikej_cislo(zjisti_azimut());                           // azimut se vyblika normalne v celych stupnich 0 az 359 
          }
        else
          {
            vyblikej_cislo(0);                                         // kdyz neni cidlo povolene, vyblika se 0
          }
      }


    if (pozice == 12)                                                  // vyblikani stavu baterie v [%] (9V = 100%, 4V = 0%)
      {
        cti_napajeni();
        long pomprom = constrain(mapfloat(napeti_9V,4,9,0,100),0,100);             // premapovani na 0 az 100%
        if (osazeno_uref == true)
          {
            vyblikej_cislo(pomprom);
          }
        else                                                           // kdyz neni osazena reference, vyblika se jen 0
          {
            vyblikej_cislo(0);
          }
        
      }


    if (pozice == 13)                                                  // vyblikani elevace Slunce (muze byt i zaporna)
      {
        astro_vypocty();
        vyblikej_cislo(Slu_elevace - 90);                              // v podprogramu se elevace zvysuje, aby byla vzdycky kladna, tady se zase musi odecist, aby mohla byt i zaporna
      }


    if (pozice == 14)                                                  // vyblikani elevace Mesice (muze byt i zaporna)
      {
        astro_vypocty();
        vyblikej_cislo(Mes_elevace - 90);                              // v podprogramu se elevace zvysuje, aby byla vzdycky kladna, tady se zase musi odecist, aby mohla byt i zaporna
      }

    delay(1000);
    auto_exit_LED_menu = AUTOEXITMENULED;                              // po provedeni cinnosti v podmenu se vypadne z podmenu do hlavniho menu (polozka blika zelene)
  }
//----------------------------------------------
