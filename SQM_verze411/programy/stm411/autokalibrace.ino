// Podprogramy pro praci s externim kalibratorem (IR LED, W LED, servo)
//===================================================================================


uint16_t pom_svetlo;
uint8_t pozadavek_mA_W  =0;
uint8_t pozadavek_mA_IR =0;
int16_t kaluhel=0;



String txt_urovne[] =                   // pozadovane urovne proudu se zapisuji do CSV souboru
    {
       "0,0" ,      // [0]
       "0,5" ,      // [1]
       "1,0" ,      // [2]
       "1,5" ,      // [3]
       "2,0" ,      // [4]
       "2,5" ,      // [5]
       "3,0" ,      // [6]
       "3,5" ,      // [7]
       "4,0" ,      // [8]
       "4,5" ,      // [9]
       "5,0" ,      // [10]
       "6,0" ,      // [11]
       "7,0" ,      // [12]
       "8,0" ,      // [13]
       "9,0" ,      // [14]
      "10,0" ,      // [15]
      "manu."       // [16]         // pri rucnim nastavovani jasu pomoci joysticku na kalibratoru se do CSV souboru zapise znacka "manu."
      
    };

void kal_postupy(void)
  {
    kalibruje_se = true;
    bool prerusit = false;

    bool kal_tla_up = HIGH;
    bool kal_tla_dn = HIGH;
    bool kal_tla_ok = HIGH;
    bool kal_tla_ts = HIGH;

    uint8_t volba = 0b1111;

    uint16_t odpocet_kalibrace = 489;

    zobraz_text(3);                                                 // na displeji rozsviti napis "CALib"


    // nejdriv se z kalibratoru stahnou dig hodnoty pro nastaveni presnych proudu LED
    for (uint8_t uroven_proudu = 0 ; uroven_proudu  < 16 ; uroven_proudu ++)
      {
        cist_proudove_konstanty(uroven_proudu);
      }


    nastav_uroven_jasu_LED(1, 0);                                   // Infra LED se zhasne
    delay(1000);
    nastav_uroven_jasu_LED(0, 15);                                  // do bile LED na kalibratoru se posle proud, ktery v kalibracni tabulce odpovida 10mA (index 15)
    delay(1000);

    while (kaluhel != 90)                                           // ceka se, nez se spusti kalibrator a rucne se zafixuje na stredni pozici (90 stupnu)
      {
        cist_kalibrator();                                          // stahnout data
        delay(1000);
        kaluhel= pole_KALIB_I2C[0];                                 // aktualni uhel
        if (bitRead(pole_KALIB_I2C[9],4) == true)                   // kalibracni posdstavec ceka na rucni natoceni na nulovou polohu
          {
           zobraz_text(104);                                        // na displeji rozsviti napis "PuSH "     
          }
        else
          {
            zobraz_text(3);                                         // na displeji rozsviti napis "CALib"    
          }
      
      }


    zobraz_text(98);                                                // "rEAdy"                                                      (#411spec)
    

    prikaz_kalibrator(50, 7);                                       // na displeji kalibratoru se zobrazi napis "btn "
    delay(100);
    prikaz_kalibrator(50,102);                                      // displej na kalibratoru bude po zobrazeni uhlu automaticky zhasinat 


    prikaz_kalibrator(60 , 1);                                      // rozblikat laser
    delay(1000);

   // ----------------------
   //  Tady se ceka na mechanicke ustaveni polohy proti vysilaci LED.
   //  Potvrzeni spravneho ustaveni se provadi stiskem tlacitka.
   //-----------------------

    

    while ( volba  == 0b1111)                                       // dokud neni stisknuto nejake tlacitko, vyckava se
      {
        kal_tla_ok = digitalRead(pin_tl_ok);
        kal_tla_up = digitalRead(pin_tl_up);
        kal_tla_dn = digitalRead(pin_tl_dn);
        kal_tla_ts = digitalRead(pin_TS);                           //                                                              (#411spec)           
        volba = (kal_tla_ok << 3) + (kal_tla_up << 2) + (kal_tla_dn << 1) + kal_tla_ts;
        delay(30);              
      }


    prikaz_kalibrator(60 , 0);                                      // zhasnout laser
    delay(100);

    prikaz_kalibrator(50 , 1);                                      // na displeji kalibratoru rozsvitit napis "run "
    delay(100);

    zobraz_text(99);                                                // " run "                                                      (#411spec)
    delay(1000);                                                    // cas na uvolneni stisknuteho tlacitka  


    zalozeni_souboru();
    delay(1500);



    // =============================================================================================================================================================
    // ----------- test smerove charakteristiky ------------------   
    if (volba == 0b0111 or volba == 0b1011)                         // test se spousti tlacitkem [OK] nebo [UP]
      {
        soubor.println("\n\nSmerova charakteristika");
        soubor.println(";IR;full;config;pozadavek_W;DAC_W;pozadavek_IR;DAC_IR;proud_W;proud_IR;servo;W_0mA/IR_0mA;W_0.5mA/IR_0mA;W_2mA/IR_0mA;W_0mA/IR_1mA;W_0mA/IR_8mA");
        soubor.print("; ; ; ;[mA];[dig];[mA];[dig];[dig];[dig];[");
        soubor.write('\'');                       // nahreda za znak stupne (kod 176 dela pri zpetnem cteni souboru problemy)
        soubor.println("];[mmag/arcsec^2];[mmag/arcsec^2];[mmag/arcsec^2];[mmag/arcsec^2];[mmag/arcsec^2]");

        Serial.println("\n\nSmerova charakteristika");
        Serial.println(";IR;full;config;pozadavek_W;DAC_W;pozadavek_IR;DAC_IR;proud_W;proud_IR;servo;W_0mA/IR_0mA;W_0.5mA/IR_0mA;W_2mA/IR_0mA;W_0mA/IR_1mA;W_0mA/IR_8mA");
        Serial.print("; ; ; ;[mA];[dig];[mA];[dig];[dig];[dig];[");
        Serial.print(u8"\u00B0");          // znak stupen
        Serial.println("];[mmag/arcsec^2];[mmag/arcsec^2];[mmag/arcsec^2];[mmag/arcsec^2];[mmag/arcsec^2]");


        prikaz_kalibrator(20 , 1);                                  // zapnout napajeni serva

        zobraz_text(66);                                            // zhasnout displej                                              (#411spec)
        delay(2000);


        for (uint8_t profil = 0; profil < 5; profil ++)          // smycka s 3 ruznymi kombinacemi jasu LED, kazda kombinace se zapisuje do samostatneho sloupce
          {
            prikaz_kalibrator(10 , 20);                             // najet kousek pod vychozi polohu (20 stupnu), nasledne se dotahne na vychozi polohu 25 stupnu
            delay(4000);

            switch (profil)                                       // podle aktualniho kroku se nastavi jas LED
              {

                case 0:
                  nastav_uroven_jasu_LED(0, 0);                     // zhasne bilou LED (index 0)
                  pozadavek_mA_W = 0;
                  delay(1000);
                  nastav_uroven_jasu_LED(1, 0);                     // zhasnout IR
                  pozadavek_mA_IR = 0 ;
                  break;

                case 1:
                  nastav_uroven_jasu_LED(0, 1);                     // rozsvitit bilou LED proudem 0.5mA  (index 1)
                  pozadavek_mA_W = 1;
                  delay(1000);
                  nastav_uroven_jasu_LED(1, 0);                     // zhasnout IR
                  pozadavek_mA_IR = 0 ;
                  break;

                case 2:
                  nastav_uroven_jasu_LED(0, 4);                    // rozsvitit bilou LED proudem 2mA  (index 4)
                  pozadavek_mA_W = 4;
                  delay(1000);
                  nastav_uroven_jasu_LED(1, 0);                     // zhasnout IR
                  pozadavek_mA_IR = 0;
                  break;
      
                case 3:
                  nastav_uroven_jasu_LED(0, 0);                     // zhasnout bilou LED
                  pozadavek_mA_W = 0;
                  delay(1000);
                  nastav_uroven_jasu_LED(1, 2);                    // rozsvitit INFRA LED proudem 1mA  (index 2)
                  pozadavek_mA_IR = 2;
                  break;

                case 4:
                  nastav_uroven_jasu_LED(0, 0);                     // zhasnout bilou LED
                  pozadavek_mA_W = 0;
                  delay(1000);
                  nastav_uroven_jasu_LED(1, 13);                    // rozsvitit INFRA LED proudem 8mA  (index 13)
                  pozadavek_mA_IR = 13;
                  break;


                default:
                  nastav_uroven_jasu_LED(0, 0);                     // zhasnout bilou LED
                  pozadavek_mA_W = 0;
                  delay(1000);
                  nastav_uroven_jasu_LED(1, 0);                     // zhasnout infra LED
                  pozadavek_mA_IR = 0;
                  break;
              }
            delay(1000);
            zobraz_text(66);                                        // pred merenim zhasnout displej                                 (#411spec)
            delay(500);


            //--------------------
            for (cyk_uhel=25 ; cyk_uhel <= 155 ; cyk_uhel = cyk_uhel + 2)           // primy smer pohybu 25 az 155 stupnu probiha v zakladu po dvou stupnich (hrube)
              {
                if (cyk_uhel > 70 and cyk_uhel < 110) cyk_uhel = cyk_uhel - 1;          // v pasmu 71 az 109 stupnu se ale pricita jen po jednom stupni (jemne)
                

                if ((zrychlena_kalibrace == true) and (profil == 0 or profil == 3))     // pri zrychlene kalibraci se preskakuje profil 0 (tma) a profil 3 (infra LED na 1mA)
                  {
                    pom_svetlo = 0;                                   // nehybe se servem, an se nic nemeri
                  }
                else                                                  // pri normalni (kompletni) kalibraci nebo pri kalibraci profilu 1, 2, 4 se normalne meri
                  {
                    prikaz_kalibrator(10 , cyk_uhel);                 // najet na pozadovanou pozici (najizdi se jen jednim smerem od nizkych uhlu k vysokym)
                    delay(700);
                    zobraz_text(66);                                  // zhasnout displej                                               (#411spec)
                    prikaz_kalibrator(70 , 0);                        // aktualizuje mereni proudu pres LED
                  }
                
                zobraz_cislo(odpocet_kalibrace,0);                    // na displeji blikne odpocet
                delay(50);                                            // prodlouzeni zobrazeni odpoctu na displeji. napis se maze pri nasledujicim mereni jasu v podprogramu 'tisk_1_radky_csv()'

                tisk_1_radky_csv(profil);                             // v podprogramu se meri 1x jas, ze zmerenych parametru pak vytiskne 1 radku do souboru a do seriove linky (podle aktualniho kroku nastaveni jasu se pridavaji prazdne sloupce)

                odpocet_kalibrace --;


                if (digitalRead(pin_tl_ok) == LOW)                    // predcasne ukonceni smerove kalibrace tlacitkem [OK]
                  {
                    zobraz_text(12);                                  // "StoP "
                    while (digitalRead(pin_tl_ok) == LOW) delay(50);  // cekani na uvolneni [OK]
                    delay(100);
                    prerusit = true;
                    break;                                            // preruseni smycky uhlu
                  }    
                
              }                                                       // konec smycky uhlu
        
            if (prerusit == true)
              {
                break;                                              // preruseni smycky 3 kombinaci jasu
              }

            prikaz_kalibrator(10 , 20);                             // najet pod krajni polohu (20 stupnu)
            delay(5000);
            prikaz_kalibrator(10 , 25);                             // najet na krajni polohu (25 stupnu) s vymezenim vule prevodu
            delay(1000);

            soubor.close();
            SdFile::dateTimeCallback(dateTime);
            
            open_OK = soubor.open(cesta,  O_WRITE | O_APPEND | O_CREAT);
            if (open_OK == false)
              {
                zobraz_text(37);                                    // "5d-Er"
                while (true);                                       // zastaveni v nekonecny smycce
                  {
                    delay(10);
                  }
              }
          
          }                                                         // konec jasove smycky   
        

        while (digitalRead(pin_tl_ok) == LOW) delay(50);            // cekani na uvolneni OK tlacitka pro pripad predcasneho ukonceni mereni
        
        nastav_uroven_jasu_LED(0, 0);                               // zhasnout bilou LED
        delay(1000);    
        nastav_uroven_jasu_LED(1, 0);                               // zhasnout infra LED
        delay(1000);    
      }


    if (zrychlena_kalibrace == true)                                    // pri zrychlene kalibraci se preskakuje zaverecne mereni kombinaci jasu obou LED
      {
        prerusit = true;      
      }
    
    // =============================================================================================================================================================
    // -------------- kalibrace ruznych kombinaci jasu na stredni poloze (90 stupnich)
    if (volba == 0b0111 or volba == 0b1101)                         // test se spousti tlacitkem [OK] nebo [DN]
      {
        if (prerusit == false)                                      // pokud bylo v predchozim testu stisknuto tlacitko, nasledujici krok se preskakuje 
          {
            // tahle funkce ma zabudovanou pauzu pro pripadne upraveni opticke cesty
            // doplneni rozptylovace bodoveho svetla do plochy
            //  Po prenastaveni optiky a stisku tlacitka se pokracuje

            zobraz_text(98);                                           // "rEAdy"
            tone(pin_bzuk, 300, 30);                                        // kratke pipnuti pro upozorneni na pauzu

            volba = 0b1111;
            while ( volba  == 0b1111)                                       // dokud neni stisknuto nejake tlacitko, vyckava se
              {
                kal_tla_ok = digitalRead(pin_tl_ok);
                kal_tla_up = digitalRead(pin_tl_up);
                kal_tla_dn = digitalRead(pin_tl_dn);
                kal_tla_ts = digitalRead(pin_TS);                           //                                                              (#411spec)           
                volba = (kal_tla_ok << 3) | (kal_tla_up << 2) | (kal_tla_dn << 1) | kal_tla_ts;
                delay(30);              
              }
            zobraz_text(66);                                       // zhasnout displej
            delay(200);

            while (digitalRead(pin_tl_up) == LOW)   delay(50);     // cekani na uvolneni
            delay(500);

            
            soubor.println("\n\nKombinace ruznych jasu obou LED");

            datum_do_csv();
           
            soubor.println(";IR;full;config;pozadavek_W;DAC_W;pozadavek_IR;DAC_IR;proud_W;proud_IR;servo;jas");
            soubor.print("; ; ; ;[mA];[dig];[mA];[dig];[dig];[dig];[");
            soubor.write('\'');                       // znak stupne (kod 176 dela pri zpetnem cteni souboru)
            soubor.println("];[mmag/arcsec^2]");


            Serial.println("\n\nKombinace ruznych jasu obou LED");
            Serial.println(";IR;full;config;pozadavek_W;DAC_W;pozadavek_IR;DAC_IR;proud_W;proud_IR;servo;jas");
            Serial.print("; ; ; ;[mA];[dig];[mA];[dig];[dig];[dig];[");
            Serial.print(u8"\u00B0");          // znak stupen
            Serial.println("];[mmag/arcsec^2]");
       
            zobraz_text(8);                                         // "JA5 "
            delay(1000);    
        
            prikaz_kalibrator(10 , 85);                             // pres podjeti pres uhel 85 stupnu najede kvuli vymezeni vule v prevodech na stredovou polohu
            delay(2000);
            prikaz_kalibrator(10 , 90);                             // najet na stredni polohu 90 stupnu zdola
            delay(3000);

    //                                     0.0 , 0.5 , 1.0 , 1.5 ,  2.0 , 4.0 , 6.0 , 8.0 [mA]
            uint8_t pozadovane_wjasy[] = { 0  ,  1  ,  2  ,  3  ,   4  ,  8  ,  11 , 13};     // kombinace 8 ruznych urovni proudu pro bilou LED

    //                                     0.0 , 0.5 , 1.0 , 2.0 ,  4.0 , 6.0 , 8.0 , 10.0 [mA]
            uint8_t pozadovane_ijasy[] = { 0  ,  1  ,  2  ,  3  ,   8  ,  11 ,  13  , 15};     // kombinace 8 ruznych urovni proudu pro infra LED

            for (uint8_t wjas = 0; wjas < 8; wjas ++)                  // smycka s ruznymi kombinacemi jasu bile LED
              {
                delay(500);
                nastav_uroven_jasu_LED (0 , pozadovane_wjasy[wjas]); // ovladani bile LED
                delay(1000);
        
                for (uint8_t ijas = 0; ijas < 8; ijas ++)              // smycka s ruznymi kombinacemi jasu infra LED
                  {
                    nastav_uroven_jasu_LED (1 , pozadovane_ijasy[ijas]);   // ovladani infra LED
                    delay(1000);
                    prikaz_kalibrator(70 , 0);                          // aktualizuje mereni proudu pres LED
                    zobraz_text(66);                                //zhasnout displej                                               (#411spec)

                    pozadavek_mA_W = pozadovane_wjasy[wjas];
                    pozadavek_mA_IR = pozadovane_ijasy[ijas];
        
                    zobraz_cislo(odpocet_kalibrace,0);              // na displeji blikne odpocet
            
                    tisk_1_radky_csv(0);                            // ze zmerenych parametru vytiskne 1 radku do souboru a do seriove linky (zadne sloupce se v tomto pripade nepridavaji)

        
                    odpocet_kalibrace --;
        
                    if (digitalRead(pin_tl_ok) == LOW)              // predcasne ukonceni jasove kalibrace tlacitkem [OK]
                      {
                        zobraz_text(12);                            // "StoP "
                        while (digitalRead(pin_tl_ok) == LOW) delay(50);    // cekani na uvolneni [OK]
                        delay(100);
                        prerusit = true;
                        break;                                      // preruseni smycky pro infra LED
                      }    

                  }                                                 // konec vnitrni infra smycky
        
                if (prerusit == true)
                  {
                    prikaz_kalibrator(10 , 90);                     // najet na stredni polohu 90 stupnu
                    delay(3000);
                    break;                                          // preruseni smycky pro white LED
                  }
              }                                                     // konec vnejsi white smycky 
          }        

      }
    delay(1000);

   
    // =============================================================================================================================================================
    // -------------- zaverecne operace
    
   


    konec_souboru();

    prikaz_kalibrator(20 , 0);                                  // vypnout napajeni serva
    delay(1000);   
    prikaz_kalibrator(50 , 2);                                  // napis "StoP" na kalibratoru
    zobraz_text(12);                                                // "StoP "



    tone(pin_bzuk, 300, 30);                                        // kratke pipnuti po ukonceni testu

    while (true) ;                                                  // nekonecna smycka po dokonceni mereni cekajici na reset        (#411spec)


  } 












