
volatile uint16_t    zakmity       = 0;              // pocitadlo zakmitu v interruptech
volatile uint32_t   prvni_zmena   = 0;              // cas prvni zjistene zmeny stavu v mikrosekundach
volatile uint32_t   posledni_kmit = 0;              // cas posledniho zjisteneho zakmitu v mikrosekundach
volatile uint16_t    tlacitko      = 0;              // testovani, ze bylo stisknute nejake tlacitko, nebo ze se 10 sekund nic nestisklo
volatile uint32_t   start_testu   = 0;
volatile uint32_t   trvani_testu  = 0;              // po prvnim pruchodu interruptem se zkrati 20-sekundovy interval cekani na stisk tlacitka na 2 sekundy mereni zakmitu

#define  OUT_X_L_A            0x28
#define  CTRL_REG1_A          0x20
#define  CTRL_REG4_A          0x23
#define  CRA_REG_M            0x00
#define  CRB_REG_M            0x01
#define  MR_REG_M             0x02
#define  DLHC_OUT_X_H_M       0x03

uint8_t err = 0;
bool ukoncit_test = false;
bool automenu = true;                                           // po provedeni kazde operace se znova vypise testovaci menu


//=========================================================================================================
// prerusi cinnost pristroje a v nekonecne smycce zacne testovat jednotlive casti HW
void HW_test(bool resetuj)
  {

    pinMode(pin_DERE, OUTPUT);
    
    test_help();
    char znak = ' ';

    while (znak != 'x')                                         // testovani se ukonci znakem 'x'
      {
        if (Serial.available())
          {
            znak = Serial.read();
            if (znak == 'a')   sken_i2c();
            if (znak == 'b')   test_adc();
            if (znak == 'c')   test_eeprom();
            if (znak == 'd')   test_eeprom30();
            if (znak == 'e')   test_piskak();
            if (znak == 'f')   test_bme();
            if (znak == 'g')   test_tsl();
            if (znak == 'h')   test_displej();
            if (znak == 'i')   test_kont();
            if (znak == 'j')   cyk_interrupt();
            if (znak == 'k')   test_led();
            if (znak == 'l')   test_SD();
            if (znak == 'm')   test_485();
            if (znak == 'n')   test_GPS();
            if (znak == 'o')   test_kompas_lsm303();
            if (znak == 'p')   odposlech_GPS();
//            if (znak == 'q')   rezerva();
            if (znak == 'r')   test_rtc();
            if (znak == 's')   nastav_rtc();
            if (znak == 't')   test_DO(0);
            if (znak == 'T')   test_DO(1);
            if (pouzit_ds3231 == true)
              {
                if (znak == 'u')   test_DS3231(0);            
                if (znak == 'U')   test_DS3231(1);
              }    

            if (ukoncit_test == true) break;          // po testovaci funkci 'U' se provede automaticke ukonceni testovaciho rezimu a navrat do bezneho rezimu
            if (znak == 'x')          break;          // znak 'x' ukoncuje testovaci rezim a provede navrat do bezneho rezimu 

            if (znak == '?')      test_help();        // kdykoliv je mozne menu zobrazit znakem '?'
            if (znak == '-')
              {
                automenu = !automenu;
                if (automenu == false)  Serial.println("Automenu OFF");
                else                    Serial.println("Automenu ON");
              }
    
            if (err != 0)
              {
                Wire.begin();
                err = 0;
              }
            while (Serial.available()) Serial.read();
            delay(1000);
            if (automenu == true) test_help();
            else                  test_text(0);                                      //  "tESt "
          }
          
  
      }
    if (resetuj == true) SOFT_RESET();             // pokud je po testu HW pozadovany reset, tak se vykona

  }
//=========================================================================================================


//--------------------------------------------------------------------------
// Jen napoveda do seriove linky
void test_help(void)
  {
    test_text(0);                                      //  "tESt "

    Serial.println("  ");
    Serial.println("=============== M E N U ===================");
    Serial.println(lng_test_001);                                         // "a .... sken I2C"
    Serial.println(lng_test_002);                                         // "b .... test A/D"
    Serial.println(lng_test_003);                                         // "c .... test EEPROM 128k"
    Serial.println(lng_test_004);                                         // "d .... test extra EEPROM (r30s)"
    Serial.println(lng_test_005);                                         // "e .... test piskak"
    Serial.println(lng_test_006);                                         // "f .... test BME280"
    Serial.println(lng_test_007);                                         // "g .... test TSL2591"  
    Serial.println(lng_test_008);                                         // "h .... test displej"
    Serial.println(lng_test_009);                                         // "i .... test EXT kontaktu a zasunute karty"
    Serial.println(lng_test_010);                                         // "j .... test tlacitek"
    Serial.println(lng_test_011);                                         // "k .... test LED"
    Serial.println(lng_test_012);                                         // "l .... test SD karty"
    Serial.println(lng_test_013);                                         // "m .... test linky RS485"
    Serial.println(lng_test_014);                                         // "n .... test GPS"
    Serial.println(lng_test_015);                                         // "o .... test kompasu a naklonomeru LSM303"
    Serial.println(lng_test_016);                                         // "p .... odposlech GPS"
//    Serial.println("q .... rezerva");
    Serial.println(lng_test_017);                                         // "r .... test RTC v STM32F411"
    Serial.println(lng_test_018);                                         // "s .... nastaveni nejakeho casu do RTC v STM32F411"
    Serial.println(lng_test_019);                                         // "t .... bocni vystup na LOW"
    Serial.println(lng_test_020);                                         // "T .... bocni vystup na HIGH"    
    if (pouzit_ds3231 == true)
      {
        Serial.println(lng_test_021);                                     // "u .... test casu v RTC DS3231"
        Serial.println(lng_test_022);                                     // "U .... test probuzeni za 30s"    
      }
    Serial.println(lng_test_096);                                         // "- .... Automenu OFF/ON"    
    Serial.println(lng_test_023);                                         // "x .... navrat z testu HW do normalniho rezimu"    
    Serial.println("===========================================");   
  }

