// Amaterske SQM - Sky Quality Meter
//   AstroMiK
//
//
// Detaily (schema, plosnak, navod, fotky):
//           http://sqm.astromik.org
//
#define verzeSW "2026-05-18 (STM32F4x1CEU)"
//        V desce SQM-GPS je nutna verze programu alespon "2026-05-17..INT" (doplneni nezprumerovanych souradnic v I2C registrech 20 az 29)
//============================================================================
// Odladeno pro Board Manager:
//          https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
//             Pro balik desek "STM32 MCU based boards"
//                by STMicroelectronics verze 2.6.0 !!!
//     V novejsich verzich tam porad neco predelavaji a ja uz nemam silu neustale reagovat na to, ze mi omezuji pouzivane funkce.
//============================================================================


// #doc#01
// Osazene periferie:
//     pokud nejaka periferie neni vubec osazena, nastavi se prislusna promenna na "false"
bool osazeno_DISPLEJ = true;      // pro zobrazeni je pouzity displej
bool osazeno_SD_card = true;      // je osazena SD karta
bool osazeno_BME280  = true;      // je osazene cidlo pro mereni teploty, tlaku a vlhkosti
bool osazeno_LSM303  = true;      // je osazeno cidlo naklonu s kompasem
bool osazeno_r30s    = true;      // je osazena pridavna EEPROM pro rychle (30-sekundove) zaznamy teploty, tlaku a vlhkosti
bool osazeno_DS3231  = true;      // je osazen pridavny RTC obvod pro spousteni SQM i pri vypnutem napajeni
bool osazeno_uref    = true;      // jsou osazeny casti pro mereni napeti hlavniho zdroje
bool osazeno_gps     = true;      // je osazena vnitrni deska SQM-GPS-6


// #doc#02
// pridavna I2C cidla - zatim nepouzita - jen pripravene rezervy - kazde cidlo by mohlo ukladat dva bajty do EEPROM
//   v budoucnu to muze byt vyuzito napriklad na cidlo UV zareni nebo cidlo snehu, vetru ....
//#define cidlo_rezerva_1                                  // cidlo _____
//#define cidlo_rezerva_2                                  // cidlo _____
//#define cidlo_rezerva_3                                  // cidlo _____
//#define cidlo_rezerva_4                                  // cidlo _____


// #doc#03
// vybrat jazyk
#define CZ_language                                      // cestina
//#define EN_language                                    // anglictina jeste neni uplne dokonale prelozena (texty jsou v souboru "jazyky.h")     //!!


// #doc#04
// nastaveni analogovych urovni pro test baterie (meri se za odporovym delicem R7, R8, R6)
//     merici rozsah je 0 az 3,3V a to odpovida cislu 0 az 4095.
//  Uvedene meze vychazi ze skutecne zjistenych hodnot - v priloze je dokument "test_baterie.xls"
#define bat_level_3     620                              // (7V) nad level_3 je vyborny stav baterie                        (blika   1 tecka na displeji)
#define bat_level_2     528                              // (6V) mezi level_3 a level_2 (7V az 6V)je dobry stav baterie     (blikaji 2 tecky na displeji)
#define bat_level_1     441                              // (5V) mezi level_2 a level_1 (6V az 5V) je stredni stav baterie  (blikaji 3 tecky na displeji)
                                                         // pod mezi level_1 (pod 5V)                                       (blikaji 4 tecky na displeji)
#define ref_level      2401                              // kdyz je napajeni tak nizke, ze referencni napeti prekroci tuto uroven, rozblika se vsech 5 tecek


// #doc#05
// V minimalni variante se do EEPROM uklada techto 8 bajtu:
//     1. bajt       - znacka obsazeneho nebo volneho mista, znacka SEC/SELC, index prepnuteho cidla na rozsirujici desce
//     2. az 5. bajt - casovy udaj v sekundach od 1.1.1970
//     6. a 7. bajt  - udaj o namerenem plosnem jasu v tisicinach mag/arcsec2
//     8. bajt       - popis co zpusobilo spusteni mereni (tlacitko, seriova linka, automat, kalibrace), znacka stability mereneho jasu, adresa zarizeni pro komunikaci

// volitelne polozky, ktere se budou ukladat do EEPROM pri kazdem zaznamu (vice polozek znamena, ze se do EEPROM vejde mene zaznamu)
// --- PO ZMENE UKLADANYCH POLOZEK JE NUTNE PROVEST "HARD" FORMAT EEPROM (funkce "#FH" pres seriovou linku) ---
#define ukladat_teplotu             // do EEPROM se bude ukladat informace o teplote                                                           (plus  2 bajty na zaznam)
#define ukladat_tlak                // do EEPROM se bude ukladat informace o tlaku                                                             (plus  2 bajty na zaznam)
#define ukladat_vlhkost             // do EEPROM se bude ukladat informace o vlhkosti                                                          (plus  2 bajty na zaznam)
#define ukladat_infra               // do EEPROM se bude ukladat informace o INFRA slozce svetla                                               (plus  2 bajty na zaznam)
#define ukladat_full                // do EEPROM se bude ukladat informace o FULL slozce svetla                                                (plus  2 bajty na zaznam)
#define ukladat_contr_reg           // do EEPROM se bude ukladat informace o nastaveni Control registru cidla svetla (zesileni, doba snimani)  (plus  1  bajt na zaznam)
#define ukladat_GPS                 // do EEPROM se bude ukladat informace o zemepisnych souradnicich (nutny pridavny hardware)                (plus 10 bajtu na zaznam)
#define ukladat_naklon              // do EEPROM se bude ukladat informace o naklonu (nutny pridavny hardware)                                 (plus  2 bajty na zaznam)
#define ukladat_azimut              // do EEPROM se bude ukladat informace o naklonu (nutny pridavny hardware)                                 (plus  2 bajty na zaznam)
#define ukladat_astro               // do EEPROM se bude ukladat informace o elevaci Slunce a Mesice a o jeho osvetleni                        (plus  3 bajty na zaznam)

// #doc#06
// zatim nepouzita 4 rezervni cidla (z kazdeho cidla bude mozne ukladat 16-bitovou hodnotu)
//#define ukladat_rezerva_1           // do EEPROM se bude ukladat informace o _____ REZERVA 1 ____                                              (plus  2 bajty na zaznam)
//#define ukladat_rezerva_2           // do EEPROM se bude ukladat informace o _____ REZERVA 2 ____                                              (plus  2 bajty na zaznam)
//#define ukladat_rezerva_3           // do EEPROM se bude ukladat informace o _____ REZERVA 3 ____                                              (plus  2 bajty na zaznam)
//#define ukladat_rezerva_4           // do EEPROM se bude ukladat informace o _____ REZERVA 4 ____                                              (plus  2 bajty na zaznam)



// priklad: Pro uplne ukladani vsech polozek bude 1 zaznam dlouhy 44 bajtu. Do 128kB EEPROM se tak vejde asi 2900 zaznamu.
//          Pro minimalni verzi ukladani (uklada se jen nezbytne minimum informaci a svetlo) je jeden zaznam dlouhy 8 bajtu a do stejne EEPROM se tak vejde pres 16000 zaznamu

// #doc#07
#define zobraz_rezervy           0                       // zobrazi nebo skryje rezervni cidla ze "sys_info()" a z textove hlavicky "hlavicka[]"
                                                         // pri hodnote 0 se skryji vsechny 4 rezervy
                                                         // pri hodnote 1 se zobrazi rezerva 1 a ostatni se skryji
                                                         // pri hodnote 2 se zobrazi rezervy 1 a 2 a ostatni se skryji
                                                         // pri hodnote 3 se zobrazi rezervy 1, 2 a 3 a rezerva 4 se skryje
                                                         // pri hodnote 4 se zobrazi vsechno (nic se neskryva)
uint16_t velikost_hlavicky;                              // podle skrytych rezerv rozsiri/smrskne velikost hlavicky a textovyxh zaznamu

#define pocet_polozek_v_menu    29                       // pocet polozek v menu -1  (index nejvyssi polozky) polozky se pocitaji od indexu 0
#define pocet_periferii          4                       // pocet periferii -1 , ktere se daji vypnout v terenu pres specialni hmat OK+TS+PWRon (5 polozek se zadava jako pocet 4)

// #doc#19
#define pouzivat_kalibrator                              // Experimentalni funkce pro pridavny kalibrator ktere umozni pri zapnuti napajeni automaticky zmerit smerove charakteristiky cidla a odezvu na ruzne kombinace jasu dvou LED.
bool zrychlena_kalibrace = false;                        //     Pridavny kalibrator se sklada z procesoru STM32F103 na desce BluePill a serva (pozdeji mozna krokoveho motoru) a komunikuje s SQM pres bocni konektor I2C.
int16_t cyk_uhel;                                        //     Dale obsahuje dve LED (infra a bilou) vzdelene asi 2m od serva, na kterych se da nezavisle nastavit jas.
                                                         //     Kalibracni pripravek ma i displej a ovladaci joystick, pomoci kterych se da smer a jas LED nastavovat rucne.
                                                         //     Rucne nastaveny jas se pak muze snimat originalnim SQM od Unihedronu a porovnavat zmerene hodnoty obou zarizeni
                                                         //     Funkce sice muze zustat trvale zapnuta, ale jejim vypnutim se zkrati cas startu SQM, protoze se nebude muset kontrolovat pritomnost kalibratoru na I2C lince.
                                                         //     Zrychlena kalibrace je pro rychle otestovani smerove charakteristiky cocky s vynechanim mereni tmy, a jen jednou infra urovni. Vynechava se test kombinaci LED


float lux_kal_a, lux_kal_b, lux_kal_c;                   //   kalibracni parametry pro prevod hodnoty z cidla luxmetru na zobrazovanou hodnotu v luxech (zatim vubec netusim, jaky tvar bude mit prevodni krivka)
                                                         //      (v pripade linearniho posunuti budou pouzity pouze 2 parametry, jestli bude nejake prohnuti, pouzije se i treti parametr)
// #doc#08                                                       
#define AGING          0                                 //  pripadne jemne doladeni casovani DS3231

// #doc#09                                                       
#define posun_tlaku         60000                        // kolik Pa se odecita od zmerene hodnoty tlaku, aby se ukladany vysledek vesel do 2 bajtu (0 az 65535)
                                                         //          (obvykla zmerena hodnota je 100000Pa, po odecteni cisla 60000 je vysledek 40000 a to uz se vejde do 2 bajtu)

// #doc#14
#define BAT_USB             130                          // napeti v desetinach voltu, ktere je zmerene pri samostatne zapojenem, plne nabitem 12V akumulatoru
                                                         // pri vyssi hodnote, nez je tady uvedeno, se predpoklada napajeni pres USB, nebo kombinovane napajeni USB + nejaky zdroj
                                                         //  V tom pripade se tato hodnota neuklada pro pristi zobrazeni v "sys_info" do RTC zalohovane pameti.

bool napajeno_z_USB = false;                             //  znacka, jestli je aktualne zapojeny USB kabel (pri hrane true->false dochazi ke zvukove signalizaci)

bool menu_enable[33];                                    // 32 bitu pro zapinani a vypinani jednotlivych polozek v menu
bool perif_enable[17];                                   // 16 bitu pro zapinani a vypinani jednotlivych jednotlivych periferii

uint8_t delka_zaznamu;                                   // podle zvolenych polozek pro ukladani do EEPROM se nize vypocte delka jednoho zaznamu
uint32_t adr_dalsiho_zaznamu;                            // aby nebylo nutne pred kazdym ukladanim do EEPROM hledat volne misto, zapamatuje se do teto promenne adresa pristiho volneho prostoru
uint32_t adr_posl_zaznamu;                               // adresa, kam byl ulozen posledni zaznam (pro jednodussi ziskavani dat z EEPROM)

uint32_t max_zaznam_EEPROM;                              // adresa, na ktere je ulozeny prvni bajt z posledniho zaznamu (vypocitava se z velikosti pouzitelne EEPROM a z delky zaznamu)

// #doc#10
// definice adres v systemove casti EEPROM
char EEPROM_verze[] = "ver:008\r\n";         //  PRI  KAZDE  ZMENE  ROZLOZENI  DAT  V  EEPROM  ZMENIT  VERZI ! nutno dodrzet pocet znaku vcetne CRLF
#define eeaddr_RTC_set                0UL    // (4 bajty)   adresa v EEPROM pro cas posledniho serizeni RTC v sekundach od 1.1.1970
#define eeaddr_RTC_korekce            4UL    // (4 bajty)   adresa v EEPROM s informaci, za jak dlouho se ma pridavat autokalibracni sekunda  
#define eeaddr_leto_zima              8UL    // (1 bit)     adresa v EEPROM se znackou aktualni casove zony (0=zima; 1=leto)
#define eeaddr_oddelovace             9UL    // (5 bitu)    adresa v EEPROM s informacemi o oddelovacich v CSV souborech
#define eeaddr_automat               10UL    // (1 bajt)    adresa v EEPROM s intervalem automatickeho spousteni mereni v minutach
#define eeaddr_prumerovani           11UL    // (5 bitu)    adresa v EEPROM s poctem prumerovani mereni svetla
#define eeaddr_modbus_LED            12UL    // (2 bity)    adresa v EEPROM s informacemi o zapnuti a vypnuti modbusu a LED nebo displejovem menu
#define eeaddr_LED                   13UL    // (1 bajt)    adresa v EEPROM s informacemi o vypinani jednotlivych LED funkci
#define eeaddr_SLAVE_baud            14UL    // (6 bitu)    adresa v EEPROM s informacemi o adrese a rychlosti komunikace
#define eeaddr_CRC_tset_lock         15UL    // (5 bitu)    adresa v EEPROM s informacemi o testovani CRC, blokovani debug zapisu o serizeni casu a blokovani bocniho tlacitka
#define eeaddr_stab                  16UL    // (1 bajt)    adresa v EEPROM s urovni pro urceni nestabilniho jasu
#define eeaddr_pocet_cidel           17UL    // (3 bity)    adresa v EEPROM s poctem cidel svetla na pridavne desce
#define eeaddr_10kB_znacka           18UL    // (1 bajt)    adresa v EEPROM se znackou zaplneni pameti pro zaznamy (zvysuje se po kazdych 10kB zaplneni)
#define eeaddr_uroven_EEPROM         19UL    // (1 bajt)    adresa v EEPROM s urovni svetla pod kterou se uklada zaznam do EEPROM (pri vetsi tme, nez je nastaveno)
#define eeaddr_teplotni_kalibrace    20UL    // (28 bajtu)  adresa v EEPROM s kalibracni tabulkou pro teplotu (2 sloupce * 7 dvojbajtovych hodnot)
#define eeaddr_TS_stanoviste         48UL    // (2 bajty)   adresa v EEPROM se znackou pro nulovani poradi v zaznamech casovych razitek a aktualnim prednastavenym stanovistem
#define eeaddr_svetelna_kalibrace    50UL    // (420 bajtu) adresa v EEPROM s kalibracnimi tabulkami pro svetlo (7 cidel * 2 sloupce * 15 dvojbajtovych hodnot)
#define eeaddr_TS_aktual            470UL    // (1 bajt)    adresa v EEPROM s aktualnim cislem casove znacky 
#define eeaddr_kompas_offset        471UL    // (1 bajt)    adresa v EEPROM s ulozenou hodnotou dokalibrovani kompasu
#define eeaddr_perif_bity           472UL    // (2 bajty)   adresa v EEPROM s informacemi o deaktivovanych periferiich
#define eeaddr_beep_bity            474UL    // (2 bajty)   adresa v EEPROM s nastavenim funkci, ktere jsou signalizovany pipanim
#define eeaddr_elevace_soumrak      476UL    // (1 bajt)    adresa v EEPROM s elevaci Slunce pod horizontem, pri ktere se hlasi uzivatelsky nastavitelny soumrak
#define eeaddr_GPS_track_id         477UL    // (2 bajty)   adresa v EEPROM, do ktere se uklada stav GPS trasovani (bit15) a poradove cislo trasovaciho souboru (bity 14 az 0)
#define eeaddr_GPS_track_interval   479UL    // (1 bajt)    adresa v EEPROM, do ktere se uklada pocet sekund mezi zaznamy do trasovaciho GPX souboru (1 az 255)
#define eeaddr_AUTOkom              480UL    // (1 bajt)    adresa v EEPROM, do ktere se uklada zpusoob automatickeho odesilani dat do seriove linky
#define eeaddr_CALIB_znacka         481UL    // (1 bajt)    adresa v EEPROM s aktualnim cislem kalibracniho souboru (datum se meni kazdy den, ale tato znacka az po 256 kalibracich)
#define eeaddr_MOD_autoupdate       482UL    // (1 bajty)   adresa v EEPROM s volbou pro automaticke nastavovani modbusovych registru po ukonceni mereni
#define eeaddr_bortle               483UL    // (1 bajt)    adresa v EEPROM pomoci ktere je mozne zapnout nebo vypnout zobrazovani polozky "BortL" v menu "Jas" (pro zapnuti je nutne precist upozorneni v navodu)
#define eeaddr_servis               484UL    // (1 bajt)    adresa v EEPROM pouzivana pri vyvoji k ruznym testum a simulacim. Ve verejne verzi nepouzito, ale pro dalsi vyvoj ponechat bez zasahu.
#define eeaddr_GEO_lat              485UL    // (2 bajty)   adresa v EEPROM se zemepisnou sirkou pro astronomicke vypocty
#define eeaddr_GEO_lon              487UL    // (2 bajty)   adresa v EEPROM se zemepisnou delkou pro astronomicke vypocty
#define eeaddr_zima_hod             489UL    // (1 bajt)    adresa v EEPROM s casovym posunem proti UTC pri zimnim case
#define eeaddr_leto_hod             490UL    // (1 bajt)    adresa v EEPROM s casovym posunem proti UTC pri letnim case
#define eeaddr_menu_bity            491UL    // (4 bajty)   adresa v EEPROM s povolenymi nebo zakazanymi polozkami v menu
#define eeaddr_posledni_den         495UL    // (4 bajty)   posledni den, kdy probihal nejaky zaznam. Pouziva se pro vypis posledniho souboru se zaznamy do seriove linky.
#define eeaddr_AfD                  499UL    // (1 bajt)    adresa v EEPROM s hodnotou jasu pro Alarm for Darkness
#define eeaddr_stanoviste           500UL    // (75 bajtu)  adresa v EEPROM s 5 prednastavenymi pozorovacimi stanovisti (10 znaku pro nazev a 5 bajtu pro displejovou grafiku)
#define eeaddr_txt_zony             575UL    // (8 bajtu)   adresa v EEPROM s oznacenim casovych zon v textovem formatu (napr: "SEC " a "SELC")
#define eeaddr_luxmetr              583UL    // (6 bajtu)   adresa v EEPROM s kalibracnimi konstantami pro luxmetr
#define eeaddr_AFD_styl             589UL    // (1 bajt)    adresa v EEPROM s nastavenim chovani AFD
#define eeaddr_rezerva_2            590UL    // (9 bajtu)   rezerva v EEPROM
#define eeaddr_test_write           599UL    // (1 bajt)    testovaci zapis na kontrolu, ze EEPROM funguje
#define eeaddr_kompas               600UL    // (12 bajtu)  adresa v EEPROM s kalibracnimi konstantami pro kompas
#define eeaddr_naklon_horiz         612UL    // (6 bajtu)   adresa v EEPROM s kalibracnimi konstantami pro naklonomer (horizontalni poloha)
#define eeaddr_naklon_vert          618UL    // (6 bajtu)   adresa v EEPROM s kalibracnimi konstantami pro naklonomer (vertikalni poloha)
#define eeaddr_alarmy               624UL    // (12 bajtu)  adresa v EEPROM s casy 6 alarmu pro DS3231 (posledni dvojbajt je pro funkci "odpocet").
#define eeaddr_TEST_alarm           636UL    // (1 bajt)    adresa v EEPROM se znackou pro testovaci alarm.
#define eeaddr_DSO_setup            637UL    // (2 bajty)   adresa v EEPROM pro uchovani nastavenych parametru pro vypocet viditelnosti DSO (schopnosti, prumer, ohnisko dalekohledu) - 3x4 bity
#define eeaddr_DSO_prumery          639UL    // (6 bajtu)   adresa v EEPROM pro tabulku prednastavenych prumeru dalekohledu           (3x 2 bajty v [mm])
#define eeaddr_DSO_ohniska          645UL    // (6 bajtu)   adresa v EEPROM pro tabulku prednastavenych ohniskovych delek dalekohledu (3x 2 bajty v [mm])
#define eeaddr_DSO_okulary          651UL    // (5 bajtu)   adresa v EEPROM pro tabulku prednastavenych ohniskovych delek okularu    (5x 1 bajt  v [mm])
#define eeaddr_DSO_normal_MSA       656UL    // (1 bajt)    normalovy jas oblohy pro urcovani viditelnosti otevrenych hvezdokup v desetinach mag/arcsec2
#define eeaddr_rezerva_3            657UL    // (33 bajtu)  rezerva v EEPROM
#define eeaddr_MIN_EEPROM           700UL    // Na jake adrese v EEPROM zacina oblast pro namerena data
#define eeaddr_MAX_EEPROM       0x1FFFFUL    // maximalni mozna adresovatelna bunka v EEPROM  (pro 128k x 8b EEPROM )


#include <Wire.h>                                        // knihovna pro I2C komunikaci
#include <IWatchdog.h>                                   // podpora pro Watchdog
#include "jazyky.h"

HardwareSerial Serial1(PA10, PA9);
#define Serial Serial1                                   // prejmenovani Serial1.xxx na Serial.xxx

#define ID1 (*(uint32_t *)0x1FFF7A10)                    // unikatni identifikace procesoru
#define ID2 (*(uint32_t *)0x1FFF7A14)
#define ID3 (*(uint32_t *)0x1FFF7A18)
char ID_char[] = "xxxxxxxxxxxxxxxxxxxxxxxx\0";

#define VELIKOST_FLASH (*(uint16_t *)0x1FFF7A22)        // velikost PROGMEM v procesoru 

#include <Adafruit_BME280.h>                            // knihovna pro cidlo vlhkosti, teploty a tlaku
Adafruit_BME280 cidlo_BME;                             

#include <STM32RTC.h>
/* Get the rtc object */
STM32RTC& rtclock = STM32RTC::getInstance();


//-------------------
// prevzato z puvodni knihovny pro STM32F103
//  pouziva se pro prevody mezi poctem sekund od 1.1.1970 a strukturovanou promennou nadefinovanou jako 'tm_t'
typedef struct tm_t {
  uint8_t  year;    // years since 1970
  uint8_t  month;   // month of a year - [ 1 to 12 ]
  uint8_t  day;     // day of a month - [ 1 to 31 ]
  uint8_t  weekday; // day of a week (first day is Monday) - [ 0 to 6 ]
  uint8_t  pm;      // AM: 0, PM: 1
  uint8_t  hour;    // hour of a day - [ 0 to 23 ]
  uint8_t  minute;  // minute of an hour - [ 0 to 59 ]
  uint8_t  second;  // second of a minute - [ 0 to 59 ]
} tm_t;


#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24UL)
#define LEAP_YEAR(Y)  ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )
#define SECS_YR_2000  (946684800UL)

#if !defined(__time_t_defined) // avoid conflict with newlib or other posix libc
  #warning "Using private time_t definintion"
  typedef uint32_t time_t;
#endif

static  const unsigned char monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

//-------------------



time_t tt;
time_t tt_file;
tm_t mtt;
tm_t mtt_file;
tm_t mtt_EEPROM;



#define pin_SD_CS           PA4                          // CS na SD karte

#define pin_tl_up          PA15                          // ovladaci tlacitka
#define pin_tl_dn           PB3
#define pin_tl_ok           PB4

#define pin_bzuk           PB10                          // pin pro piskak (pasivni)
#define pin_DERE           PB15                          // pin pro prepinani smeru vysilani 


#define pin_dis_CLK        PB12                          // piny pro komunikaci s displejem
#define pin_dis_DIN        PB13

#define pin_NEO            PA1                           // ovladaci pin pro NeoPixel LED

#define pin_karta_IN       PB1                           // pin pro test zasunute karty
#define pin_refu           PB0                           // pin pro referencni napeti pro kontrolu stavu baterie

#define pin_AD_BAT         PA0                           // pin pro testovani napeti baterie
#define pin_TS            PA14                           // specialni pin pro zaznam casoveho razitka (TimeStamp)

#define pin_TEST_HW       PA13                           // spusteni testu bez pripojene komunikace


#define pin_DATA_RDY       PB2                           // pin pro zjistovani pripojeneho EXT zarizeni, nebo dostupnosti dat ve vnitrnim GPS modulu
#define pin_bok_out       PB14                           // pin na bocnim konektoru, ktery se ovlada funkci "Alarm for darkness"


#define pin_SDA            PB7                           // u procesoru STM32F411 se musi nadefinovat, na kterych pinech je komunikace I2C
#define pin_SCL            PB6

#define pin_MISO           PA6                           // u procesoru STM32F411 se musi nadefinovat, na kterych pinech je komunikace SPI
#define pin_MOSI           PA7
#define pin_SCLK           PA5


#define I2C_ADDR_EEPROM_RW_0     0b1010000               // I2C adresa EEPROM pro cteni a zapis dat pro bunky 0 az 65535 (adresovaci piny v LOW)
#define I2C_ADDR_EEPROM_RW_1     0b1010001               // I2C adresa EEPROM pro cteni a zapis dat pro bunky nad 65535  (adresovaci piny v LOW)
uint8_t I2C_ADDR;                                        // podle aktualni adresy zapisovane, nebo ctene bunky se nastavuje I2C adresa EEPROM

#define I2C_ADDR_EXPA    0x20                            // I2C adresa expanderu 0x20 (vsechny adresovaci piny v LOW)
#define I2C_ADDR_GPS     0x17                            // I2C adresa pridavneho modulu pro GPS
#define I2C_ADDR_KALIB   0x33                            // I2C adresa kalibratoru (2 x LED + krokovy motor s procesorem ATmega328)


#define I2C_ADDR_KOMPAS       0b0011110
#define I2C_ADDR_NAKLON       0b0011001
#define  OUT_X_L_A            0x28
#define  CTRL_REG1_A          0x20
#define  CTRL_REG4_A          0x23
#define  CRA_REG_M            0x00
#define  CRB_REG_M            0x01
#define  MR_REG_M             0x02
#define  DLHC_OUT_X_H_M       0x03


// Modbus
#include <ModbusRtu_AstroMiK.h>                         // knihovna pro Modbus komunikaci (upravena puvodni knihovna )
Modbus MBslave(1, Serial2, pin_DERE);                   // ID = 1 (bude dale nastaven), ser.port, ridici pin (pin_DERE)
uint16_t MODdata[111];                                  // tabulka hodnot parametru Modbus
uint8_t stav_MOD = 0;
uint8_t posledni_funkce;
bool pouzit_modbus;                                     // misto puvodni verze predavani dat pres RS485 se muze pouzit klasicky modbus
uint16_t index_posledniho_zaznamu;                      // index posledniho zaznamu se prubezne aktualizuje v modbusovem registru [40040]
uint8_t MOD_autoupdate;                                 // informace, jestli se maji registry 30001 az 30031 nastavovat automaticky po kazdem mereni

uint16_t ser_out_data[31];                              // tabulka hodnot, ktere se odesilaji v uspornem formatu do obsluzneho programu 


bool leto =  false;                                     // letni cas

uint8_t data;
bool open_OK; 

uint8_t prumery;                                        // globalni promenna pro pocet mereni hodnot svetla, ze kterych se vypocte prumer
uint8_t automat;                                        // globalni promenna pro nastaveni intervalu mezi automatickym merenim svetla v minutach (0=automat vypnuty)
uint8_t selc;                                           // globalni promenna pro nastaveni casove zony (0=SEC, 1=SELC)
uint8_t slave_addr;                                     // SLAVE adresa pro komunikaci                     
int16_t list;                                           // listovani v poslednich zmerenych hodnotach
uint8_t hranice_ukladani;                               // hodnota svetla v desetinach mag/arcsec2 pod kterou se nebudou ukladat zaznamy do EEPROM ani na kartu

uint32_t posledni_autospusteni;                         // pomocna promenna pro cas posledniho automatickeho spusteni

char prijato[16];                                       // pole, do ktereho se ukladaji znaky, prijate pres seriovou linku (nastaveni casu, zadost o informace, korekce ....)
uint16_t i;                                             // pomocna globalni promenna pro ruzne smycky

uint8_t prijimaci_pole[16];                             // pole, do ktereho se ukladaji pozadavky od MASTERa pri komunikaci RS485
uint8_t vysilaci_pole[50];                              // pole, do ktereho se pripravuji data k odeslani do MASTERa pri komunikaci RS485

uint8_t dokoncene_mereni = 2;                           // na zacatku mereni se tato promenna vynuluje a po dokonceni mereni (ktere muze trvat treba i 20 sekund) se nastvi zpatky na 2
uint8_t prubeh_mereni;                                  // Kdyz prijde dotaz na posledni namerene hodnoty ( pres seriovou linku RS485), ale mereni jeste nebude ukoncene, odesle se zbyvajici pocet mereni

bool probiha_mereni_svetla;                             // pri rezimu 30s umozni zablokovat konkurencni mereni svetla z menu

uint32_t casova_znacka;                                 // pouzito pro zjisteni doby, jak dlouho se ma ignorovat komunikace pres RS485
uint16_t ignor_cas = 3000;

uint8_t pouzij_CRC;                                     // podle parametru v EEPROM[15] je mozne vypnout kontrolu CRC pri prijmu pozadavku pres RS485
uint8_t index_cidla_svetla = 1;                         // v pripade rozsirujici desky pro vice cidel je v teto promenne ulozene cislo prave aktivniho cidla 1 az 7 - (specialni pripad 0)
uint8_t pocet_cidel_svetla;                             // pocet cidel svetla na expanzni desce

uint8_t afd;                                            // nastavena hodnota "Alarm for Darkness"
uint32_t predchozi_cas_afd = 0;                         // casova znacka predchoziho rychleho testu AFD
bool status_afd;                                        // vysledek porovnani nastavene hodnoty 'afd' a zmereneho jasu pro modbusovy status registr
bool afd_bzuk;                                          // znacka, jestli je povolene nebo blokovane zobrazeni napisu "-AFd-" na displeji a bzukani pri AFD
uint32_t posledni_blik_afd = 0;                         // pri aktivnim AFD muze blikat RGB LED 2x za sekundu
bool afd_log;                                           // do syslogu se odesila prvni spusteni AfD po dosazeni tmy
bool afd_nabito;
uint8_t afd_desetiny;                                   // zmerena hodnota jasu v desetinach mag/arcsec2 zaokrouhlena na desetiny pro zaznam do syslogu
uint8_t afd_styl;                                       // nastaveni chovani AFD

uint16_t JAS_log_full;                                  // pri logovani jasu (pri spustenem menu JAS bez korekce) se nove do logu zapisuji hodnoty obou svetelnych registru a config registr cipu TSL
uint16_t JAS_log_ir;
uint8_t  JAS_log_cnf;