//----------------------------------------------
// stazeni 10 bajtu z kalibratoru a ulozeni hodnot do pole
//
//         [0] = uhel natoceni (0 az 180, stredova poloha je 90) - pozn. na displeji se zobrazuje (hodnota - 90 stupnu), takze stredova poloha je 0
//         [1] = hodnota D/A prevodniku pro bilou LED MSB
//         [2] = hodnota D/A prevodniku pro bilou LED LSB
//         [3] = hodnota D/A prevodniku pro infra LED MSB
//         [4] = hodnota D/A prevodniku pro infra LED LSB
//         [5] = skutecny proud bilou LED - merene napeti na snimacim odporu na prvnim analogovem vstupu (defautne PA2) MSB
//         [6] = skutecny proud bilou LED - merene napeti na snimacim odporu na prvnim analogovem vstupu (defautne PA2) LSB
//         [7] = skutecny proud infra LED - merene napeti na snimacim odporu na druhem analogovem vstupu (defautne PA3) MSB
//         [8] = skutecny proud infra LED - merene napeti na snimacim odporu na druhem analogovem vstupu (defautne PA3) LSB
//         [9] = info / status 
//                bit0: 1=napajeni serva zapnuto ; 0=napajeni serva vypnuto
//                bit1: 1=lasery zapnute ; 0=lasery vypnute
//                bit2: stav fotozavory
//                bit3: 1=displej neco zobrazuje; 0=displej je zhasnuty
//                bit4: 1="PUSH" (ceka na rucni nastaveni nulove polohy); 0=neceka na nulovovou polohu
//                bit5: nepouzito
//                bit6: nepouzito
//                bit7: vzdycky 1 (hlavne kvuli vypisu v binarnim stavu do seriove linky, aby se nemusely doplnovat uvodni nuly)

