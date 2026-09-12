// Rozsirujici deska k SQM (SQM-GPS-5 nebo 6) pro zpracovani GPS signalu
//=========================================================================
//  https://sqm.astromik.org
// ----------------------------------


char verzeSW[] = "2026-09-12..INT";                        // 15 znaku popisu verze SW
//  POZOR!
//       V Arduino IDE nastavit hardware (soubor boards.txt) na 4MHz krystal
//       Po nahrani prelozeneho programu pak upravit FUSE:
//                                                       LowFuse   : 0xDC
//                                                       HighFuse  : 0xDE
//                                                       ExtendFuse: 0xF7

// Strucny popis programu
//
// Poloha se zjistuje z GPS modulu (NEO-6M,  ATGM336H ...) pres seriovou linku (Tx, Rx).
// Zpracovavaji se zpravy (vety) GxRMC, GxGGA a GxGSA.
// Druha strana komunikace probiha pres I2C s hlavni SQM deskou.
// Pomoci I2C komunikace je mozne stahnout data (aktualni i zprumerovane souradnice, cas a dalsi informace o GPS modulu).
// Take je mozne pres I2C ovladat LED - docasne vsechny zhasnout (funkce 20) a pak obnovit do puvodniho stavu (funkce 21),
//   nebo provest HOT restart GPS modulu (funkce 22). Funkce 23 byla doplnena pro otestovani vyprseni 4-sekundoveho watchdogu (WDT).
//
// Pri sepnuti servisniho pinu behem zapnuti napajeni se nastavi GPS modul na prijem zprav GxRMC, GxGGA a GxGSA (ostatni zpravy se vypnou).
//   Nastavi se i rychlost odesilani NMEA zprav na 1 sekundu.
//   Do EEPROM se ulozi i typ prijimaciho modulu ve zkracene (15-znakove) verzi. Napriklad "UBX-G60xx000400"
// Pri beznem provozu se spojenim servisniho pinu na GND rozblikaji vsechny LED frekvenci 1Hz (strida 50%)
//   Po 5 sekundach spojeni dochazi k prepinani faze blikani LED1. Nektere GPS moduly odesilaji NMEA zpravy v obracenem poradi a to zpusobuje, 
//     ze LED1 blika s vyrazne delsim rozsvicenym stavem. Po prepnuti faze se pak usetri trochu energie a snizi celkove vyzarovani svetla z SQM.
//     Prepnuti faze je signalizovano rychlym zablikanim LED1.
// 3 signalizacni LEDky informuji o stavu GPS modulu:
//   LED1 - prepina se podle prave zpracovavane vety (GxRMC nesviti / GxGGA sviti) -  blikani je spravny stav, modul NEO-6M vysila obe zpravy
//   LED2 - stav prijmu datumu a casu (cas neni k dispozici - sviti / cas je k dispozici - zhasnuto)
//   LED3 - dostupnost souradnic 
//                     sviti = souradnice jeste nejsou k dispozici, neni zafixovano ani na jeden satelit
//                     blika = je zafixovano alespon na 1 satelit, ale jeste nejsou souradnice, nebo pocet prijatych souradnic nestaci na prumerovani (mene nez 10)
//                     nesviti = vsechno je v poradku, souradnice jsou k dispozici (pocet OK souradnic je 10 a vice) 
//   Spravny stav je tedy takovy, ze blika LED1 a ostatni LED jsou zhasnute.
//   V pripade, ze blika LED1 a LED2 je zhasnuta, dojde pri mereni jasu automaticky k serizeni casu v RTC na hlavni desce SQM-BAS. 

// Data se do SQM odesilaji pomoci 32 8-bitovych registru 
// priklad exportnich dat:
//  0 ---  8        prumer LAT (pro severni polokouli zvetseny o 90 stupnu)
//  1 ---  79
//  2 --- 192
//  3 --- 172

//  4 ---  11       prumer LON (pro vychodni polokouli zvetseny o 180 stupnu)
//  5 --- 149
//  6 --- 204
//  7 --- 147

//  8 --- 100       OK cnt (pocet bezchybnych zafixovanych dat v rade)

//  9 ---  21       YY     (datum a cas z GPS - v GMT zone)
// 10 ---   1       MM
// 11 ---   1       DD
// 12 ---  11       HH
// 13 ---  49       NN
// 14 ---  22       SS

// 15 ---   1       Nadmorska vyska zvysena o 500m
// 16 --- 241

// 17 ---  58       10x PDoP (Position Dilution of Precision) ... mensi cislo = vetsi presnost
// 18 ---  23       10x HDoP (Horizontal Dilution of Precision) ... mensi cislo = vetsi presnost
// 19 ---  83       10x VDoP (Vertical Dilution of Precision) ... mensi cislo = vetsi presnost
// 20 ---  50       rychlost 0 az 130 km/h v logaritmicke stupnici (nizke rychlosti maji vetsi rozliseni, vysoke rychlosti jsou ulozene nepresne)
// 21 ---   6       status bajt:
//                      bity 0 az 3 .... pocet satelitu
//                      bit 4 = '0' pole klouzaku jeste neni zaplneno
//                      bit 4 = '1' pole klouzaku zaplneno, muzou se stahovat data
//                      bit 5 = '0' cas se do RTC nezapise (v GPS nejsou casove znacky)
//                      bit 5 = '1' pri stazeni dat se do RTC ulozi cas z GPS (pokud bude k dispozci)
//                      bity 6 a 7  kvalita (spolehlivost) souradnic
//                                  "00" ....  OK    data se zapisuji do GPX souboru a pocitaji se z nich statistiky
//                                  "01" .... WAR    data se do GPX souboru zapisuji, ale oznacuji se znackou varovani a nepocitaji se do statistik
//                                  "10" .... BAD    data se do GPX souboru nezapisuji
//                                  "11" .... OFF    filtrovani je vypnute, vsechny zaznamy se zapisuji jako v predchozich verzich programu
//
// 22 ---  8        aktualni (posledni) LAT bez prumerovani (pro severni polokouli zvetseny o 90 stupnu)
// 23 ---  79
// 24 --- 192
// 25 --- 172
// 26 ---  11       aktualni (posledni) LON bez prumerovani (pro vychodni polokouli zvetseny o 180 stupnu)
// 27 --- 149
// 28 --- 204
// 29 --- 147
// 30 ---   1       aktualni (posledni) nadmorska vyska zvysena o 500m
// 31 --- 241
//
//

//   Servisni data v EXPORT_pole[] - jen pro vyvoj filtru pro rozpoznavani ztraty fixu v pripade potreby stahovat prikazem 11
// 32 ---  typ fixu (1=nofix, 2=2D fix, 3=3D fix)
// 33 ---  100x Speed over Ground prevedene na [m/s] MSB
// 34 ---                                            LSB
//
// 35 ---  100x rychlost podle souradnic v [m/s] MSB
// 36 ---                                        LSB
//
// 37 ---  rozdil nadmorskych vysek v po sobe jdoucich zpravach [m]
//
// 38 ---  bitova znacka ktera udava, ktery filtr kvality souradnic byl aktivovany
//
//
//   Na zvlastni pozadavek odeslany pres I2C sbernici (prikazy 50 az 55, 60 az 65 a 90 az 95) je mozne si vyzadat i kompletni
//           obsah poslednich NMEA vet GxRMC, GxGGA a GxGSA. Vety se zpatky do SQM odesilaji po 15 znacich.
//   Pri pozadavku s kodem 70 se vrati 15-znakovy text s verzi programu (yyyy-mm-dd..INT)
//   Pri pozadavku s kodem 80 se vrati 15-znakovy text s HW verzi prijimaci desky
//   Pozadavek s kodem 200 vypina experimentalni funkci pro filtrovani nestabilnich souradnic
//   Pozadavek s kodem 201 experimentalni funkci pro filtrovani nestabilnich souradnic zase zapne (defaultne zapnuta)
//   Debugovaci pozadavek 11 vypise informace o filtrovani souradnic pred ztratou fixu
//======================================================================================================================


#define F_CPU 4000000UL

#include <Wire.h>                                          // knihovna pro I2C komunikaci
#include <EEPROM.h>                                        // knihovna pro vnitrni EEPROM
#include <avr/wdt.h>                                       // knihovna pro watchdog


#define pin_LED1            2
#define pin_LED2            3
#define pin_LED3            4

#define pin_setup           6                              // pri prizemneni behem resetu se do GPS modulu odesle prikaz pro povoleni GxGGA, GxRMC a GxGSA vety a toto nastaveni se ulozi do vnitrni EEPROM GPS modulu
                                                           // pri prizemneni za chodu se meni strida blikanio spodni LED1

#define pin_dta_rdy        A1                              // signalizace pro SQM, ze je modul GPS zapnuty

#define I2C_ADDR_GPS     0x17                              // I2C adresa pridavneho modulu pro GPS

// do retezce 'veta' se budou zapisovat vsechna prijata data z GPS
char veta[]     = "                                                                                            @\0";

//                 $GPRMC,110109.00,A,4926.67723,N,01421.99214,E,0.521,,311220,,,A*72
char veta_RMC[] = "                                                                                            @\0";

//                 $GPGGA,103515.00,4926.65079,N,01421.99995,E,1,05,4.92,499.9,M,44.3,M,,0000*5C
char veta_GGA[] = "                                                                                            @\0";

//                 $GNGSA,A,3,65,67,80,81,82,88,66,08,09,10,11,12,1.2,0.7,1.0*20
char veta_GSA[] = "                                                                                            @\0";


//   POZOR! Nektere prijimace, ktere kombinuji prijem vice druzicovych systemu (GPS, GLONASS, BEIDOU), misto "GPRMC" a "GPGGA" vraci retezce "GNRMC" a "GNGGA"
//                Treti znak ve zprave je tedy nutne pri zpracovani zprav ignorovat


char rozlozeny_pole[18][28];                               // vety se sem do toho pole rozkladaji na jednotlive polozky (puvodne byly ve vete oddelene carkami)

byte    ukazatel_klouzaku;
long    klouzak_LAT[10];                                   // pole pro prumerovani zjistenych souradnic (znamenkove)
long    klouzak_LON[10];
float   klouzak_ALT[10];
boolean pripraveno = false;                                // az bude zaznamenany alespon 1 blok klouzaku (10 vzorku), tak se da pocitat prumer 

byte volatile prikaz_I2C;                                  // z SQM (MASTER) se da do modulu (SLAVE) poslat prikaz:
                                                           //   0 = bez prikazu (predchozi prikaz byl vykonan)
                                                           //  10 = zadost o zpacovana data
                                                           //  20 = zhasnout docasne vsechny LED (pri mereni jasu)
                                                           //  21 = obnovit puvodni svit LED
                                                           // 50 az 55 = stahnout 15 bajtu dlouhy blok textu ze zpravy GxRMC (dohromady az 90 znaku)
                                                           // 60 az 65 = stahnout 15 bajtu dlouhy blok textu ze zpravy GxGGA (dohromady az 90 znaku)
                                                           // 70       = stahnout 15 bajtu dlouhy blok textu s verzi programu
                                                           // 80       = stahnout 15 bajtu dlouhy blok textu s verzi HW prijimace NEO
                                                           // 90 az 95 = stahnout 15 bajtu dlouhy blok textu ze zpravy GxGSA (dohromady az 90 znaku)
                                                           