bool int_po_kalibraci;                                  // zvlastni pripad, kdy se do interruptu odskakuje z trvale zobrazene hodnoty po kalibraci pres menu
                                                        //    kdyz v tom pripade dojde k nejakemu podprogramu se signalizaci na displeji, zobrazi se po navratu z interruptu opet zmerena kalibracni hodnota 

uint8_t lap_count;                                      // pocitadlo mezicasu od spusteni stopek (1 az 99)

bool blokuj_TS;                                         // zaznam casove znacky je blokovan az do uvolneni tlacitka (aby pri drzeni tlacitka nebylo mozne neustale ukladani casovych znacek)
bool TS_lock;                                           // znacka, ze je bocni tlacitko zamcene proti nechtenemu sepnuti

uint8_t alarm_bat_pamet = 1;                            // pouzito pro varovne akusticke signaly pri poklesu napeti pod nastavene meze

uint8_t pole_EEPROM[55];                                // globalni promenna, do ktere se pri kazdem mereni ukladaji namerena data. Toto pole se nasledne zapisuje do EEPROM a na SD kartu
uint8_t pole_GPS_I2C[40];                               // pole, do ktereho se stahuji data z GPS modulu pres I2C
uint8_t pole_KALIB_I2C[12];                             // pole, do ktereho se ctou data z pridavneho kalibratoru
uint8_t pole_proudy_kalib[64];                          // sem se stahuji kalibracni proudove udaje z kalibratoru (hodnoty proudu LED pro 0.5, 1, 1.5 ... 10mA)

char pole_GPS_NMEA[] = "$GPRMC,110109.00,A,4926.67723,N,01421.99214,E,0.521,,311220,,,A*72   + nejaka rezerva    @\0";   // pole, do ktereho budou stahovat neupravene NMEA zpravy z GPS modulu

int16_t lat_alt_lon = 0;                                // pro listovani v podmenu "GPS"  (LAT->ALT->LON->trASA a opacne)
bool dorotovano;                                        // znacka, ze rotace souradnic na displeji probehla az do konce a od priste uz se rotovat nebude, ale bude se zobrazovat jen poslednich 5 cifer

uint16_t  int_svetlo;                                   // globalni promenne, ktere uchovavaji posledni zmerene hodnoty
uint16_t  int_teplota;
uint16_t  int_vlhkost;
uint16_t  int_tlak;
uint16_t  int_infra;
uint16_t  int_full;
uint16_t  int_naklon;
uint16_t  int_azimut;
uint32_t  int_luxy;
uint8_t   byte_cnf;
uint32_t  GPS_lat;
uint32_t  GPS_lon;
uint16_t  GPS_alt;
uint32_t  GPS_lat_akt;
uint32_t  GPS_lon_akt;
uint16_t  GPS_alt_akt;

uint16_t  int_rezerva_1;                                // zatim nepouzita rezervni cidla
uint16_t  int_rezerva_2;
uint16_t  int_rezerva_3;
uint16_t  int_rezerva_4;


uint8_t AUTOkom_export_pole[50];                        // sem do toho prostoru se poskladaji casti zaznamu, ktere se odesilaji v automatickem rezimu do nektere seriove linky
uint32_t AUTOkom_posledni;                              // adresa posledniho zaznamu, s merenim svetla, ktery se bude odesilat pri automatickem odesiani dat do seriove linky (ignoruji se TimeStampy, stopky a kalibrace) 
uint8_t AUTOkom;                                        // konfigurace automatickeho odesilani dat do seriovych linek
uint8_t AUTOkom_linka ;                                 // linka 1 (RS485), nebo 2 (USB) 
uint8_t AUTOkom_format;                                 // format odesilanych dat (0= pole dat; 1= text)
uint8_t AUTOkom_obsah;                                  // obsah odesilanych dat:
                                                        //     0bxxxx0000 - nejzakladnejsi data: datum a cas (bez casove zony, bez DvT) + jas mmag/arcsec2
                                                        //     0bxxxxxxx1 - pridat informace o pocasi (teplota, tlak, vlhkost)
                                                        //     0bxxxxxx1x - pridat detailni informace o jasu (infra / full / config / stabil)
                                                        //     0bxxxxx1xx - pridat detailni informace o naklonu a azimutu
                                                        //     0bxxxx1xxx - pridat detailni astro informace (azimut a elvace Slunce a Mesice + osvetleni)
                                                        //     0bxxxx1111 - odesle uplne cely zaznam (i informace, ktere nejsou pomoci jednotlivych bitu dostupne)



bool senzor_BME;                                        // BME280: cidlo teploty, tlaku a vlhkosti
bool modul_LSM303DLHC;                                  // naklonomer s kompasem LSM303DLHC
bool pouzivat_SD_kartu;                                 // povolene nebo zakazane pouzivani SD karty
bool pouzit_ds3231;                                     // kdyz neni pridavny RTC obvod DS3231 osazeny, blokuji se nektere funkce (I2C, menu, vypisy a zadavani budiku, testy)
bool pouzit_r30s;                                       // kdyz neni osazena extra EEPROM pro rezim 30s, blokuji se nektere funkce

uint16_t peribity_maska = 0b1111111111111111;           // bitova maska, pri ktere se nehlasi varovani na vypnute periferie
                                                        //'1' = ignorovat pripadne neshody, '0' = hlasit periferie, ktere nejsou v EEPROM v '1'
                                                        //          bit4=r30s; bit3=DS3231; bit2=SD karta; bit1=LSM303; bit0=BME
                                                         
uint32_t GPS_temp_time;                                 // pri automatickem mereni se zapnutou a zafixovanou GPS se nenastavuje RTC, ale do zaznamu se pouziva prijaty cas z GPS
bool GPS_RTC_flag;                                      // globalni promenna, ktera signaluzuje, ze se misto casu z RTC ma pouzit cas z GPS ('GPS_temp_time')
bool vnitrniGPS = false;                                // typ pouzite GPS desky (urcuje se z prijatych dat z GPS modulu)
uint8_t test_prg_flag = 1;                              // jen pro testovaci ucely - SMAZAT !!--

uint8_t posledni_i2C_prikaz;                            // aby se zbytecne neustale neobnovoval stav LED v GPS modulu

uint8_t         znacka;                                 // znacka, co spustilo mereni (tlacitko, automat , seriova linka) + dalsi info (SLAVE adresa a stabilita svetla)

int16_t pamet_LED_svitilna = 1;                         // az do vypnuti napajeni se v teto promenne uchovavanastaveny jas cervene svitilny (0...zhasnuto az 5...sviti vsech 5 sedmisegmentovek)

// promenne pro pripadnou korekci vnitrniho RTC
uint32_t cas_minuleho_nastaveni;                        // cas predchoziho nastaveni casu v sekundach od 1.1.1970
uint32_t korekce;                                       // po jake dobe se ma pricist nebo odecist 1 sekunda

uint8_t rozhod_stab;                                    // rozhodovaci uroven pro stabilni jas

uint8_t D_pamet[5];                                     // V tomto poli se bitove uchovava aktualni stav vsech rozsvicenych segmentu
uint8_t D_pam_pred_int[5];
uint8_t pozice_tecky;                                   // Pocet desetinnych mist pro zobrazeni cisla (0 az 4) - na pozici 0 ale tecka nikdy nesviti
long posledni_n;                                        // Pri aktualizaci pozice desetinne tecky se znovu zobrazi posledni zadane cislo 

//uint8_t lat_scroll[20];                               // pole, do ktereho se uklada retezec k rotaci na displeji pri zobrazeni zemepisne sirky
//uint8_t lon_scroll[20];                               // pole, do ktereho se uklada retezec k rotaci na displeji pri zobrazeni zemepisne delky
//uint8_t alt_scroll[5];                                // pole, do ktereho se uklada retezec pri zobrazeni nadmorske vysky (tohle nerotuje, ale zachoval jsem podobne nazvy)
uint8_t pom_disp_scroll[30];

int16_t LOC_hod, LOC_den, LOC_mes, LOC_min, LOC_sek;    // promenne, ktere obsahuji aktualni cas v RTC (mistni cas)
uint16_t LOC_rok ;                                      // rok (od 1970)
uint8_t LOC_dvt;                                        // den v tydnu (po ... ne)
uint32_t rokmesden;                                     // rok mesic a den ve formatu yyyymmdd
uint16_t mesden;                                        // mesic a den ve formatu mmdd

bool RTC_SD_DEBUG;                                      // pokud je tato znacka aktivni, budou se pri kazdem nastaveni hodin pres GPS zapisovat do zvlastniho souboru informace o presnosti RTC

int16_t LOC_den_file, LOC_mes_file;                     // promenne, ktere obsahuji datum pro nazev slozky na SD karte
uint16_t LOC_rok_file ;                                 // rok (od 1970)

uint32_t dvouminutovy_bzuk=0;                           // pipani pri alarmu bude trvat maximalne 2 minuty. Sem se ulozi cas, ve kterem pipani zacalo

                  // definice znaku pro displej se spolencou anodou
                  //       0    ,       1    ,       2    ,       3    ,       4    ,       5    ,       6    ,       7    ,       8    ,       9    
uint8_t graf_def[10] = {0b00111111 , 0b00000110 , 0b01011011 , 0b01001111 , 0b01100110 , 0b01101101 , 0b01111101 , 0b00000111 , 0b01111111 , 0b01101111 };

uint8_t pole_citlivosti[4] = {0 , 16 , 32 , 48 };       // stupen citlivosti cidla
int16_t posledni_citlivost = 3;                         // posledni pouzity stupen citlivosti (pouziva se k uychleni mereni svetla pri vice merenich v rade)


uint32_t pole_baudrate[4] = {9600 , 19200 , 38400 , 115200 };      // prednastavene komunikacni rychlosti

// zmena jasu LED neni provadena linearne, ale logaritmicky.
//    (protoze je dobre znatelny rozdil v jasu mezi cisly 3 a 4, ale neni pozorovatelny zadny rozdil v jasech mezi cisly 250 a 251 )
//        stupen jasu             0,   1,   2,   3,   4,   5,   6,   7
const uint8_t logaritmy_jasu[] = {   0,   1,   3,   7,  16,  40, 102, 255};   // 0 = zhasnuto ; 255 = maximalni jas
uint32_t barva_LED = 0;


uint16_t bat_in, bat_3d;
float Vcc, napeti_9V, bat_3V, ref_in; 
int16_t bat_proc;
bool logbat = true;                                     // 5 sekund po zapnuti napajeni se loguje napeti 3V zalohovaci baterie
bool testbat_probehl;                                   // znacka se kazdou 0.8 sekundy nastavuje a kazdou 0.2 sekundy shazuje, aby test baterie probihal jen 1x za sekundu

uint8_t signal_LED;                                     // informace o tom, ktere veci ma signalizovat RGB LED
uint16_t signal_BEEP;                                   // informace o tom, ktere veci se maji signalizovat piskanim

uint8_t err_bit;                                        // globalne dostupna promenna s informacemi o chybach (predava se pak treba pres komunikaci)
                                                        // 0bxxxxxxx1 - chyba I2C
                                                        // 0bxxxxxx1x - chyba pri zapisu na SD kartu
                                                        // 0bxxxxx1xx - neni zasunuta SD karta
                                                        // 0bxxxx1xxx - chyba RTC
                                                        // 0bxxx1xxxx - chyba naklonomeru


bool menu_dis_led;                                      // touto promennou je mozne volit typ menu podle varianty SQM (rucni verze s displejem / trvale nainstalovana verze se signalizacni LED)

bool break_blik;                                        // moznost preruseni vyblikavani dlouheho cisla pomoci LED

uint8_t znacka_listu;                                   // pri vypisu svetla se jeste pred datumem vypise informace o typu zaznamu:
                                                        //   'c' = jeden z 10 kalibracnich vzorku
                                                        //   'C' = prumer vsech 10 kalibracnich vzorku
                                                        //   'u' = spusteno tlacitkem "Up"
                                                        //   'd' = spusteno tlacitkem "Down"
                                                        //   'o' = spusteno tlacitkem "Ok"
                                                        //   'A' = spusteno automatem
                                                        //   'M' = spusteno komunikaci


uint8_t znak_stupen = 32;

uint16_t kal_svetlo[20][2] ;                            // pole kalibracnich hodnot pro svetlo
uint16_t kal_teplota[10][2];                            // pole kalibracnich hodnot pro teplotu
uint16_t kal_serial[20][2] ;                            // pres seriovou linku se nejdriv ulozi prijata data do tohoto pole. Po jejich prekontrolovani se pak prepisou do jednoho z poli kal_teplota[], nebo kal_svetlo[]


//  index pole 100x                                                                                                            111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111122222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222223
//  index pole 10x                   111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990
//  index pole 1x          0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
//                         -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------   
char vystupni_retezec[] = "                                                                                                                                                                                                                                                                                               \0";
// priklad s rezervami    " 00018 ; 000823 ; 1609232363 ; 29.12.2020 ; 08:59:23 ; Ut ;  SEC ; 08881 ; 08,881 ; 01 ; 01 ; komun ;  STABIL  ; 17989 ; 48311 ; 600 ;  428x ; 07196 ; +21,96 ; 05151 ; 051,51 ; 093410 ; +49,457154 ; +120,457156 ; -1234 ; +165.8 ; 123 ;  -45 ;  -27 ;  58  ; 65535 ; 65535 ; 65535 ; 65535 \0";    

char DATCAS_retezec[] = "2022/01/24;18:45:00";          // pomocny globalni retezec pro predavani datumu a casu

uint32_t stisk_time;                                    // pro pocitani delky stisku tlacitka
bool stisk;                                             // pro pociteni delky stisku tlacitka
bool bude_save;                                         // pri opusteni menu tato promenna urcuje, jestli se ma nova hodnota ulozit do EEPROM/(RTC), nebo jestli se nic ukladat nebude 
uint8_t jas_displeje;                                   // globalni promenna pro nastaveni jasu displeje podle posledni zmerene hodnoty svetla

#define AUTOEXITMENU      300000UL                      //  po jake dobe necinnosti v [ms] se menu automaticky opusti (5 minut * 60 sekund * 1000 milisekund)
bool auto_exit_menu;                                    // pri 'true' se okamzite vypadne ze vsech vnorenych podprogramu jakoby se stiskavalo kratce tlacitko OK az do doby vypadnuti do hlavni smycky
uint32_t auto_exit_casovani;                            // po dobehnuti teto promenne na hodnotu udanou v definici AUTOEXITMENU se automaticky vypadne z menu

uint8_t spec_rezim = 0;                                 // podle pripojenych I2C periferii je mozne provozovat zarizeni v ruznych specialnich rezimech //!!(r30s)

uint32_t predchozi_zapis_afd;

bool pole_zobr_menu[33];                                // nastavuje, ktere polozky se budou zobrazovat v menu ([0]=List; [1]=Auto; [2]=zona .... [13]=GPS)
bool prvni_vstup_do_menu = true;                        // pri prvnim vstupu do menu po zapnuti napajeni se hleda nejnizsi zobrazitelna polozka v menu

uint8_t oddelovaci_bajt;                                // obsah EEPROM(9), ktery obsahuje informace o oddelovacich znaminkach pro CSV soubor na SD karte
bool des_carka;                                         // oddelovac desetinnych mist (tecka = false; carka = true)
bool uzavirat_uvozovky;                                 // uzavirat kazdou hodnotu na SD karte do uvozovek
bool vytvaret_hlavicku;                                 // Na zacatek kazdeho souboru na SD karte zapise (nebo nezapise) textovy popis sloupcu
uint8_t oddelovac_polozek;                              // Urcuje znak, kterym se budou oddelovat jednotlive polozky v CSV souboru (mezera, strednik, carka, tabulator)

char oddelovac;                                         // znak pro oddelovani polozek v zaznamech (strednik, carka, mezera, tabulator)
char uvozovky;                                          // znak pro uzavirani polozek do uvozovek (nebo mezera)
char desod;                                             // znak pro oddeleni desetinnych mist (carka, nebo tecka)

uint8_t pocet_bzuku;                                    // Vypinani akusticke signalizace chyby po 5 bzuknutich

int16_t styl_vodovahy;                                  // 0 ... elevace; 1 ... dvouosa vodovaha horizontalne nebo svisle (podle aktualniho uhlu naklonu)
int16_t astro_parametr;
bool stabilni_naklon;                                   // znacka, jesli posledni mereni naklonu vratilo stabilni hodnotu, nebo jestli bylo posledni mereni nestabilni

double GeoLon;                                          // souradnice pozorovaciho mista pro astro vypocty polohy Slunce a Mesice
double GeoLat;

int16_t    astro_UTC_min;
int16_t    astro_UTC_rok;
int16_t    astro_UTC_mes;
int16_t    astro_UTC_den;
int16_t    astro_UTC_hod;
int16_t    casova_zona;

// globalni promenne pro vypocty planet
double pla_i; 
double pla_o;
double pla_p; 
double pla_a; 
double pla_e; 
double pla_l; 
double rads = 1.74532925199433E-02;
double pla_tpi = 6.28318530717958;
double degs = 57.2957795130823;
double globra, globde, globvzd;
int16_t index_planety = 1;                              // po zapnuti se index prvni planety v menu nastavi na 1 (Merkur)
uint8_t elevace_soumrak;                                // jak hluboko musi byt Slunce pod horizontem (ve stupnich), aby se hlasil uzivatelsky nastavitelny soumrak


int16_t SIM_hod, SIM_den, SIM_mes, SIM_min;             // Casove promenne pro simulaci vychodu a zapadu planet
uint16_t SIM_rok ;                                    
                                                        // promenne pouzivane pro simulaci slunecni soustavy a pro zobrazeni udaju pres menu "AStro"
int16_t SIM_azim;                                       // na jakem azimutu dojde k nasledujicimu vychodu / zapadu planety
uint8_t SIM_vyza;                                       //   0=planeta behem pristich 24 hodin nevyjde ani nezapadne, 1=pristi bude vychod, 2=pristi bude zapad
int16_t SIM_zamin;                                      // za kolik minut dojde k vychodu nebo zapadu planety
int16_t SIM_aktel;                                      // aktualni elevace planety
int16_t SIM_aktaz;                                      // aktualni azimut planety
int16_t SIM_aktvz;                                      // aktualni vzdalenost planety
int16_t SIM_cas_h;                                      // vypocteny cas udalosti (hodiny)
int16_t SIM_cas_m;                                      // vypocteny cas udalosti (minuty)
double SIM_aktvzd_presne;                               // vzdalenost Mesice nebo planety bez zaokrouhleni pro dalsi prepocet na procenta
int16_t SIM_osvit;                                      // aktualni osvetleni Mesice v %
double R_mag;                                           // magnituda saturnova prstence (nepresny vypocet)
double P_mag_SAT;                                       // magnituda samotne platety Saturn bez prstencu
bool SIM_break;                                         // pro moznost predcasne ukoncit simulaci Slunce a Mesice


int16_t astro_noc_konec;                                // pro hledani casu, kdy nastane astronomicka noc (Slunce -18 stupnu pod horizontem)
int16_t astro_noc_zacatek;
int16_t astro_noc_trvani;

int16_t letni_posun;                                    // casovy posun mistni casove zony proti UTC v lete (SELC/CEST = 2) je ulozeny v EEPROM (489)
int16_t zimni_posun;                                    // casovy posun mistni casove zony proti UTC v zime ( SEC/CET  = 1) je ulozeny v EEPROM (490)

char char_zima[5];                                      // 4 uzivatelsky nastavene znaky pro oznaceni zimni casove zony + '\0'
char char_leto[5];                                      // 4 uzivatelsky nastavene znaky pro oznaceni zimni casove zony + '\0'
char char_Xzima[5];                                     // stejne jako char_zima, akorat prvni znak je nahrazen podtrzitkem
char char_Xleto[5];                                     // stejne jako char_leto, akorat prvni znak je nahrazen podtrzitkem

float  Mes_elevace;
float  Slu_elevace;
double Mes_osvit;
float  Mes_colong;
float  Mes_term;
float  Mes_azimut;
float  Slu_azimut;

double P_faze;                                          // po spusteni podprogramu "vypocty_elongace(y, m, d, h, n, index)" se do techto promennych ukladaji vysledky
double P_elongace; 
double P_vzdalenost;
double P_ra, P_de; 
double S_ra, S_de; 
uint8_t   P_east_elo;                                   // vychodni elongace vnitrni planety (Merkur nebo Venuse) = 1, zapadni elongace = 0, pro ostatni planety = 2 (nema vyznam)
double P_mag;                                           // pro Merkur a Venusi je mozne spocitat i magnitudu

double fazovy_uhel;

double dist_SP, dist_ZS;


int16_t zadana_rektascenze;
int16_t zadana_deklinace;
float obecna_elevace;
float obecny_azimut; 

float offset_kompasu;                                   // kvuli spatne nalepenemu kompasu je mozne v urcitem pasmu poopravit sever (-12,8 az 12,7 stupnu)

int16_t pole_azimutu[20];                               // pole, do ktereho se budou ukladat azimuty k prumerovani
uint8_t velikost_pole_azimutu;                          // pocet zaznamu v poli azimutu, ktere se budou pouzivat k prumerovani (pri beznem mereni to bude stejna hodnota jako 'prumery')
int16_t povoleny_rozptyl_azimutu = 10;                  // hodnota ve stupnich. Konstanta pro vyhodnoceni stabilniho / nestabilniho mereni azimutu
bool problem_azimutu;                                   // pri mereni pres menu se v pripade nestabilnich hodnot signalizuje problem


int16_t lux_citlivost;                                  // pro automaticke urcovani citlivosti cidla v rezimu luxmetr
long lux_klouzak[6];                                    // pro klouzave prumerovani poslednich 5 mereni
uint8_t lux_index_klouzak;                              // ukazatel po pole klouzaku pro luxometr

uint8_t bortle_povoleno;

uint16_t index_naklonu;                                 // znacka, kam se v kazdem zaznamu uklada naklon
uint16_t index_azimutu;                                 // znacka, kam se v kazdem zaznamu uklada azimut

uint8_t casovani30;                                     //!! pro spec rezim (r30s) - casovaci interval
uint16_t adrsave30;                                     //!! pro spec rezim (r30s) - dalsi volna bunka v EEPROM
bool blokuj_znacku30 = false;                           //!! pro spec rezim (r30s) - pri zobrazenem menu nebo nejakych jinych informacich na displeji nezobrazuje znacku na prvni segmentovce
uint32_t posledni_zaznam30;                             //!! pro spec rezim (r30s) - cas posledniho zaznamu v milisekundach od zapnuti
bool pauza30;                                           //!! pro spec rezim (r30s) - docasne blokovani zaznamu
uint16_t max_EEPROM30;                                  //!! pro spec rezim (r30s) - kapacita osazene pameti

bool predchozi_data_RDY;                                // jen pro logovani zapnuti vypinace u GPS modulu

bool blokuj_err_i = true;                               // pro hlaseni az kazde druhe chyby I2C (k prvni neosetritelne chybe dochazi pri zapnuti GPS modulu)
uint32_t  cas_odblokovani_err_i;                        // pro zablokovani zobrazeni prvni chyby I2C po delsi dobe nez je 5 sekund bezchyleho provozu

char read_buffer[300];                                  // pro vypis souboru do seriove linky se sem bude nacitat obsah souboru a prubezne zpracovavat

char helpfile_name[] = "HELP/#_#.txt";                  // pro jmena souboru na SD karte s napovedami pro jednotlive funkce

#include <Adafruit_NeoPixel.h>                          // knihovna pro ovladani programovatelne RGB LED
// #doc#16
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, pin_NEO, NEO_GRB + NEO_KHZ800);          // moznost prohozeni barev na RGB LED (WS2812D)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, pin_NEO, NEO_RGB + NEO_KHZ800);            // moznost prohozeni barev na RGB LED ()


#define SD_CONFIG SdSpiConfig(pin_SD_CS, DEDICATED_SPI, SD_SCK_MHZ(4))
#include "SdFat.h"                                      // nova knihovna pro praci s SD kartou
#include "sdios.h"
SdFat sd;                                               // pro SD kartu
SdFile soubor;
SdFile log_soubor;

uint32_t posledni_SD_free;                              // kazde dve hodiny se aktualizuje informace o volnem prostoru na SD karte


bool sd_out_err = true;                                 // chyba vysunute SD karty "No-Sd" se hlasi jenom 1x pri startu 
uint16_t SD_volno = 0;                                  // volny prostor na SD karte v [MB]. Aktualizace po kazdem zapnuti, nebo vypisu sys_info().
uint16_t SDkapacita = 0;                                // celkova kapacita SD karty v [MB]. Aktualizace po kazdem zapnuti, nebo vypisu sys_info().

bool aktual_odp;                                        // jen kvuli prubezne 5-sekundove aktualizaci odpoctu pri zobrazenem menu "Alarm" a polozce "Odpocet"

uint8_t vyckavaci_kolecko = 1;                          // pri casove narocnych operacich (uvodni zapis do kalibracni tabulky svetla a teploty) zobrazuje na prostredni segmentovce rotujici kolecko

uint8_t index_kodu = 0;                                 // pri odesilani vsech ovladacich kodu pres seriovou linku (prikaz *f) se na zacatek radky zapisuje jeste ciselny index
char    ovladaci_znak = ' ';                            //  pri odesilani vsech ovladacich kodu pres seriovou linku (prikaz *f) se pred kod zapisuje jeste kod pro sadu znaku ('#','@','*','%')

uint16_t RTC_alarm[7] = {0,1440,1440,1440,1440,1440, 1440}; // az 6 pripravenych casu budiku pro pridavne RTC (v minutach od pulnoci - maximalne 23:59 predem) index [0] se nepocita, index 6 je pro odpocet
uint32_t posledni_time_synchro;                         // cas v DS3231 se 1 za hodinu synchroniizuje podle RTC v STM32F411. V teto promenne je ulozeny cas,kdy k posledni synchronizaci doslo.
uint8_t ds_min;
uint8_t ds_hod;
uint8_t ds_den;
uint8_t ds_mes;
uint8_t ds_rok;
uint8_t ds_sek;

bool blokuj_3231 = false;                               // Pro pripad nejake poruchy DS3231 by se mohla pres I2C neustale ziskavat informace o nahozenem AF.
                                                        //    To by zpusobilo nemoznost ovladani SQM, protoze by se stale hlasil ALARM.
                                                        //    Stiskem vsech 3 tlacitek najednou pri probihajicim alarmu se az do dalsiho resetu bude Alarm Flag ignorovat.

bool kalibruje_se = false;                              // pri kalibraci je mozne okamzite prerusit mereni svetla (nemusi se cekat na uvolneni tlacitek)

bool trackuj = false;                                   // znacka pro spousteni GPS trasovani. Na konci setupu se nastavuje na posledni stav podle EEPROM
uint16_t track_interval;                                // interval v sekundach pro zaznamenavani tracku se bere z EEPROM, ale neni bezne uzivatelsky nastavitelny
uint32_t posledni_zaznam_tracku;                        // cas posledniho zaznamu tracku (pro odpocet nastaveneho intervalu)
bool track_blik_LED;                                    // kdyz je zapnute trasovani, prepina se pri kazdem interruptu stav signalizacni LED
char GPS_track_cesta[] = "TRCK/trck1234.gpx";           // retezec pro vytvoreni cesty k trasovacimu souboru (cislo se pri kazdem spusteni trasovani zvetsuje a zaznamenava do EEPROM)

char cesta[] = "CALB/123-1025.csv";                     // retezec pro konstrukci cele cesty k souboru pro funkce kalibratoru    



//-----------------  globalni promenne pro urcovani viditelnosti DSO objektu
int16_t   DSO_index_podmenu = 0;                        // pamatovak pro posledni pouzitou polozku podmenu DSO, aby bylo kam se vracet kratkym stiskem OK (po resetu 0=Messierovky)

uint16_t  DSO_posledni_messier = 1;                     // pro funkci menu "DSO": pamet pro posledni zvoleny index DSO z katalogu Messierovek (nula neni pouzita)
uint16_t  DSO_posledni_ngc = 0;                         // pro funkci menu "DSO": pamet pro posledni zvoleny index DSO z katalogu NGC
uint16_t  DSO_posledni_ic  = 0;                         // pro funkci menu "DSO": pamet pro posledni zvoleny index DSO z katalogu IC
uint8_t   DSO_index_prumeru;                            // pro funkci menu "DSO": nastaveni indexu prumeru objektivu (odkaz do tabulky 'DSO_pole_prumery_dalekohledu[]') - uklada se do EEPROM
uint8_t   DSO_index_ohniska_dalekohledu;                // pro funkci menu "DSO": nastaveni indexu delky hlavniho ohniska (odkaz do tabulky 'DSO_pole_delky_dalekohledu[]') - uklada se do EEPROM
uint8_t   DSO_index_ohniska_okularu = 0;                // pro funkci menu "DSO": nastaveni ohniska okularu  (odkaz do tabulky 'DSO_pole_ohniska_okularu[]') - startuje se vzdycky prvnim typem (index 0) - neuklada se do EEPROM

uint16_t  DSO_pole_prumery_dalekohledu[3];              // prevodni tabulka mezi indexem prumeru dalekohledu [0] az [2] a skutecnym prumerem v [mm]
uint16_t  DSO_pole_delky_dalekohledu[3];                // prevodni tabulka mezi indexem hlavni ohniskove vzdalenosti dalekohledu [0] az [2] a skutecnou vzdalenosti v [mm]
uint8_t   DSO_pole_ohniska_okularu[5];                  // prevodni tabulka mezi indexem ohnoskove vzdalenosti okularu [0] az [5] a skutecnym ohnoskem v [mm]

float     DSO_zornice_mm = 7.0;                         // konstanta pro vypocet dalekohledu - zornice oka v mm (typicky 7 mm ve tme)
float     DSO_zvetseni = 0.0;                           // Vypocty pro dalekohled: zvetseni (pomer ohniska dalekohledu a okularu)
float     DSO_vystupni_pupila = 0.0;                    // Vypocty pro dalekohled: vystupni pupila
float     DSO_delta_SB = 0.0;                           // Vypocty pro dalekohled: zmena povrchove jasnosti v mag/arcsec^2 pro nastavene parametry dalekohledu

uint16_t  DSO_prumer_dalekohledu;                       // Posledni zvoleny prumer dalekohledu v mm
uint16_t  DSO_delka_dalekohledu;                        // Posledni zvolena ohniskova delka dalekohledu v mm
uint8_t   DSO_delka_okularu;                            // Posledni zvolena ohniskova delka okularu v mm
uint8_t   DSO_schopnosti;                               // Nastaveni stupne schopnosti pozorovatele - uklada se do EEPROM
float     DSO_pozorovatel;                              // stupen schopnosti pozorovatele prevedeny na pokles nebo prirustek magnitudy: DSO_pozorovatel = (DSO_schopnosti - 3) * 0.3
uint16_t  DSO_index_DB = 0;                             // index zvoleneho DSO objektu z celkove databaze DSO objektu 'pole_DSO[]'
uint16_t  DSO_MSA;                                      // zmerena hodnota jasu pozadi ve funkci zjistovani viditelnosti DSO v tisicinach mag/arcsek2