//--------------------------------------------------------------------------




//----------------------------------------------
void test_displej(void)
  {    
    Serial.println(lng_test_024);                                         // "Test displeje"

    for (uint8_t jas = 0; jas <= 7 ; jas = jas + 7)
      {
        for (uint8_t i = 0 ; i< 5 ; i++)        D_pamet[i] = 0;
        D_pamet[0] = 255;
        aktualizuj_displej(jas);
        delay(500);            
        D_pamet[1] = 255;
        aktualizuj_displej(jas);
        delay(500);            
        D_pamet[2] = 255;
        aktualizuj_displej(jas);
        delay(500);            
        D_pamet[3] = 255;
        aktualizuj_displej(jas);
        delay(500);            
        D_pamet[4] = 255;
        aktualizuj_displej(jas);
        delay(500);            
        for (uint8_t i = 0 ; i< 5 ; i++)        D_pamet[i] = 0;
        aktualizuj_displej(jas);    
      }
    
    Serial.println(lng_test_025);                                         // "Konec testu displeje"
}
//----------------------------------------------




//----------------------------------------------
// zobrazi na displeji text z pole dis_popisy[]
void test_text(uint8_t polozka)
  {
    D_pamet[0] =  test_popisy[polozka][4];                                             // pretoceni nejvyssich a nejnizsich radu pro spravne zobrazeni na spravnych jednotkych
    D_pamet[1] =  test_popisy[polozka][3];
    D_pamet[2] =  test_popisy[polozka][2];
    D_pamet[3] =  test_popisy[polozka][1];
    D_pamet[4] =  test_popisy[polozka][0];
    aktualizuj_displej(7);
  }
//----------------------------------------------




//----------------------------------------------
void test_adc(void)
  {
    test_text(8);                                                 //  " AdC "
                                                                  // v testovacich funkcich se nebere ohled na to jestli je reference osazena, nebo ne
    uint16_t napeti_ref = analogRead(pin_refu);
    uint16_t napeti_bat = analogRead(pin_AD_BAT); 

    Serial.print(lng_test_026);                                   // "Skutecne napeti na pinu Ref:  "
    Serial.print(napeti_ref);
    Serial.print(" [dig] ( ");
    Serial.print( (3.3 * napeti_ref) / 4096);                     // napeti na referencnim pinu se zenerkou 1,8V
    Serial.println(" [V])");

    Serial.print(lng_test_027);                                   // "Skutecne napeti na pinu Ubat: "
    Serial.print(napeti_bat);
    Serial.print(" [dig] ( ");
    Serial.print( (3.3 * napeti_bat) / 4096);                     // napeti na analogovem pinu pro mereni napeti baterie
    Serial.println(" [V])"); 

  
    uint16_t uavref = analogRead(AVREF);
    Vcc = (4096 * 1210 / uavref)/1000.0 ;                         // napajeci napeti [V]  - (analogova reference je na desce spojena s napajenim, 1210 je konstanta z kat.listu, 4096 je rozliseni )
    bat_3d = analogRead(AVBAT);                                   // 1/4 napeti na zalohovaci baterii [dig]
    bat_3V    = 4 * Vcc * bat_3d / 4096.0 ;                       // knoflikova baterie (3V)

    Serial.print(lng_test_092);                                   // "Napeti na procesoru:          "
    Serial.print(uavref);
    Serial.print(" [dig] ( ");
    Serial.print(Vcc);
    Serial.println(" [V])");

    Serial.print(lng_test_093);                                   // "Napeti na 3V baterii:         "
    if (bat_3d < 1000)  Serial.print(' ');
    if (bat_3d <  100)  Serial.print(' ');
    if (bat_3d <   10)  Serial.print(' ');
    Serial.print(bat_3d);
    Serial.print(" [dig] ( ");
    Serial.print(bat_3V);
    Serial.println(" [V])");
  
  
  }
//----------------------------------------------