void cist_kalibrator(void)
  {
    uint8_t i;

    for (i = 0 ; i < 10 ; i++)                                       // nejdriv smazat vsechna predchozi data (naplni se hodnotou 255)
      {
        pole_KALIB_I2C[i] = 255;
      }
    
    prikaz_kalibrator(100 , 0);
    
    i = 0;

    Wire.requestFrom(I2C_ADDR_KALIB,10);                             // zadost o 10 bajtu (registru)
    delay(12);
    while (Wire.available())                                        // postupne ulozeni prijatych hodnot do pole
      {
        pole_KALIB_I2C[i] = Wire.read();
        i++;
      }     
  }
//----------------------------------------------
// na pozadovanem kanalu posle do LED proud podle tabulky
//      "[0]  =  0.0mA"
//      "[1]  =  0.5mA"
//      "[2]  =  1.0mA"
//      "[3]  =  1.5mA"
//      "[4]  =  2.0mA"
//      "[5]  =  2.5mA"
//      "[6]  =  3.0mA"
//      "[7]  =  3.5mA"
//      "[8]  =  4.0mA"
//      "[9]  =  4.5mA"
//      "[10] =  5.0mA"
//      "[11] =  6.0mA"
//      "[12] =  7.0mA"
//      "[13] =  8.0mA"
//      "[14] =  9.0mA"
//      "[15] = 10.0mA"
// vyzaduje, aby v poli kalibracnich proudu uz byly natazene hodnoty z kalibratoru pro vsechny pozadovane proudy
//      musel uz probehnout podporogram "cist_proudove_konstanty()"