uint8_t   DSO_excentricita;                             // setiny excentricity. ani po zaokrouhleni nesmi byt kvuli zobrazeni na displeji vetsi nez 99
uint8_t   DSO_stupen_plochy;                            // plocha objektu (pocitano jako elipsa s velkou a malou osou z databaze) prevedena na 1 z 5 stupnu (1=mala plocha az 5=velka plocha)

float     DSO_individualni_korekce;                     // priprava na drobne doladeni viditelnosti hvezdokup pro pripad, ze se nachazi na hustem hvezdnem pozadi

uint8_t   DSO_viditelnost;                              // vysledna hodnota vypoctene viditelnosti DSO objektu (0 az 6) na zaklade parametru dalekohledu, jasu pozadi, schopnosti pozorovatele a jasu objektu
                                                        //  0 = neviditelny DSO z duvodu velkeho jasu pozadi, 6 = neviditelny DSO z duvodu jeho polohy pod horizontem
uint8_t   DSO_normal;                                   // normalovy jas oblohy pro urcovani viditelnosti otevrenych hvezdokup v desetinach mag/arcsek2 (217 = 21.7 mag/arcsek2)

float     DSO_extinkce;                                 // zjednoduseny prevod elevace objektu na tloustku atmosfery
float     DSO_plocha;                                   // plocha elipsoviteho DSO objektu spoctena pomoci hlavni a vedlejsi osy [uhlove minuty]

//-----------------

//======================================================================================================================
// prototypy funkci   
//              ctags -x -f vystup.txt --c++-kinds=f --fields=+n --output-format=u-ctags --language-force=C++ *.ino
void     AUTO_hlavicka(void);                                                                                                          // "ser_kom.ino"
void     AUTOdata_RS485(void);                                                                                                         // "rs485.ino"
void     AUTOdata_USB(void);                                                                                                           // "ser_kom.ino"
void     AUTOdata_tabulka(void);                                                                                                       // "ser_kom.ino"
void     BEEP_info(void);                                                                                                              // "stm411.ino"
uint16_t DSO_rd_data(uint8_t start_pozice);                                                                                            // "ser_kom.ino"
uint8_t  EEPROM_read(uint32_t addr);                                                                                                   // "eeprom.ino"
uint8_t  EEPROM_read30(uint16_t addr);                                                                                                 // "rezim30s.ino"
uint32_t EEPROM_read_halflong30(uint16_t adresa);                                                                                      // "rezim30s.ino"
uint16_t EEPROM_read_int(uint32_t adresa);                                                                                             // "eeprom.ino"
uint16_t EEPROM_read_int30(uint16_t adresa);                                                                                           // "rezim30s.ino"
uint32_t EEPROM_read_long(uint32_t adresa);                                                                                            // "eeprom.ino"
uint32_t EEPROM_read_long30(uint16_t adresa);                                                                                          // "rezim30s.ino"
void     EEPROM_to_MOD(uint16_t index_zaznamu);                                                                                        // "rs485.ino"
void     EEPROM_to_MOD(void);                                                                                                          // "rs485.ino"
void     EEPROM_to_ser_out(uint16_t index_zaznamu);                                                                                    // "ser_kom.ino"
void     EEPROM_update(uint32_t addr, uint8_t wrdata);                                                                                 // "eeprom.ino"
void     EEPROM_write(uint32_t adresa, uint8_t wrdata);                                                                                // "eeprom.ino"
void     EEPROM_write30(uint16_t adresa, uint8_t wr30data);                                                                            // "rezim30s.ino"
void     EEPROM_write_int(uint32_t adresa, uint16_t wrdata);                                                                           // "eeprom.ino"
void     EEPROM_write_long(uint32_t adresa, uint32_t wrdata);                                                                          // "eeprom.ino"
bool     EXPA_test(void);                                                                                                              // "expander.ino"
void     EXPA_write(uint8_t registr, uint8_t exdata);                                                                                  // "expander.ino"
void     GET_I2C_NMEA(uint8_t blok);                                                                                                   // "gps.ino"
void     GPS_nastav_cas(uint8_t RTC_param);                                                                                            // "gps.ino"
void     HW_test(bool resetuj);                                                                                                        // "test.ino"
bool     KAL_test(void);                                                                                                               // "autokalibrace.ino"
void     LED_B_onoff(bool onoff);                                                                                                      // "led.ino"
void     LED_C_onoff(bool onoff);                                                                                                      // "led.ino"
void     LED_G_onoff(bool onoff);                                                                                                      // "led.ino"
void     LED_R_onoff(bool onoff);                                                                                                      // "led.ino"
void     LED_W(bool onoff);                                                                                                            // "led.ino"
void     LED_Y_onoff(bool onoff);                                                                                                      // "led.ino"
void     PC_funkce(void);                                                                                                              // "ser_kom.ino"
void     RGB_LED_info(void);                                                                                                           // "stm411.ino"
uint8_t  RTC3231_read(uint8_t registr);                                                                                                // "ds3231.ino"
void     RTC3231_write(uint8_t registr, uint8_t hodnota);                                                                              // "ds3231.ino"
void     RTC_SD(uint32_t novy, uint32_t v_rtc, long odchylka, uint32_t interval, uint32_t log_korekce , uint16_t logznacka);           // "sd_karta.ino"
void     RTC_dump(void);                                                                                                               // "ds3231.ino"
void     RTC_info(void);                                                                                                               // "rtc.ino"
void     RaToAzim( uint8_t teleso , double qRA, double qDECL, int16_t qrok, uint8_t qmes, uint8_t qden, uint8_t qhod, uint8_t qmin , int16_t qutcdif, double qlong, double qlat); // "astro.ino"
void     SD_log(uint32_t adresa , uint16_t logdata);                                                                                   // "sd_karta.ino"
void     SD_save(void);                                                                                                                // "sd_karta.ino"
void     SD_save30(bool autozaloha , bool ser_print);                                                                                  // "rezim30s.ino"
void     SD_sysMEM_READ(void);                                                                                                         // "sd_karta.ino"
void     SD_sysMEM_WRITE(void);                                                                                                        // "sd_karta.ino"
void     SIM_minus_den(void);                                                                                                          // "astro.ino"
void     SIM_plus_den(void);                                                                                                           // "astro.ino"
void     SIM_plus_minuta(void);                                                                                                        // "astro.ino"
void     SOFT_RESET(void);                                                                                                             // "stm411.ino"
void     STM_DS(bool AFtest);                                                                                                          // "ds3231.ino"
float    TSL_calculateLux(uint8_t conf, uint16_t ch0, uint16_t ch1);                                                                   // "svetlo.ino"
void     TSL_config(uint8_t confdata);                                                                                                 // "svetlo.ino"
void     TSL_disable(void);                                                                                                            // "svetlo.ino"
void     TSL_enable(void);                                                                                                             // "svetlo.ino"
uint8_t  TSL_read(uint8_t registr);                                                                                                    // "svetlo.ino"
uint16_t TSL_read_int(uint8_t registr);                                                                                                // "svetlo.ino"
uint16_t TSL_read_int_old(uint8_t registr);                                                                                            // "svetlo.ino"
void     TSL_write(uint8_t registr, uint8_t tsldata);                                                                                  // "svetlo.ino"
void     TS_zamek(bool zamknout);                                                                                                      // "timestamp.ino"
void     aktualizuj_displej(uint8_t vstup_jas);                                                                                        // "displej.ino"
void     aktualizuj_displej(void);                                                                                                     // "displej.ino"
void     alarm_3231(void);                                                                                                             // "ds3231.ino"
void     alarm_bat(uint8_t stupen);                                                                                                    // "stm411.ino"
void     astro_noc(uint8_t hranicni_elevace);                                                                                          // "astro.ino"
void     astro_vypocty(void);                                                                                                          // "astro.ino"
void     bezkorekcni_funkce(void) ;                                                                                                    // "ser_kom.ino"
void     blik_B(void);                                                                                                                 // "led.ino"
void     blik_C(void);                                                                                                                 // "led.ino"
void     blik_C_kratky(void);                                                                                                          // "led.ino"
void     blik_F_fast(void);                                                                                                            // "led.ino"
void     blik_F_kratky(void);                                                                                                          // "led.ino"
void     blik_G(void);                                                                                                                 // "led.ino"
void     blik_G_fast(void);                                                                                                            // "led.ino"
void     blik_R(void);                                                                                                                 // "led.ino"
void     blik_Y(void);                                                                                                                 // "led.ino"
void     blik_Y_kratky(void);                                                                                                          // "led.ino"
void     blikblik_G (uint8_t pocet);                                                                                                   // "led.ino"
uint8_t  bortle(uint16_t vstupni_svetlo);                                                                                              // "stm411.ino"
void     breakTimeFCE(time_t timeInput, tm_t & tmm);                                                                                   // "rtc.ino"
long     buf_to_long(uint8_t zacatek_cisla,uint8_t tecka);                                                                             // "sd_karta.ino"
void     casove_razitko(void);                                                                                                         // "timestamp.ino"
void     chyba(uint8_t kod_chyby);                                                                                                     // "stm411.ino"
void     cist_kalibrator(void);                                                                                                        // "autokalibrace.ino"
void     cist_proudove_konstanty(uint8_t index_pole);                                                                                  // "autokalibrace.ino"
float    citelna_teplota(uint16_t necitelna_teplota);                                                                                  // "bme280.ino"
float    citelna_vlhkost(uint16_t necitelna_vlhkost);                                                                                  // "bme280.ino"
float    citelne_svetlo(uint16_t necitelne_svetlo);                                                                                    // "svetlo.ino"
uint32_t citelny_tlak(uint16_t necitelny_tlak);                                                                                        // "bme280.ino"
void     cti_ds3231(void);                                                                                                             // "ds3231.ino"
void     cti_napajeni(void);                                                                                                           // "stm411.ino"
void     cyk_interrupt(void);                                                                                                          // "test.ino"
void     d1_simulace_SM(int16_t ss_rok, int16_t ss_mes, int16_t ss_den);                                                               // "astro.ino"
double   dacos(double x);                                                                                                              // "astro.ino"
void     dalsi_budik(void);                                                                                                            // "ds3231.ino"
double   dasin(double x);                                                                                                              // "astro.ino"
double   datan(double x);                                                                                                              // "astro.ino"
double   datan2(double y, double x);                                                                                                   // "astro.ino"
void     dateTime(uint16_t* date, uint16_t* time);                                                                                     // "sd_karta.ino"
void     datum_do_csv(void);                                                                                                           // "autokalibrace.ino"
double   day2000(int16_t y, int16_t m, int16_t d, double h);                                                                           // "astro.ino"
double   dcos(double x);                                                                                                               // "astro.ino"
void     default_kalibrace_svetla(void);                                                                                               // "stm411.ino"
void     default_kalibrace_teploty(void);                                                                                              // "stm411.ino"
void     default_kompas(void);                                                                                                         // "kompas.ino"
void     default_naklonomer(void);                                                                                                     // "kompas.ino"
void     default_parametry(void);                                                                                                      // "stm411.ino"
void     default_stanoviste(void);                                                                                                     // "stm411.ino"
void     deleni(uint16_t pozice);                                                                                                      // "konverze.ino"
bool     disp_zaznam_scroll(uint8_t pocet_znaku);                                                                                      // "menu_dis.ino"
void     dopln_mezery(int16_t cislo, uint8_t pocet_mist);                                                                              // "stm411.ino"
double   dsin(double x);                                                                                                               // "astro.ino"
double   dtan(double x);                                                                                                               // "astro.ino"
void     editace_periferii(void);                                                                                                      // "menu_dis.ino"
void     editace_polozek_menu(void);                                                                                                   // "menu_dis.ino"
void     element(double d, uint8_t pnum);                                                                                              // "astro.ino"
void     expa_switch(uint8_t cislo);                                                                                                   // "expander.ino"
void     file_to_serkom(char typ, uint16_t param2, uint16_t param3);                                                                   // "sd_karta.ino"
void     formatuj_ds_cas(void);                                                                                                        // "ds3231.ino"
void     formatuj_float(float cislo , uint16_t index_znamenka, uint16_t index_jednotek, uint8_t pocet_celych_mist , uint8_t pocet_desetin);    // "konverze.ino"
void     formatuj_float30(float cislo , uint8_t index_znamenka, uint8_t index_jednotek, uint8_t pocet_celych_mist , uint8_t pocet_desetin);    // "rezim30s.ino"
void     formatuj_int(int16_t cislo , uint16_t index_znamenka , bool zobrazit_znamenko);                                               // "konverze.ino"
void     formatuj_string(String retezec , uint16_t index_zacatek, uint8_t pocet_znaku);                                                // "konverze.ino"
void     formatuj_string30(String retezec , uint8_t index_zacatek, uint8_t pocet_znaku);                                               // "rezim30s.ino"
void     formatuj_ulong(uint32_t cislo , uint16_t start_jednotek, uint8_t pocet_mist);                                                 // "konverze.ino"
void     formatuj_ulong30(uint32_t cislo , uint8_t start_jednotek, uint8_t pocet_mist);                                                // "rezim30s.ino"
void     gps(uint8_t RTC_param);                                                                                                       // "gps.ino"
void     gps_NMEA(uint8_t typ_zpravy);                                                                                                 // "gps.ino"
void     graf_natazeni(uint32_t vstupni_cislo , uint32_t delitel);                                                                     // "displej.ino"
void     hard_format_EEPROM(void);                                                                                                     // "stm411.ino"
void     helpfile(void);                                                                                                               // "sd_karta.ino"
void     hlaseni_chyb(void);                                                                                                           // "stm411.ino"
void     ho_mi(void);                                                                                                                  // "rtc.ino"
void     interDN(void);                                                                                                                // "test.ino"
void     interOK(void);                                                                                                                // "test.ino"
void     interTS(void);                                                                                                                // "test.ino"
void     interUP(void);                                                                                                                // "test.ino"
void     interrupty(void);                                                                                                             // "stm411.ino"
double   ipart(double x);                                                                                                              // "astro.ino"
void     kal_postupy(void);                                                                                                            // "autokalibrace.ino"
void     kalibrace_magnet(bool serlinka);                                                                                              // "kompas.ino"
void     kalibrace_naklon(void);                                                                                                       // "kompas.ino"
double   kepler(double m, double ecc, double eps);                                                                                     // "astro.ino"
void     kolecko(void);                                                                                                                // "displej.ino"
void     kompas_setup(void);                                                                                                           // "kompas.ino"
void     konec_souboru(void);                                                                                                          // "autokalibrace.ino"
uint8_t  kontrolni_bajt(uint8_t pocet_prvku,uint8_t kontrolni_pole[]);                                                                 // "rs485.ino"
int16_t  korekce_svetla(int16_t zmerene_svetlo);                                                                                       // "svetlo.ino"
uint16_t korekce_teploty(uint16_t zmerena_teplota);                                                                                    // "svetlo.ino"
void     logafd(uint32_t logovane_cislo);                                                                                              // "sd_karta.ino"
void     logjas(uint16_t logovane_cislo);                                                                                              // "sd_karta.ino"
void     loguj_rozdily_casu(void);                                                                                                     // "ds3231.ino"
void     long32_to_HEX(uint32_t cislo, uint8_t start_pozice);                                                                          // "stm411.ino"
void     loop(void);                                                                                                                   // "stm411.ino"
float    lux_kalibrace(float vstup);                                                                                                   // "svetlo.ino"
float    luxometr_1x(void);                                                                                                            // "svetlo.ino"
void     magnet_write(uint8_t registr, uint8_t magdata);                                                                               // "kompas.ino"
void     magnetometr_info(bool test);                                                                                                  // "kompas.ino"
time_t   makeTimeFCE(tm_t & tmm);                                                                                                      // "rtc.ino"
float    mapfloat(float x, float in_min, float in_max, float out_min, float out_max);                                                  // "kompas.ino"
void     menu_1(void);                                                                                                                 // "menu_dis.ino"
void     menu_LED(void);                                                                                                               // "menu_LED.ino"
void     menu_stanoviste(void);                                                                                                        // "menu_dis.ino"
void     mereni(uint8_t param);                                                                                                        // "mereni.ino"
void     mi_se(void);                                                                                                                  // "rtc.ino"
void     min_na_hm (int16_t pozice_dhod, int16_t vstupni_minuty);                                                                      // "astro.ino"
int16_t  minus_hodnota(char typ_odecitani , int16_t hodnota, int16_t minimum, int16_t maximum);                                        // "menu_dis.ino"
void     msleep(int16_t n);                                                                                                            // "stm411.ino"
void     nacti_3D_magnet(void);                                                                                                        // "kompas.ino"
void     nacti_3D_zrychleni(void);                                                                                                     // "kompas.ino"
uint32_t najdi_prazdne_misto(uint8_t start_hledani);                                                                                   // "eeprom.ino"
uint16_t najdi_volne_misto30(void);                                                                                                    // "rezim30s.ino"
void     naklon_setup(void);                                                                                                           // "kompas.ino"
void     naklon_write(uint8_t registr, uint8_t nakdata);                                                                               // "kompas.ino"
void     naklonomer_info(bool test);                                                                                                   // "kompas.ino"
void     nastav_HC(void);                                                                                                              // "gps.ino"
void     nastav_RTC(void);                                                                                                             // "rtc.ino"
void     nastav_cestu_tracku(void);                                                                                                    // "sd_karta.ino"
void     nastav_rtc(void);                                                                                                             // "test.ino"
void     nastav_uroven_jasu_LED(uint8_t kal_kanal, uint8_t kal_uroven);                                                                // "autokalibrace.ino"
void     nastaveni_oddelovacu(void);                                                                                                   // "ser_kom.ino"
void     oddeleni_polozek(void);                                                                                                       // "konverze.ino"
void     odesli_pole(uint8_t pocet);                                                                                                   // "rs485.ino"
void     odposlech_GPS(void);                                                                                                          // "test.ino"
void     odskok_int(void);                                                                                                             // "rezim30s.ino"
void     planet(double d, int16_t pnumber);                                                                                            // "astro.ino"
int16_t  plus_hodnota(char typ_pricitani , int16_t hodnota, int16_t minimum , int16_t maximum);                                        // "menu_dis.ino"
int16_t  plusminus(char znak1, int16_t start_hodnota, int16_t minimalni_hodnota, int16_t maximalni_hodnota);                           // "menu_dis.ino"
void     pocitej_viditelnost(void);                                                                                                    // "dso_db.ino"
void     podmenu(uint8_t polozka);                                                                                                     // "menu_dis.ino"
void     podmenu_LED(uint8_t pozice);                                                                                                  // "menu_LED.ino"
void     pomlcky(uint8_t pocet_pomlcek);                                                                                               // "stm411.ino"
void     posli_kod_f(char kod_znaku);                                                                                                  // "ser_kom.ino"
uint16_t prednuly(uint16_t vstupni_cislo, uint8_t pocet_mist);                                                                         // "test.ino"
int32_t  prednuly_v2(int32_t vstupni_cislo, uint8_t pocet_mist);                                                                       // "test.ino"
void     prepni_cidlo_svetla(uint8_t index);                                                                                           // "stm411.ino"
void     prepni_cidlo_svetla(void);                                                                                                    // "stm411.ino"
uint16_t pridej_minuty(uint16_t plusminuty);                                                                                           // "ds3231.ino"
uint16_t prijato_int( uint8_t znak_start, uint8_t znak_stop);                                                                          // "sd_karta.ino"
void     prijem(void);                                                                                                                 // "rs485.ino"
void     prikaz_kalibrator(uint8_t kalib_prikaz, uint16_t kalib_parametr);                                                             // "autokalibrace.ino"
void     print_co_kam(bool co, bool kam);                                                                                              // "stm411.ino"
void     pripoj_tecku(void);                                                                                                           // "displej.ino"
void     priprav_1_zaznam(uint32_t adresa);                                                                                            // "konverze.ino"
void     priprav_radku(uint16_t adr_eeprom);                                                                                           // "rezim30s.ino"
void     priprava_EEPROM_stopky(uint8_t fce, uint32_t cas_data);                                                                       // "stopky.ino"
void     priprava_pole_EEPROM(void);                                                                                                   // "stm411.ino"
void     priprava_rotujiciho_retezce(uint32_t souradnice, uint8_t smer_osy);                                                           // "gps.ino"
uint16_t prumeruj_azimuty(void) ;                                                                                                      // "kompas.ino"
void     ra_dec_slunce(double t);                                                                                                      // "astro.ino"
double   range2pi(double pla_X);                                                                                                       // "astro.ino"
void     reset_AF(void);                                                                                                               // "ds3231.ino"
uint16_t rezervni_cidlo_1(void);                                                                                                       // "rezervy.ino"
uint16_t rezervni_cidlo_1(void);                                                                                                       // "rezervy.ino"
uint16_t rezervni_cidlo_2(void);                                                                                                       // "rezervy.ino"
uint16_t rezervni_cidlo_2(void);                                                                                                       // "rezervy.ino"
uint16_t rezervni_cidlo_3(void);                                                                                                       // "rezervy.ino"
uint16_t rezervni_cidlo_3(void);                                                                                                       // "rezervy.ino"
uint16_t rezervni_cidlo_4(void);                                                                                                       // "rezervy.ino"
uint16_t rezervni_cidlo_4(void);                                                                                                       // "rezervy.ino"
void     rezim30s(void);                                                                                                               // "rezim30s.ino"
uint16_t rozdil_jasu(uint16_t jas_A, uint16_t jas_B);                                                                                  // "svetlo.ino"
bool     rozdily_v_poli(void);                                                                                                         // "kompas.ino"
double   rozsah(double x);                                                                                                             // "astro.ino"
uint32_t rtc_korekce(void);                                                                                                            // "rtc.ino"
uint16_t rychly_naklon(void);                                                                                                          // "kompas.ino"
uint64_t SDfree(SdFat& sd);                                                                                                            // "sd_karta.ino"
void     sek1970_datcas(uint32_t parametr, bool seriovy_vystup);                                                                       // "rtc.ino"
void     sek1970_datcas(uint32_t parametr, bool seriovy_vystup, bool secselc);                                                         // "rtc.ino"
void     ser_kom30(void);                                                                                                              // "rezim30s.ino"
void     ser_napoveda0(void);                                                                                                          // "ser_kom.ino"
void     ser_napoveda1(void);                                                                                                          // "ser_kom.ino"
void     ser_napoveda2(void);                                                                                                          // "ser_kom.ino"
void     ser_napoveda3(void);                                                                                                          // "ser_kom.ino"
void     ser_napoveda4(void);                                                                                                          // "rezim30s.ino"
void     ser_napoveda5(void);                                                                                                          // "ser_kom.ino"
void     seriovy_vypis(uint16_t hranice);                                                                                              // "rezim30s.ino"
void     servisni_funkce(void);                                                                                                        // "ser_kom.ino"
void     setup(void);                                                                                                                  // "stm411.ino"
void     sim_SM( uint16_t ss_pocet_kroku , uint16_t ss_den, uint16_t ss_mes , uint16_t ss_rok);                                        // "astro.ino"
void     sim_SS( uint16_t ss_planeta, uint16_t ss_pocet_kroku , uint16_t ss_den, uint16_t ss_mes , uint16_t ss_rok);                   // "astro.ino"
void     simulace(uint8_t ss_planeta, uint16_t ss_pocet_kroku , uint16_t ss_den, uint16_t ss_mes , uint16_t ss_rok);                   // "astro.ino"
void     simulace_planet(uint8_t sim_planeta);                                                                                         // "astro.ino"
void     sken_i2c(void);                                                                                                               // "test.ino"
void     soft_format_EEPROM(void);                                                                                                     // "stm411.ino"
void     spec_dis_menu(uint8_t polozka);                                                                                               // "rezim30s.ino"
void     spec_funkce(void);                                                                                                            // "ser_kom.ino"
void     spolecne_DSO_vypocty(void);                                                                                                   // "dso_db.ino"
int32_t  square(int16_t vstup);                                                                                                        // "kompas.ino"
void     srovnej_pod_sebe(int16_t vstup, uint8_t sirka);                                                                               // "kompas.ino"
void     stopky(void);                                                                                                                 // "stopky.ino"
uint16_t svetlo_1x(bool mozno_prerusit, bool bzuk);                                                                                    // "svetlo.ino"
void     svitilna(void);                                                                                                               // "stm411.ino"
void     sys_info(void);                                                                                                               // "stm411.ino"
void     tajne_funkce(void);                                                                                                           // "ser_kom.ino"
void     teleread(void);                                                                                                               // "sd_karta.ino"
uint16_t teplota(bool korekce);                                                                                                        // "bme280.ino"
void     test_485(void);                                                                                                               // "test.ino"
void     test_485_kom(void);                                                                                                           // "rs485.ino"
void     test_485_mod(void);                                                                                                           // "rs485.ino"
uint16_t test_AFD(bool faze);                                                                                                          // "svetlo.ino"
void     test_DO(bool stav);                                                                                                           // "test.ino"
void     test_DS3231(bool stav);                                                                                                       // "test.ino"
void     test_GPS(void);                                                                                                               // "test.ino"
void     test_SD(void);                                                                                                                // "test.ino"
void     test_adc(void);                                                                                                               // "test.ino"
void     test_baterie(void);                                                                                                           // "stm411.ino"
void     test_blik_B(uint8_t stupen_jasu);                                                                                             // "test.ino"
void     test_blik_G(uint8_t stupen_jasu);                                                                                             // "test.ino"
void     test_blik_R(uint8_t stupen_jasu);                                                                                             // "test.ino"
void     test_bme(void);                                                                                                               // "test.ino"
bool     test_datumu(uint16_t test_den , uint16_t test_mes , uint16_t test_rok);                                                       // "rtc.ino"
void     test_displej(void);                                                                                                           // "test.ino"
void     test_eeprom(void);                                                                                                            // "test.ino"
void     test_eeprom30(void);                                                                                                          // "test.ino"
void     test_help(void);                                                                                                              // "test.ino"
void     test_kompas_lsm303(void);                                                                                                     // "test.ino"
void     test_kont(void);                                                                                                              // "test.ino"
void     test_korekcni_tabulky_svetla(void);                                                                                           // "svetlo.ino"
void     test_korekcni_tabulky_teploty(void);                                                                                          // "svetlo.ino"
void     test_led(void);                                                                                                               // "test.ino"
void     test_nulovani_citace(void);                                                                                                   // "timestamp.ino"
void     test_piskak(void);                                                                                                            // "test.ino"
void     test_rtc(void);                                                                                                               // "test.ino"
void     test_ser_kom(void);                                                                                                           // "ser_kom.ino"
bool     test_stability(bool bzuk, uint8_t bliktecka);                                                                                 // "svetlo.ino"
bool     test_stisku(void);                                                                                                            // "ds3231.ino"
void     test_text(uint8_t polozka);                                                                                                   // "test.ino"
void     test_tsl(void);                                                                                                               // "test.ino"
uint8_t  test_zasunuti(void);                                                                                                          // "eeprom.ino"
void     tisk_1_radky_csv(uint8_t i_profil);                                                                                           // "autokalibrace.ino"
uint16_t tlak(void);                                                                                                                   // "bme280.ino"
void     tone_X(uint8_t tone_pin, int16_t tone_frq, int16_t tone_dur, int16_t skupina);                                                // "stm411.ino"
void     track_GPS(void);                                                                                                              // "sd_karta.ino"
uint16_t uhel(void);                                                                                                                   // "kompas.ino"
float    uhlova_vzdalenost(int16_t azimut1, int16_t elevace1, int16_t azimut2, int16_t elevace2);                                      // "astro.ino"
void     ukonci_GPX_soubor(void);                                                                                                      // "sd_karta.ino"
uint16_t vlhkost(void);                                                                                                                // "bme280.ino"
void     vodovaha_hor(void);                                                                                                           // "kompas.ino"
void     vodovaha_zen(void);                                                                                                           // "kompas.ino"
void     vstup_kalibrace_svetla(void);                                                                                                 // "ser_kom.ino"
void     vstup_kalibrace_teploty(void);                                                                                                // "ser_kom.ino"
void     vyblikej_cislo(long cislo);                                                                                                   // "led.ino"
void     vypis_EEPROM(uint16_t parametr, uint16_t casova_hloubka);                                                                     // "stm411.ino"
void     vypis_EEPROM(uint16_t parametr, uint16_t casova_hloubka, uint16_t dat);                                                       // "stm411.ino"
void     vypis_LU(void);                                                                                                               // "stm411.ino"
uint8_t  vypis_budiku(uint8_t funkce);                                                                                                 // "ds3231.ino"
void     vypisove_funkce(void);                                                                                                        // "ser_kom.ino"
uint8_t  vypocet_GLX(void);                                                                                                            // "dso_db.ino"
uint8_t  vypocet_OC(void);                                                                                                             // "dso_db.ino"
void     vypocty_elongace(int16_t plarok, int16_t plamesic, int16_t pladen, int16_t plahodina, int16_t plaminuta, uint8_t plaindex);   // "astro.ino"
void     vysilani(void);                                                                                                               // "rs485.ino"
double   xday2000(int16_t d2000_rok, int16_t d2000_mes, int16_t d2000_den, int16_t d2000_hod, int16_t d2000_min);                      // "astro.ino"
void     z_LOC_na_Astro_UTC(int16_t offset)    ;                                                                                       // "astro.ino"
void     zaloz_GPX_soubor(void);                                                                                                       // "sd_karta.ino"
void     zalozeni_souboru(void);                                                                                                       // "autokalibrace.ino"
int16_t  zaokrouhlit(double n);                                                                                                        // "astro.ino"
void     zapis_data(uint8_t disdata);                                                                                                  // "displej.ino"
void     zaznam_EEPROM(void);                                                                                                          // "eeprom.ino"
uint16_t zbyvajici_minuty(uint16_t cilove_minuty);                                                                                     // "ds3231.ino"
int16_t  zjisti_azimut(void);                                                                                                          // "kompas.ino"
void     zmer_lux(bool prepocitavat , bool infra_par);                                                                                 // "svetlo.ino"
void     zobraz_RTC(bool serkom);                                                                                                      // "rtc.ino"
void     zobraz_RTC(bool serkom, bool secselc);                                                                                        // "rtc.ino"
void     zobraz_cislo(long cislo, uint8_t styl);                                                                                       // "displej.ino"
void     zobraz_detail_dso(uint8_t index_menu_det);                                                                                    // "dso_db.ino"
void     zobraz_ho_mi(uint16_t pocet_minut, uint8_t poradi_budiku);                                                                    // "displej.ino"
void     zobraz_info_viditelnost(void);                                                                                                // "dso_db.ino"
void     zobraz_odpocmi(uint16_t pocet_minut_od_pulnoci);                                                                              // "displej.ino"
void     zobraz_stopky(uint16_t cislo_na_displej);                                                                                     // "stopky.ino"
void     zobraz_text(uint8_t polozka);                                                                                                 // "displej.ino"
void     zobraz_tvar(uint8_t plocha , uint8_t elipsa);                                                                                 // "dso_db.ino"
void     zobraz_vyzapla(uint8_t typ);                                                                                                  // "displej.ino"
void     zobraz_zvolene_dso(uint8_t katalog, uint16_t index_kat);                                                                      // "dso_db.ino"
void     zpracuj_pozadavek(void);                                                                                                      // "rs485.ino"