//----------------------------------------------
void cyk_interrupt(void)
  {
    test_text(9);                                                                  // "tLAC "
    Serial.println(lng_test_028);                                                  // "Test funkce tlacitek.");

    while (true)
      {

        Serial.print(lng_test_029);                                                // " Stiskni a drz tlacitko ... "
        tlacitko = 0;
        zakmity = 0;                                           

        start_testu = millis(); 
        trvani_testu = 10000;                                                      // pred prvnim interruptem se nastavi delka trvani testovani tlacitka na 10 sekund
        while (millis() - start_testu < trvani_testu)  ;                           // testuje se 10 sekund v pripade, ze k zadnemu interruptu nedojde, nebo 2 sekundy od prvniho pruchodu interruptem
        

        if (tlacitko == 0)                                                         // behem 10 sekund nedoslo ani k jednomu interruptu
          {
            Serial.println(lng_test_030);                                          // "\r\nV intervalu 10 sekund nebylo zadne tlacitko stisknuto - KONEC TESTU."
            break;                                                                 // vypadne ze smycky while (true)
          }

        switch (tlacitko)
          {
            case 1:
              Serial.print(lng_test_031);                                          // "[NAHORU]"
              break;
            case 2:
              Serial.print(lng_test_032);                                          // "[DOLU]  "
              break;
            case 3:
              Serial.print(lng_test_033);                                          // "[OK]    "
              break;
            case 4:
              Serial.print(lng_test_034);                                          // "[TS]    "
              break;
          }
   
        Serial.print(lng_test_035);                                                // "   Zakmity = "
        Serial.print(prednuly(zakmity,5));
        Serial.print(lng_test_036);                                                // "   Trvani = "
        Serial.print(prednuly(posledni_kmit - prvni_zmena,7));                     // trvani je rozdil casu prvni_zmeny a posledniho kmitu v mikrosekundach
        Serial.print(" [us] ");
    

        zakmity = 0;
        tlacitko = 0;

        prvni_zmena = micros();                                                    // vynulovani intervalu pro pripad, ze by pri rozpinani nedoslo k zadnemu interruptu
        posledni_kmit = prvni_zmena;

        
        Serial.print(lng_test_037);                                                // "  Uvolni tlacitko ... "

        start_testu = millis(); 
        trvani_testu = 10000;                                                      // pred prvnim interruptem se nastavi delka trvani testovani tlacitka na 10 sekund
        while (millis() - start_testu < trvani_testu)  ;                           // testuje se 10 sekund v pripade, ze k zadnemu interruptu nedojde, nebo 2 sekundy od prvniho pruchodu interruptem

        
        if (tlacitko == 0)                                                         // behem 10 sekund nedoslo ani k jednomu interruptu
          {
            Serial.println(lng_test_038);                                          // "\r\nV intervalu 10 sekund nebylo zadne tlacitko uvolneno - KONEC TESTU."
            break;                                                                 // vypadne ze smycky while (true)
          }


        Serial.print(lng_test_035);                                                // "   Zakmity = "
        Serial.print(prednuly(zakmity,5));
        Serial.print(lng_test_036);                                                // "   Trvani = "
        Serial.print(prednuly(posledni_kmit - prvni_zmena,7));                     // trvani je rozdil casu prvni_zmeny a posledniho kmitu v mikrosekundach

        
        Serial.println(" [us] ");
      }
             
  }


void interUP(void)
  { 
    if (tlacitko == 0)
      {
        prvni_zmena = micros();                                                    // znacka pro vypocet delky kmitani v mikrosekundach
        start_testu = millis();                                                    // znacka pro 2-sekundovy interval testovani zakmitu
        trvani_testu = 2000;                                                       // trvani testu se nastavi na 2 sekundy od prvniho pruchodu interruptem
        tlacitko = 1;                                                              // znacka, ze bylo stisknute tlacitko cislo 1 (nahoru)
      }
    posledni_kmit = micros();                                                      // zaznam casu posledniho kmitnuti v mikrosekundach (kdyz je jen jeden zakmit, bude o neco vetsi nez prvni_zmena)
    zakmity ++;                                                                    // pricteni jednoho zakmitu
  }


void interDN(void)
  { 
    if (tlacitko == 0)
      {
        prvni_zmena = micros();                                                    // znacka pro vypocet delky kmitani v mikrosekundach
        start_testu = millis();                                                    // znacka pro 2-sekundovy interval testovani zakmitu
        trvani_testu = 2000;                                                       // trvani testu se nastavi na 2 sekundy od prvniho pruchodu interruptem
        tlacitko = 2;                                                              // znacka, ze bylo stisknute tlacitko cislo 2 (dolu)
      }
    posledni_kmit = micros();                                                      // zaznam casu posledniho kmitnuti v mikrosekundach (kdyz je jen jeden zakmit, bude o neco vetsi nez prvni_zmena)
    zakmity ++;                                                                    // pricteni jednoho zakmitu
  }


void interOK(void)
  { 
    if (tlacitko == 0)
      {
        prvni_zmena = micros();                                                    // znacka pro vypocet delky kmitani v mikrosekundach
        start_testu = millis();                                                    // znacka pro 2-sekundovy interval testovani zakmitu
        trvani_testu = 2000;                                                       // trvani testu se nastavi na 2 sekundy od prvniho pruchodu interruptem
        tlacitko = 3;                                                              // znacka, ze bylo stisknute tlacitko cislo 3 (OK)
      }
    posledni_kmit = micros();                                                      // zaznam casu posledniho kmitnuti v mikrosekundach (kdyz je jen jeden zakmit, bude o neco vetsi nez prvni_zmena)
    zakmity ++;                                                                    // pricteni jednoho zakmitu
  }


void interTS(void)
  { 
    if (tlacitko == 0)
      {
        prvni_zmena = micros();                                                    // znacka pro vypocet delky kmitani v mikrosekundach
        start_testu = millis();                                                    // znacka pro 2-sekundovy interval testovani zakmitu
        trvani_testu = 2000;                                                       // trvani testu se nastavi na 2 sekundy od prvniho pruchodu interruptem
        tlacitko = 4;                                                              // znacka, ze bylo stisknute tlacitko cislo 4 (TimeStamp)
      }
    posledni_kmit = micros();                                                      // zaznam casu posledniho kmitnuti v mikrosekundach (kdyz je jen jeden zakmit, bude o neco vetsi nez prvni_zmena)
    zakmity ++;                                                                    // pricteni jednoho zakmitu
  }



