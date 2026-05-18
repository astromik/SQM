//  Podprogramy pro pripravu CSV retezce z dat v EEPROM
//=============================================



//----------------------------------------------
// z hodnot v EEPROM na zadane adrese pripravi CSV retezec zaznamu (globalni promennou 'vystupni_retezec')
void priprav_1_zaznam(uint32_t adresa)
  {
    if (adresa > 0)
      {
        formatuj_ulong(1 + ((adresa-eeaddr_MIN_EEPROM) / delka_zaznamu)  ,  5,   5);       // index polozky v EEPROM
        formatuj_ulong(adresa                                     ,  14,  6);              // adresa prvni bunky prislusneho zaznamu v EEPROM    
      }
    else                                                                                   // zaznam neni ulozen v EEPROM, protoze byl moc velky jas
      {
        formatuj_string("-----"     , 1,  5);                                              // misto indexu a adresy se zapisi pomlcky
        formatuj_string("------"    , 9,  6);
      }
    
    uint32_t cas_EEPROM;
    if (adresa > 0)   cas_EEPROM = EEPROM_read_long(adresa+1);
    else              cas_EEPROM = (pole_EEPROM[1] << 24) + (pole_EEPROM[2] << 16) + (pole_EEPROM[3] << 8) + pole_EEPROM[4];

    formatuj_ulong(cas_EEPROM                                 ,  27, 10);                  // pocet sekund od 1.1.1970
    
    breakTimeFCE(cas_EEPROM, mtt_EEPROM);                                                  // z poctu sekund od 1.1.1970 se ziska citelny datum a cas

    LOC_dvt = mtt_EEPROM.weekday + 1;
    LOC_den = mtt_EEPROM.day;
    LOC_mes = mtt_EEPROM.month;
    LOC_rok = mtt_EEPROM.year+1970;
    LOC_hod = mtt_EEPROM.hour;
    LOC_min = mtt_EEPROM.minute;
    LOC_sek = mtt_EEPROM.second;

    formatuj_ulong(LOC_den      ,  32,  2);                                                // citelny den
    vystupni_retezec[33] = '.';                                                            // tecka za dnem
    formatuj_ulong(LOC_mes      ,  35,  2);                                                // citelny mesic
    vystupni_retezec[36] = '.';                                                            // tecka za mesicem
    formatuj_ulong(LOC_rok      ,  40,  4);                                                // citelny rok

    
    formatuj_ulong(LOC_hod      ,  45,  2);                                                // citelna hodina
    vystupni_retezec[46] = ':';                                                            // dvojtecka za hodinou
    formatuj_ulong(LOC_min      ,  48,  2);                                                // citelna minuta
    vystupni_retezec[49] = ':';                                                            // dvojtecka za minutou
    formatuj_ulong(LOC_sek      ,  51,  2);                                                // citelna sekunda

    
    formatuj_string(dny[LOC_dvt],  55,  2);                                                // dva znaky pro den v tydnu

    uint8_t pomprom_nulty_bajt;
    if (adresa > 0) pomprom_nulty_bajt = EEPROM_read(adresa);
    else            pomprom_nulty_bajt = pole_EEPROM[0];

    if ((pomprom_nulty_bajt & 0b10000000) > 0)                                            // zaznam nebyl preformatovany
      {
        if ((pomprom_nulty_bajt & 0b00000001) > 0)  formatuj_string(char_leto   ,  60,  4);// oznaceni casove zony (letni cas)     "SELC"
        else                                        formatuj_string(char_zima   ,  60,  4);// oznaceni casove zony ("zimni" cas)   " SEC"
      }
    else                                                                                   // zaznam byl preformatovany (chybi nejvyssi bit prvniho bajtu)
      {
        if ((pomprom_nulty_bajt & 0b00000001) > 0)  formatuj_string(char_Xleto   ,  60,  4);   // oznaceni casove zony (letni cas) se znackou smazaneho zaznamu    "_ELC"
        else                                        formatuj_string(char_Xzima   ,  60,  4);   // oznaceni casove zony ("zimni" cas) se znackou smazaneho zaznamu  " _EC"
      }

    uint16_t pomprom_svetlo;
    if (adresa > 0) pomprom_svetlo = EEPROM_read_int(adresa + 5);
    else            pomprom_svetlo = (pole_EEPROM[5] << 8) + pole_EEPROM[6];

    formatuj_ulong(pomprom_svetlo           ,  71,  5);                                    // svetlo ve formatu uint16_t ( zmerena hodnota x1000)
    formatuj_float(citelne_svetlo(pomprom_svetlo)   , 255,  76, 2 , 3);                    // svetlo v desetinnem formatu    

    
    vystupni_retezec[77]  = desod;                                                         // desetinny oddelovac na spravnou pozici

    uint8_t pomprom_poznamka;
    if (adresa > 0)  pomprom_poznamka = EEPROM_read(adresa + 7);
    else             pomprom_poznamka = pole_EEPROM[7];

    formatuj_ulong(pomprom_poznamka >> 4                   ,  85,  2);                     // SLAVE adresa 



    uint8_t pomprom_cidlo = (pomprom_nulty_bajt & 0b00001110) >> 1;


    formatuj_ulong(pomprom_cidlo                           ,  90,  2);                     // index cidla 1 az 7 pro verzi s vice cidly na rozsirujici desce

    if (bitRead(pomprom_poznamka , 3)) formatuj_string(lng254    , 102,  8);               // bit3 obsahuje znacku, jestli byl pri mereni jas stabilni='1', nebo ne='0'
    else                               formatuj_string(lng255    , 102,  8);               //                            " NESTAB "


    pomprom_poznamka = (pomprom_poznamka & 0b00000111);                                    // textova poznamka ze spodnich 3 bitu 
                              
     String txt_poznamka = "     ";
     switch (pomprom_poznamka)
       {
         case 1:
           txt_poznamka = lng256;                                                          //"tl_UP";
           znacka_listu =  28;                                                             // znak 'u' pred vypisem na displej
           break;
         case 2:
           txt_poznamka = lng257;                                                          //"tl_DN";
           znacka_listu =  94;                                                             // znak 'd' pred vypisem na displej
           break;
         case 3:
           txt_poznamka = lng258;                                                          //"tl_OK";
           znacka_listu =  92;                                                             // znak 'o' pred vypisem na displej
           break;
         case 4:
           txt_poznamka = lng259;                                                          //"komun";
           znacka_listu =  55;                                                             // znak 'M' pred vypisem na displej
           break;
         case 5:
           txt_poznamka = lng260;                                                          //" auto";
           znacka_listu =  119;                                                            // znak 'A' pred vypisem na displej
           break;
         case 6:
           txt_poznamka = lng261;                                                          //"kalib";         // kazde z 10 kalibracnich mereni 
           znacka_listu =  88;                                                             // znak 'c' pred vypisem na displej
           break;
         case 7:
           txt_poznamka = lng262;                                                          //"Ka-Pr";    // prumer posledni kalibrace
           znacka_listu =  57;                                                             // znak 'C' pred vypisem na displej
           break;

         default:
           txt_poznamka = "-----";
       }
    formatuj_string(txt_poznamka    , 94,  5);

    
    uint16_t  indexpole = adresa + 8;

#ifdef ukladat_infra 
    uint16_t pomprom_infra;
    if (adresa > 0) pomprom_infra = EEPROM_read_int(indexpole);
    else            pomprom_infra = (pole_EEPROM[indexpole] << 8) + pole_EEPROM[indexpole+1];
    formatuj_ulong(pomprom_infra    , 117,  5);                                            // infra slozka svetla v uint16_t formatu
    indexpole = indexpole + 2;
#else
    formatuj_string("-----"    , 113,  5);                                                 // kdyz se infra slozka neuklada, poslou se do vystupniho retezce jen pomlcky
#endif

#ifdef ukladat_full
    uint16_t pomprom_full;
    if (adresa > 0) pomprom_full = EEPROM_read_int(indexpole);
    else            pomprom_full = (pole_EEPROM[indexpole] << 8) + pole_EEPROM[indexpole+1];
    formatuj_ulong(pomprom_full     , 125,  5);                                            // full slozka svetla v uint16_t formatu
    indexpole = indexpole + 2;
#else
    formatuj_string("-----"    , 121,  5);                                                 // kdyz se full slozka neuklada, poslou se do vystupniho retezce jen pomlcky
#endif

#ifdef ukladat_contr_reg
    uint8_t control;
    if (adresa > 0) control = EEPROM_read(indexpole);                                      // volitelny zaznam control registru cidla svetla
    else            control = pole_EEPROM[indexpole];
    
    if (control != 255)
      {
        formatuj_ulong(((control & 0b111) + 1) * 100       , 131,  3);                     // cas snimani svetla v [ms]
        
        String         zaz_citlivost = "     ";
        control = (control & 0b110000);                                                    // z control registru se zjisti i citlivost (zesileni) pri mereni svetla
        switch(control)                                                                    // prevede jednu ze 4 urovni citlivosti na text s nasobkem citlivosti
          {
            case 0b000000:
              zaz_citlivost = "   1x";
              break;
            case 0b010000:
              zaz_citlivost = "  25x";
              break;
            case 0b100000:
              zaz_citlivost = " 428x";
              break;
            case 0b110000:
              zaz_citlivost = "9876x";
              break;
          }
        formatuj_string(zaz_citlivost   , 135,  5);                                        // citlivost v textovem formatu        
      }
    else                                                                                   // Control Registr byl prepsany na 255, to znamena, ze se hodnoty IR,FULL, a Gain nezobrazi
      {
        formatuj_string("---"           , 129,  3);                                        // kdyz se control registr neuklada do EEPROM, zobrazi se misto casu snimani
        formatuj_string("-----"         , 135,  5);                                        //   a citlivosti jen pomlcky
//        formatuj_string("-----"         , 113,  5);                                        // FULL se take prepise pomlckami (???)  PROC? 
//        formatuj_string("-----"         , 121,  5);                                        // a IR take                      (???)  PROC? 
        
      }
    indexpole = indexpole + 1;
#else
    formatuj_string("---"           , 129,  3);                                            // kdyz se control registr neuklada do EEPROM, zobrazi se misto casu snimani
    formatuj_string("-----"         , 135,  5);                                            //   a citlivosti jen pomlcky

#endif

#ifdef ukladat_teplotu
    uint16_t pom_prom_teplota;
    if (adresa > 0) pom_prom_teplota = EEPROM_read_int(indexpole);
    else            pom_prom_teplota = (pole_EEPROM[indexpole] << 8) + pole_EEPROM[indexpole+1];

    if (pom_prom_teplota == 65535 or pom_prom_teplota == 0)
      {    
        formatuj_string("-----"         , 143,  5);
        formatuj_string("------"        , 151,  6);
      }
      
    else
      {    
        formatuj_ulong(pom_prom_teplota        , 147,  5);                                 // teplota v uint16_t formatu (pripocteno 50'C a vynasobeno x1000)
        formatuj_float(citelna_teplota(pom_prom_teplota)  , 151, 153, 2 , 2);              // teplota v citelnem desetinnem float formatu
        vystupni_retezec[154]  = desod;                                                    // desetinny oddelovac na spravnou pozici
      }
    indexpole = indexpole + 2;
#else
    formatuj_string("-----"    , 143,  5);                                                 // kdyz se teplota neuklada, zobrazi se misto ni jen pomlcky
    formatuj_string("------"   , 151,  6);

#endif

#ifdef ukladat_vlhkost
    uint16_t pom_prom_vlhkost;
    if (adresa > 0) pom_prom_vlhkost = EEPROM_read_int(indexpole);
    else            pom_prom_vlhkost = (pole_EEPROM[indexpole] << 8) + pole_EEPROM[indexpole+1];

    if (pom_prom_vlhkost == 65535 or pom_prom_vlhkost == 0)
      {    
        formatuj_string("-----"         , 160,  5);
        formatuj_string("------"        , 168,  6);
      }
    else
      {
        formatuj_ulong(pom_prom_vlhkost              , 164,  5);                           // vlhkost v uint16_t formatu (zmerena vlhkost * 100)
        formatuj_float(citelna_vlhkost(pom_prom_vlhkost)  , 255, 170, 3 , 2);              // vlhkost v % - v citelnem float desetinnem cisle    
        vystupni_retezec[171]  = desod;                                                    // desetinny oddelovac na spravnou pozici
      }
    indexpole = indexpole + 2;
#else
    formatuj_string("-----"     , 160,  5);                                                // kdyz se vlhkost neuklada do EEPROM, tak ze misto ni zobrazi jen pomlcky
    formatuj_string("------"    , 168,  6);
#endif

#ifdef ukladat_tlak
    uint16_t pom_prom_tlak;
    if (adresa > 0) pom_prom_tlak = EEPROM_read_int(indexpole);
    else            pom_prom_tlak = (pole_EEPROM[indexpole] << 8) + pole_EEPROM[indexpole+1];
    
    if (pom_prom_tlak == 65535 or pom_prom_tlak == 0)
      {    
        formatuj_string("------"        , 177,  6);
      }
    else
      {    
        formatuj_ulong(citelny_tlak(pom_prom_tlak)     , 182,  6);                         // tlak v [Pa] ve formatu uint32_t
      }
    indexpole = indexpole + 2;

#else
    formatuj_string("------"    , 177,  6);                                                // kdyz se tlak neuklada do EEPROM, zobrazi se misto nej jen pomlcky
#endif


#ifdef ukladat_GPS    
    //---  LAT ---
    uint32_t pom_prom_gps;                                                            // GEO LAT   priklad: 49123456 = "+49,123456"
    if (adresa > 0) pom_prom_gps = EEPROM_read_long(indexpole);
    else            pom_prom_gps = (pole_EEPROM[indexpole] << 24) + (pole_EEPROM[indexpole + 1] << 16) + (pole_EEPROM[indexpole + 2] << 8) + pole_EEPROM[indexpole + 3];
      
    if (pom_prom_gps > 180000000UL)                                                        // mimo rozsah  (+/- 90') s presnosti na 6 desetinnych mist se vejde do uint32_t (4 bajty)
      {    
        uint8_t pred_stan = ((pom_prom_gps & 0xE0000000) >> 29);                              // v nejvyssich 3 bitech je pripadne ulozeno prednastavene pozorovaci stanoviste 0b001 az 0b101        
        if ( pred_stan > 0  and pred_stan < 6)
          {
            for (uint8_t popisek = 186 ; popisek < 196 ; popisek ++)                          // misto GPS LAT se zkopiruje 10 znaku popisku z EEPROM
              {
                vystupni_retezec[popisek] = EEPROM_read(eeaddr_stanoviste + ((pred_stan-1) * 15) + popisek - 186 ); 
              }
          }
        else                                                                                // prednastavene pozorovaci stanoviste je zrusene a souradnice jsou mimo rozsah 
          {
            formatuj_string("----------"         , 186, 10);        
          }
        

        
      }
    else
      {
        if (pom_prom_gps > 90000000UL)
          {
            vystupni_retezec[186] = '+';                                                   // ('+' = SEVER)
            pom_prom_gps = pom_prom_gps - 90000000UL;   
          }
        else
          {
            vystupni_retezec[186] = '-';                                                   // ('-' = JIH)        
          }

        uint8_t stupne = (pom_prom_gps / 1000000UL);
        uint32_t desetiny = (pom_prom_gps % 1000000UL);
        formatuj_ulong(stupne   , 188, 2);
        formatuj_ulong(desetiny , 195, 6);
        vystupni_retezec[189]  = desod;                                                    // desetinny oddelovac na spravnou pozici
      }

    indexpole = indexpole + 4;

    //---  LON ---                                                                         // GEO LON   priklad: 114123456 = "+114,123456"
    if (adresa > 0) pom_prom_gps = EEPROM_read_long(indexpole);
    else            pom_prom_gps = (pole_EEPROM[indexpole] << 24) + (pole_EEPROM[indexpole + 1] << 16) + (pole_EEPROM[indexpole + 2] << 8) + pole_EEPROM[indexpole + 3];
    
    if (pom_prom_gps > 360000000UL)                                                        // mimo rozsah  (+/- 180') s presnosti na 6 desetinnych mist se vejde do uint32_t (4 bajty)
      {    
        formatuj_string("-----------"        , 199, 11);
      }
    else
      {
        if (pom_prom_gps > 180000000UL)
          {
            vystupni_retezec[199] = '+';                                                   // ('+' = Vychod)
            pom_prom_gps = pom_prom_gps - 180000000UL;   
          }
        else
          {
            vystupni_retezec[199] = '-';                                                   // ('-' = Zapad)        
          }


        uint8_t stupne = (pom_prom_gps / 1000000UL);
        uint32_t desetiny = (pom_prom_gps % 1000000UL);
        formatuj_ulong(stupne   , 202, 3);
        formatuj_ulong(desetiny , 209, 6);
        vystupni_retezec[203]  = desod;                                                    // desetinny oddelovac na spravnou pozici
      }

    indexpole = indexpole + 4;

    //---  ALT ---                                                                         // GEO ALT   priklad: 1010 = "+0510" m.n.m. (V EEPROM je hodnota zvysena o 500m)
                                                                                           //                     490 = "-0010" m.n.m.
    if (adresa > 0) pom_prom_gps = EEPROM_read_int(indexpole);
    else            pom_prom_gps = (pole_EEPROM[indexpole] << 8) + pole_EEPROM[indexpole+1];
                                                                                               
    if (pom_prom_gps > 10000)                                                              // mimo rozsah  ( maximalni vyska muze byt 9500m)
      {    
        formatuj_string("-----"        , 213, 5);
      }
    else
      {
        if (pom_prom_gps > 500)
          {
            vystupni_retezec[213] = '+';                                                   // ('+' = nad morem)
            pom_prom_gps = pom_prom_gps - 500;   
          }
        else
          {
            vystupni_retezec[213] = '-';                                                   // ('-' = pod morem)        
            pom_prom_gps = 500 - pom_prom_gps;
          }

        formatuj_ulong(pom_prom_gps , 217, 4);
      }

    indexpole = indexpole + 2;
        
        



#else                                                                                      // kdyz se GPS neuklada, zobrazi se ve vypisu jen pomlcky
        formatuj_string("----------"    , 186, 10);                                        // LAT  "+49.123456"
        formatuj_string("----------"    , 198, 10);                                        // LONG "+120.12345"
        formatuj_string("-----"         , 213,  5);                                        // ALT  "+1245"
#endif


#ifdef ukladat_naklon                                                                      // uhel v desetinach stupne zvetseny o 1000 stupnu 
    uint16_t pom_prom_uhel;                                                                //   (cislo 10000 = 0 stupnu, cislo 10900 = 90 stupnu 9100 = -90 stupnu)
    if (adresa > 0) pom_prom_uhel = EEPROM_read_int(indexpole);
    else            pom_prom_uhel = (pole_EEPROM[indexpole] << 8) + pole_EEPROM[indexpole+1];

    if (pom_prom_uhel == 65535)
      {
        formatuj_string("------"    , 221,  6);                                            // pri nekterych typech mereni se uhel neuklada (kalibrace), zobrazi se misto nej jen pomlcky
      }
    else
      {
        formatuj_float(((pom_prom_uhel - 10000) / 10.0) , 221 , 224 , 3 , 1);              // preformatovany uhel do citelneho tvaru
        vystupni_retezec[225]  = desod;
      }
    indexpole = indexpole + 2;

#else
    formatuj_string("------"    , 221,  6);                                                // kdyz se uhel neuklada do EEPROM, zobrazi se misto nej jen pomlcky
#endif


#ifdef ukladat_azimut                                                                      // uhel v celych stupnich (0 az 359)
    uint16_t pom_prom_azimut;
    if (adresa > 0) pom_prom_azimut = EEPROM_read_int(indexpole);
    else            pom_prom_azimut = (pole_EEPROM[indexpole] << 8) + pole_EEPROM[indexpole+1];
       
    if (pom_prom_azimut == 65535)
      {
        formatuj_string("---"    , 230,  3);                                               // pri nekterych typech mereni se uhel neuklada (kalibrace), zobrazi se misto nej jen pomlcky
      }
    else
      {
        formatuj_ulong(pom_prom_azimut                  , 232,  3);                        // azimut se neprepocitava, zustava v rozsahu int
      }
    indexpole = indexpole + 2;

#else
    formatuj_string("---"    , 230,  3);                                                   // kdyz se azimut neuklada do EEPROM, zobrazi se misto nej jen pomlcky
#endif



#ifdef ukladat_astro       
    if (EEPROM_read(indexpole) != 255)                                                     // pri casove znacce se astro vypocty neprovadeji a do elevace se uklada hodnota 255
      {
        formatuj_string("    "    , 236,  4);                                              // smazani pripadnych predchozich pomlcek (pokud po casove znacce nasleduje normalni mereni)
        formatuj_string("    "    , 243,  4);
        formatuj_string("    "    , 250,  4);

        int16_t pom_elevace;                                                                   // elevace Slunce (muze byt kladna i zaporna)
        if (adresa > 0) pom_elevace = EEPROM_read(indexpole) - 90;
        else            pom_elevace = pole_EEPROM[indexpole] - 90;
        formatuj_int(pom_elevace , 237 ,true);


        if (adresa > 0) pom_elevace = EEPROM_read(indexpole + 1) - 90;                     // elevace Mesice (muze byt kladna i zaporna)
        else            pom_elevace = pole_EEPROM[indexpole + 1] - 90;       
        formatuj_int(pom_elevace , 244 , true);

        int16_t pom_osvit;
        if (adresa > 0) pom_osvit = EEPROM_read(indexpole + 2);                            // stav osvetleni Mesice (omezeni na 0 az 99 procent)
        else            pom_osvit = pole_EEPROM[indexpole + 2];  
        formatuj_int(pom_osvit   , 250 , false);
      }
    else
      {
        formatuj_string("----"    , 236,  4);                                              // po  casove znacce se do astro vypoctu ulozi nesmyslne hodnoty 255 a proto se ve vypisu nahradi pomlckami
        formatuj_string("----"    , 243,  4);
        formatuj_string("----"    , 250,  4);
      }
    indexpole = indexpole + 3;
#else
    formatuj_string("----"    , 236,  4);                                                  // kdyz se astro informace neukladaji do EEPROM, zobrazi se misto nich jen pomlcky
    formatuj_string("----"    , 243,  4);                                                  // kdyz se astro informace neukladaji do EEPROM, zobrazi se misto nich jen pomlcky
    formatuj_string("----"    , 250,  4);                                                  // kdyz se astro informace neukladaji do EEPROM, zobrazi se misto nich jen pomlcky
#endif

#ifdef ukladat_rezerva_1       
    uint16_t pom_rezerva_1;                                                            // hodnota z rezervniho cidla 1 (v pripade zapornych hodnot predelat)
    pom_rezerva_1 = (EEPROM_read(indexpole) * 256) + EEPROM_read(indexpole+1);
    formatuj_string("     "     , 257,  5);                                                // smazani pripadnych predchozich pomlcek (pokud po casove znacce nasleduje normalni mereni)
    formatuj_ulong(pom_rezerva_1 , 261,  5);                                               // 16-bitova (0 az 65535)kladna hodnota bez zobrazeneho znamenka, doplnena uvodnimi nulami
    indexpole = indexpole + 2;
#else
    formatuj_string("-----"     , 257,  5);                                                // kdyz se rezerva 1 neuklada do EEPROM, zobrazi se misto ni jen pomlcky
#endif

#ifdef ukladat_rezerva_2       
    uint16_t pom_rezerva_2;                                                            // hodnota z rezervniho cidla 2 (v pripade zapornych hodnot predelat)
    pom_rezerva_2 = (EEPROM_read(indexpole) * 256) + EEPROM_read(indexpole+1);
    formatuj_string("     "     , 265,  5);                                                // smazani pripadnych predchozich pomlcek (pokud po casove znacce nasleduje normalni mereni)
    formatuj_ulong(pom_rezerva_2 , 269,  5);                                               // 16-bitova (0 az 65535)kladna hodnota bez zobrazeneho znamenka, doplnena uvodnimi nulami
    indexpole = indexpole + 2;
#else
    formatuj_string("-----"     , 265,  5);                                                // kdyz se rezerva 2 neuklada do EEPROM, zobrazi se misto ni jen pomlcky
#endif

#ifdef ukladat_rezerva_3       
    uint16_t pom_rezerva_3;                                                            // hodnota z rezervniho cidla 3 (v pripade zapornych hodnot predelat)
    pom_rezerva_3 = (EEPROM_read(indexpole) * 256) + EEPROM_read(indexpole+1);
    formatuj_string("     "     , 273,  5);                                                // smazani pripadnych predchozich pomlcek (pokud po casove znacce nasleduje normalni mereni)
    formatuj_ulong(pom_rezerva_3 , 277,  5);                                               // 16-bitova (0 az 65535)kladna hodnota bez zobrazeneho znamenka, doplnena uvodnimi nulami
    indexpole = indexpole + 2;
#else
    formatuj_string("-----"     , 273,  5);                                                // kdyz se rezerva 3 neuklada do EEPROM, zobrazi se misto ni jen pomlcky
#endif

#ifdef ukladat_rezerva_4       
    uint16_t pom_rezerva_4;                                                            // hodnota z rezervniho cidla 1 (v pripade zapornych hodnot predelat)
    pom_rezerva_4 = (EEPROM_read(indexpole) * 256) + EEPROM_read(indexpole+1);
    formatuj_string("     "     , 281,  5);                                                // smazani pripadnych predchozich pomlcek (pokud po casove znacce nasleduje normalni mereni)
    formatuj_ulong(pom_rezerva_4 , 285,  5);                                               // 16-bitova (0 az 65535)kladna hodnota bez zobrazeneho znamenka, doplnena uvodnimi nulami
    indexpole = indexpole + 2;
#else
    formatuj_string("-----"     , 281,  5);                                                // kdyz se rezerva 4 neuklada do EEPROM, zobrazi se misto ni jen pomlcky
#endif


    // ---------------------
    if ((EEPROM_read(adresa) & 0b01100000) == 64)                                          // specialni pripad zaznamu pro funkci stopky (0bx10xxxxx)
      {
        
        for (uint8_t ic = 66; ic < 254 ; ic++)    vystupni_retezec[ic] = '-';

        
        formatuj_string(" TIMER  " , 102, 8);


        if (EEPROM_read(adresa+7) == 1)     formatuj_string("START" , 94, 5);              // pro zaznam START se neprovadi zadne prepocty casu a zobrazi se beznym zpusobem
        if (EEPROM_read(adresa+7) == 2)     formatuj_string("STOP " , 94, 5);
        if (EEPROM_read(adresa+7) == 3)     formatuj_string(" LAP " , 94, 5);
        if (EEPROM_read(adresa+7) == 4)     formatuj_string("CONT " , 94, 5);              // pro zaznam CONT se neprovadi zadne prepocty casu a zobrazi se beznym zpusobem


        



        if (EEPROM_read(adresa+7) == 2 or EEPROM_read(adresa+7) == 3)                      // zaznam "STOP" nebo "LAP": provadi se prepocet casu na format HH:NN:SS +0.nnn
          {
            for (uint8_t ic = 30; ic < 53 ; ic++)  vystupni_retezec[ic] = ' ';
            for (uint8_t ic = 53; ic < 67 ; ic++)  vystupni_retezec[ic] = '-';

            // cele sekundy se prevadi na format "HH:NN:SS"
            uint32_t stopky_cas = cas_EEPROM / 1000;                                  // celkovy cas v sekundach
 
            uint8_t pomprom_stopky = stopky_cas % 60;                                         // jen samotne sekundy
            formatuj_ulong(pomprom_stopky              , 39,  2);

            stopky_cas = (stopky_cas - pomprom_stopky) / 60;                               // od celkoveho casu se odectou zobrazene sekundy a vysledek se prevede na minuty
            pomprom_stopky = stopky_cas % 60;                                              // jen samotne minuty
            
            formatuj_ulong(pomprom_stopky              , 36,  2);
            
            stopky_cas = (stopky_cas - pomprom_stopky) / 60;                               // od celkoveho casu se odectou zobrazene minuty a vysledek se prevede na hodiny

            formatuj_ulong(stopky_cas                  , 33,  2);

            vystupni_retezec[34] = ':';                                                    // oddelovac mezi hodinami a minutami
            vystupni_retezec[37] = ':';                                                    // oddelovac mezi minutami a sekundami
            
            vystupni_retezec[45] = '+';                                                    // tisiciny sekundy se ukladaji ve formatu "+0.nnn"
            vystupni_retezec[46] = '0';
            vystupni_retezec[47] = desod;
            
            formatuj_ulong((cas_EEPROM % 1000)              , 50,  3);
            
            formatuj_ulong(EEPROM_read(adresa+6)            , 90,  2);                     // pocitadlo mezicasu zapisuje na pozici puvodniho cisla cidla

            
          }

      }
    // ---------------------
      

    // ---------------------
    if ((EEPROM_read(adresa) & 0b01100000) == 32)                                          // specialni pripad zaznamu pro funkci casoveho razitka (0bx01xxxxx)
      {
        for (uint16_t ic = 75 ; ic < 219 ; ic++)    vystupni_retezec[ic] = '-';        // vypomlckuji se vsechny hodnoty krome azimutu, naklonu a astro informaci
        for (uint16_t ic = 256; ic < 286 ; ic++)    vystupni_retezec[ic] = '-';        //  (vcetne vsech rezervnich hodnot na konci zaznamu)
       
        formatuj_string(" STAMP  " , 102, 8);
        formatuj_string(" TIME" , 94, 5);   
      }
    // ---------------------


    // ---------------------
    //if ((EEPROM_read(adresa) & 0b01100000) == 96)                                        // debugovaci rezerva (0bx11xxxxx)
    //  {
    //    Serial.println("DEBUG");
    //  }
    // ---------------------

    oddeleni_polozek();                                                                    // nastaveni oodelovacu polozek

  }
