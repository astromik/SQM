// Funkce 'Stopky'
//================================================

//  Spousti se pri zapnuti napajeni a soucasnem stisku tlacitka OK, nebo pres menu (polozka StoP)
//  Na zacatkku se na displeji zobrazi "  0.0"
//  POZOR !  -  Behem stopek nefunguji zadne dalsi SQM funkce (automaticke mereni ani komunikace)
//  Ukonceni funkce se provadi vypnutim napajeni nebo soucasnym stiskem tlacitek nahoru a dolu
//  Ovladani:
//                Tlacitko OK     = START / CONT
//                Tlacitko nahoru = mezicas (LAP) (cas bezi dal) / kdyz je zastaveno, tak nulovani
//                Tlacitko dolu   = STOP
//                Tlacitko nahoru a dolu v jakekoliv fazi = vypadnuti zpatky do menu, nebo do hlavni smycky loop()
// Zaznam:
//  v prvnim bajtu zaznamu EEPROM[0] se nastavi bit6 do '1' - to je informace, ze zaznam neobsahuje svetlo, ale stopky.
//  Zbytek EEPROM[0] je ulozeny stejne jako obvykle.   
//  Pri startu se na SD kartu a do EEPROM ulozi znacka datumu a casu (bez svetla, teploty, GPS a dalsich parametru)
//         Do EEPROM[1] az EEPROM[4] se zapise cas beznym zpusobem, EEPROM[5] a [6] se naplni hodnotou 65535, a EEPROM[7] se nastavi 0.
//         Pokud je povoleno ukladat i dalsi veliciny (teplota, infra, GPS ....), zapisi se na prislusne pozice chyby cidel
//  Mezicas se take zapisuje na SD kartu a do EEPROM. EEPROM[0], bezny zapis s oznacenim pouzite bunky.
//         Na pozice EEPROM[1] az EEPROM[4] se ulozi cas v milisekundach od startu.   EEPROM[5] a [6] se naplni hodnotou 65535, a EEPROM[7] se nastavi na 1. 
//  STOP se take zapisuje na SD kartu a do EEPROM. EEPROM[0], bezny zapis s oznacenim pouzite bunky.
//         Na pozice EEPROM[1] az EEPROM[4] se ulozi cas v milisekundach od startu.   EEPROM[5] a [6] se naplni hodnotou 65535, a EEPROM[7] se nastavi na 2. 
//  Vypis namerenych casu do seriove linky se prizpusobi tomu, ze pro cislo 65535 v EEPROM[5] a [6] zobrazi misto datumu cas v ms.
//  
//  Priklad vypisu EEPROM (od adresy 755):
//     0xC1,0x63,0x5A,0xF3,0xF1,0xFF,0x2B,0x01,0x01,0x18,0x05,0x1B,0x35,0x1C,0xCC,0x18,0x64,0x90,0x46,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x27,0x08    (Start)
//     0xC1,0x00,0x00,0x22,0xA7,0xFF,0x01,0x03,0x01,0x18,0x05,0x1B,0x35,0x1C,0xCC,0x18,0x64,0x90,0x46,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x27,0x08    (Mezicas 1)
//     0xC1,0x00,0x00,0x42,0xAA,0xFF,0x02,0x03,0x01,0x18,0x05,0x1B,0x35,0x1C,0xCC,0x18,0x64,0x90,0x46,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x27,0x08    (Mezicas 2)
//     0xC1,0x00,0x00,0x5A,0x79,0xFF,0x03,0x03,0x01,0x18,0x05,0x1B,0x35,0x1C,0xCC,0x18,0x64,0x90,0x46,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x27,0x08    (Mezicas 3)
//     0xC1,0x00,0x00,0x76,0xE8,0xFF,0x04,0x02,0x01,0x18,0x05,0x1B,0x35,0x1C,0xCC,0x18,0x64,0x90,0x46,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x27,0x08    (Stop)
//     0xC1,0x63,0x5A,0xF4,0x15,0xFF,0x04,0x04,0x01,0x18,0x05,0x1B,0x35,0x1C,0xCC,0x18,0x64,0x90,0x46,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x27,0x08    (Pokracovani bez vynulovani)
//     0xC1,0x00,0x00,0x9A,0x26,0xFF,0x05,0x02,0x01,0x18,0x05,0x1B,0x35,0x1C,0xCC,0x18,0x64,0x90,0x46,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x27,0x08    (Stop)
//
//
//  Tomu odpovidajici vypis (hlavicka je stejna jako pro normalni zaznamy jasu a proto ji nektere polozky ze stopek neodpovidaji):
//  pol. ; EEPROM ;  sek_1970  ;    datum   ;    cas   ; dvt; zona ;  jas  ;   jas  ;adr.;cid.; pozn. ; stab_jas ; infra ;  full ; [ms]; citl. ;teplota; tep.'C ;vlhkost; vlh. % ;  tlak  ;   GEO_lat  ;   GEO_long  ;  ALT  ; naklon 
// 00006 ; 000755 ; 1666905073 ; 27.10.2022 ; 21:11:13 ; Ct ; SELC ; ----- ; ------ ; -- ; -- ; START ;  TIMER   ; ----- ; ----- ; --- ; ----- ; ----- ; ------ ; ----- ; ------ ; ------ ; ---------- ; ----------- ; ----- ; ------ 
// 00007 ; 000786 ; 0000008871 ;  00:00:08  ;  +0,871  ; -- ; ---- ; ----- ; ------ ; -- ; 01 ;  LAP  ;  TIMER   ; ----- ; ----- ; --- ; ----- ; ----- ; ------ ; ----- ; ------ ; ------ ; ---------- ; ----------- ; ----- ; ------ 
// 00008 ; 000817 ; 0000017066 ;  00:00:17  ;  +0,066  ; -- ; ---- ; ----- ; ------ ; -- ; 02 ;  LAP  ;  TIMER   ; ----- ; ----- ; --- ; ----- ; ----- ; ------ ; ----- ; ------ ; ------ ; ---------- ; ----------- ; ----- ; ------ 
// 00009 ; 000848 ; 0000023161 ;  00:00:23  ;  +0,161  ; -- ; ---- ; ----- ; ------ ; -- ; 03 ;  LAP  ;  TIMER   ; ----- ; ----- ; --- ; ----- ; ----- ; ------ ; ----- ; ------ ; ------ ; ---------- ; ----------- ; ----- ; ------ 
// 00010 ; 000879 ; 0000030440 ;  00:00:30  ;  +0,440  ; -- ; ---- ; ----- ; ------ ; -- ; 04 ; STOP  ;  TIMER   ; ----- ; ----- ; --- ; ----- ; ----- ; ------ ; ----- ; ------ ; ------ ; ---------- ; ----------- ; ----- ; ------ 
// 00011 ; 000910 ; 1666905109 ; 27.10.2022 ; 21:11:49 ; Ct ; SELC ; ----- ; ------ ; -- ; -- ; CONT  ;  TIMER   ; ----- ; ----- ; --- ; ----- ; ----- ; ------ ; ----- ; ------ ; ------ ; ---------- ; ----------- ; ----- ; ------ 
// 00012 ; 000941 ; 0000039462 ;  00:00:39  ;  +0,462  ; -- ; ---- ; ----- ; ------ ; -- ; 05 ; STOP  ;  TIMER   ; ----- ; ----- ; --- ; ----- ; ----- ; ------ ; ----- ; ------ ; ------ ; ---------- ; ----------- ; ----- ; ------ 
// 
//  'sek_1970'     pocet milisekund od zacatku mereni                         (nebo pocet sekund od roku 1970 pri startu mereni)
//  'datum'        pocet hodin minut a sekund od zacatku mereni               (nebo datum spusteni mereni)
//  'cas'          pocet milisekund, ktere se pridavaji k predchozi polozce   (nebo cas spusteni mereni)
//  'cid'          poradove cislo jednoho mereni casu (kazdy mezicas nebo stop ma vlastni poradove cislo. START provadi nulovani pocitadla) (max. = 99)
//  'pozn.'        informace o typu zaznamu (START, STOP, LAP, CONT)
//  'stab_jas'     pro zaznamy stopek vzdycky napis "TIMER"   


