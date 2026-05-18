//  Obsluha displeje pres TM1637
//=============================================


//----------------------------------------------
// zobrazi na displeji text z pole dis_popisy[]
void zobraz_text(uint8_t polozka)
  {
    D_pamet[0] =  dis_popisy[polozka][4];                                             // pretoceni nejvyssich a nejnizsich radu pro spravne zobrazeni na spravnych jednotkach
    D_pamet[1] =  dis_popisy[polozka][3];
    D_pamet[2] =  dis_popisy[polozka][2];
    D_pamet[3] =  dis_popisy[polozka][1];
    D_pamet[4] =  dis_popisy[polozka][0];
    aktualizuj_displej();
  }
//----------------------------------------------



//----------------------------------------------
// odesle do datoveho pinu 8 bitu se soucasnym kmitanim pinu CLK. 
// Na zaver posle jeste jeden hodinovy impulz pro prijem ACK bitu - ten ale netestuji.
void zapis_data(uint8_t disdata)
  {
    for(uint8_t i = 0; i < 8; i++)
      {
        digitalWrite(pin_dis_CLK, LOW);
        delayMicroseconds(2);
        if(bitRead(disdata , i) == true) digitalWrite(pin_dis_DIN  , HIGH);
        else                             digitalWrite(pin_dis_DIN  , LOW);

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
//----------------------------------------------



//----------------------------------------------
// zapise do displeje obsah pole promennych D_pamet[]
// kdyz neni zadany parametr funkce, vezme se jas z globalni promenne 'jas_displeje'
void aktualizuj_displej(void)
  {
    aktualizuj_displej(jas_displeje);
  }
//----------------------------------------------



//----------------------------------------------
// zapise do displeje obsah pole promennych D_pamet[]
// pokud bude zadany parametr funkce, nastavi se podle nej jas displeje
void aktualizuj_displej(uint8_t vstup_jas)
  {
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

    zapis_data(D_pamet[0]);
    zapis_data(D_pamet[1]);
    zapis_data(D_pamet[2]);
    zapis_data(D_pamet[3]);
    zapis_data(D_pamet[4]); 

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
//----------------------------------------------



//----------------------------------------------
// Zobrazeni cisla na  5-znakovem displeji
//  Uvodni nuly se nezobrazuji. (cislo 56 se zobrazi jako "   56")
//  Jedna nula pred desetinnou teckou se zobrazuje. Pri nastaveni napriklad 3 desetinnych mist a zobrazovanem cisle 6 ukaze displej " 0.006"
//  Nula pred desetinnou teckou se nezobrazi pouze v pripade zaporneho cisla se 4 desetinnymi misty: napriklad "-0,1234" se zobrazi jako "-.1234" ).
//  Znamenko minus se zobrazuje vzdycky na nejvyssi pozici (leva sedmisegmentovka).  (-7 = "-   7")
//  Pri zobrazeni cisla bez desetinnych mist se desetinna tecka vpravo nezobrazuje.  (78 = "   78")
//  Parametr 'styl' udava zpusob zobrazeni:
//       0 = zobrazi cislo pres cely displej (prepisuje nebo maze i 1. sedmisegmentovku zleva)
//       1 = zachova zobrazeny znak na prvni jednotce. Cislo pak musi byt v rozsahu 0 az 9999 (nesmi byt zaporne a nesmi byt moc velke).
//       2 = specialni pripad, kdy se zobrazuje napis "L   0" az "L  -99" (pri listovani v ulozenych hodnotach)
//       3 = zobrazeni odpoctu pri mereni svetla ve formatu "-20- " az "-0-  " (nahrada puvodniho sloupcoveho grafu)
//       4 = rezerva (puvodne tam bylo neco hodne podobne stylu 0 ale uz to nemuzu najit. Pro jistotu nechavam neobsazene.)
//       5 = styl zobrazeni odpoctu pri Hard Formatu (H.F.123)
//       6 = styl zobrazeni odpoctu pri dlouhych vypisech EEPROM (rd.123)
//       7 = styl zobrazeni odpoctu pri Soft Formatu (S.F.123)
//       8 = podobne jako styl 1, akorat se zachovavaji 2 leve sedmisegmentovky a cislo musi byt v rozsahu 0 az 999
//       9 = styl pro zobrazeni uhlu natoceni - prvni segmentovka zobrazuje stupne, nekdy muze obsahovat i znamenko '-', druha je vzdycky volna, pak nasleduji dva znaky pro celou cast cisla (uvodni nula se nezobrazuje), desetinna tecka a jedno desetinne cislo. Ocekava vstup cislo v rozsahu -999 az 999. Desetinne cislo z toho dela automaticky
//      10 = styl pro zobrazeni Elevace Slunce   - prvni dva znaky jsou "SE.", treti je prazdny nebo minus, zbyle 2 znaky zobrazuji cele cislo v rozsahu 0 az 99 bez uvodnich nul)
//      11 = styl pro zobrazeni Elevace Mesice   - prvni dva znaky jsou "ME.", treti je prazdny nebo minus, zbyle 2 znaky zobrazuji cele cislo v rozsahu 0 az 99 bez uvodnich nul)
//      12 = styl pro zobrazeni osvetleni Mesice - prvni dva znaky jsou "Mo.", treti je prazdny, zbyle 2 znaky zobrazuji cele cislo v rozsahu 0 az 99 bez uvodnich nul)
//      13 = styl pro zobrazeni azimutu Slunce - prvni dva znaky jsou "SA." a pak hned nasleduje cislo mezi 0 a 359
//      14 = styl pro zobrazeni azimutu Mesice - prvni dva znaky jsou "MA." a pak hned nasleduje cislo mezi 0 a 359
//      15 = styl pro zobrazeni azimutu krabicky - prvni dva znaky jsou " A." a pak hned nasleduje cislo mezi 0 a 359
//      16 = styl pro zobrazeni odpoctu pri kalibraci kompasu - prvni tri znaky jsou "CAL." a pak hned nasleduje cislo mezi 0 a 10
//      17 = styl pro zobrazeni zadavane rektascenze - prvni dva znaky jsou "rA." a pak hned nasleduje cislo mezi 0 a 23.9
//      18 = styl pro zobrazeni zadavane deklinace - prvni dva znaky jsou "dE." a pak hned nasleduje cislo mezi -90 a 90
//      19 = styl pro zobrazeni zadavaneho intervalu 10 az 250s, 5M, 7M, 10M, 20M, 30M
//      20 = styl 1, akorat jeste pred zobrazenim prepise pravou sedmisegmentovku znakem '%'. v pripade, ze je hodnota mensi nez 10, zobrazi na druhe segmentovce zparva "0"
//      21 = styl pro zobrazeni zadavane hodnoty Alarm for Darkness a pak hned nasleduje cislo mezi 13.0 a 25.5
//      22 = styl pro zobrazeni dvou cisel oddelenych pomlckou "15 29". Kazde z cisel nezobrazuje pripadne uvodni nuly (" 6  9"). Vstup je cele cislo v rozsahu 0 az 99999. Prostredni cifra se bez nahrady maze.
//      23 = styl pro zobrazeni Bortleho stupnice: "bor.=5"
//      24 = styl pro zadavani MHV (na zacatku znaky "MHV." a cislo na jedno desetinne misto)
//      25 = stejny styl jako 8, ale doplnuje tecku pred 1 desetinne misto (na druhou segmentovku zprava)

void zobraz_cislo(long cislo, uint8_t styl)
  {
    uint8_t vstup_seg1 = D_pamet[4];                                                    // pro styl 8 se musi zapamatovat prvni dve segmentovky
    uint8_t vstup_seg2 = D_pamet[3];
    
    uint8_t predtext = 0;                                                                // pro styly, ktere zobrazuji nejaky dlouhy odpocet (formatovani a vypisy) se pred cislem zobrazuji jeste nejake texty
    posledni_n = cislo;                                                               // Pamet posledniho zadaneho cisla kvuli aktualizaci zobrazeni pri zmene polohy desetinne tecky
    if (styl == 5)
      {
        styl = 0;
        predtext = 1;                                                                 // na prvnich sedmisegmentovkach se zobrazi znaky "H.F." jako Hard Format
        pozice_tecky = 0;                                                             // tecka se v tomto pripade nezobrazuje
      }

    if (styl == 6)
      {
        styl = 0;
        predtext = 2;                                                                 // na prvnich sedmisegmentovkach se zobrazi znaky "rd." jako Read (vypis dlouhych dat z EEPROM)
        pozice_tecky = 0;                                                             // tecka se v tomto pripade nezobrazuje
      }

    if (styl == 7)
      {
        styl = 0;
        predtext = 3;                                                                 // na prvnich sedmisegmentovkach se zobrazi znaky "S.F." jako Soft Format
        pozice_tecky = 0;                                                             // tecka se v tomto pripade nezobrazuje
      }

    if (styl == 8)
      {
        styl = 0;
        pozice_tecky = 0;                                                             // tecka se v tomto pripade nezobrazuje
        predtext = 19;
      }

    if (styl == 25)
      {
        styl = 0;
        pozice_tecky = 1;                                                             // tecka tak, aby zobrazene cislo melo 1 desetinne misto
        predtext = 19;
      }


    if (styl == 9)
      {
        styl = 0;
        predtext = 4;                                                                 // na prvnich sedmisegmentovkach se zobrazi stupen, nebo stupen a minus
        pozice_tecky = 1;
      }

    if (styl == 10)
      {
        styl = 0;
        predtext = 5;                                                                 // na prvnich trech sedmisegmentovkach se zobrazi "S  " nebo "S -"
        pozice_tecky = 0;
      }

    if (styl == 11)
      {
        styl = 0;
        predtext = 6;                                                                 // na prvnich trech sedmisegmentovkach se zobrazi "M  " nebo "M -"
        pozice_tecky = 0;
      }

    if (styl == 12)
      {
        styl = 0;
        predtext = 7;                                                                 // na prvnich trech sedmisegmentovkach se zobrazi "o  "
        pozice_tecky = 0;
      }

    if (styl == 13)
      {
        styl = 0;
        predtext = 8;                                                                 // na prvnich dvou sedmisegmentovkach se zobrazi "SA."
        pozice_tecky = 0;
      }

    if (styl == 14)
      {
        styl = 0;
        predtext = 9;                                                                 // na prvnich dvou sedmisegmentovkach se zobrazi "MA."
        pozice_tecky = 0;
      }

    if (styl == 15)
      { 
        styl = 0;
        predtext = 10;                                                                // na prvnich dvou sedmisegmentovkach se zobrazi " A."
        pozice_tecky = 0;
      }

    if (styl == 16)
      {
        styl = 0;
        predtext = 11;                                                                // na prvnich trech sedmisegmentovkach se zobrazi "CAL."
        pozice_tecky = 0;
      }

    if (styl == 17)
      {
        styl = 0;
        predtext = 12;                                                                // na prvnich dvou sedmisegmentovkach se zobrazi "rA."
        pozice_tecky = 1;
      }

    if (styl == 18)
      {
        styl = 0;
        predtext = 13;                                                                // na prvnich dvou sedmisegmentovkach se zobrazi "dE."
        pozice_tecky = 0;
      }

    if (styl == 19)
      {
        styl = 0;
        predtext = 14;                                                                // na prvnich dvou sedmisegmentovkach se zobrazi "I.="
        pozice_tecky = 0;
      }

    if (styl == 21)
      {
        styl = 0;
        predtext = 15;                                                                // na prvnich dvou sedmisegmentovkach se zobrazi "A.d."
        pozice_tecky = 1;
      }



   
    bool znamenko = false;                                                            // Zapamatovat si znamenko, ktere se pak zobrazi na 1. segmentovce zleva (index 4)
    if (cislo < 0)                                                                    // Zaporna cisla ...
      {
        znamenko = true;                                                              // ... se prevedou na kladna, ale zapamatuje se znamenko
        cislo=abs(cislo);
      }

    bool zobraz_nuly = false;                                                         // Nuly pred cislem se normalne nezobrazuji
                                                                                      // Prepnutim na true se zacnou zobrazovat uz od 1.pozice zleva na displeji

    if (styl == 3)                                                                    // zobrazeni cisla ve formatu "-nn- "
      {
        cislo = cislo * 100;                                                          // cislo se posune o 2 mista vlevo (priklad: z puvodniho cisla 15 se stane " 1500" a na displeji se zobrazi jen pozice tisicu a stovek)
        zobraz_nuly = true;                                                           // budou se zobrazovat nuly pred cislem, takze  napriklad z puvodniho 8 se stane "00800" a na displeji se v zaveru zobrazi "-08- "
        pozice_tecky = 0;                                                             // tecka se v tomto pripade nezobrazuje
      }

    
    if (styl == 22)                                                                   // 12345 ... "12 45" ;  78009 ... "78  9"; 6903... " 6  3"; 0 ... " 0  0"                                                      
      {
        styl = 0;
        predtext = 16;                                                                // cislo se rozdeli na 2 casti a smazou se pripadne uvodni nuly kazde z casti
        pozice_tecky = 0;                                                             // tecka se v tomto pripade nezobrazuje
        zobraz_nuly = true;
      }
    
    if (styl == 23)
      {
        styl = 0;
        predtext = 17;                                                                // na prvnich ctyrech sedmisegmentovkach se zobrazi "bor.="
        pozice_tecky = 0;
      }

    if (styl == 24)
      {
        styl = 0;
        predtext = 18;                                                                // na prvnich trech sedmisegmentovkach se zobrazi "MHV." (nebo "nEL." pro anglickou verzi)
        pozice_tecky = 1;
      }

    
    if (styl == 0)
      {
    
        
        //  ------------------ rad desetitisicu  - tohle se pouzije jen pri mereni kladneho svetla na 3 desetinna mista ----------------------
        if (cislo > 9999)
          {
            D_pamet[4] = graf_def[cislo / 10000];
            cislo = cislo % 10000;
            zobraz_nuly = true;
          }
        else
          {
            if (zobraz_nuly == true)                      D_pamet[4] = graf_def[0];   // Znak '0'
            else                                          D_pamet[4] = 0;             // nebo prazdny znak
          }

      }

    // pripadne zaporne znamenko se zapise na levou segmentovku (index 4)
    if (znamenko == true)    D_pamet[4] = D_pamet[4] | 0b01000000;                    // k puvodnimu znaku se prida znamenko "-" (napriklad pri prubeznem mereni teploty se nemaze levy horni segment)


    if (styl != 8  and styl != 25)
      {
        //  ------------------ rad tisicu ----------------------
        if (pozice_tecky == 3) zobraz_nuly = true;
        if (cislo > 999)
          {
            D_pamet[3] = graf_def[cislo / 1000];
            cislo = cislo % 1000;
            zobraz_nuly = true;
          }
        else
          {
            if (zobraz_nuly == true)                          D_pamet[3] = graf_def[0];   // Znak '0'
            else                                              D_pamet[3] = 0;             // nebo prazdny znak
          }
      }
          

    //  ------------------- rad stovek ----------------------
    if (pozice_tecky == 2) zobraz_nuly = true;

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


    //  -------------------- rad desitek --------------------
    if (pozice_tecky == 1) zobraz_nuly = true;

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

    if (styl == 2)                                                                    // zobrazeni "L   0" az "L  -99" pro kladne cislo 0 az 99
      {
        if (posledni_n > 0)  D_pamet[2] = 64;                                         // minus na prostredni sedmisegmentovku
        else                 D_pamet[2] =  0;                                         // na prostredni sedmisegmentovku nic
        pozice_tecky = 0;                                                             // tecka se v tomto pripade nezobrazuje
      }


    pripoj_tecku();

    if (styl == 3)                                                                    // zobrazeni cisla ve formatu "-nn- "
      {
        D_pamet[4] = 64;                                                              // na prvni sedmisegmentovku zleva (rad desetitisicu) se zapise pomlcka
        D_pamet[1] = 64;                                                              // na druhou sedmisegmentovku zprava (rad desitek) se zapise pomlcka
        D_pamet[0] =  0;                                                              // prvni sedmisegmentovka zprava (rad jednotek) se smaze
      }

    if (predtext == 1)
      {
        D_pamet[4] = 246;                                                             // na prvni sedmisegmentovku zleva (rad desetitisicu) znak 'H.'
        D_pamet[3] = 241;                                                             // na druhou sedmisegmentovku zleva (rad tisicu) znak 'F.'
      }
    
    if (predtext == 2)
      {
        D_pamet[4] = 80;                                                              // na prvni sedmisegmentovku zleva (rad desetitisicu) znak 'r'
        D_pamet[3] = 222;                                                             // na druhou sedmisegmentovku zleva (rad tisicu) znak 'd.'
      }

    if (predtext == 3)
      {
        D_pamet[4] = 237;                                                             // na prvni sedmisegmentovku zleva (rad desetitisicu) znak 'S.'
        D_pamet[3] = 241;                                                             // na druhou sedmisegmentovku zleva (rad tisicu) znak 'F.'
      }

    if (predtext == 4)
      {
        if (znamenko == true)    D_pamet[4] = 0b01100000;                             // na prvni sedmisegmentovku zleva (rad desetitisicu) znak pro stupen a minus
        else                     D_pamet[4] = 0b00100000;                             // na prvni sedmisegmentovku zleva (rad desetitisicu) jen znak pro stupen
        if (stabilni_naklon == false)   D_pamet[4] = D_pamet[4] | 128 ;               // nestabilni naklon rozsviti tecku na prvnim displeji
      }


    if (predtext == 5)
      {
        D_pamet[4] = znak_eS;                                                         // na prvni sedmisegmentovku zleva (rad desetitisicu) znak "S" (Slunce)
        D_pamet[3] = znak_Etecka;                                                     // druha sedmisegmentovka je znak "E." s teckou jako "Elevace"
        
        if (znamenko == true)    D_pamet[2] = 64;                                     // zaporne hodnoty maji znamenko na treti sedmisegmentovce zleva
        else                     D_pamet[2] =  0;                                     // kladne hodnoty maji praznou sedmisegmentovku
      }


    if (predtext == 6)
      {
        D_pamet[4] = znak_eM;                                                         // na prvni sedmisegmentovku zleva (rad desetitisicu) znak "M" (Mesic)
        D_pamet[3] = znak_Etecka;                                                     // druha sedmisegmentovka je znak "E." s teckou jako "Elevace"
        
        if (znamenko == true)    D_pamet[2] = 64;                                     // zaporne hodnoty maji znamenko na treti sedmisegmentovce zleva
        else                     D_pamet[2] =  0;                                     // kladne hodnoty maji praznou sedmisegmentovku
      }

    if (predtext == 7)
      {
        D_pamet[4] = znak_eM;                                                         // na prvni sedmisegmentovku zleva (rad desetitisicu) znak "M" (jako Mesic)
        D_pamet[3] = znak_oM;                                                         // druha sedmisegmentovka je znak "o." s teckou (jako osvetleni)
        D_pamet[2] =   0;                                                             // treti sedmisegmentovka je prazdna (vzdycky, protoze zaporne osvetleni neni mozne)
      }


    if (predtext == 8)
      {
        D_pamet[4] = znak_eS;                                                         // na prvni sedmisegmentovku zleva (rad desetitisicu) znak "S" (Slunce)
        D_pamet[3] = znak_Atecka;                                                     // druha sedmisegmentovka zobrazuje znak A s teckou (Azimut)
      }

    if (predtext == 9)
      {
        D_pamet[4] = znak_eM;                                                         // na prvni sedmisegmentovku zleva (rad desetitisicu) znak "M" (Mesic)
        D_pamet[3] = znak_Atecka;                                                     // druha sedmisegmentovka zobrazuje znak A s teckou (Azimut)
      }

    if (predtext == 10)
      {
        D_pamet[4] = 0;                                                               // prvni segmentovka zleva bude prazdna
        D_pamet[3] = znak_Atecka;                                                     // druha sedmisegmentovka zobrazuje znak A s teckou (Azimut)

        if (problem_azimutu == true)                                                  // pri problemu s prumerovanim azimutu (nestabilni hodnoty)
          {                                                                           // se k nezprumerovane posledni hodnote azimutu zobrazi i vystrazne tecky
             D_pamet[4] = D_pamet[4] + 128;                                           //    prvni prazdna segmentovka 
             D_pamet[2] = D_pamet[2] + 128;                                           //    stovky stupnu 
             D_pamet[1] = D_pamet[1] + 128;                                           //    desitky stupnu
             D_pamet[0] = D_pamet[0] + 128;                                           //    jednotky stupnu
          }
      }

    if (predtext == 11)
      {
        D_pamet[4] = znak_C;                                                          // prvni sedmisegmentovka zobrazuje znak "C"
        D_pamet[3] = znak_A;                                                          // druha sedmisegmentovka zobrazuje znak "A"
        D_pamet[2] = znak_Ltecka;                                                     // treti sedmisegmentovka zobrazuje znak "L."
      }

    if (predtext == 12)
      {
        D_pamet[4] = znak_R1;                                                         // prvni segmentovka zleva zobrazuje znak "r"
        D_pamet[3] = znak_R2;                                                         // druha sedmisegmentovka zobrazuje znak "A."
      }

    if (predtext == 13)
      {
        D_pamet[4] = znak_D1;                                                         // prvni segmentovka zleva zobrazuje znak "d"
        D_pamet[3] = znak_D2;                                                         // druha sedmisegmentovka zobrazuje znak "E."
        if (znamenko == true)    D_pamet[2] = 64;                                     // zaporne hodnoty maji znamenko na treti sedmisegmentovce zleva
        else                     D_pamet[2] =  0;                                     // kladne hodnoty maji praznou sedmisegmentovku
      }

    if (predtext == 14)
      {
        D_pamet[4] = 134;                                                         // prvni segmentovka zleva zobrazuje znak "I."
        D_pamet[3] = 72;                                                          // druha sedmisegmentovka zobrazuje znak "="
        switch (posledni_n)
          {
            case 251:                                                             // specialita "i= 5M"
              D_pamet[2] = 0;                                                     // prostredni segmentovka zobrazuje mezeru " "
              D_pamet[1] = 109;                                                   // ctvrta segmentovka zleva zobrazuje znak "5"
              D_pamet[0] = 55;                                                    // posledni segmentovka zobrazuje znak "M"
              break;               

            case 252:                                                             // specialita "i= 7M"
              D_pamet[2] = 0;                                                     // prostredni segmentovka zobrazuje mezeru " "
              D_pamet[1] = 7;                                                     // ctvrta segmentovka zleva zobrazuje znak "7"
              D_pamet[0] = 55;                                                    // posledni segmentovka zobrazuje znak "M"
              break;               

            case 253:                                                             // specialita "i=10M"
              D_pamet[2] = 6;                                                     // prostredni segmentovka zobrazuje mezeru "1"
              D_pamet[1] = 63;                                                    // ctvrta segmentovka zleva zobrazuje znak "0"
              D_pamet[0] = 55;                                                    // posledni segmentovka zobrazuje znak "M"
              break;               
          
            case 254:                                                             // specialita "i=20M"
              D_pamet[2] = 91;                                                    // prostredni segmentovka zobrazuje mezeru "2"
              D_pamet[1] = 63;                                                    // ctvrta segmentovka zleva zobrazuje znak "0"
              D_pamet[0] = 55;                                                    // posledni segmentovka zobrazuje znak "M"
              break;               

            case 255:                                                             // specialita "i=20M"
              D_pamet[2] = 79;                                                    // prostredni segmentovka zobrazuje mezeru "2"
              D_pamet[1] = 63;                                                    // ctvrta segmentovka zleva zobrazuje znak "0"
              D_pamet[0] = 55;                                                    // posledni segmentovka zobrazuje znak "M"
              break;               
          
          }
          
      }

    if (predtext == 15)
      {
        D_pamet[4] = znak_AD1;                                                    // prvni segmentovka zleva zobrazuje znak "A."
        D_pamet[3] = znak_AD2;                                                    // druha sedmisegmentovka zobrazuje znak "d."
      }

    if (predtext == 16)
      {
        if (D_pamet[4] == graf_def[0]) D_pamet[4]= 0;                             // pripadna nula se na prvni segmentovce zleva maze
        D_pamet[2] = 0;                                                           // prostredni segmentovka se vzdycky maze
        if (D_pamet[1] == graf_def[0]) D_pamet[1]= 0;                             // pripadna nula se na druhe segmentovce zprava maze        
      }


    if (predtext == 17)
      {
        D_pamet[4] = dis_popisy[100][0];                                     // 'b'
        D_pamet[3] = dis_popisy[100][1];                                     // 'o'
        D_pamet[2] = dis_popisy[100][2];                                     // 'r.'
        D_pamet[1] = dis_popisy[100][3];                                     // '='
      }

    if (predtext == 18)
      {
        D_pamet[4] = dis_popisy[101][0];                                     // 'M'             ('n')
        D_pamet[3] = dis_popisy[101][1];                                     // 'H'             ('E')
        D_pamet[2] = dis_popisy[101][2];                                     // 'V.'            ('L.')
      }

    if (predtext == 19)                                                           // styl 8 obnovuje prvni dva znaky na displeji
      {
        D_pamet[4] = vstup_seg1;
        D_pamet[3] = vstup_seg2;
      }

    if(styl == 20)
      {
        if (posledni_n < 10)  D_pamet[1] = graf_def[0];                           // kdyz jsou nulova procenta, tak se ta nula na 2. segmentovce zprava zobrazi
        D_pamet[0] = 36;                                                          // posledni segmentovka zobrazuje znak "%"
      }
    
    aktualizuj_displej();                                                         // Presype cele pole 'D_pamet[]' po seriove komunikaci do displeje

  }
//---------------------- konec obsluhy displeje 5x7 znaku -------------------






//----------------------------------------------
// K jedne sedmisegmentovce prida jeste desetinnou tecku, ostatni tecky smaze
void pripoj_tecku(void)
  {
    for (uint8_t i = 1; i < 4 ; i++)                                                     // (na nulte pozici tecka nikdy nesviti, proto zacina smycka od 1)
      {
        if (pozice_tecky == i) D_pamet[i] = D_pamet[i] | 0b10000000;                  // rozsvit nejvyssi bit
        else                   D_pamet[i] = D_pamet[i] & 0b01111111;                  // zhasni nejvyssi bit
      }
  }
//----------------------------------------------



//----------------------------------------------
// Na 5 mistnem displeji provede postupne rozsviceni vodorovnych segmentu
// Zaplnuje se od leveho horniho rohu do praveho horniho rohu
//    pak se ve stejnem smeru prida stredni rada
//      a na zaver spodni rada (dohromady se tedy muze rozsvitit az 15 segmentu) 
// Pouziva se pro operace, kde je nutne nejakou dobu drzet tlacitko pred tim, nez se aktivuje nejaka "nebezpecna" akce (napriklad kalibrace magnetometru).
// Behem drzeni je mozne tlacitko uvolnit a akce se nespusti.
//  Prvni parametr je cislo 0 az ....
//  Druhy parametr udava, kolik cisel pripada na jeden z 15 segmentu
//    priklad: vstupni_cislo je pocet milisekund od stisku tlacitka, a delitel je 133. Kazdy nasledujici segment se tedy rozsviti po 133ms. Vsech 15 vodorovnych segmentu bude svitit po 2 sekundach
void graf_natazeni(uint32_t vstupni_cislo , uint32_t delitel)
  {
    uint8_t krok_grafu = vstupni_cislo / delitel;
    uint8_t segmentova_radka;
    uint8_t podklad;
    if (krok_grafu < 5)
      {
        podklad = 0;                                                                  // vsechny zobrazovaci jednotky se smazou
        segmentova_radka = 1;                                                         // horni segmenty
      }
    if (krok_grafu >= 5 and krok_grafu < 10)
      {
        podklad = 1;                                                                  // vsechny zobrazovaci jednotky budou mit rozsviceny horni segment
        segmentova_radka = 64;                                                        // prostredni segmenty
      }
    if (krok_grafu >= 10  and krok_grafu <= 15)
      {
        podklad = 65;                                                                 // vsechny zobrazovaci jednotky budou mit rozsviceny horni a prosredni segment
        segmentova_radka = 8;                                                         // spodni segmenty
      }

    for (uint8_t i = 0; i < 5 ; i++)
      {
        if (krok_grafu < 15)
          {
            if ((krok_grafu % 5) > i)  D_pamet[4-i] = podklad | segmentova_radka;
            else                       D_pamet[4-i] = podklad;
          }
        else
          {
            D_pamet[4-i] = 73;
          }
     }
   aktualizuj_displej();  
  }
//----------------------------------------------



//----------------------------------------------
// specialni zobrazeni pro alarmy:  "b. 2  0. 4  5" pro budik ve 20:45
//                             nebo "b. -  -  -  3" pro treti neaktivni budik
void zobraz_ho_mi(uint16_t pocet_minut, uint8_t poradi_budiku)
  {
    D_pamet[4] = znak_b_tecka;                  // "b."
    if (pocet_minut == 1440)                    // neaktivni budik
      {
        D_pamet[3] = 64;                        // "-"
        D_pamet[2] = 64;                        // "-"
        D_pamet[1] = 64;                        // "-"
        D_pamet[0] = graf_def[poradi_budiku];   // cislo budiku (1 az 5)
      }
    else
      {
        uint8_t pomhod = pocet_minut / 60;
        D_pamet[3] = graf_def[pomhod / 10];             // desitky hodin
        D_pamet[2] = graf_def[pomhod % 10] + 128;       // jednotky hodin s teckou
        uint8_t pommin = pocet_minut % 60;
        D_pamet[1] = graf_def[pommin / 10];             // desitky minut
        D_pamet[0] = graf_def[pommin % 10];             // jednotky minut bez tecky

      }

    aktualizuj_displej();      
  }


// zobrazi na displeji pocet minut mezi aktualnim casem v DS3231 a poctem minutu od pulnoci  (zadany parametr) 
void zobraz_odpocmi(uint16_t pocet_minut_od_pulnoci)
  {
    if (pocet_minut_od_pulnoci == 1440)         // neaktivni odpocet
      {
        D_pamet[2] = 64;                        // "-"
        D_pamet[1] = 64;                        // "-"
        D_pamet[0] = 64;                        // "-"
      }
    else
      {
        uint16_t zobraz_minuty = zbyvajici_minuty(pocet_minut_od_pulnoci);        // vypocte rozdil mezi aktualnim casem a poctem minut od pulnoci v EEPROM
        zobraz_cislo(zobraz_minuty, 8);         // styl zobrazeni zachovava prvni 2 segmentovky zleva a zobrazovane cislo muze byt v rozsahu 0 az 999
      }

    D_pamet[4] = znak_o_tecka;                  // "o."
    D_pamet[3] = 64;                            // "-"

    aktualizuj_displej();      
  }    



//----------------------------------------------
// specialni zobrazeni pro vychody a zapady planet:                   [typ = 0]   "^. 2  0. 4  5" pro vychod planety ve 20:45 (nebo konec astronomicke noci)
//                                              nebo                  [typ = 0]   "u. 1  4. 2  8" pro zapad planety ve 14:28  (nebo zacatek astronomicke noci)

//     pripadne bez tecky mezi hodinami a minutami                    [typ = 1]   "^.    7  5  3" pro vychod za 753 minut
//                                                                    [typ = 1]   "u.       1  5" pro zapad za 15 minut

//     pripadne se znakem 'A.' (na jakem azimutu k udalosti dojde)    [typ = 2]   "^. A.    9  8" pro azimut vychodu 98 stupnu

//  aktualni azimut ma znacku "A." na prvni pozici a nasleduje mezera [typ = 3]   "A.    1  3  5" pro aktualni azimut 135 stupnu

//  aktualni elevace ma nzacku "E." na prvni pozici. Kladna elevace   [typ = 4]   "E.       3  8" pro aktualni elevaci 38 stupnu
//                                                  Zaporna elevace   [typ = 4]   "E.    -     6" pro aktualni elevaci -6 stupnu znamenko je vzdycky na prostredni segmentovce

//  vzdalenost v AU ma znacku "d." na prvni pozici.                   [typ = 5]   "d. 3  5. 7  4" pro 35,74 AU
//                nemusi obsahovat uvodni nulu na pozici desitek AU   [typ = 5]   "d.    0. 9  3" pro vzdalenost 0,93 AU

//              pro vzdalenost Mesice se misto AU zobrazuji tisice km [typ = 6]   "d. 3  7  5. 3" pro vzdalenost 375 300 km


//  vzdalenost Mesice v procentech rozsahu Peri-Apo.                  [typ = 7]   "d. %.    3  2" pro 32%

//  vzdalenost Slunce v procentech rozsahu Peri-Apo.                  [typ = 8]   "d. %.    6  5" pro 65%

//  osvetleni Mesice se zobrazuje se znackou %                        [typ = 9]   "o.%.     5  6" pro osvetleni 56%
//                 pripadne uvodni nuly se mazou                      [typ = 9]   "o.%.        8" pro osvetleni 8%                         

//     doba trvani aktualni, nebo nasledujici astronomicke noci       [typ = 10]  "t.    6  8  8" tato noc (nebo pristi noc) bude trvat 688 minut

//     zacatek intervalu astronomicke noci (vecerni cas)              [typ = 11]  "   2  3. 1  2" pro zacatek aktualni noci ve 23:12
//     konec intervalu astronomicke noci (vecerni cas)                [typ = 12]  "      3. 4  5" pro konec aktualni noci ve 3:45

//     zobrazi elongaci Venuse                                        [typ = 13]  "E  L. r. 2  7" pro zapadni elongaci 27 stupnu (Venuse je videt rano)
//     zobrazi elongaci Venuse                                        [typ = 13]  "E  L. V. 3  5" pro vychodni elongaci 35 stupnu (Venuse je videt vecer)

//  osvetleni Venuse se zobrazuje se znackou %                        [typ = 14]  "o.%.  9  5. 6" pro osvetleni 95,6%

//  Magnituda planety                                                 [typ = 15]  "o.%.  9  5. 6" pro osvetleni 95,6%

//  Colongitudo Mesice                                                [typ = 16]  "c o.  2  4  8" pro colongiudo 248 stupnu




// vypoctene hodnoty se prebiraji z globalnich promennych.
// parametr 'typ' voli jeden ze 17 typu zobrazeni ( 0=cas v hod:min          ; 1=cas za min               ;  2=azimut vychodu/zapadu      ;  3=aktualni azimut ; 
//                                                  4=aktualni elevace       ; 5=aktualni vzdalenost v AU ;  6=aktualni vzdalenost Mesice ;  7=% vzdalenosti Mesice  ;
//                                                  8=%vzdalenosti Slunce    ; 9=% osvitu Mesice          ; 10=trvani astronomicke noci   ; 11=zacatek astronomicke noci
//                                                 12=konec astronomicke noci;13=elongace Venuse          ; 14=%osvetleni Venuse          ; 15=osvetleni Mesice
//                                                 16=colongitudo Mesice)
void zobraz_vyzapla(uint8_t typ)
  {
    if (typ == 0)                                            // zobrazeni v hodinach a minutach
      {
        if (SIM_vyza == 0)                                   // specialni pripad, kdy nedojde k vychodu ani zapadu behem 24 nasledujicich hodin
          {
            D_pamet[4] = 0;
            D_pamet[3] = 64;                                 // pomlcka na desitkach hodin
            D_pamet[2] = 64 + 128;                           // pomlcka na jednotkach hodin s teckou
            D_pamet[1] = 64;                                 // pomlcka na desitkach minut
            D_pamet[0] = 64;                                 // pomlcka na jednotkach minut bez tecky
          }
        else                                                 // behem nasledujicich 24 hodin k nejakemu vychodu nebo zapadu dojde
          {
            if (SIM_vyza == 1) D_pamet[4] = 163;             // na prvni segmentovce bude znak vychodu planety s teckou ('^.')
            if (SIM_vyza == 2) D_pamet[4] = 156;             // na prvni segmentovce bude znak zapadu planety s teckou ('u.')
            D_pamet[3] = graf_def[SIM_cas_h / 10];           // desitky hodin
            D_pamet[2] = graf_def[SIM_cas_h % 10] + 128;     // jednotky hodin s teckou
            D_pamet[1] = graf_def[SIM_cas_m / 10];           // desitky minut
            D_pamet[0] = graf_def[SIM_cas_m % 10];           // jednotky minut bez tecky
            if ((SIM_cas_h / 10 == 0)) D_pamet[3] = 0;       // zhasnuti segmentovky na pozici desitek hodin - v pripade, ze bude nulovy
          }          
      }






    if (typ == 1)                                            // zobrazeni jen v minutach
      {
        if (SIM_vyza == 0)                                   // specialni pripad, kdy nedojde k vychodu ani zapadu behem 24 nasledujicich hodin
          {
            D_pamet[4] = 0;
            D_pamet[3] = 64;                                 // pomlcka na tisicich minut
            D_pamet[2] = 64;                                 // pomlcka na stovkach minut
            D_pamet[1] = 64;                                 // pomlcka na desitkach minut
            D_pamet[0] = 64;                                 // pomlcka na jednotkach minut
          }
        else                                                 // behem nasledujicich 24 hodin k nejakemu vychodu nebo zapadu dojde
          {
            if (SIM_vyza == 1) D_pamet[4] = 163;             // na prvni segmentovce bude znak vychodu planety s teckou ('^.')
            if (SIM_vyza == 2) D_pamet[4] = 156;             // na prvni segmentovce bude znak zapadu planety s teckou ('u.')
            
            int16_t pomprom_zamin = SIM_zamin;
            
            D_pamet[3] = graf_def[pomprom_zamin / 1000];                              // tisice minut
            pomprom_zamin = pomprom_zamin - (1000 * (pomprom_zamin / 1000));          // vymazani tisicu minut

            D_pamet[2] = graf_def[pomprom_zamin / 100];                               // stovky minut
            pomprom_zamin = pomprom_zamin - (100 * (pomprom_zamin / 100));            // vymazani stovek minut

            D_pamet[1] = graf_def[pomprom_zamin / 10];                                // desitky minut
            pomprom_zamin = pomprom_zamin - (10 * (pomprom_zamin / 10));              // vymazani desitek minut

            D_pamet[0] = graf_def[pomprom_zamin];                                     // jednotky minut

            if (SIM_zamin < 1000)        D_pamet[3] = 0;                              // pripadne uvodni nuly pred poctem minut se mazou
            if (SIM_zamin <  100)        D_pamet[2] = 0;
            if (SIM_zamin <   10)        D_pamet[1] = 0;            
          }          
      }

      
    if (typ == 2)                                            // zobrazeni azimutu vychodu nebo zapadu
      {
        if (SIM_vyza == 0)                                   // specialni pripad, kdy nedojde k vychodu ani zapadu behem 24 nasledujicich hodin
          {
            D_pamet[4] = 0;
            D_pamet[3] = 247;                                // znak "A." na druhe segmentovce
            D_pamet[2] = 64;                                 // pomlcka na stovkach stupnu
            D_pamet[1] = 64;                                 // pomlcka na desitkach stupnu
            D_pamet[0] = 64;                                 // pomlcka na jednotkach stupnu
          }
        else                                                 // behem nasledujicich 24 hodin k nejakemu vychodu nebo zapadu dojde
          {
            if (SIM_vyza == 1) D_pamet[4] = 163;             // na prvni segmentovce bude znak vychodu planety s teckou ('^.')
            if (SIM_vyza == 2) D_pamet[4] = 156;             // na prvni segmentovce bude znak zapadu planety s teckou ('u.')
            D_pamet[3] = 247;                                // znak "A." na druhe segmentovce

            int16_t pomprom_azim = SIM_azim;
            
            D_pamet[2] = graf_def[pomprom_azim / 100];                               // stovky stupnu
            pomprom_azim = pomprom_azim - (100 * (pomprom_azim / 100));              // vymazani stovek stupnu

            D_pamet[1] = graf_def[pomprom_azim / 10];                                // desitky stupnu
            pomprom_azim = pomprom_azim - (10 * (pomprom_azim / 10));                // vymazani desitek stupnu

            D_pamet[0] = graf_def[pomprom_azim];                                     // jednotky stupnu

            if (SIM_azim <  100)        D_pamet[2] = 0;                              // pripadne uvodni nuly pred poctem stupnu se mazou
            if (SIM_azim <   10)        D_pamet[1] = 0; 
          }
      }



            
    if (typ == 3)                                          // zobrazeni aktualniho azimutu
      {
        D_pamet[4] = 247;                                  // znak "A." na prvni segmentovce
        D_pamet[3] = 0;                                    // druha segmentovka je vzdycky prazdna
        
        int16_t pomprom_azim = SIM_aktaz;
        
        D_pamet[2] = graf_def[pomprom_azim / 100];                                   // stovky stupnu
        pomprom_azim = pomprom_azim - (100 * (pomprom_azim / 100));                  // vymazani stovek stupnu

        D_pamet[1] = graf_def[pomprom_azim / 10];                                    // desitky stupnu
        pomprom_azim = pomprom_azim - (10 * (pomprom_azim / 10));                    // vymazani desitek stupnu

        D_pamet[0] = graf_def[pomprom_azim];                                         // jednotky stupnu

        if (SIM_aktaz <  100)        D_pamet[2] = 0;                                 // pripadne uvodni nuly pred poctem stupnu se mazou
        if (SIM_aktaz <   10)        D_pamet[1] = 0; 
      }
      
    
    if (typ == 4)                                            // zobrazeni aktualni elevace
      {
        D_pamet[4] = 249;                                    // znak "E." na prvni segmentovce
        D_pamet[3] = 0;                                      // druha segmentovka je vzdycky prazdna

        int16_t pomprom_elev = SIM_aktel;

        if (pomprom_elev < 0) D_pamet[2] = 64;               // treti segmentovka obsahuje znamenko
        else                  D_pamet[2] = 0;                //   nebo nic (pri kladne elevaci)
        
        pomprom_elev = abs(pomprom_elev);                    // po zobrazeni znamenka se bude s elevaci pracovat jako s kladnym cislem
        
        D_pamet[1] = graf_def[pomprom_elev / 10];                                    // desitky stupnu
        pomprom_elev = pomprom_elev - (10 * (pomprom_elev / 10));                    // vymazani desitek stupnu

        D_pamet[0] = graf_def[pomprom_elev];                                         // jednotky stupnu

        if (abs(SIM_aktel) <   10)        D_pamet[1] = 0;                            // pripadne uvodni nuly pred poctem stupnu se mazou
      }    


    if (typ == 5)                                            // zobrazeni aktualni vzdalenosti v AU
      {
        D_pamet[4] = 222;                                    // znak "d." na prvni segmentovce

        int16_t pomprom_vzd = SIM_aktvz;
        
        D_pamet[3] = graf_def[pomprom_vzd / 1000];                         // desitky AU
        pomprom_vzd = pomprom_vzd - (1000 * (pomprom_vzd / 1000));         // vymazani desitek AU

        D_pamet[2] = graf_def[pomprom_vzd / 100] + 128;                    // jednotky AU s desetinnou teckou
        pomprom_vzd = pomprom_vzd - (100 * (pomprom_vzd / 100));           // vymazani jednotek AU

        D_pamet[1] = graf_def[pomprom_vzd / 10];                           // desetiny AU
        pomprom_vzd = pomprom_vzd - (10 * (pomprom_vzd / 10));             // vymazani desetin AU

        D_pamet[0] = graf_def[pomprom_vzd];                                // setiny AU

        if (SIM_aktvz < 1000)     D_pamet[3] = 0;                          // pripadna uvodni nula na pozici desitek AU se maze
      }    

    if (typ == 6)                                            // zobrazeni aktualni vzdalenosti Mesice v tisicich km
      {
        D_pamet[4] = 222;                                    // znak "d." na prvni segmentovce

        int16_t pomprom_vzd = SIM_aktvz;
        
        D_pamet[3] = graf_def[pomprom_vzd / 1000];                         // stovky tisic km
        pomprom_vzd = pomprom_vzd - (1000 * (pomprom_vzd / 1000));         // odecteni stovek tisic km

        D_pamet[2] = graf_def[pomprom_vzd / 100];                          // desitky tisic km
        pomprom_vzd = pomprom_vzd - (100 * (pomprom_vzd / 100));           // odecteni desitek tisic km

        D_pamet[1] = graf_def[pomprom_vzd / 10] + 128;                     // jednotky tisic km s desetinnou teckou
        pomprom_vzd = pomprom_vzd - (10 * (pomprom_vzd / 10));             // odecteni jednotek tisic km

        D_pamet[0] = graf_def[pomprom_vzd];                                // stovky km (jeden rad za desetinnou teckou)
      }    

    if (typ == 7)                                            // zobrazeni aktualni vzdalenosti Mesice v procentech perigea a apogea
      {
        D_pamet[4] = 222;                                    // znak "d." na prvni segmentovce
        D_pamet[3] = 36 + 128;                               // znak "%." na druhe segmentovce

        // prepocet z dlouhodobe minimalni a maximalni vzdalenosti Mesice od Zeme na procenta tohoto intervalu
        // zdroj dat:  https://en.wikipedia.org/wiki/Lunar_distance
        //         Jean Meeus gives the following extreme values for 1500 BC to AD 8000:
        //               greatest distance: 406 719.97 km on January 7, AD 2266
        //               smallest distance: 356 352.93 km on November 13, 1054 BC        
        
        uint8_t procenta_delky = 100 * (((SIM_aktvzd_presne - 356.353) / (406.720 - 356.353))+ 0.005);   // vzdalenosti jsou v tisicich km, 
        procenta_delky = constrain(procenta_delky,0,100);
        uint8_t pompromvzd = procenta_delky;                                                             // pomocna promenna kvuli mazani uvodnich nul
        
       
        D_pamet[2] = graf_def[procenta_delky / 100];                       // stovky procent
        procenta_delky = procenta_delky - (100 * (procenta_delky / 100));  // odecteni stovek procent

        D_pamet[1] = graf_def[procenta_delky / 10];                        // desitky procent
        procenta_delky = procenta_delky - (10 * (procenta_delky / 10));    // odecteni desitek procent

        D_pamet[0] = graf_def[procenta_delky];                             // jednotky procent

        if (pompromvzd < 100) D_pamet[2] = 0;                              // pripadne smazani uvodni nuly na stovkach procent
        if (pompromvzd < 10)  D_pamet[1] = 0;                              // pripadne smazani uvodni nuly na desitkach procent

      }    

    if (typ == 8)                                            // zobrazeni aktualni vzdalenosti Slunce v procentech perihelu a apohelu
      {
        D_pamet[4] = 222;                                    // znak "d." na prvni segmentovce
        D_pamet[3] = 36 + 128;                               // znak "%." na druhe segmentovce

        
        // zdroj dat: https://www.astropixels.com/ephemeris/perap/perap2401.html
        //  Earth's distance from the Sun (center-to-center) varies with
        //   mean values of 0.9832899 AU (147,098,074 km) at perihelion (closest)
        //               to 1.0167103 AU (152,097,701 km) at aphelion (most distant). 
        
        uint8_t procenta_delky = 100 * (((SIM_aktvzd_presne - 0.9832899) / (1.0167103 - 0.9832899))+ 0.005);
        procenta_delky = constrain(procenta_delky,0,100);
        uint8_t pompromvzd = procenta_delky;                                                             // pomocna promenna kvuli mazani uvodnich nul

       
        D_pamet[2] = graf_def[procenta_delky / 100];                       // stovky procent
        procenta_delky = procenta_delky - (100 * (procenta_delky / 100));  // odecteni stovek procent

        D_pamet[1] = graf_def[procenta_delky / 10];                        // desitky procent
        procenta_delky = procenta_delky - (10 * (procenta_delky / 10));    // odecteni desitek procent

        D_pamet[0] = graf_def[procenta_delky];                             // jednotky procent

        if (pompromvzd < 100) D_pamet[2] = 0;                              // pripadne smazani uvodni nuly na stovkach procent
        if (pompromvzd < 10)  D_pamet[1] = 0;                              // pripadne smazani uvodni nuly na desitkach procent
      }    

    if (typ == 9)                                            // Osvetleni Mesice v %
      {
        int16_t pomprom_osvit = SIM_osvit;
        
        D_pamet[4] = znak_oM;                                              // prvni segmentovka obsahuje znak male 'o.' jako osvetleni. V ENG verzi je 'i.' jako illumination
        D_pamet[3] = 36 + 128;                                             // znak "%." na druhe segmentovce

        D_pamet[2] = graf_def[pomprom_osvit / 100];                        // stovky procent osvitu (k tomu obvykle nedojde, protoze 100% je jen na kratkou chvili)
        pomprom_osvit = pomprom_osvit - (100 * (pomprom_osvit / 100));     // odecteni stovek procent

        D_pamet[1] = graf_def[pomprom_osvit / 10];                         // desitky procent osvitu
        D_pamet[0] = graf_def[pomprom_osvit % 10];                         // jednotky procent osvitu
        if (SIM_osvit < 100) D_pamet[2] = 0;                               // pripadne smazani uvodni nuly na stovkach procent
        if (SIM_osvit < 10)  D_pamet[1] = 0;                               // pripadne smazani uvodni nuly na desitkach procent
      }    


    if (typ == 10)                                           // trvani astronomicke noci v minutach
      {
        int16_t pomprom_trvani = astro_noc_trvani;                                     // trvani muze byt i nulove
        D_pamet[4] = 120+128;                                                      // na prvni segmentovce bude znak 't.' jako trvani. Sice to nesedi s anglickou verzi (duration),
                                                                                   //    ale pak by se to mohlo plest s 'd.' jako distance, ktera se normalne pouziva u planet
        
        D_pamet[3] = graf_def[pomprom_trvani / 1000];                              // tisice minut
        pomprom_trvani = pomprom_trvani - (1000 * (pomprom_trvani / 1000));        // vymazani tisicu minut
  
        D_pamet[2] = graf_def[pomprom_trvani / 100];                               // stovky minut
        pomprom_trvani = pomprom_trvani - (100 * (pomprom_trvani / 100));          // vymazani stovek minut
  
        D_pamet[1] = graf_def[pomprom_trvani / 10];                                // desitky minut
        pomprom_trvani = pomprom_trvani - (10 * (pomprom_trvani / 10));            // vymazani desitek minut
  
        D_pamet[0] = graf_def[pomprom_trvani];                                     // jednotky minut
  
        if (astro_noc_trvani < 1000)        D_pamet[3] = 0;                        // pripadne uvodni nuly pred poctem minut se mazou
        if (astro_noc_trvani <  100)        D_pamet[2] = 0;
        if (astro_noc_trvani <   10)        D_pamet[1] = 0;            
      }


    if (typ == 11 or typ == 12)                              // zacatek nebo konec probihajici nebo nasledujici astronomicke noci v hh:mm
      {
        D_pamet[4] = 0;                                      // v tomto zobrazeni je prvni segmentovka vzdycky prazdna

        int16_t pomprom_minuty_od_pulnoci;
        if (typ == 11) pomprom_minuty_od_pulnoci = astro_noc_zacatek;
        else           pomprom_minuty_od_pulnoci = astro_noc_konec;
        
        if (pomprom_minuty_od_pulnoci == 9999)               // specialni pripad, kdy noc nenastava
          {
            D_pamet[3] = 64;                                 // pomlcka na desitkach hodin
            D_pamet[2] = 64 + 128;                           // pomlcka na jednotkach hodin s teckou
            D_pamet[1] = 64;                                 // pomlcka na desitkach minut
            D_pamet[0] = 64;                                 // pomlcka na jednotkach minut bez tecky
          }
        else                                                 // behem nasledujicich 24 hodin k nejakemu vychodu nebo zapadu dojde
          {
            int16_t pomprom_jen_hodiny = pomprom_minuty_od_pulnoci / 60;   // z celkoveho poctu minut od pulnoci se vezmou jen cele hodiny
            int16_t pomprom_jen_minuty = pomprom_minuty_od_pulnoci % 60;   // z celkoveho poctu minut od pulnoci se vezmou jen zbyvajici minuty
            D_pamet[3] = graf_def[pomprom_jen_hodiny / 10];            // desitky hodin
            D_pamet[2] = graf_def[pomprom_jen_hodiny % 10] + 128;      // jednotky hodin s teckou
            D_pamet[1] = graf_def[pomprom_jen_minuty / 10];            // desitky minut
            D_pamet[0] = graf_def[pomprom_jen_minuty % 10];            // jednotky minut bez tecky
            if ((pomprom_jen_hodiny / 10 == 0)) D_pamet[3] = 0;        // zhasnuti segmentovky na pozici desitek hodin - v pripade, ze bude nulovy
          }          
      }

    if (typ == 13)                                           // elongace vnitrnich planet (Merkur a Venuse)
      {
        D_pamet[4] =  121;                                   // znak "E" na prvni segmentovce
        D_pamet[3] =  92 + 128;                              // znak "o." na druhe segmentovce

        int16_t pomprom_elong = (int)(P_elongace + 0.5);         // zaokrouhleni na cele stupne


                                                             // ... planeta vychazi a zapada pozdeji nez Slunce, jde o vychodni elongaci (Wikipedia)

        if (P_east_elo == 1)       D_pamet[2] =  znak_vecer; // vychodni elongace (Planeta je videt vecer)  (sviti znak 'V.')
        if (P_east_elo == 0)       D_pamet[2] =  znak_rano ; // zapadni elongace  (Planeta je videt rano)  (sviti znak 'r.')
        if (pomprom_elong < 2 )    D_pamet[2] =  128;        // Planeta je velmi blizko Slunce - pod 2 stupne elongace (prazdna segmentovka s teckou)


        D_pamet[1] = graf_def[pomprom_elong / 10];           // desitky stupnu
        D_pamet[0] = graf_def[pomprom_elong % 10];           // jednotky stupnu

        if (pomprom_elong < 10)     D_pamet[1] = 0;          // pripadna uvodni nula na pozici desitek stupnu se maze
      }    


    if (typ == 14)                                           // Osvetleni Planety v %
      {
        int16_t pomprom_osvit = (int)((P_faze + 0.0005) * 1000.0 );            // zaokrouhleni na desetiny % a prevod na cele cislo 0 az 999 (to odpovida 0,0 az 99,9%)
        if (pomprom_osvit > 999) pomprom_osvit = 999;                      // 100% by se neveslo na displej, v tom pripade zustava 99,9%       
        SIM_osvit = pomprom_osvit;                                         //   jen kvuli pripadnemu mazani uvodni nuly na desitkach procent
        
        D_pamet[4] = znak_oM;                                              // prvni segmentovka obsahuje znak male 'o.' jako osvetleni. V ENG verzi je 'i.' jako illumination
        D_pamet[3] = 36 + 128;                                             // znak "%." na druhe segmentovce

        D_pamet[2] = graf_def[pomprom_osvit / 100];                        // desitky procent osvitu
        pomprom_osvit = pomprom_osvit - (100 * (pomprom_osvit / 100));     // odecteni desitek procent

        D_pamet[1] = graf_def[pomprom_osvit / 10] + 128;                   // jednotky procent osvitu s desetinnou teckou
        D_pamet[0] = graf_def[pomprom_osvit % 10];                         // desetiny procent osvitu
        if (SIM_osvit < 100) D_pamet[2] = 0;                               // pripadne smazani uvodni nuly na desitkach procent
      }   

    if (typ == 15)                                                         // Magnituda Merkuru a Venuse
      {
        int16_t pomprom_magnituda;                                             // magnituda Venuse je vzdycky v rozsahu -2 az -5. Merkur muze mit i kladnou magnitudu (treba az +6)
                                                                           // Zaokrouhluje se na setiny mag a prevede se na cele cislo -500 az +600 (to odpovida -5,00 az +6,00 mag)

        if (P_mag < 0)                                                     // magnituda je zaporna
          {
            pomprom_magnituda = (int)((P_mag - 0.005) * 100.0 );           // zaokrouhluje se o 0,005 dolu
            pomprom_magnituda = abs(pomprom_magnituda);                    // prevod na kladne cislo
            D_pamet[3] = 64;                                               // znak "-" na druhe segmentovce
          }
        else                                                               // magnituda je kladna
          {
            pomprom_magnituda = (int)((P_mag + 0.005) * 100.0 );           // zaokrouhluje se o 0,005 nahoru
            D_pamet[3] = 0;                                                // druha segmentovka je prazdna (jako kladne cislo)
          }

        D_pamet[4] = 55 + 128;                                             // prvni segmentovka obsahuje znak "M."

        D_pamet[2] = graf_def[pomprom_magnituda / 100] + 128;              // jednotky magnitudy s desetinnou teckou
        pomprom_magnituda = pomprom_magnituda - (100 * (pomprom_magnituda / 100));     // odecteni jednotek magnitudy

        D_pamet[1] = graf_def[pomprom_magnituda / 10];                     // desetiny magnitudy
        D_pamet[0] = graf_def[pomprom_magnituda % 10];                     // setiny magnitudy
      }

    if (typ == 16)                                                                 // Colongitudo mesice
      {
        D_pamet[4] =  88;                                                          // znak "c" na prvni segmentovce
        D_pamet[3] =  92 + 128;                                                    // znak "o." na druhe segmentovce

        int16_t colong = (int)(Mes_colong + 0.5);                                      // zaokrouhleni na cele stupne
        int16_t pomprom_colong = colong;
          
        D_pamet[2] = graf_def[pomprom_colong / 100];                               // stovky stupnu
        pomprom_colong = pomprom_colong - (100 * (pomprom_colong / 100));          // vymazani stovek stupnu
  
        D_pamet[1] = graf_def[pomprom_colong / 10];                                // desitky stupnu
        pomprom_colong = pomprom_colong - (10 * (pomprom_colong / 10));            // vymazani desitek stupnu
  
        D_pamet[0] = graf_def[pomprom_colong];                                     // jednotky stupnu
  
        if (colong <  100)        D_pamet[2] = 0;                                  // pripadne uvodni nuly pred stupnema se mazou
        if (colong <   10)        D_pamet[1] = 0;   
      }    


    if (typ == 17)                                                                 // terminator na Mesici
      {
        int16_t term;
        D_pamet[4] =  120;                                                         // znak "t" na prvni segmentovce
        D_pamet[3] =  121 + 128;                                                   // znak "E." na druhe segmentovce

        if (Mes_term < 0)
          {
            D_pamet[2] = 64;                                                       // treti segmentovka obsahuje znamenko
            term = (int)(Mes_term - 0.5);                                          //  zaokrouhleni zapornych cisel
          }
        else
          {
            D_pamet[2] = 0;                                                        //  treti segmentovka nezobrazuje nic (pri kladne elevaci)
            term = (int)(Mes_term + 0.5);                                          //  zaokrouhleni kladnych cisel     
          }
        
        term = abs(term);                                                          // po zobrazeni znamenka se bude s terminatorem pracovat jako s kladnym cislem
        
        int16_t pomprom_term = term;
           
        D_pamet[1] = graf_def[pomprom_term / 10];                                  // desitky stupnu
        pomprom_term = pomprom_term - (10 * (pomprom_term / 10));                  // vymazani desitek stupnu
  
        D_pamet[0] = graf_def[pomprom_term];                                       // jednotky stupnu
  
        if (term <   10)        D_pamet[1] = 0;   
      }    




    aktualizuj_displej();      
  }


// animovane kolecko pri uvodnim plneni defaultnich kalibracnich hodnot 
//         (puvodne to bylo asi 10 sekund prazdneho displeje, takze se zdalo, ze po prvnim zapnuti napajeni neco nefunguje)
void kolecko(void)
  {
    if (prumery > 0 and prumery < 21)   return;                 // kolecko se zobrazuje jen pri prvnim zapnuti napajeni, kdyz je jeste v poctu prumeru nesmyslna hodnota
    
    vyckavaci_kolecko = vyckavaci_kolecko << 1;
    if (vyckavaci_kolecko > 32) vyckavaci_kolecko = 1;
    D_pamet[0] = 0;
    D_pamet[1] = 0;
    D_pamet[2] = vyckavaci_kolecko;
    D_pamet[3] = 0;
    D_pamet[4] = 0;
    aktualizuj_displej();

    switch (vyckavaci_kolecko)                     // pro pripad, ze by nebyl k dispozici displej, blika barevne i LED a ruznymi tony i bzuk
      {
        case 1:
          strip.setPixelColor(0, 0xFF0000);
          strip.show();
          tone_X(pin_bzuk, 523, 10, 65535);
          break;

        case 2:
          strip.setPixelColor(0, 0x00FF00);
          strip.show();
          tone_X(pin_bzuk, 587, 10, 65535);
          break;

        case 4:
          strip.setPixelColor(0, 0x0000FF);
          strip.show();
          tone_X(pin_bzuk, 659, 10, 65535);
          break;

        case 8:
          strip.setPixelColor(0, 0x00FFFF);
          strip.show();
          tone_X(pin_bzuk, 698, 10, 65535);
          break;

        case 16:
          strip.setPixelColor(0, 0xFF00FF);
          strip.show();
          tone_X(pin_bzuk, 784, 10, 65535);
          break;

        case 32:
          strip.setPixelColor(0, 0xFFFF00);
          strip.show();
          tone_X(pin_bzuk, 880, 10, 65535);
          break;
      
      }
    
    delay(100);
    strip.setPixelColor(0, 0x000000);
    strip.show();

  }