//======================================================================================================================
void setup(void)
  {
  
    pinMode(pin_SD_CS, OUTPUT);

    pinMode(pin_tl_up, INPUT_PULLUP);
    pinMode(pin_tl_dn, INPUT_PULLUP);
    pinMode(pin_tl_ok, INPUT_PULLUP);

    if (osazeno_gps == true)   pinMode(pin_DATA_RDY, INPUT);                        // pri pouziti vnitrni GPS se pin pri vypnute GPS pridrzuje odporem v LOW
    else                       pinMode(pin_DATA_RDY, INPUT_PULLUP);                 // pri pouziti vnejsi GPS se nezapojeny pin pridrzuje odporem v HIGH

    pinMode(pin_bzuk,OUTPUT);

    pinMode(pin_dis_CLK, OUTPUT);
    pinMode(pin_dis_DIN, OUTPUT);
    pinMode(pin_DERE, OUTPUT);


    digitalWrite(pin_dis_CLK , LOW);
    digitalWrite(pin_dis_DIN , LOW);


    pinMode(pin_karta_IN, INPUT_PULLUP);
    pinMode(pin_AD_BAT  , INPUT_ANALOG);
    pinMode(pin_TEST_HW , INPUT_PULLDOWN);               // pin pro specialni testy urovni napajeciho napeti (kdyz nesmi byt zapojena komunikace)
    pinMode(pin_refu    , INPUT_ANALOG);
    pinMode(pin_TS      , INPUT_PULLUP);

    pinMode(pin_bok_out,OUTPUT);                         // pin na bocnim konektoru jako vystup (ovlada AfD)

    attachInterrupt(digitalPinToInterrupt(pin_TS   ), interTS, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pin_tl_up), interUP, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pin_tl_dn), interDN, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pin_tl_ok), interOK, CHANGE);


    analogReadResolution(12);

    Wire.setSDA(pin_SDA);
    Wire.setSCL(pin_SCL);
    Wire.begin();
    
    delay(100);

    SPI.setMISO(pin_MISO);
    SPI.setMOSI(pin_MOSI);
    SPI.setSCLK(pin_SCLK);
    SPI.begin();

    rtclock.setClockSource(STM32RTC::LSE_CLOCK);
    rtclock.begin();

    pocet_bzuku = 5;
    jas_displeje = 3;

    velikost_hlavicky = 287 - (8 * (4-zobraz_rezervy));     // podle skrytych nebo zobrazenych rezervnich sloupcu vypocte sirku textovych vypisu zaznamu a hlavicek

// vypocet delky zaznamu podle zvolenych polozek pro ukladani do EEPROM
    delka_zaznamu = 8;
#ifdef ukladat_teplotu    
    delka_zaznamu = delka_zaznamu + 2;
#endif
#ifdef ukladat_tlak       
    delka_zaznamu = delka_zaznamu + 2;
#endif
#ifdef ukladat_vlhkost    
    delka_zaznamu = delka_zaznamu + 2;
#endif
#ifdef ukladat_infra      
    delka_zaznamu = delka_zaznamu + 2;
#endif
#ifdef ukladat_full       
    delka_zaznamu = delka_zaznamu + 2;
#endif
#ifdef ukladat_contr_reg 
    delka_zaznamu = delka_zaznamu + 1;
#endif

#ifdef ukladat_GPS 
    delka_zaznamu = delka_zaznamu + 10;
#endif

#ifdef ukladat_naklon 
    index_naklonu = delka_zaznamu;
    delka_zaznamu = delka_zaznamu + 2;
#endif

#ifdef ukladat_azimut 
    index_azimutu = delka_zaznamu;
    delka_zaznamu = delka_zaznamu + 2;
#endif

#ifdef ukladat_astro 
    delka_zaznamu = delka_zaznamu + 3;
#endif

#ifdef ukladat_rezerva_1
    delka_zaznamu = delka_zaznamu + 2;
#endif

#ifdef ukladat_rezerva_2
    delka_zaznamu = delka_zaznamu + 2;
#endif

#ifdef ukladat_rezerva_3
    delka_zaznamu = delka_zaznamu + 2;
#endif

#ifdef ukladat_rezerva_4
    delka_zaznamu = delka_zaznamu + 2;
#endif

    strip.begin();                                                               // start NeoPixelu
    strip.setPixelColor(0, 0x000000);                                            // zhasnout vsechny barvy na NeoLED
    strip.show();
    barva_LED = 0;

    peribity_maska = 0b1111111111111111;                     // bitova maska, pri ktere se nehlasi varovani na vypnute periferie
                                                             //'1' = ignorovat pripadne neshody (nebo neni periferie osazena), '0' = hlasit periferie, ktere nejsou v EEPROM v '1'
    if (osazeno_BME280 ) bitClear(peribity_maska,0);
    if (osazeno_LSM303 ) bitClear(peribity_maska,1);
    if (osazeno_SD_card) bitClear(peribity_maska,2);
    if (osazeno_DS3231 ) bitClear(peribity_maska,3);
    if (osazeno_r30s   ) bitClear(peribity_maska,4);
    



    uint16_t peribity_ee = EEPROM_read_int(eeaddr_perif_bity);                   // periferie je mozne libovolne povypinat. V teto casti EEPROM je ulozena informace o tom, ktere polozky jsou viditelne (2 bajty = az 16 periferii)
    for (int16_t polozka = 0 ; polozka <= pocet_periferii ; polozka ++)
      {
        perif_enable[polozka] = bitRead(peribity_ee, polozka);
      }

    senzor_BME        = perif_enable[0];
    modul_LSM303DLHC  = perif_enable[1];
    pouzivat_SD_kartu = perif_enable[2];
    pouzit_ds3231     = perif_enable[3];
    pouzit_r30s       = perif_enable[4];

    // neosazene periferie se pouzivat nebudou - nezavisle na tom, co se nastavi v menu "PEriF" pro blokaci periferii (nezavisle na poli "perif_enable[]")
    if (osazeno_BME280  == false)    senzor_BME        = false;
    if (osazeno_LSM303  == false)    modul_LSM303DLHC  = false;
    if (osazeno_SD_card == false)    pouzivat_SD_kartu = false;
    if (osazeno_DS3231  == false)    pouzit_ds3231     = false;
    if (osazeno_r30s    == false)    pouzit_r30s       = false;



    prumery = EEPROM_read(eeaddr_prumerovani);           // v EEPROM se na adrese 'eeaddr_prumerovani' nachazi cislo, ktere udava, kolik se ma prumerovat hodnot povoleny rozsah 1 az 20

    if (prumery == 30)                                   // v predchozim cyklu doslo k nastaveni defaultnich parametru a kalibracnich tabulek po prvnim spusteni s prazdnou pameti
      {
        EEPROM_write(eeaddr_prumerovani,3);              // defaultni prumerovani nastavit na 3, aby se po pristim resetu do teto casti programu uz nevstoupilo
        Serial.begin(9600);                              // servisni a nahravaci UART (piny PA9, PA10) 
        Serial2.begin(9600);                             // RS485  (piny PA2, PA3)
        
        test_text(0);                                    //  "tESt "
    
        for (uint8_t cyk = 0; cyk < 5 ; cyk++)           // 5x zapipani a zelene zablikani jako signalizace vstupu do testovaciho rezimu v pripade, ze neni osazeny displej
          {
            tone_X(pin_bzuk, 300, 20, 65535);
            blik_G_fast();
            delay(100);            
          }         
        
        HW_test(true);                                   // rovnou vstoupit do podprogramu testovani, TRUE znamena, ze po ukonceni testu se provede reset a pak uz normalni nabeh
      }
    
    
    if (prumery == 0 or prumery > 20)                    // kdyz je tam nejaky nesmysl (zatim prazdna EEPROM), tak se provede kompletni prepsani defaultnich parametru systemu do EEPROM
      {
        EEPROM_write(eeaddr_test_write,123);             // pokus o zapis nejakeho cisla (123) do EEPROM
        delay(50);
        uint8_t test_bajt = EEPROM_read(eeaddr_test_write); // kdyz se zpatky precte stejne cislo, znamena to, ze EEPROM spravne komunikuje a funguje
        if (test_bajt != 123)                            // kdyz se zpatky precte jine cislo, nez bylo ulozeno, znamena to nefunkcni EEPROM (nebo nefunkcni I2C komuninace)
          {
            zobraz_text(40);                             // kdyz je zapojeny displej, zobrazi se varovani "Err-i"  
            while(true)                                  // nekonecna smycka se da prerusit jen rucnim resetem 
              {
                if ((millis() % 2000) == 0 and (millis() < 60000))     // signalizace chyby pipnutim a LED se provadi 1x za 2 sekundy a jen 1 minutu po zapnuti
                  {
                    blik_C_kratky();                     // blika se svetle modre a pipa se vysokym tonem
                    tone_X(pin_bzuk, 2000, 20, 65535);
                    delay(100);                
                    LED_C_onoff(true);
                  }
              }                               
          }

        
        default_parametry();                             // test EEPROM dopadl dobre (precetla se hodnota 123 z adresy 599), provede se defaultni nastaveni parametru a kalibrace
        default_kalibrace_svetla();
        default_kalibrace_teploty();
        EEPROM_write(eeaddr_prumerovani,30);             // defaultni prumerovani jeste docasne prenastavit na nepovolenych 30, aby se po pristim resetu okamzite vstoupilo do testu HW
        
        delay(1000);
        SOFT_RESET();
      }




    // nastaveni komunikacni rychlosti podle bitu 4 a 5 na adrese 'eeaddr_SLAVE_baud' v EEPROM
    Serial.begin(pole_baudrate[(EEPROM_read(eeaddr_SLAVE_baud) & 0b00110000) >> 4]);            // servisni a nahravaci UART (piny PA9, PA10) 
    Serial2.begin(pole_baudrate[(EEPROM_read(eeaddr_SLAVE_baud) & 0b00110000) >> 4]);           // RS485  (piny PA2, PA3)
    Serial.setTimeout(500);                                                                     // pro USB linku se zkrati cekani na ParseInt() z 1 sek. na 0,5 sek. 


    // vypocet adresy prvniho bajtu posledniho zaznamu
    max_zaznam_EEPROM = eeaddr_MIN_EEPROM + ((eeaddr_MAX_EEPROM - eeaddr_MIN_EEPROM - delka_zaznamu) / delka_zaznamu) * delka_zaznamu;

    uint8_t zachytny_bod_hledani = EEPROM_read(eeaddr_10kB_znacka);
    adr_dalsiho_zaznamu = najdi_prazdne_misto(zachytny_bod_hledani);             // pri zapnuti se musi prohledat cela pamet od posledniho zachytneho bodu, ale priste uz se bude brat prazdny prostor z hodnoty teto promenne


    automat = EEPROM_read(eeaddr_automat);               // v EEPROM se na adrese 'eeaddr_automat' nachazi cislo, ktere udava, v jakych intervalech (minuty) se ma merit jas povoleny rozsah 0 az 255 (0=automat vypnuty)

    hranice_ukladani = EEPROM_read(eeaddr_uroven_EEPROM);// od jake urovne svetla se maji ukladat zaznamy do EEPROM


    signal_LED = EEPROM_read(eeaddr_LED);                // nastavene chovani RGB LED
    signal_BEEP = EEPROM_read_int(eeaddr_beep_bity);     // nastavene chovani piskaku


    zimni_posun = EEPROM_read(eeaddr_zima_hod) - 100;
    letni_posun = EEPROM_read(eeaddr_leto_hod) - 100;
    elevace_soumrak = EEPROM_read(eeaddr_elevace_soumrak);

    MOD_autoupdate = EEPROM_read(eeaddr_MOD_autoupdate);

    for (uint16_t i = eeaddr_txt_zony; i < eeaddr_txt_zony + 4 ; i++)   // uzivatelsky definovane popisy casovych zon jsou ulozene v EEPROM
      {
        char_zima[i-eeaddr_txt_zony] = EEPROM_read(i);
        char_leto[i-eeaddr_txt_zony] = EEPROM_read(i+4);
        char_Xzima[i-eeaddr_txt_zony] = EEPROM_read(i);
        char_Xleto[i-eeaddr_txt_zony] = EEPROM_read(i+4);
      }
    char_zima[4] = '\0';
    char_leto[4] = '\0';
    char_Xzima[4] = '\0';
    char_Xleto[4] = '\0';
    char_Xzima[0] = '_';                                 // pro vypisy smazanych zaznamu z EEPROM se vypisuje oznaceni zony s prvnim znakem nahrazenym podtrzitkem
    char_Xleto[0] = '_';
     
    slave_addr = (0b00001111 & EEPROM_read(eeaddr_SLAVE_baud));   // v EEPROM se na adrese 'eeaddr_SLAVE_baud' nachazi SLAVE adresa pro komunikaci (bity 0 az 3). Na bitech 4 a 5 je komunikacni rychlost.
    if (slave_addr == 0 or slave_addr > 15)              // kdyz je tam nejaky nesmysl, tak se prepise na 1
      {
        slave_addr = 1;
        EEPROM_write(eeaddr_SLAVE_baud,1);               // komunikacni rychlost se v tom pripade take zmeni na defaultni 9600
      }

    pouzit_modbus = EEPROM_read(eeaddr_modbus_LED) & 0b00000001;        // povoleny nebo zakazany modbus

    menu_dis_led = ((EEPROM_read(eeaddr_modbus_LED) & 0b00000010) >> 1); // plne displejove menu, nebo omezene LED menu



    AUTOkom = EEPROM_read(eeaddr_AUTOkom);
    if ((AUTOkom & 0b11000000) > 0)                           // je nastavene automaticke odesilani dat do seriove linky
      {
        AUTOkom_linka    = (AUTOkom >> 6);                    // linka 1 (RS485), nebo 2 (USB) 
        AUTOkom_format   = ((AUTOkom & 0b00010000) >> 4);     // format odesilanych dat (0= pole dat; 1= text)
        AUTOkom_obsah    = (AUTOkom & 0b00001111);            // obsah odesilanych dat
        if (AUTOkom_linka == 1 and pouzit_modbus == true)
          {
            Serial.println(lng366);                           // "Automatickym odesilanim zaznamu pres RS485 byl vypnut modbus"
            pouzit_modbus = false;                            // automaticke odesilani zaznamu do linky RS485 vypina MODBUS
          }
      }

    bortle_povoleno = EEPROM_read(eeaddr_bortle);             // zjisteni, jestli je povolena funkce zobrazeni Bortleho stupne v menu "JAS"
    if (bortle_povoleno != 1) bortle_povoleno = 0;            // cokoliv jineho nez 1 znamena, ze neni funkce povolena

    if (pouzit_modbus == true)
      {
        MBslave.setID(slave_addr);              // ID pro modbus
        MBslave.start();
    
    
        MODdata[31] = (err_bit << 7);           // status mereni na 0 (mereni dokonceno), pripadne chyby zustavaji dostupne v hornim bajtu
        
        index_posledniho_zaznamu = ((adr_dalsiho_zaznamu - eeaddr_MIN_EEPROM) / delka_zaznamu);
        MODdata[39] = index_posledniho_zaznamu; // [40040] index posledniho zaznamu
        MODdata[40] = 0;                        // [40041] index pozadovaneho zaznamu se nastavi na 0 (posledni zaznamu)
    
        MODdata[41] = prumery;                  // [40042] pocet prumerovani
        MODdata[42] = zimni_posun;              // [40043] zimni posun casu 
        MODdata[43] = letni_posun;              // [40044] letni posun casu 
        MODdata[44] = rozhod_stab;              // [40045] rozhodovaci uroven  pro stabilni jas
        MODdata[45] = hranice_ukladani;         // [40046] hranice jasu pro ukladani do EEPROM
        MODdata[46] = automat;                  // [40047] interval automatickeho spousteni
    
        MODdata[47] = 0xFFFF;                   // [40048] rezerva
        MODdata[48] = 0xFFFF;                   // [40049] rezerva
        MODdata[49] = 0xFFFF;                   // [40050] rezerva
        MODdata[50] = 0xFFFF;                   // [40051] rezerva
        
        MODdata[51] = 0xFFFF;                   // [40052] adresa EEPROM pro funkci pristupu do systemu (0 az 699)
        MODdata[52] = 0xFFFF;                   // [40053] obsah EEPROM pro funkci pristupu do systemu  (0 az 255)
    
        MODdata[59] = 0;                        // vymazani posledniho modbus prikazu

        MODdata[61] = 42765;                    // jednoznacna identifikace SQM pri komunikaci pres modbus (ID SQM)
    
        if (adr_dalsiho_zaznamu > eeaddr_MIN_EEPROM)                             // kdyz nejaky starsi zaznam existuje, tak se pripravi pro stazeni pres MODBUS
          {
            for (uint8_t i = 0 ; i < delka_zaznamu ; i++)
              {
                pole_EEPROM[i] = EEPROM_read(adr_dalsiho_zaznamu - delka_zaznamu  + i);
              }
            EEPROM_to_MOD(index_posledniho_zaznamu);              
          }
        
      }

    pouzij_CRC = EEPROM_read(eeaddr_CRC_tset_lock) & 0b00000011;           // z adresy 'eeaddr_CRC_tset_lock' se pouzivaji pro CRC jen 2 nejnizsi bity (nejvyssi bit je 'TS_lock')

    if ((EEPROM_read(eeaddr_CRC_tset_lock) & 0b01000000) == 0)  RTC_SD_DEBUG = false;   // zapis do souboru "RTC_set.csv" zakazan
    else                                                        RTC_SD_DEBUG = true;    // zapis do souboru "RTC_set.csv" povolen

    rozhod_stab = EEPROM_read(eeaddr_stab);

    pocet_cidel_svetla = EEPROM_read(eeaddr_pocet_cidel);

    afd = EEPROM_read(eeaddr_AfD);
    afd_styl = EEPROM_read(eeaddr_AFD_styl);
    if (afd_styl > 3 )                                          // pri nejake nesmyslne hodnote v EEPROM se nastavuje defaultni styl 0 
      {
        afd_styl = 0;
        EEPROM_update(eeaddr_AFD_styl,0);
      }
    afd_log = true;                                             // po zapnuti napajeni je logovani prvniho AFD povolene


    if (afd_styl == 0)      afd_bzuk = false;            // Puvodni styl chovani AFD (po resetu je piskani a zobrazeni "-AFd-" zakazano.  Povoluje se jen pri vstupu do polozky menu "AFD" nebo po editaci pres seriovou linku.)
    if (afd_styl == 1)      afd_bzuk = true;             // Po resetu se pipani a zobrazeni "-AFd-" vzdycky obnovi. Povoluje se pres menu AFD, docasne se vypina tlacitkem
    if (afd_styl == 2)      afd_bzuk = false;            // Po resetu je piskani a zobrazeni "-AFd-" zakazano.  Nepovoluje se ani pri vstupu do polozky menu "AFD" nebo po editaci pres seriovou linku.
    if (afd_styl == 3)                                   // Po resetu se AFD vypina (nastavuje se na hodnotu 25.5)
      {
        afd_bzuk = false;                                // Po resetu je piskani a zobrazeni "-AFd-" zakazano.
        EEPROM_update(eeaddr_AfD , 255);                 // Kdyz je od minule AFD zapnute (nejaka hodnota mensi nez 25.5), tak se automaticky vypne (nastavi se na 25.5)
        afd = 255;
      }

    
    selc = EEPROM_read(eeaddr_leto_zima);                // v EEPROM se na adrese 8 nachazi casova zona (0=SEC, 1 = SELC)
    if (selc > 1) 
      {
        selc = 1;
        EEPROM_write(eeaddr_leto_zima,selc);
      }
    if (selc == 0) leto = false;
    else           leto = true;


    if (pouzivat_SD_kartu == true)                       // kdyz je nastaveno pouzivani SD karty, ale karta neni zasunuta, zobrazi se pri startu na sekundu varovani, nebo blikne modra LED
      {
        if (digitalRead(pin_karta_IN)== HIGH)            // karta vysunuta
          {
            chyba(3);                                    // No-SD
          }
      }


    if (pouzit_ds3231 == true)
      {
        if ((RTC3231_read(0x0F) & 0b00000001) == 0)          // Alarm Flag od DS3231 neni aktivni (k zapnuti napajeni doslo vypinacem)
          {
            STM_DS(false);                                   // serizeni casu v DS3231 podle STM32 (false = bez 30-sekundoveho testu)
          }     
        else                                                 // Alarm Flag je aktivni (A1F = '1'), 
          {
            if ((RTC3231_read(0x0E) & 0b00000001) == 0)      //    ale alarmove funkce jsou zakazane (A1IE = '0')
              {                                              //          (predchozi alarm byl vypnuty tlacitkem, zadny dalsi alarm v EEPROM nebyl,
                                                             //           proto byl A1IE nastaven na '0', tim se provedlo okamzite vypnuti napajeni,
                                                             //            ale pred vypnutim napajeni se uz nestacil smazat Alarm Flag)
                reset_AF();                                  //  Alarm Flag se tedy smaze ted (A1F = '0')
              }     
          }
      }


    oddelovaci_bajt = EEPROM_read(eeaddr_oddelovace);
    des_carka         = bitRead(oddelovaci_bajt,2);
    uzavirat_uvozovky = bitRead(oddelovaci_bajt,3);
    vytvaret_hlavicku = bitRead(oddelovaci_bajt,4);
    oddelovac_polozek = oddelovaci_bajt & 0b00000011;

    if (des_carka == true)  desod = ',';
    else                    desod = '.';

    if (uzavirat_uvozovky == true)  uvozovky = '"';
    else                            uvozovky = ' ';     


    uint16_t pomprom_dso = EEPROM_read_int(eeaddr_DSO_setup);                 // ulozene prednastavene parametry pro funkci DSO
    DSO_schopnosti  =  pomprom_dso & 0b0000000000001111;                      // schopnosti pozorovatele - v nejnizsim nibblu                           - bity  3 az 0 (hodnoty 1 az 5)
    DSO_index_prumeru  = (pomprom_dso & 0b0000000011110000) >> 4;             // prumer dalekohledu (jeden z prednastavenych stupnu)                    - bity  7 az 4 (hodnoty 0 az 2)
    DSO_index_ohniska_dalekohledu = (pomprom_dso & 0b0000111100000000) >> 8;  // ohniskova vzdalenost hlavniho zrcadla (jeden z prednastavenych stupnu) - bity 11 az 8 (hodnoty 0 az 2)
    DSO_normal = EEPROM_read(eeaddr_DSO_normal_MSA);                          // normalovy jas oblohy pro urcovani viditelnosti otevrenych hvezdokup
    
    for (i = 0 ; i < 3 ; i++)                                                 // naplneni RAM poli hodnotami z EEPROM (EEPROM je editovatelna pres seriovou linku)
      {
        DSO_pole_prumery_dalekohledu[i] = EEPROM_read_int(eeaddr_DSO_prumery + (2*i));    // prumery a hlavni ohniska jsou dvojbajtove
        DSO_pole_delky_dalekohledu[i]   = EEPROM_read_int(eeaddr_DSO_ohniska + (2*i));
      }

    for (i = 0 ; i < 5 ; i++)
      {
        DSO_pole_ohniska_okularu[i] = EEPROM_read(eeaddr_DSO_okulary + i);            // ohniska okularu jsou jednobajtove
      }

    DSO_prumer_dalekohledu = DSO_pole_prumery_dalekohledu[DSO_index_prumeru];             // prumer dalekohledu v mm
    DSO_delka_dalekohledu = DSO_pole_delky_dalekohledu[DSO_index_ohniska_dalekohledu];    // ohniskova vzdalenost hlavniho zrcadla v mm
    DSO_delka_okularu = DSO_pole_ohniska_okularu[0];                                  // ohniskova delka okularu v mm  (neuklada se do EEPROM, nastavuje se vzdycky na prvni okular)



    float geopomprom = EEPROM_read_int(eeaddr_GEO_lat);
    if (geopomprom > 32767) geopomprom = geopomprom - 65536  ; 
    GeoLat = geopomprom / 10.0;
    
    geopomprom = EEPROM_read_int(eeaddr_GEO_lon);
    if (geopomprom > 32767) geopomprom = geopomprom - 65536  ; 
    GeoLon = geopomprom / 10.0;


    if (GeoLat > 90 or GeoLat < -90 or GeoLon > 180 or GeoLon < -180)            // souradnice jsou uplne mimo, provede se default na 50/15
      {
        EEPROM_write_int(   eeaddr_GEO_lat   ,         500);        // zemepisna sirka pro astro vypocty na 50.0 stupnu
        EEPROM_write_int(   eeaddr_GEO_lon   ,         150);        // zemepisna delka pro astro vypocty na 15.0 stupnu
      }

    // po resetu je mozne obnovit defaultni nastaveni promennych v EEPROM (bez kalibrace) dlouhym stiskem vsech 3 tlacitek zaroven
    if (digitalRead(pin_tl_up) == LOW and digitalRead(pin_tl_dn) == LOW and digitalRead(pin_tl_ok) == LOW) 
      {
        tone_X(pin_bzuk,4000, 10 , 6);
        uint32_t starttime = millis();
        while (digitalRead(pin_tl_up) == LOW and digitalRead(pin_tl_dn) == LOW and digitalRead(pin_tl_ok) == LOW)
          {
            delay(50);
            if (millis() - starttime > 10000UL)
              {
                tone_X(pin_bzuk,4000, 10 , 6);
                delay(100);
                default_parametry();

                zobraz_text(42);                         // dEFA
                delay(500);                
                zobraz_text(66);                         // prazdny displej

                LED_W(true);                             // signalizacni LED se na 0,5 sekundy rozsviti bile
                delay(500);                
                LED_W(false);                
                break;
              }
            else
              {
                graf_natazeni(millis() - starttime , 666);    // vystrazny graf z pomlcek na displeji - zaplneni celeho displeje trva asi 10 sekund
              }
          }
        while (digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW or digitalRead(pin_tl_ok) == LOW)
          {
            tone_X(pin_bzuk,3000, 10 , 6);
            delay(500);
          }
        delay(100);

        delay(1000);
        SOFT_RESET();
      
      }

    if ((RTC->BKP0R) != 0xAA)                            // znacka ze byl cas nastaveny. Po vypadku 3V zalozni baterie se znacka automaticky maze
      {
        chyba(4);                                        // chyba RTC
        if (EEPROM_read_long(eeaddr_RTC_set) != 0)                 EEPROM_write_long(eeaddr_RTC_set,0);                // zruseni znacky posledniho nastaveni casu
        if (EEPROM_read_long(eeaddr_RTC_korekce) != 0x7FFFFFFFUL)  EEPROM_write_long(eeaddr_RTC_korekce,0x7FFFFFFFUL); // zruseni korekce
      }
    else
      {
        bitClear(err_bit,3);                              // RTC je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
      }

    // aby se nemuselo pri kazdem pozadavku o cas z RTC  sahat do EEPROM, nastavi se nasledujici globalni promenne jen pri startu, nebo pri dalsim serizeni RTC
    cas_minuleho_nastaveni = EEPROM_read_long(eeaddr_RTC_set);        // Z EEPROM se zjisti, kdy se naposled serizoval cas (cas je v sekundach od 1.1.1970)
    korekce                = EEPROM_read_long(eeaddr_RTC_korekce);    // a jaka byla pri te prilezitosti vypoctena korekce (za jak dlouho se pricist, nebo odecist extra sekunda)


    test_nulovani_citace();                              // nulovani citace casovych razitek a prednastaveneho stanoviste po kazdem poledni

    if (digitalRead(pin_TS) == HIGH)                     // kdyz je pri zapnuti napajeni TS tlacitko uvolnene (normalni zapnuti posuvnym prepinacem)
      {
        TS_zamek(false);                                 // zamek TS tlacitka se automaticky odblokuje
      }

    // SW blokovani bocniho tlacitka je mozne pri zapnuti napajeni a soucasnem stisku tlacitka DOLU
    if (digitalRead(pin_tl_up) == HIGH and digitalRead(pin_tl_dn) == LOW and digitalRead(pin_tl_ok) == HIGH) 
      {
        TS_zamek(true);
        zobraz_text(36);                                 // "-Loc-"
        while (digitalRead(pin_tl_dn) == LOW) delay(100);          // ceka se na uvolneni tlacitka DOLU
        delay(100);
        zobraz_text(66);                                 // zhasne displej
      }

    if ((EEPROM_read(eeaddr_CRC_tset_lock) & 0b10000000) > 0)  TS_lock = true;       // podle hodnoty v EEPROM se nastavi globalni promenna pro zamek TS tlacitka
    else                                                       TS_lock = false;

    if (modul_LSM303DLHC == true)                        // pokud je zapojeny naklonomer, provede se jeho zakladni nastaveni a nacteni kalibrace
      {
        naklon_setup();
        kompas_setup();    
      }

    if (senzor_BME == true)
      {
        cidlo_BME.begin(0x76);
      }


    uint32_t menubity = EEPROM_read_long(eeaddr_menu_bity);    // polozky v menu je mozne libovolne povypinat. V teto casti EEPROM je ulozena informace o tom, ktere polozky jsou viditelne (4 bajty = max 32 polozek)
    for (int16_t polozka = 0 ; polozka <= pocet_polozek_v_menu ; polozka ++)
      {
        menu_enable[polozka] = bitRead(menubity, polozka);
      }


    if (digitalRead(pin_TS) == LOW and TS_lock == false) // pri zkratu pinu SCK a GND na bocnim konektoru desky WeAct (BlackPill) se zaznamena casove razitko
      {
        if (digitalRead(pin_tl_up) == LOW and digitalRead(pin_tl_dn) == HIGH and digitalRead(pin_tl_ok) == HIGH)   // kdyz je pri stisku bocniho tlacitka zaroven stisknuto UP, prejde se do volby viditelnych polozek v menu
          {
            editace_polozek_menu();
          }

        if (digitalRead(pin_tl_up) == HIGH and digitalRead(pin_tl_dn) == HIGH and digitalRead(pin_tl_ok) == LOW)   // kdyz je pri stisku bocniho tlacitka zaroven stisknuto [OK], prejde se do volby vypinani poskozenych periferii
          {
            editace_periferii();
          }
                
        if (digitalRead(pin_TS) == LOW and digitalRead(pin_tl_up) == HIGH and digitalRead(pin_tl_dn) == HIGH and digitalRead(pin_tl_ok) == HIGH)  // kdyz je pri stisku bocniho tlacitka neni zaroven stisknuto zadne tlacitko, zaznamena se casova znacka
          {
            casove_razitko();                            // pri dlouhem drzeni se po nekolika sekundach zobrazi cas - soucast podprogramu 'casove_razitko()'
            delay(100);                                  // uvolnenim tlacitka v teto fazi se vypne napajeni a program skonci                
          }
      }

    if (digitalRead(pin_TS) == LOW and TS_lock == true)  // kdyz je TS tlacitko blokovane, zobrazi se jen kratke upozorneni na displeji a casova znacka se neuklada
      {
        chyba(7);                                        // pipnuti a zobrazeni napisu "-Loc-"       
        while(true)   asm("wfi") ;                       // nekonecna smycka s prechodem procesoru do nizke spotreby, ktera se prerusi az vypnutim napajeni
      }


    prijem();

    zobraz_text(66);                                     // zhasne displej pro pripad, ze by neco zustalo v jeho pametech

    LED_B_onoff(false);                                  // zhasne modrou LED


    
    // nacteni kalibracnich dat pro svetlo z EEPROM pro cidlo s indexem 1
    for (uint8_t i = 0 ; i < 15 ; i++)
      {
        kal_svetlo[i][0] = EEPROM_read_int((i*4) + eeaddr_svetelna_kalibrace);
        kal_svetlo[i][1] = EEPROM_read_int((i*4) + eeaddr_svetelna_kalibrace + 2);
      }

    // nasledujici 2 kalibracni hodnoty jsou napevno a neukladaji se do EEPROM
    kal_svetlo[15][0] = 0;                               //  [15]   - tohle musi byt minimalni zmeritelna hodnota svetla (nejvetsi svetlo) (0 mag)  (nic mensiho se nesmi namerit)
    kal_svetlo[15][1] = 0;
    kal_svetlo[16][0] = 24000;                           //  [16]   - tohle musi byt maximalni zmeritelna hodnota svetla (nejvetsi tma) (24,000 mag)(nic vetsiho se nesmi namerit)
    kal_svetlo[16][1] = 24000;

    test_korekcni_tabulky_svetla();                      // zkontroluje spravne zadane udaje



    // nacteni kalibracnich dat pro teplotu z EEPROM
    for (uint8_t i = 0 ; i < 7 ; i++)
      {
        kal_teplota[i][0] = EEPROM_read_int((i*4) + eeaddr_teplotni_kalibrace);
        kal_teplota[i][1] = EEPROM_read_int((i*4) + eeaddr_teplotni_kalibrace + 2);
      }
    // nasledujici 2 kalibracni hodnoty jsou napevno a neukladaji se do EEPROM
    kal_teplota[7][0] = 0;                               //  [7]   - tohle musi byt minimalni zmeritelna hodnota teploty (-50'C)(nic mensiho se nesmi namerit)
    kal_teplota[7][1] = 0;
    kal_teplota[8][0] = 10000;                           //  [8]   - tohle musi byt maximalni zmeritelna hodnota teploty (+50'C) (nic vetsiho se nesmi namerit)
    kal_teplota[8][1] = 10000;

    test_korekcni_tabulky_teploty();                     // zkontroluje spravne zadane udaje

    // vytvoreni unikatniho identifikacni cislo procesoru (cislo waferu, x-y pozice cipu na waferu, vyrobni sarze)
    long32_to_HEX(ID1, 0);
    long32_to_HEX(ID2, 10);
    long32_to_HEX(ID3, 21);

    lux_kal_a = ((int16_t)EEPROM_read_int(eeaddr_luxmetr)) / 100.0; // nacteni kalibracnich konstant pro luxmetr (v EEPROM mohou byt ulozena i zaporna cisla -32768 az +32767)
    lux_kal_b = ((int16_t)EEPROM_read_int(eeaddr_luxmetr + 2)) / 100.0;
    lux_kal_c = ((int16_t)EEPROM_read_int(eeaddr_luxmetr + 4)) / 1000000.0;


    prepni_cidlo_svetla(1);                              // v pripade pouziti rozsirujici desky pro vice cidel svetla se prepne na to prvni.
                                                         //   kdyz deska neexistuje, zustane jen jedno cidlo na zakladni desce

    if (vypis_budiku(3) > 0)                             // zjisteni poctu aktivnich alarmu v EEPROM
      {
        zobraz_text(71);                                 // "ALA.on"
        delay(800);
      }
    
    if (automat > 0)                                     // kdyz je nastavene automaticke snimani
      {
        posledni_autospusteni = millis();
        zobraz_text(1);                                  // "Auto "
        delay(800);
//        mereni(6);                                     // odkomentovanim by se prvni mereni provedlo hned po zapnuti napajeni
      }  

    if ((peribity_ee | peribity_maska) != 0xFFFF)        // kdyz jsou vypnute (EEPROM v '0') nejake sledovane (maska v '0') periferie, zobrazi se upozorneni
      {
        zobraz_text(72);                                 // "PEriF"
        tone_X(pin_bzuk,600, 20 , 3);                    // pokud neni pipani pri chybach HW zakazane prikazem "@P", tak se i vystrazne pipne
        delay(800);
      }

    if (afd < 255)                                       // Alarm for Darkness ma zapnute logovani. Po resetu ale muze byt signalizace prekroceni urovne na displeji neaktivni.
      {
        if (afd_styl == 0 or afd_styl == 2)  zobraz_text(70);   // "-A.F.d.-"   (AFD je aktivni, ale pri startu je signalizace na displeji zakazana)
        if (afd_styl == 1)                   zobraz_text(78);   // "AFd.on"     (AFD je aktivni, pri startu je signalizace na displeji povolena)
        delay(800);
      }


    if (pouzit_r30s == true)                             // jen kdyz je R30s povolena periferie
      {
        spec_rezim = test_zasunuti();                    // test zasunuti pridavneho HW //!! (r30s)
        if (spec_rezim == 100)
          { 
            max_EEPROM30 = (EEPROM_read30(4) *256) + EEPROM_read30(5);
            casovani30 = EEPROM_read30(1);
            adrsave30 = najdi_volne_misto30();
            posledni_zaznam30 = millis();
            if (EEPROM_read30(3) == 0)
              {
                pauza30 = false;
                zobraz_text(43);                         // "#-on "
              }
            else
              {
                pauza30 = true;
                zobraz_text(44);                         // "#-oFF"
              }
            delay(700);
          }
      }
    else                                                 // rezim r30s je blokovana periferie
      {
        spec_rezim = 0;
      }

    SD_log(999 , 1);                                     // ... uklada se do zvlastniho logovaciho souboru na SD karte datum a cas normalniho zapnuti napajeni (kod/adresa 999)

    cti_napajeni();                                      // naplni globalni promenne zmerenymi napetimi

    

    for (uint8_t i = 0; i < 6; i++)                      // obnoveni casu alarmu z EEPROM do pole RTC_alarm[]
      {
        uint16_t cas_alarmu = EEPROM_read_int(eeaddr_alarmy + (i*2));
        if (cas_alarmu > 1440) cas_alarmu = 1440;        // hodnota mimo rozsah se prepise deaktivovanym budikem (1440 minut = 24 hodin)        
        RTC_alarm[i+1] = cas_alarmu;
      }

    trackuj = bitRead(EEPROM_read_int(eeaddr_GPS_track_id), 15);         // z EEPROM se zjisti, jestli predchozi stav GPS trasovani (pred vypnutim napajeni)
    track_interval = EEPROM_read(eeaddr_GPS_track_interval);             // z EEPROM se zjisti interval zapisu bodu na kartu (20 sekund)
    nastav_cestu_tracku();                                               // podle cisla v EEPROM pripravi cestu k aktualnimu souboru GPX

    if (trackuj == true)                                                 // kdyz je od minule trasovani zapnute
      {
        zobraz_text(97);                                                 // blikne na displeji napis "tr. on"
        delay(800);
      }

    SD_log(994 , (uint16_t)napeti_9V);                   // napeti v desetinach V (0 az 300 odpovida 0,0V az 30,0V)

    SD_volno = SDfree(sd)>>20;                           // Zjisteni volneho prostoru na SD karte (prevod z bajtu na MB)