//----------------------------------------------



//----------------------------------------------
// doplneni oddelovacu mezi polozkami (a pripadne uzavreni kazde polozky do uvozovek)
//  tyka se to i hlavicky
void oddeleni_polozek(void)
  {
    switch(oddelovac_polozek)
      {
        case 0:
          oddelovac = ' ';
          break;
        case 1:
          oddelovac = ',';
          break;
        case 2:
          oddelovac = ';';
          break;
        case 3:
          oddelovac = '\t';   
      } 
    vystupni_retezec[0] = uvozovky;                                                        // uvodni uvozovky (nebo mezera)
    deleni(6);                                                                             // oddeleni jednotlivych polozek zacina vzdycky zakoncenim predchozi polozky uvozovkami
    deleni(15);                                                                            //    pak nasleduje oddelovac a na zaver jsou uvodni uvozovky dalsi polozky
    deleni(28);
    deleni(41);
    deleni(52);
    deleni(57);
    deleni(64);
    deleni(72);
    deleni(81);
    deleni(86);
    deleni(91);
    deleni(99);
    deleni(110);
    
    deleni(118);
    deleni(126);
    deleni(132);
    deleni(140);
    deleni(148);
    deleni(157);
    deleni(165);
    deleni(174);
    deleni(183);
    deleni(196);
    deleni(210);
    deleni(218);

    deleni(227);
    deleni(233);
    deleni(240);
    deleni(247);
    
    deleni(254);
    deleni(262);
    deleni(270);
    deleni(278);

    vystupni_retezec[286] = uvozovky;                                                      // zaverecne uvozovky (nebo mezera)
  }