boolean  stav_LED_1;                                       // kvuli zhasinani a obnovovavni LED pre I2C prikaz se musi zapamatovat aktualni stav LED
boolean  stav_LED_2;
boolean  stav_LED_3;

boolean  pamet_LED_1;                                      // kvuli zhasinani a obnovovavni LED pre I2C prikaz se musi zapamatovat stav LED pred zhasnutim
boolean  pamet_LED_2;
boolean  pamet_LED_3;

boolean ignoruj_vetu;                                      // kdyz se prijme veta, o kterou nemam zajem

byte OK_byte;                                              // pocitadlo nepretrzitych prijatych dat


byte CRC_end;                                              // na ktere pozici ve vete se objevila '*' (pouzito pro vypocet CRC)
byte ukazatel;
byte EXPORT_pole[40];                                      // sem se ukladaji zpracovana data z GPS a odsud se pak preposilaji pres I2C

char rd;

byte aktivace_dta_rdy = 0;                                 // "pin_dta_rdy" se prepina do HIGH az kdyz se podari alespon 1x nacist z modulu GPS obe NMEA vety (GGA i RMC)

unsigned long starttime;
char typ_vety;                                             // aktualni typ prijate vety ('M' , 'G' nebo 'S')
char ocekavana_veta = 'M';                                 // zacina se s prijmem vety GxRMC


byte veta1[]     = { 'G', 'G', 'A'};                       // prvni povolena veta je GxGGA
byte veta2[]     = { 'R', 'M', 'C'};                       // druha povolena veta je GxRMC
byte veta3[]     = { 'G', 'S', 'A'};                       // treti povolena veta je GxGSA

byte prijate_CRC [] = { 'A', 'B'};                         // posledni dva znaky za hvezdickou z kazde vety se ukladaji sem a pak se porovnavaji s vypoctenym CRC

char EW_polokoule[10] ;                                    // reseni problemu s prumerovanim delky podel 180. poledniku (Tichy ocean) 
                                                           //  V tech mistech se muze stridave prijimat zemepisna delka +180 a -180 stupnu. To se neda zprumerovat


byte statusy_LED;                                          // promenna, do ktere se prubezne uklada pozadovany stav 3 LED (bit0=LED1; bit1=LED2; bit2 = LED3; 
                                                           //                                                              bit5=blikani LED3 nadrazene nad bit2 )
boolean zhasnuto;                                          // pri docasnem zhasnuti blokuje dalsi rozsveceni LED

boolean blik3;                                             // pouzito pro blikani 3. LED

byte faze_LED1;                                            // protoze neni mozne urcit poradi zprav GGA, RMC a GSA. Muze spodni LED blikat s velkou stridou 
                                                           //  zmenou teto promenne je mozne v servisnim nastaveni zvolit takovy rezim blikani, ktery rozsviti LED jen kratce
unsigned long cas_prvniho_cyklu;                           // pro podprogram na zmenu faze LED1 (5-sekundovy odpocet drzeni servisniho pinu v LOW)
boolean prvni_cyklus;

byte pocet_satelitu;


float PDoP_f;
float HDoP_f;
float VDoP_f;

float Pfloat_f;
float Hfloat_f;
float Vfloat_f;

float Pfloat;
float Hfloat;
float Vfloat;


// pro analyzu signalu mezi nasledujicimi zpravami
uint16_t  predchozi_geoalt;                                // nadmorska vyska v predchozi zprave pro vypocet vyskoveho skoku pri filtrovani souradnic
uint16_t  aktual_geoalt;                                   // posledni zjistena nadmorska vyska

int32_t   predchozi_LAT;                                   // aktualni a predchozi souradnice pro urceni nerealnych skoku pri ztrate signalu (prevedeno na miliontiny stupne)
int32_t   predchozi_LON;
int32_t   aktualni_LAT;
int32_t   aktualni_LON;
float     speed_lat_lon;                                   // vypoctena rychlost na zaklade vzdalenosti mezi souradnicemi ze sousednich zprav a casu prechozich souradnic
uint32_t  aktualni_cas_rych;                               // cas ziskani aktualnich souradnic (rozdil aktualniho casu a casu poslednich souradnic urcuje rychlost skoku)
uint32_t  predchozi_cas_rych;                              // cas ziskani predchozich souradnic (rozdil aktualniho casu a casu poslednich souradnic urcuje rychlost skoku)
bool      pocitej_rychlost = false;                        // po ztrate fixu se vzdalenost nepocita, protoze by dochazelo k deleni nulou

uint8_t typ_fixu;
uint8_t predchozi_typ_fixu;

float SoG;                                                 // Speed over Ground ze zpravy RMC (v uzlech)

uint8_t filtr_ON_OFF;                                      // moznost ovladat fitrovani souradnic pri ztrate signalu


unsigned long blik500;                                     // casovani 500ms pro servisni blikani vsech LED
boolean stav500;                                           // pro prepinani stavu LED pri servisnim blikani

// prikazy pro GPS modul (UBX protokol)
                                     //   sync  sync class   id   delka LiEn   class   id   rate  CK_A  CK_B
const PROGMEM byte zapni_GxGGA[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x00, 0x01, 0xFB, 0x10};
const PROGMEM byte zapni_GxRMC[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x04, 0x01, 0xFF, 0x18};
const PROGMEM byte zapni_GxGSA[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x02, 0x01, 0xFD, 0x14};

const PROGMEM byte vypni_GxGLL[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x01, 0x00, 0xFB, 0x11};
const PROGMEM byte vypni_GxGSV[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x03, 0x00, 0xFD, 0x15};
const PROGMEM byte vypni_GxVTG[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x05, 0x00, 0xFF, 0x19};
const PROGMEM byte vypni_GxGRS[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x06, 0x00, 0x00, 0x1B};
const PROGMEM byte vypni_GxGST[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x07, 0x00, 0x01, 0x1D};
const PROGMEM byte vypni_GxZDA[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x08, 0x00, 0x02, 0x1F};
const PROGMEM byte vypni_GxGBS[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x09, 0x00, 0x03, 0x21};
const PROGMEM byte vypni_GxDTM[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x0A, 0x00, 0x04, 0x23};
const PROGMEM byte vypni_GxGNS[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x0D, 0x00, 0x07, 0x29};
const PROGMEM byte vypni_GxVLW[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x0F, 0x00, 0x09, 0x2D};
const PROGMEM byte vypni_GPTXT[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x41, 0x00, 0x3B, 0x91};
const PROGMEM byte vypni_PUBX1[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF1, 0x00, 0x00, 0xFB, 0x12};
const PROGMEM byte vypni_PUBX2[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF1, 0x03, 0x00, 0xFE, 0x18};
const PROGMEM byte vypni_PUBX3[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF1, 0x04, 0x00, 0xFF, 0x1A};
const PROGMEM byte vypni_XXXX1[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x0E, 0x00, 0x08, 0x2B};
const PROGMEM byte vypni_XXXX2[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF1, 0x01, 0x00, 0xFC, 0x14};
const PROGMEM byte vypni_XXXX3[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF1, 0x05, 0x00, 0x00, 0x1C};
const PROGMEM byte vypni_XXXX4[]      = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF1, 0x06, 0x00, 0x01, 0x1E};
//const PROGMEM byte vypni_GxGSA[]    = { 0xB5, 0x62, 0x06, 0x01, 0x03, 0x00,   0xF0, 0x02, 0x00, 0xFC, 0x13};
const PROGMEM byte jeden_Hz[]         = { 0xB5, 0x62, 0x06, 0x08, 0x06, 0x00,   0xE8, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x39, 0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30};
const PROGMEM byte SAVE_EEPROM[]      = { 0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00,   0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x31, 0xBF};
const PROGMEM byte hot_start[]        = { 0xB5, 0x62, 0x06, 0x04, 0x04, 0x00,   0x00, 0x00, 0x02, 0x00, 0x10, 0x68};

char verzeHW[] = "               ";                     // 15 znaku popisu verze (uklada se do EEPROM pri zakladnim konfiguraci desky a pak se pri kazdem startu nacita z EEPROM)


//======================================================================================================================