void nastav_uroven_jasu_LED(uint8_t kal_kanal, uint8_t kal_uroven)
  {
    if (kal_kanal == 0)                            // bila LED
      {
        prikaz_kalibrator(30, (pole_proudy_kalib[(kal_uroven*4) + 0] * 256) + (pole_proudy_kalib[(kal_uroven*4) + 1]));
      }
    if (kal_kanal == 1)                            // infra LED
      {
        prikaz_kalibrator(40, (pole_proudy_kalib[(kal_uroven*4) + 2] * 256) + (pole_proudy_kalib[(kal_uroven*4) + 3]));
      }
  }


//----------------------------------------------
// stazeni 4 bajtu z kalibratoru s proudovymi konstantami a ulozeni do pole "pole_proudy_kalib[]"
void cist_proudove_konstanty(uint8_t index_pole)
  {
    uint8_t i;

    for (i = 0 ; i < 4 ; i++)                                       // nejdriv smazat vsechna predchozi data (naplni se hodnotou 255)
      {
        pole_proudy_kalib[i + (4*(index_pole))] = 255;
      }
    
    prikaz_kalibrator(110 , index_pole);
    
    i = 4 * index_pole;

    Wire.requestFrom(I2C_ADDR_KALIB,4);                             // zadost o 4 bajty (registry)
    delay(12);
    while (Wire.available())                                        // postupne ulozeni prijatych hodnot do pole
      {
        pole_proudy_kalib[i] = Wire.read();
        i++;
      }    

  
  }
