// V pripade, ze neni pouzity displej, je umoznena omezena signalizace pomoci 3-barevne NeoPixelove LED
//================================================

//----------------------------------------------
void blik_R(void)
  {          //GGRRBB
    uint32_t jas_red = (logaritmy_jasu[jas_displeje] << 8);     // cervena cast jasu LED se posila na prostrednich 8 bitech
    strip.setPixelColor(0, jas_red);                            // RED
    strip.show();

    delay(40);
    strip.setPixelColor(0, 0x000000);                           // zhasnout
    strip.show();
    delay(500);
    barva_LED = 0;
  }
//----------------------------------------------



//----------------------------------------------
void blik_G(void)
  {          //GGRRBB
    uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16);  // zelena cast jasu LED se posila na levych 8 bitech
    strip.setPixelColor(0, jas_green);                          // GREEN
    strip.show();
    delay(40);

    strip.setPixelColor(0, 0x000000);                           // zhasnout
    strip.show();
    delay(800);
    barva_LED = 0;
  }
//----------------------------------------------



//----------------------------------------------
void blik_G_fast(void)
  {          //GGRRBB
    uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16);  // zelena cast jasu LED se posila na levych 8 bitech
    strip.setPixelColor(0, jas_green);                          // GREEN
    strip.show();
    delay(80);

    strip.setPixelColor(0, 0x000000);                           // zhasnout
    strip.show();
    delay(300);
    barva_LED = 0;
  }
//----------------------------------------------


//----------------------------------------------
void blik_F_fast(void)
  {          //GGRRBB

    uint32_t jas_red  = (logaritmy_jasu[jas_displeje] << 8);  // cervena cast jasu LED se posila na prostrednich 8 bitech
    uint32_t jas_blue = (logaritmy_jasu[jas_displeje]);       // modra cast jasu LED se posila na pravych (nejnizsich) 8 bitech
    strip.setPixelColor(0, (jas_red | jas_blue));             // RED | BLUE    
    strip.show();
    delay(80);

    strip.setPixelColor(0, 0x000000);                           // zhasnout
    strip.show();
    delay(300);
    barva_LED = 0;
  }
//----------------------------------------------



//----------------------------------------------
void blik_B(void)
  {          //GGRRBB
    uint32_t jas_blue = (logaritmy_jasu[jas_displeje]);         // modra cast jasu LED se posila na pravych 8 bitech
    strip.setPixelColor(0, jas_blue);                           // BLUE
    strip.show();
    delay(40);

    strip.setPixelColor(0, 0x000000);                           // zhasnout
    strip.show();
    delay(500);
    barva_LED = 0;
  }
//----------------------------------------------



//----------------------------------------------
void blik_C(void)
  {          //GGRRBB
    uint32_t jas_blue  = (logaritmy_jasu[jas_displeje] );       // modra cast jasu je ve spodnich (pravych) 8 bitech
    uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16);  // zelena cast jasu LED se posila na levych 8 bitech
    strip.setPixelColor(0, (jas_blue | jas_green));             // BLUE | GREEN
    strip.show();
    delay(1000);

    strip.setPixelColor(0, 0x000000);                           // zhasnout
    strip.show();
    delay(1000);
    barva_LED = 0;
  }
//----------------------------------------------



//----------------------------------------------
void blik_Y(void)
  {          //GGRRBB
    uint32_t jas_red   = (logaritmy_jasu[jas_displeje] << 8);   // cervena cast jasu LED se posila na prostrednich 8 bitech
    uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16);  // zelena cast jasu LED se posila na levych 8 bitech
    strip.setPixelColor(0, (jas_red | jas_green));              // RED | GREEN
    strip.show();
    delay(1000);

    strip.setPixelColor(0, 0x000000);                           // zhasnout
    strip.show();
    delay(1000);
    barva_LED = 0;
  }
//----------------------------------------------



//----------------------------------------------
void blik_Y_kratky(void)
  {          //GGRRBB
    uint32_t jas_red   = (logaritmy_jasu[jas_displeje] << 8);   // cervena cast jasu LED se posila na prostrednich 8 bitech
    uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16);  // zelena cast jasu LED se posila na levych 8 bitech
    strip.setPixelColor(0, (jas_red | jas_green));              // RED | GREEN
    strip.show();
    delay(100);

    strip.setPixelColor(0, 0x000000);                           // zhasnout
    strip.show();
    barva_LED = 0;
  }
//----------------------------------------------