//----------------------------------------------
// sken I2C sbernice
void sken_i2c(void)
  {
    test_text(11);                                                                 // " i2C "
    Serial.print(lng_test_041);                                                    // "Sken I2C\r\|n"
    for (uint8_t i = 1; i < 127; i++)
      {
        if (i % 3 == 0)    Serial.print('-');
      }
    Serial.println('|');
    Serial.print('|');
    
    bool zarizeni[128];
    for (uint8_t i = 1; i < 127; i++)
      {
        Wire.beginTransmission (i);
        if (Wire.endTransmission () == 0)
          {
            zarizeni[i] = true;
          }
        else
          {
            zarizeni[i] = false;
            Wire.begin();
            delay(100);
          }
        if (i % 3 == 0)    Serial.print('#');
      }
    Serial.println('|');

    for (uint8_t i = 1; i < 127; i++)
      {
        if (zarizeni[i] == true)
          {
            Serial.print (lng_test_091);                       // "Nalezena adresa: "
            if (i<10)  Serial.print(' ');
            if (i<100) Serial.print(' ');
            Serial.print (i, DEC);
            Serial.print (" (0x");
            Serial.print (i, HEX);
            
            Serial.print (") ");
            switch(i)                                  // u nalezenych adres se znamymi zarizenimi se zobrazi i predpoklany typ zarizeni
              {
                case 23:  
                  Serial.print ("(GPS)");
                  break;

                case 25:  
                case 30:  
                  Serial.print ("(LSM303)");
                  break;


                case 32:  
                  Serial.print ("(Expander)");
                  break;
              
                case 41:  
                  Serial.print ("(TSL2591)");
                  break;

                case 51:  
                  Serial.print ("(Ext. Calib.)");
                  break;

                case 80:  
                  Serial.print ("(EEPROM < 64kB)");
                  break;

                case 81:  
                  Serial.print ("(EEPROM > 64kB)");
                  break;

                case 87:  
                  Serial.print ("(Extra EEPROM)");
                  break;

                case 104:  
                  Serial.print ("(DS3231)");
                  break;

                case 118:  
                  Serial.print ("(BME280)");
                  break;
              }
            Serial.println (' ');
          }
      }
  }
//----------------------------------------------


//----------------------------------------------
// test cteni a zapisu do EEPROM
void test_eeprom(void)
  {
    test_text(4);                                                                  //  "EEPro"
    randomSeed(millis());
    uint8_t pomprom1;
    uint8_t pomprom2;
    for (uint8_t i = 0; i < 5 ; i++)                                                  // 5 pokusu o cteni a zapis na ruzna mista eeprom
      {
        pomprom1 = EEPROM_read(10000 + (i * 20000UL));                             // precte puvodni hodnotu
        Serial.print(10000 + (i * 20000UL));                                       // adresa
        Serial.print(" ... R:");
        Serial.print(prednuly(pomprom1,3));                                        // puvodni hodnota
        pomprom2 = pomprom1;
        while (pomprom1 == pomprom2)                                               // vymysli nahodne cislo, ktere je jine nez puvodni
          {
            pomprom2 = random(0,256);
          }
        Serial.print(" ... W:");     
        Serial.print(prednuly(pomprom2,3));                                        // zobrazi ho
        EEPROM_write(10000 + (i * 20000UL),pomprom2);                              // a zapise
        pomprom2 = EEPROM_read(10000 + (i * 20000UL));                             // pokusi se ho precist
        Serial.print(" ... R:");
        Serial.print(prednuly(pomprom2,3));
        EEPROM_write(10000 + (i * 20000UL),pomprom1);                              // zapise puvodni
        Serial.print(" ... W:");
        Serial.print(prednuly(pomprom1,3));
        pomprom2 = EEPROM_read(10000 + (i * 20000UL));                             // a pokusi se ho precist
        Serial.print(" ... R:");
        Serial.println(prednuly(pomprom2,3));
      }
  }
//----------------------------------------------



//----------------------------------------------
// test cteni a zapisu do pridavne EEPROM pro rezim 30s
void test_eeprom30(void)
  {
    test_text(14);                                                                 // "EE-30"
    uint8_t pomprom0 = EEPROM_read30(0);                                              // puvodni hodnota na adrese 0 se zapamatuje
    uint8_t pomprom32 = EEPROM_read30(32768);                                         // puvodni hodnota na adrese 32768 se zapamatuje

    Serial.print(lng_test_042);                                                    // "puvodni hodnota na adrese [00000] = "
    Serial.println(pomprom0);
    Serial.print(lng_test_043);                                                    // "puvodni hodnota na adrese [32768] = "
    Serial.println(pomprom32);

    EEPROM_write30(0, 123);                                                        // na adresu 0 se zapise nejake cislo (123)
    EEPROM_write30(32768, 234);                                                    // pokud je osazena pamet jen 32kB, dojde k preteceni adresy 32768 na 0 a prepise se hodnota 123 na adrese nula cislem 234
    uint8_t pomprom = EEPROM_read30(0);                                               // znovu se precte adresa 0
    if (pomprom == 123) Serial.println(lng_test_044);                              // "Kapacita 64kB" puvodni hodnota se neprepsala, obsahuje cislo 123
    if (pomprom == 234) Serial.println(lng_test_045);                              // "Kapacita 32kB" puvodni hodnota se prepsala, obsahuje cislo 234
    if (pomprom !=123 and pomprom != 234) Serial.println(lng_test_046);            // "Chyba pameti"  zapis se nepodaril (pamet obsahuje 0, 100, nebo 255)

    EEPROM_write30(0, pomprom0);                                                   // obnoveni puvodnich hodnot
    EEPROM_write30(32768, pomprom32);
  }
