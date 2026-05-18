//========================================================================================
//       Svetelny kalibrator
//
//
#define    verze_prg   "ver.: 2025-11-25" 
//
//========================================================================================
//  Obsahuje servo pro pohyb testovaneho SQM a 2 proudove ovladatelne LED (bilou a infra)
//  Pohyb serva a prepinani jasu LED je mozne i pomoci pripojitelneho joysticku nebo alternativnimi tlacitky
//  Zakladni informace (uhel natoceni, jas LED, zhruba vypocteny proud LEDkami) se zobrazuji na jednoduchem 4-znakovem sedmisegmentovem displeji.
//  Pro opticke nasmerovani slouzi 2 lasery (jeden u serva a druhy u LED), ktere v rezimu zamireni stridave blikaji
//
//  Z MASTERA (amaterskeho SQM nebo z frekvencni ctecky) je kalibrator ovladany pomoci I2C prikazu a parametru
//
// Prijimaji se vzdycky 3 bajty

// Ovladaci I2C prikazy kalibratoru
//   1. bajt je prikaz:         10 = pohyb serva
//                              20 = ovladani napajeni serva
//                              30 = ovladani bile LED
//                              40 = ovladani infra LED
//                              50 = ovladani displeje (napisy / automaticke zhasinani)
//                              60 = ovladani LASERU
//                              70 = aktualizovat mereni proudu
//                              80 = defaultni stav
//                             100 = pozadavek o data z kalibratoru
//                             110 = pozadavek o kalibracni proudove konstanty z kalibratoru
//
//  2. a 3. bajt je parametr (vzdycky 2 bajty v poradi MSB a LSB)
//                             pro prikaz 10 ........... 0 az 180 = uhel natoceni serva
//                             pro prikaz 20 ........... 0 = vypnout napajeni serva
//                                                       1 = zapnout napajeni serva
//                             pro prikazy 30 a 40  .... 0 az 1023 = ovladani 10-bitoveho D/A prevodniku
//                             pro prikaz 50 ........... 0 az 19 = predpripravene napisy na displeji
//                                                       100 = displej bude pri zobrazeni uhlu nebo jasu trvale zhasnuty
//                                                       101 = displej bude pri zobrazeni uhlu nebo jasu trvale rozsviceny
//                                                       102 = displej pri zobrazeni uhlu nebo jasu jen na chvilku blikne
//                             pro prikaz 60 ........... 0 = lasery zhasnute
//                                                       1 = lasery blikaji
//                             pro prikaz 70 ........... 0 = aktualizovat mereni proudu (jen se znova nactou hodnoty na A/D vstupech a zapisi se do pole)
//                             pro prikaz 80 ........... 0 = nastavit defaultni stav (zhasnout LED a lasery, na displeji zobrazit aktualni uhel, displej nastavit na trvale rozsviceno)
//                             pro prikaz 100 .......... 0 = stahnout pole s aktualnimi hodnotami (kazdy index pole je jeden bajt) 
//                                                          [0] = uhel natoceni (0 az 180, stredova poloha je 90) - pozn. na displeji se zobrazuje (hodnota - 90 stupnu), takze stredova poloha je 0
//                                                          [1] = hodnota D/A prevodniku pro bilou LED MSB
//                                                          [2] = hodnota D/A prevodniku pro bilou LED LSB
//                                                          [3] = hodnota D/A prevodniku pro infra LED MSB
//                                                          [4] = hodnota D/A prevodniku pro infra LED LSB
//                                                          [5] = skutecny proud bilou LED - merene napeti na snimacim odporu na prvnim analogovem vstupu (defautne PA2) MSB
//                                                          [6] = skutecny proud bilou LED - merene napeti na snimacim odporu na prvnim analogovem vstupu (defautne PA2) LSB
//                                                          [7] = skutecny proud infra LED - merene napeti na snimacim odporu na druhem analogovem vstupu (defautne PA3) MSB
//                                                          [8] = skutecny proud infra LED - merene napeti na snimacim odporu na druhem analogovem vstupu (defautne PA3) LSB
//                                                          [9] = info / status 
//                                                                       bit0: 1=napajeni serva zapnuto ; 0=napajeni serva vypnuto
//                                                                       bit1: 1=lasery zapnute ; 0=lasery vypnute
//                                                                       bit2: stav fotozavory
//                                                                       bit3: 1=displej neco zobrazuje; 0=displej je zhasnuty
//                                                                       bit4: 1="PUSH" (ceka na rucni nastaveni nulove polohy); 0=neceka na nulovovou polohu
//                                                                       bit5: nepouzito
//                                                                       bit6: nepouzito
//                                                                       bit7: vzdycky 1 (hlavne kvuli vypisu v binarnim stavu do seriove linky, aby se nemusely doplnovat uvodni nuly)
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


//     I kdyz by pro nektere parametry stacilo odesilat jen 1 bajt (ovladani laseru, nastaveni uhlu), pro zachovani jednotneho ovladani se odesilaji oba 2 bajty              
//          priklady:
//                        10,   0, 150  = natoci servo na 150 stupnu (kdyz je zapnute napajeni serva, program zajisti pomaly prejezd na pozici)
//                        20,   0,   0  = vypne napajeni serva
//                        30,   0, 255  = na vystupu pro bilou LED nastavi ctvrtinovy proud (hodnota 255 z 1023)
//                        40,   2,   0  = na vystupu pro infra LED nastavi polovicni proud (hodnota 512 z 1023)
//                        50,   0,   6  = na displeji zobrazi napis s indexem [6] ("noSd")
//                        60,   0,   1  = rozblika lasery
//                       100,   0,   0  = odesle exportni pole dat (9 bajtu) z kalibratoru do MASTERa
//                       110,   0,   3  = odesle kalibracni hodnoty pro nastaveni 1.5 mA pro obe LED (dohromady 4 bajty)
//
//  Existuje i moznost ovladani pres USB seriovou linku.
//   Odeslani dvou oddelenych cisel v textovem formatu (prikaz, parametr), ale je to jen pozustatek z testovani pri vyvoji. Neni to planovano jako hlavni ovladaci funkce.
//      priklady: 10,  80 = natoci servo na 80 stupnu
//                40, 512 = nastavi infra LED na polovicni proud
//               100,   0 = do seriove linky vypise pole exportnich dat
//
//       Specialni prikazy pro seriovou linku, ktere nejsou jinymi prostredky dostupne:
//            111 , uroven , hodnota = nastaveni kalibracni proudove hodnoty pro zadanou uroven pro bilou LED (zapisuje se do EEPROM)
//            112 , uroven , hodnota = nastaveni kalibracni proudove hodnoty pro zadanou uroven pro infra LED (zapisuje se do EEPROM)
//            254 = vypis verze programu (napriklad "ver.: 2025-06-13")
//            253 = testovaci funkce (zobrazi se ovladaci menu. Prikazy jsou znaky odesilane pres seriovou linku
//                      (I2C sken, test displeje, test A/D prevodniku, test DAC, test optozávory ... )
//                      
//            252 , 1 = HW prohozeni bileho a infra kanalu. Menu, ovladani i datove vystupy jsou navenek stejne, prohazuji se jen vnitrni HW linky - zapisuje se do EEPROM
//                                                         (DAC0 = infra, DAC1 = bila; PA2 = snimani proudu infra LED, PA3 = snimani proudu bilou LED; )
//            252 , 0 = HW kanaly se vrati na puvodne planovane funkce - zapisuje se do EEPROM
//                                                         (DAC0 = bila, DAC1 = infra; PA2 = snimani proudu bilou LED, PA3 = snimani proudu infra LED; )
//            250 , 9999 = nastaveni defaultnich hodnot pro kalibraci proudu - zapisuje se do EEPROM
 
//========================================================================================

#include <FlashStorage_STM32.h>                     // knihovna pro virtualni EEPROM, ktera se chova jako obycejna EEPROM z Arduina (8-bitu)

#include <Wire.h>                                   // knihovna pro I2C
#include <Servo.h>                                  // knihovna pro servo
Servo servo1;


#define I2C_ADRESA 0x33                             // adresa teto desky vuci MASTERovi
#define I2C_DAC    0x60                             // adresa D/A prevodniku

#define pin_FB_I_1        PA2                       // prvni analogovy vstup pro mereni napeti na prvnim snimacim odporu (defaultne BILA LED)
#define pin_FB_I_2        PA3                       // druhy analogovy vstup pro mereni napeti na druhem snimacim odporu (defaultne INFRA LED)
#define pin_servo_PWR     PA11                      // pin pro vypinani napajeni serva
#define pin_servo         PA8                       // pin pro ovladani nataceni srva (PWM vystup)
#define pin_laser_1       PA7                       // pin pro ovladani zamerovaciho laseru na strane u kalibratoru
#define pin_laser_2       PB4                       // pin pro ovladani zamerovaciho laseru na strane u LED
#define pin_joy_BUT       PA4                       // pin pro tlacitko na joysticku (v soucasne verzi slouzi jen ke zhasinani displeje)
#define pin_opto_kolektor PA6                       // pin pro fotocidlo na stredove poloze serva (90 stupnu)
#define pin_dis_CLK       PB12                      // pin pro ovladani displeje (hodiny)
#define pin_dis_DIN       PB13                      // pin pro ovladani displeje (data)
#define pin_joy_X         PA0                       // piny pro ovladani joystickem (osa X = pohyb mezi polozkami menu)
#define pin_joy_Y         PA1                       // piny pro ovladani joystickem (osa Y = pridavani a ubirani hodnoty ve zvolene polozce menu)
#define pin_ref_U         PB1                       // vstup mereni referencniho napeti
#define pin_test_12V      PB0                       // vstup pro mereni nastaveneho napeti 12V