//----------------------------------------------
void blik_C_kratky(void)
  {          //GGRRBB
    uint32_t jas_blue  = (logaritmy_jasu[jas_displeje] );       // modra cast jasu je ve spodnich (pravych) 8 bitech
    uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16);  // zelena cast jasu LED se posila na levych 8 bitech
    strip.setPixelColor(0, (jas_blue | jas_green));             // BLUE | GREEN
    strip.show();
    delay(100);

    strip.setPixelColor(0, 0x000000);                           // zhasnout
    strip.show();
    barva_LED = 0;
  }
//----------------------------------------------



//----------------------------------------------
void blik_F_kratky(void)
  {          //GGRRBB
    uint32_t jas_red  = (logaritmy_jasu[jas_displeje] << 8);  // cervena cast jasu LED se posila na prostrednich 8 bitech
    uint32_t jas_blue = (logaritmy_jasu[jas_displeje]);       // modra cast jasu LED se posila na pravych (nejnizsich) 8 bitech
    strip.setPixelColor(0, (jas_red | jas_blue));             // RED | BLUE
    strip.show();
    delay(100);

    strip.setPixelColor(0, 0x000000);           // zhasnout
    strip.show();
    barva_LED = 0;
  }
//----------------------------------------------



//----------------------------------------------
void blikblik_G (uint8_t pocet)
  {
    for (uint8_t i = 0 ; i < pocet ; i++)
      {
        blik_G();
        if (digitalRead(pin_tl_ok) == LOW)                        // tlacitkem OK behem vyblikavani cisla se blikani prerusi (i pro nasledujici rady)
          {
            LED_W(true);                                          // docasna signalizace preruseni - SMAZAT
            break_blik = true;
            break;
          }
      }
  }
//----------------------------------------------



//----------------------------------------------
void LED_R_onoff(bool onoff)
  {                         //GGRRBB
    barva_LED = barva_LED & 0xFF00FF;                             // zhasnuti pouze cervene slozky LED
    if (onoff == true)
      {
        uint32_t jas_red = (logaritmy_jasu[jas_displeje] << 8);  // cervena cast jasu LED se posila na prostrednich 8 bitech
        barva_LED = (barva_LED | jas_red);
      }
    strip.setPixelColor(0, barva_LED);
    strip.show();
  
  
  }
//----------------------------------------------



//----------------------------------------------
void LED_B_onoff(bool onoff)
  {                         //GGRRBB
    barva_LED = barva_LED & 0xFFFF00;                              // zhasnuti pouze modre slozky LED
    if (onoff == true)
      {
        uint32_t jas_blue = (logaritmy_jasu[jas_displeje]);        // modra cast jasu LED se posila na pravych 8 bitech
        barva_LED = (barva_LED | jas_blue);
      }
    strip.setPixelColor(0, barva_LED);
    strip.show();
  }
//----------------------------------------------


//----------------------------------------------
void LED_G_onoff(bool onoff)
  {                         //GGRRBB
    barva_LED = barva_LED & 0x00FFFF;                              // zhasnuti pouze zelene slozky LED
    if (onoff == true)
      {
        uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16); // zelena cast jasu LED se posila na levych 8 bitech
        barva_LED = (barva_LED | jas_green);
      }
    strip.setPixelColor(0, barva_LED);
    strip.show();
  }
//----------------------------------------------


//----------------------------------------------
void LED_Y_onoff(bool onoff)
  {                         //GGRRBB
    barva_LED = barva_LED & 0x0000FF;                               // zhasnuti zlutych slozek LED
    if (onoff == true)
      {
        uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16);  // zelena cast jasu LED se posila na levych 8 bitech
        uint32_t jas_red   = (logaritmy_jasu[jas_displeje] <<  8);  // ruda cast jasu LED se posila na pravych 8 bitech
        barva_LED = (barva_LED | jas_green | jas_red);
      }
    strip.setPixelColor(0, barva_LED);
    strip.show();
  }
//----------------------------------------------



//----------------------------------------------
void LED_C_onoff(bool onoff)
  {                         //GGRRBB   
    barva_LED = barva_LED & 0x00FF00;                               // zhasnuti modrych a zelenych slozek LED
    if (onoff == true)
      {
        uint32_t jas_blue  = (logaritmy_jasu[jas_displeje] );       // modra cast jasu je ve spodnich (pravych) 8 bitech
        uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16);  // zelena cast jasu LED se posila na levych 8 bitech
        barva_LED = (barva_LED | jas_blue | jas_green);
      }
    strip.setPixelColor(0, barva_LED);
    strip.show();
  }
//----------------------------------------------