//======================================================================================================================
void setup(void)
  {
    MCUSR = 0;                                                                                   // vymazat vsechny flagy resetu (vcetne WDRF)
    wdt_disable();
    UBRR0H = 0;                                                                                  // doladeni rychlosti seriove linky na 9600bps pro 4MHz krystal (kat. list ATmega328 str. 201)
    UBRR0L = 25;



    Wire.begin(I2C_ADDR_GPS);                                                                    // rozsirujici deska bude pres I2C komunikovat na adrese 0x17
    Wire.onRequest(requestEvent); 
    Wire.onReceive(receiveEvent);
    Serial.begin(9600);                                                                          // rychlost komunikace s GPS modulem
    
    pinMode(pin_setup ,INPUT_PULLUP);                                                            // jumper pro zakladni nastaveni GPS modulu

    pinMode(pin_LED1  ,OUTPUT);                                                                  // LEDKY
    pinMode(pin_LED2  ,OUTPUT);
    pinMode(pin_LED3  ,OUTPUT);

    pinMode(pin_dta_rdy, OUTPUT);
    digitalWrite(pin_dta_rdy, LOW);                                                              // po spusteni "pin_dta_rdy" zustane jeste chvili v LOW

    rd = EEPROM.read(100);                                                                       // pokud EEPROM jeste neobsahuje typ HW, tak se 3 sekundy bude pokouset o jeho vycteni ze seriove linky
    if (rd == '?' or rd > 127 or rd < 33)
      {
        LED3(true);
        zjisti_typ_HW();                                                                         // maximalne 3 sekundy po zapnuti se bude snazit v prijimanych zpravach objevit retezec "HW " nebo "IC="
        LED3(false);
      }

    faze_LED1 = EEPROM.read(0);                                                                  // promenna pro zjisteni ulozene faze blikani LED1
    if (faze_LED1 > 3)
      {
        EEPROM.write(0,2);
        faze_LED1 = 2;                                                                           // pro nejbeznejsi prijimace NEO6 vyvola nejkratsi bliknuti hodnota 'faze_LED1' = 2, proto se nastavi defaultne 
      }

    filtr_ON_OFF = EEPROM.read(50);                                                              // podle hodnoty v EEPROM je mozne vypnout filtrovani souradnic pro ztrate signalu
    if (filtr_ON_OFF != 0 and filtr_ON_OFF != 1)                                                 // po prvnim zapnuti napajeni se filtr defaultne aktivuje (EEPROM[50] <- 1)
      {
        EEPROM.write(50,1);
        delay(100);
        filtr_ON_OFF = EEPROM.read(50);
      }


    if (digitalRead(pin_setup) == LOW)                                                           // kdyz je pro zapnuti prizemneny pin_setup, dojde k prenastaveni NMEA vet z GPS modulu
      {
        for (byte i = 1 ; i < 6 ; i++)                                                           // 5x bliknuti frekvenci 1Hz kvuli kontrole spravne rychlosti krystalu (4MHz)
          {
            LED1(false);                                                                         // strida 50%
            LED2(false);
            LED3(false);
            delay(500);
            LED1(true);                                                                          
            LED2(true);
            LED3(true);
            delay(500);            
          }

        if (digitalRead(pin_setup) == LOW)                                                       // kdyz je i po 5 sekundach sepnuty kontakt, nastavi se modul NEO-6M
          {
            GPS_setup();                                                                         // povoleni GxGGA, GxGSA a GxRMC vet. Ostatni zrusit a zapsat do EEPROM.

            while (true)                                                                         // Z teto nekonecne smycky se vypadne jen resetem
              {
                LED1(true);                                                                      // nekonecne rychle blikani (1Hz) vsech LED najednou
                LED2(true);                                                                      // strida 10%
                LED3(true);
                delay(100);
                LED1(false);
                LED2(false);
                LED3(false);
                delay(900);
              }
          }
      }

    bitSet(statusy_LED,0);                                                                       // po zapnuti napajeni se vsechny LED rozsviti (znameni vsech chyb: GPS neodesila vety, signal neobsahuje cas, signal je spatny)
    bitSet(statusy_LED,1); 
    bitSet(statusy_LED,2); 

    
    LED1(true);                                                                                  // rozsviti vsechny LED
    LED2(true);
    LED3(true);
    delay(500);

    if (filtr_ON_OFF == 1)  EXPORT_pole[21] = 0b00000000;                                        // po zapnuti se status registr naplni hodnotami podle toho, jestli je filtrovani zapnute, nebo vypnute
    else                    EXPORT_pole[21] = 0b11000000;
                            //                  ||||^^^^--- pocet satelitu = 0
                            //                  |||^------- neni zaplneno pole pro klouzaky
                            //                  ||^-------- neni dostupny cas
                            //                  ^^--------- kvalita souradnic BAD ('11'=OFF / '01' nebo '10'=WAR / '00'=BAD)

    EXPORT_pole[20] = 0;                                                                         // rychlost (SoG) se pri startu nastavi na 0 km/h (defaultne by byla I2C hodnota 255 a to je pres 130km/h)

    for (byte i = 100; i < 115 ; i++)                                                            // Precteni drive ulozeneho typu HW prijimace NEO
      {
        char znak = EEPROM.read(i);
        if (znak >= 32 and znak <= 127)            verzeHW[i-100] = znak;
        else                                       verzeHW[i-100] = 32;
      }

    // pro novejsi typy modulu NEO8-M se musi provest nastaveni konfigurace pri kazdem zapnuti (nemaji EEPROM)
    setup_neo_8();

    wdt_enable(WDTO_4S);                                                                         // kdyz se program z nejakeho duvodu zasekne na vic nez 4 sekundy, dojde k resetu  
  }
//======================================================================================================================



//======================================================================================================================
void loop(void)
  {
    wdt_reset();                                                                                 // v kazde smycce se resetuje WDT (nesmi se prekrocit 4 sekundy)
    
    ignoruj_vetu = false;
    if(prikaz_I2C == 0)
      {
        if (Serial.available())                                                                  // kdyz se neco objevi na seriove lince z GPS modulu
          {
            CRC_end = 1;
    
            while( rd != '$')   rd = Serial.read();                                              // cekani na zacatek vety
            ukazatel = 0;
            veta[ukazatel] = rd;                                                                 // '$' je vzdycky na zacatku
            starttime = millis();
            while (rd != '\r' and (millis() - starttime) < 700 and ukazatel < 90)                // kdyz uz je znamy zacatek vety, musi se zbytek vety stahnout do 0.7 sekundy nebo do 90 znaku
              {
                if (Serial.available())
                  {
                    rd = Serial.read();
                    if (rd == '*') CRC_end = ukazatel;                                           // znacka, do ktereho znaku se bude pocitat CRC
                    ukazatel++;
                    veta[ukazatel] = rd;                                                         // vsechny prijate znaky se zapisuji do znakoveho pole 'veta[]'
                  }
              }

           // tady v tom miste je poli 'veta[]' nactena cela veta z NEO modulu (nezavisle na typu - treba tam muze byt veta GxVTG)

           ignoruj_vetu = true;
           if (veta[3] == veta1[0] and veta[4] == veta1[1] and veta[5] == veta1[2])              // jedna se o povolenou vetu GxGGA
             {
               ignoruj_vetu = false;                                                             // veta se tedy bude zpracovavat
               typ_vety = 'G';                                                                   //    a jeste znacka, ze je to veta GGA
             }

           if (veta[3] == veta2[0] and veta[4] == veta2[1] and veta[5] == veta2[2])              // jedna se o povolenou vetu GxRMC
             {
               ignoruj_vetu = false;                                                             // veta se tedy bude zpracovavat
               typ_vety = 'M';                                                                   //    a jeste znacka, ze je to veta RMC
             }   

           if (veta[3] ==  veta3[0] and veta[4] == veta3[1] and veta[5] == veta3[2])              // jedna se o povolenou vetu GxGSA
             {
               ignoruj_vetu = false;                                                             // veta se tedy bude zpracovavat
               typ_vety = 'S';                                                                   //    a jeste znacka, ze je to veta GSA
             }   

    
            if (ignoruj_vetu == false)                                                           // kdyz je nactena nejaka povolena veta, tak se zpracuje
              {

                byte blok = 0;                                                                   // rozlozeni vety na jednotlive bloky mezi carkami
                byte pozice = 0;
                byte rad = 0;
                char znak = ' ';
                while (znak != '\r' and pozice < 90 )
                  {
                    znak = veta[pozice];
                    if (typ_vety == 'G') veta_GGA[pozice] = znak;
                    if (typ_vety == 'M') veta_RMC[pozice] = znak;
                    if (typ_vety == 'S') veta_GSA[pozice] = znak;
                    
                    if (znak == ',' or znak == '*')
                      {
                        rozlozeny_pole[blok][rad] = '\0';
                        blok ++;
                        rad = 0;
                      }
                    else
                      {
                        rozlozeny_pole[blok][rad] = znak;
                        rad ++;                
                      }
                    pozice ++;
                  }            


                while (pozice < 90)                                                              // kompletni smazani aktualne zpracovavane vety pred jejim naplnenim novymi daty
                  {
                     if (typ_vety == 'G')   veta_GGA[pozice] = ' ';
                     if (typ_vety == 'M')   veta_RMC[pozice] = ' ';
                     if (typ_vety == 'S')   veta_GSA[pozice] = ' ';
                     pozice ++;
                  }

                switch (typ_vety)
                  {
                    case 'G':
                       if (ocekavana_veta == 'G')  zpracuj_vetu_GxGGA();
                       break;
                    case 'M':
                       if (ocekavana_veta == 'M')  zpracuj_vetu_GxRMC();
                       break;
                    case 'S':
                       if (ocekavana_veta == 'S')  zpracuj_vetu_GxGSA();
                       break;
                  }

                wdt_reset();
    
                byte i;
                for (i = 0 ; i < 90 ; i ++)                                                      // po zpracovani prislusne vety se pole veta[] smaze
                  {
                    veta[i] = ' ';
                  }
                veta[i+1] = '\0';
              }
          }

      }

    if (aktivace_dta_rdy == 0b11)                                                                // Na prijem se testuji jen vety GGA a RMC a obe vety byly alespon 1x prijaty
      {                                                                                          //    Veta GSA s aktivaci pinu neovlivnuje - i kdyby nedorazila, tak se vystup aktivuje
        digitalWrite(pin_dta_rdy, HIGH);                                                         // "pin_dta_rdy" se prepne na HIGH
      }

    obsluha_LED();


  }
//======================================================================================================================