TwoWire Wire_DAC(PB11, PB10);                       // Druha I2C linka pro D/A prevodnik (SDA2=PB11, SCL2=PB10)

volatile bool byl_prijem = false;
volatile uint8_t I2C_prikaz;
volatile uint8_t I2C_param_MSB;
volatile uint8_t I2C_param_LSB;
uint16_t I2C_param;
bool servo_pwr = false;                             // znacka, jestli ma servo zapnute napajeni (pres trnazistor ovladany pinem 'pin_servo_PWR')

byte prohodit_kanaly = 0;                           // moznost prohodit bily a infra kanal (0= neprohazovat; 1 = prohodit; >1 = prvni spusteni programu, nastavit na 0)
                                                    //   pri true se LED prohodi na konektoru, ale jinak zustane ovladani, napisy a vsechno ostatni stejne
                                                    //   prohazovani se zajisti prepnutim vystupu DAC a hodnoty zmerene na FB-I-A nebo FB-I-B se priradi ke spravnym vystupnim bunkam pole

uint8_t EXPORT_pole[10];                            // pole, ktere se na pozadani (prikaz 100,0,0) odesila do MASTERA pres I2C

uint16_t proudove_pole[70];                         // pole s proudovymi konstantami, ktere se na pozadani (prikaz 110,0,x) odesila do MASTERA pres I2C


//-------- promenne pro displej---------
byte D_pamet[4];                                    // V tomto poli se bitove uchovava aktualni stav vsech rozsvicenych segmentu

                  // definice znaku pro displej se spolecnou anodou
                  //       0    ,       1    ,       2    ,       3    ,       4    ,       5    ,       6    ,       7    ,       8    ,       9    
byte graf_def[10] = {0b00111111 , 0b00000110 , 0b01011011 , 0b01001111 , 0b01100110 , 0b01101101 , 0b01111101 , 0b00000111 , 0b01111111 , 0b01101111 };

byte dis_popisy[35][4] =                            // graficke definice 4-znakovych textu na displej
    {
      { 115,  28, 109, 118} ,                       // "PuSH"      [0]
      {  80,  28,  84,   0} ,                       // "run "      [1]
      { 109, 120,  92, 115} ,                       // "StoP"      [2]
      {  30, 119, 109,   0} ,                       // "JAS "      [3]
      { 109,  55, 121,  80} ,                       // "SMEr"      [4]      
      {   0,   0,   0,   0} ,                       // "    "      [5]
      {  84,  92, 109,  94} ,                       // "noSd"      [6]      
      { 124, 120,  84,   0} ,                       // "btn "      [7]      
      { 121,  80,  80,   0} ,                       // "Err "      [8]      
      {   0,  92, 113, 113} ,                       // " oFF"      [9]
      {   0,  92,  84,   0} ,                       // " on "      [10]
      {  80, 121, 113,  28} ,                       // "rEFu"      [11]
      {  57,  55, 119,  15} ,                       // "[MA]"      [12]
      {  57,  94,   4,  15} ,                       // "[di]"      [13]
      {  55, 119,  84,  28} ,                       // "MAnu"      [14]
      {  80, 109, 121, 120} ,                       // "rSEt"      [15]

      {   0,   0,   0, 255} ,                       // "   8"      [16]    - rezervy
      {   0,   0, 255,   0} ,                       // "  8 "      [17]
      {   0, 255,   0,   0} ,                       // " 8:  "     [18]    test rozsviceni dvojtecky mezi druhou a treti sedmisegmentovkou
      { 255,   0,   0,   0} ,                       // "8   "      [19]

      {  28, 118, 121,  56} ,                       // "uHEL"      [20]    - MENU pro ovladani joystickem
      { 124,   4,  56, 119} ,                       // "biLA"      [21]
      {   4,   6,  64, 124} ,                       // "i1-b"      [22]
      {   4,  84, 113,  80} ,                       // "inFr"      [23]
      {   4,  91,  64,   4} ,                       // "i2-i"      [24]
      {  56, 119, 109, 121}                         // "LASE"      [25]
    }; 



String txt_urovne[] =                               // textovy popis kalibracnich proudovych urovni
    {
      " [0] =  0.0 mA",       // [0]
      " [1] =  0.5 mA" ,      // [1]
      " [2] =  1.0 mA" ,      // [2]
      " [3] =  1.5 mA" ,      // [3]
      " [4] =  2.0 mA" ,      // [4]
      " [5] =  2.5 mA" ,      // [5]
      " [6] =  3.0 mA" ,      // [6]
      " [7] =  3.5 mA" ,      // [7]
      " [8] =  4.0 mA" ,      // [8]
      " [9] =  4.5 mA" ,      // [9]
      "[10] =  5.0 mA" ,      // [10]
      "[11] =  6.0 mA" ,      // [11]
      "[12] =  7.0 mA" ,      // [12]
      "[13] =  8.0 mA" ,      // [13]
      "[14] =  9.0 mA" ,      // [14]
      "[15] = 10.0 mA"        // [15]
    };

uint32_t cas_bliku_laseru;                          // stridave blikani dvema lasery frekvenci 0.5 sek.
bool aktual_laser;

uint32_t cas_aktualizace_proudu;                    // pro sekundove aktualizace zobrazeni skutecnych proudu (hodnot na digitalnich vstupech) na displeji

uint16_t uhel = 90;                                 // prubezne aktualizovana poloha uhlu (kdyz by doslo k pozadavku o posun o vetsi uhel, bude se najizdet postupne krok za krokem)
uint16_t jas_bila = 0;                              // prubezne aktualizovany jas bile LED
uint16_t jas_infra = 0;                             // prubezne aktualizovany jas infra LED

uint16_t pamet_pozadavku;                           // pri pokusu o zmenu polohy pri vypnutem napajeni serva se sem uklada posledni pozavek. Ke zpracovani tohoto pozadavku dojde pri dalsim zapnuti napajeni
uint16_t pozadovany_uhel;                           // docasna promenna pro zapamatovani polohy, na kterou ma najet servo

uint32_t t_start;

uint8_t polozka_menu = 20;                          // pro prepinani polozek v menu (cisluje se podle textovych definic - 20 az 25)
bool prvni_krok_y = true;
bool superfast = false;                             // po nekolika sekundach drzeni tlacitka nahoru nebo dolu se zacne jas zvysovat nebo snizovat po 10 digitech
#define superfast_delay           5000              // po jak dlouhle dobe drzeni tlacitka [ms] se aktivuje rezim superfast
uint16_t ady;
uint16_t adx;
bool prvni_ma = true;                               // v polozkach mereni proudu se pri drzeni tlacitka prepina na zobrazeni [mA] a prvni vstup do tohoto zobrazeni se kratce signalizuje zobrazenim textu "[MA]" na displeji
bool prvni_di = true;                               // v polozkach mereni proudu se pri uvolneni tlacitka prepina na zobrazeni [dig] a prvni vstup do tohoto zobrazeni se kratce signalizuje zobrazenim textu "[di]" na displeji
 
uint8_t zmena;                                      // pri normalni rychlosti zadavanych hodnot se hodnoty meni po 1; pri velmi rychle zmeme zadavanych hodnot se hodnoty meni po 10

uint16_t joy_X_0 = 2040;                            // hodnoty na analogovych vstupech pri vyrovnanem joysticku
uint16_t joy_Y_0 = 2040;
uint16_t joy_vychylka = 500;                        // odchylka joysticku od nulove polohy, od ktere se vyhodnoti naklon joysticku

bool laser_stav = false;

uint8_t funkce_displeje = 1;                        // 0 ... displej je trvale zhasnuty. Krome specialnich informaci (PuSH, StoP, Err ...)
                                                    // 1 ... displej trvale sviti a zobrazuje skutecny uhel natoceni
                                                    // 2 ... udaj o uhlu na displeji jen blikne a zhasne