//----------------------------------------------



//----------------------------------------------
// oddelovac polozek v CSV souboru (carka, strednik, tabulator a uzavirani polozek do uvozovek, nebo mezer)
void deleni(uint16_t pozice)
  {
    vystupni_retezec[pozice + 0] = uvozovky;
    vystupni_retezec[pozice + 1] = oddelovac;
    vystupni_retezec[pozice + 2] = uvozovky;

    hlavicka[pozice + 1] = oddelovac;                                                      // oddelovac mezi polozkami se upravuje i v hlavicce, ale v hlavicce se polozky nikdy neuzaviraji do uvozovek

  }
//----------------------------------------------



//----------------------------------------------
// formatovani male INT hodnoty na 3 znaky (rozsah -99 az +99)
// cislo                ... int (znamenkove) cislo ke zkopirovani do vystupniho retezce
// index_znamenka       ... index znaku ve vystupnim retezci, kam se bude zapisovat znamenko (zapisuje se vzdycky '+' nebo '-') cislo pokracuje hned za znamekem s pripadnou 0 u kratkych cisel
void formatuj_int(int16_t cislo , uint16_t index_znamenka , bool zobrazit_znamenko)
  {
    if (cislo >  99) cislo =  99;                                                          // ochrana pred prekrocenim povoleneho rozsahu
    if (cislo < -99) cislo = -99;

    if (zobrazit_znamenko == true)
      {
        if (cislo <  0) vystupni_retezec[index_znamenka] = '-';
        if (cislo >  0) vystupni_retezec[index_znamenka] = '+';
        if (cislo == 0) vystupni_retezec[index_znamenka] = ' ';                            // pro nulu se nezobrazuje ani '+' ani '-'
      }
    if (cislo < 0) cislo = -cislo;
    vystupni_retezec[index_znamenka + 1] = (cislo / 10) + 48;                              // desitky
    vystupni_retezec[index_znamenka + 2] = (cislo % 10) + 48;                              // jednotky
  }