//----------------------------------------------


void tisk_1_radky_csv(uint8_t i_profil)
  {
    uint16_t wjas_z_KAL;
    uint16_t ijas_z_KAL;
    uint16_t wproud_z_KAL;
    uint16_t iproud_z_KAL;

    if ((zrychlena_kalibrace == true) and (i_profil == 0 or i_profil == 3))     // pri zrychlene kalibraci se preskakuje profil 0 (tma) a profil 3 (infra LED na 1mA)
      {
        wjas_z_KAL = 0;
        ijas_z_KAL = 0;
        wproud_z_KAL = 0;
        iproud_z_KAL = 0;
        pole_KALIB_I2C[0] = cyk_uhel;     // uhel natoceni z globalni promenne
        
      }
    else                                                   // pri normalni (kompletni) kalibraci nebo pri kalibraci profilu 1, 2, 4 se normalne meri
      {
        cist_kalibrator();                                  // stahnout data
        wjas_z_KAL = ((pole_KALIB_I2C[1]) * 256) + (pole_KALIB_I2C[2]);             // nastavene hodnoty jasu
        ijas_z_KAL = ((pole_KALIB_I2C[3]) * 256) + (pole_KALIB_I2C[4]);
    
        wproud_z_KAL = ((pole_KALIB_I2C[5]) * 256) + (pole_KALIB_I2C[6]);           // skutecne zmerene proudy (v digitech) 
        iproud_z_KAL = ((pole_KALIB_I2C[7]) * 256) + (pole_KALIB_I2C[8]);
    
        pom_svetlo = svetlo_1x(false,false);

      }






    
    soubor.print(';');                                  // prvni strednik pro odsazeni tabulky o jeden sloupec
    soubor.print(int_infra);
    soubor.print(';');
    soubor.print(int_full);
    soubor.print(';');
    soubor.print(byte_cnf);
    soubor.print(';');

    soubor.print(txt_urovne[pozadavek_mA_W]);       // pozadovany proud bilou LED (jako text)
    soubor.print(';');
    soubor.print(wjas_z_KAL);                       // nastaveny jas bile LED
    soubor.print(';');

    
    soubor.print(txt_urovne[pozadavek_mA_IR]);      // pozadovany proud infra LED (jako text)
    soubor.print(';');
    soubor.print(ijas_z_KAL);                       // nastaveny jas infra LED

    soubor.print(';');
    soubor.print(wproud_z_KAL);                     // skutecne zmereny proud bile LED
    soubor.print(';');
    soubor.print(iproud_z_KAL);                     // skutecne zmereny proud infra LED


    soubor.print(';');
    soubor.print(pole_KALIB_I2C[0]-90);                 // zapisovany uhel natoceni se prepocita tak, aby 90 stupnu ve skutecnosti bylo 0 stupnu ve vyslednem grafu
    soubor.print(';');

    for (uint8_t stredniky = 0; stredniky < i_profil ; stredniky ++)   // za uhel se vlozi strednik, ktery posouva v CSV souboru bloky cyklu (pak je snazsi delat oddelene X-Y grafy pro kazdy pruchod)
      {
        soubor.print(';');
      }

    soubor.println(pom_svetlo);                         // jas prevedeny na [mmag/arcsec2]

    Serial.print(';');                                  // prvni strednik pro odsazeni tabulky o jeden sloupec
    Serial.print(int_infra);
    Serial.print(';');
    Serial.print(int_full);
    Serial.print(';');
    Serial.print(byte_cnf);
    Serial.print(';');

    Serial.print(txt_urovne[pozadavek_mA_W]);       // pozadovany proud bilou LED (jako text)
    Serial.print(';');
    Serial.print(wjas_z_KAL);                       // nastaveny jas bile LED
    Serial.print(';');

    
    Serial.print(txt_urovne[pozadavek_mA_IR]);      // pozadovany proud infra LED (jako text)
    Serial.print(';');
    Serial.print(ijas_z_KAL);                       // nastaveny jas infra LED

    Serial.print(';');
    Serial.print(wproud_z_KAL);                     // skutecne zmereny proud bile LED
    Serial.print(';');
    Serial.print(iproud_z_KAL);                     // skutecne zmereny proud infra LED


    Serial.print(';');
    Serial.print(pole_KALIB_I2C[0]-90);                 // zapisovany uhel natoceni se prepocita tak, aby 90 stupnu ve skutecnosti bylo 0 stupnu ve vyslednem grafu
    Serial.print(';');

    for (uint8_t stredniky = 0; stredniky < i_profil ; stredniky ++)   // za uhel se vlozi strednik, ktery posouva v CSV souboru bloky cyklu (pak je snazsi delat oddelene X-Y grafy pro kazdy pruchod)
      {
        Serial.print(';');
      }

    Serial.println(pom_svetlo);                             // jas prevedeny na [mmag/arcsec2]
    
  }