//========================================================================================
void setup()
  {
    Serial.begin(9600);
    analogReadResolution(12);
    Wire.begin(I2C_ADRESA);
    Wire.onRequest(onRequestHandler);
    Wire.onReceive(onReceiveHandler);
    Wire_DAC.begin();                               // pro druhou I2C linku se procesor chova jako MASTER a ovlada DAC

    pinMode(pin_servo,OUTPUT);                      // nastaveni smeru pinu
    servo1.attach(pin_servo,544,2400);              // nastaveni serva


    pinMode(pin_servo_PWR,OUTPUT);                  // napajeni serva
    digitalWrite(pin_servo_PWR, LOW);               // okamzite vypnout

    pinMode(pin_laser_1,OUTPUT);                    // laser pro zamireni na zdroj svetla na strane u SQM 
    digitalWrite(pin_laser_1, LOW);                 // okamzite vypnout

    pinMode(pin_laser_2,OUTPUT);                    // laser pro zamireni na cidlo SQM na strane u LED
    digitalWrite(pin_laser_2, LOW);                 // okamzite vypnout

    pinMode(pin_joy_BUT,INPUT_PULLUP);              // tlacitko na joysticku
 
    pinMode(pin_opto_kolektor,INPUT_PULLUP);        // testovani rucniho najeti na stredovou polohu pomoci fotozavory

    pinMode(pin_dis_CLK,OUTPUT);                    // ovladani displeje
    pinMode(pin_dis_DIN,OUTPUT);

    pinMode(pin_FB_I_1   ,  INPUT_ANALOG);          // (analogovy vstup)
    pinMode(pin_FB_I_2   ,  INPUT_ANALOG);          // (analogovy vstup)
    pinMode(pin_joy_X    ,  INPUT_ANALOG);          // (analogovy vstup)
    pinMode(pin_joy_Y    ,  INPUT_ANALOG);          // (analogovy vstup)
    pinMode(pin_ref_U    ,  INPUT_ANALOG);          // (analogovy vstup)
    pinMode(pin_test_12V ,  INPUT_ANALOG);          // (analogovy vstup)

    prohodit_kanaly = EEPROM.read(100);             // nastaveni prirazeni HW kanalu se uklada do virtualni EEPROM (FLASH)
    if (prohodit_kanaly > 1)                        // prvni spusteni, pri kterem je v EEPROM nesmyslna hodnota
      {
        prohodit_kanaly = 0;                        // defaultne je prvni kanal bily a druhy infra
        EEPROM.write(100, 0); 
      }


    writeToDAC(0,0);                                // defaultne bila LED zhasnout
    writeToDAC(1,0);                                // defaultne infra LED zhasnout

    servo1.write(90);                               // nastavit na stred  (90 stupnu)
    delay(200);

    EXPORT_pole[9] = 0b10000000;                    // napajeni serva vypnute;  lasery zhasnute; displej zhasnuty; nejvyssi bit vzdycky '1'
    if (digitalRead(pin_opto_kolektor) == LOW)   bitClear(EXPORT_pole[9],2);               //aktualni stav fotozavory
    else                                           bitSet(EXPORT_pole[9],2);

    for (byte i = 0 ; i < 64 ; i++)
      {
        proudove_pole[i] = EEPROM.read(i);          // POZOR - do EEPROM se ukladaji bloky 8-bitovych hodnot (MSB bila, LSB bila, MSB infra, LSB infra)
      }


    zobraz_text(0);                                 // "PuSH" (cekani na rucni nastaveni serva do nulove polohy)
    bitSet(EXPORT_pole[9],4);                       // na displeji sviti "PuSH"
    while (digitalRead(pin_opto_kolektor) == LOW)   // fotocidlo dava v libovolne poloze LOW, akorat ve stredni poloze (90 stupnu) na chvili prepne do HIGH (dira v odrazne plose)
      {
        if (Serial.available() > 0)                 // pri cekani na rucni nastaveni nulove polohy neco prislo pres seriovou linku
          {
            serkom();                               // pres seriovou linku je mozne i pri cekani na nulovou polohu komunikovat
          }
      }

    bitClear(EXPORT_pole[9],4);                     // na displeji uz nesviti "PuSH"

    delay(100);
    digitalWrite(pin_servo_PWR, HIGH);              // ve stredove poloze (90 stupnu) se zapina napajeni serva
    bitSet(EXPORT_pole[9],0);                       // napajeni serva zapnute    
    servo_pwr = true;

    zobraz_text(20);                                // "uHEL"
    delay(1000);
    zobraz_cislo(90-90 , 0);                        // aktualni poloha je 90 stupnu, zobrazuje se ale 0 (zobrazovany uhel je -90 az 90 stupnu, vnitrne ale pocita v rozsahu 0 az 180 stupnu)
    uhel = 90;
    EXPORT_pole[0]= uhel;
  }
//========================================================================================



//========================================================================================
void loop()
  {
    //--- rucni testovaci ovladani pres USB ----------------------------------
    if (Serial.available() > 0)                                   // neco prislo pres seriovou linku
      {
        serkom();
      }
    //--- konec rucniho testovaciho ovladani pres USB ------------------------


    //--- ovladani pres I2C --------------------------------------------------
    if (byl_prijem == true)                                       // pres I2C prisel novy pozadavek
      {
        byl_prijem = false;
        I2C_param = (I2C_param_MSB << 8) + I2C_param_LSB;

        if (I2C_prikaz == 10)         servo_uhel(I2C_param);      // pomale najeti serva na pozaovany uhel

        if (I2C_prikaz == 20)                                     // napajeni serva
          {
            if (I2C_param == 0)
              {
                servo_pwr = false;
                digitalWrite(pin_servo_PWR,LOW);                  // vypnuti napajeni serva
                bitClear(EXPORT_pole[9],0);                       // napajeni serva vypnute
              }
            if (I2C_param == 1)
              {
                servo_pwr = true;
                digitalWrite(pin_servo_PWR,HIGH);                 // zapnuti napajeni serva
                bitSet(EXPORT_pole[9],0);                         // napajeni serva zapnute
                if (uhel != pamet_pozadavku)                      // kdyz pri vypnutem napajeni doslo k nejakemu pozadavku na zmenu uhlu natoceni, provede se hned po zapnuti napajeni serva
                  {
                    servo_uhel(pamet_pozadavku); 
                  }
              
              }
          }

        if (I2C_prikaz == 30)         writeToDAC(0 , I2C_param);  // nastaveni bile LED

        if (I2C_prikaz == 40)         writeToDAC(1 , I2C_param);  // nastaveni infra LED

        if (I2C_prikaz == 50)                                     // ovladani displeje
          {
            if (I2C_param == 100)     funkce_displeje = 0;        // displej nebude zobrazovat uhly
            if (I2C_param == 101)     funkce_displeje = 1;        // displej bude zobrazovat uhly trvale
            if (I2C_param == 102)     funkce_displeje = 2;        // uhly na displeji jen kratce bliknou a pak displej zhasne

            if (I2C_param < 20)       zobraz_text(I2C_param);     // zobrazeni prednastaveneho textu
          }
        
        if (I2C_prikaz == 60)         laser(I2C_param);           // nastaveni laseru
        
        if (I2C_prikaz == 70)         aktualizuj_A_D();           // jen nacte aktualni hodnoty mereni proudu na analogovych vstupech do exportniho pole

        if (I2C_prikaz == 80)         rSEt();                     // neco jako reset (zhasnuti LED a laseru, nastaveni displeje na zobrazeni uhlu)

      }
    //--- konec  ovladani pres I2C -------------------------------------------


    //--- ovladani pres joystick ---------------------------------------------
    adx = analogRead(pin_joy_X);
    ady = analogRead(pin_joy_Y);
    
    if (adx < (joy_X_0 - joy_vychylka))                  joystick_menu_minus();                 // pohyb v polozkach menu
    if (adx > (joy_X_0 + joy_vychylka))                  joystick_menu_plus();                  // pohyb v polozkach menu

    
    if (ady > (joy_Y_0 + joy_vychylka))                  joystick_plus_Y();                     // pridavani hodnoty nastavene polozky menu
    if (ady < (joy_Y_0 - joy_vychylka))                  joystick_minus_Y();                    // ubirani hodnoty nastavene polozky menu
    if (ady > ((joy_Y_0 - joy_vychylka)) and (ady < (joy_Y_0 + joy_vychylka)))                  // joystick ve stredove poloe, pristi vychyleni bude pomale krokovani
      {
        prvni_krok_y = true;
      }

    if (digitalRead(pin_joy_BUT) == HIGH)                         // tlacitko na joysticku uvolneno
      {
        if ((polozka_menu == 22 and prvni_di == true) or (polozka_menu == 24 and prvni_di == true))  // zobrazeni proudu
          {
            zobraz_text(13);                                      // proud je zobrazen v digitech "[di]"
            delay(500);
            prvni_di = false;
          }
      }

    if (digitalRead(pin_joy_BUT) == LOW)                          // tlacitko na joysticku stisknuto 
      {
        prvni_di = true;
        if ((polozka_menu != 22) and (polozka_menu != 24))        // zhasina displej (krome polozek pro mereni proudu (22 a 24)
          {
            zobraz_text(5);         
          }
        else                                                      // v polozkach 22 a 24 ze pri prvnim stisku zobrazi znacka "[mA]"
          {
            if (prvni_ma == true)                                 // (jen pri prvnim stisku)
              {
                zobraz_text(12);
                delay(500);
                prvni_ma = false;
              }
          }
      }
    //--- konec ovladani pres joystick ---------------------------------------


    //--- pro stridave blikani obou laseru -----------------------------------
    if (laser_stav == true)
      {
        if ((millis() - cas_bliku_laseru) > 500)
          {
            cas_bliku_laseru = millis();
            if (aktual_laser == true)
              {
                aktual_laser = false;
                digitalWrite(pin_laser_2, false);
                digitalWrite(pin_laser_1, true);
              }
            else
              {
                aktual_laser = true;
                digitalWrite(pin_laser_1, false);
                digitalWrite(pin_laser_2, true);
              }
          }
      }
    else                                                          // laser_stav je false, oba lasery se vypinaji
      {
        digitalWrite(pin_laser_1, false);
        digitalWrite(pin_laser_2, false);
      }
    //--- konec ovladani blikani laseru --------------------------------------


    if (polozka_menu == 22 or polozka_menu == 24)                 // pro polozky menu pro zobrazeni zmerenych proudu se kazdou sekundu provadi aktualizace merenych proudu
      {
        if ((millis() - cas_aktualizace_proudu) > 1000)
          {
            aktualizuj_A_D();
            if (polozka_menu == 22)
              { 
                if (digitalRead(pin_joy_BUT) == LOW)
                  {
                    zobraz_cislo(prevod_na_mA(1,((EXPORT_pole[5]) << 8) + EXPORT_pole[6]), 1);  // pri stisku tlacitka zobrazuje prevod na mA
                  }
                else
                  {
                    zobraz_cislo(((EXPORT_pole[5]) << 8) + EXPORT_pole[6] , 0);                
                    prvni_ma = true;
                  }
              }


            if (polozka_menu == 24)
              {
                if (digitalRead(pin_joy_BUT) == LOW)
                  {
                    zobraz_cislo(prevod_na_mA(2,((EXPORT_pole[7]) << 8) + EXPORT_pole[8]) , 1);  // pri stisku tlacitka zobrazuje prevod na mA                
                  }
                else
                  {
                    zobraz_cislo(((EXPORT_pole[7]) << 8) + EXPORT_pole[8] , 0);                
                    prvni_ma = true;
                  }
              }
            cas_aktualizace_proudu = millis();
          }
      }

    if (digitalRead(pin_opto_kolektor) == LOW)   bitClear(EXPORT_pole[9],2);               //aktualni stav fotozavory
    else                                           bitSet(EXPORT_pole[9],2);
  }