//----------------------------------------------
// Z vety GxRMC se berou souradnice, datum a cas. Taky se tu sleduji vystrahy, pri kterych se nuluje pocet bezchybnych prijmu
void zpracuj_vetu_GxRMC(void)
  {
    aktivace_dta_rdy = aktivace_dta_rdy | 0b10;                                                  // GxRMC veta byla alespon 1x prijata

    byte CRC=0;                                                                                  // vypocet CRC z prijatych dat
    for (byte i = 1 ; i <= CRC_end ; i++)   CRC = CRC xor veta[i];

    prijate_CRC[0] = veta[CRC_end+2];
    prijate_CRC[1] = veta[CRC_end+3];

    if (crc_check(CRC , prijate_CRC[0],prijate_CRC[1]))                                          // kontrolni soucet souhlasi
      {
        if (faze_LED1 == 0) bitClear(statusy_LED,0);                                             // LED1 rozsvit
        if (faze_LED1 == 3) bitClear(statusy_LED,0);                                             // LED1 zhasni

        // rozklad casu na hodiny, minuty a sekundy
        byte GMT_hod = 0;
        byte GMT_min = 0;
        byte GMT_sek = 0;
        GMT_hod =           (rozlozeny_pole[1][0] - 48) * 10;                                // desitky hodin
        GMT_hod = GMT_hod + (rozlozeny_pole[1][1] - 48)     ;                                // jednotky hodin
        GMT_min =           (rozlozeny_pole[1][2] - 48) * 10;                                // desitky minut
        GMT_min = GMT_min + (rozlozeny_pole[1][3] - 48)     ;                                // jednotky minut
        GMT_sek =           (rozlozeny_pole[1][4] - 48) * 10;                                // desitky sekund
        GMT_sek = GMT_sek + (rozlozeny_pole[1][5] - 48)     ;                                // jednotky sekund
                         
        // rozklad datumu na dny, mesice a roky
        byte GMT_den = 0;
        byte GMT_mes = 0;
        byte GMT_rok = 0;
        GMT_den =           (rozlozeny_pole[9][0] - 48) * 10;                                // desitky dni
        GMT_den = GMT_den + (rozlozeny_pole[9][1] - 48)     ;                                // jednotky dni
        GMT_mes =           (rozlozeny_pole[9][2] - 48) * 10;                                // desitky mesicu
        GMT_mes = GMT_mes + (rozlozeny_pole[9][3] - 48)     ;                                // jednotky mesicu
        GMT_rok =           (rozlozeny_pole[9][4] - 48) * 10;                                // desitky roku
        GMT_rok = GMT_rok + (rozlozeny_pole[9][5] - 48)     ;                                // jednotky roku

        if (GMT_rok > 20 and rozlozeny_pole[9][5] != '\0' )                                  // kdyz je rok vetsi, nez 2020, da se predpokladat, ze alespon datum a cas je prijaty v poradku
          {
            EXPORT_pole[9]  = GMT_rok;
            EXPORT_pole[10] = GMT_mes;
            EXPORT_pole[11] = GMT_den;
            EXPORT_pole[12] = GMT_hod;
            EXPORT_pole[13] = GMT_min;
            EXPORT_pole[14] = GMT_sek;
                
            bitClear(statusy_LED,1);                                                         // LED2 zhasnout
    
    
            EXPORT_pole[21] = EXPORT_pole[21] | 0b00100000;                                  // vzdycky zapnout nastaveni RTC podle GPS
          }
        else                                                                                 // prijaty rok neni v poradku
          {
            EXPORT_pole[9]  = 0;
            EXPORT_pole[10] = 0;
            EXPORT_pole[11] = 0;
            EXPORT_pole[12] = 0;
            EXPORT_pole[13] = 0;
            EXPORT_pole[14] = 0;
            bitSet(statusy_LED,1);                                                           // LED2 rozsvitit 
          }
        
        if (rozlozeny_pole[2][0] == 'A')                                                         // znacka, ze je signal v poradku
          {
            // ----------------------
            // prepocet zemepisne sirky
            unsigned long geoLAT = 0;
            unsigned long aktual_geolat = 0;
            unsigned long aktual_geolon = 0;

            unsigned long pomprom_geomin;
            long geolat_sign;                                                                    // pro vypocty a prumerovani se pouzivaji znamenkove souradnice

            geoLAT =          (rozlozeny_pole[3][0] - 48) *    10000000UL;                       // desitky stupnu
            geoLAT = geoLAT + (rozlozeny_pole[3][1] - 48) *     1000000UL;                       // jednotky stupnu
    
            pomprom_geomin =                  (rozlozeny_pole[3][2] - 48)  *     1000000UL;      // desitky minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][3] - 48)  *      100000UL;      // jednotky minut
                                            // rozlozeny_pole[3][4] =  desetinna tecka
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][5] - 48)  *       10000UL;      // desetiny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][6] - 48)  *        1000UL;      // setiny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][7] - 48)  *         100UL;      // tisiciny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][8] - 48)  *          10UL;      // desettisiciny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[3][9] - 48)  *           1UL;      // stotisiciny minut
            pomprom_geomin = pomprom_geomin / 6UL;
    
            geoLAT = geoLAT + pomprom_geomin;                                                    // tady je zemepisna sirka v rozsahu +90 000 000 az 0 a 0 az +90 000 000 miliontin stupnu 
                                                                                                 //  rozliseni SEVER/JIH je znak v poli "rozlozeny_pole[4][0]"
            if (rozlozeny_pole[4][0] == 'N')            geolat_sign = geoLAT;                    // severni polokoule je kladna
            else if (rozlozeny_pole[4][0] == 'S')       geolat_sign = -geoLAT;                   // jizni polokoule je zaporna
            else                                        geolat_sign = 200000000L;                // neni zafixovano, vraci se kod pro poruchu

            if (geolat_sign !=  200000000L )
              {
                aktualni_LAT = geolat_sign;                                                      // pro vypocet vzdalenosti mezi dvema sousednimi body (hodnota se znamenkem)
              }




            if (geolat_sign >= 0) aktual_geolat = geolat_sign + 90000000;                        // pro severni polokouli se k zmerene hodnote pricita +90 stupnu
            else                  aktual_geolat = abs(geolat_sign);                              // pro jizni polokouli se zmerena hodnota jen prevede na kladne cislo
            EXPORT_pole[22] = (aktual_geolat >> 24) & 0xFF;                                      // do I2C pole se tedy ukladaji kladna cisla a SEVER je odlisen tak, ze ma vic nez 90 stupnu
            EXPORT_pole[23] = (aktual_geolat >> 16) & 0xFF;
            EXPORT_pole[24] = (aktual_geolat >>  8) & 0xFF;
            EXPORT_pole[25] = aktual_geolat         & 0xFF;

            klouzak_LAT[ukazatel_klouzaku] = geolat_sign;

            // ----------------------
            // prepocet zemepisne delky
            unsigned long geoLON = 0;
            long geolon_sign = 0;  

            geoLON =          (rozlozeny_pole[5][0] - 48) *    100000000UL;                      // stovky stupnu
            geoLON = geoLON + (rozlozeny_pole[5][1] - 48) *     10000000UL;                      // desitky stupnu
            geoLON = geoLON + (rozlozeny_pole[5][2] - 48) *      1000000UL;                      // jednotky stupnu
    
            pomprom_geomin =                  (rozlozeny_pole[5][3]  - 48)  *     1000000UL;     // desitky minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][4]  - 48)  *      100000UL;     // jednotky minut
                                            // rozlozeny_pole[5][5] =  desetinna tecka
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][6]  - 48)  *       10000UL;     // desetiny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][7]  - 48)  *        1000UL;     // setiny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][8]  - 48)  *         100UL;     // tisiciny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][9]  - 48)  *          10UL;     // desettisiciny minut
            pomprom_geomin = pomprom_geomin + (rozlozeny_pole[5][10] - 48)  *           1UL;     // stotisiciny minut
            pomprom_geomin = pomprom_geomin / 6UL;
    
            geoLON = geoLON + pomprom_geomin;                                                    // tady je zemepisna delka v rozsahu +180 000 000 az 0 a 0 az +180 000 000 miliontin stupnu
                                                                                                 //  rozliseni VYCHOD/ZAPAD je znak v poli "rozlozeny_pole[6][0]"

            if (rozlozeny_pole[6][0] == 'E')            geolon_sign = geoLON;                    // vychodni polokoule je kladna
            else if (rozlozeny_pole[6][0] == 'W')       geolon_sign = -geoLON;                   // zapadni polokoule je zaporna
            else                                        geolon_sign = 200000000L;                // neni zafixovano, vraci se kod pro poruchu

            if (geolon_sign != 200000000L )
              {
                aktualni_cas_rych = millis();                                                    // cas, pro ktery pocita rychlost zmeny souradnic
                aktualni_LON = geolon_sign;                                                      // pro vypocet vzdalenosti mezi dvema sousednimi body (znamenkova promenna)
                pocitej_rychlost = true;
              }
            else                                                                                 // kdyz neni fix, nebude se pocitat rychlost (problem deleni nulou)
              {
                pocitej_rychlost = false;
              }

            
            aktualni_LON = geolon_sign;                                                          // pro vypocet vzdalenosti mezi dvema sousednimi body


            if (geolon_sign >= 0) aktual_geolon = geolon_sign + 180000000;                       // pro vychodni polokouli se k zmerene hodnote pricita +180 stupnu
            else                  aktual_geolon = abs(geolon_sign);                              // pro zapadni polokouli se zmerena hodnota jen prevede na kladne cislo




            EXPORT_pole[26] = (aktual_geolon >> 24) & 0xFF;                                      // do I2C pole se tedy ukladaji kladna cisla a VYCHOD je odlisen tak, ze ma vic nez 180 stupnu
            EXPORT_pole[27] = (aktual_geolon >> 16) & 0xFF;
            EXPORT_pole[28] = (aktual_geolon >>  8) & 0xFF;
            EXPORT_pole[29] = aktual_geolon         & 0xFF;

            SoG = atof(rozlozeny_pole[7]);                                                       // Speed Over Ground v uzlech
            float c = 5.0;
            float kmh_log = 250 * log( (SoG * 1.852 + c) / c ) / log( (130 + c) / c );          // prevod na logaritmickou stupnici, kde nizke rychlosti maji velke bitove rozliseni
            
            if (kmh_log > 255) EXPORT_pole[20] = 255;                                            // pri prekroceni 130 km/h se zapisuje maximalni povolene cislo 
            else               EXPORT_pole[20] = (uint8_t)(round(kmh_log));


            EW_polokoule[ukazatel_klouzaku] = rozlozeny_pole[6][0];                              // kvuli problemum s prumerovanim v okoli +/- 180 stupnu se do extra pole poznamenava i aktualni znamenko polokoule

            klouzak_LON[ukazatel_klouzaku] = geolon_sign;
    
            ukazatel_klouzaku ++;
            if (ukazatel_klouzaku == 10)
              {
                ukazatel_klouzaku = 0;
                pripraveno = true;                    
                EXPORT_pole[21] = EXPORT_pole[21] | 0b00010000;                                  // pole pro klouzaky bylo zaplneno a muze se stahovat
              }
    
            if (pripraveno == true)
              {
                byte poskozene = 0;                                                              // poskozene souradnice (vypadek signalu)
                long suma_LAT = 0;
                long suma_LON = 0;
                long prumer_LAT = 0;
                long prumer_LON = 0;
                
                for (byte i = 0 ; i < 10 ; i ++)
                  {
                    if (klouzak_LAT[i] == 200000000L)                                            // pri poskozenych souradnicich se nic nepricita
                      {
                        poskozene ++;                                                            // vysledek se bude delit o jedna mensim cislem
                      }
                    else
                      {
                        suma_LAT = suma_LAT + klouzak_LAT[i];                                    // klouzak obsahuje normalni souradnice, tak se prictou pro pocitani prumeru                    
                      }                    
                  }             
                prumer_LAT = suma_LAT / (10-poskozene);
                if (prumer_LAT >= 0) prumer_LAT = prumer_LAT + 90000000;                         // pro severni polokouli se k prumeru pricita +90 stupnu
                else                 prumer_LAT = abs(prumer_LAT);                               // pro jizni polokouli se prumer jen prevede na kladne cislo




                // u zemepisne delky se nejdriv zkontroluje, ze jsou vsechny hodnoty ze stejne polokoule (problemy mohou nastat kolem +/- 180 stupnu)
                bool blokuj_prumer_lon = false;                                                  // prumerovani delky je na zacatku povoleno
                if (abs(geolon_sign) > 170000000L and abs(geolon_sign) <= 180000000L)            // kdyz je delka nekde v Tichem oceanu ...
                  {
                    bool vychod = false;
                    bool zapad = false;
                    for (byte i = 0 ; i < 10 ; i ++)
                      {
                        if (EW_polokoule[i] == 'E') vychod = true;                               // pole obsahuje alespon jednu vychodni souradnici
                        if (EW_polokoule[i] == 'W') zapad  = true;                               // pole obsahuje alespon jednu zapadni souradnici
                      }
                    if (vychod == true and zapad == true) blokuj_prumer_lon = true;              // kdyz pole obsauje obe souradnice, prumerovani se zakaze
                  }

                poskozene = 0;
                if (blokuj_prumer_lon == false)
                  {
                    for (byte i = 0 ; i < 10 ; i ++)
                      {
                        if (klouzak_LON[i] == 200000000L)                                        // pri poskozenych souradnicich se nic nepricita
                          {
                            poskozene ++;                                                        // vysledek se bude delit o jedna mensim cislem
                          }
                        else
                          {
                            suma_LON = suma_LON + klouzak_LON[i];                                // klouzak obsahuje normalni souradnice, tak se prictou pro pocitani prumeru                    
                          }
                      }             
                    
                    prumer_LON = suma_LON / (10-poskozene);
                  }
                else                                                                             // kdyz je bezne prumerovani zemepisne delky blokovano ...
                  {
                    for (byte i = 0; i < 10 ; i++)                                               //  ... provede se prumerovani prevodem souradnic o pul otacky niz (kolem nulteho poledniky)
                      {
                        if (klouzak_LON[i] == 200000000L)                                        // poskozene souradnice se ignoruji
                          {
                            poskozene ++;                                                        // vysledek se bude delit o jedna mensim cislem
                          }
                        else
                          {
                            if (klouzak_LON[i] < 0)                                              // klouzak ukazuje na zapadni polokouli (zaporna delka)
                              {
                                suma_LON = suma_LON + (klouzak_LON[i] + 180000000L);             // k delce se pricita 180 stupnu, takze se souradnice dostavaji k nule na kladnou cast osy 
                              }
                            else                                                                 // klouzak ukazuje na vychodni polokouli (kladna delka)
                              {
                                suma_LON = suma_LON + (klouzak_LON[i] - 180000000L);             // od delky se odecita 180 stupnu, takze se souradnice dostavaji k nule na zapornou cast osy 
                              }
                          }
                      }
                    prumer_LON = suma_LON / (10-poskozene);                                     // prumer upravenych souradnic (kolem nulteho poledniku)

                    if (prumer_LON < 0) prumer_LON = prumer_LON + 180000000L;                   // souradnice prumeru se musi vratit zpatky o pul otacky
                    else                prumer_LON = prumer_LON - 180000000L;                  
                  }

                if (prumer_LON >= 0) prumer_LON = prumer_LON + 180000000;                        // pro vychodni polokouli se k prumeru pricita +180 stupnu
                else                 prumer_LON = abs(prumer_LON);                               // pro zapadni polokouli se prumer jen prevede na kladne cislo



                
                EXPORT_pole[0] = (prumer_LAT >> 24) & 0xFF;                                      // do I2C pole se tedy ukladaji kladna cisla a SEVER je odlisen tak, ze ma vic nez 90 stupnu
                EXPORT_pole[1] = (prumer_LAT >> 16) & 0xFF;
                EXPORT_pole[2] = (prumer_LAT >>  8) & 0xFF;
                EXPORT_pole[3] = prumer_LAT         & 0xFF;
                
                EXPORT_pole[4] = (prumer_LON >> 24) & 0xFF;                                      // do I2C pole se tedy ukladaji kladna cisla a VYCHOD je odlisen tak, ze ma vic nez 180 stupnu
                EXPORT_pole[5] = (prumer_LON >> 16) & 0xFF;
                EXPORT_pole[6] = (prumer_LON >>  8) & 0xFF;
                EXPORT_pole[7] = prumer_LON         & 0xFF;
    
                OK_byte ++;
                if (OK_byte > 100) OK_byte = 100; 
                EXPORT_pole[8] = OK_byte ;                                                       // kolik vzorku proslo v rade bez problemu (maximalni hodnota je 100, dal se nepocita)
             

              }
    
            EXPORT_pole[9]  = GMT_rok;
            EXPORT_pole[10] = GMT_mes;
            EXPORT_pole[11] = GMT_den;
            EXPORT_pole[12] = GMT_hod;
            EXPORT_pole[13] = GMT_min;
            EXPORT_pole[14] = GMT_sek;
          }
        else                                                                                     // spatny signal, nebo nejake varovani ve vete
          {
            OK_byte = 0;                                                                         // nulovani poctu bezchybnych prijmu v rade
            
            bitSet(statusy_LED,2);                                                               // LED3 rozsvitit 

            EXPORT_pole[0]  = 255;                                                               // LAT nastavit mimo rozsah (Err)
            EXPORT_pole[1]  = 255;
            EXPORT_pole[2]  = 255;
            EXPORT_pole[3]  = 255;

            EXPORT_pole[4]  = 255;                                                               // LON nastavit mimo rozsah (Err)
            EXPORT_pole[5]  = 255;
            EXPORT_pole[6]  = 255;
            EXPORT_pole[7]  = 255;

            EXPORT_pole[8] = 0;        

          
            EXPORT_pole[15] = 255;                                                               // ALT nastavit mimo rozsah (Err)
            EXPORT_pole[16] = 255;

            EXPORT_pole[17] = 255;                                                               // PDoP  (Err)
            EXPORT_pole[18] = 255;                                                               // HDoP  (Err)
            EXPORT_pole[19] = 255;                                                               // VDoP  (Err)

            EXPORT_pole[20] = 0;                                                                 // rychlost 0 km/h

            
            EXPORT_pole[21] = EXPORT_pole[21] & 0b00100000;                                      // status maze informaci o zaplnenem poli klouzaku a nuluje pocet satelitu

            EXPORT_pole[22]  = 255;                                                              // aktual LAT nastavit mimo rozsah (Err)
            EXPORT_pole[23]  = 255;
            EXPORT_pole[24]  = 255;
            EXPORT_pole[25]  = 255;

            EXPORT_pole[26]  = 255;                                                              // aktual LON nastavit mimo rozsah (Err)
            EXPORT_pole[27]  = 255;
            EXPORT_pole[28]  = 255;
            EXPORT_pole[29]  = 255;
            
            EXPORT_pole[30]  = 255;                                                              // aktual ALT nastavit mimo rozsah (Err)
            EXPORT_pole[31]  = 255;
          }

      }
    else                                                                                         // nesouhlasi CRC
      {
        OK_byte = 0;                                                                             // nulovani poctu bezchybnych prijmu v rade
        EXPORT_pole[8] = 0;
      }

    ocekavana_veta = 'G';                                                                        // priste zpracuj GxGGA vetu

  }