//----------------------------------------------


//----------------------------------------------
// doplneni nul pred cislo odesilane do serioveho monitoru (pouziti ve vypisu EEPROM)
uint16_t prednuly(uint16_t vstupni_cislo, uint8_t pocet_mist)
  {
    uint8_t puvodni_sirka = 0;
    uint16_t pomprom = vstupni_cislo;

    if (pomprom == 0)  puvodni_sirka = 1;

    while (pomprom > 0)
      {
        pomprom = pomprom / 10;
        puvodni_sirka ++;
      }
    if (pocet_mist > puvodni_sirka)
      {
        for (uint8_t i = 0 ; i < pocet_mist - puvodni_sirka ; i++)
          {
            Serial.print(' ');
          }        
      }
    return vstupni_cislo;
  }
//----------------------------------------------


//----------------------------------------------
// doplneni nul pred cislo odesilane do serioveho monitoru vcetne osetreni zapornych cisel
int32_t prednuly_v2(int32_t vstupni_cislo, uint8_t pocet_mist)
  {
    uint8_t puvodni_sirka = 0;
    int32_t pomprom = vstupni_cislo;
    bool zaporne = false;
    if (pomprom < 0)
      {
        zaporne = true;
        pomprom = -pomprom;  // prevedeni na kladne cislo    
      }


    if (pomprom == 0)  puvodni_sirka = 1;

    if (zaporne == true) puvodni_sirka++;

    while (pomprom > 0)
      {
        pomprom = pomprom / 10;
        puvodni_sirka ++;
      }
    if (pocet_mist > puvodni_sirka)
      {
        for (uint8_t i = 0 ; i < pocet_mist - puvodni_sirka ; i++)
          {
            Serial.print(' ');
          }        
      }
    return vstupni_cislo;
  }
//----------------------------------------------



//----------------------------------------------
// test piskaku
void test_piskak(void)
  {
    test_text(5);                                                                  //  "bEEP "
    Serial.println(lng_test_047);                                                  // "Test piskaku"
    tone(pin_bzuk  ,   50, 50); 
    delay(200);
    tone(pin_bzuk ,  100 , 50); 
    delay(200);
    tone(pin_bzuk ,  500 , 50); 
    delay(200);
    tone(pin_bzuk , 1000 , 50); 
    delay(200);
    noTone(pin_bzuk);  
  }
//----------------------------------------------


//----------------------------------------------
void test_rtc(void)
  {
    test_text(18);                                                                 //  " rtc "

    Serial.print (lng_test_048);                                                   // "Pocet sekund v RTC: "
    uint32_t tt =  rtclock.getY2kEpoch();
    Serial.println (tt);

    breakTimeFCE(tt,mtt);
    LOC_dvt = mtt.weekday + 1;                                                     // primo v RTC se den v tydnu uklada v rozsahu 0(Po) az 6(Ne), proto se pro spravne zobrazeni pricita +1 (1=Po....7=Ne)
    LOC_den = mtt.day;
    LOC_mes = mtt.month;
    LOC_rok = mtt.year+1970;
    LOC_hod = mtt.hour;
    LOC_min = mtt.minute;
    LOC_sek = mtt.second;

    Serial.print(LOC_den);
    Serial.print('.');    
    Serial.print(LOC_mes);
    Serial.print('.');
    Serial.print(LOC_rok);
    Serial.print(' ');    
    Serial.print(LOC_hod);
    Serial.print(':');    
    if (LOC_min < 10)     Serial.print('0');
    Serial.print(LOC_min);
    Serial.print(':');    
    if (LOC_sek < 10)     Serial.print('0');
    Serial.print(LOC_sek);
    Serial.print(' ');    
    Serial.print(dny[LOC_dvt]);  
    Serial.println("  (UTC)");    

    if ((RTC->BKP0R) != 0xAA)                            // znacka ze byl cas nastaveny. Po vypadku 3V zalozni baterie se znacka automaticky maze
      {
        Serial.println(lng_test_094);                    // "Cas je nespolehlivy (vypadek 3V clanku)"
      }
    else
      {
        Serial.println(lng_test_095);                    // "Od posledniho serizeni nenastal vypadek 3V clanku" 
      }
    
  }
//----------------------------------------------




//----------------------------------------------
void nastav_rtc(void)
  {
    test_text(18);                                                                 //  " rtc "

    mtt.day = 21;
    mtt.month = 10;
    mtt.year = 2023 - 1970;
    mtt.hour = 11;
    mtt.minute = 12;
    mtt.second = 13; 
    tt = makeTimeFCE(mtt);
    
    rtclock.setY2kEpoch(tt);

    uint32_t tt =  rtclock.getY2kEpoch();
    RTC->BKP0R = (uint8_t)0xAA;                                                    // znacka ze byl cas nastaveny. Po vypadku 3V zalozni baterie se znacka automaticky maze 
    bitClear(err_bit,3);                                                           // RTC je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'

    Serial.print(lng_test_049);                                                    // "tt po ulozeni: "
    Serial.println (tt);

    Serial.println(lng_test_050);                                                  // "RTC nastaveno na: 21.10.2023 11:12:13 Ne"
  }