#ifdef pouzivat_kalibrator
    if (KAL_test() == true)                                              // kdyz je pripojeny kalibrator ...
      {
        kal_postupy();                                                   // ... spusti se kalibracni postupy (na konci kalibracnich podprogramu se zastavi v nekonecne smycce)            
      }
#endif

    tone_X(pin_bzuk, 300, 30, 8);                                        // pokud je povolene pipnuti pri zapnuti napajeni, tak se pipne pri dokonceni "setup" bloku
  }
//======================================================================================================================



//======================================================================================================================
void loop(void)
  {

    test_baterie();

    if(digitalRead(pin_TEST_HW) == HIGH and osazeno_uref == true)    // jen testy A/D prevodniku pro vyhodnoceni stavu baterie - bez pripojene komunikace
      {
        zobraz_text(34);                                 // "rEF_U"
        delay(3800);        
        zobraz_cislo(analogRead(pin_refu),0);
        delay(2500);

        zobraz_text(35);                                 // "U_bAt"
        delay(3800);        
        zobraz_cislo(analogRead(pin_AD_BAT),0);
        delay(2500);    
      }
      
    if (digitalRead(pin_DATA_RDY) != predchozi_data_RDY) // kdyz dojde k prepnuti stavu pinu 'pin_DATA_RDY' (zapnuti / vypnuti GPS)
      {
        predchozi_data_RDY = digitalRead(pin_DATA_RDY);  // zaloguje se tento stav
        delay(100);
        SD_log(997,predchozi_data_RDY);
      }

    if (afd < 255)                                                               // Alarm for Darkness je aktivni
      {
        if (afd_nabito == false and (millis() - predchozi_cas_afd) > 10000)      // pokud od minuleho mereni svetla ubehlo alespon 10 sekund
          {
            test_AFD(false);                                                     // provede se jen nastartovani mereni, ktere pak ale v cidle trva neco pres sekundu
            predchozi_cas_afd = millis();
            afd_nabito = true;
          }
    
        if (afd_nabito == true and (millis() - predchozi_cas_afd) > 2000)        // pokud od nastartovani mereni AFD ubehly alespon 2 sekundy
          {
            uint32_t zmerene_AFD = test_AFD(true);                               // provede se cteni registru svetla a jejich zpracovani
            if (millis() - predchozi_zapis_afd > 60000UL)                        // zapis na SD kartu 1x za minutu
              {
                predchozi_zapis_afd = millis();
                logafd(zmerene_AFD);                                             // loguje se ve 32-bitech
              }

            if ((zmerene_AFD % 100) >= 50)                                       // zaokrouleni (priklad mag = 15.278; zmerene_AFD = 15278)
              {
                zmerene_AFD = zmerene_AFD + 100;                                 // po pricteni 100: (zmerene_AFD = 15378)
              }
            zmerene_AFD = zmerene_AFD / 100;                                     // prevod na bajt:  (zmerene_AFD = 153)
            afd_desetiny = zmerene_AFD;                                          // nastaveni globalni promenne pro pripadne logovani do syslogu

            if (zmerene_AFD > afd)                                               // obloha je tmavsi, nez nastavena hodnota
              {
                status_afd = true;
                MODdata[31] = MODdata[31] | 0b1000000000000000;
                digitalWrite(pin_bok_out, HIGH);
              }
            else                                                                 // obloha je svetlejsi, nez nastavena hodnota
              {
                status_afd = false;
                MODdata[31] = MODdata[31] & 0b0111111111111111;
                digitalWrite(pin_bok_out, LOW);
              }

            predchozi_cas_afd = millis();                                        // blokovani dalsiho mereni AFD na dalsich 10 sekund
            afd_nabito = false;
          }        
      }
    else                                                                         // AFD je vypnute (nastavene na 25,5 mag/arcsec2)
      {
        status_afd = false;
        MODdata[31] = MODdata[31] & 0b0111111111111111;
        digitalWrite(pin_bok_out, LOW);
      }

    if (bitRead(signal_LED,4) == true and status_afd == true)                        // kdyz je blikani povolene a zaroven je AFD aktivni
      {
        if (millis() - posledni_blik_afd > 500)                                      // LED pro AFD blika 2x za sekundu
          {
            LED_W(true);
            delay(50);
            LED_W(false);
            posledni_blik_afd = millis();
          }      
      }



    interrupty();                                        // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni
    auto_exit_menu = false;                              // v hlavni smycce se rusi pozadavek na automaticke ukonceni menu

    index_posledniho_zaznamu = 1 + (adr_posl_zaznamu - eeaddr_MIN_EEPROM) / delka_zaznamu;

    if (afd_bzuk == true and status_afd == true)         // kdyz je povolene zobrazeni hlaseni "-AFD-" a AFD opravdu nastal
      {
        while (digitalRead(pin_tl_up) == LOW or digitalRead(pin_tl_dn) == LOW or digitalRead(pin_tl_ok) == LOW)  // libovolnym tlacitkem [OK] [NAHORU] [DOLU]
          {
            if (afd_bzuk == true)
              {
                SD_log(992,0);                           // logovani vypnuti alarmoveho displeje tlacitkem
              }
            delay(50);
            afd_bzuk = false;                            // se bzukani trvale vypina (az do dalsiho otevreni polozky menu "AFD" nebo prenastaveni urovne AFD pres seriovou linku)
          }
      }


    if (digitalRead(pin_tl_up) == LOW)                   // zaznam hodnoty s tlacitkem "NAHORU"
      {
        delay(20);
        mereni(1);                                       // parametr je znacka tlacitka, ktera se uklada do zaznamu
        while (digitalRead(pin_tl_up) == LOW) delay(20); // cekani na uvolneni tlacitka
        delay (20);                                      // odruseni zakmitu pri uvolneni tlacitka "NAHORU"
      }


    if (digitalRead(pin_tl_dn) == LOW)                   // zaznam hodnoty s tlacitkem "DOLU"
      {
        delay(20);
        mereni(2);                                       // parametr je znacka tlacitka, ktera se uklada do zaznamu
        while (digitalRead(pin_tl_dn) == LOW) delay(20); // cekani na uvolneni tlacitka
        delay (20);                                      // odruseni zakmitu pri uvolneni tlacitka "NAHORU"
      }


    if (digitalRead(pin_tl_ok) == LOW)                   // stisk tlacitka OK (tady se rozlisuje kratky a dlouhy stisk)
      {
        delay(50);                                       // odruseni zakmitu pri stisku tlacitka
        stisk_time = millis();
        stisk = true;
        uint32_t trvani = 0;
        while (digitalRead(pin_tl_ok) == LOW)
          {
            delay(20);                                   // cekani na uvolneni tlacitka
            trvani = millis() - stisk_time;
            if (trvani >= 500)                           // dlouhy stisk = vstup do menu
              {
                if (menu_dis_led == true)   menu_1();    // vstup do menu se zobrazenim polozek na nekolika 7-segmentovych displejich
                else                        menu_LED();  // vstup do LED menu (jen zakladni polozky, ktere jsou signalizovany blikanim LED )
              }
            if (digitalRead(pin_tl_ok) == HIGH) delay(50); // kdyz se behem drzeni tlacitka OK objevi kratky vypadek kontaktu, tak se odignoruje        
          }
        if (trvani < 500 and trvani > 100)               // kratky stisk = mereni (stisk ale musi trvat alespon 0,1 sekundy)
          {
            mereni(3);                                   // pro tlacitko OK se provadi mereni az pri uvolneni
          }       
    
        stisk=false;
      }

    msleep(100);

  }
//======================================================================================================================



//------------------------------------------
// systemove informace
void sys_info(void)
  {
    uint16_t pomprom;
    
    Serial.print("SW ver.: ");                                 //             verze software
    Serial.println(verzeSW);                                   //             (datum nebo jiny text nadefinovany na zacatku programu)
    Serial.println(' ');                                       //             prazdna radka


    zobraz_RTC(true,true);                                     //             zobrazeni aktualniho zkorigovaneho casu

    pomprom = EEPROM_read(eeaddr_oddelovace);
    Serial.println(lng112);                                    //             Serial.println("\r\nOddelovace:");
    
    Serial.print(lng113);                                      //             Serial.print(" Polozky v souboru: ");
    if ((pomprom & 0b11) ==     0 )  Serial.println(lng078);   //             Serial.println("mezera");
    if ((pomprom & 0b11) ==     1 )  Serial.println(lng079);   //             Serial.println("carka");
    if ((pomprom & 0b11) ==     2 )  Serial.println(lng080);   //             Serial.println("strednik");
    if ((pomprom & 0b11) ==     3 )  Serial.println(lng081);   //             Serial.println("tabulator");

    Serial.print(lng114);                                      //             Serial.print(" Desetinny oddelovac: ");
    if ((pomprom & 0b100) ==    4 )  Serial.println(lng079);   //             Serial.println("carka");
    else                             Serial.println(lng083);   //             Serial.println("tecka");

    if ((pomprom & 0b1000) ==   8 )  Serial.println(lng115);   //             Serial.println(" Uzavirat polozky do uvozovek");
    else                             Serial.println(lng116);   //             Serial.println(" Neuzavirat polozky do uvozovek");

    if ((pomprom & 0b10000) == 16 )  Serial.println(lng117);   //             Serial.println(" Do souboru vkladat hlavicky");
    else                             Serial.println(lng118);   //             Serial.println(" Hlavicky do souboru nevkladat");

    Serial.print(lng119);                                      //             Serial.println("\r\nAutomat:");        
    pomprom = EEPROM_read(eeaddr_automat);
    if (pomprom == 0)                Serial.println(lng120);   //             Serial.println(" Bez automatickeho mereni");
    else
      {
                                     Serial.print(lng121);     //             Serial.print("  Spoustet mereni kazdych ");
                                     Serial.print(pomprom);
                                     Serial.println(lng122);   //             Serial.println(" minut");
      }

    pomprom = EEPROM_read(eeaddr_prumerovani);
    Serial.print(lng123);                                      //             Serial.print("\r\nPrumerovat ");
    Serial.print(pomprom);
    Serial.println(lng124);                                    //             Serial.println(" mereni plosneho jasu");

    pomprom = (EEPROM_read(eeaddr_CRC_tset_lock) & 0b00000001);//             informace o CRC je v bitu 0

    if (pouzit_modbus == false)
      {
        if (pomprom == 0)  Serial.println(lng125);             //             Serial.println("Kontrolni bajt se pri komunikaci pres RS485 nevyhodnocuje.");
        else               Serial.println(lng126);             //             Serial.println("Pri komunikaci pres RS485 je nutne odesilat spravny kontrolni bajt.");    
      }

    AUTOdata_tabulka();                                        //             Informace o automatickem odesilani dat (Master rezim)


    if (pouzit_modbus == false) Serial.println(lng280);        //             Serial.println("MODBUS vypnuty");
    else                        Serial.println(lng279);        //             Serial.println("MODBUS zapnuty");
    
    pomprom = EEPROM_read(eeaddr_stab);
    Serial.print(lng127);                                      //             Serial.print("Rozhodovaci rozdil pro stabilni/nestabilni jas je ");
    Serial.print(pomprom/10.0,1);
    Serial.println(" %");

    pomprom = EEPROM_read(eeaddr_uroven_EEPROM);
    if (pomprom > 0)
      {
        Serial.print(lng128);                                  //             Serial.print("Do EEPROM se ukladaji zaznamy s plosnym jasem nad (tmavsi nez) ");
        Serial.print(pomprom/10.0);
        Serial.println(" mag/arcsec2");
      }
    else
      {
        Serial.println(lng207);                                //             Serial.print("Do EEPROM se ukladaji vsechny zaznamy");        
      }
    if (afd < 255)
      {
        Serial.print(lng303);                                  //             Serial.print("AfD nastaven na ");
        Serial.print(afd / 10.0);
        Serial.println(" mag/arcsec2");
      }
    else
      {
        Serial.println(lng304);                                //             Serial.println("AfD vypnuty");
      }


   
    Serial.print(lng129);                                      //             Serial.print("Unikatni identifikace: ");
    Serial.print(" 0x");
    Serial.println(ID_char);

    pomprom = (0b00001111 & EEPROM_read(eeaddr_SLAVE_baud));
    Serial.print(lng019);                                      //             Serial.print("SLAVE adresa: ");
    Serial.println(pomprom);

    Serial.println(lng130);                                    //             Serial.println("\r\nHardware:");

    Serial.print(lng131);                                      //             Serial.println(" Procesor: ");
    uint32_t idcode = DBGMCU->IDCODE & 0xFFF;
    bool znamy_proc = false;
    switch (idcode)
      {
        case 0x433:
          Serial.print("STM32F401CEU...");
          znamy_proc = true;
          break;

        case 0x431:
          Serial.print("STM32F411CEU...");
          znamy_proc = true;
          break;

        default:
          Serial.println("Unknown");
      }

    if (znamy_proc == true)
      {
        Serial.print(" (FLASH: ");
        Serial.print(VELIKOST_FLASH);
        Serial.println(" kb) - BlackPill");
      }

    if (menu_dis_led == true)
      {
        Serial.println(lng296);                                //             Serial.println(" Displejova verze SQM");      
        Serial.println(lng140);                                //             Serial.println(" Displej: 5x7 segmentu (TM1637)");
      }
    else
      {
        Serial.println(lng297);                                //             Serial.println(" LED verze SQM");    
      }
    
    Serial.println(lng132);                                    //             Serial.println(" Cidlo jasu: TSL2591");    
    if (senzor_BME == true)
      {
        Serial.println(lng135);                                //             Serial.println(" Cidlo vlhkosti, teploty a tlaku: BME280");
      }

    if (modul_LSM303DLHC == true)
      {
        Serial.println(lng265);                                //             Serial.println(" Kompas / naklon LSM303DLHC");    
      }

    Serial.print(lng136);                                      //             Serial.print(" SD karta: ");
    if (pouzivat_SD_kartu == true)
      {
        if (digitalRead(pin_karta_IN) == LOW)
          {
            Serial.print(lng137);                              //             Serial.println("zasunuta");
            SD_volno = SDfree(sd)>>20;                         // prevod z bajtu na MB
            Serial.print(" (Free: ");
            if (SD_volno > 0)                                  // nejaky volny prostor v [MB] na karte jeste existuje
              {
                Serial.print(SD_volno);
                Serial.print('/');
                Serial.print(SDkapacita);
                Serial.println(" [MB])");
              }
            else                                               // nejaka chyba pri zjistovani volneho prostoru z MBR, Boot, nebo FSinfo sektoru
              {
                Serial.println("???)");                
              }
          }
        else
          {
            Serial.println(lng138);                            //             Serial.println("vysunuta");        
          }
      }
    else
      {
        Serial.println(lng139);                                //             Serial.println("nepouziva se");
      }
    

    RGB_LED_info();
    BEEP_info();

    if (pocet_cidel_svetla > 1)                                // kdyz neni nastavena expanzni deska pro cidla svetla, nema cenu vypisovat hlaseni o dostupnych vstupech
      {
        Serial.print(lng142);                                  //             Serial.print(" Pocet vstupu rozsirujici desky: ");
        Serial.println(pocet_cidel_svetla);
        Serial.print(lng143);                                  //             Serial.print(" Aktualne aktivni cidlo jasu:    ");
        Serial.println(index_cidla_svetla);
      }


    Serial.println(lng144);                                    //             Serial.println("\r\nEEPROM:");
    Serial.print(lng145);                                      //             Serial.print(" Zacatek oblasti pro data: ");
    dopln_mezery(eeaddr_MIN_EEPROM,6);
    Serial.println(eeaddr_MIN_EEPROM);
    
    Serial.print(lng146);                                      //             Serial.print(" Konec oblasti pro data:   ");
    dopln_mezery(max_zaznam_EEPROM + delka_zaznamu - 1 ,6);
    Serial.println(max_zaznam_EEPROM + delka_zaznamu - 1);

    Serial.print(lng147);                                      //             Serial.print(" Pristi zaznam se ulozi na adresu: ");
    Serial.println(adr_dalsiho_zaznamu);


    Serial.print(lng148);                                      //             Serial.print(" Jeden zaznam v EEPROM je dlouhy: ");
    Serial.print(delka_zaznamu);
    Serial.println(lng149);                                    //             Serial.println (" bajtu");

    Serial.println(lng150);                                    //             Serial.println(" obsahuje tyto polozky: ");
    Serial.println(lng151);                                    //             Serial.println("  - plosny jas           ANO");

    Serial.print(lng152);                                      //             Serial.print  ("  - teplota              ");
#ifdef ukladat_teplotu
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif

    Serial.print(lng155);                                      //             Serial.print  ("  - tlak                 ");
#ifdef ukladat_tlak
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif

    Serial.print(lng156);                                      //             Serial.print  ("  - vlhkost              ");
#ifdef ukladat_vlhkost
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif

    Serial.print(lng157);                                      //             Serial.print  ("  - infra slozka svetla  ");
#ifdef ukladat_infra
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif

    Serial.print(lng158);                                      //             Serial.print  ("  - full svetlo          ");
#ifdef ukladat_full
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif

    Serial.print(lng159);                                      //             Serial.print  ("  - ctrl registr TSL2591 ");        
#ifdef ukladat_contr_reg
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif

    Serial.print(lng206);                                      //             Serial.print  ("  - GPS souradnice       ");        
#ifdef ukladat_GPS
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif

    Serial.print(lng228);                                      //             Serial.print  ("  - aktualni naklon      ");        
#ifdef ukladat_naklon
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif

    Serial.print(lng264);                                      //             Serial.print  ("  - azimut               ");        
#ifdef ukladat_azimut
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif


    Serial.print(lng236);                                      //             Serial.print  ("  - poloha Slun. a Mes.  ");        
#ifdef ukladat_astro
    Serial.println(lng153);                                    //             Serial.println("ANO");
#else
    Serial.println(lng154);                                    //             Serial.println("NE");
#endif

    if (zobraz_rezervy >= 1)
      {
        Serial.print(lng305);                                  //             Serial.print  ("  - rezerva 1            ");        
#ifdef ukladat_rezerva_1
        Serial.println(lng153);                                //             Serial.println("ANO");
#else
        Serial.println(lng154);                                //             Serial.println("NE");
#endif    
      }

    if (zobraz_rezervy >= 2)
      {
        Serial.print(lng306);                                  //             Serial.print  ("  - rezerva 2            ");        
#ifdef ukladat_rezerva_1
        Serial.println(lng153);                                //             Serial.println("ANO");
#else
        Serial.println(lng154);                                //             Serial.println("NE");
#endif    
      }

    if (zobraz_rezervy >= 3)
      {
        Serial.print(lng307);                                  //             Serial.print  ("  - rezerva 3            ");        
#ifdef ukladat_rezerva_1
        Serial.println(lng153);                                //             Serial.println("ANO");
#else
        Serial.println(lng154);                                //             Serial.println("NE");
#endif    
      }

    if (zobraz_rezervy >= 4)
      {
        Serial.print(lng308);                                  //             Serial.print  ("  - rezerva 4            ");        
#ifdef ukladat_rezerva_1
        Serial.println(lng153);                                //             Serial.println("ANO");
#else
        Serial.println(lng154);                                //             Serial.println("NE");
#endif    
      }


    Serial.print(lng237);                                      //             Serial.print("Aktualni souradnice pro astro vypocty (delka / sirka): ")
    if(GeoLon >= 0)  Serial.print("[E] ");
    else             Serial.print("[W] ");
    Serial.print(GeoLon,1);
    Serial.print(" / ");
    if(GeoLat >= 0)  Serial.print("[N] ");
    else             Serial.print("[S] ");   
    Serial.println(GeoLat,1);

    if (trackuj == true)
      {
        Serial.print(lng356);                                    //             Serial.print("Trasovani do souboru 'trck")
        Serial.print(GPS_track_cesta[9]);                        //                 (GPS_track_cesta[] = "TRCK/trck1234.gpx")
        Serial.print(GPS_track_cesta[10]);
        Serial.print(GPS_track_cesta[11]);
        Serial.print(GPS_track_cesta[12]);
        Serial.println(lng357);                                  //             Serial.print(".gpx' ZAPNUTO.")
      }
    else
      {
        Serial.println(lng358);                                  //             Serial.print("Trasovani do GPX souboru vypnuto.")        
      }

                                                               //  vypis napajecich napeti
    cti_napajeni();

    Serial.println(' ');

    Serial.print(lng273);                                      // "Posledni merene napeti zdroje (5V-30V): "
    uint16_t RTC_9V = (RTC->BKP1R);                            //  posledni ulozene napajeci napeti pred zapnutim USB
    Serial.print(RTC_9V / 10.0);
    Serial.println(lng384);                                    // "[V] (nyni na USB)"
    
    Serial.print(lng274);                                      // "Napeti zalohovaci baterie (3V): "
    Serial.print(bat_3V);
    Serial.print("[V] (");
    Serial.print(bat_proc);
    Serial.println("%)");
    if (osazeno_uref == true)                                  // referencni napeti se zobrazuje jen kdyz je reference skutecne osazena
      {
        Serial.print(lng275);                                  // "Reference (1V8): "
        Serial.print(ref_in);
        Serial.println("[V]");    
      }

    Serial.print(lng276);                                      // "Napeti procesoru (3V3): "
    Serial.print(Vcc);
    Serial.println("[V]");

    Serial.println(' ');
    vypis_budiku(2);                                           // vypise budiky pro DS3231 (neaktivni budiky vynechava)

   
    Serial.println(lng160);                                    //             Serial.println("\r\nKalibrace cidla teploty");
    Serial.println(lng161);                                    //             Serial.println("index ; merena  ; skutecna");
    Serial.println("-----------------------------");
    for (uint8_t i = 0 ; i < 7 ; i++)
      {
        pomprom = EEPROM_read_int((i*4)+eeaddr_teplotni_kalibrace);
        Serial.print(i+1);
        Serial.print(" ; ");
        dopln_mezery(citelna_teplota(pomprom),5);
        Serial.print( citelna_teplota(pomprom),2);             // (citelna merena teplota)
        Serial.print(" 'C ; ");

        pomprom = EEPROM_read_int((i*4)+eeaddr_teplotni_kalibrace + 2);
        dopln_mezery(citelna_teplota(pomprom),5);
        Serial.print( citelna_teplota(pomprom),2);             // (citelna zobrazovana teplota)
        Serial.println(" 'C");        
      }


    Serial.print(lng162);                                      //             Serial.print("\r\nKalibrace cidla plosneho jasu [");
    Serial.print(index_cidla_svetla);
    Serial.println("]");
    Serial.println(lng163);                                    //             Serial.println("index ; merene ; skutecne");
    Serial.println("--------------------------");
    uint16_t posun = ((index_cidla_svetla-1) * 60) + eeaddr_svetelna_kalibrace; 
    for (uint8_t i = 0 ; i < 15 ; i++)
      {
        if( i < 9)   Serial.print(' ');                        // jen zarovnani cisel polozek
        Serial.print(i+1);
        Serial.print(" ; ");
        pomprom = EEPROM_read_int(posun + (i*4));
        dopln_mezery(citelne_svetlo(pomprom),5);
        Serial.print(citelne_svetlo(pomprom),3);
        Serial.print(" ; ");
        pomprom = EEPROM_read_int(posun + (i*4) + 2);
        dopln_mezery(citelne_svetlo(pomprom),5);
        Serial.println(citelne_svetlo(pomprom),3);
      }    
  
  }
//----------------------------------------------



//---------------------------------------------------------------------
// formatovani vypisu kalibracnich konstant
//   cislo se hned na zacatku oseka o desetinne casti
//  'pocet_mist' udava, na jak dlouhy retezec se doplni cela cast cisla
//   priklady parametru:    15.354 , 4     prida pred retezec 2 mezery
//                          1.078  , 4     prida pred retezec 3 mezery
//                         -24.11  , 3     neprida zadnou mezeru
void dopln_mezery(int16_t cislo, uint8_t pocet_mist)
  {
    uint8_t delka_cisla = 5;
    if (cislo <     0)   delka_cisla ++;                       // zaporne cislo pridava jeden znak pro znamenko
    cislo = abs(cislo);
    if (cislo < 10000)   delka_cisla --;
    if (cislo <  1000)   delka_cisla --;
    if (cislo <   100)   delka_cisla --;
    if (cislo <    10)   delka_cisla --;

    while (delka_cisla < pocet_mist)
      {
        delka_cisla ++;
        Serial.print(' ');
      }
    
  }