//----------------------------------------------
void LED_W(bool onoff)
  {          //GGRRBB
    barva_LED = 0;                                                   // zhasnuti vsech slozek LED
    if (onoff == true)
      {
        uint32_t jas_red   = (logaritmy_jasu[jas_displeje] << 8);    // cervena cast jasu LED se posila na levych 8 bitech
        uint32_t jas_green = (logaritmy_jasu[jas_displeje] << 16);   // zelena cast jasu LED se posila na prostrednich 8 bitech
        uint32_t jas_blue  = (logaritmy_jasu[jas_displeje]);         // modra cast jasu LED se posila na pravych 8 bitech
        barva_LED = (jas_red | jas_green | jas_blue);
      }
    strip.setPixelColor(0, barva_LED);
    strip.show();

  
  }
//----------------------------------------------



//----------------------------------------------
// nouzove vyblikani cisla pomoci 3-barevne LED (maximalne 6 radu = max cislo 999999)
// uvodni nuly se ignoruji
void vyblikej_cislo(long cislo)
  {

//---------------------------
//    Serial.print("Blikani: ");
//    Serial.println(cislo);
//----------------------------------
    
    long pomprom;
    if (cislo >= 0 )
      {
        LED_Y_onoff(true);                                                    // kladne cislo = rozsviti se zluta LED    
      }
    else
      {
        LED_C_onoff(true);                                                    // zaporne cislo = rozsviti svetle modrou LED
        cislo = -cislo;                                                       // a dal s cislem pracovat jakoby bylo kladne    
      }

    while (digitalRead(pin_tl_ok) == LOW)                                     // dokud je stisknute tlacitko OK, nic se nedeje a ceka se na jeho uvolneni
      {
        delay(50);
      }
    LED_W(false);                                                             // po uvolneni tlacitka OK se znamenkova signalizace LED zhasne 

    delay(500);                                                               // znamenkova signalizace je zhasnuta 0,5 sekundy 

    bool ignoruj_0 = true;

    // rad stotisicu ------------------------------------------------        // a pokracuje se blikanim pozadovaneho cisla
    if (cislo > 99999 and break_blik == false)
      {
        pomprom = cislo / 100000UL;   
        blikblik_G(pomprom);
        cislo = cislo % 100000UL;        
        ignoruj_0 = false;
      }  

    if (ignoruj_0 == false and break_blik == false)
      {
        delay(100);
        blik_R();                                                        // cervene bliknuti jako oddelovac radu
        delay(2000);        
      }

    // rad desetitisicu ------------------------------------------------
    if (cislo > 9999 and break_blik == false)
      {
        pomprom = cislo / 10000;   
        blikblik_G(pomprom);
        cislo = cislo % 10000;        
        ignoruj_0 = false;
      }  

    if (ignoruj_0 == false and break_blik == false)
      {
        delay(100);
        blik_R();                                                        // cervene bliknuti jako oddelovac radu
        delay(2000);        
      }
    
    // rad tisicu ------------------------------------------------
    if (cislo > 999 and break_blik == false)
      {
        pomprom = cislo / 1000;   
        blikblik_G(pomprom);
        cislo = cislo % 1000;
        ignoruj_0 = false;
      }  

    if (ignoruj_0 == false and break_blik == false)
      {
        delay(100);
        blik_R();                                                        // cervene bliknuti jako oddelovac radu
        delay(2000);        
      }

    
    // rad stovek ------------------------------------------------
    if (cislo > 99 and break_blik == false)
      {
        pomprom = cislo / 100;   
        blikblik_G(pomprom);
        cislo = cislo % 100;        
        ignoruj_0 = false;
      }  

    if (ignoruj_0 == false and break_blik == false)
      {
        delay(100);
        blik_R();                                                        // cervene bliknuti jako oddelovac radu
        delay(2000);        
      }

      
    // rad desitek -----------------------------------------------
    if (cislo > 9 and break_blik == false)
      {
        pomprom = cislo / 10;   
        blikblik_G (pomprom);
        cislo = cislo % 10;        
        ignoruj_0 = false;
      }  
    if (ignoruj_0 == false and break_blik == false)
      {
        delay(100);
        blik_R();                                                        // cervene bliknuti jako oddelovac radu
        delay(2000);        
      }


    // rad jednotek ----------------------------------------------
    if (break_blik == false)
      {
        blikblik_G (cislo);        
      }
      
    delay(200);
    LED_R_onoff(true);                                                             // konec odesilani = dlouhe cervene bliknuti
    delay(500);

    while(digitalRead(pin_tl_ok) == LOW)                                           // pri predcasnem ukonceni blikani tlacikem ok se muze stat, ze je stale stisknute
      {                                                                            //     proto se ceka na jeho uvolneni
        delay(50);
      }
    LED_R_onoff(false);                                                             // konec odesilani = dlouhe cervene bliknuti
    
    break_blik = false;                                                            // pristi blikani se uz zase provadet bude
  }
//----------------------------------------------