//----------------------------------------------
void stopky(void)
  {
    uint32_t start_cas = 0;                                                          // milisekundy pri zapnuti stopek
    uint32_t stop_cas = 0;                                                           // milisekundy pri zastaveni stopek
    uint32_t posledni_zobrazeni = 0;                                                 // displej se aktualizuje kazdou desetinu sekundy a v teto promenne uchovava cas posledni aktualizzace
    uint32_t aktual;                                                                 // cislo, ktere ma zobrazovat prubezne na displeji
    
    
    uint8_t faze = 0;                                                                        // stav, ve kterem se stopky aktualne nachazi 
                                                                                          // 0 = stop - vynulovano
                                                                                          // 1 = beh
                                                                                          // 2 = stop - zobrazen posledni cas
        
    pozice_tecky = 1;
    zobraz_cislo(0, 0);

    bool vypadnout = false;
    while (vypadnout == false)                                                            // rychla smycka, ze ktere se vyskoci soucasnym stiskem nahoru a  dolu
      {
        if (digitalRead(pin_tl_ok) == LOW and faze == 0)                                  // START
          {
            start_cas = millis();
            posledni_zobrazeni = start_cas;
            faze = 1;
            lap_count = 0;                                                                // vynulovani pocitadla mezicasu
            tone_X(pin_bzuk,800, 20 , 7);
            priprava_EEPROM_stopky(1, 0);                                                 // zapis do EEPROM
          }

        
        if (digitalRead(pin_tl_dn) == LOW and faze == 1)                                  // STOP
          {
            stop_cas = millis();
            faze = 2;
            lap_count ++;                                                                 // i pri zastaveni stopek se pocitadlo mezicasu zvetsi o 1
            tone_X(pin_bzuk,800, 20 , 7);
            uint32_t vysledek = stop_cas - start_cas;
            zobraz_stopky((50 + vysledek) / 100);                                         // cas v desetinach sekundy (zaokrouhlene)
            priprava_EEPROM_stopky(2, vysledek);                                          // zapis do EEPROM      
          }

        if (digitalRead(pin_tl_ok) == LOW and faze == 2)                                  // POKRACOVANI PO STOPU
          {
            start_cas = start_cas + millis() - stop_cas;
            faze = 1;
            tone_X(pin_bzuk,800, 20 , 7);
            priprava_EEPROM_stopky(4, 0);                                                 // zapis do EEPROM
          
          }




        if (digitalRead(pin_tl_up) == LOW and faze == 1)                                  // MEZICAS
          {
            uint32_t vysledek = millis() - start_cas;                                // cas v milisekundach
            tone_X(pin_bzuk,800, 20 , 7);
            lap_count ++;                                                                 // pocitadlo mezicasu se zvetsi o 1
            if (lap_count == 100) lap_count = 99;                                         // maximalni hodnota je 99 (pri prekroceni zustava na 99 i pro dalsi mezicasy)
            zobraz_stopky((50 + vysledek) / 100);                                         // cas v desetinach sekundy (zaokrouhlene)
            priprava_EEPROM_stopky(3, vysledek);                                          // zapis do EEPROM
            while (digitalRead(pin_tl_up) == LOW)                                         // cekani na uvolneni tlacitka (nez se tlacitko uvolni, porad sviti puvodni mezicas)
              {
                delay(1);
                if (digitalRead(pin_tl_dn) == LOW) vypadnout = true;                      // nahoru a dolu soucasne znamena vypadnuti ze stopek
                if (digitalRead(pin_tl_up) == HIGH) delay(50);                            // kdyz se behem drzeni tlacitka UP objevi kratky vypadek kontaktu, tak se odignoruje        
              }
            delay(20);                                                                    // odruseni zakmitu pri uvolneni
          }

        
        
        
        if (digitalRead(pin_tl_up) == LOW and faze == 2)                                  // NULOVANI
          {
            faze = 0;
            posledni_zobrazeni = millis();
            pozice_tecky = 1;
            tone_X(pin_bzuk,800, 20 , 7);
            zobraz_cislo(0, 0);
          }





        if ((millis() - 100) > posledni_zobrazeni and faze == 1)                          // displej se pri behu obcerstvuje kazdou desetinu sekundy
          {
            posledni_zobrazeni = millis();
            aktual = (posledni_zobrazeni - start_cas) / 100;                              // cas v desetinach sekundy
            zobraz_stopky(aktual);
            
          }

        if (digitalRead(pin_tl_dn) == LOW and digitalRead(pin_tl_up) == LOW) vypadnout = true;                  // nahoru a dolu soucasne znamena vypadnuti z teto smycky

      
      }
    
    zobraz_text(66);                                                                      // zhasnout displej
    while (digitalRead(pin_tl_dn) == LOW or digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_ok) == LOW)     // ceka se na uvolneni vsech tlacitek
      {
        delay(10);
      }
    delay(20);

                                                                                          // navrat do menu, nebo do loop() smycky
  
  }