//----------------------------------------------


//----------------------------------------------
// vypis aktualniho nastaveni funkci pro RGB LED
void RGB_LED_info(void)
  {
    Serial.println(lng290);                                                 // " Funkce RGB LED:"
    Serial.print(lng286);                                                   // "   LED baterie:         "
    if (bitRead(signal_LED,0) == true) Serial.println(lng153);              // "ANO"
    else                               Serial.println(lng154);              // "NE"

    Serial.print(lng287);                                                   // "   LED pri mereni:      "
    if (bitRead(signal_LED,1) == true) Serial.println(lng153);              // "ANO"
    else                               Serial.println(lng154);              // "NE
  
    Serial.print(lng288);                                                   // "   LED pri chybe:       "
    if (bitRead(signal_LED,2) == true) Serial.println(lng153);              // "ANO"
    else                               Serial.println(lng154);              // "NE

    Serial.print(lng289);                                                   // "   LED pri komunikaci: "
    if (bitRead(signal_LED,3) == true) Serial.println(lng153);              // "ANO"
    else                               Serial.println(lng154);              // "NE

    Serial.print(lng300);                                                   // "   LED pri AfD:        "
    if (bitRead(signal_LED,4) == true) Serial.println(lng153);              // "ANO"
    else                               Serial.println(lng154);              // "NE

    Serial.print(lng355);                                                   // "   LED pri GPS tracku  "
    if (bitRead(signal_LED,5) == true) Serial.println(lng153);              // "ANO"
    else                               Serial.println(lng154);              // "NE

  
  }


//----------------------------------------------
// vypis aktualniho nastaveni funkci pro signalizaci pipanim
void BEEP_info(void)
  {
    Serial.println(lng330);                                                 // " Signalizace piskaku:"
    Serial.print(lng331);                                                   // "   baterie:                "
    if (bitRead(signal_BEEP,0) == true) Serial.println(lng153);             // "ANO"
    else                                Serial.println(lng154);             // "NE"

    Serial.print(lng332);                                                   // "   mereni:                 "
    if (bitRead(signal_BEEP,1) == true) Serial.println(lng153);             // "ANO"
    else                                Serial.println(lng154);             // "NE
  
    Serial.print(lng333);                                                   // "   budik a odpocet:        "
    if (bitRead(signal_BEEP,2) == true) Serial.println(lng153);             // "ANO"
    else                                Serial.println(lng154);             // "NE

    Serial.print(lng334);                                                   // "   HW problemy:            "
    if (bitRead(signal_BEEP,3) == true) Serial.println(lng153);             // "ANO"
    else                                Serial.println(lng154);             // "NE

    Serial.print(lng335);                                                   // "   operace s TS tlacitkem: "
    if (bitRead(signal_BEEP,4) == true) Serial.println(lng153);             // "ANO"
    else                                Serial.println(lng154);             // "NE

    Serial.print(lng336);                                                   // "   priblizovani k Ra-Dec:  "
    if (bitRead(signal_BEEP,5) == true) Serial.println(lng153);             // "ANO"
    else                                Serial.println(lng154);             // "NE

    Serial.print(lng337);                                                   // "   dalsi vystrahy:         "
    if (bitRead(signal_BEEP,6) == true) Serial.println(lng153);             // "ANO"
    else                                Serial.println(lng154);             // "NE

    Serial.print(lng338);                                                   // "   stopky:                 "
    if (bitRead(signal_BEEP,7) == true) Serial.println(lng153);             // "ANO"
    else                                Serial.println(lng154);             // "NE

    Serial.print(lng353);                                                   // "   zapnuti [Z]:            "
    if (bitRead(signal_BEEP,8) == true) Serial.println(lng153);             // "ANO"
    else                                Serial.println(lng154);             // "NE


    
  }



//----------------------------------------------
void default_kalibrace_svetla(void)
  {

    for (uint8_t i = 1; i < 8 ; i++)
      {
        kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce
        // v EEPROM se nachazi 7 bloku kalibracnich dat (pro 7 cidel na rozsirujici desce), kazdy blok ma 15 bodu, kazdy bod ma 2 + 2 bajty (merena a skutecna hodnota) = 60 bajtu na blok
        //               bloky zacinaji na adrese 50 (za kalibracni tabulkou teplot)
        // Nasledujici zapis nulovych hodnot by se dal udelat jednoduchou smyckou, ale pro pripad, ze by se ukazalo, ze je lepsi zapisovat nejake jine hodnoty jsem to nechal oddelene
        
        uint16_t posun = ((i-1) * 60) + eeaddr_svetelna_kalibrace;   
        EEPROM_write_int( 0 + posun   ,     0);                //  1. kalibracni bod pro svetlo -  index[0]
        EEPROM_write_int( 2 + posun   ,     0);
    
        EEPROM_write_int( 4 + posun   ,     0);                //  2. kalibracni bod pro svetlo -  index[1]
        EEPROM_write_int( 6 + posun   ,     0);
    
        EEPROM_write_int( 8 + posun   ,     0);                //  3. kalibracni bod pro svetlo -  index[2]
        EEPROM_write_int(10 + posun   ,     0);
    
        EEPROM_write_int(12 + posun   ,     0);                //  4. kalibracni bod pro svetlo -  index[3]
        EEPROM_write_int(14 + posun   ,     0);   
    
        EEPROM_write_int(16 + posun   ,     0);                //  5. kalibracni bod pro svetlo -  index[4]
        EEPROM_write_int(18 + posun   ,     0);
    
        EEPROM_write_int(20 + posun   ,     0);                //  6. kalibracni bod pro svetlo -  index[5]
        EEPROM_write_int(22 + posun   ,     0);
    
        EEPROM_write_int(24 + posun   ,     0);                //  7. kalibracni bod pro svetlo -  index[6]
        EEPROM_write_int(26 + posun   ,     0);
    
        EEPROM_write_int(28 + posun   ,     0);                //  8. kalibracni bod pro svetlo -  index[7]
        EEPROM_write_int(30 + posun   ,     0);

        kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
    
        EEPROM_write_int(32 + posun   ,     0);                //  9. kalibracni bod pro svetlo -  index[8]
        EEPROM_write_int(34 + posun   ,     0);
    
        EEPROM_write_int(36 + posun   ,     0);                // 10. kalibracni bod pro svetlo -  index[9]
        EEPROM_write_int(38 + posun   ,     0);
    
        EEPROM_write_int(40 + posun   ,     0);                // 11. kalibracni bod pro svetlo -  index[10]
        EEPROM_write_int(42 + posun   ,     0);
    
        EEPROM_write_int(44 + posun   ,     0);                // 12. kalibracni bod pro svetlo -  index[11]
        EEPROM_write_int(46 + posun   ,     0);
    
        EEPROM_write_int(48 + posun   ,     0);                // 13. kalibracni bod pro svetlo -  index[12]
        EEPROM_write_int(50 + posun   ,     0);
        
        EEPROM_write_int(52 + posun   ,     0);                // 14. kalibracni bod pro svetlo -  index[13]
        EEPROM_write_int(54 + posun   ,     0);
    
        EEPROM_write_int(56 + posun   ,     0);                // 15. kalibracni bod pro svetlo -  index[14]
        EEPROM_write_int(58 + posun   ,     0);
        
      }
  }
//----------------------------------------------



//----------------------------------------------
void default_kalibrace_teploty(void)
  {
    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce
    // Nasledujici zapis hodnot 0 by se dal udelat jednoduchou smyckou, ale pro pripad, ze by se ukazalo, ze je lepsi zapisovat nejake jine hodnoty jsem to nechal oddelene
    EEPROM_write_int(eeaddr_teplotni_kalibrace       ,  0);                               // 1. kalibracni bod pro teplotu -  index[0]   
    EEPROM_write_int(eeaddr_teplotni_kalibrace + 2   ,  0);

    EEPROM_write_int(eeaddr_teplotni_kalibrace + 4   ,  0);                               // 2. kalibracni bod pro teplotu -  index[1]   
    EEPROM_write_int(eeaddr_teplotni_kalibrace + 6   ,  0);

    EEPROM_write_int(eeaddr_teplotni_kalibrace + 8   ,  0);                               // 3. kalibracni bod pro teplotu -  index[2]   
    EEPROM_write_int(eeaddr_teplotni_kalibrace +10   ,  0);

    EEPROM_write_int(eeaddr_teplotni_kalibrace +12   ,  0);                               // 4. kalibracni bod pro teplotu -  index[3]   
    EEPROM_write_int(eeaddr_teplotni_kalibrace +14   ,  0);

    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    

    EEPROM_write_int(eeaddr_teplotni_kalibrace +16   ,  0);                               // 5. kalibracni bod pro teplotu -  index[4]   
    EEPROM_write_int(eeaddr_teplotni_kalibrace +18   ,  0);

    EEPROM_write_int(eeaddr_teplotni_kalibrace +20   ,  0);                               // 6. kalibracni bod pro teplotu -  index[5]   
    EEPROM_write_int(eeaddr_teplotni_kalibrace +22   ,  0); 

    EEPROM_write_int(eeaddr_teplotni_kalibrace +24   ,  0);                               // 7. kalibracni bod pro teplotu -  index[6]   
    EEPROM_write_int(eeaddr_teplotni_kalibrace +26   ,  0); 
  }

// #doc#12
void default_parametry(void)
  {
    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
    EEPROM_write_long(    eeaddr_RTC_set       ,            0);                 // cas posledniho serizeni = 1.1.1970 0:00:00
    EEPROM_write_long(    eeaddr_RTC_korekce   , 0x7FFFFFFFUL);                 // sekunda se pricita jednou za 68 let
    EEPROM_write(         eeaddr_leto_zima     ,            0);                 // SEC
    EEPROM_write(         eeaddr_oddelovace    ,   0b00010110);                 // des. oddelovac carka, oddelovac polozek strednik, hlavicku vkladat, nezavirat do uvozovek
    EEPROM_write(         eeaddr_automat       ,            0);                 // automaticke spousteni vypnute
    EEPROM_write(         eeaddr_prumerovani   ,            3);                 // 3x prumerovani
    if (osazeno_DISPLEJ == true)
      {
        EEPROM_write(     eeaddr_modbus_LED    ,   0b00000011);                 // osazeny displej = displejova verze SQM; modbus pres RS485 je defaultne zapnuty    
      }
    else
      {
        EEPROM_write(     eeaddr_modbus_LED    ,   0b00000001);                 // neosazeny displej = LED verze SQM; modbus pres RS485 je defaultne zapnuty    
      }
      
    EEPROM_write(         eeaddr_LED           ,            0);                 // RGB LED je defaultne vypnuta pro vsechny operace (zapina se pres @L_)
    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
    EEPROM_write_int(     eeaddr_beep_bity     ,  0b111111111);                 // veskere pipani je povolene (zatim pouzito 9 spodnich bitu)
    EEPROM_write(         eeaddr_elevace_soumrak  ,        10);                 // uzivatlsky nastavitalna elevace Slunce pro soumrak
    EEPROM_write(         eeaddr_SLAVE_baud    ,            1);                 // komunikacni rychlost 9600; SLAVE adresa = 1
    EEPROM_write(         eeaddr_CRC_tset_lock ,   0b00000001);                 // testuje kontrolni bajt pri komunikaci pres RS485; tlacitko TS je odemknute, RTC_set.csv zakazan
    EEPROM_write(         eeaddr_stab          ,           20);                 // nestabilni jas je hlaseny pri rozdilu dvou sousednich mereni nad 2 %
    EEPROM_write(         eeaddr_pocet_cidel   ,            1);                 // pocet cidel na expanzni desce
    EEPROM_write(         eeaddr_10kB_znacka   ,            0);                 // zachytny bod hledani (po kazdem 100. zaznamu se aktualizuje kvuli rychlejsimu hledani volneho mista v EEPROM)
    EEPROM_write(         eeaddr_uroven_EEPROM ,            0);                 // hranice plosneho jasu pro ukladani do EEPROM a na kartu v desetinach mag/arcsec2) (0 az 25,5 mag/arcsec2)
    EEPROM_write_int(     eeaddr_GEO_lat       ,          500);                 // domaci zemepisna sirka pro astro vypocty na 50.0 stupnu
    EEPROM_write_int(     eeaddr_GEO_lon       ,          150);                 // domaci zemepisna delka pro astro vypocty na 15.0 stupnu
    EEPROM_write(         eeaddr_zima_hod      ,          101);                 // posun casu proti UTC v zime = +1 hodina (SEC)      (hodnota je zvetsena o 100, takze -5 hodin by se zapsalo jako 95)
    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
    EEPROM_write(         eeaddr_leto_hod      ,          102);                 // posun casu proti UTC v lete = +2 hodiny (SELC)
    EEPROM_write_long(    eeaddr_menu_bity     , 0xFFFFFFFFUL);                 // defaultne jsou vsechny polozky v menu povolene
    EEPROM_write_int(     eeaddr_perif_bity    ,       0x001F);                 // defaultne je vsech 5 periferii zapnutych (neosazene periferie se pak vypinaji automaticky)
    EEPROM_write(         eeaddr_AfD           ,          255);                 // Alam for Darkness vypnuty (nastaveny na tak vysokou hodnotu, ktera nemuze nastat - 25,5 mag/arcsec2)
    EEPROM_write(         eeaddr_txt_zony      ,          ' ');                 // 4 ASCII znaky pro 'zimni' casovou zonu
    EEPROM_write(         eeaddr_txt_zony + 1  ,          'S');                 // 
    EEPROM_write(         eeaddr_txt_zony + 2  ,          'E');                 // 
    EEPROM_write(         eeaddr_txt_zony + 3  ,          'C');                 // 
    EEPROM_write(         eeaddr_txt_zony + 4  ,          'S');                 // 4 ASCII znaky pro letni casovou zonu
    EEPROM_write(         eeaddr_txt_zony + 5  ,          'E');                 // 
    EEPROM_write(         eeaddr_txt_zony + 6  ,          'L');                 // 
    EEPROM_write(         eeaddr_txt_zony + 7  ,          'C');                 // 
    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
    EEPROM_write_int(     eeaddr_luxmetr       ,          250);                 // parametr 'a' pro kalibraci luxmetru (*0.01)   (s ulozenym cislem 250 se bude pocitat jako s 2.5)
    EEPROM_write_int(     eeaddr_luxmetr + 2   ,            0);                 // parametr 'b' pro kalibraci luxmetru (*0.01)  zaporna cisla se ukladaji jako binarni doplnek (-10.55 = 64481)
    EEPROM_write_int(     eeaddr_luxmetr + 4   ,            0);                 // parametr 'c' pro kalibraci luxmetru (*0.000 000 1) ulozene cislo 16 by se pocitalo jako 0.000 001 6
    EEPROM_write(         eeaddr_kompas_offset ,          127);                 // doladeni kompasu (127 = pootoceni o 0 stupnu)
    EEPROM_write_int(     eeaddr_alarmy        ,         1440);                 // prvni  budik vypnout
    EEPROM_write_int(     eeaddr_alarmy +  2   ,         1440);                 // druhy  budik vypnout
    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
    EEPROM_write_int(     eeaddr_alarmy +  4   ,         1440);                 // treti  budik vypnout
    EEPROM_write_int(     eeaddr_alarmy +  6   ,         1440);                 // ctvrty budik vypnout
    EEPROM_write_int(     eeaddr_alarmy +  8   ,         1440);                 // paty   budik vypnout
    EEPROM_write_int(     eeaddr_alarmy + 10   ,         1440);                 // odpocet (sesty budik) vypnout
    EEPROM_update(        eeaddr_TEST_alarm    ,            0);                 // znacka testovaciho alarmu se maze
    EEPROM_write_int(     eeaddr_GPS_track_id  ,            0);                 // cislovani trasovacich souboru od nuly a zastavit trasovani
    EEPROM_write(         eeaddr_GPS_track_interval ,      20);                 // defaultne se trackuje po 20 sekundach
    EEPROM_write(         eeaddr_AUTOkom       ,            0);                 // defaultne se hodnoty do seriovych linek neodesilaji (rezim "master" je VYPNUTY)
    EEPROM_write(         eeaddr_MOD_autoupdate,            0);                 // defaultne se modbusove registry 30001 az 30031 po kazdem mereni neaktualizuji (musi se vyzadat prikazem 2)
    EEPROM_write(         eeaddr_bortle        ,            0);                 // defaultni zobrazeni Bortleho stupne v menu zakazano
    EEPROM_write_int(     eeaddr_DSO_setup     ,       0x0003);                 // nastaveni paramnetru funkce pro urcovani viditelnosti DSO (schopnosti=3, ostatni parametry 0, 0)
    EEPROM_write(         eeaddr_DSO_normal_MSA,          210);                 // normalovy jas oblohy pro urcovani viditelnosti otevrenych hvezdokup
    EEPROM_write(         eeaddr_AFD_styl      ,            0);                 // AFD po zapnuti napajeni nebude signalizovat, ale bude logovat

    EEPROM_write(         eeaddr_servis        ,            0);                 // adresa pro vyvoj (testovani novych funkci je ve verejnych verzich programu vypnute)

    
// #doc#21
    // defaultni hodnoty prumeru a ohniskovych vzdalenosti dalekohledu a okularu
    EEPROM_write_int(     eeaddr_DSO_prumery     ,     200);                 // 200 mm
    EEPROM_write_int(     eeaddr_DSO_prumery + 2 ,      70);                 // 70 mm
    EEPROM_write_int(     eeaddr_DSO_prumery + 4 ,     150);                 // 150 mm

    EEPROM_write_int(     eeaddr_DSO_ohniska     ,    1000);                 // 1000 mm
    EEPROM_write_int(     eeaddr_DSO_ohniska + 2 ,     400);                 // 400 mm
    EEPROM_write_int(     eeaddr_DSO_ohniska + 4 ,     700);                 // 700 mm

    EEPROM_write(         eeaddr_DSO_okulary     ,     24);                  // 24 mm          // po resetu se tento okular navoli automaticky
    EEPROM_write(         eeaddr_DSO_okulary + 1 ,    100);                  // 100 mm
    EEPROM_write(         eeaddr_DSO_okulary + 2 ,     16);                  // 16 mm
    EEPROM_write(         eeaddr_DSO_okulary + 3 ,      8);                  // 8 mm
    EEPROM_write(         eeaddr_DSO_okulary + 4 ,      5);                  // 5 mm



    if (modul_LSM303DLHC == true)
      {
        kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
        default_naklonomer();                                  // prednastaveni defaultnich hodnot pro naklonomer
        kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
        default_kompas();                                      // prednastaveni defaultnich hodnot pro kompas
      }
    default_stanoviste();                                      // prednastaveni popisku pro 5 pozorovacich stanovist 
     
  }
//----------------------------------------------



//----------------------------------------------
void default_stanoviste(void)
  {
    for (uint8_t i = 1; i < 6 ; i ++)
      {
        kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
        
        for (uint8_t j = 0; j < 10 ; j ++)
          {
            EEPROM_write(eeaddr_stanoviste + ((i-1) * 15) + j , poz_st_txt[i].charAt(j));            
          }

        for (uint8_t j = 0; j < 5 ; j ++)
          {
            EEPROM_write(eeaddr_stanoviste + 10 + ((i-1) * 15) + j , poz_st_gr[i][j]);            
          }
      } 
  }
//----------------------------------------------



//----------------------------------------------
void chyba(uint8_t kod_chyby)
  {
    bool povol_pipnuti_i2c = false;                                // moznost blokovani pipnuti pri prvni chybe I2C (pipa se az po druhe chybe)

    if (kod_chyby == 3 and sd_out_err == false) return;            // vysunuta SD karta ("No-Sd") se hlasi jen 1x pri startu, nebo pri prvnim pokusu o zapis po vytazeni SD karty    

    
    if (kod_chyby != 3 and kod_chyby != 2)                         // logujou se vsechny chyby krome chyb SD karty (neni kam logovat, zpusobilo by to zablokovani programu)
      {
        SD_log(998 , kod_chyby);                                   // kod chyby se loguje do zvlastniho logovaciho souboru na SD karte: (kod/adresa = 998, data = cislo chyby)        
      }
    
    switch(kod_chyby)
      {
        case 1:                                                    // chyba I2C komunikace (cidlo svetla, EEPROM, expander, naklonomer, kompas)
          if (blokuj_err_i == true)                                // kvuli strhavani I2C sbernice pri zapnuti GPS se hlasi az dalsi chyba I2C
            {                                                      //  kdyz je hlaseni chyb I2C blokovane, tak se na 5 sekund odblokuje
              blokuj_err_i = false;                                // odblokovani zobrazeni chyb I2C sbernice
              cas_odblokovani_err_i = millis();                    // zaznamenani casu odblokovani hlaseni I2C chyb (po 5 sekundach se znova zapne blokovani hlaseni chyb I2C)              
            }
          else                                        
            {
              if (millis() - cas_odblokovani_err_i < 5000)         // kdyz je odblokovano, a cas od prechoziho odblokovani je kratsi nez 5 sekund, muze se zobrazit chybove hlaseni
                {
                  zobraz_text(40);                                 //   "Err-i"
                  povol_pipnuti_i2c = true;
                }
              blokuj_err_i = true;                                 // po sude chybe se dalsi hlaseni I2C chyb zase zablokuje
            }
          bitSet(err_bit,0);                                       // nastavi se chybovy bit v promenne 'err_bit'
          Wire.begin();                                            // pokus o restart I2C
          break;

        case 2:                                                    // chyba SD karty
          zobraz_text(37);                                         //   Sd-Er
          bitSet(err_bit,1);                                       // nastavi se chybovy bit v promenne 'err_bit'
          break;

        case 3:                                                    // neni zasunuta SD karta
          zobraz_text(45);                                         //   no-5d  (No-SD)
          bitSet(err_bit,2);                                       // nastavi se chybovy bit v promenne 'err_bit'
          sd_out_err = false;
          break;

        case 4:                                                    // Spatny datum v RTC
          zobraz_text(61);                                         //   Err-r
          bitSet(err_bit,3);                                       // nastavi se chybovy bit v promenne 'err_bit'
          break;

        case 5:                                                    // Chyba v kalibracni tabulce svetla
          zobraz_text(30);                                         //   Err-S
          break;

        case 6:                                                    // Chyba v kalibracni tabulce teploty
          zobraz_text(31);                                         //   Err-t
          break;

        case 7:                                                    // blokovane bocni tlacitko
          zobraz_text(36);                                         //   -Loc-
          break;

        case 8:                                                    // chyba naklonomeru
          zobraz_text(55);                                         //   Err-n
          bitSet(err_bit,4);                                       // nastavi se chybovy bit v promenne 'err_bit'
          break;

        case 9:                                                    // chyba kalibrace kompasu
          zobraz_text(60);                                         //   Err-C
          break;

        case 10:                                                   // chyba formatovani EEPROM
          zobraz_text(63);                                         //   Err-F
          break;

        default:
          break;
      }

    if (pocet_bzuku > 0)                                           // od zapnuti napajeni je pocet pipnuti pri chybach omezen na 5 (to je proto, aby se pri opakovanych chybach komunikace nebzukalo nepretrzite)
      {
        pocet_bzuku --;
        for (uint8_t bz = 0; bz < 3 ; bz++ )                          // pri chybe se jeste 3x rychle zapipa
          {
            switch(kod_chyby)
              {
                case 1:                                            // u chyb I2C se da vypnout pipani zvlast
                  if (povol_pipnuti_i2c == true)                   // ale zaroven se jeste zohlednuje, jestli neni hlaseni prvni chyby blokovane
                    {
                      tone_X(pin_bzuk,600, 10 , 3);                // bit 3 = HW problemy                  
                    }
                  break;

                
                case 2:                                            // u bloku dalsich HW chyb se da vypnout pipani zvlast
                case 3:
                case 4:
                case 8:
                case 9:
                case 10:
                  tone_X(pin_bzuk,600, 10 , 3);                    // bit 3 = HW problemy
                  break;

                case 5:                                            // u bloku spatnych kalibraci se da vypnout pipani zvlast
                case 6:
                  tone_X(pin_bzuk,600, 10 , 6);                    // bit 6 = dalsi vystrahy
                  break;

                case 7:                                            // u operaci s TS tlacitkem se da vypnout pipani zvlast
                  tone_X(pin_bzuk,600, 10 , 4);                    // bit 4 = operace s TS tlacitkem
                  break;
              }     
            
            delay(100);
          }    
      }
    
    if (bitRead(signal_LED,2) == true)                             // kdyz je povolena signalizace chyb pomoci modre LED
      {
        LED_B_onoff(true);                                         // rozsviti modrou LED
      }

    
    delay(500);
    
    zobraz_text(66);                                               // po pul sekunde se zhasne displej
    LED_B_onoff(false);                                            // zhasne modrou LED        
  }
//----------------------------------------------



//----------------------------------------------
// prepinani cidel svetla
//   Kdyz je index mimo povoleny rozsah 1 az "pocet_cidel_svetla", nic se nestane
//  kdyz je index zadany, prepne se na konkretni cislo
void prepni_cidlo_svetla(uint8_t index)
  {
    if (index <= pocet_cidel_svetla and index > 0)
      {
        index_cidla_svetla = index;
        
        // nacist novou kalibracni tabulku z EEPROM podle aktualne prepnuteho cidla svetla
        uint16_t posun = ((index_cidla_svetla-1) * 60) + eeaddr_svetelna_kalibrace;   // podle aktualniho indexu cidla svetla se zvoli spravna oblast v EEPROM pro kalibracni tabulku
        for (uint8_t i = 0 ; i < 15 ; i++)
          {
            kal_svetlo[i][0] = EEPROM_read_int(posun   +   (i*4));
            kal_svetlo[i][1] = EEPROM_read_int(posun + 2 + (i*4));
          }
        expa_switch(index_cidla_svetla);                           // prepnout napajeni na prislusne cidlo
      }
  }
//----------------------------------------------



//----------------------------------------------
// kdyz parametr zadany neni, prepne se na nasledujici cidlo
void prepni_cidlo_svetla(void)
  {
    index_cidla_svetla ++;
    if (index_cidla_svetla > pocet_cidel_svetla) index_cidla_svetla = 1;
    prepni_cidlo_svetla(index_cidla_svetla);    
  }
//----------------------------------------------



//----------------------------------------------
// pri dlouhych operacich (nebo ve smyckach, ktere bezi mimo hlavni loop) se pravidelne kontroluje, jestli doslo k nejakemu pozadavku o komunikaci,
// nebo jestli uz neni cas provest automaticke spusteni mereni
void interrupty(void)
  {   
    if (pouzit_modbus == true) test_485_mod();                        // testovani, jestli neprislo neco po seriove lince RS485
    else                       test_485_kom();

    test_ser_kom();                                                   // testovani, jestli neprislo neco po USB lince

    if (pouzit_ds3231 == true)
      {
        if (millis() - posledni_time_synchro > 3600000UL)             // po hodine behu se provadi synchronizace casu v DS3231 podle casu v STM32
          {
            STM_DS(false);
          }
    
        if (blokuj_3231 == false)                                     // alarmy je mozne trvale (do pristiho resetu) zablokovat siskem vsech 3 tlacitek pri poslednim alarmu
          {
            if ((RTC3231_read(0x0F) & 0b00000001) == 1 and (RTC3231_read(0x0E) & 0b00000001) == 1)       // Alarm Flag od DS3231 je aktivni (doslo k alarmu) a zaroven je povolene A1IE
              {
                alarm_3231();
              }
          }
      }

    if (pouzivat_SD_kartu == true and digitalRead(pin_karta_IN) == LOW)       // kdyz je nastaveno pouzivani SD karty a karta je zasunuta
      {
        if (millis() - posledni_SD_free > 7200000UL)                          // tk se kazde dve hodina aktualizuje informace o volnem miste na SD karte
          {
            SD_volno = SDfree(sd)>>20;                                        // Zjisteni volneho prostoru na SD karte (prevod z bajtu na MB)
            posledni_SD_free = millis();
          }
      }

    if (digitalRead(pin_TS) == LOW and blokuj_TS == false and TS_lock == false) // pri zkratu pinu SCK a GND na bocnim konektoru desky BlackPill + se zaznamena casove razitko
      {
        blokuj_TS = true;                                             // zablokovani dalsiho zapisu casove znacky az do uvolneni tlacitka
        casove_razitko();
      }

    if (digitalRead(pin_TS) == HIGH)
      {
        blokuj_TS = false;                                            // odblokovani dalsiho zapisu casove znacky po uvolneni tlacitka
      }

    if (automat > 0)                                                  // kdyz je nastavene automaticke snimani
      {
        if (millis() - posledni_autospusteni > (automat * 60000UL))   // automat udava cas v minutach - to je (60 * 1000ms)
          {
            posledni_autospusteni = millis();  
            mereni(6);                                                // spusteni automatickeho mereni          
          }
      }      

    if (spec_rezim == 100)                                            // v kazdem interruptu se muze odskocit na specialni rezim //!! (r30s)
      {
        rezim30s();
      }        

    if (trackuj == true)                                              // kdyz je zapnute GPS trasovani, testuje se, ze uz ubehnul dostatecny cas od posledniho zaznamu tracku
      {
        if (millis() > (posledni_zaznam_tracku + (track_interval * 1000)))
          {
            track_GPS();
          }
      }
  
  
  }
//----------------------------------------------