//========================================================================================



//========================================================================================
// zmeri napeti na analogovych obou vstupech pro mereni proudu skrz LED a hodnoty ulozi do exportniho pole
void aktualizuj_A_D(void)
  {
    // skutecny proud LEDkami se aktualizuje pri kazde zmene DAC - nezavisle na kanale
    uint16_t mereni;
    
    if (prohodit_kanaly == 0)   mereni = prumeruj(pin_FB_I_1);    // bila na pinu FB_I_1
    else                        mereni = prumeruj(pin_FB_I_2);    // bila na pinu FB_I_2
    EXPORT_pole[5] = (mereni >> 8);                               // MSB skutecneho proudu bilou LED
    EXPORT_pole[6] = (mereni & 0xFF);                             // LSB skutecneho proudu bilou LED

    if (prohodit_kanaly == 0)   mereni = prumeruj(pin_FB_I_2);    // infra na pinu FB_I_2
    else                        mereni = prumeruj(pin_FB_I_1);    // infra na pinu FB_I_1
    EXPORT_pole[7] = (mereni >> 8);                               // MSB skutecneho proudu infra LED
    EXPORT_pole[8] = (mereni & 0xFF);                             // LSB skutecneho proudu infra LED    
  }
//========================================================================================



//========================================================================================
// prumerovani hodnot na analogovem vstupu s kontrolou rozptylu minima a maxima
//      kdyz jsou hodnoty nestabilní (rozptyl je vetsi nez 5 digitu), provede se jeste jeden nebo dva pokusy o prumerovani
uint16_t prumeruj(uint16_t vstupni_pin)
  {
    uint16_t napeti;
    uint16_t soucet = 0;
    uint16_t min_vzorek = 4095;
    uint16_t max_vzorek = 0;
    float rozptyl = 9999;
    byte pokusy = 0;
    float prumer = 0;
    while (rozptyl > 5 and pokusy < 3)
      {
        soucet = 0;
        min_vzorek = 4095;
        max_vzorek = 0;
        for (byte i = 1; i < 5 ; i++)                             // 4 vzorky k prumerovani
          {
            delay(10);
            napeti = analogRead(vstupni_pin);
            if (napeti > max_vzorek) max_vzorek = napeti;
            if (napeti < min_vzorek) min_vzorek = napeti;
            soucet = soucet + napeti;
          }
        rozptyl = max_vzorek - min_vzorek;
        prumer = (soucet >> 2 ) + 0.5;                            // prumer = (soucet / 4) + 0.5
        pokusy ++;
      }  
    return (uint16_t)prumer;
  }
//========================================================================================



//========================================================================================
// pro zobrazeni skutecnych proudu se pouziva polynomicky vzorec (vypocteny pomoci OpenAI z realne zmerenych vzorku)
//      Pro kazdy kanal muze byt prepocet odlisny
// skutecne zmereny proud pres LED a tomu odpovidajici digity na analogovych vstupech pro zpetne cteni proudu
            //   [mA] ;  bila ; infra ;
            //   0.00 ;    32 ;     5 ;
            //   0.50 ;   339 ;   287 ;
            //   1.00 ;   569 ;   515 ;
            //   1.50 ;   791 ;   738 ;
            //   2.00 ;  1020 ;   965 ;
            //   3.00 ;  1458 ;  1407 ;
            //   4.00 ;  1842 ;  1817 ;
            //   5.00 ;  2133 ;  2150 ;
            //   6.00 ;  2343 ;  2391 ;
            //   7.00 ;  2497 ;  2569 ;
            //   8.00 ;  2618 ;  2706 ;
            //   9.00 ;  2717 ;  2815 ;
            //  10.00 ;  2800 ;  2907 ;
            //  11.00 ;  2872 ;  2985 ;
            //  11.56 ;  2907 ;  3023 ;
uint16_t prevod_na_mA(byte kanal, uint16_t x)
  {
    float vysledek = 0;
    uint16_t navrat;
    if (kanal == 1)
      {
        vysledek = (5.585E-13*x*x*x*x) - (2.4006E-9*x*x*x) + (3.4955E-6*x*x) + (3.496E-4*x) + 1.629E-2;
      }

    if (kanal == 2)
      {
        vysledek = (4.426E-13*x*x*x*x) - (1.9464E-9*x*x*x) + (2.832E-6*x*x) + (7.851E-4*x) + 3.373E-2;
      }
    navrat = (vysledek * 100) + 0.5;                              // na dislpeji se zobrazuji [mA] na 2 desetinna mista
    return navrat;
  }

//========================================================================================




//========================================================================================
//          ****     I 2 C     ****
//========================================================================================
// Obsluha I2C pozadavku od MASTERa
void onRequestHandler()                                           // kod prikazu 100 odesle zpatky do MASTERa 10-bajtove pole
  {
    if (I2C_prikaz == 100)                                        // mohly by se tu testovat jeste parametry, ale vynechavam to. U prikazu 100 na parametech nezalezi.
      {
        Wire.write(EXPORT_pole, 10);
      }

    if (I2C_prikaz == 110)                                        // kod 110 odesila 4 bajty kalibracnich proudovych konstant podle zadaneho parametru 
      {
         Wire.write(proudove_pole[(I2C_param_LSB * 4) + 0]);
         Wire.write(proudove_pole[(I2C_param_LSB * 4) + 1]);
                  
         Wire.write(proudove_pole[(I2C_param_LSB * 4) + 2]);
         Wire.write(proudove_pole[(I2C_param_LSB * 4) + 3]);
      } 
  }
//========================================================================================



//========================================================================================
// prichozi prikaz pres linku I2C od MASTERa
void onReceiveHandler(int howMany)
  {
    I2C_prikaz = Wire.read();                                     // Prvni bajt je prikaz
    I2C_param_MSB = Wire.read();                                  // Druhy bajt je MSB parametru
    I2C_param_LSB = Wire.read();                                  // Treti bajt je LSB parametru
    byl_prijem = true;
  }
//========================================================================================



//========================================================================================
//          ****     P E R I F E R I E     ****
//========================================================================================
// Zapis hodnoty do vybraneho kanalu DAC
// Zaroven se aktualizuji prislusne polozky v exportnim poli (vcetne skutecne zmereneho proudu LEDkama)
void writeToDAC(uint8_t kanal, uint16_t hodnotaDAC)
  {
    uint8_t command; 
    if (prohodit_kanaly == 0)                                     // pri defaultne prirazenych kanalech ...
      {
        if (kanal == 0)   command = 0b00000000;                   // zapsat do kanalu 0 (bila LED)
        if (kanal == 1)   command = 0b00001000;                   // zapsat do kanalu 1 (infra LED)
      }
    else                                                          // pri prohozenych kanalech ...
      {
        if (kanal == 0)   command = 0b00001000;                   // zapsat do kanalu 1 (bila LED)
        if (kanal == 1)   command = 0b00000000;                   // zapsat do kanalu 0 (infra LED)            
      }
    uint8_t msb = (hodnotaDAC >> 8) & 0b00000011;                 // hornich 8 bitu (pouzity jsou jen bity 9 a 8 ze vstupniho 16-bitoveho cisla)
    uint8_t lsb = (hodnotaDAC & 0xFF);                            // dolnich 8 bitu

    Wire_DAC.beginTransmission(I2C_DAC);                          // Zacatek prenosu na adrese DAC
    Wire_DAC.write(command);                                      // zapsat do kanalu 0 nebo 1
    Wire_DAC.write(msb);                                          // MSB dat
    Wire_DAC.write(lsb);                                          // LSB dat
    byte i2c_status =  Wire_DAC.endTransmission(true);            // Konec prenosu 
                                                       
    if (i2c_status != 0)      msb = msb | 0b10000000;             // pri chybe komunikace s DAC se nahodi nejvyssi bit zapisovane hodnoty na '1' (za normalnich okolnosti je hodnota maximalne 1023)
    EXPORT_pole[(kanal*2) + 1] = msb;                             // MSB pozadovane hodnoty se ulozi do pole s indexem [1](kanal 0), nebo [3](kanal 1)
    EXPORT_pole[(kanal*2) + 2] = lsb;                             // LSB pozadovane hodnoty se ulozi do pole s indexem [2](kanal 0), nebo [4](kanal 1)

    if (kanal == 0)  jas_bila = hodnotaDAC;                       // prubezne aktualizovana hodnota jasu bile LED
    if (kanal == 1)  jas_infra = hodnotaDAC;                      // prubezne aktualizovana hodnota jasu infra LED
    delay(50);                                                    // chvili pauza na ustaleni proudu snimacimi odpory
    aktualizuj_A_D();                                             // zmeri napeti na analogovych vsupech pro mereni proudu skrz LED
  }
//========================================================================================



//========================================================================================
// ovladani laseru
void laser(byte prepni)
  {
    if (prepni == 0)
      {
        laser_stav = false;                      // vypne laser
        bitClear(EXPORT_pole[9],1);
      }
    else
      {
        laser_stav = true;                       // zapne laser
        bitSet(EXPORT_pole[9],1);
      }
    

    
  }
//========================================================================================