//----------------------------------------------



//----------------------------------------------
void test_GPS(void)
  {
    test_text(2);                                                                  //  "6P5  "
    if (digitalRead(pin_DATA_RDY) == osazeno_gps)
      {
        Serial.println("NMEA:");
        gps_NMEA(0);
        gps_NMEA(1);
        gps_NMEA(2);    
      }
    else
      {
        Serial.println(lng_test_051);                                              // "GPS neni zapnuta (data nejsou k dispozici)."      
      }
  }
//----------------------------------------------



//----------------------------------------------
void odposlech_GPS(void)
  {
    test_text(2);                                                                  //  "6P5  "
    Serial.println(lng_test_052);                                                  // "Propoj dratem pin Tx na desce SQM-BAS6 a pin Rx na desce SQM-GPS6"    
    Serial.println(lng_test_053);                                                  // "K ukonceni testu zrus propojku a pockej 5 sekund"

    delay(200);
    pinMode(PA9,INPUT_PULLUP);                                                     // Pin PA9 se "odpoji" od procesoru

    uint32_t timeout = millis();
    bool akt_stav_kom = false;
    bool pred_stav_kom = false;

    while (millis() - timeout < 5000)                                              // 5 sekund od posledni zmeny logickeho stavu na pinu PA9 se testuje, jestli nenastala dalsi zmena
      {
        akt_stav_kom = digitalRead(PA9);
        if (akt_stav_kom != pred_stav_kom)                                         // nastala zmena stavu na vstupu PA9
          {
            timeout = millis();                                                    // pri zmene stavu se nuluje 5-sekundove pocitadlo
            pred_stav_kom = akt_stav_kom;                                          // novy stav se zapamatuje
          }    
      }

    delay(100);
    

    Serial.begin(pole_baudrate[(EEPROM_read(eeaddr_SLAVE_baud) & 0b00110000) >> 4]);   // servisni a nahravaci UART (piny PA9, PA10) 
    delay(100);
    Serial.println(lng_test_054);                                                  // "odposlech GPS ukoncen"    

  
  }
//----------------------------------------------



//----------------------------------------------
void test_bme(void)
  {
    test_text(7);                                       //  "bME  "
    cidlo_BME.begin(0x76);
    delay(100);
    
    Wire.beginTransmission(0x76);                       // zacatek komunikace s cidlem
    delay(1);
    Wire.write(uint8_t(0xF4));                             // adresa prvniho konfiguracniho registru
    Wire.write(uint8_t(0xFF));                             // registr ctrl_meas (0xF4)
    Wire.write(uint8_t(0xF7));                             // registr config (0xF5)

    
    err = Wire.endTransmission();
    if (err != 0)
      {
        Serial.print(lng_test_055);                     // "Chyba I2C ... kod: "
        Serial.println(err);
        return;        
      }
    delay(1); 

    float cidlo_teploty = cidlo_BME.readTemperature();
    float cidlo_vlhkosti = cidlo_BME.readHumidity();
    uint32_t cidlo_tlaku = cidlo_BME.readPressure();

    Serial.print(lng_test_056);                         // "Teplota: "
    Serial.print(cidlo_teploty);
    Serial.print(lng_test_057);                         // "      Vlhkost: "
    Serial.print(cidlo_vlhkosti);
    Serial.print(lng_test_058);                         // "      Tlak: ");
    Serial.println(cidlo_tlaku); 
  } 
//----------------------------------------------



//----------------------------------------------
void test_tsl(void)
  {    
    uint16_t ir, full;
    uint8_t TSL_cas = 5;                                               // cas snimani  (0=100ms, 1=200ms, 2=300ms .... 5=600ms)
    uint8_t cnf;                                                       // konfiguracni registr (slouceny cas mereni a citlivost)

    test_text(6);                                                 //  "JA5  "
    
    for (int16_t TSL_gain = 48 ; TSL_gain >= 0 ; TSL_gain  = TSL_gain - 16)    // TSL_gain ma 4 urovne: 48 (9876x)
                                                                    //                              32 (428x)
                                                                    //                              16 (25x)
                                                                    //                               0 (1x) 
      {
        cnf = TSL_gain + TSL_cas;
        TSL_config(cnf);
        TSL_enable();
        if (err == 0)
          {
            delay(1000);                                            // obvod musi mit dostatek casu na mereni pri libovolnem casu mereni
            
            full = TSL_read_int(TSL2591_COMMAND_BIT | 0x14);        // CH_0
            ir  =  TSL_read_int(TSL2591_COMMAND_BIT | 0x16);        // CH_1
    
            TSL_disable();
            Serial.print("GAIN: ");
            Serial.print(prednuly(TSL_gain,3));
            Serial.print("    IR: ");
            Serial.print(prednuly(ir,5));
            Serial.print("    FULL: ");
            Serial.println(prednuly(full,5));        
          }
      }    
  }
//----------------------------------------------