//----------------------------------------------
// Prepise jen jeden bit v kazdem zaznamu, takze je prostor oznaceny jako prazdny.
// Data jsou ale az do dalsiho prepsani citelna (ve vypisech ze poznaji podle toho, ze misto napisu " SEC" nebo "SELC" maji zobrazeno " _EC", nebo "_ELC")
void soft_format_EEPROM(void)
  {
    zobraz_text(66);                                                  // smaze displej
    Serial.println(lng164);                                           // Serial.println("SOFT format EEPROM ...");    // bargraf do serioveho terminalu
    uint16_t pocitadlo = 0;

    pomlcky((((eeaddr_MAX_EEPROM-eeaddr_MIN_EEPROM) / delka_zaznamu) / 100));

    Serial.print('|');

    zobraz_cislo((((eeaddr_MAX_EEPROM-eeaddr_MIN_EEPROM) / delka_zaznamu) / 100) - (eeaddr_MIN_EEPROM / delka_zaznamu  / 100) , 7);     // vystup odpocitavani na displej podle delky zaznamu se zacina na 34 az 160 (pro 3400 az 16000 zaznamu) 

    dokoncene_mereni = 0;                                             // je to dlouha operace, behem ktere je blokovano spusteni dalsiho mereni pres RS485    
    for (uint32_t adresa = eeaddr_MIN_EEPROM; adresa <= max_zaznam_EEPROM ; adresa = adresa + delka_zaznamu)
      {
        test_485_kom();                                               // test, jestli neprisel nejaky pozadavek po komunikaci RS485   
        pocitadlo ++;
        if ((pocitadlo % 100) == 0)
          {
            Serial.print('#');
            zobraz_cislo((((eeaddr_MAX_EEPROM-eeaddr_MIN_EEPROM) / delka_zaznamu) / 100) - (adresa / delka_zaznamu  / 100) , 7);        // vystup odpocitavani na displej podle delky zaznamu se zacina na 34 az 160 (pro 3400 az 16000 zaznamu) 
          }
        uint8_t pomprom = EEPROM_read(adresa);
        if ((pomprom & 0b10000000) > 0)                               // pokud je v EEPROM na zacatku bloku nastaveny bit7 na '1', znamena to obsazeny blok a tato '1' se smaze
          {
            delay(3);
            EEPROM_write(adresa , (pomprom & 0b01111111));            // bit7 se nastavi na '0' 
          }

         if (digitalRead(pin_tl_ok) == LOW)                           // moznost predcasneho ukonceni tlacitkem OK
           {
              delay(50);
              uint32_t exit_znacka = millis();
              while (digitalRead(pin_tl_ok) == LOW)                   // ceka se na uvolneni tlacitka OK
                {
                  graf_natazeni(millis() - exit_znacka , 113);        // zaroven se na displeji zobrazuje pomlckovy bargraf
                  delay(20);
                }
              if (millis() - exit_znacka > 2000)                      // kdyz stisk trval alespon 2 sekundy, smycka for se prerusi 
                {
                  chyba(10);                                          // "Err-F"
                  delay(1000);
                  break;              
                }
              else zobraz_cislo((((eeaddr_MAX_EEPROM-eeaddr_MIN_EEPROM) / delka_zaznamu) / 100) - (adresa / delka_zaznamu  / 100) , 7);
           }
      
      }
    EEPROM_write(eeaddr_10kB_znacka ,0);                              // zachytny bod pro pristi hledani volneho mista v EEPROM se nastavi na zacatek (adresa 'eeaddr_MIN_EEPROM')
    
    dokoncene_mereni = 2;                                             // po skonceni formatovani se odblokuje zablokovane spousteni dalsiho mereni pres RS485

    adr_posl_zaznamu = 0;                                             // nula znamena, ze jeste neni zadny zaznam. Pouziva se pri blokovani vypisu okamzite po formatu.
    
    Serial.println('|');
    Serial.println(lng166);                                           //             Serial.println("Formatovani EEPROM ukonceno");

    delay(1000);
    SOFT_RESET();
    
  }
//----------------------------------------------



//----------------------------------------------
// smaze uplne vsechny datove bunky v EEPROM
void hard_format_EEPROM(void)
  {
    zobraz_text(66);                                                  // smaze displej
    Serial.println(lng165);                                           //             Serial.println("HARD format EEPROM ...");        // bargraf do serioveho terminalu

    pomlcky(1+((eeaddr_MAX_EEPROM-eeaddr_MIN_EEPROM) / 1000));
    
    Serial.print('|');

    zobraz_cislo((eeaddr_MAX_EEPROM / 1000) , 5);
    dokoncene_mereni = 0;                                             // je to dlouha operace, behem ktere je blokovano spusteni dalsiho mereni   
    for (uint32_t adresa = eeaddr_MIN_EEPROM; adresa < eeaddr_MAX_EEPROM ; adresa ++)
      {
         test_485_kom();                                              // test, jestli neprisel nejaky pozadavek po komunikaci RS485   
         if (adresa % 1000 == 0)
           {
             Serial.print('#');
                                                                       // vystup odpocitavani na displej (zacina se od 123 a pri kazdem kilobajtu se odecte 1 ) 
             zobraz_cislo((eeaddr_MAX_EEPROM / 1000) - ((adresa - eeaddr_MIN_EEPROM) / 1000) , 5);
           }
         delay(3);
         EEPROM_write(adresa ,0);                                     // smazani bunky
         if (digitalRead(pin_tl_ok) == LOW)                           // moznost predcasneho ukonceni tlacitkem OK
           {
              delay(50);
              uint32_t exit_znacka = millis();
              while (digitalRead(pin_tl_ok) == LOW)                   // ceka se na uvolneni tlacitka OK
                {
                  graf_natazeni(millis() - exit_znacka , 113);        // zaroven se na displeji zobrazuje pomlckovy bargraf
                  delay(20);
                }
              if (millis() - exit_znacka > 2000)                      // kdyz stisk trval alespon 2 sekundy, smycka for se prerusi 
                {
                  chyba(10);                                          // "Err-F"
                  delay(1000);
                  break;              
                }
              else zobraz_cislo((eeaddr_MAX_EEPROM / 1000) - ((adresa - eeaddr_MIN_EEPROM) / 1000) , 5);
           }
      }
    EEPROM_write(eeaddr_10kB_znacka ,0);                              // zachytny bod pro pristi hledani volneho mista v EEPROM se nastavi na zacatek (adresa 'eeaddr_MIN_EEPROM')

    dokoncene_mereni = 2;                                             // po skonceni formatovani se odblokuje zablokovane spousteni dalsiho mereni pres RS485

    adr_posl_zaznamu = 0;                                             // nula znamena, ze jeste neni zadny zaznam. Pouziva se pri blokovani vypisu okamzite po formatu.
    
    Serial.println('|');
    Serial.println(lng166);                                           //             Serial.println("Formatovani EEPROM ukonceno");
    delay(1000);
    SOFT_RESET();
  
  }
//----------------------------------------------


//----------------------------------------------
// #doc#13
void vypis_LU(void)                                           // vypis posledniho zaznamu ve formatu SQM-LU (USB)
  {
    if (adr_posl_zaznamu > 0)
      {
        priprav_1_zaznam(adr_posl_zaznamu);                   // v promenne 'adr_posl_zaznamu' je ulozena adresa zacatku posledniho zaznamu
        // "r, 06.70m,0000022921Hz,0000000020c,0000000.000s, 039.4C"
        Serial.print("r, ");
        float UHJ = (vystupni_retezec[75]-48) * 10 + vystupni_retezec[76]-48 +  (vystupni_retezec[78]-48) * 0.1 + (vystupni_retezec[79]-48) * 0.01 + (vystupni_retezec[80]-48) * 0.001;
        if (UHJ < 10) Serial.print('0');
        Serial.print (UHJ,2);       
        Serial.print("m,0000022921Hz,0000000020c,0000000.000s,");
        if (vystupni_retezec[151] == '+')   Serial.print(' ');
        else                                Serial.print('-');
        float UHT = (vystupni_retezec[152]-48) * 10 + vystupni_retezec[153]-48 +  (vystupni_retezec[155]-48) * 0.1 + (vystupni_retezec[156]-48) * 0.01;
        if (UHT < 10) Serial.print('0');
        Serial.print (UHT,1);               
        Serial.println('C');        
      }
  }
//----------------------------------------------

  
//----------------------------------------------
void vypis_EEPROM(uint16_t parametr, uint16_t casova_hloubka) // pretizena funkce, ve ktere schazi parametr 'dat'
  {
    vypis_EEPROM(parametr, casova_hloubka, 0);
  }

void vypis_EEPROM(uint16_t parametr, uint16_t casova_hloubka, uint16_t dat) // casova_hloubka je pouzita jen pro zadani, jak stare zaznamy se maji vypsat
  {
    bool prazdno = true;                                              // kdyz nebude ve vypisu ani jeden zaznam, zahlasi se na konci "Zadny ulozeny zaznam"

    if (dat == 0)
      {
        if (parametr != 65531)    print_co_kam(false, false);         // v seriovem terminalu se hlavicka zobrazuje vzdycky (krome funkce LIST z menu)        
      }




    switch (parametr)
      {

        case   65534:                                                 // vypis posledniho zaznamu
          {
            if (adr_posl_zaznamu > 0)
              {
                priprav_1_zaznam(adr_posl_zaznamu);                   // v promenne 'adr_posl_zaznamu' je ulozena adresa zacatku posledniho zaznamu
                print_co_kam(true, false);                            // Serial.println(vystupni_retezec);
                prazdno = false;            
              }
            else
              {
                prazdno = true;            
              }
          }
        break;
        
        
        case  65533:                                                  // vypis vsech zaznamu (zaznamu ktere maji znacku v prvnim bajtu) - nevypisuji se SOFT formatovane zaznamy
          {
            dokoncene_mereni = 0;                                     // je to dlouha operace, behem ktere je blokovano spusteni dalsiho mereni pres seriovou linku nebo pres autoomat

            uint16_t pocitadlo_radek = (max_zaznam_EEPROM - eeaddr_MIN_EEPROM) / delka_zaznamu;
            for (uint32_t adresa = eeaddr_MIN_EEPROM; adresa <= max_zaznam_EEPROM ; adresa = adresa + delka_zaznamu)
              {
                if ((EEPROM_read(adresa) & 0b10000000) != 0)          // polozka v EEPROM je obsazena
                  {
                    prazdno = false;
                    priprav_1_zaznam(adresa);
                    print_co_kam(true, false);                        // Serial.println(vystupni_retezec);

                    test_485_kom();                                   // test, jestli neprisel nejaky pozadavek po komunikaci RS485

                    if (digitalRead(pin_tl_ok) == LOW)                // predcasne ukonceni dlouheho vypisu stiskem tlacitka OK
                      {
                        delay(20);
                        while (digitalRead(pin_tl_ok) == LOW)    delay(20);
                        break;
                      }
                  }

                pocitadlo_radek --;
                if (pocitadlo_radek % 20 == 0)
                  {
                    zobraz_cislo((pocitadlo_radek / 20) , 6);         // vystup odpocitavani na displej
                  }
              }                     
          }
        dokoncene_mereni = 2;                                         // po skonceni vypisu se odblokuje zablokovane spousteni dalsiho mereni

        break;


        case  65530:                                                  // vypis poslednich zaznamu za obdobi poslednich zadanych hodin
          {
            dokoncene_mereni = 0;                                     // je to dlouha operace, behem ktere je blokovano spusteni dalsiho mereni pres RS485
            if (casova_hloubka == 0)     casova_hloubka = 24;         // bez zadane casove hloubky se predpoklada hloubka 24 hodin
            uint32_t historie = rtc_korekce() - (casova_hloubka * 3600UL);    // prevod casu z RTC na mistni cas vcetne autokorekce

            uint16_t pocitadlo_radek = (max_zaznam_EEPROM - eeaddr_MIN_EEPROM) / delka_zaznamu;            
            zobraz_cislo((pocitadlo_radek / 20) , 6);                 // vystup odpocitavani na displej
            bool vypis_STOPLAP = false;                               // u vypisu zaznamu stopek typ "STOP" a "LAP" se neuklada do EEPROM aktualni cas, proto se musi vypisovat podle teto znacky 
            for (uint32_t adresa = eeaddr_MIN_EEPROM; adresa <= max_zaznam_EEPROM ; adresa = adresa + delka_zaznamu)
              {

                if (EEPROM_read(adresa) >= 127)                       // Vypisuji se jen plne zaznamy. Zaznamy ktere byly smazane SOFT formatem se ignoruji.
                  {
                    if (EEPROM_read_long(adresa+1) >= historie)       // vypisuji se jen zaznamy, ktere maji datum vetsi, nez je rozdil aktualniho casu a zadane casove hloubky
                      {
                        vypis_STOPLAP = true;                         // od prvniho vypisovaneho zaznamu se budou vypisovat i zaznamy "STOP" a "LAP" - nezavisle na hodnote datumu a casu
                        prazdno = false;
                        priprav_1_zaznam(adresa);
                        print_co_kam(true, false);                    // Serial.println(vystupni_retezec);
                        test_485_kom();                               // test, jestli neprisel nejaky pozadavek po komunikaci RS485
    
                        if (digitalRead(pin_tl_ok) == LOW)            // predcasne ukonceni dlouheho vypisu stiskem tlacitka OK
                          {
                            delay(20);
                            while (digitalRead(pin_tl_ok) == LOW)    delay(20);
                            break;
                          }                      
                      }
                    else                                              // zaznam neodopvida pozadovane hloubce historie
                      {
                        if (vypis_STOPLAP == true)                    // ale pokud je nastavena tato znacka ...
                          {
                            if ((EEPROM_read(adresa) & 0b01000000) > 0)   // ... a zaroven se jedna o specialni pripad zaznamu pro funkci stopky ...
                              {
                                if ((EEPROM_read(adresa + 7) == 2) or (EEPROM_read(adresa + 7) == 3) )   // ... a zaroven se jedna o typ zaznamu "STOP" nebo "LAP"
                                  {
                                    priprav_1_zaznam(adresa);         // tak se zaznam vytiskne bez ohledu na jeho datum a cas
                                    print_co_kam(true, false);        // Serial.println(vystupni_retezec);
                                    
                                  }
                              }
                             
                          }
                      }
                     
                  }

                pocitadlo_radek --;
                if (pocitadlo_radek % 20 == 0)
                  {
                    zobraz_cislo((pocitadlo_radek / 20) , 6);         // vystup odpocitavani na displej
                  }                    

              }                     
          }
        dokoncene_mereni = 2;                                         // po skonceni vypisu se odblokuje zablokovane spousteni dalsiho mereni pres RS485
        break;

        
        case  65532:                                                  // vypis cele EEPROM (krome tech zaznamu, ktere maji datum nastaveny na 1.1.1970) - vypisuji se i SOFT formatovane zaznamy
          {
            dokoncene_mereni = 0;                                     // je to dlouha operace, behem ktere je blokovano spusteni dalsiho mereni pres RS485

            uint16_t pocitadlo_radek = (max_zaznam_EEPROM - eeaddr_MIN_EEPROM) / delka_zaznamu;            
            for (uint32_t adresa = eeaddr_MIN_EEPROM; adresa <= max_zaznam_EEPROM ; adresa = adresa + delka_zaznamu)
              {
                if (EEPROM_read_long(adresa+1) > 0)                   // vypisuji se jen zaznamy, ktere maji nejaky normalni datum
                  {
                    prazdno = false;
                    priprav_1_zaznam(adresa);
                    print_co_kam(true, false);                        // Serial.println(vystupni_retezec);
                    test_485_kom();                                   // test, jestli neprisel nejaky pozadavek po komunikaci RS485

                    if (digitalRead(pin_tl_ok) == LOW)                // predcasne ukonceni dlouheho vypisu stiskem tlacitka OK
                      {
                        delay(20);
                        while (digitalRead(pin_tl_ok) == LOW)    delay(20);
                        break;
                      }
                  }

                pocitadlo_radek --;
                if (pocitadlo_radek % 20 == 0)
                  {
                    zobraz_cislo((pocitadlo_radek / 20) , 6);         // vystup odpocitavani na displej
                  }
              }                     
          }
        dokoncene_mereni = 2;                                         // po skonceni vypisu se odblokuje zablokovane spousteni dalsiho mereni pres RS485
        break;

        case   65531:                                                 // vypis stareho zaznamu na displej
          {
            //---------------
            if (adr_posl_zaznamu > 0)
              {                        
                bool vyskocit_ze_zobrazeni = false;
                
                while (vyskocit_ze_zobrazeni == false)
                  {
    
                    // nalezeni adresy pred 'list' zaznamy
                    uint32_t adr_100 = adr_posl_zaznamu;
                    
                    int16_t pocet = list;
    
                    while (pocet > 0)                                 // nekolikrat ('list' krat) se odecte delka zaznamu ...
                      {
                        pocet --;
                        adr_100 = adr_100 - delka_zaznamu;
                        if (adr_100 < eeaddr_MIN_EEPROM)              //  ... kdyz behem toho dojde k podteceni pod zacatek oblasti s daty ...
                          {
                            adr_100 = max_zaznam_EEPROM;              // ... nastavi se adresa na posledni pouzitelny zaznam v EEPROM
                          }
                      }
    
                    if ((EEPROM_read(adr_100) & 0b10000000) > 0)      // v EEPROM se nachazi platny zaznam
                      {
                        priprav_1_zaznam(adr_100);                
    
                        if ((EEPROM_read(adr_100) & 0b01000000) > 0)  // specialni pripad zaznamu pro funkci stopky
                          {
                            switch (EEPROM_read(adr_100 + 7))
                              {
                                case 1:                                                            // START
                                  {
                                    pom_disp_scroll[4]  = 109;                                     // S
                                    pom_disp_scroll[5]  = 120;                                     // t
                                    pom_disp_scroll[6]  = 119;                                     // A
                                    pom_disp_scroll[7]  =  80;                                     // r
                                    pom_disp_scroll[8]  = 120;                                     // t
                                    pom_disp_scroll[9]  =   0;                                     // ' '
                                    pom_disp_scroll[10] =   graf_def[vystupni_retezec[31]-48];     // desitky dni
                                    pom_disp_scroll[11] =   graf_def[vystupni_retezec[32]-48] + 128;   // jednotky dni + tecka
                                    pom_disp_scroll[12] =   graf_def[vystupni_retezec[34]-48];     // desitky mesicu
                                    pom_disp_scroll[13] =   graf_def[vystupni_retezec[35]-48] + 128;   // jednotky mesicu + tecka
                                    pom_disp_scroll[14] =   0;                                     // ' '
                                    pom_disp_scroll[15] =   graf_def[vystupni_retezec[44]-48];     // desitky hodin
                                    pom_disp_scroll[16] =   graf_def[vystupni_retezec[45]-48];     // jednotky hodin
                                    pom_disp_scroll[17] =   64;                                    // '-'
                                    pom_disp_scroll[18] =   graf_def[vystupni_retezec[47]-48];     // desitky minut
                                    pom_disp_scroll[19] =   graf_def[vystupni_retezec[48]-48];     // jednotky minut
                                   
                                    vyskocit_ze_zobrazeni = disp_zaznam_scroll(20);
                                  }
                                break;
            
                                case 2:                                                            // STOP
                                  {
                                    pom_disp_scroll[4]  = 109;                                     // S
                                    pom_disp_scroll[5]  = 120;                                     // t
                                    pom_disp_scroll[6]  =  92;                                     // o
                                    pom_disp_scroll[7]  = 115;                                     // P
                                    pom_disp_scroll[8]  =   0;                                     // ' '
                                    pom_disp_scroll[9]  =   graf_def[vystupni_retezec[32]-48];     // desitky hodin
                                    pom_disp_scroll[10] =   graf_def[vystupni_retezec[33]-48];     // jednotky hodin
                                    pom_disp_scroll[11] =   64;                                    // '-'
                                    pom_disp_scroll[12] =   graf_def[vystupni_retezec[35]-48];     // desitky minut
                                    pom_disp_scroll[13] =   graf_def[vystupni_retezec[36]-48];     // jednotky minut
                                    pom_disp_scroll[14] =   64;                                    // '-'
                                    pom_disp_scroll[15] =   graf_def[vystupni_retezec[38]-48];     // desitky sekund
                                    pom_disp_scroll[16] =   graf_def[vystupni_retezec[39]-48] + 128;   // jednotky sekund + tecka
                                    pom_disp_scroll[17] =   graf_def[vystupni_retezec[48]-48];     // desetiny sekund
                                    pom_disp_scroll[18] =   graf_def[vystupni_retezec[49]-48];     // setiny sekund
                                    pom_disp_scroll[19] =   graf_def[vystupni_retezec[50]-48];     // tisiciny sekund
                                    
                                    vyskocit_ze_zobrazeni = disp_zaznam_scroll(20);                                 
                                  }
                                break;
            
                                case 3:                                                            // LAP
                                  {
                                    pom_disp_scroll[4]  =  56;                                     // L
                                    pom_disp_scroll[5]  = 119;                                     // A
                                    pom_disp_scroll[6]  = 115;                                     // P
                                    pom_disp_scroll[7]  =   0;                                     // ' '
                                    pom_disp_scroll[8]  =   graf_def[vystupni_retezec[89]-48];     // desitky z pocitadla mezicasu
                                    pom_disp_scroll[9]  =   graf_def[vystupni_retezec[90]-48];     // jednotky z pocitadla mezicasu
                                    pom_disp_scroll[10] =   0;                                     // ' '                        
                                    pom_disp_scroll[11] =   graf_def[vystupni_retezec[32]-48];     // desitky hodin
                                    pom_disp_scroll[12] =   graf_def[vystupni_retezec[33]-48];     // jednotky hodin
                                    pom_disp_scroll[13] =   64;                                    // '-'
                                    pom_disp_scroll[14] =   graf_def[vystupni_retezec[35]-48];     // desitky minut
                                    pom_disp_scroll[15] =   graf_def[vystupni_retezec[36]-48];     // jednotky minut
                                    pom_disp_scroll[16] =   64;                                    // '-'
                                    pom_disp_scroll[17] =   graf_def[vystupni_retezec[38]-48];     // desitky sekund
                                    pom_disp_scroll[18] =   graf_def[vystupni_retezec[39]-48] + 128;   // jednotky sekund + tecka
                                    pom_disp_scroll[19] =   graf_def[vystupni_retezec[48]-48];     // desetiny sekund
                                    pom_disp_scroll[20] =   graf_def[vystupni_retezec[49]-48];     // setiny sekund
                                    pom_disp_scroll[21] =   graf_def[vystupni_retezec[50]-48];     // tisiciny sekund
            
                                    vyskocit_ze_zobrazeni = disp_zaznam_scroll(22);
                                  }
                                break;
            
                                case 4:                                                            // CONT
                                  {
                                    pom_disp_scroll[4]  =  57;                                     // C
                                    pom_disp_scroll[5]  =  92;                                     // o
                                    pom_disp_scroll[6]  =  84;                                     // n
                                    pom_disp_scroll[7]  = 120;                                     // t
                                    pom_disp_scroll[8]  =   0;                                     // ' '
                                    pom_disp_scroll[9]  =   graf_def[vystupni_retezec[31]-48];     // desitky dni
                                    pom_disp_scroll[10] =   graf_def[vystupni_retezec[32]-48] + 128;   // jednotky dni + tecka
                                    pom_disp_scroll[11] =   graf_def[vystupni_retezec[34]-48];     // desitky mesicu
                                    pom_disp_scroll[12] =   graf_def[vystupni_retezec[35]-48] + 128;   // jednotky mesicu + tecka
                                    pom_disp_scroll[13] =   0;    // ' '
                                    pom_disp_scroll[14] =   graf_def[vystupni_retezec[44]-48];     // desitky hodin
                                    pom_disp_scroll[15] =   graf_def[vystupni_retezec[45]-48];     // jednotky hodin
                                    pom_disp_scroll[16] =   64;                                    // '-'
                                    pom_disp_scroll[17] =   graf_def[vystupni_retezec[47]-48];     // desitky minut
                                    pom_disp_scroll[18] =   graf_def[vystupni_retezec[48]-48];     // jednotky minut
            
                                    vyskocit_ze_zobrazeni = disp_zaznam_scroll(19);
                                  }
                                break;
                              }
                          }
        
                        //----                  
                        if ((EEPROM_read(adr_100) & 0b00100000) > 0)                               // specialni pripad zaznamu pro funkci casoveho razitka
                          {
                            pom_disp_scroll[4]  = 120;                                             // t
                            pom_disp_scroll[5]  =   4;                                             // i
                            pom_disp_scroll[6]  =  55;                                             // M
                            pom_disp_scroll[7]  = 121;                                             // E                    
                            pom_disp_scroll[8]  =   0;                                             // ' '
                            pom_disp_scroll[9]  =   graf_def[vystupni_retezec[69]-48];             // stovky z poradoveho cisla razitka
                            pom_disp_scroll[10] =   graf_def[vystupni_retezec[70]-48];             // desitky  z poradoveho cisla razitka
                            pom_disp_scroll[11] =   graf_def[vystupni_retezec[71]-48];             // jednotky z poradoveho cisla razitka
                            pom_disp_scroll[12] =   0;                                             // ' '
                            pom_disp_scroll[13] =   graf_def[vystupni_retezec[31]-48];             // desitky dni
                            pom_disp_scroll[14] =   graf_def[vystupni_retezec[32]-48] +128;        // jednotky dni s teckou
                            pom_disp_scroll[15] =   graf_def[vystupni_retezec[34]-48];             // desitky mesicu
                            pom_disp_scroll[16] =   graf_def[vystupni_retezec[35]-48] +128;        // jednotky mesicu s teckou
                            pom_disp_scroll[17] =   0;                                             // ' '
                            pom_disp_scroll[18] =   graf_def[vystupni_retezec[44]-48];             // desitky hodin
                            pom_disp_scroll[19] =   graf_def[vystupni_retezec[45]-48];             // jednotky hodin
                            pom_disp_scroll[20] =   64;                                            // '-'
                            pom_disp_scroll[21] =   graf_def[vystupni_retezec[47]-48];             // desitky minut
                            pom_disp_scroll[22] =   graf_def[vystupni_retezec[48]-48];             // jednotky minut
                            pom_disp_scroll[23] =   64;                                            // '-'
                            pom_disp_scroll[24] =   graf_def[vystupni_retezec[50]-48];             // desitky sekund
                            pom_disp_scroll[25] =   graf_def[vystupni_retezec[51]-48];             // jednotky sekund
        
                            vyskocit_ze_zobrazeni = disp_zaznam_scroll(26);
                          }
        
        
                        //----                  
                        if ((EEPROM_read(adr_100) & 0b01100000) == 0)                              // bezny zaznam svetla (nebo kalibracni zaznam)
                          {
                            pom_disp_scroll[4]  =  znacka_listu;                                   // 1 znak z poznamky (A=automat, C=kalibrace, U=tlacitko Up ....)
                            pom_disp_scroll[5]  =  0;                                              // mezera
                            
                            pom_disp_scroll[6]  =  graf_def[vystupni_retezec[31]-48];              // desitky dni
                            pom_disp_scroll[7]  =  graf_def[vystupni_retezec[32]-48] | 0b10000000; // jednotky dni s teckou
                            pom_disp_scroll[8]  =  graf_def[vystupni_retezec[34]-48];              // desitky mesicu
                            pom_disp_scroll[9]  =  graf_def[vystupni_retezec[35]-48] | 0b10000000; // jednotky mesicu s teckou
                            pom_disp_scroll[10] =  0;                                              // mezera
                            pom_disp_scroll[11] =  graf_def[vystupni_retezec[44]-48];              // desitky hodin
                            pom_disp_scroll[12] =  graf_def[vystupni_retezec[45]-48];              // jednotky hodin
                            pom_disp_scroll[13] =  64;                                             // mezi hodinami a minutami je pomlcka
                            pom_disp_scroll[14] =  graf_def[vystupni_retezec[47]-48];              // desitky minut
                            pom_disp_scroll[15] =  graf_def[vystupni_retezec[48]-48];              // jednotky minut
                            pom_disp_scroll[16] =  0;                                              // mezera
                            if (vystupni_retezec[75] == '0')   pom_disp_scroll[17] = 0;            // nula na radu desitek mag se nezobrazuje
                            else       pom_disp_scroll[17] = graf_def[vystupni_retezec[75]-48];    // kdyz jsou nejake desitky mag, tak se zobrazi
                            pom_disp_scroll[18] =  graf_def[vystupni_retezec[76]-48] | 0b10000000; // jednotky mag s teckou
                            pom_disp_scroll[19] =  graf_def[vystupni_retezec[78]-48];              // desetiny mag
                            pom_disp_scroll[20] =  graf_def[vystupni_retezec[79]-48];              // setiny mag
                            pom_disp_scroll[21] =  graf_def[vystupni_retezec[80]-48];              // tisiciny mag
                                            
                            vyskocit_ze_zobrazeni = disp_zaznam_scroll(22);
                          }
                      
                      }
                    else                          // v EEPROM neni platny zaznam (bud je tam prazdna oblast, nebo je tam nejaky drive smazany zanam - SOFTformatovany)
                      {
                        // najeti 5 pomlcek na displej
                        pom_disp_scroll[4]  = 64;                                                  // -
                        pom_disp_scroll[5]  = 64;                                                  // -
                        pom_disp_scroll[6]  = 64;                                                  // -
                        pom_disp_scroll[7]  = 64;                                                  // -
                        pom_disp_scroll[8]  = 64;                                                  // -
                        vyskocit_ze_zobrazeni = disp_zaznam_scroll(9);
                      }
        
        
                    uint8_t pompam4 = D_pamet[4];                                                     // zapamatuje se stav displeje pro nasledujici smycku, ve ktere muze dojit k jeho zhasnuti
                    uint8_t pompam3 = D_pamet[3];                                                     //    pri aktivaci interruptu (automat, seriova komunikace)
                    uint8_t pompam2 = D_pamet[2];
                    uint8_t pompam1 = D_pamet[1];
                    uint8_t pompam0 = D_pamet[0];
        
                    
                    uint8_t pom_pocitadlo = 0;
    
                    // dokud neni stisknuto zadne tlacitko, testuje se komunikace a na displeji je zobrazen konec posledniho rotovaneho udaje
                    while (digitalRead(pin_tl_ok) == HIGH and digitalRead(pin_tl_up) == HIGH and digitalRead(pin_tl_dn) == HIGH )
                      {                                
                        interrupty();                                                              // odskoceni na test, jestli neni nejaky pozadavek o komunikaci, nebo jestli neni cas spustit automaticke mereni
        
                        D_pamet[0] = pompam0;                                                      // po navratu z interruptu zobrazi puvodni stav displeje
                        D_pamet[1] = pompam1;
                        D_pamet[2] = pompam2;
                        D_pamet[3] = pompam3;
                        D_pamet[4] = pompam4;
                        aktualizuj_displej();
                        
                        delay(100);
                      }
    
                    // neco bylo stisknuto:
                    if (digitalRead(pin_tl_ok) == LOW)                                             // stisknuto bylo tlacitko OK
                      {
                        pom_pocitadlo = 0;
                        while (digitalRead(pin_tl_ok) == LOW)                                      // dokud se tlacitko OK neuvolni ...
                          {
                            delay(20);                                                             // ... cekej
                            pom_pocitadlo ++;
                            if (pom_pocitadlo == 50) zobraz_text(66);                              // prazdny displej v okamziku, kdy je dlouhym stiskem aktivovane opakovani rotace
                            if (pom_pocitadlo > 100) pom_pocitadlo = 100;
                          }
                        delay(20);                                                                 // odruseni zakmitu pri uvolnovani tlacitka
                        prazdno = false;
        
                        if (pom_pocitadlo < 50)                                                    // OK bylo stisknuto na delsi dobu nez sekundu ...
                          {
                            vyskocit_ze_zobrazeni = true;                                          // nastavi se znacka, ze se ma provest navrat do menu
                          }                        
                      }
                        
        
    
                    if (digitalRead(pin_tl_dn) == LOW)                                             // stisknuto bylo tlacitko DOLU
                      {
                        list ++;                                                                   // posunuti na predchozi zaznam (starsi)
                      }                                                                            //    promenna 'list' ukazuje, kolikaty zaznam do zpatku se ma zobrazit
    
                    if (digitalRead(pin_tl_up) == LOW)                                             // stisknuto bylo tlacitko NAHORU
                      {
                        list --;                                                                   // posunuti na dalsi zaznam (cerstvejsi)
                        if (list < 0)                                                              // tady musi byt kontrala na podlezeni 0 (neda se zobrazit zaznam, ktery jeste nebyl zaznamenany)
                          {
                            list = 0;                           
                            tone_X(pin_bzuk,800, 30 , 6);                                          // chybove pipnuti po dosazeno minima (posledni zaznam) - novejsi zaznam uz neni
                          }
                      }
                  }
    
                zobraz_text(66);                                                                   // prazdny displej
              }
            else
              {
                zobraz_text(68);                                                                   // "-----"
              }
          }
        break;


        case  65535:                                                                               // vypise poslednich 100 zaznamu (s testem na prechod pres vrchol pouzitelne EEPROM) 
          {
            uint8_t pocet = 100;
            // nalezeni adresy pred 100 zaznamy
            uint32_t adr_100 = adr_dalsiho_zaznamu;

            while (pocet > 0)                                                                      // 100x se odecte delka zaznamu ...
              {
                pocet --;
                adr_100 = adr_100 - delka_zaznamu;
                if (adr_100 < eeaddr_MIN_EEPROM)                                                   //  ... kdyz behem toho dojde k podteceni pod zacatek oblasti s daty ...
                  {
                    adr_100 = max_zaznam_EEPROM;                                                   // ... nastavi se adresa na posledni pouzitelny zaznam v EEPROM
                  }
              }
            
            pocet = 100;
            dokoncene_mereni = 0;                                                                  // je to dlouha operace, behem ktere je blokovano spusteni dalsiho mereni

            while (pocet > 0)
              {
                test_485_kom();                                                                    // test, jestli neprisel nejaky pozadavek po komunikaci RS485                
                pocet --;
                adr_100 = adr_100 + delka_zaznamu;
                if (adr_100 > max_zaznam_EEPROM)
                  {
                    adr_100 = eeaddr_MIN_EEPROM;
                  }
                if ((EEPROM_read(adr_100) & 0b10000000) != 0)                                      // polozka v EEPROM je obsazena
                  {
                    prazdno = false;
                    priprav_1_zaznam(adr_100);
                    print_co_kam(true, false);                                                     // Serial.println(vystupni_retezec);

                    if (digitalRead(pin_tl_ok) == LOW)                                             // predcasne ukonceni dlouheho vypisu stiskem tlacitka OK
                      {
                        delay(20);
                        while (digitalRead(pin_tl_ok) == LOW)    delay(20);
                        break;
                      }
                    zobraz_cislo( pocet , 6);                                                      // zbyvajici pocet radek na displej
                  }
              }                    
          }

        dokoncene_mereni = 2;                                                                      // po skonceni vypisu se odblokuje zablokovane spousteni dalsiho mereni pres RS485
        break;
        
//        case  0:                                                                                   // mimo rozsah
//          prazdno = false;                                                                         //  na konci nevypisuje hlaseni "Zadny ulozeny zaznam" (ani v pripade, ze by opravdu zadny zaznam na zvolene polozce neexistoval)
//          Serial.print(lng167);                                                                    //             Serial.print("Zvolena polozka je mimo rozsah EEPROM (1 - ");
//          Serial.print((eeaddr_MAX_EEPROM - eeaddr_MIN_EEPROM) / delka_zaznamu);
//          Serial.println(')');
//        break;


        default:                                                                                   // jakykoliv jiny parametr znamena vypis jedne zvolene polozky
          {
            prazdno = false;                                                                       // pri vypisu zvolene polozky se na konci nevypisuje hlaseni "Zadny ulozeny zaznam"
            uint32_t adresa = ((parametr-1) * delka_zaznamu) + eeaddr_MIN_EEPROM;                  //       (ani v pripade, ze by opravdu zadny zaznam na zvolene polozce neexistoval)      

            if (dat == 0)
              {
                if (adresa <= max_zaznam_EEPROM)
                  {
                    priprav_1_zaznam(adresa);
                    print_co_kam(true, false);
                  }
                else
                  {
                    Serial.print(lng167);                                                          // Serial.print("Zvolena polozka je mimo rozsah EEPROM (1 - ");
                    Serial.print((eeaddr_MAX_EEPROM - eeaddr_MIN_EEPROM) / delka_zaznamu);
                    Serial.println(')');
                  }            
              }
            else                                                                                   // (dat = 1) datovy vypis jednoho zvoleneho zaznamu
              {
                EEPROM_to_ser_out(parametr);                                                       // parametr je index pozadovaneho zaznamu (1 az ....) pri 0 je to "posledni zaznam"
                Serial.print("DATA:");                                                             // citelna hlavicka
                for (uint8_t i = 0; i < 31 ; i++)                                                     //  odesle cele pole 31 polozek (62 bajtu) do seriove linky (necitelny blok dat) 
                  {
                    Serial.write (ser_out_data[i] / 256);                                          //  MSB
                    Serial.write (ser_out_data[i] % 256);                                          //  LSB 
                  }
                Serial.println(' ');                                                               // vypis zaznamu konci znaky <SP><CR><LF>
              }              
          }                
      }
      
    if (prazdno == true)
      {
        Serial.println(lng168);                                                                    // Serial.println("Zadny ulozeny zaznam");   
      }
  }