//========================================================================================
// pomale najeti serva na zadany uhel
void servo_uhel(uint16_t pozadovany_uhel)
  {
    if (servo_pwr == true)                                        // napajeni do serva je zapnute, dochazi k normalnimu postupnemu najizdeni na polohu
      {
        if (pozadovany_uhel <= 180)
          {
            if (uhel < pozadovany_uhel)
              {
                while (uhel < pozadovany_uhel)
                  {
                    uhel = uhel + 1;

                    if (funkce_displeje == 1 or funkce_displeje == 2)  zobraz_cislo(uhel-90 , 0);       // kdyz se ma uhel zobrazovat, tak se zobrazi
                    servo1.write(uhel);
                    delay(40);
                    if (funkce_displeje == 2)                          delay(20);               // pri bliknuti pred smazanim jeste chvili pockat
                    if (funkce_displeje == 0 or funkce_displeje == 2)  zobraz_text(5);          // smazat displej
                  }      
              }
            else
              {
                while (uhel > pozadovany_uhel)
                  {
                    uhel = uhel - 1;

                    if (funkce_displeje == 1 or funkce_displeje == 2)  zobraz_cislo(uhel-90 , 0);       // kdyz se ma uhel zobrazovat, tak se zobrazi
                    servo1.write(uhel);
                    delay(40);
                    if (funkce_displeje == 2)                          delay(20);               // pri bliknuti pred smazanim jeste chvili pockat
                    if (funkce_displeje == 0 or funkce_displeje == 2)  zobraz_text(5);          // smazat displej
                  }      
              }
        
            EXPORT_pole[0] = uhel;
          }
        
      }
    else                                                          // doslo k pokusu o prenastaveni polohy, ale napajeni serva je vypnute
      {
        pamet_pozadavku = pozadovany_uhel;                        // do doby, nez se zapne napajeni, se servo nehybe. Po zapnuti napajeni pak prejizdi na polohu pomalu
      }
  }
//========================================================================================



//========================================================================================
//          ****     J O Y S T I C K     ****
//========================================================================================
// pohyb joystickem mezi polozkami menu
void joystick_menu_minus(void)
  {
    polozka_menu --;

    if (polozka_menu < 20) polozka_menu = 20;
    zobraz_text(polozka_menu);
    delay(500);
    adx = analogRead(pin_joy_X);
    while (adx < (joy_X_0 - joy_vychylka) or adx > (joy_X_0 + joy_vychylka))                    // cekani na srovnani joysticku do nulove polohy
      {
        delay(50);
        adx = analogRead(pin_joy_X);
      }

    if (polozka_menu == 20)  zobraz_cislo(uhel-90 , 0);                                         // servo
    if (polozka_menu == 21)  zobraz_cislo(jas_bila , 0);                                        // bila vstup DAC
    if (polozka_menu == 22)  zobraz_cislo((EXPORT_pole[5] << 8) + EXPORT_pole[6] , 0);          // bila proud
    if (polozka_menu == 23)  zobraz_cislo(jas_infra , 0);                                       // infra vstup DAC
    if (polozka_menu == 24)  zobraz_cislo((EXPORT_pole[7] << 8) + EXPORT_pole[8] , 0);          // infra proud
//  if (polozka_menu == 25) .... // polozka [25] se v tomto podprogramu nikdy nevyskytne


  }
//========================================================================================



//========================================================================================
// pohyb joystickem mezi polozkami menu
void joystick_menu_plus(void)
  {
    polozka_menu ++;

    if (polozka_menu > 25) polozka_menu = 25;
    zobraz_text(polozka_menu);
    delay(500);
    adx = analogRead(pin_joy_X);
    while (adx < (joy_X_0 - joy_vychylka) or adx > (joy_X_0 + joy_vychylka))                    // cekani na srovnani joysticku do nulove polohy
      {
        delay(50);
        adx = analogRead(pin_joy_X);
      }

//  if (polozka_menu == 20)  ....                                                               // polozka [20] se v tomto podprogramu nikdy nevyskytne
    if (polozka_menu == 21)  zobraz_cislo(jas_bila , 0);                                        // bila vstup DAC
    if (polozka_menu == 22)  zobraz_cislo((EXPORT_pole[5] << 8) + EXPORT_pole[6] , 0);          // bila proud
    if (polozka_menu == 23)  zobraz_cislo(jas_infra , 0);                                       // infra vstup DAC
    if (polozka_menu == 24)  zobraz_cislo((EXPORT_pole[7] << 8) + EXPORT_pole[8] , 0);          // infra proud    
    if (polozka_menu == 25)
      {
        if (laser_stav == false) zobraz_text(9);
        if (laser_stav == true)  zobraz_text(10);
      } 
  }
//========================================================================================



//========================================================================================
// Pricitani hodnoty polozky v menu joystickem  az do povoleneho maxima
//   Prvni vychyleni joysticku nahoru provede 1 krok. Pri delsim drzeni vychyleneho joysticku se krokovani zrychli
void joystick_plus_Y(void)
  {
    bool krokplus = false;
 
    if (prvni_krok_y == true)
      {
        krokplus = true;
        prvni_krok_y = false;
        t_start = millis();
        superfast = false;
      }
    else
      {
        if (millis() - t_start > 600)      krokplus = true;       // drzeni tlacitka nad 0,6 sekundy zpusobuje rychle pricitani +1
        else                               krokplus = false;      // drzeni tlacitka pod 0,6 sekundy zpusobuje krokovani

        if (millis() - t_start > superfast_delay)    superfast = true;    // drzeni tlacitka nad zadany pocet milisekund zpusobuje superrychle pricitani +10
        else                                         superfast = false;

      }

    if (krokplus == true)
      {
        if (polozka_menu == 20)                                   // nastaveni uhlu serva
          {
            digitalWrite(pin_servo_PWR,HIGH);                     // pri pokusu o zmenu natoceni pres joystick se zapina napajeni serva
            bitSet(EXPORT_pole[9],0);                             // napajeni serva zapnute
            servo_pwr = true;
            
            if (uhel < 180) pozadovany_uhel = uhel + 1;
            zobraz_cislo(pozadovany_uhel-90 , 0);
            servo_uhel(pozadovany_uhel);
            delay(50);            
          }


        if (polozka_menu == 21)                                   // nastaveni D/A prevodniku pro bilou LED
          {
            digitalWrite(pin_servo_PWR,LOW);                      // nastaveni D/A prevodniku vypina napajeni serva
            bitClear(EXPORT_pole[9],0);                           // napajeni serva vypnute
            servo_pwr = false;
            if (superfast == true and jas_bila < 1013)        zmena = 10;
            else                                              zmena = 1;
            if (jas_bila < 1023) jas_bila = jas_bila + zmena ;
            zobraz_cislo(jas_bila , 0);
            writeToDAC(0, jas_bila);
          }


        if (polozka_menu == 22)                                   // nastaveni D/A prevodniku pro bilou LED (bez zobrazeni jasu, ale se zobrazenim proudu)
          {
            digitalWrite(pin_servo_PWR,LOW);                      // nastaveni D/A prevodniku vypina napajeni serva
            bitClear(EXPORT_pole[9],0);                           // napajeni serva vypnute
            servo_pwr = false;

            if (superfast == true and jas_bila < 1013)        zmena = 10;
            else                                              zmena = 1;
            if (jas_bila < 1023) jas_bila = jas_bila + zmena ;
            writeToDAC(0, jas_bila);
          }


        if (polozka_menu == 23)                                   // nastaveni D/A prevodniku pro infra LED
          {
            digitalWrite(pin_servo_PWR,LOW);                      // nastaveni D/A prevodniku vypina napajeni serva
            bitClear(EXPORT_pole[9],0);                           // napajeni serva vypnute
            servo_pwr = false;
            if (superfast == true and jas_infra < 1013)       zmena = 10;
            else                                              zmena = 1;
            if (jas_infra < 1023) jas_infra = jas_infra + zmena ;
            zobraz_cislo(jas_infra , 0);
            writeToDAC(1, jas_infra);
          }


        if (polozka_menu == 24)                                   // nastaveni D/A prevodniku pro infra LED (bez zobrazeni jasu, ale se zobrazenim proudu)
          {
            digitalWrite(pin_servo_PWR,LOW);                      // nastaveni D/A prevodniku vypina napajeni serva
            bitClear(EXPORT_pole[9],0);                           // napajeni serva vypnute
            servo_pwr = false;
            if (superfast == true and jas_infra < 1013)       zmena = 10;
            else                                              zmena = 1;
            if (jas_infra < 1023) jas_infra = jas_infra + zmena ;
            writeToDAC(1, jas_infra);
          }


        if (polozka_menu == 25)                                   // nastaveni laseru (ON)
          {
            zobraz_text(10);                                      // " on" 
            laser(1);                                             // laser rozblikat
          }
      }
  }
//========================================================================================