//----------------------------------------------  


//----------------------------------------------
//  v GxGGA vete se zjistuje nadmorska vyska a pocet satelitu
void zpracuj_vetu_GxGGA(void)
  {

    aktual_geoalt = 0;
    aktivace_dta_rdy = aktivace_dta_rdy | 0b01;                                                  // GxGGA veta byla alespon 1x prijata
    byte CRC=0;                                                                                  // vypocet CRC z prijatych dat
    for (byte i = 1 ; i <= CRC_end ; i++)   CRC = CRC xor veta[i];
    prijate_CRC[0] = veta[CRC_end+2];
    prijate_CRC[1] = veta[CRC_end+3];

    if (crc_check(CRC , prijate_CRC[0],prijate_CRC[1]))                                          // kontrolni soucet souhlasi, takze nejake vety se z GPS modulu ziskavaji
      {

        if (faze_LED1 == 0)   bitSet(statusy_LED,0);                                             // LED1 rozsvit
        if (faze_LED1 == 1) bitClear(statusy_LED,0);                                             // LED1 zhasni
        if (faze_LED1 == 2)   bitSet(statusy_LED,0);                                             // LED1 rozsvit
    
        if (rozlozeny_pole[6][0] != '0')                                                         // kdyz je zafixovany signal, provede se zpracovani (GPS quality indicator)
          {
            pocet_satelitu =                  (rozlozeny_pole[7][0] - 48) * 10;                  // desitky satelitu
            pocet_satelitu = pocet_satelitu + (rozlozeny_pole[7][1] - 48)     ;                  // jednotky satelitu

    

            EXPORT_pole[21] = (EXPORT_pole[21] & 0b11110000) | pocet_satelitu;                   // maximalne by jich melo byt 12 (to se vejde do 4 bitu statusoveho registru)
    
    

            klouzak_ALT[ukazatel_klouzaku] = atof(rozlozeny_pole[9]);                            // klouzavy prumer souradnic z 10 vzorku
            aktual_geoalt = 500 + klouzak_ALT[ukazatel_klouzaku];                                // aktualni hodnota se bude ukladat o 500m zvetsena
            EXPORT_pole[30] = aktual_geoalt >>  8 & 0xFF;
            EXPORT_pole[31] = aktual_geoalt       & 0xFF;

            if (pripraveno == true)
              {
                float suma_ALT = 0;
                for (byte i = 0 ; i < 10 ; i ++)
                  {
                    suma_ALT = suma_ALT + klouzak_ALT[i];
                  }
                unsigned int prumer_ALT = 500 +( (suma_ALT + 5) / 10);                           // Vyska se prenasi posunuta o 500m (0 mnm = 500; 380mnm = 880 ; 550mnm = 1050, -200mnm = 300)
                
                EXPORT_pole[15] = prumer_ALT >>  8 & 0xFF;
                EXPORT_pole[16] = prumer_ALT       & 0xFF;
                
                OK_byte ++;

                if (OK_byte > 100) OK_byte = 100; 
                EXPORT_pole[8] = OK_byte ;                                                       // kolik vzorku proslo v rade bez problemu (maximalni hodnota je 100, dal se nepocita)
              }    
          }
        else                                                                                     // neni zafixovano (GPS quality indicator = 0)
          {
            bitSet(statusy_LED,2);                                                               // LED3 rozsvitit 

            EXPORT_pole[0]  = 255;                                                               // LAT nastavit mimo rozsah (Err)
            EXPORT_pole[1]  = 255;
            EXPORT_pole[2]  = 255;
            EXPORT_pole[3]  = 255;

            EXPORT_pole[4]  = 255;                                                               // LON nastavit mimo rozsah (Err)
            EXPORT_pole[5]  = 255;
            EXPORT_pole[6]  = 255;
            EXPORT_pole[7]  = 255;

            EXPORT_pole[8] = 0;                                                                  // OK cnt 
   
            EXPORT_pole[15] = 255;                                                               // ALT nastavit mimo rozsah (Err)
            EXPORT_pole[16] = 255;

            EXPORT_pole[20] = 0;                                                                 // rychlost 0 km/h

            EXPORT_pole[21] = (EXPORT_pole[21] & 0b11110000);                                    // pocet satelitu vynulovat, nejvyssi bity ve status registru zustanou beze zmeny

            EXPORT_pole[17] = 255;                                                               // PDoP (Err)
            EXPORT_pole[18] = 255;                                                               // HDoP (Err)
            EXPORT_pole[19] = 255;                                                               // VDoP (Err)
            
            EXPORT_pole[22]  = 255;                                                              // aktual LAT nastavit mimo rozsah (Err)
            EXPORT_pole[23]  = 255;
            EXPORT_pole[24]  = 255;
            EXPORT_pole[25]  = 255;
            
            EXPORT_pole[26]  = 255;                                                              // aktual LON nastavit mimo rozsah (Err)
            EXPORT_pole[27]  = 255;
            EXPORT_pole[28]  = 255;
            EXPORT_pole[29]  = 255;
            
            EXPORT_pole[30]  = 255;                                                              // aktual ALT nastavit mimo rozsah (Err)
            EXPORT_pole[31]  = 255;           
          }    

      }
    else                                                                                         // neco je spatne (nebyly prijate souradnice nebo nesouhlasi CRC, nebo je ve vete vystrazna znacka)
      {
        OK_byte = 0;                                                                             // nulovani poctu bezchybnych prijmu v rade
        EXPORT_pole[8] = 0;
        bitSet(statusy_LED,2);                                                                   // LED3 rozsvitit 
      }  

    ocekavana_veta = 'S';                                                                        // priste zpracuj GxGSA vetu
  
  }