void  datum_do_csv(void)
  {
    zobraz_RTC(false);                                      // nastaveni casovych promennych LOC... podle casu v RTC        (#411spec)

    soubor.print(LOC_den);
    soubor.print('.');
    soubor.print(LOC_mes);
    soubor.print('.');
    soubor.print(LOC_rok);
    soubor.print(' ');
    if (LOC_hod < 10)   soubor.print('0'); 
    soubor.print(LOC_hod);
    soubor.print(':');
    if (LOC_min < 10)   soubor.print('0'); 
    soubor.print(LOC_min);
    soubor.print(':');
    if (LOC_sek < 10)   soubor.print('0'); 
    soubor.println(LOC_sek);



    Serial.print(LOC_den);
    Serial.print('.');
    Serial.print(LOC_mes);
    Serial.print('.');
    Serial.print(LOC_rok);
    Serial.print(' ');
    if (LOC_hod < 10)   Serial.print('0'); 
    Serial.print(LOC_hod);
    Serial.print(':');
    if (LOC_min < 10)   Serial.print('0'); 
    Serial.print(LOC_min);
    Serial.print(':');
    if (LOC_sek < 10)   Serial.print('0'); 
    Serial.println(LOC_sek);


    
  }






// odeslani prikazu do kalibratoru (bez navratove hodnoty)
void prikaz_kalibrator(uint8_t kalib_prikaz, uint16_t kalib_parametr)
  {
    Wire.beginTransmission(I2C_ADDR_KALIB);
    delay(12);
    Wire.write(kalib_prikaz);
    delay(12);

    Wire.write(kalib_parametr / 256);
    delay(12);
    Wire.write(kalib_parametr % 256);
    delay(12);
    
    Wire.endTransmission();    
  }