//----------------------------------------------



//----------------------------------------------
// zobrazeni cisla na displeji (pri male hodnote v desetinach sekundy, pri velkych hodnotach v sekundach)
void zobraz_stopky(uint16_t cislo_na_displej)
  {

    if (cislo_na_displej < 100000UL)
      {
        pozice_tecky = 1;            
      }
    else
      {
        pozice_tecky = 0;
        cislo_na_displej = cislo_na_displej / 10;                                         // cas v sekundach
      }
    if (cislo_na_displej < 100000UL)
      {
        zobraz_cislo(cislo_na_displej , 0);
      }
    else
      {
        zobraz_text(68);                                                                  // pomlcky na displeji "-----"  
      }
    
  }
//----------------------------------------------



//----------------------------------------------
// ulozeni merenych hodnot do pole 
void priprava_EEPROM_stopky(uint8_t fce, uint32_t cas_data)
  {
    pole_EEPROM[0] =  0b11000000 + selc;                                                  // znacka, ze je blok adres v EEPROM obsazeny (bit7='1') jestli je SEC(bit0='0'), nebo SELC(bit0='1')
                                                                                          // bit6 signalizuje, ze zaznam se tyka stopek
                                                                                          
    if (fce == 1 or fce == 4)                                                             // START    / CONT
      {
        uint32_t tt = rtc_korekce();                                                 // zjisteni aktualniho casu v RTC

        pole_EEPROM[1] =  ((uint32_t)tt >> 24) & 0xFFUL;                             // 4 bajty pro pocet sekund od 1.1.1970 (s korekci nepresnosti a pripadnym prevodem na letni cas)
        pole_EEPROM[2] =  ((uint32_t)tt >> 16) & 0xFFUL;
        pole_EEPROM[3] =  ((uint32_t)tt >>  8) & 0xFFUL;
        pole_EEPROM[4] =  ((uint32_t)tt      ) & 0xFFUL;   
        
      }

    if (fce == 2 or fce == 3)                                                             // STOP (PAUZA)   /   MEZICAS 
      {
        pole_EEPROM[1] =  ((uint32_t)cas_data >> 24) & 0xFFUL;                       // 4 bajty pro ulozeni casu v milisekundach
        pole_EEPROM[2] =  ((uint32_t)cas_data >> 16) & 0xFFUL;
        pole_EEPROM[3] =  ((uint32_t)cas_data >>  8) & 0xFFUL;
        pole_EEPROM[4] =  ((uint32_t)cas_data      ) & 0xFFUL;

        pole_EEPROM[6] = lap_count;                                                       //  cislo mezicasu se uklada do prostoru, ktery je normalne urcen pro LSB svetla
      }
  
    pole_EEPROM[5] = 255;                                                                 // MSB bajt ze svetla je nepouzity  
    pole_EEPROM[7] = fce;                                                                 // info bajt se nastavi podle aktualni funkce (START, STOP, MEZICAS)
  
    zaznam_EEPROM();
  }
//----------------------------------------------


  