//========================================================================================
//  Odecitani hodnoty polozky v menu joystickem az do nuly
//     Prvni vychyleni joysticku dolu provede 1 krok. Pri delsim drzeni vychyleneho joysticku se krokovani zrychli
void joystick_minus_Y(void)
  {
    bool krokminus = false;

    if (prvni_krok_y == true)
      {
        krokminus = true;
        prvni_krok_y = false;
        t_start = millis();
        superfast = false;
      }
    else
      {
        if (millis() - t_start > 600)      krokminus = true;      // drzeni tlacitka nad 0,6 sekundy zpusobuje rychle odecitani -1
        else                               krokminus = false;     // drzeni tlacitka pod 0,6 sekundy zpusobuje krokovani
        
        if (millis() - t_start > superfast_delay)    superfast = true;     // drzeni tlacitka nad zadany pocet milisekund zpusobuje superrychle odecitani -10
        else                                         superfast = false;
      }

    if (krokminus == true)
      {
        if (polozka_menu == 20)                                   // nastaveni uhlu serva
          {
            digitalWrite(pin_servo_PWR,HIGH);                     // pri pokusu o zmenu natoceni pres joystick se zapina napajeni serva
            bitSet(EXPORT_pole[9],0);                             // napajeni serva zapnute
            servo_pwr = true;
            if (uhel > 0) pozadovany_uhel = uhel - 1;
            zobraz_cislo(pozadovany_uhel-90 , 0);
            servo_uhel(pozadovany_uhel);
            delay(50);
          }

        if (polozka_menu == 21)                                   // nastaveni D/A prevodniku pro bilou LED
          {
            digitalWrite(pin_servo_PWR,LOW);                      // nastaveni D/A prevodniku vypina napajeni serva
            bitClear(EXPORT_pole[9],0);                           // napajeni serva vypnute
            servo_pwr = false;               
            if (superfast == true and jas_bila > 10)        zmena = 10;
            else                                            zmena = 1;
            if (jas_bila > 0) jas_bila = jas_bila - zmena ;
            zobraz_cislo(jas_bila , 0);
            writeToDAC(0, jas_bila);
          }

        
        if (polozka_menu == 22)                                   // nastaveni D/A prevodniku pro bilou LED (bez zobrazeni jasu, ale se zobrazenim proudu)
          {
            digitalWrite(pin_servo_PWR,LOW);                      // nastaveni D/A prevodniku vypina napajeni serva
            bitClear(EXPORT_pole[9],0);                           // napajeni serva vypnute
            servo_pwr = false;               
            if (superfast == true and jas_bila > 10)        zmena = 10;
            else                                            zmena = 1;
            if (jas_bila > 0) jas_bila = jas_bila - zmena ;
            writeToDAC(0, jas_bila);
          }

        
        if (polozka_menu == 23)                                   // nastaveni D/A prevodniku pro infra LED
          {
            digitalWrite(pin_servo_PWR,LOW);                      // nastaveni D/A prevodniku vypina napajeni serva
            bitClear(EXPORT_pole[9],0);                           // napajeni serva vypnute
            servo_pwr = false;
            if (superfast == true and jas_infra > 10)        zmena = 10;
            else                                             zmena = 1;
            if (jas_infra > 0) jas_infra = jas_infra - zmena ;
            zobraz_cislo(jas_infra , 0);
            writeToDAC(1, jas_infra);
          }

        if (polozka_menu == 24)                                   // nastaveni D/A prevodniku pro infra LED (bez zobrazeni jasu, ale se zobrazenim proudu)
          {
            digitalWrite(pin_servo_PWR,LOW);                      // nastaveni D/A prevodniku vypina napajeni serva
            bitClear(EXPORT_pole[9],0);                           // napajeni serva vypnute
            servo_pwr = false;               
            if (superfast == true and jas_infra > 10)        zmena = 10;
            else                                             zmena = 1;
            if (jas_infra > 0) jas_infra = jas_infra - zmena ;
            writeToDAC(1, jas_infra);
          }

        if (polozka_menu == 25)                                   // nastaveni laseru (OFF)
          {          
             zobraz_text(9);                                      // " oFF"
             laser(0);                                            // laser zhasnout
          }
      }
  }
//========================================================================================



//========================================================================================
// po prikazu na prohozeni HW kanalu se aktualiuje DAC a znova se zmeri proudy na prehozenych FBI pinech
void aktualizace_HW_kanalu(void)
  {
    writeToDAC(0, jas_bila);        
    writeToDAC(1, jas_infra);        
  }
//========================================================================================



//========================================================================================
void testovaci_funkce()
  {
    char kod_testu = '?';
    Serial.println("START TESTU");
    while (kod_testu != 'x')                                      // kodem 'x' se test ukoncuje
      {
        if (Serial.available())
          {
            delay(10);
            kod_testu = Serial.read();
            
            while (Serial.available()) Serial.read();

            // ---------------- test D/A prevodniku ---
            if (kod_testu == 'a')
              {
                Serial.println("Test D/A prevodniku");
                while (Serial.available()) Serial.read();
                for (uint16_t i = 0; i < 1024 ; i = i + 10)
                  {
                    if (Serial.available()) 
                      {
                        kod_testu = Serial.read();
                        while (Serial.available()) Serial.read();
                        break;
                      }
                    writeToDAC(0, i);
                    writeToDAC(1, 1023-i);
                    Serial.print(i);
                    Serial.print(';');
                    Serial.print((EXPORT_pole[5] << 8) + EXPORT_pole[6]);
                    Serial.print(';');
                    Serial.print(1023-i);
                    Serial.print(';');
                    Serial.println((EXPORT_pole[7] << 8) + EXPORT_pole[8]);
                  }
                    writeToDAC(0, 0);                             // zhasnout bilou LED
                    writeToDAC(1, 0);                             // zhasnout infra LED


                kod_testu = '?';
              }        
            // ---------------- konec testu D/A prevodniku ---


            // ---------------- test serva ---
            if (kod_testu == 'b')
              {
                digitalWrite(pin_servo_PWR, LOW);
                servo_pwr = false;
                Serial.println("napajeni serva vypnute na 5 sekund");
                for (byte i = 0 ; i < 5 ; i++)
                  {
                    Serial.println(5-i);
                    delay(1000);
                  }

                Serial.println("napajeni serva zapnute, prejezd na 0 stupnu");
                digitalWrite(pin_servo_PWR, HIGH);
                servo_pwr = true;
                servo_uhel(0);
                Serial.println("pauza 5 sekund");
                for (byte i = 0 ; i < 5 ; i++)
                  {
                    Serial.println(5-i);
                    delay(1000);
                  }

                Serial.println("prejezd na 180 stupnu");
                servo_uhel(180);
                Serial.println("pauza 5 sekund");
                for (byte i = 0 ; i < 5 ; i++)
                  {
                    Serial.println(5-i);
                    delay(1000);
                  }

                Serial.println("prejezd na 90 stupnu");
                servo_uhel(90);
                Serial.println("pauza 5 sekund");
                for (byte i = 0 ; i < 5 ; i++)
                  {
                    Serial.println(5-i);
                    delay(1000);
                  }

                
                Serial.println("napajeni serva vypnute");
                digitalWrite(pin_servo_PWR, LOW);
                bitClear(EXPORT_pole[9],0);                       // napajeni serva vypnute                
                servo_pwr = false;
                kod_testu = '?';
              }
            // ---------------- konec testu serva ---


            // ---------------- test kalibracnich konstant ---
            if (kod_testu == 'c')
              {
                byte test_index;
                Serial.println("Test kalibracnich konstant (parametry 1 az 15; 0=konec)");
                
                for (test_index = 1 ; test_index < 16 ; test_index++)
                  {
                    Serial.print(" obsah RAM:   ");
                    Serial.print(txt_urovne[test_index]);
                    Serial.print("   DAC0 = ");
                    Serial.print  ((proudove_pole[(test_index*4) + 0] << 8) + (proudove_pole[(test_index*4) + 1]));
                    Serial.print("   DAC1 = ");
                    Serial.println((proudove_pole[(test_index*4) + 2] << 8) + (proudove_pole[(test_index*4) + 3]));

                    Serial.print(" obsah EEPROM:");
                    Serial.print(txt_urovne[test_index]);
                    Serial.print("   DAC0 = ");
                    Serial.print  ((EEPROM.read((test_index*4) + 0) << 8) + (EEPROM.read((test_index*4) + 1)));
                    Serial.print("   DAC1 = ");
                    Serial.println((EEPROM.read((test_index*4) + 2) << 8) + (EEPROM.read((test_index*4) + 3)));                 
                  }
                
                test_index = 99;
                while (test_index != '0')                         // zadanim cisla 0 (nebo cislem vetsim nez 15) se test ukoncuje
                  {
                    if (Serial.available()) 
                      {
                        test_index = Serial.parseInt();
                        while (Serial.available()) Serial.read();
                        if (test_index > 15)
                          {
                            test_index = 0;
                            Serial.println("konec testu");
                          }
                        else
                          {
                            Serial.println(txt_urovne[test_index]);
                            writeToDAC(0, (proudove_pole[(test_index*4) + 0] << 8) + (proudove_pole[(test_index*4) + 1]));
                            writeToDAC(1, (proudove_pole[(test_index*4) + 2] << 8) + (proudove_pole[(test_index*4) + 3]));
                          }
                      }
                    delay(100);
                  }
                kod_testu = '?';
              }
            // ---------------- konec testu kalibracnich konstant ---


            // ---------------- test displeje ---
            if (kod_testu == 'd')
              {
                Serial.println("Test displeje");
                for (byte i = 0 ; i < 18 ; i++)
                  {
                    zobraz_text(i);
                    delay(500);
                  }

                zobraz_text(5);                                   // zhasnout displej
                
                kod_testu = '?';
              }
            // ---------------- konec testu displeje ---


            // ---------------- test optozavory (digitalni dignal) ---
            if (kod_testu == 'e')
              {
                Serial.println("Test optozavory - digital");
                while (kod_testu != 'x')                          // kodem 'x' se test ukoncuje
                  {
                    if (Serial.available()) 
                      {
                        kod_testu = Serial.read();
                        while (Serial.available()) Serial.read();
                      }
                    Serial.print("PA6 = ");
                    Serial.println(digitalRead(pin_opto_kolektor));
                    delay(100);
                  }
                kod_testu = '?';
              }
            // ---------------- konec testu optozavory (digitalni) ---


            // ---------------- test optozavory (analogovy signal) ---
            if (kod_testu == 'f')
              {
                Serial.println("Test optozavory - analog");
                pinMode(pin_opto_kolektor,INPUT);
                while (kod_testu != 'x')                          // kodem 'x' se test ukoncuje
                  {
                    if (Serial.available()) 
                      {
                        kod_testu = Serial.read();
                        while (Serial.available()) Serial.read();
                      }
                    Serial.print("PA6 = ");
                    Serial.println(analogRead(pin_opto_kolektor));
                    delay(100);
                  }
                pinMode(pin_opto_kolektor,INPUT_PULLUP);
                kod_testu = '?';
              }
            // ---------------- konec testu optozavory (analogova) ---


            // ---------------- test reference ---
            if (kod_testu == 'g')
              {
                Serial.println("Test reference");
                while (kod_testu != 'x')                          // kodem 'x' se test ukoncuje
                  {
                    if (Serial.available()) 
                      {
                        kod_testu = Serial.read();
                        while (Serial.available()) Serial.read();
                      }
                    Serial.print("PB1 = ");
                    Serial.println(analogRead(pin_ref_U));
                    delay(100);
                  }
                kod_testu = '?';
              }
            // ---------------- konec testu reference ---


            // ---------------- test 12V napajeni ---
            if (kod_testu == 'h')
              {
                Serial.println("Test mereni 12V");
                while (kod_testu != 'x')                          // kodem 'x' se test ukoncuje
                  {
                    if (Serial.available()) 
                      {
                        kod_testu = Serial.read();
                        while (Serial.available()) Serial.read();
                      }
                    Serial.print("PB0 = ");
                    Serial.println(analogRead(pin_test_12V));
                    delay(100);
                  }
                kod_testu = '?';
              }
            // ---------------- konec testu 12V napajeni ---


            // ---------------- test mereni proudu ---
            if (kod_testu == 'i')
              {
                Serial.println("Test mereni proudu");
                while (kod_testu != 'x')                          // kodem 'x' se test ukoncuje
                  {
                    if (Serial.available()) 
                      {
                        kod_testu = Serial.read();
                        while (Serial.available()) Serial.read();
                      }
                    Serial.print("PA2 = ");
                    Serial.print(analogRead(pin_FB_I_1));
                    Serial.print("     PA3 = ");
                    Serial.println(analogRead(pin_FB_I_2));
                    delay(100);
                  }
                kod_testu = '?';
              }
            // ---------------- konec testu mereni proudu ---

          
            // ---------------- test joysticku ---
            if (kod_testu == 'j')
              {
                Serial.println("Test joysticku");
                while (kod_testu != 'x')                          // kodem 'x' se test ukoncuje
                  {
                    if (Serial.available()) 
                      {
                        kod_testu = Serial.read();
                        while (Serial.available()) Serial.read();
                      }
                    Serial.print("X (PA0) = ");
                    Serial.print(analogRead(pin_joy_X));
                    Serial.print("     Y (PA1) = ");
                    Serial.print(analogRead(pin_joy_Y));
                    Serial.print("     T (PA4) = ");
                    Serial.println(analogRead(pin_joy_BUT));
                    delay(100);
                  }
                kod_testu = '?';
              }
            // ---------------- konec testu joysticku ---


            // ---------------- i2c sken ---
            if (kod_testu == 'k')
              {
                i2c_sken();                
                kod_testu = '?';
              }
            // ---------------- konec I2C skenu ---


            // ---------------- test laseru ---
            if (kod_testu == 'l')
              {
                Serial.println("Test laseru");
                while (kod_testu != 'x')                          // kodem 'x' se test ukoncuje
                  {
                    if (Serial.available()) 
                      {
                        kod_testu = Serial.read();
                        while (Serial.available()) Serial.read();
                      }
                    digitalWrite(pin_laser_1,HIGH);
                    digitalWrite(pin_laser_2,LOW);
                    delay(500);
                    digitalWrite(pin_laser_2,HIGH);
                    digitalWrite(pin_laser_1,LOW);
                    delay(500);
                  }
                digitalWrite(pin_laser_2,LOW);
                digitalWrite(pin_laser_1,LOW);
                kod_testu = '?';
              }
            // ---------------- konec testu laseru ---


            // ---------------- vypis EEPROM ---
            if (kod_testu == 'm')
              {
                Serial.println("Test laseru");
                vypis_EEPROM();
                kod_testu = '?';
              }
            // ---------------- konec vypisu EEPROM ---


          }


        

        if (kod_testu == '?')                                     // napoveda
          {
            Serial.println("==================");
            Serial.println(" a ... DAC");
            Serial.println(" b ... servo");
            Serial.println(" c ... test kalibracnich konstant");
            Serial.println(" d ... displej");
            Serial.println(" e ... digitalni optozavora");
            Serial.println(" f ... analogova optozavora");
            Serial.println(" g ... reference");
            Serial.println(" h ... napeti 12V");
            Serial.println(" i ... proudy");
            Serial.println(" j ... joystick");
            Serial.println(" k ... sken i2c");
            Serial.println(" l ... lasery");
            Serial.println(" m ... vypis EEPROM");
            Serial.println(" x ... konec testu");
            Serial.println("==================");
            kod_testu = '$';
          }
      }
    Serial.println("Konec testovacich funkci");
  }