//----------------------------------------------
      


//----------------------------------------------
//  v GxGSA vete se zjistuje PDoP, HDoP, VDoP a typ fixu (2D / 3D)
void zpracuj_vetu_GxGSA(void)
  {
    byte CRC=0;                                                                                  // vypocet CRC z prijatych dat
    for (byte i = 1 ; i <= CRC_end ; i++)   CRC = CRC xor veta[i];
    prijate_CRC[0] = veta[CRC_end+2];
    prijate_CRC[1] = veta[CRC_end+3];
    if (crc_check(CRC , prijate_CRC[0],prijate_CRC[1]))                                          // kontrolni soucet souhlasi, takze nejake vety se z GPS modulu ziskavaji
      {

        if (faze_LED1 == 1)   bitSet(statusy_LED,0);                                             // LED1 rozsvit
        if (faze_LED1 == 2) bitClear(statusy_LED,0);                                             // LED1 zhasni
        if (faze_LED1 == 3)   bitSet(statusy_LED,0);                                             // LED1 rozsvit

        typ_fixu = rozlozeny_pole[2][0] - '0';                                                   // znacka typu fixu 1 / 2D / 3D
        EXPORT_pole[32] = typ_fixu;
        
        if (pocet_satelitu >= 4)                                                                 // pri 4 satelitech uz by se mela objevit nejaka realna xDoP data
          {
            if (OK_byte >= 10) bitClear(statusy_LED,5);                                          // dostatek satelitu a alespon 10 souradnic v rade maze blikaci bit pro 3. LED

            Pfloat = atof(rozlozeny_pole[15]);                                                   // float presne toho, co se ziska z NMEA zpravy (bez uprav s nejvetsim rozlisenim)
            PDoP_f= (Pfloat + 0.05) * 10;                                                        // priprava pro ukladani do EXPORT_pole[17]
            if (PDoP_f > 255) PDoP_f = 255;                                                      // omezeni na maximalne 25.5
            EXPORT_pole[17] = (uint8_t)PDoP_f;                                                   //  export jako 1 bajt
    
    
            Hfloat = atof(rozlozeny_pole[16]);                                                   // float presne toho, co se ziska z NMEA zpravy (bez uprav s nejvetsim rozlisenim)
            HDoP_f= (Hfloat + 0.05) * 10;                                                        // priprava pro ukladani do EXPORT_pole[18]
            if (HDoP_f > 255) HDoP_f = 255;
            EXPORT_pole[18] = (uint8_t)HDoP_f;
    
                    
            Vfloat = atof(rozlozeny_pole[17]);                                                   // float presne toho, co se ziska z NMEA zpravy (bez uprav s nejvetsim rozlisenim)
            VDoP_f= (Vfloat + 0.05) * 10;                                                        // priprava pro ukladani do EXPORT_pole[19]
            if (VDoP_f > 255) VDoP_f = 255;
            EXPORT_pole[19] = (uint8_t)VDoP_f;
            
          }
        else                                                                                    // maly pocet satelitu prepisuje xDoP ze zprav na "spatne" udaje 25.5
          {
            EXPORT_pole[17] = 255;
            EXPORT_pole[18] = 255;
            EXPORT_pole[19] = 255;
            if (pocet_satelitu == 0)                                                             // vubec zadny satelit (napriklad po zapnuti napajeni)
              {
                bitClear(statusy_LED,5);                                                         // signal neni vubec (pocet satelitu = 0, takze blikani je zrusene (LED trvale sviti)
              }
            else                                                                                 // signal zafixovany alespon na 1 az 3 satelity, ale jeste neni dostatek zaznamu pro klouzak - blikani aktivni
              {
                if (OK_byte < 10)                                                                    
                  {
                    bitSet(statusy_LED,5);                                                       // doplneni blikani o kratky "predblik"                                                 
                  }
                else                                                                             // uz je dostek zaznamu pro klouzak - blikani vypnute (LED trvale zhasnuta)
                  {
                    bitClear(statusy_LED,5);
                  }
              }
          }


          
        if (OK_byte > 10)                                                                        //alespon 10 vzorku bez chyb
          {
            bitClear(statusy_LED,2);                                                             // LED3 zhasnout
          }
        else
          {
            bitSet(statusy_LED,2);                                                               // LED3 rozsvitit 
          }



//******************************************************
        // Analyza pro urceni kvality souradnic.
        //    Do teto casti vstupuji nasledujici promenne: 
        //      xDoP_f                         .... aktualni VDoP, PDoP a HDoP zaokrouhlene na desetiny a vynasobene x10 (hodnota 27 tedy znamena neco mezi 2.65 az 2.74 v NMEA zpravach)
        //      predchozi_xDoP                 .... stejny format jako xDoP_f, akorat z prechoziho cyklu (z predchozi GSA zpravy)
        //      OK_byte                        .... pocet bezchybnych mereni v rade
        //      pocet_saelitu                  .... pocet zafixovanych satelitu
        //      typ_fixu                       .... kvalita zafixovani 2D/3D
        //      predchozi_typ_fixu             .... kvalita zafixovani 2D/3D z predchozi zpravy
        //      SoG                            .... Speed over Ground v uzlech
        //      aktualni_LAT, aktualni_LON     .... aktualni (posledni) souradnice pro vypocet vzdalenosti mezi poslednimi dvema zpravami (v miliontinach stupne)
        //      predchozi_LAT, predchozi_LON   .... predchozi souradnice pro vypocet vzdalenosti mezi poslednimi dvema zpravami (v miliontinach stupne)
        //
        //    Vystupem je hodnoceni ve dvou nejvyssich bitech registru 'EXPORT_pole[21]'     ('00xxxxxx'=OK ; '11xxxxxx'=OFF ; '01xxxxxx'=WAR ; '10xxxxxx'=BAD)
        //      Na signalizacni LED toto hodnoceni nema vliv.

        float rychlost_coord;
        float dist_m;
        float rychlost_SOG;
       
        EXPORT_pole[38] = 0;                                                                    // servisni znacka, ktery filtr byl aktivovan se na zacatku vynuluje

        //------------------------------------------------------
        // FILTR 1
        // Porovnani rychlosti (hodnota SoG - Speed Over Groud) proti vypoctene rychlosti mezi sousednimi souradnicemi.
        // Kdyz je vypoctena rychlost vetsi nez ____ m/s a zaroven pomer vypoctene rychlosti proti SoG [uzly] vetsi nez ____, nahazuje se znacka "BAD".

        if (pocitej_rychlost == true)
          {
            dist_m = vzdalenost_m (aktualni_LAT / 1000000.0 , aktualni_LON / 1000000.0, predchozi_LAT / 1000000.0 , predchozi_LON / 1000000.0);
            rychlost_coord = dist_m / ((aktualni_cas_rych - predchozi_cas_rych) / 1000.0);  // rychlost ze zmeny souradnic v [m/s]
            rychlost_SOG   = SoG  * 1.852 / 3.6;                                            // rychlost ze SoG  v [m/s]

            EXPORT_pole[33] = (uint16_t)(rychlost_SOG * 100) >> 8;                          // export do servisniho pole ve 100x presnejsi rozliseni
            EXPORT_pole[34] = (uint16_t)(rychlost_SOG * 100) % 256;
            EXPORT_pole[35] = (uint16_t)(rychlost_coord * 100) >> 8;
            EXPORT_pole[36] = (uint16_t)(rychlost_coord * 100) % 256;

                
            if (rychlost_coord > 1)                                                         // filtr se pouziva jen pro vetsi rychlosti (nestabilni souradnice)
              {
                if (abs(rychlost_coord - rychlost_SOG) > 5)                                 // rozdil mezi SoG a rychlosti podle zmeny souradnic je nezvykle vysoky
                  {
                    bitSet(EXPORT_pole[38],7);                                              // servisni znacka, ktery filtr byl aktivovan 
                  }
              }

            predchozi_cas_rych = aktualni_cas_rych;                                         // cas poslednch souradnic se zapamatuje pro pristi cyklus
            predchozi_LAT = aktualni_LAT;
            predchozi_LON = aktualni_LON;
          }
        else
          {
            EXPORT_pole[33] = 0;
            EXPORT_pole[34] = 0;
            EXPORT_pole[35] = 0;
            EXPORT_pole[36] = 0;
          }

        //------------------------------------------------------
        // FILTR 2
        // Typ fixace 2D/3D z GxGSA zpravy
        // 2D fix nahodi znacku "WAR".
        // Uplna ztrata fixu znamena "BAD".

        EXPORT_pole[32] = typ_fixu;
        if (typ_fixu == 2)
          {
            bitSet(EXPORT_pole[38],6);                                                      // servisni znacka, ktery filtr byl aktivovan 
          }
        if (typ_fixu == 1)
          {
            bitSet(EXPORT_pole[38],5);                                                      // servisni znacka, ktery filtr byl aktivovan 
          }



        //------------------------------------------------------
        // FILTR 3
        // Rychla zmena nadmorske vysky
        // Kdyz je rozdil nadmorske vysky mezi dvema zpravami vetsi nez ___ metry za sekundu, nahodi se znacka "varovani"

        uint16_t rozdil_vysek = (predchozi_geoalt > aktual_geoalt) ? (predchozi_geoalt - aktual_geoalt) : (aktual_geoalt - predchozi_geoalt); 
        if (rozdil_vysek > 255) rozdil_vysek = 255;
        EXPORT_pole[37] = rozdil_vysek;
        if (rozdil_vysek > 4)
          {
             bitSet(EXPORT_pole[38],4);                                                     // servisni znacka, ktery filtr byl aktivovan 
          }
        predchozi_geoalt = aktual_geoalt;


        //------------------------------------------------------
        // FILTR 4
        //  4 nebo mene satelitu a zaroven HDoP nad 5.0 (prenasenych 50) = BAD

        if (HDoP_f > 50 and pocet_satelitu <= 4)
          {
            bitSet(EXPORT_pole[38],3);                                                      // servisni znacka, ktery filtr byl aktivovan 
          }

        //------------------------------------------------------
        // FILTR 5
        //  velmi vysoke VDoP, HDop, nebo PDoP = BAD

        if (VDoP_f > 100 or HDoP_f > 100 or PDoP_f > 100)                                   // kterakoliv hodnota xDoP prekrocila 10.0
          {
            bitSet(EXPORT_pole[38],2);                                                      // servisni znacka, ktery filtr byl aktivovan 
          }


        //------------------------------------------------------
        // vyhodnoceni filtru WAR/BAD/OK do EXPORT_pole[21]
        const byte maska_varovani =                    // "WAR "
            (1 << 4) |    // velky rozdil sousednich nadmorskych vysek
            (1 << 6) ;    // zhorseny fix (2D)
        
        const byte maska_chyba =                       // "BAD "
            (1 << 2) |    // nektery z parametru xDoP je moc velky (pres 10.0)
            (1 << 3) |    // malo satelitu a zaroven vysoke HDoP
            (1 << 5) |    // uplna ztrata fixu
            (1 << 7);     // velky rozdil rychlosti mezi SoG a rychlosti mezi dvema sousednimi souradnicemi

        
        //------------------------------------------------------
        if (filtr_ON_OFF == 1)                                                                  // kdyz je filtrovani souradnic povolene ...
          {           
            EXPORT_pole[21] = EXPORT_pole[21] & 0b00111111;                                     // na zacatku vzdycky stav OK nejvyssi bity na   "0b00......."
            if (EXPORT_pole[38] & maska_varovani)
              {
                bitSet(EXPORT_pole[21],6);                                                      // varovani (WAR) nastavi se nejvyssi bity na    "0b01......."
              }
            if (EXPORT_pole[38] & maska_chyba)                                                  // chyba (BAD) nastavi nejvyssi bity na          "0b10......."
              {
                bitSet(EXPORT_pole[21],7);
                bitClear(EXPORT_pole[21],6);
              }
          }                                                                                     // kdyz je filtrovani vypnute, jen se nastavi nejvyssi bity status registru na 0b11......
        else                                                                                    // filtrovani vypnute (filtr_ON_OFF = 0)
          {
            EXPORT_pole[21] = EXPORT_pole[21] | 0b11000000;                                     // nejvyssi 2 bity status registru v '1' znamenaji vypnute filtry
          }


        predchozi_typ_fixu = typ_fixu ;                                                         // puvodne slouzilo k detekci zmeny typu fixu, nakonec nepouzito
        
//******************************************************  konec analyzy kvality souradnic
        
      }
    ocekavana_veta = 'M';                                                                        // priste zpracuj GxRMC vetu
  }