//----------------------------------------------



//----------------------------------------------
// formatovani FLOAT hodnoty
// cislo                ... cislo ke zkopirovani do vystupniho retezce
// index_znamenka       ... index znaku ve vystupnim retezci, kam se bude zapisovat znamenko (pri 255 se znamenko nezobrazuje - bude to vzdycky kladne cislo)
// index_jednotek       ... index znaku ve vystupnim retezci, kam se budou zapisovat jednotky (nasledovane desetinnym oddelovacem)
// pocet_celych_mist    ... do kolika radu se budou pripadne doplnovat nuly (pripraveno az na 3 rady, ale ve skutecnosti se budou pouzivat maximalne 2)
// pocet_desetin        ... kolik bude mist za desetinnym oddelovacem
void formatuj_float(float cislo , uint16_t index_znamenka,  uint16_t index_jednotek, uint8_t pocet_celych_mist , uint8_t pocet_desetin)
  {
    if (cislo < 0)
      {
        vystupni_retezec[index_znamenka] = '-';
        cislo = -cislo;
      }
    else
      {
        if (index_znamenka < 255)   vystupni_retezec[index_znamenka] = '+';
      }

    uint16_t zacatek = index_jednotek - pocet_celych_mist + 1;

    if (pocet_celych_mist > 3)
      {
        if (cislo < 1000)
          {
            vystupni_retezec[index_jednotek - 3] = '0';
            zacatek ++;       
          }
      }

    if (pocet_celych_mist > 2)
      {
        if (cislo < 100)
          {
            vystupni_retezec[index_jednotek - 2] = '0';
            zacatek ++;       
          }
      }

    if (pocet_celych_mist > 1)
      {
        if (cislo < 10)
          {
            vystupni_retezec[index_jednotek - 1] = '0';
            zacatek ++;       
          }
      }

    String pomprom;
    if (pocet_desetin > 0)  pomprom = String(cislo , pocet_desetin);
    else                    pomprom = String((int)cislo);

    for (uint16_t i = zacatek ; i < zacatek + pocet_celych_mist + pocet_desetin + 1 ; i++)
      {
        vystupni_retezec[i] = pomprom.charAt(i-zacatek);
      }
  }