//========================================================================================



//========================================================================================
//  sken i2c sbernice (hledani D/A prevodniku s adresou 0x60)
void i2c_sken(void)
  {
    Serial.println("Sken I2C");
    Serial.print('|');
    for (byte i = 1; i < 127; i++)
      {
        if (i % 3 == 0)    Serial.print('-');
      }
    Serial.println('|');
    Serial.print('|');
    
    bool zarizeni[128];
    for (byte i = 1; i < 127; i++)
      {
        Wire_DAC.beginTransmission (i);
        if (Wire_DAC.endTransmission () == 0)
          {
            zarizeni[i] = true;
          }
        else
          {
            zarizeni[i] = false;
            Wire_DAC.begin();
            delay(100);
          }
        if (i % 3 == 0)    Serial.print('#');
      }
    Serial.println('|');

    for (byte i = 1; i < 127; i++)
      {
        if (zarizeni[i] == true)
          {
            Serial.print ("Nalezena adresa: ");
            if (i<10)  Serial.print(' ');
            if (i<100) Serial.print(' ');
            Serial.print (i, DEC);
            Serial.print (" (0x");
            Serial.print (i, HEX);
            
            Serial.print (") ");
            if (i == 0x60) Serial.print(" - DAC");                // DAC by mel byt nalezen na adrese 0x60
            Serial.println (' ');
          }
      }    
    Serial.println("|------------------------------------------|");

  }
//========================================================================================


//========================================================================================
// rychly "reset" (uvedeni kalibratoru do stavu, v jakem je po resetu - zhasnute LED, prvni polozka menu na displeji ...)
//         Neprobiha rucni "PUSH" otocneho drzaku. Na displeji se zobrazuje aktualni uhel, servo je vypnute.
void rSEt(void)
  {
    jas_bila = 0;                            // zhasnout LED
    jas_infra = 0;
    polozka_menu = 20;                       // menu na prvni polozku (uhel)
    prvni_krok_y = true;                     // krokovani
    superfast = false;                       // pomale krokovani
    
    funkce_displeje = 1;                     // displej trvale rozsviceny
    
    laser_stav = false;                      // laser zhasnout
    bitClear(EXPORT_pole[9],1);
    
    writeToDAC(0, jas_bila);                 // zhasnuti bile LED
    writeToDAC(1, jas_infra);                // zhasnuti infra LED

    zobraz_text(15);                         // "rSEt"
    delay(1000);

    zobraz_text(20);                         // "uHEL"
    delay(1000);

    zobraz_cislo(uhel-90 , 0);               // aktualni poloha na displej (zobrazovany uhel je -90 az 90 stupnu, vnitrne ale pocita v rozsahu 0 az 180 stupnu)    
  }


//========================================================================================
//  nastaveni defaultnich digitu do DAC pro jednotlive proudove urovne
void defaultni_proudy(void)
  {
    // pred zapisem novych hodnot se jeste zobrazi tabulka tech puvodnich
    Serial.println("Puvodni hodnoty v EEPROM (pred prepsanim)");
    vypis_EEPROM();

    // prednastavene kalibracni hodnoty pro prvni kanal (defaultne bilou LED)
    zapis_default(0,0,0);
    zapis_default(0,1,60);      // 0.5 mA
    zapis_default(0,2,104);     //  1  mA
    zapis_default(0,3,147);     // 1.5 mA
    zapis_default(0,4,191);     //  2  mA
    zapis_default(0,5,234);     // 2.5 mA
    zapis_default(0,6,278);     //  3  mA
    zapis_default(0,7,321);     // 3.5 mA
    zapis_default(0,8,365);     //  4  mA
    zapis_default(0,9,408);     // 4.5 mA
    zapis_default(0,10,452);    //  5  mA
    zapis_default(0,11,539);    //  6  mA
    zapis_default(0,12,626);    //  7  mA
    zapis_default(0,13,713);    //  8  mA
    zapis_default(0,14,800);    //  9  mA
    zapis_default(0,15,887);    // 10  mA

    // prednastavene kalibracni hodnoty pro druhy kanal (defaultne infra LED)
    zapis_default(1,0,0);
    zapis_default(1,1,59);      // 0.5 mA
    zapis_default(1,2,103);     //  1  mA
    zapis_default(1,3,147);     // 1.5 mA
    zapis_default(1,4,192);     //  2  mA
    zapis_default(1,5,234);     // 2.5 mA
    zapis_default(1,6,278);     //  3  mA
    zapis_default(1,7,322);     // 3.5 mA
    zapis_default(1,8,366);     //  4  mA
    zapis_default(1,9,409);     // 4.5 mA
    zapis_default(1,10,453);    //  5  mA
    zapis_default(1,11,540);    //  6  mA
    zapis_default(1,12,627);    //  7  mA
    zapis_default(1,13,714);    //  8  mA
    zapis_default(1,14,802);    //  9  mA
    zapis_default(1,15,889);    // 10  mA

    EEPROM.commit();

    Serial.println("\n\nNove hodnoty v EEPROM (po prepsani)");
    vypis_EEPROM();    
  }