//----------------------------------------------


//----------------------------------------------
// porovnani prijateho hexadecimalniho kodu na konci radky s vypoctem kodem
// 'CRC_bajt' je hodnota, ktera se pocita z cele prijate vety (od znaku s indexem 1 az k hvezdicce)
// 'HigHalf'  je prvni znak za hvezdickou v prijate vete
// 'LoHalf'   je druhy znak za hvezdickou v prijate vete
boolean crc_check(byte CRC_bajt, char HigHalf, char LoHalf)
  {
    byte CRC_pul = CRC_bajt >> 4;                                                                // horni pulka bajtu
    char pomprom;
    if (CRC_pul > 9)  pomprom = 65 + CRC_pul - 10;
    else              pomprom = 48 + CRC_pul;
    if (pomprom !=  HigHalf) return false;                                                       // porovnava se s prvnim prijatym HEXA znakem

    CRC_pul = CRC_bajt & 0b00001111;                                                             // spodni pulka
    if (CRC_pul > 9)  pomprom = 65 + CRC_pul - 10;
    else              pomprom = 48 + CRC_pul;
    if (pomprom !=  LoHalf) return false;                                                        // porovnava se s druhym prijatym HEXA znakem
    else                    return true;
  }
//----------------------------------------------



//----------------------------------------------
void LED1(boolean stav)                                                                          // rozsvecovani a zhasinani LED
  { 
    stav_LED_1 = stav;
    if (stav == true)   digitalWrite(pin_LED1, HIGH);
    else                digitalWrite(pin_LED1, LOW);
  }
//----------------------------------------------



//----------------------------------------------
void LED2(boolean stav)
  { 
    stav_LED_2 = stav;
    if (stav == true)   digitalWrite(pin_LED2, HIGH);
    else                digitalWrite(pin_LED2, LOW);
  }
//----------------------------------------------



//----------------------------------------------
void LED3(boolean stav)
  { 
    stav_LED_3 = stav;
    if (stav == true)   digitalWrite(pin_LED3, HIGH);
    else                digitalWrite(pin_LED3, LOW);
  }
//----------------------------------------------



//----------------------------------------------
void obsluha_LED(void)
  {
    while (digitalRead(pin_setup) == LOW)                                                        // servisni funkce, ktera by mela kdykoliv rozblikat vsechny LED frekvenci 1Hz
      {
        wdt_reset();                                                                             // i po dobu drzeni servisniho pinu se musi obcerstvovat WDT
        if ((millis() % 1000) < 500)                                                             // pravidelne blikani vsech LED s periodou 1s
          {
            digitalWrite(pin_LED1, LOW);
            digitalWrite(pin_LED2, LOW);
            digitalWrite(pin_LED3, LOW);            
          }
        else
          {
            digitalWrite(pin_LED1, HIGH);
            digitalWrite(pin_LED2, HIGH);
            digitalWrite(pin_LED3, HIGH);                        
          }
    
        if ((millis() - cas_prvniho_cyklu) > 5000)                                               // po 5 sekundach spojeni servisniho pinu do LOW se memi faze LED1
          {
            faze_LED1 ++;
            if (faze_LED1 == 4) faze_LED1 = 0;

            EEPROM.write(0,faze_LED1);

            for (byte i = 0; i < 10 ; i++)                                                       // rychle zablikani LEDl jako informace o zmene stavu
              {
                 digitalWrite(pin_LED1, LOW);
                 delay(50);
                 digitalWrite(pin_LED1, HIGH);
                 delay(50);
              }
            cas_prvniho_cyklu = millis();                                                        // dalsi prepnuti stavu je mozne az po 5 sekundach
          }
      }
   
    cas_prvniho_cyklu = millis();                                                                //  pri rozepnutem servisnim pinu se zaznamenava cas, ze ktereho se pak urcuje doba trvani sepnuti pinu 
    
    if (zhasnuto == false)                                                                       // obsluha LED je povolena
      {
        LED1(bitRead(statusy_LED,0));        
        LED2(bitRead(statusy_LED,1));        

        if (bitRead(statusy_LED,5) == 0)                                                         // bit5 obsahuje informaci o blikani 3. LED
          {
            LED3(bitRead(statusy_LED,2));                                                        // kdyz blikat nema, tak je trvale rozsvicena, nebo zhasnuta podle stavu bitu 2
          }
        else                                                                                     // kdyz blikat ma, prepina se s globalni promennou 'blik3'
          {
            if (blik3 == true)
              {
                digitalWrite(pin_LED3, LOW);
                blik3 = false;            
              }
            else
              {
                digitalWrite(pin_LED3, HIGH);
                blik3 = true;                            
              }
              
          }
      }
  
  }
//----------------------------------------------



//----------------------------------------------
// Prisla zadost o data pres I2C, podle kodu prikazu se odeslou pozadovane bloky dat (POZOR! buffer I2C zvlada maximalne 32 Bajtu)
void requestEvent(void)
  {
    if (prikaz_I2C >= 50 and prikaz_I2C < 60)                                                    // pro prikazy 50 az 55 se odesilaji 15 znaku dlouhe casti vety GxRMC
      {
        for (byte snd = (prikaz_I2C-50) * 15 ; snd < ((prikaz_I2C-50) * 15)+15 ; snd ++)
          {
            Wire.write(veta_RMC[snd]);
          }
      }

    if (prikaz_I2C >= 60 and prikaz_I2C < 70)                                                    // pro prikazy 60 az 65 se odesilaji 15 znaku dlouhe casti vety GxGGA
      {
        for (byte snd = (prikaz_I2C-60) * 15 ; snd < ((prikaz_I2C-60) * 15)+15 ; snd ++)
          {
            Wire.write(veta_GGA[snd]);
          }
      }

    if (prikaz_I2C >= 90 and prikaz_I2C < 100)                                                   // pro prikazy 90 az 95 se odesilaji 15 znaku dlouhe casti vety GxGSA
      {
        for (byte snd = (prikaz_I2C-90) * 15 ; snd < ((prikaz_I2C-90) * 15)+15 ; snd ++)
          {
            Wire.write(veta_GSA[snd]);
          }
      }


    if (prikaz_I2C == 70)                                                                        // pro prikaz 70 vrati v textovem formatu verzi programu ("yyyy-mm-dd..INT")
      {
        for (byte snd = 0 ; snd < 15 ; snd ++)
          {
            Wire.write(verzeSW[snd]);
          }
      }


    if (prikaz_I2C == 80)                                                                        // pro prikaz 80 vrati v textovem formatu verzi HW ("UBX-G60xx000400")
      {
        for (byte snd = 0 ; snd < 15 ; snd ++)
          {
            Wire.write(verzeHW[snd]);
          }
      }
    
    
    if (prikaz_I2C == 10)                                                                        // prikaz 10 odesle 32 bajtu velke pole zpracovanych dat
      {
         Wire.write(EXPORT_pole, 32);
      }

    if (prikaz_I2C == 11)                                                                        // prikaz 11 odesle servisni data od registru 32
      {
         Wire.write(EXPORT_pole + 32, 10);
      }

    prikaz_I2C = 0;                                                                              // az do dalsiho kodu prikazu se prikaz vypne

  }
//----------------------------------------------



//----------------------------------------------
// zjisteni kodu prikazu ktery prisel pres I2C
void receiveEvent(int prijato)
  {
    prikaz_I2C = prijato;                                                                        // uplne k nicemu, protoze na dalsich radkach se 'prikaz_I2C' stejne hned prepise. 
                                                                                                 //    zabrani ale kompilatoru ve varovani pro nepouzity parametr 'prijato'
    if (Wire.available())
      {
        prikaz_I2C = Wire.read();         
      }
    else
      {
        prikaz_I2C = 0;
      }

    if (prikaz_I2C == 20)                                                                        // zhasnuti vsech LED pred merenim jasu
      {
        pamet_LED_1 = stav_LED_1;
        pamet_LED_2 = stav_LED_2;
        pamet_LED_3 = stav_LED_3;
        LED1(false);
        LED2(false);
        LED3(false);
        zhasnuto = true;
        prikaz_I2C = 0;
      }

    if (prikaz_I2C == 21)                                                                        // obnoveni stavu vsech LED po mereni jasu
      {
        LED1(pamet_LED_1);
        LED2(pamet_LED_2);
        LED3(pamet_LED_3);
        zhasnuto = false;
        prikaz_I2C = 0;
      }

    if (prikaz_I2C == 22)                                                                        // HOT restart GPS modulu
      {
        LED1(false);
        LED2(false);
        LED3(false);
        wdt_reset();
        delay(1500);
        LED1(true);
        LED2(true);
        LED3(true);
        Serial.write(hot_start , 12);
        wdt_reset();
        delay(1500);
        wdt_reset();
      }

    if (prikaz_I2C == 23)                                                                        // zamerne vyprseni 4-sekundoveho WTD
      {
        wdt_reset();
        LED1(false);
        LED2(true);
        LED3(false);
        while(true);
      }


    if (prikaz_I2C == 200)                                                                        // prikaz 200 vypne filtovani zapisem cisla 0 na adresu 50 v EEPROM
      {
         EEPROM.write(50,0);
         filtr_ON_OFF = 0;
      }

    if (prikaz_I2C == 201)                                                                        // prikaz 201 zapne filtovani zapisem cisla 1 na adresu 50 v EEPROM
      {
         EEPROM.write(50,1);
         filtr_ON_OFF = 1;
      }


  }