//----------------------------------------------



//----------------------------------------------
// formatovani STRING textu
// retezec        ... String, ktery se ma nakopirovat od prislusne pozice
// index_zacatek  ... index znaku ve vystupnim retezci, kam se nakopiruje prvni znak Stingu
// pocet_znaku    ... kolik znaku z Stringu se ma zkopirovat do vystupniho retezce
//                       (bez kontroly, ze vstupni String obsahuje potrebny pocet znaku)
void formatuj_string(String retezec , uint16_t index_zacatek, uint8_t pocet_znaku)
  {
    for (uint16_t i = index_zacatek ; i < index_zacatek + pocet_znaku + 1 ; i++)
      {
        vystupni_retezec[i] = retezec.charAt(i - index_zacatek);
      }    
  }
//----------------------------------------------



//----------------------------------------------
// formatovani uint32_t hodnoty
// cislo          ... cislo, ktere se ma zformatovat  (Nebude nikdy zaporne, rozsah je 0 az 4,2 miliardy)
// start_jednotek ... index znaku ve vystupnim retezci, kam se budou zapisovat jednotky (nejnizsi rad)
// pocet_mist     ... kolik znaku se zobrazi (priklad: pro cislo 0 az 9999 je pocet mist 4) kdyz je cislo mensi, nez pocet_mist, doplni se zleva nulami
//                     (bez ochrany, ze je cislo vetsi, nez pocet mist - to se proste nestane)
void formatuj_ulong(uint32_t cislo , uint16_t start_jednotek, uint8_t pocet_mist)
  {
    if (pocet_mist > 9)
      {
        if (cislo < 1000000000UL) vystupni_retezec[start_jednotek - 9]   = '0';
        else
          {
            vystupni_retezec[start_jednotek - 9] = (cislo / 1000000000UL) + 48;
            cislo = cislo % 1000000000UL;
          }
      }

    if (pocet_mist > 8)
      {
        if (cislo < 100000000UL) vystupni_retezec[start_jednotek - 8]   = '0';
        else
          {
            vystupni_retezec[start_jednotek - 8] = (cislo / 100000000UL) + 48;
            cislo = cislo % 100000000UL;
          }
      }

    if (pocet_mist > 7)
      {
        if (cislo < 10000000UL) vystupni_retezec[start_jednotek - 7]   = '0';
        else
          {
            vystupni_retezec[start_jednotek - 7] = (cislo / 10000000UL) + 48;
            cislo = cislo % 10000000UL;
          }
      }

    if (pocet_mist > 6)
      {
        if (cislo < 1000000UL) vystupni_retezec[start_jednotek - 6]   = '0';
        else
          {
            vystupni_retezec[start_jednotek - 6] = (cislo / 1000000UL) + 48;
            cislo = cislo % 1000000UL;
          }
      }

    if (pocet_mist > 5)
      {
        if (cislo < 100000UL) vystupni_retezec[start_jednotek - 5]   = '0';
        else
          {
            vystupni_retezec[start_jednotek - 5] = (cislo / 100000UL) + 48;
            cislo = cislo % 100000UL;
          }
      }
    
    if (pocet_mist > 4)
      {
        if (cislo < 10000) vystupni_retezec[start_jednotek - 4]   = '0';
        else
          {
            vystupni_retezec[start_jednotek - 4] = (cislo / 10000) + 48;
            cislo = cislo % 10000;
          }
      }

    
    if (pocet_mist > 3)
      {
        if (cislo < 1000) vystupni_retezec[start_jednotek - 3]   = '0';
        else
          {
            vystupni_retezec[start_jednotek - 3] = (cislo / 1000) + 48;
            cislo = cislo % 1000;
          }
      }

    if (pocet_mist > 2)
      {
        if (cislo <  100) vystupni_retezec[start_jednotek - 2]   = '0';
        else
          {
            vystupni_retezec[start_jednotek - 2] = (cislo / 100) + 48;
            cislo = cislo % 100;
          }
      }
    

    if (pocet_mist > 1)
      {    
        if (cislo <   10) vystupni_retezec[start_jednotek - 1]  = '0';
        else
          {
            vystupni_retezec[start_jednotek - 1] = (cislo / 10) + 48;
            cislo = cislo % 10;
          }
      }
    vystupni_retezec[start_jednotek] = cislo + 48;
  }
//----------------------------------------------