//========================================================================================



//========================================================================================
// vypis obsahu EEPROM s kalibrovanymi hodnotami DAC pro jednotlive proudove urovne
void vypis_EEPROM(void)
  {
    for (byte i = 0; i < 16 ; i ++)
      {
        Serial.print("uroven ");
        Serial.print(txt_urovne[i]);
        
        Serial.print("     EEPROM adresa: ");
        if ((i*4) < 10) Serial.print(0);
        Serial.print(i*4);
        Serial.print("     kanal 1 = ");
        uint16_t rd_eeprom = (EEPROM.read(i*4) << 8) + EEPROM.read((i*4) + 1);
        if (rd_eeprom < 1000)  Serial.print(0);
        if (rd_eeprom < 100)   Serial.print(0);
        if (rd_eeprom < 10)    Serial.print(0);
        
        Serial.print(rd_eeprom);
        Serial.print("     kanal 2 = ");
        rd_eeprom = (EEPROM.read((i*4) + 2) << 8) + EEPROM.read((i*4) + 3);
        if (rd_eeprom < 1000)  Serial.print(0);
        if (rd_eeprom < 100)   Serial.print(0);
        if (rd_eeprom < 10)    Serial.print(0);
        Serial.println(rd_eeprom);        
      }    
  }
//========================================================================================



//========================================================================================
// zapis jedne kalibracni hodnoty rozlozene na 2 bajty do EEPROM
//    POZOR! "EEPROM.commit();" se provadi az na zaver po priprave celeho bloku v podprogramu 'defaultni_proudy()'
void zapis_default(byte defkanal, byte defindex, unsigned int defdata)
   {
     byte adrindex = (defindex * 4) + (defkanal *2);
     proudove_pole[adrindex]     = defdata >>   8;
     proudove_pole[adrindex + 1] = defdata & 0xFF;

     EEPROM.write(adrindex     , defdata >>   8);
     EEPROM.write(adrindex + 1 , defdata & 0xFF);
   }
//========================================================================================




void serkom(void)
  {
    uint8_t prikaz = Serial.parseInt();
    uint16_t parametr = Serial.parseInt();


    if (prikaz == 10)            servo_uhel(parametr);        // pomale najeti serva na pozadovany uhel

    if (prikaz == 20)                                         // napajeni serva
      {
        if (parametr == 0)
          {
            servo_pwr = false;
            digitalWrite(pin_servo_PWR,LOW);                  // vypnuti napajeni serva
            bitClear(EXPORT_pole[9],0);                       // napajeni serva vypnute
          }
        if (parametr == 1)
          { 
            servo_pwr = true;
            digitalWrite(pin_servo_PWR,HIGH);                 // zapnuti napajeni serva
            bitSet(EXPORT_pole[9],0);                         // napajeni serva zapnute         
            if (uhel != pamet_pozadavku)                      // kdyz pri vypnutem napajeni doslo k nejakemu pozadavku na zmenu uhlu natoceni, provede se hned po zapnuti napajeni serva
              {
                servo_uhel(pamet_pozadavku); 
              }
          }
      }

    if (prikaz == 30)            writeToDAC(0 , parametr);    // nastaveni bile LED

    if (prikaz == 40)            writeToDAC(1 , parametr);    // nastaveni infra LED       

    if (prikaz == 50)                                         // ovladani displeje
      {
        if (parametr == 100)     funkce_displeje = 0;         // displej nebude zobrazovat uhly
        if (parametr == 101)     funkce_displeje = 1;         // displej bude zobrazovat uhly trvale
        if (parametr == 102)     funkce_displeje = 2;         // uhly na displeji jen kratce bliknou a pak displej zhasne
        if (parametr < 20)       zobraz_text(parametr);       // zobrazeni prednastaveneho textu s indexem 0 az 19
      }

    if (prikaz == 60)            laser(parametr);             // nastaveni laseru

    if (prikaz == 70)            aktualizuj_A_D();            // jen nacte aktualni hodnoty mereni proudu na analogovych vstupech do exportniho pole

    if (prikaz == 80)            rSEt();                      // neco jako reset (zhasnuti LED a laseru, nastaveni displeje na zobrazeni uhlu)

    if (prikaz == 100)                                        // vypis aktualniho pole do seriove linky
      {
        for (byte i = 0 ; i < 10 ; i++)
          {
            Serial.print("EXPORT_pole[");
            Serial.print(i);
            Serial.print("] = ");
            if (i != 9)           Serial.println(EXPORT_pole[i]);   // pole [0] az [8] se vypisuje normalne v desitkove soustave
            else
              {
                Serial.print("0b");                                 // pole [9] se vypisuje binarne, aby byly videt jednotlive bity
                Serial.println(EXPORT_pole[9],BIN);
              }
          }
      }

    if (prikaz == 110)                                        // vypis jedne urovne pole proudovych konstant do seriove linky
      {
        Serial.print("proudova uroven: ");
        Serial.print(parametr);
        Serial.print("   bila= ");
        Serial.print((proudove_pole[(parametr*4) + 0] << 8) + proudove_pole[(parametr*4) + 1]);
        Serial.print("   infra= ");
        Serial.println((proudove_pole[(parametr*4) + 2] << 8) + proudove_pole[(parametr*4) + 3]);
        Serial.print("EEPROM adresa:   ");
        Serial.print(parametr*4);
        Serial.print("   bila= ");
        uint16_t rd_eeprom = (EEPROM.read(parametr*4) << 8) + EEPROM.read((parametr*4) + 1);
        Serial.print(rd_eeprom);
        Serial.print("   infra= ");
        rd_eeprom = (EEPROM.read((parametr*4) + 2) << 8) + EEPROM.read((parametr*4) + 3);
        Serial.println(rd_eeprom);
      }

 

    if (prikaz == 111)                                        // zapis jedne urovne pole proudovych konstant pro BILOU LED (prvni kanal) do EEPROM
      {
        uint16_t proudova_konstanta = Serial.parseInt(); 
        proudove_pole[ parametr*4]        =  proudova_konstanta >> 8 ;                      // MSB BILA
        proudove_pole[(parametr*4) + 1]   =  proudova_konstanta & 0xFF ;                    // LSB BILA
        
        EEPROM.write(  parametr*4        ,   proudova_konstanta  >>  8 ); 
        EEPROM.write( (parametr*4) + 1   ,   proudova_konstanta  & 0xFF); 
        EEPROM.commit();            

        Serial.print("bila :  ");
        Serial.print(txt_urovne[parametr]);
        Serial.print("    ");
        Serial.print(EEPROM.read(parametr*4)<<8 | EEPROM.read((parametr*4) +1));
        Serial.println(" [dig]");
        
      
      }

    if (prikaz == 112)                                        // zapis jedne urovne pole proudovych konstant pro INFRA LED (druhy kanal) do EEPROM
      {
        uint16_t proudova_konstanta = Serial.parseInt(); 
        proudove_pole[ (parametr*4) + 2]   =  proudova_konstanta >> 8 ;                     // MSB INFRA
        proudove_pole[ (parametr*4) + 3]   =  proudova_konstanta & 0xFF ;                   // LSB INFRA
        
        EEPROM.write( (parametr*4)  + 2   ,   proudova_konstanta  >>  8 ); 
        EEPROM.write( (parametr*4)  + 3   ,   proudova_konstanta  & 0xFF); 
        EEPROM.commit();            

        Serial.print("infra : ");
        Serial.print(txt_urovne[parametr]);
        Serial.print("    ");
        Serial.print(EEPROM.read((parametr*4)+ 2)<<8 | EEPROM.read((parametr*4) +3));
        Serial.println(" [dig]");
      
      }


    if (prikaz == 254)
      {
        Serial.println(verze_prg);                            // kod 254 zobrazi verzi programu (nezavisle na parametru)
        if (prohodit_kanaly == 0)
          {
            Serial.println("Defaultni prirazeni kanalu (prvni = bila, druhy = infra)");
          }
        else
          {
            Serial.println("POZOR! Prohozene kanaly (prvni = infra, druhy = bila)");
          }
          
      }

    if (prikaz == 253)          testovaci_funkce();           // kod 253 Test HW


    if (prikaz == 252)                                        // HW prohozeni bileho a infra kanalu
      {
        if (parametr == 0)
          {
            if (prohodit_kanaly == 1)
              {
                EEPROM.write(100,0);                          // pripadna zmena nastaveni se uklada do virtualni EEPROM (FLASH)
                EEPROM.commit();                
              }
            prohodit_kanaly = 0;
            aktualizace_HW_kanalu();
          }
        if (parametr == 1)
          { 
            if (prohodit_kanaly == 0)
              {
                EEPROM.write(100,1);                          // pripadna zmena nastaveni se uklada do virtualni EEPROM (FLASH)
                EEPROM.commit();                
              }
            prohodit_kanaly = 1;
            aktualizace_HW_kanalu();
          }
      }


    if (prikaz == 250 and parametr == 9999)                   // defaultni nastaveni kalibracnich proudovych urovni pro obe LED
      {
        defaultni_proudy();
      }
         
    while (Serial.available()) Serial.read();                 // smazat zbytky pripadne komunikace z bufferu (CR, LF ...)
    
  }
   