//----------------------------------------------



//----------------------------------------------
// Spousti se vzdycky okamzite po zapnuti napajeni (pokud EEPROM na adrese 100 neobsahuje nejaky smysluplny znak).
// Kdyz EEPROM neobsahuje zadne informace o HW (je po predchozim servisnim nastaveni zaplnena mezerami), 
//  snazi se 3 sekundy cist zpravy z modulu a hledat trojice znaku "HW " nebo "IC=" . 
//  Kdyz dorazi, nasledujicich 15 znaku ulozi do EEPROM (mezery se preskakuji)
void zjisti_typ_HW(void)
  {
    uint32_t start_time = millis();                  // zacatek 3-sekundoveho timeoutu
    char znak;                                       // prijaty znak ze seriove linky
    uint16_t adresa  = 0;                            // aktualni ukazatel na adresu do EEPROM, kam se bude ukladat prijaty znak
    uint8_t pozice_H = 0;                            // hledani znaku "HW "
    uint8_t pozice_I = 0;                            // hledani znaku "IC="
    
    while (millis() - start_time < 3000)         // 3 sekundy po hot startu testuj seriovou linku
      {
        if (Serial.available())
          {
            znak = Serial.read();
            
            // --- hledani retezce "HW " ---
            switch (pozice_H)
              {
                case 0: if (znak == 'H') pozice_H = 1;
                        break;
                case 1: if (znak == 'W') pozice_H = 2;
                        else             pozice_H = 0;
                        break;
                case 2: if (znak == ' ') pozice_H = 100;                     // nalezen cely hledany retezec "HW "
                        else             pozice_H = 0;
                        break;
              }
      
            // --- hledani retezce "IC=" ---
            switch (pozice_I)
              {
                case 0: if (znak == 'I') pozice_I = 1;
                        break;
                case 1: if (znak == 'C') pozice_I = 2;
                        else             pozice_I = 0;
                        break;
                case 2: if (znak == '=') pozice_I = 100;                     // nalezen cely hledany retezec "IC="  
                        else             pozice_I = 0;
                        break;
              }
            
            if (pozice_H >= 100 and znak != ' ')       // byl nalezen cely hledany retezec "HW ", pripadne mezery se preskakuji a do EEPROM se nezapisuji
              {
                adresa = pozice_H;
                EEPROM.write(pozice_H , znak);
                pozice_H ++;
                if (pozice_H == 115)  return;          // po 15. znaku se podprogram ukonci (neceka se na dobehnuti 3s timeoutu)
              } 
           
            if (pozice_I >= 100 and znak != ' ')       // byl nalezen cely hledany retezec "IC=", pripadne mezery se preskakuji a do EEPROM se nezapisuji
              {
                adresa = pozice_I;
                EEPROM.write(pozice_I , znak);
                pozice_I ++;
                if (pozice_I == 115)  return;          // po 15. znaku se podprogram ukonci (neceka se na dobehnuti 3s timeoutu)
              } 
          }
      }

      // zprava o HW nedorazila a vyprsel 3-sekundovy timeout (nebo dorazilo jen mene nez 15 znaku)
    for (byte i = adresa; i < 115 ; i++)             // blok EEPROM se zaplni mezerama (pokud ale nekolik znaku stacilo dorazit, tak se neprepisuji)
      {
        EEPROM.write(i , ' ');
      }

  }
//----------------------------------------------



//----------------------------------------------
// nastaveni konfigurace pro novejsi moduly NEO
//  protoze nemaji EEPROM, musi se nastavovat kofigurace pri kazdem zapnuti, je ale mozne pouzit vetsi rychlost nastavovani
// podobne jako v podprogramu "GPS_setup(void)" se vypnou vsechny NMEA zpravy a zustanou jen GxGGA, GxRMC a GxGSA
void setup_neo_8(void)
  {
    delay(50);
    sendPGM(vypni_GxGLL, 11);
    LED1(false);
    delay(40);
    sendPGM(vypni_GPTXT, 11);
    LED2(false);
    delay(40);
    sendPGM(vypni_GxGSV, 11);
    LED3(false);
    delay(40);
    sendPGM(vypni_GxVTG, 11);
    LED1(true);
    delay(40);
    sendPGM(vypni_GxGRS, 11);
    LED2(true);
    delay(40);
    sendPGM(vypni_GxGST, 11);
    LED3(true);
    delay(40);
    sendPGM(vypni_GxZDA, 11);
    LED1(false);
    delay(40);
    sendPGM(vypni_GxGBS, 11);
    LED2(false);
    delay(40);
    sendPGM(vypni_GxDTM, 11);
    LED3(false);
    delay(40);
    sendPGM(vypni_GxGNS, 11);
    LED1(true);
    delay(40);
    sendPGM(vypni_GxVLW, 11);
    LED2(true);
    delay(40);
    sendPGM(vypni_PUBX1, 11);
    LED3(true);
    delay(20);
    sendPGM(vypni_PUBX2, 11);
    LED1(false);
    delay(40);
    sendPGM(vypni_PUBX3, 11);
    LED2(false);
    delay(40);
    sendPGM(vypni_XXXX1, 11);
    LED3(false);
    delay(40);
    sendPGM(vypni_XXXX2, 11);
    LED1(true);
    delay(40);
    sendPGM(vypni_XXXX3, 11);
    LED2(true);
    delay(40);
    sendPGM(vypni_XXXX4, 11);
    LED3(true);
    delay(40);
    
    sendPGM(zapni_GxGSA, 11);
    LED1(false);
    LED2(false);
    LED3(false);
    delay(400);   
    sendPGM(zapni_GxGGA, 11);
    LED1(true);
    delay(400);
    sendPGM(zapni_GxRMC, 11);
    LED2(true);
    delay(400);
    sendPGM(jeden_Hz , 22);
    LED3(true);
    delay(400);

  }
//----------------------------------------------



//----------------------------------------------
//  POZOR! funguje jen pro verzi GPS modulu: NEO6-M
//        Pri pouziti verze NEO8-M se kofigurace do EEPROM neuklada a musi se pouzit prodprogram "setup_neo_8()"
// vsechny nepotrebne NMEA zpravy se vypnou, zustanou jen GxGGA, GxRMC a GxGSA
void GPS_setup(void)
  {
    Serial.flush();

    sendPGM(vypni_GxGLL, 11);
    LED1(false);
    delay(1500);
    sendPGM(vypni_XXXX4, 11);  
    LED2(false);
    delay(1500);
    sendPGM(vypni_GxGSV, 11);
    LED3(false);
    delay(1500);
    sendPGM(vypni_GxVTG, 11);
    LED1(true);
    delay(1500);
    sendPGM(vypni_GxGRS, 11);
    LED2(true);
    delay(1500);
    sendPGM(vypni_GxGST, 11);
    LED3(true);
    delay(1500);
    sendPGM(vypni_GxZDA, 11);
    LED1(false);
    delay(1500);
    sendPGM(vypni_GxGBS, 11);
    LED2(false);
    delay(1500);
    sendPGM(vypni_GxDTM, 11);
    LED3(false);
    delay(1500);
    sendPGM(vypni_GxGNS, 11);
    LED1(true);
    delay(1500);
    sendPGM(vypni_GxVLW, 11);
    LED2(true);
    delay(1500);
    sendPGM(vypni_PUBX1, 11);
    LED3(true);
    delay(1500);
    sendPGM(vypni_PUBX2, 11);
    LED1(false);
    delay(1500);
    sendPGM(vypni_PUBX3, 11);
    LED2(false);
    delay(1500);
    sendPGM(vypni_XXXX1, 11);
    LED3(false);
    delay(1500);
    sendPGM(vypni_XXXX2, 11);
    LED1(true);
    delay(1500);
    sendPGM(vypni_XXXX3, 11);
    LED2(true);
    delay(1500);
    sendPGM(vypni_GPTXT, 11);    
    LED3(true);
    delay(1500);
    
    sendPGM(zapni_GxGGA, 11);
    LED1(false);
    delay(1500);
    sendPGM(zapni_GxRMC, 11);
    LED2(false);
    delay(1500);
    sendPGM(zapni_GxGSA, 11);
    LED3(false);
    delay(1500);

    sendPGM(jeden_Hz , 22);
    LED1(true);
    delay(1500);
    
    sendPGM(SAVE_EEPROM , 21);
    LED2(true);
    delay(1500);
    LED3(true);

    for (byte i = 100; i < 115 ; i++)             // blok EEPROM s verzi HW se vymaze (zaplni cislem 255)
      {
        EEPROM.write(i ,255);
        verzeHW[i-100] = 'x';                     // textovy popis verze_HW se nastavi na "xxxxxxxxxxxxxxx". Po resetu se pokusi obnovit z textovych zprav modulu GPTXT 
      }

    sendPGM(hot_start , 12);    
  }
//----------------------------------------------



//----------------------------------------------
// Pro praci s daty ulozenymi ve FLASH (setri RAM)
void sendPGM(const byte *data, size_t len)
  {
  for (size_t i = 0; i < len; i++)
    {
      Serial.write(pgm_read_byte(data + i));
    }
  }
//----------------------------------------------

// ze dvojice zadanych souradnic spocte vzdalenost
//   zjednoduseno pro male rozdily vzdalenosti (nekolik kilometru)
float vzdalenost_m(float lat1, float lon1, float lat2, float lon2)
  {
    const float METRY_NA_STUPEN = 111320.0f;

    float latRad = ((lat1 + lat2) * 0.5f) * DEG_TO_RAD;

    float dLat = lat2 - lat1;
    float dLon = lon2 - lon1;

    float dy = dLat * METRY_NA_STUPEN;
    float dx = dLon * METRY_NA_STUPEN * cos(latRad);

    return sqrt(dx * dx + dy * dy);
  }