//----------------------------------------------
void test_SD(void)
  {
    test_text(12);                                                                   //  " 5d  "
    if (digitalRead(pin_karta_IN) == HIGH)
      {
        Serial.println(lng_test_059);                                                // "Karta neni zasunuta - Konec testu"
      }
    else
      {
        sd.begin(SD_CONFIG);                                                         //inicializace SD karty pro pripad, ze by byla predtim vytazena
    
        char jmeno_souboru[] = "test_HW.txt";                                        // retezec pro konstrukci jmena souboru
         
        open_OK = soubor.open(jmeno_souboru, O_WRITE| O_APPEND  | O_CREAT);

            
        Serial.print(lng_test_060);                                                // "testovaci soubor: "
        Serial.println(jmeno_souboru);


        if (open_OK)
          {
            randomSeed(millis());
            int16_t nahodne_cislo = random(1000,9999);
            Serial.print(lng_test_061);                                              // "Nahodne cislo: "
            Serial.println(nahodne_cislo);

            soubor.print(lng_test_062);                                              // "pokusny zapis nahodneho cisla: "
            soubor.println(nahodne_cislo);
            delay(20);
            soubor.close();
            Serial.println(lng_test_063);                                            // "konec zapisu"


            
            Serial.print(lng_test_064);                                              // "Vypis zaznamnaneho souboru: "
            Serial.println(jmeno_souboru);
            Serial.println("--------------------------------");

            char buff[10];                                                           // buffer pro ukladani ctenych dat ze souboru
            sd.begin(SD_CONFIG);                                                     //inicializace SD karty pro pripad, ze by byla predtim vytazena
            open_OK = soubor.open(jmeno_souboru, O_READ);
            if (open_OK)
              {            
                while ((soubor.fgets(buff, sizeof(buff))) > 0)                       // cist znak po znaku, dokud neco obsahuje
                  {
                    Serial.print(buff);
                  }
                delay(20);
                soubor.close();
                Serial.println("--------------------------------");
              }
            else
              {
                Serial.println(lng_test_065);                                        // "funkce 'soubor.open()' pro cteni skoncila chybou"
              }
          
          }    
        else
          {
            Serial.println(lng_test_066);                                            // "funkce 'soubor.open()' pro zapis skoncila chybou"
          }        
      }       
  }
//----------------------------------------------



//----------------------------------------------
void test_kont(void)
  {
    test_text(13);                                                                   //  " EXt "
    Serial.print(lng_test_067);                                                      // "bocni konektor (Data_Ready): "
    bool drdy = digitalRead(pin_DATA_RDY);
    if (drdy == true)    Serial.println(lng_test_068);                               // "stav: HIGH"
    else                 Serial.println(lng_test_069);                               // "stav: LOW"

  
  
    Serial.print(lng_test_070);                                                      // "Kontakt na drzaku SD: "
    bool stavSD = digitalRead(pin_karta_IN);
    if (stavSD == true)  Serial.println(lng_test_068);                               // "stav: HIGH"
    else                 Serial.println(lng_test_069);                               // "stav: LOW"
  }
//----------------------------------------------



//----------------------------------------------
void test_led(void)
  {
    test_text(1);                                                                    //  "LEd  "
    Serial.println(lng_test_073);                                                    // "Test RGB LED"

    Serial.println(lng_test_075);                                                    // "Blika ruda..."
    for (uint8_t i = 1 ; i < 8 ; i ++)
      {
        test_blik_R(i);
      }

    Serial.println(lng_test_074);                                                    // "Blika zelena..."
    for (uint8_t i = 1 ; i < 8 ; i ++)
      {
        test_blik_G(i);
      }

    Serial.println(lng_test_076);                                                    // "Blika modra..."
    for (uint8_t i = 1 ; i < 8 ; i ++)
      {
        test_blik_B(i);
      }
    barva_LED = 0;

  }
//----------------------------------------------


//----------------------------------------------
void test_blik_G(uint8_t stupen_jasu)
  {
    uint32_t jas_green = (logaritmy_jasu[stupen_jasu] << 16); // zelena cast jasu LED se posila na levych 8 bitech
    strip.setPixelColor(0, jas_green);                        // GREEN
    strip.show();
    delay(40);
    strip.setPixelColor(0, 0x000000);                         // zhasnout
    strip.show();
    delay(500);
  }
//----------------------------------------------



//----------------------------------------------
void test_blik_R(uint8_t stupen_jasu)
  {
    uint32_t jas_red = (logaritmy_jasu[stupen_jasu] << 8);    // cervena cast jasu LED se posila na prostrednich 8 bitech
    strip.setPixelColor(0, jas_red);                          // RED
    strip.show();
    delay(40);
    strip.setPixelColor(0, 0x000000);                         // zhasnout
    strip.show();
    delay(500);
  }
//----------------------------------------------



//----------------------------------------------
void test_blik_B(uint8_t stupen_jasu)
  {
    uint32_t jas_blue = (logaritmy_jasu[stupen_jasu]);        // modra cast jasu LED se posila na pravych 8 bitech
    strip.setPixelColor(0, jas_blue);                         // BLUE
    strip.show();
    delay(40);
    strip.setPixelColor(0, 0x000000);                         // zhasnout
    strip.show();
    delay(500);
  }
//----------------------------------------------