//----------------------------------------------
// test prevzaty z I2C skeneru, jestli se na I2C adrese kalibratoru vubec neco hlasi
bool KAL_test(void)
  {
    Wire.beginTransmission (I2C_ADDR_KALIB);
    if (Wire.endTransmission () == 0)  return true;
    else                               return false;
     
  }



void zalozeni_souboru(void)
  {    

    uint8_t CALIB_poradova_znacka = EEPROM_read(eeaddr_CALIB_znacka) + 1;
    EEPROM_write(eeaddr_CALIB_znacka , CALIB_poradova_znacka);
    
    char jmeno_slozky[] = "CALB";                                   // retezec pro konstrukci nazvu slozky
//    char cesta[] = "CALB/123-1025.csv";                             // retezec pro konstrukci cele cesty k souboru    
    
    zobraz_RTC(false);                                              // nastaveni casovych promennych LOC... podle casu v RTC        (#411spec)


    cesta[5] = 48 +  (CALIB_poradova_znacka / 100);
    cesta[6] = 48 +  (CALIB_poradova_znacka % 100) / 10 ;
    cesta[7] = 48 +  (CALIB_poradova_znacka %  10);
    cesta[9] = 48 + (LOC_mes / 10);
    cesta[10] = 48 + (LOC_mes % 10);
    cesta[11] = 48 + (LOC_den / 10);
    cesta[12] = 48 + (LOC_den % 10);


    if (digitalRead(pin_karta_IN) == LOW)                                // LOW = karta zasunuta, muze se provest pokus o zapis
      {

        sd.begin(SD_CONFIG);                                            //inicializace SD karty pro pripad, ze by byla predtim vytazena (#411spec)

        if (!sd.exists(jmeno_slozky))   sd.mkdir(jmeno_slozky);
        
        
        SdFile::dateTimeCallback(dateTime);
        
        open_OK = soubor.open(cesta,  O_WRITE | O_APPEND | O_CREAT);
        delay(1500);
    
        soubor.print("Soubor: SD:\\");
        cesta[4] = '\\';
        soubor.println(cesta);
        cesta[4] = '/';

        datum_do_csv();

        soubor.print("ID: 0x");
        soubor.println(ID_char);

        soubor.println(verzeSW);

        soubor.print("Teplota: ");
        soubor.print(citelna_teplota(teplota(true)));
        soubor.write('\'');                       // znak stupne (zapsani kodu 176 zpusobuje problemy v seriovem terminalu - spatne kodovani)
        soubor.println('C');
      }
    else
      {
        prikaz_kalibrator(50,6);     // zobrazit na displeji podstavce napis "noSd"
        delay(1000);
      }
      


    

  }