//----------------------------------------------



//----------------------------------------------
// do docasneho pole EEPROM posklada namerene hodnoty a aktualni cas
//  vsechny potrebne hodnoty si bere  z globalnich promennych:
//       selc, int_svetlo, slave_addr, int_infra, int_full, byte_cnf, int_teplota, int_vlhkost a int_tlak
//  vystupem je globalni 'pole_EEPROM[]'
void priprava_pole_EEPROM(void)
  {
    uint32_t tt;
                                                                      // ulozeni merenych hodnot do pole 
    pole_EEPROM[0] =  0b10000000 + selc;                              // znacka, ze je blok adres v EEPROM obsazeny (bit7='1') jestli je SEC(bit0='0'), nebo SELC(bit0='1')

    pole_EEPROM[0] = pole_EEPROM[0] | (index_cidla_svetla << 1);      // pro rozsirujici desku s vice cidly svetla se uklada do prvniho bajtu i index cidla svetla 1 az 7

    if (GPS_RTC_flag == true)
      {
        tt = GPS_temp_time;                                           // zjisteni aktualniho casu v GPS (UTC)    
      }
    else
      {
        tt = rtc_korekce();                                           // zjisteni aktualniho casu v RTC (UTC)    
      }

    GPS_RTC_flag = false;                                             // zruseni docasneho pouziti GPS casu misto RTC casu
    


    pole_EEPROM[1] =  ((uint32_t)tt >> 24) & 0xFF;                    // 4 bajty pro pocet sekund od 1.1.1970 (s korekci nepresnosti a pripadnym prevodem na letni cas)
    pole_EEPROM[2] =  ((uint32_t)tt >> 16) & 0xFF;
    pole_EEPROM[3] =  ((uint32_t)tt >>  8) & 0xFF;
    pole_EEPROM[4] =  ((uint32_t)tt      ) & 0xFF;    

    pole_EEPROM[5] = (int_svetlo >> 8) & 0xFF;
    pole_EEPROM[6] = (int_svetlo     ) & 0xFF;
    
    pole_EEPROM[7] = (slave_addr << 4) + znacka ;                     // SLAVE adresa se uklada do hornich 4 bitu, spodni 1 bit pro stabilni jas; 3 bity jsou znacky (tlacitko,kalibrace,rucni spousteni, automat)

    uint8_t indexpole = 8;

#ifdef ukladat_infra      
    pole_EEPROM[indexpole]   = (int_infra >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_infra     ) & 0xFF;
    indexpole = indexpole + 2;
#endif

#ifdef ukladat_full       
    pole_EEPROM[indexpole]   = (int_full >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_full     ) & 0xFF;
    indexpole = indexpole + 2;
#endif

#ifdef ukladat_contr_reg 
    pole_EEPROM[indexpole] = byte_cnf;
    indexpole = indexpole + 1;
#endif

#ifdef ukladat_teplotu    
    pole_EEPROM[indexpole]   = (int_teplota >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_teplota     ) & 0xFF;
    indexpole = indexpole + 2;
#endif

#ifdef ukladat_vlhkost    
    pole_EEPROM[indexpole]   = (int_vlhkost >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_vlhkost     ) & 0xFF;
    indexpole = indexpole + 2;
#endif

#ifdef ukladat_tlak       
    pole_EEPROM[indexpole]   = (int_tlak >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_tlak     ) & 0xFF;
    indexpole = indexpole + 2;
#endif

#ifdef ukladat_GPS
    pole_EEPROM[indexpole]   = (GPS_lat >> 24) & 0xFF;
    pole_EEPROM[indexpole+1] = (GPS_lat >> 16) & 0xFF;
    pole_EEPROM[indexpole+2] = (GPS_lat >>  8) & 0xFF;
    pole_EEPROM[indexpole+3] = (GPS_lat      ) & 0xFF;
    pole_EEPROM[indexpole+4] = (GPS_lon >> 24) & 0xFF;
    pole_EEPROM[indexpole+5] = (GPS_lon >> 16) & 0xFF;
    pole_EEPROM[indexpole+6] = (GPS_lon >>  8) & 0xFF;
    pole_EEPROM[indexpole+7] = (GPS_lon      ) & 0xFF;
    pole_EEPROM[indexpole+8] = (GPS_alt >>  8) & 0xFF;
    pole_EEPROM[indexpole+9] = (GPS_alt      ) & 0xFF;

    if ((EEPROM_read(eeaddr_TS_stanoviste) & 0b11100000) > 0)                   // pokud je aktualne prednastavene nejake pozorovaci stanoviste, zapise se do GPS bloku EEPROM na nejvyssi 3 bity
      {
        pole_EEPROM[indexpole] = (pole_EEPROM[indexpole] | (EEPROM_read(eeaddr_TS_stanoviste) & 0b11100000));
      }

    indexpole = indexpole + 10;
#endif    

#ifdef ukladat_naklon       
    pole_EEPROM[indexpole]   = (int_naklon >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_naklon     ) & 0xFF;
    indexpole = indexpole + 2;
#endif

#ifdef ukladat_azimut 
    pole_EEPROM[indexpole]   = (int_azimut >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_azimut     ) & 0xFF;
    indexpole = indexpole + 2;
#endif


#ifdef ukladat_astro       
    pole_EEPROM[indexpole]   = Slu_elevace;                           // posledni vypoctene astro hodnoty pred kazdym zaznamem
    pole_EEPROM[indexpole+1] = Mes_elevace;
    pole_EEPROM[indexpole+2] = Mes_osvit;
    indexpole = indexpole + 3;
#endif

#ifdef ukladat_rezerva_1                                              // posledni zmerena hodnota 1. rezervniho cidla
    pole_EEPROM[indexpole]   = (int_rezerva_1 >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_rezerva_1     ) & 0xFF;
    indexpole = indexpole + 2;
#endif

#ifdef ukladat_rezerva_2                                              // posledni zmerena hodnota 2. rezervniho cidla
    pole_EEPROM[indexpole]   = (int_rezerva_2 >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_rezerva_2     ) & 0xFF;
    indexpole = indexpole + 2;
#endif

#ifdef ukladat_rezerva_3                                              // posledni zmerena hodnota 3. rezervniho cidla
    pole_EEPROM[indexpole]   = (int_rezerva_3 >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_rezerva_3     ) & 0xFF;
    indexpole = indexpole + 2;
#endif

#ifdef ukladat_rezerva_4                                              // posledni zmerena hodnota 4. rezervniho cidla
    pole_EEPROM[indexpole]   = (int_rezerva_4 >> 8) & 0xFF;
    pole_EEPROM[indexpole+1] = (int_rezerva_4     ) & 0xFF;
    indexpole = indexpole + 2;
#endif


  
  }
//----------------------------------------------



//----------------------------------
// reset zpusobeny zamernym pretecenim watchdogu
void SOFT_RESET(void)
  {
    zobraz_text(46);                                                  //  "rE5Et" 
    SD_log(999,2);
    delay(1000);
    IWatchdog.begin(50000);                                           // 50 ms
    IWatchdog.reload();                                               // a okamzite obcerstveni WD
    delay(1000);                                                      // behem pauzy dojde k aktivaci WD a resetu procesoru 
    
  }
//----------------------------------------------



//----------------------------------------------
// jednoducha cervena svitilna s regulaci jasu v 5 stupnich 
void svitilna(void)
  {
    zobraz_text(68);                                                  // pomlcky na displeji "-----" 
    while (digitalRead(pin_tl_ok) == LOW)                             // do teto funkce se vlezlo stiskem tlacitka OK, tak se musi pockat, az se uvolni
      {
        delay(50);
      }
    delay(100);

    uint8_t pamet_jasu = jas_displeje;
    jas_displeje = 7;
    int16_t uroven_jasu = pamet_LED_svitilna;
    bool updn = false;                                                // po kazdem stisku tlacitka nahoru nebo dolu nasleduje extra pauza kvuli zakmitum
    bool lampa_onoff = true;
    bool konec_lampy = false;
    while (konec_lampy == false)            
      {
        if (lampa_onoff == true)                                      // lampa je rozsvicena
          {
            if (digitalRead(pin_tl_up) == LOW)                        // kdyz je rozsviceno, tak se pomoci tlacitek nahoru a dolu muze prenastavit jas
              {
                uroven_jasu ++;
                if (uroven_jasu > 5) uroven_jasu = 5;
                updn = true;
              }

            if (digitalRead(pin_tl_dn) == LOW)
              {
                uroven_jasu --;
                if (uroven_jasu < 1) uroven_jasu = 1;
                updn = true;
              }


            for (uint8_t segmentovka = 0; segmentovka < 5; segmentovka ++)
              {
                if (segmentovka < uroven_jasu )  D_pamet[segmentovka] = 255;
                else                             D_pamet[segmentovka] = 0;
              }
            aktualizuj_displej();            

            if(updn == true)                                          // pri zmene jasu se provadi extra pauzy proti zakmitum
              {
                delay(50);
                while((digitalRead(pin_tl_up) == LOW) or (digitalRead(pin_tl_dn) == LOW))  // ceka se, az budou obe tlacitka uvolnena
                  {
                    delay(20); 
                  }
    
                delay(50);                                            // po uvolneni tlacitek se aktualizuje displej na novou uroven jasu
                updn = false;
              }          
          
          }
        else                                                          // lampa je zhasnuta
          {
            for (uint8_t segmentovka = 0; segmentovka < 5; segmentovka ++)    // aktualizuj displej na zhasnuty stav
              {
                D_pamet[segmentovka] = 0;
              }
            aktualizuj_displej();
          }

        if (digitalRead(pin_tl_ok) == LOW)                            // je stisknuto tlacitko OK
          {
            delay(20);
            stisk_time = millis();
            while(digitalRead(pin_tl_ok) == LOW)
              {
                delay(50);
                if (millis() - stisk_time > 500)                      // OK bylo stisknuto na delsi dobu nez pul sekundy
                  {
                    zobraz_text(68);                                  // zobrazeni pomlcek
                    pamet_LED_svitilna = uroven_jasu;                 // pred ukoncenim se zaznamena posledni nastavena hodnota
                    konec_lampy = true;                               // lampa se ukoncuje.
                  }
              }                                                       // tlacitko OK bylo uvolneno
            delay(100);
            if(lampa_onoff == true) lampa_onoff = false;              // lampa se prepne ON / OFF
            else                    lampa_onoff = true;
          }
      
      }

    jas_displeje = pamet_jasu;                                        // navratem z funkce lampy se vraci puvodni jas displeje
    
  }
//----------------------------------------------



//----------------------------------------------
//  test baterie (blikani desetinnych tecek podle stavu baterie)
void test_baterie(void)
  {

    if (millis()/200 % 5 == 4) testbat_probehl = false;               // test baterie jen 1x za sekundu (pri 0,8 sek se znacka maze)

    if (millis()/200 % 5 == 1 and testbat_probehl == false)           // test baterie jen 1x za sekundu (pri 0,2 sek probehne 1x mereni)
      {
        cti_napajeni();
        testbat_probehl = true;
        if (millis() > 5000 and logbat == true)                       // 5 sekund po zapnuti napajeni se loguje i napeti 3V zalohovaci baterie
          {
            SD_log(993 , (uint16_t)bat_3V);                           // napeti v setinach V (0 az 300 odpovida 0,00V az 3,00V)
            logbat = false;
          }

        if (afd_bzuk == true and status_afd == true)                  // kdyz je povolene zobrazeni hlaseni "-AFD-" a AFD opravdu nastal
          {            
            if (afd_log == true)                                      // pri prvnim dosazeni AfD se provede zaznam do syslogu
              {
                afd_log = false;                                      // dalsi logovani je zakazane az do doby zmeny urovne pro AfD
                SD_log(992 , afd_desetiny);                           // loguje se jas v desetinach mag/arcsec2 (130 az 255) vejde se tak do jednoho bajtu
              }
            if (afd_styl != 2)                                        // pri afd_styl == 2 k zadnemu bzukani ani zobrazovani na displeji nedochazi 
              {
                zobraz_text(70);                                      // v ostatnich stylech blikne misto tecek baterioveho ukazatele napis "-AFD-"
                tone_X(pin_bzuk,3000, 10 , 2);                        //     a bzukne se            
              }
          }
        else                                                          // kdyz neniADF aktivni, nebo kdyz je zaakzane, zobarzuji se tecky jako signalizace stavu baterie
          {
            D_pamet[4] =   0;                                         // Nejdriv se 4 desetinne tecky pripravi na rozsviceni (jako by byla vybita baterie)
            D_pamet[3] = 128;                                         //    (je to jen priprava - skutecne rozsviceni nastane az na konci tohoto bloku funkci 'aktualizuj_displej')
            D_pamet[2] = 128;
            D_pamet[1] = 128;
            D_pamet[0] = 128;                                         // tecka na radu jednotek blika vzdycky - nezavisle na stavu baterie
    
            // ostatni LED se podle stavu baterie mohou pripravit do zhasnuteho stavu
            uint16_t napeti_baterie;
            uint16_t napeti_ref;
            if (osazeno_uref == true)
              {
                napeti_baterie = analogRead(pin_AD_BAT);
              }
            else                                                      // kdyz neni osazena reference, napeti se nemeri, ale nastavi se napevno na 100%
              {
                napeti_baterie = 4095;
              }
    
            uint8_t mez_bat_alarmu = 0;
            if (napeti_baterie > bat_level_3)   D_pamet[1] = 0;       // napeti na baterii je vyssi nez LEVEL 3, tecka na radu desitek se nerozsviti (vic nez 7V)
            else                                mez_bat_alarmu = 1;
    
            if (napeti_baterie > bat_level_2)   D_pamet[2] = 0;       // napeti na baterii je vyssi nez LEVEL 2, tecka na radu stovek se nerozsviti  (vic nez 6V)
            else                                mez_bat_alarmu = 2;
    
            if (napeti_baterie > bat_level_1)   D_pamet[3] = 0;       // napeti na baterii je vyssi nez LEVEL 1, tecka na radu tisicu se nerozsviti  (vic nez 5V)
            else                                mez_bat_alarmu = 3;
    
                                                                      // pri napeti mensim nez LEVEL_1 (mene nez 5V) zustavaji svitit vsechny 4 tecky (od radu jednotek do radu tisicu)
    
    
            if (osazeno_uref == true)
              {
                napeti_ref = analogRead(pin_refu);                    // test referencniho napeti (1,8V na ZD1)            
              }
            else                                                      // kdyz neni osazena reference, napeti se nemeri, ale nastavi se tak, aby nezpusobovalo rozblikani vsech LED tecek
              {
                napeti_ref = 0;
              }
            if (napeti_ref > ref_level)                               // kdyz se digity na referencnim pinu zacnou zvysovat, znamena to, ze klesa referencni (napajeci) napeti pro ADC
              {
                D_pamet[4] = 128;                                     // pak se rozblikaji vsechny LED, nezavisle na tom, jaka je zmerena hodnota na pinu AD_BAT    
                D_pamet[3] = 128;
                D_pamet[2] = 128;
                D_pamet[1] = 128;
                mez_bat_alarmu = 4;            
              }
            alarm_bat(mez_bat_alarmu);                                // akusticky alarm pri poklesu napeti baterie mezi urovnemi
            aktualizuj_displej();                                     // Skutecne rozsviceni tecek, ktere zustaly kvuli nizkemu napeti na baterii pripravene k rozsviceni
            if (osazeno_uref == true)                                 // kdyz neni osazena reference, k zadne signalizaci nedochazi
              {
                if (bitRead(signal_LED,0) == true)                    // pri povolene signalizaci pomoci LED se testuje baterie jen ve 2 urovnich
                  {
                    if (analogRead(pin_AD_BAT) < bat_level_2)         // napeti na baterii je nizke ...
                      {
                        LED_Y_onoff(true);                            // rozsviti na chvili LED zlute
                      }
                    else                                              // napeti na baterii je v poradku ...
                      {
                        LED_R_onoff(true);                            // rozsviti na chvili LED cervene
                      }
                  }          
                
              }
            
          }

      }
    if (millis()/200 % 5 == 2)                                        // po nejakem case vsechny tecky zhasnou
      {
        D_pamet[0] = 0;
        D_pamet[1] = 0;
        D_pamet[2] = 0;
        D_pamet[3] = 0;
        D_pamet[4] = 0;

        aktualizuj_displej();      
        LED_W(false);                                                 // LED se zhasne
      }

  }
//----------------------------------------------



//----------------------------------------------
// obycejne pipnuti pomoci funkce tone(), ale s testem s moznosti v nekterych skupinach funkcich uplne vypnout zvuk podle nastaveni bitu v promenne 'signal_BEEP'
// posledni parametr funkce urcuje skupinu funkci (bit) podle ktereho se voli vypnuty/zapnuty zvuk
//  skupina: 0=baterie, 1=mereni, 2=budik/afd .... 7=stopky, 8=po zapnuti               65535=povoleno nezavisle na skupinach
void tone_X(uint8_t tone_pin, int16_t tone_frq, int16_t tone_dur, int16_t skupina)
  {   
    bool pipnuti_povoleno;

    if (skupina == 65535) pipnuti_povoleno = true;                            // skupina 255 ma pipani povolene vzdycky (uvodni nastavovani defaultnich hodnot)
    else                  pipnuti_povoleno = bitRead(signal_BEEP,skupina);
     
    if (pipnuti_povoleno == true)
      {
        tone(tone_pin , tone_frq , tone_dur); 
        delay(tone_dur+2);                                   // bez pauzy a nasledneho prikazu noTone() po prvnim pipnuti prestava fungovat komunikace s cidlem svetla (funkce luxmeter)
        noTone(tone_pin);                                    //            (ve starsim procesoru STM32F103 to ale fungovalo)  
      }
    
  }
//----------------------------------------------



//----------------------------------------------
// vytiskne do seriove linky vodorovny bargraf slozeny s pomlcek a ohraniceny svislymi carami. Za zaverecnou svislo carou je CRLF
//      priklad:    |-----------|
void pomlcky(uint8_t pocet_pomlcek)
  {
    Serial.print('|');
    for (uint8_t i = 0; i < pocet_pomlcek ; i ++)
      {
        Serial.print('-');
      }
    Serial.println('|');    
  }
//----------------------------------------------



//----------------------------------------------
// Varovne bzuknuti pri poklesu napeti baterie pod nastavene meze
// Bzuknuti se provadi jen 1x pro kazdou mez (od zapnuti napajeni).
// Ze uz probehlo bzuknuti na prislusne mezi je zaznamenano do globalni promenne 'alarm_bat_pamet'
void alarm_bat(uint8_t stupen)
  {
    if (bitRead(alarm_bat_pamet , stupen) == false)
      {
        for (uint8_t cyk = stupen; cyk > 0 ; cyk--)                      // v pripade, ze se zabzuka hned nejakym vyssim stupnem, nizsi stupne se automaticky blokuji
          {
            bitSet(alarm_bat_pamet,cyk);
          }

        for (uint8_t cyk = 0; cyk < 2 ; cyk++)
          {
            tone_X(pin_bzuk, 1000, 20 , 0);
            delay(50);            
            tone_X(pin_bzuk,  800, 20 , 0);
            delay(50);            
          }             
        SD_log(996,stupen);
      }
  }
//----------------------------------------------



//----------------------------------------------
// prevede az tri 32-bitova cisla do jednoho 24-znakoveho HEX retezce "ID_CHAR[]"
void long32_to_HEX(uint32_t cislo, uint8_t start_pozice)
  {
    for (uint8_t pozice = start_pozice; pozice < start_pozice + 32 ; pozice = pozice + 4)
     {
       uint8_t ctyrbit = cislo & 0b1111;
       if (ctyrbit < 10)        ID_char[7 - (pozice/4) + start_pozice] = ctyrbit + 48;
       else                     ID_char[7 - (pozice/4) + start_pozice] = ctyrbit + 55;        
       cislo = (cislo >> 4);
     }    
  }
//----------------------------------------------



//----------------------------------------------
// Podle globalni promenne 'err_bit' vypise do seriove linky aktualni problemy
// Hlaseni chyb se vypisuje jen pri mereni, ktere je spustene pres USB seriovou linku
void hlaseni_chyb(void)
  {
    if (bitRead(err_bit, 0))    Serial.println(lng291);             //    "Problem se sbernici I2C"
    if (bitRead(err_bit, 1))    Serial.println(lng292);             //    "Problem s SD kartou"
    if (bitRead(err_bit, 2))    Serial.println(lng293);             //    "SD karta vysunuta"
    if (bitRead(err_bit, 3))    Serial.println(lng294);             //    "Problem s RTC (zalohovaci baterie)"
    if (bitRead(err_bit, 4))    Serial.println(lng295);             //    "Chyba naklonomeru"  
  }
//----------------------------------------------



//----------------------------------------------
// tisk vystupniho retezce nebo hlavicky do seriove linky nebo na kartu s omezenim podle skrytych nebo zobrazenych rezervnich cidel
//  co: false = hlavicka; true = vystupni_retezec
// kam: false = seriova linka; true = SD karta
void print_co_kam(bool co, bool kam)
  {
    for (uint16_t po_znaku = 0; po_znaku <= velikost_hlavicky; po_znaku++)
      {
        if (co == false and kam == false) Serial.print(hlavicka[po_znaku]);
        if (co == false and kam == true)  soubor.print(hlavicka[po_znaku]);
        if (co == true  and kam == false) Serial.print(vystupni_retezec[po_znaku]);
        if (co == true  and kam == true)  soubor.print(vystupni_retezec[po_znaku]);
      }

    if (kam == false) Serial.print("\r\n");
    if (kam == true)  soubor.print("\r\n");
    
  }


//----------------------------------------------


//----------------------------------------------
// pouze jednoduche roztrideni MSA do 9-pasmove stupnice
// 
// POZOR: Skutecny Bortle stupen se urcuje podle viditelnosti nekterych objektu na obloze a je proto zavisly na schopnostech uzivatele
//     Tento tridici algoritmus ale schopnosti pozorovatele vubech nebere do uvahy. Zmereny Bortleho stupen se tedy muze od skutecneho lisit.
//     Podprogram byl do SQM pridan jen jako reakce na "konkurencni" SDM, ktery z MSA take pocita Bortle stupen jednoduchym tridenim.
//         Ma ale trochu jinak nastavene meze a uzsi snimaci pasmo.
// Meze pro moje SQM urceny podle diskuse: //  https://www.astro-forum.cz/viewtopic.php?p=551407#p55140
//
// Vstup do podprogramu je v tisicinach MSA
uint8_t bortle(uint16_t vstupni_svetlo)
  {
// #doc#20
    if (vstupni_svetlo > 21700 )   return 1;           // nejtmavsi obloha
    if (vstupni_svetlo > 21550 )   return 2;
    if (vstupni_svetlo > 21400 )   return 3;
    if (vstupni_svetlo > 21050 )   return 4;
    if (vstupni_svetlo > 20400 )   return 5;
    if (vstupni_svetlo > 19500 )   return 6;
    if (vstupni_svetlo > 18500 )   return 7;
    if (vstupni_svetlo > 17500 )   return 8;
    return 9;                                          // vsechno ostatni je moc velke svetlo (Bortle = 9)
    
    //--------------------------------
    
  }

//----------------------------------------------


//----------------------------------------------
// podprogram pro mereni a vyhodnocovani napajecich napeti
// meri se pouze v pripade, ze je osazena reference. 
// vystupem jsou globalni promenne: 
//         napeti_9V      = float [desetiny V] prepoctene napeti hlavniho zdroje 
//         Vcc            = float [V] napeti procesoru
//         bat_3V         = float [V] napeti 3V baterie
//         bat_proc       = int   [%] procenta napeti 3V baterie pro RTC
//         ref_in         = float [V] referencni napeti
//         napajeno_z_USB = bool      podle zmereneho napeti hlavniho zdroje odhaduje, jestli se jedna o napajeni z USB konektoru nebo ze zdroje
void cti_napajeni(void)
  {
    Vcc = (4096 * 1210 / analogRead(AVREF))/1000.0 ;                 // napajeci napeti [V]  - (analogova reference je na desce spojena s napajenim, 1210 je konstanta z kat.listu, 4096 je rozliseni )
    bat_in  = analogRead(pin_AD_BAT);                                // napeti na zdroji za delicem [dig]
    bat_3d = analogRead(AVBAT);                                      // 1/4 napeti na zalohovaci baterii [dig]


    if (osazeno_uref == true)
      {
        ref_in = (Vcc * analogRead(pin_refu)) / 4096.0;              // napeti na ZD1 [V]    - (melo by byt presne 1.8V pro libovolne napajeci napeti)
      }
    else                                                             // neni osazena reference
      {
        ref_in = 1.8;                                                // pro vypocty se pouzije napeti 1.8V i kdyz neni skutecne merene
      }
    
// #doc#11
             //  DZ1=1.8V                                           R8  / R7+R8
    napeti_9V = ((((1.8 / ref_in) * 10 * Vcc * bat_in) / 4096.0) / (8.2 / 108.2)) + 0.5;     // napeti zdroje v desetinach [V] (149 = 14.9V)
    bat_3V    = 4 * Vcc * bat_3d / 4096.0 ;
    bat_proc = constrain(map(bat_3V * 1000 , 1650 , 3100 , 0 , 99), 0 , 99);   // 1650mV = minimalni hranice zalozni baterie pri ktere RTC jeste funguje (zobrazuje 0%)
                                                                               // 3100mV = napeti 3,1V na nove baterii = 99%
    if (napeti_9V < BAT_USB)                                       // kdyz neni zapojeno USB (napaji se jen z 9V baterie, nebo z 12V AKU) ...
      {
        RTC->BKP1R = (uint16_t)napeti_9V;                          //  ... zapise se do RTC registru aktualne zmerene napeti zdroje
        if (napajeno_z_USB == true)                                // az do ted bylo napajeno pres USB, ale doslo k prepnuti na baterii
          {
            for (uint8_t cyk = 0; cyk < 2 ; cyk++)                 // kdyz je povolene bateriove bzukani, tak se bzukne
              {
                tone_X(pin_bzuk, 1000, 20 , 0);
                delay(50);            
                tone_X(pin_bzuk,  800, 20 , 0);
                delay(50);            
              }
            napajeno_z_USB = false;
          }
      }
    else                                                           // je zapojeno na USB
      {
        napajeno_z_USB = true;            
      }
  }
//----------------------------------------------


//----------------------------------------------
// pauza s uspanim procesoru - snizeni odberu
void msleep(int16_t n)
  {
    while(n-- > 0)
      asm("wfi");
  }
