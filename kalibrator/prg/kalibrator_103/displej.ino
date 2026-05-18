//========================================================================================
//
//              ****     D I S P L E J     ****
//
//   (4 znaky s dvojteckou uprostred, s ovladacem TM1637)
//========================================================================================
// odesle do datoveho pinu 8 bitu se soucasnym kmitanim pinu CLK. 
// Na zaver posle jeste jeden hodinovy impulz pro prijem ACK bitu - ten ale netestuji.
void zapis_data(byte disdata)
  {
    for(byte i = 0; i < 8; i++)
      {
        digitalWrite(pin_dis_CLK, LOW);
        delayMicroseconds(2);
        if(bitRead(disdata , i) == true)     digitalWrite(pin_dis_DIN  , HIGH);
        else                                 digitalWrite(pin_dis_DIN  , LOW);

        delayMicroseconds(2);

        digitalWrite(pin_dis_CLK, HIGH);
        delayMicroseconds(2);
      }

    digitalWrite(pin_dis_CLK, LOW);
    delayMicroseconds(10);
    
                                                                                      // (tady by se mel testovat signal ACK, ale kaslu na to)
    
    digitalWrite(pin_dis_CLK, HIGH);
    delayMicroseconds(2);
    
    digitalWrite(pin_dis_DIN, LOW);                                                   // ukonceni odesilani bajtu prepnutim obou pinu do LOW
    delayMicroseconds(2);
    digitalWrite(pin_dis_CLK, LOW);
    delayMicroseconds(2);
  }
//========================================================================================



//========================================================================================
// zapise do displeje obsah pole promennych D_pamet[]
// pokud bude zadany parametr funkce, nastavi se podle nej jas displeje
void aktualizuj_displej(void)
  {
    byte vstup_jas = 2;
    digitalWrite(pin_dis_CLK,LOW);
    digitalWrite(pin_dis_DIN,LOW);                                                    // start_kom
    delayMicroseconds(2);

    zapis_data(0b01000000);                                                           // automaticke zvetsovani adresy (pozice sedmisegmentovky)

    digitalWrite(pin_dis_CLK, HIGH);                                                  // stop_kom
    delayMicroseconds(2);
    digitalWrite(pin_dis_DIN, HIGH);
    delayMicroseconds(2);

    digitalWrite(pin_dis_DIN, LOW);                                                   // start_kom
    delayMicroseconds(2);

    zapis_data(0b11000000);                                                           // bit2 az bit0 = startovni pozice na nulte segmentovce (nejvic vlevo)

    zapis_data(D_pamet[3]);
    zapis_data(D_pamet[2]);
    zapis_data(D_pamet[1]);
    zapis_data(D_pamet[0]);

    digitalWrite(pin_dis_CLK, HIGH);                                                  // stop_kom
    delayMicroseconds(2);
    digitalWrite(pin_dis_DIN, HIGH);
    delayMicroseconds(2);

    digitalWrite(pin_dis_DIN, LOW);                                                   // start_kom
    delayMicroseconds(2);

    zapis_data(0b10001000 + vstup_jas);                                               // bit2 az bit0 nastaveni jasu (bit3 = 0/1 = zhasnout/rosvitit displej)

    digitalWrite(pin_dis_CLK, HIGH);                                                  // stop_kom
    delayMicroseconds(2);
    digitalWrite(pin_dis_DIN, HIGH);
    delayMicroseconds(2);
  }
//========================================================================================



//========================================================================================
// zobrazi na displeji text z pole dis_popisy[]
void zobraz_text(byte polozka)
  {
    D_pamet[0] =  dis_popisy[polozka][3];                                             // pretoceni nejvyssich a nejnizsich radu pro spravne zobrazeni na spravnych jednotkych
    D_pamet[1] =  dis_popisy[polozka][2];
    D_pamet[2] =  dis_popisy[polozka][1];
    D_pamet[3] =  dis_popisy[polozka][0];
    aktualizuj_displej();
    if (polozka == 5)   bitClear(EXPORT_pole[9],3);                  // zhasnuty displej
    else                  bitSet(EXPORT_pole[9],3);                  // na displeji neco sviti
  
  }
//========================================================================================



//========================================================================================
// Zobrazi na displeji bud cele cislo v rozsahu -999 az 9999, nezobrazuje uvodni nuly
//   styl 0 ... bezne zobrazeni celeho cisla 0 az 9999 nebo zaporneho cisla 0 az - 999
//   styl 1 ... zobrazeni dvojtecky mezi 2. a 3. sedmisegmentovkou (neco jako desetinna tecka pro zobrazeni proudu v mA)
void zobraz_cislo(int cislo, byte styl)
  {
    bool zobraz_nuly = false;                                                         // uvodni nuly se defaultne nezobrazuji
    bool preskoc_tisice = false;                                                      // pro zaporna cisla se preskakuje ovladani nejvyssi sedmisegmentovky (nastavi se napevno na '-')

    if (cislo < 0)
      {
        D_pamet[3] = 64;                                                              // pomlcka
        cislo = -cislo;                                                               // prevod na kladne cislo
        preskoc_tisice = true;
      }
    
    if (styl == 1) zobraz_nuly = true;                                                // pri zobrazeni proudu jako desetinne cislo se zobrazuji vsechny uvodni nuly (pripadna nejvyssi nula se zhasina na konci podprogramu)

    
    //  ------------------ rad tisicu ----------------------
    if (cislo > 999)
      {
        D_pamet[3] = graf_def[cislo / 1000];
        cislo = cislo % 1000;
        zobraz_nuly = true;
      }
    else
      {
        if (preskoc_tisice == false)                                                  // pri zapornych cislech se tato segmentovka preskakuje
          {
            if (zobraz_nuly == true)                      D_pamet[3] = graf_def[0];   // Znak '0'
            else                                          D_pamet[3] = 0;             // nebo prazdny znak
          }
      }
    
    //  ------------------ rad stovek ----------------------
    if (cislo > 99)
      {
        D_pamet[2] = graf_def[cislo / 100];
        cislo = cislo % 100;
        zobraz_nuly = true;
      }
    else
      {
        if (zobraz_nuly == true)                          D_pamet[2] = graf_def[0];   // Znak '0'
        else                                              D_pamet[2] = 0;             // nebo prazdny znak
      }



    //  ------------------ rad desitek ----------------------
    if (cislo > 9)
      {
        D_pamet[1] = graf_def[cislo / 10];
        cislo = cislo % 10;
      }
    else
      {
        if (zobraz_nuly == true)                          D_pamet[1] = graf_def[0];   // Znak '0'
        else                                              D_pamet[1] = 0;             // nebo prazdny znak
      }



    //  ------- rad jednotek (prava sedmisegmentovka) -------
    D_pamet[0] = graf_def[cislo];



    
    // --- uprava pro styl desetinnych cisel ---------
    if (styl == 1)                                                                    // pro zobrazeni desetinnych cisel (proudy v mA)
      {
        D_pamet[2] = D_pamet[2] + 128;                                                // prostredni dvojtecka se rozsviti
        if (D_pamet[3] == graf_def[0])                                                // kdyz je na nejvyssim radu '0' (desitky mA), tak se nezobrazuje. Ale na druhe nejvyssi (jednotky mA) se uz zobrazovat bude
          {
            D_pamet[3] = 0;                                                           // zhasnout znak na nejvyssim radu
          }
      }
    // ----------------------------------------------


    aktualizuj_displej(); 
    bitSet(EXPORT_pole[9],3);                                                         // na displeji neco sviti
  }
//========================================================================================
