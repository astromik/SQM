// Podprogramy pro vytvoreni zaznamu s casovou znackou
//================================================


//----------------------------------------------
void casove_razitko(void)
  {
    uint32_t cas_stisku_TS = millis();
    uint32_t tt = rtc_korekce();                                                    // zjisteni aktualniho casu v RTC

    pole_EEPROM[0] =  0b10100000 + selc;                                            // znacka, ze je blok adres v EEPROM obsazeny (bit7='1') jestli je SEC(bit0='0'), nebo SELC(bit0='1')
                                                                                    // bit5 signalizuje, ze zaznam se tyka stopek
    pole_EEPROM[1] =  ((uint32_t)tt >> 24) & 0xFFUL;                                // 4 bajty pro pocet sekund od 1.1.1970 (s korekci nepresnosti a pripadnym prevodem na letni cas)
    pole_EEPROM[2] =  ((uint32_t)tt >> 16) & 0xFFUL;
    pole_EEPROM[3] =  ((uint32_t)tt >>  8) & 0xFFUL;
    pole_EEPROM[4] =  ((uint32_t)tt      ) & 0xFFUL;   

    test_nulovani_citace();                                                         // v poledne se nuluje citac
    uint8_t TS_poradova_znacka = EEPROM_read(eeaddr_TS_aktual) + 1;
    EEPROM_write(eeaddr_TS_aktual , TS_poradova_znacka);                            // tahle bunka v EEPROM se sice prepisuje pri kazde znacce, ale nepredpokladam, ze by se 
                                                                                    //   pri normalnim provozu provedlo pres milion povolenych zaznamu casoveho razitka, ktere by EEPROM poskodily
        
    pole_EEPROM[5] = 0;                                                             // Misto informace o svetle se uklada poradove cislo casoveho razitka
    pole_EEPROM[6] = TS_poradova_znacka;  
    
    pole_EEPROM[7] = 0;                                                             // info bajt se pri casovem razitku nepouziva

    pozice_tecky = 0;

    zobraz_cislo(TS_poradova_znacka,0);

    for (uint8_t i = 8 ; i < 37 ; i++)                                                 // pri casove znacce se neukladaji ani astro vypocty ani GPS souradnice, tak se vsechny tyto bunky v poli EEPROM smazou
      {
        pole_EEPROM[i] = 255; 
      }
    int_naklon  = 65535;                                                            // pro pripad, ze se naklon nepouziva, nastavi se na 65535
    int_azimut  = 65535;
    if (modul_LSM303DLHC == true)
      {
#ifdef ukladat_naklon
        int_naklon  = uhel();                                                       // kdyz je zapnute cidlo naklonu a povolene ukladani, tak zmeri i uhel nakloneni krabicky 
        pole_EEPROM[index_naklonu] = (int_naklon >> 8) & 0xFF;                      // a ulozi se na spravnou pozici v zaznamu
        pole_EEPROM[index_naklonu + 1] = (int_naklon     ) & 0xFF;
#endif

#ifdef ukladat_azimut
        int_azimut  = zjisti_azimut();                                              // kdyz je zapnute cidlo naklonu s kompasem a povolene ukladani, tak zmeri i azimut
        pole_EEPROM[index_azimutu]     = (int_azimut >> 8) & 0xFF;                  // a ulozi se na predposledni 2 pozice v poli EEPROM
        pole_EEPROM[index_azimutu + 1] = (int_azimut     ) & 0xFF;
#endif
      }


    hranice_ukladani = 0;
    zaznam_EEPROM();

    tone_X(pin_bzuk,800, 30 , 4);

    while (digitalRead(pin_TS) == LOW)
      {
        if (millis() - cas_stisku_TS > 3000)                                        // po 3 sekundach trvaleho drzeni bocniho tlacitka se zobrazi cas "HH-MM"
          {
            ho_mi();
          }
        delay(100);                                                                 // cas se bude akktualizovat 10x za sekundu                                
      }


    delay(100);
  }
//----------------------------------------------



//----------------------------------------------
// kazde poledne se nuluje pocitadlo casovych znacek
void test_nulovani_citace(void)
  {
    // nejdriv zjistit, jestli je dneska pred polednem, nebo po poledni. Podle toho se zjisti znacka.
    //  Kdyz je dnesek jeste pred polednem, spocte se znacka predchoziho dne
    
    uint32_t tt = rtc_korekce();                                                    // aktualni cas v RTC v sekundach od 1.1.1970

    sek1970_datcas(tt,false);

                                                                                    // mtt_file se pouziva k vytvoreni jmena souboru a slozky, a proto tam muze byt nekdy i vcerejsek
    if (mtt.hour < 12)                                                              // od pulnoci se cele dopoledne dnesniho dne ulkada jeste do vcerejsiho souboru
      {
        tt_file = tt - 43200UL;                                                     // datum je o 12 hodin starsi (vcerejsi)
        breakTimeFCE(tt_file,mtt_file);
      }
    else                                                                            // od poledne az do nasledujici pulnoci se uz vytvari normalni dnesni soubor
      {
        breakTimeFCE(tt,mtt_file);
      }

    LOC_den_file = mtt_file.day;                                                    // den, mesic a rok pro jmeno souboru a slozky
    LOC_mes_file = mtt_file.month;

    // znacka, ke ktere rade patri soucasny datum 
    mesden = LOC_mes_file*100 + LOC_den_file;                                       // zacatek obdobi, pro ktere plati zaznamenane hodnoty (napriklad pri aktualnim case "23.5.2019 16:48" se zaznamena 0523)

    uint16_t mesden_EEPROM = (EEPROM_read_int(eeaddr_TS_stanoviste) & 0b0000011111111111);     // osekat nejvyssich 5 bitu (aktualne nastavene preddefinovane pozorovaci stanoviste)


    if (mesden != mesden_EEPROM)
      {
        EEPROM_write_int(eeaddr_TS_stanoviste, mesden);                             // pozorovaci stanoviste se po poledni maze (hornich 5 bitu)
        EEPROM_write(eeaddr_TS_aktual , 0);
      }
  }
//----------------------------------------------



//----------------------------------------------
// SW blokovani a odblokovavani bocniho tlacitka
void TS_zamek(bool zamknout)
  {
    uint8_t adresa15 = EEPROM_read(eeaddr_CRC_tset_lock);                              // adresa 15 obsahuje i informace o testovani CRC pri komunikaci - to se musi zachovat
    if (zamknout == true)
      {
        adresa15 = adresa15 | 0b10000000;                                           // nastavit LOCK bit
        TS_lock = true;    
      }
    else
      {
        adresa15 = adresa15 & 0b01111111;                                           // smazat LOCK bit
        TS_lock = false;            
      }
    EEPROM_update(eeaddr_CRC_tset_lock, adresa15);                                  // zapis upraveneho bajtu zpatky do EEPROM

  }