// uzavre otevreny soubor
void konec_souboru(void)
  {

    soubor.print("\nKonec testu: ");                                // odradkovani od predchozi tabulky 

    datum_do_csv();      
    soubor.close();

    // zapis posledniho jmena souboru do seznamu ukladanych souboru
    zobraz_RTC(false);                                              // nastaveni casovych promennych LOC... podle casu v RTC        (#411spec)
    SdFile::dateTimeCallback(dateTime);
    open_OK = soubor.open("CALB/seznam.txt",  O_WRITE | O_APPEND | O_CREAT);             // postupne ukladani vsech nazvu souboru. posledni je na konci souboru
    delay(20);
    soubor.println(cesta);
    soubor.close();
    
    
  }






//================================================================================================================================================
// Ovladaci I2C prikazy kalibratoru
//   1. bajt je prikaz:         10 = pohyb serva
//                              20 = ovladani napajeni serva
//                              30 = ovladani bile LED
//                              40 = ovladani infra LED
//                              50 = ovladani displeje (napisy / automaticke zhasinani)
//                              60 = ovladani LASERU
//                              70 = aktualizovat mereni proudu
//                             100 = pozadavek o data z kalibratoru
//                             110 = pozadavek o kalibracni proudove konstanty z kalibratoru
//
//  2. a 3. bajt je parametr (vzdycky 2 bajty v poradi MSB a LSB)
//                             pro prikaz 10 ........... 0 az 180 = uhel natoceni serva
//                             pro prikaz 20 ........... 0 = vypnout napajeni serva
//                                                       1 = zapnout napajeni serva
//                             pro prikazy 30 a 40  .... 0 az 1023 = ovladani 10-bitoveho D/A prevodniku
//                             pro prikaz 50 ........... 0 az 20 = predpripravene napisy na displeji
//                                                       100 = displej bude pri zobrazeni uhlu nebo jasu trvale zhasnuty
//                                                       101 = displej bude pri zobrazeni uhlu nebo jasu trvale rozsviceny
//                                                       102 = displej pri zobrazeni uhlu nebo jasu jen na chvilku blikne
//                             pro prikaz 60 ........... 0 = lasery zhasnute
//                                                       1 = lasery blikaji
//                             pro prikaz 70 ........... 0 = aktualizovat mereni proudu (jen se znova nactou hodnoty na A/D vstupech a zapisi se do pole)
//                             pro prikaz 100 .......... 0 = stahnout pole s aktualnimi hodnotami (kazdy index pole je jeden bajt) 
//                                                          [0] = uhel natoceni (0 az 180, stredova poloha je 90) - pozn. na displeji se zobrazuje hodnota - 90 stupnu, takze stredova poloha je 0
//                                                          [1] = hodnota D/A prevodniku pro bilou LED MSB
//                                                          [2] = hodnota D/A prevodniku pro bilou LED LSB
//                                                          [3] = hodnota D/A prevodniku pro infra LED MSB
//                                                          [4] = hodnota D/A prevodniku pro infra LED LSB
//                                                          [5] = skutecny proud bilou LED - merene napeti na snimacim odporu na prvnim analogovem vstupu (defautne PA2) MSB
//                                                          [6] = skutecny proud bilou LED - merene napeti na snimacim odporu na prvnim analogovem vstupu (defautne PA2) LSB
//                                                          [7] = skutecny proud infra LED - merene napeti na snimacim odporu na druhem analogovem vstupu (defautne PA3) MSB
//                                                          [8] = skutecny proud infra LED - merene napeti na snimacim odporu na druhem analogovem vstupu (defautne PA3) LSB
//                             pro prikaz 110 .......... 1 = stahnout pole s kalibracnimi hodnotami pro 0,5mA
//                                                       2 = stahnout pole s kalibracnimi hodnotami pro 1mA
//                                                       3 = stahnout pole s kalibracnimi hodnotami pro 1,5mA
//                                                       4 = stahnout pole s kalibracnimi hodnotami pro 2mA
//                                               ... az 15 = stahnout pole s kalibracnimi hodnotami pro 10mA
//                                                         vysledky se vraci ve 4-bajtovem poli               
//                                                          [0] = MSB pro pozadovany proud bilou LED
//                                                          [1] = LSB pro pozadovany proud bilou LED
//                                                          [2] = MSB pro pozadovany proud infra LED
//                                                          [3] = LSB pro pozadovany proud infra LED