//----------------------------------------------
void test_kompas_lsm303(void)
  {
    test_text(15);                                                            //  "CoMPA" 
    Serial.println(lng_test_077);                                             //"15 sekundovy test kompasu a naklonomeru LSM303"
    uint32_t konec_testu = millis() + 15000;
    naklon_setup();
    kompas_setup();

    while (Serial.available() == false and (konec_testu > millis()))
      {

        Wire.beginTransmission(I2C_ADDR_NAKLON);
        // assert the MSB of the address to get the accelerometer
        // to do slave-transmit subaddress updating.
        Wire.write(OUT_X_L_A | (1 << 7));
        last_status = Wire.endTransmission();
        Wire.requestFrom((uint8_t)I2C_ADDR_NAKLON, (uint8_t)6);
        delay(5);
      
        uint8_t xla = Wire.read();
        uint8_t xha = Wire.read();
        uint8_t yla = Wire.read();
        uint8_t yha = Wire.read();
        uint8_t zla = Wire.read();
        uint8_t zha = Wire.read();
      
        int16_t ax = (int16_t)(xha << 8 | xla);
        int16_t ay = (int16_t)(yha << 8 | yla);
        int16_t az = (int16_t)(zha << 8 | zla);

        Serial.print(lng_test_078);                                                     // "zbyva: "
        Serial.print(prednuly_v2((konec_testu-millis()) / 1000,2));
        Serial.print(lng_test_079);                                                     // "          zrychleni: aX= "
        Serial.print(prednuly_v2(ax,6));
        Serial.print(lng_test_080);                                                     // "    aY= "
        Serial.print(prednuly_v2(ay,6));
        Serial.print(lng_test_081);                                                     // "    aZ= "
        Serial.print(prednuly_v2(az,6));

        Wire.beginTransmission(I2C_ADDR_KOMPAS);
        // assert the MSB of the address to get the accelerometer
        // to do slave-transmit subaddress updating.
        Wire.write(DLHC_OUT_X_H_M | (1 << 7));
        last_status = Wire.endTransmission();
        Wire.requestFrom((uint8_t)I2C_ADDR_KOMPAS, (uint8_t)6);
            
        uint8_t xhm = Wire.read();
        uint8_t xlm = Wire.read();
        uint8_t zhm = Wire.read();
        uint8_t zlm = Wire.read();
        uint8_t yhm = Wire.read();
        uint8_t ylm = Wire.read();
      
        int16_t mx = (int16_t)(xhm << 8 | xlm);
        int16_t my = (int16_t)(yhm << 8 | ylm);
        int16_t mz = (int16_t)(zhm << 8 | zlm);
    
        Serial.print(lng_test_083);                                       // "          magnetometr: mX= "
        Serial.print(prednuly_v2(mx,6));
        Serial.print(lng_test_084);                                       // "    mY= "
        Serial.print(prednuly_v2(my,6));
        Serial.print(lng_test_085);                                       // "    mZ= "
        Serial.println(prednuly_v2(mz,6));        
      }
  }
//----------------------------------------------



//----------------------------------------------
void test_485(void)
  {
    test_text(10);                                              // "485  "
    Serial.print(lng_test_086);                                 // "Odesli nekolik znaku z PC pres RS485 a sleduj na PC odpoved");
    Serial.print("  (");
    Serial.print(pole_baudrate[(EEPROM_read(eeaddr_SLAVE_baud) & 0b00110000) >> 4]);
    Serial.println(" Bd)");
    Serial.println(lng_test_087);                               // "  (libovolnym znakem v teto USB lince se test ukonci)");
    prijem();

    while (Serial.available() == 0)
      {
        if (Serial2.available())                                // pres RS485 neco prislo (uklada se to do HW bufferu)
          {
            vysilani();                                         // prepnuti na vysilani
            Serial2.print(lng_test_088);                        // "ODPOVED: "
            Serial.print(lng_test_089);                         // "prijato / odesilam zpet: "
            while (Serial2.available())
              {
                char znak485 = Serial2.read();                  // cteni bufferu
                Serial.print(znak485);                          // zobrazeni znaku z bufferu v USB seriove lince
                Serial2.print(znak485);                         // a okamzite odeslani znaku pres RS485
              }
            Serial.println("\r\n");
          }
        prijem();
      }
  }
//----------------------------------------------




//----------------------------------------------
void test_DO(bool stav)
  {
    test_text(19);                                      //  "d-out"

    pinMode(pin_bok_out,OUTPUT);
    digitalWrite(pin_bok_out,stav);
    Serial.print(lng_test_090);                         // "Bocni vystup nastaven na: "
    if (stav == true) Serial.println("HIGH");
    else              Serial.println("LOW");
  }
//----------------------------------------------



//----------------------------------------------
void test_DS3231(bool stav)
  {    
    test_text(20);                                      //  "rtc 2"

    if (stav == false)                                  // funkce 'u'
      {
        Serial.print(lng314);                           // "Cas v DS3231: "
        cti_ds3231();                                   // nacteni casovych registru DS3231 a jejich prevod na globalni promenne 'ds_xxx'
        formatuj_ds_cas();                              // posle cas do seriove linky ve format "d.m.yyyy h:nn:ss"        
        RTC_dump();                                     // pod aktualni datum se vypise 18 registru z DS3231
      }
    else                                                // funkce 'U'
      {
        STM_DS(true);                                   // nastaveni casu v DS3231 podle casu v STM32F411 (true = alarmove registry v SD3231 se nastavi o 30 sekund pozdeji)
        ukoncit_test = true;
      }
  
  }
//----------------------------------------------



  
