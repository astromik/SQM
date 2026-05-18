//  Jazykove verze 
//================================================




//===============================================================================================================================================
#ifdef CZ_language
//   CZ:
//----------------------------------------------

//  retezec, ktery se pouziva k formatovanemu vystupu na SD kartu, nebo do serioveho terminalu
//  index pole 100x                                                                                                            111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111122222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222223
//  index pole 10x                   111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990
//  index pole 1x          0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
//                         -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------   
char hlavicka[]         = "  pol. ; EEPROM ;  sek_1970  ;    datum   ;    cas   ; dvt; zona ;  jas  ;   jas  ;adr.;cid.; pozn. ; stab_jas ; infra ;  full ; [ms]; citl. ;teplota; tep.'C ;vlhkost; vlh. % ;  tlak  ;   GEO_lat  ;   GEO_long  ;  ALT  ; naklon ; azm ; S_el ; M_el ; M_sv ; rez.1 ; rez.2 ; rez.3 ; rez.4 \0";
// priklad s rezervami    " 00018 ; 000823 ; 1609232363 ; 29.12.2020 ; 08:59:23 ; Ut ;  SEC ; 08881 ; 08,881 ; 01 ; 01 ; komun ;  STABIL  ; 17989 ; 48311 ; 600 ;  428x ; 07196 ; +21,96 ; 05151 ; 051,51 ; 093410 ; +49,457154 ; +120,457156 ; -1234 ; +165.8 ; 123 ;  -45 ;  -27 ;  58  ; 65535 ; 65535 ; 65535 ; 65535 \0";    




// vystupni retezec pro simulaci slunecni soustavy
//  index pole 100x                                                                                                            111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111122222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222223
//  index pole 10x                   111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990
//  index pole 1x          0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
//                         -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------   
char sss_hlavicka[]     = "    datum   ; planeta ; index ;  elongace  ; E/W ; RA_planety ; DEC_planety ; RA_Slunce  ; DEC_Slunce ; vzdalenost_PZ ; vzdalenost_SP ; vzdalenost_ZS ; fazovy_uhel ;   osvetleni  ; mag. samotne planety ; mag. prstencu ; mag prstence + planety ;\0";           
// priklad                " 28.12.2020 ; Jupiter ;   4   ; 023,725750 ;  ?  ; 303,258917 ; -20,310040  ; 277,872566 ; -77,872566 ;   10,861111   ;   09,968692   ;    0,983389   ;  xx2,274772 ;   0,999606   ;      01,392652       ;   00,791857   ;      00,600795         ;\0";                


// vystupni retezec pro simulaci Slunce a Mesice
//  index pole 100x                                                                                                            111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111122222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222223
//  index pole 10x                   111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222233333333334444444444555555555566666666667777777777888888888899999999990
//  index pole 1x          0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
//                         -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------   
char ssm_hlavicka[]     = "    datum   ;  zapad Sl. ; azimut ; AS konec ; DS zacatek ; DS konec ; AS zacatek ; vychod Sl. ; azimut ; max elevace Sl ; elevace ; vzdalenost Sl. [AU] ; vychod Mes. ; azimut ; zapad Mes. ; azimut ; max elevace Mes. ; elevace ; vzdalenost Mes. [km] ; faze Mes. ;\0";
// priklad                " 13.07.2025 ;    21:07   ;   306  ;   0:23   ;    0:23    ;   1:56   ;    1:56    ;    5:12    ;   54   ;      13:09     ;    62   ;       1.016536      ;    22:43    ;   110  ;    9:18    ;   254  ;       3:59       ;    28   ;        379996        ;     85    ;\0";

#define kod_jazyka    "CZ"               // kod pouzity v PC programu pro automatickou zmenu jazyka

#define lng001    "Vlhkost: "
#define lng002    "Teplota bez korekce: "
#define lng003    "Teplota s korekci podle tabulky: "
#define lng004    "Absolutni tlak (bez prepoctu na hladinu more): "
#define lng005    "Nastavena zona "
#define lng006    "Vstup mimo rozsah "
#define lng007    "(10 az 255)"
#define lng008    "Plosny jas bez korekce:             "
#define lng009    "Plosny jas s korekci podle tabulky: "
#define lng010    "Automat vypnuty"
#define lng011    "Automaticky spoustet mereni po "
#define lng012    "Aktualne je aktivni cidlo: "
#define lng013    "Plosny jas bez kalibracni korekce: "
#define lng014    "Teplota bez kalibracni korekce: "
#define lng015    "Prumerovat "
#define lng016    " mereni"
#define lng017    "(1-20)"
#define lng018    "Rozhodovaci uroven pro stabilni jas je nastavena na "
#define lng019    "SLAVE adresa: "
#define lng020    "(1-15)"
#define lng021    "CRC se nekontroluje"
#define lng022    "CRC se testuje"
#define lng023    "CRC se testuje, chyby se hlasi do USB seriove linky"
#define lng024    "Do EEPROM se budou ukladat zaznamy s hodnotou plosneho jasu vetsi (tmavsi) nez "
#define lng025    "(0-255)"
#define lng026    "Zadany pocet vstupu pro cidla na expanzni desce: "
#define lng027    "(1 az 7)"
#define lng028    " puvodni: EEPROM["
#define lng359    " puvodni: EEPROM2["
#define lng029    "Nastaveny defaultni kalibracni hodnoty pro cidlo jasu"
#define lng030    "Nastaveny defaultni kalibracni hodnoty pro teplotu"
#define lng031    "Nastaveny defaultni parametry"
#define lng036    "j        ... zmer plosny jas  - bez ukladani"
#define lng037    "v        ... zmer vlhkost     - bez ukladani"
#define lng038    "t        ... zmer teplotu     - bez ukladani"
#define lng039    "p        ... zmer tlak        - bez ukladani"
#define lng040    "z 0      ... prepnout na zimni zonu"
#define lng041    "z 1      ... prepnout na letni zonu"
#define lng042    "a nnn    ... nastaveni automatickeho spousteni 0-255 minut"
#define lng043    "e n      ... pri pouziti expanzni desky prepne na zvolene cidlo (1-7)"
#define lng044    "r        ... vypis poslednich 100 zaznamu z EEPROM"
#define lng045    "rp       ... vypis posledniho zaznamu z EEPROM"
#define lng046    "ra       ... vypis vsech zaznamu z EEPROM"
#define lng047    "rz nnnnn ... vypis jednoho zvoleneho zazamu"
#define lng048    "c        ... aktualni cas"
#define lng049    "i        ... informace o aktualnim nastaveni systemu"
#define lng050    "h        ... zobrazeni hlavicky pro formatovany vystup"
#define lng051    "m        ... spusteni vsech mereni se zapisem do EEPROM"
#define lng052    "kj       ... kalibracni mereni jasu (vystup bez korekce)"
#define lng053    "kt       ... kalibracni mereni teploty (vystup bez korekce)"
#define lng054    "#?       ... napoveda pro dalsi funkce"
#define lng055    "#Tyyyymmddhhnnss ... nastaveni casu"
#define lng056    "#P nn        ... nastaveni poctu prumerovani jasu (1-20)"
#define lng057    "#S nnn       ... nastaveni urovne stability (0-255)"
#define lng058    "#I           ... detailni informace o kalibraci vnitrnich hodin"
#define lng059    "#A nn        ... nastaveni SLAVE adresy pro komunikaci (1-15)"
#define lng060    "#C 0         ... vypnuti CRC kontroly pro RS485"
#define lng061    "#C 1         ... zapnuti CRC kontroly pro RS485"
#define lng062    "#O?          ... nastaveni oddelovacu"
#define lng063    "#FS          ... SOFT format EEPROM"
#define lng064    "#FH          ... HARD format EEPROM"
#define lng065    "#U nnn       ... hranice jasu pro ukladani - v desetinach mag/arcsec2 (0-255)"
#define lng066    "#KJ n;x;y*   ... zapis bodu do kalibracni tabulky jasu"
#define lng067    "#KT n;x;y*   ... zapis bodu do kalibracni tabulky teploty"
#define lng068    "@W addr,data ... primy zapis do EEPROM"
#define lng069    "@R addr      ... prime cteni z EEPROM"
#define lng070    "@E           ... test postupneho prepinani cidel na expanzni desce"
#define lng071    "@X n         ... nastaveni poctu vstupu na expanzni desce (1-7)"
#define lng072    "@DS          ... defaultni kalibracni hodnoty pro jas"
#define lng073    "@DT          ... defaultni kalibracni hodnoty pro teplotu"
#define lng074    "@DP          ... defaultni parametry systemu"
#define lng076    "@@           ... softwarovy RESET"
#define lng077    "Oddelovac polozek: "
#define lng078    "mezera"
#define lng079    "carka"
#define lng080    "strednik"
#define lng081    "tabulator"
#define lng082    "Oddelovac desetinnych mist: "
#define lng083    "tecka"
#define lng084    "Uzavirat"
#define lng085    "Neuzavirat"
#define lng086    " hodnoty do uvozovek"
#define lng087    "Na zacatku souboru se hlavicka vytvaret "
#define lng088    "bude."
#define lng089    "nebude."
#define lng090    "#O,1 ... des. oddelovac: carka"
#define lng091    "#O,0 ... des. oddelovac: tecka"
#define lng092    "#O\"1 ... uzavirat polozky do uvozovek"
#define lng093    "#O\"0 ... neuzavirat polozky do uvozovek"
#define lng094    "#Oh1 ... vytvaret hlavicky"
#define lng095    "#Oh0 ... nevytvaret hlavicky"
#define lng096    "#Oo_ ... oddelovac polozek: mezera"
#define lng097    "#Oo, ... oddelovac polozek: carka"
#define lng098    "#Oo; ... oddelovac polozek: strednik"
#define lng099    "#Oot ... oddelovac polozek: tabulator"
#define lng100    "Varovani: oddelovac polozek i desetinnych mist je carka."
#define lng102    "(merena hodnota max 24 mag/arcsec2 = 24000)"
#define lng103    "(skutecna hodnota max 24 mag/arcsec2 = 24000)"
#define lng104    "zadane kalibracni hodnoty pro plosny jas:"
#define lng105    "pozice - merena hodnota - (mag/arcsec2) - spravna hodnota - (mag/arcsec2)"
#define lng106    "(pozice 1 az 7)"
#define lng107    "(merena hodnota max. 50'C = 10000)"
#define lng108    "(zobrazena hodnota max. 50'C = 10000)"
#define lng109    "zadane kalibracni hodnoty pro teplotu:"
#define lng110    "pozice - merena hodnota ('C) - zobrazena hodnota ('C)"
#define lng111    "Casova zona: "
#define lng112    "\r\nOddelovace:"
#define lng113    " Polozky v souboru: "
#define lng114    " Desetinny oddelovac: "
#define lng115    " Uzavirat polozky do uvozovek"
#define lng116    " Neuzavirat polozky do uvozovek"
#define lng117    " Do souboru vkladat hlavicky"
#define lng118    " Hlavicky do souboru nevkladat"
#define lng119    "\r\nAutomat:"
#define lng120    " Bez automatickeho mereni"
#define lng121    "  Spoustet mereni kazdych "
#define lng122    " minut."
#define lng123    "\r\nPrumerovat "
#define lng124    " mereni plosneho jasu"
#define lng125    "Kontrolni bajt se pri komunikaci pres RS485 nevyhodnocuje."
#define lng126    "Pri komunikaci pres RS485 je nutne odesilat spravny kontrolni bajt."
#define lng127    "Rozhodovaci rozdil pro stabilni/nestabilni jas je "
#define lng128    "Do EEPROM se ukladaji zaznamy s plosnym jasem nad (tmavsi nez) "
#define lng129    "Unikatni identifikace: "
#define lng130    "\r\nHardware:"
#define lng131    " Procesor: "
#define lng132    " Cidlo jasu: TSL2591"
#define lng032    "  "        // rezerva
#define lng033    "  "        // rezerva
#define lng034    "  "        // rezerva
#define lng035    "  "        // rezerva
#define lng133    "  "        // rezerva
#define lng134    "  "        // rezerva
#define lng211    "  "        // rezerva
#define lng231    "  "        // rezerva
#define lng235    "  "        // rezerva
#define lng247    "  "        // rezerva
#define lng075    "  "        // rezerva
#define lng135    " Cidlo vlhkosti, teploty a tlaku: BME280"
#define lng136    " SD karta: "
#define lng137    "zasunuta"
#define lng138    "vysunuta"
#define lng139    "nepouziva se"
#define lng140    " Displej: 5x7 segmentu (TM1637)"
#define lng141    " Pouze signalizacni LED"
#define lng142    " Pocet vstupu rozsirujici desky: "
#define lng143    " Aktualne aktivni cidlo jasu:    "
#define lng144    "\r\nEEPROM:"
#define lng145    " Zacatek oblasti pro data: "
#define lng146    " Konec oblasti pro data:   "
#define lng147    " Pristi zaznam se ulozi na adresu: "
#define lng148    " Jeden zaznam v EEPROM je dlouhy: "
#define lng149    " bajtu"
#define lng150    " obsahuje tyto polozky: "
#define lng151    "  - plosny jas             ANO"
#define lng152    "  - teplota                "
#define lng155    "  - tlak                   "
#define lng156    "  - vlhkost                "
#define lng157    "  - infra slozka svetla    "
#define lng158    "  - full svetlo            "
#define lng159    "  - ctrl registr TSL2591   "
#define lng206    "  - GPS souradnice         "
#define lng228    "  - naklon                 "
#define lng264    "  - azimut                 "
#define lng236    "  - poloha Slun. a Mes.    "
#define lng305    "  - rezerva 1              "    // zatim nepouzita rezervni cidla (napisy do "sys_info()")
#define lng306    "  - rezerva 2              "
#define lng307    "  - rezerva 3              "
#define lng308    "  - rezerva 4              "
#define lng153    "ANO"
#define lng154    "NE"
#define lng160    "\r\nKalibrace cidla teploty"
#define lng161    "index ; merena  ; skutecna"
#define lng162    "\r\nKalibrace cidla plosneho jasu ["
#define lng163    "index ; merene ; skutecne"
#define lng164    "SOFT format EEPROM ..."
#define lng165    "HARD format EEPROM ..."
#define lng166    "Formatovani EEPROM ukonceno."
#define lng167    "Zvolena polozka je mimo rozsah EEPROM (1 - "
#define lng168    "Zadny ulozeny zaznam"
#define lng169    "Kontrolni bajt nesouhlasi:"
#define lng170    "   ocekavane CRC: "
#define lng171    "   prijate CRC: "
#define lng172    "Pro tuto funkci nelze pouzit univerzalni adresu 127."
#define lng173    "univerzalni pozadavek vykonan"
#define lng174    "Informace o RTC"
#define lng175    "Aktualne je "
#define lng176    "Cas v RTC bez korekci (UTC): "
#define lng177    "Cas posledniho serizeni RTC (data z EEPROM) (UTC): "
#define lng178    " ... v sekundach od 1.1.1970: "
#define lng179    " ... v citelnem tvaru: "
#define lng180    "Interval od posledniho nastaveni casu: "
#define lng181    "Korekce casu (data z EEPROM): "
#define lng182    " ... ulozene cislo v HEX formatu : "
#define lng183    " ... sekunda se "
#define lng184    "odecita"
#define lng185    "pricita"
#define lng186    " kazdych "
#define lng187    " sekund od posledniho serizeni."
#define lng188    "           (to je asi "
#define lng189    " hodin)"
#define lng190    " dni)"
#define lng191    " roku)"
#define lng192    "Zatim se takto melo pridat/ubrat: "
#define lng193    " extra sekund."
#define lng194    "Takze zkorigovany cas pro zobrazeni je: "
#define lng195    "Prijato: "
#define lng197    "Rok: "
#define lng198    "Mesic: "
#define lng199    "Den: "
#define lng200    "Hodina: "
#define lng201    "Minuta: "
#define lng202    "Sekunda: "
#define lng203    "Rozdil mezi casem v RTC a zadanym casem [s]: "
#define lng204    "Chyba RTC (sekunda se ma korigovat casteji, nez za 5 minut)."
#define lng205    "Automaticka kalibrace RTC zrusena."
#define lng207    "Do EEPROM se ukladaji vsechny zaznamy"
#define lng208    "@G           ... vypis dat z GPS modulu"
#define lng209    "@S n         ... rychlost seriove komunikace (0=9600; 1=19200; 2=38400; 3=115200)"
#define lng210    "kalibracni bod smazan"
#define lng212    "#E              ... vypis pozorovacich stanovist"
#define lng213    "#ETn text       ... editace textu u pozorovacich stanovist"
#define lng214    "#EGn g,g,g,g,g  ... editace grafiky u pozorovacich stanovist"
#define lng215    "Zapis do 'RTC_set.csv' POVOLEN"
#define lng216    "Zapis do 'RTC_set.csv' ZAKAZAN"
#define lng217    "@> t,n,par   ... vypis souboru z SD karty do seriove linky"
#define lng218    "@h   (H)     ... zakazat (povolit) zapis do souboru RTC_set.csv"
#define lng219    "Velka odchylka mezi novym casem a casem v RTC"
#define lng220    "Kratky interval od posledniho serizeni (< 1 den)"
#define lng221    "Dlouhy interval od posledniho serizeni (> pul roku)"
#define lng223    "Pipani povoleno"
#define lng224    "Pipani zakazano"
#define lng225    "rh nnnnn ... vypsat zaznamy z poslednich 'nnnnn' hodin"
#define lng226    "n        ... zobrazit aktualni naklon"
#define lng227    "#Nk          ... Kalibrace naklonomeru"
#define lng229    " Aktualni naklon: "
#define lng230    "#Ni          ... Info o naklonomeru"
#define lng232    "Poloz SQM horizontalne"
#define lng233    "\r\n\r\nPostav SQM svisle"
#define lng234    "\r\n\r\nChyba kalibrace naklonu - nastavuji defaultni hodnoty"
#define lng237    "Aktualni souradnice pro astro vypocty (delka / sirka): "
#define lng238    "Domaci zemepisna sirka: "
#define lng239    "Domaci zemepisna delka: "
#define lng240    "@Gs nnn      ... domaci zemepisna sirka pro astro vypocty"
#define lng241    "@Gd nnn      ... domaci zemepisna delka pro astro vypocty"
#define lng242    "@Gz nn       ... domaci zimni casova zona"
#define lng243    "@Gl nn       ... domaci letni casova zona"
#define lng246    "@Gp SEC SELC ... textovy popis pro zimni a letni casovou zonu"
#define lng244    "Casovy posun zima/leto: "
#define lng245    "x        ... zmer osvetleni (lux)  - bez ukladani"
#define lng248    "#Xa n.nn     ... kalibrace luxmetru - parametr 'a'"
#define lng249    "#Xb n.nn     ... kalibrace luxmetru - parametr 'b'"
#define lng263    "#Xc n.nn     ... kalibrace luxmetru - parametr 'c'"
#define lng265    " Kompas / naklon : LSM303DLHC"
#define lng266    "#Mk          ... kalibrace magnetometru (kompasu)"
#define lng267    "Chyba kalibrace kompasu"
#define lng268    "s        ... smer natoceni (azimut)"
#define lng269    "Azimut: "
#define lng270    "@Z           ... Zapsat parametry na SD"
#define lng271    "@O           ... Obnovit parametry z SD"
#define lng272    "@T           ... Test HW"
#define lng273    "Posledni merene napeti zdroje (5V-30V): "
#define lng274    "Napeti zalohovaci baterie (3V):         "
#define lng275    "Reference (1V8):                        "
#define lng276    "Napeti procesoru (3V3):                 "
#define lng277    "#TG              ... nastaveni casu z GPS"
#define lng278    "Cas neni v signalu GPS dostupny"
#define lng279    "MODBUS zapnuty"
#define lng280    "MODBUS vypnuty"
#define lng281    "@m   (M)     ... vypnout (zapnout) modbus"
#define lng339    "@L           ... vypnout / zapnout RGB LED"   
#define lng282    "     @Lb (B) ... vypnout (zapnout) LED pro test baterie"
#define lng283    "     @Lm (M) ... vypnout (zapnout) LED mereni"
#define lng284    "     @Le (E) ... vypnout (zapnout) LED pri chybe"
#define lng285    "     @Lk (K) ... vypnout (zapnout) LED pri komunikaci"
#define lng301    "     @La (A) ... vypnout (zapnout) LED pri funkci AfD"
#define lng354    "     @Lt (T) ... vypnout (zapnout) LED pri GPS trasovani"
#define lng290    " Funkce RGB LED:"
#define lng286    "   LED baterie [B]:        "
#define lng287    "   LED pri mereni [M]:     "
#define lng288    "   LED pri chybe [E]:      "
#define lng289    "   LED pri komunikaci [K]: "
#define lng300    "   LED pri AfD [A]:        "
#define lng355    "   LED pri GPS tracku [T]: "
#define lng291    "Problem se sbernici I2C"
#define lng292    "Problem s SD kartou"
#define lng293    "SD karta vysunuta"
#define lng294    "Problem s RTC (zalohovaci baterie)"
#define lng295    "Chyba naklonomeru"
#define lng296    " Displejova verze SQM"
#define lng297    " LED verze SQM"
#define lng298    "@Vd          ... displejova verze SQM"
#define lng299    "@Vl          ... LED verze SQM"
#define lng302    "#D ff.f      ... nastaveni hranice AfD (13.0 - 25.4) ostatni = vypnuto"
#define lng303    "AfD nastaven na "
#define lng304    "AfD vypnuty"
#define lng309    "rd nnnnn ... vypis jednoho zvoleneho zazamu v integerovem (datovem) formatu"
#define lng310    "b n hh:mm ... nastaveni jednoho z 5 budiku"
#define lng311    "budik c. "
#define lng312    " vypnuty"
#define lng313    "Vypni napajeni a odpoj USB kabel. Za 30 sekund by se mel spustit alarm."
#define lng314    "Cas v DS3231: "
#define lng315    "Obsah registru DS3231:"
#define lng316    "#Mo nn.n     ... nastaveni offsetu kompasu (-12.7 ... +12.7 stupne)"
#define lng317    "Kompas pootocen o [stupne]: "
#define lng318    "odpocet   "
#define lng319    "o -nnn   ... nastaveni odpoctoveho alarmu [nnn = minuty]"
#define lng320    "\nchyba pri otevirani souboru / soubor neexistuje"
#define lng321    "Verze zalohovaneho souboru se neshoduje s verzi pozadovanou v tomto programu."
#define lng222    "@P           ... zakazat / povolit pipani"
#define lng322    "     @Pb (B) ... zakazat (povolit) pipani pro signalizaci baterie"
#define lng323    "     @Pm (M) ... zakazat (povolit) pipani pro mereni"
#define lng324    "     @Pa (A) ... zakazat (povolit) pipani pro budiky a odpocet"
#define lng325    "     @Pe (E) ... zakazat (povolit) pipani pro HW chyby"
#define lng326    "     @Pt (T) ... zakazat (povolit) pipani pro operace s TS tlacitkem"
#define lng327    "     @Po (O) ... zakazat (povolit) pipani pro priblizovani k objektu (Ra/Dec)"
#define lng328    "     @Pv (V) ... zakazat (povolit) pipani pro dalsi vystrahy"
#define lng329    "     @Ps (S) ... zakazat (povolit) pipani pro stopky"
#define lng352    "     @Pz (Z) ... zakazat (povolit) pipnuti pri zapnuti"
#define lng330    " Signalizace piskaku:"
#define lng331    "   baterie [B]:            "
#define lng332    "   mereni [M]:             "
#define lng333    "   budik a odpocet [A]:    "
#define lng334    "   HW problemy [E]:        "
#define lng335    "   TS tlacitko [T]:        "
#define lng336    "   funkce Ra-Dec [O]:      "
#define lng337    "   dalsi vystrahy [V]:     "
#define lng338    "   stopky [S]:             "
#define lng353    "   zapnuti napajeni [Z]:   "
#define lng340    "Cas nastaven "
#define lng341    "#Mi          ... Info o magnetometru"
#define lng342    "     (pocet radek: "
#define lng343    "*m           ... spustit mereni bez vystupu do seriove linky"
#define lng344    "*n           ... zadost o index posledniho zaznamu"
#define lng345    "*a           ... test na shodu SLAVE adresy"
#define lng346    "*f           ... vypis kodu vsech funkci"
#define lng347    "*r nnn       ... stazeni jednoho vybraneho zaznamu v datovem formatu (alternativa k prikazu 'rd')"
#define lng348    "@N nn        ... nastavit elevaci Slunce pro uzivatelsky definovany soumrak"
#define lng349    "Uzivatelsky definovany soumrak je pocitan pri elevaci Slunce -"
#define lng350    " stupnu."
#define lng351    "*s p c d.m.r ... simulace planety slunecni soustavy"
#define lng356    "Trasovani do souboru 'trck"
#define lng357    ".gpx' ZAPNUTO."
#define lng358    "Trasovani do GPX souboru vypnuto."
#define lng383    "Celkovy pocet GPS bodu v souboru: "
#define lng360    "pocet GPS bodu: "            // zapis poznamky do GPX souboru. Musi byt dodrzena delka textu.
#define lng361    "prumery  "                   // zapis nazvu waypointu do GPX souboru. Musi byt dodrzena delka textu.
#define lng362    "@A l,f,o     ... automaticke odesilani zaznamu do seriove linky (bez vyzadani)"
#define lng363    "   Linka:    "
#define lng364    " USB"
#define lng365    " RS485"
#define lng366    "Automatickym odesilanim zaznamu pres RS485 byl vypnut modbus"
#define lng367    " "
#define lng368    " "
#define lng369    "   Format:   "
#define lng370    " data"
#define lng371    " text"
#define lng372    "   Obsah:    "
#define lng373    " datum + jas "
#define lng374    "+ pocasi "
#define lng375    "+ detaily jasu "
#define lng376    "+ naklon + azimut "
#define lng377    "+ poloha Slunce a Mesice "
#define lng378    "kompletni zaznam"
#define lng379    "Automaticke odesilani dat "
#define lng380    "vypnuto"
#define lng381    "Nejsou data k odeslani"
#define lng382    "Zapnutim modbusu bylo vypnuto automaticke odesilani zaznamu pres RS485"
#define lng384    "[V] (nyni na USB)"
#define lng385    "*e aaa       ... cteni 1 bajtu z EEPROM"
// pokracovat:  #define lng386


// textove popisky do testovacich funkci
#define lng_test_001     "a .... sken I2C"
#define lng_test_002     "b .... test A/D"
#define lng_test_003     "c .... test EEPROM 128k"
#define lng_test_004     "d .... test extra EEPROM (r30s)"
#define lng_test_005     "e .... test piskak"
#define lng_test_006     "f .... test BME280"
#define lng_test_007     "g .... test TSL2591"
#define lng_test_008     "h .... test displej"
#define lng_test_009     "i .... test EXT kontaktu a zasunute karty"
#define lng_test_010     "j .... test tlacitek"
#define lng_test_011     "k .... test LED"
#define lng_test_012     "l .... test SD karty"
#define lng_test_013     "m .... test linky RS485"
#define lng_test_014     "n .... test GPS"
#define lng_test_015     "o .... test kompasu a naklonomeru LSM303"
#define lng_test_016     "p .... odposlech GPS"
#define lng_test_017     "r .... test RTC v STM32F4x1"
#define lng_test_018     "s .... nastaveni nejakeho casu do RTC v STM32F4x1"
#define lng_test_019     "t .... bocni vystup na LOW"
#define lng_test_020     "T .... bocni vystup na HIGH"
#define lng_test_021     "u .... test casu v RTC DS3231"
#define lng_test_022     "U .... test probuzeni za 30s"
#define lng_test_023     "x .... navrat z testu HW do normalniho rezimu"
#define lng_test_024     "Test displeje"
#define lng_test_025     "Konec testu displeje"
#define lng_test_026     "Skutecne napeti na pinu Ref:  "
#define lng_test_027     "Skutecne napeti na pinu Ubat: "
#define lng_test_028     "Test funkce tlacitek."
#define lng_test_029     " Stiskni a drz tlacitko ... "
#define lng_test_030     "\r\nV intervalu 10 sekund nebylo zadne tlacitko stisknuto - KONEC TESTU."
#define lng_test_031     "[NAHORU]"
#define lng_test_032     "[DOLU]  "
#define lng_test_033     "[OK]    "
#define lng_test_034     "[TS]    "
#define lng_test_035     "   Zakmity = "
#define lng_test_036     "   Trvani = "
#define lng_test_037     "  Uvolni tlacitko ... "
#define lng_test_038     "\r\nV intervalu 10 sekund nebylo zadne tlacitko uvolneno - KONEC TESTU."
#define lng_test_041     "Sken I2C\r\n|"            // posledni znak za odradkovanim musi byt svislitko '|'
#define lng_test_042     "puvodni hodnota na adrese [00000] = "
#define lng_test_043     "puvodni hodnota na adrese [32768] = "
#define lng_test_044     "Kapacita 64kB "
#define lng_test_045     "Kapacita 32kB "
#define lng_test_046     "Chyba pameti"
#define lng_test_047     "Test piskaku"
#define lng_test_048     "Pocet sekund v RTC: "
#define lng_test_049     "tt po ulozeni: "
#define lng_test_050     "RTC nastaveno na: 21.10.2023 11:12:13 Ne"
#define lng_test_051     "GPS neni zapnuta (data nejsou k dispozici)."
#define lng_test_052     "Propoj dratem pin Tx na desce SQM-BAS7 a pin GPS-OUT na desce SQM-GPS6"
#define lng_test_053     "K ukonceni testu zrus propojku a pockej 5 sekund"
#define lng_test_054     "odposlech GPS ukoncen"
#define lng_test_055     "Chyba I2C ... kod: "
#define lng_test_056     "Teplota: "
#define lng_test_057     "      Vlhkost: "
#define lng_test_058     "      Tlak: "
#define lng_test_059     "Karta neni zasunuta - Konec testu"
#define lng_test_060     "Testovaci soubor: "
#define lng_test_061     "Nahodne cislo: "
#define lng_test_062     "pokusny zapis nahodneho cisla: "
#define lng_test_063     "Konec zapisu"
#define lng_test_064     "Vypis obsahu souboru: "
#define lng_test_065     "funkce 'soubor.open()' pro cteni skoncila chybou"
#define lng_test_066     "funkce 'soubor.open()' pro zapis skoncila chybou"
#define lng_test_067     "Bocni konektor (Data_Ready): "
#define lng_test_068     "stav: HIGH"
#define lng_test_069     "stav: LOW"
#define lng_test_070     "Kontakt na drzaku SD: "
#define lng_test_073     "Test RGB LED"
#define lng_test_074     "Blika zelena..."
#define lng_test_075     "Blika ruda..."
#define lng_test_076     "Blika modra..."
#define lng_test_077     "15 sekundovy test kompasu a naklonomeru LSM303"
#define lng_test_078     "zbyva: "
#define lng_test_079     "          zrychleni: aX= "
#define lng_test_080     "    aY= "
#define lng_test_081     "    aZ= "
#define lng_test_083     "          magnetometr: mX= "
#define lng_test_084     "    mY= "
#define lng_test_085     "    mZ= "
#define lng_test_086     "Odesli nekolik znaku z PC pres RS485 a sleduj na PC odpoved"
#define lng_test_087     "  (libovolnym znakem v USB lince se test ukonci)"
#define lng_test_088     "ODPOVED: "
#define lng_test_089     "prijato / odesilam zpet: "
#define lng_test_090     "Bocni vystup nastaven na: "
#define lng_test_091     "Nalezena adresa: "
#define lng_test_092     "Napeti na procesoru:          "
#define lng_test_093     "Napeti na 3V baterii:         "
#define lng_test_094     "Cas je nespolehlivy (vypadek 3V clanku)"
#define lng_test_095     "Od posledniho serizeni nenastal vypadek 3V clanku"
#define lng_test_096     "- .... Automenu OFF/ON"

#define lng30_001    "Stav mereni: "
#define lng30_002    "Interval mezi merenim: "
#define lng30_003    "Konec vypisu"
#define lng30_004    "Pozastaveno"
#define lng30_005    "Spusteno"
#define lng30_006    "Zachytny bod pro hledani volneho mista: "
#define lng30_007    "Adresa pro ulozeni dalsiho zaznamu: "
#define lng30_008    "Velikost pridavne EEPROM2: "
#define lng30_009    "Mereni pozastaveno"
#define lng30_010    "Mereni spusteno"
#define lng30_011    "puvodni casovani: "
#define lng30_012    "nove casovani: "
#define lng30_013    "chyba formatovani"
#define lng30_014    "Dalsi zaznam na adresu 10."
#define lng30_015    "Ukladam zaznamy na SD kartu ... "
#define lng30_016    "ULOZENO"
#define lng30_017    "funkce 'soubor.open()' skoncila chybou"
#define lng30_018    "%I     ... detailni informace"
#define lng30_019    "%L     ... vypis poslednich 24 hod. do seriove linky"
#define lng30_020    "%Lh n  ... poslednich 'n' hod. do seriove linky"
#define lng30_021    "%La    ... kompletni vypis do seriove linky"
#define lng30_022    "%Lz n  ... poslednich 'n' zaznamu do seriove linky"
#define lng30_023    "%S     ... ulozeni na SD kartu"
#define lng30_024    "%Lb    ... bajtovy vypis"
#define lng30_025    "%P     ... pozastavit mereni"
#define lng30_026    "%R     ... pokracovat v mereni"
#define lng30_027    "%C n   ... nastaveni mericiho intervalu (10-255)"
#define lng30_028    "%F     ... formatovani pridavne EEPROM"
#define lng30_029    "%N     ... nastaveni na nulovou pozici"
#define lng30_030    "%?     ... tato napoveda"
#define lng30_031    "%W a,d ... zapis hodnoty [d] na adresu [a] v EEPROM2"
#define lng30_032    "%X a   ... cteni hodnoty z adresy [a] z EEPROM2"




//                           0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999
//                           0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
char hlavicka30[]         = "index ;  sek 1970  ;   datum  ;   cas  ; DvT ; teplota[C]; vlhkost[%]; tlak[Pa] ; osvetleni [lx] ;\0";
//                          "00044 ; 1631464959 ;12.09.2021;16:42:39;  Ne ;   +26,48  ;   056,21  ;  095953  ;    78123,12    ;\0";

char hlavicka_E0[]  =       "  adr. ; typ ;inter; blok;PAUZA;  kapacita ; inx ;     REZERVA     ;\0";
//                          "     0 ; 100 ;  30 ;   0 ;   0 ; 255 ; 220 ; 001 ; 255 ; 255 ; 255 ;"  

char hlavicka_E10[] =       "  adr. ;        sek 1970       ;  teplota  ;  vlhkost  ;    tlak   ;    osvetleni    ;\0";
//                          " 32500 ;  97 ;  62 ;  40 ; 154 ;  29 ; 213 ;  22 ;  35 ; 140 ; 128 ; 132 ; 123 ; 231 ;"  





// popisky do CSV zaznamu - NUTNO DODRZET PRESNY POCET ZNAKU (dlouhe texty zkratit, kratke doplnit mezerami)
#define lng254    " STABIL "            // znacka stabilniho jasu
#define lng255    " NESTAB "            // znacka nestabilniho jasu
#define lng256    "tl_UP"               // znacka, ze bylo mereni spusteno tlacitkem "Sipka nahoru"
#define lng257    "tl_DN"               // znacka, ze bylo mereni spusteno tlacitkem "Sipka dolu"
#define lng258    "tl_OK"               // znacka, ze bylo mereni spusteno tlacitkem "OK"
#define lng259    "komun"               // znacka, ze bylo mereni spusteno pres komunikacni linku (USB nebo RS485)
#define lng260    " auto"               // znacka, ze bylo mereni spusteno automatickym casovacem
#define lng261    "kalib"               // znacka, ze je zaznamenana hodnota svetla bez kalibracni korekce 
#define lng262    "Ka-Pr"               // znacka, ze je zaznamenana hodnota matematickym prumerem z predchozich 10 vzorku svetla bez kalibracni korekce 



// prirazeni znakovych kodu pro funkce spoustene pres USB seriovou linku
//  bezne funkce (bez znacky)
//           v kazdem sloupci musi byt znak unikatni
#define USB_fce_1_01 'v'             // ... zmer vlhkost     - bez ukladani
#define USB_fce_1_02 't'             // ... zmer teplotu     - bez ukladani
#define USB_fce_1_03 'p'             // ... zmer tlak        - bez ukladani
#define USB_fce_1_04 'j'             // ... zmer plosny jas  - bez ukladani
#define USB_fce_1_05 'z'             // ... prepnout na SEC ("z0") nebo SELC ("z1")
#define USB_fce_1_06 'a'             // ... nastaveni automatickeho spousteni 0-255 minut
#define USB_fce_1_07 'e'             // ... pri pouziti expanzni desky prepne na zvolene cidlo (1-7)
#define USB_fce_1_08 'r'             // ... vypis zaznamu z EEPROM (bez dalsiho parametru vypise poslednich 100 zaznamu)
#define USB_fce_1_09     'p'         // ...          "rp" ... vypis posledniho zaznamu
#define USB_fce_1_10     'a'         // ...          "ra" ... vypis vsech zaznamu
#define USB_fce_1_11     'z'         // ...          "rz" ... vypis jednoho vybraneho zaznamu
#define USB_fce_1_19     'h'         // ...          "rh" ... vypis zaznamu za posledni obdobi (treba za posledni den)
#define USB_fce_1_23     'x'         // ...          "rx" ... vypis posledniho zaznamu ve formatu pro software od Unihedronu
#define USB_fce_1_26     'd'         // ...          "rd" ... vypis jednoho vybraneho zaznamu v datovem formatu
#define USB_fce_1_12 'c'             // ... aktualni datum a cas
#define USB_fce_1_13 'i'             // ... informace o aktualnim nastaveni systemu
#define USB_fce_1_25     'x'         // ...          "ix" informace o zarizeni ve formatu pro software od Unihedronu
#define USB_fce_1_14 'h'             // ... zobrazeni hlavicky pro formatovany vystup
#define USB_fce_1_15 'm'             // ... spusteni vsech mereni se zapisem do EEPROM
#define USB_fce_1_16 'k'             // ... kalibrace
#define USB_fce_1_17         'j'     // ... kalibrace jasu  ("kj")
#define USB_fce_1_18         't'     // ... kalibrace teploty ("kt")
#define USB_fce_1_20 'n'             // ... n aktualni naklon
#define USB_fce_1_21 'x'             // ... zmer osvetleni (lux)
#define USB_fce_1_22 's'             // ... zjisti smer natoceni (azimut)
#define USB_fce_1_24 'R'             // ... "R...." vypis posledniho zaznamu ve formatu pro software od Unihedronu
#define USB_fce_1_27 'b'             // ... nastaveni jednoho z 5 budiku  "b 2 15:48"
#define USB_fce_1_28 'o'             // ... nastaveni odpoctu




//  servisni funkce (se znackou '#')
//           v kazdem sloupci musi byt znak unikatni
#define USB_fce_2_01 'T'                             // ... #T = nastaveni datumu a casu
#define USB_fce_2_33     'G'                         // ... #TG = nastaveni datumu a casu z GPS
#define USB_fce_2_02 'P'                             // ... #P = nastaveni poctu prumerovani
#define USB_fce_2_03 'S'                             // ... #S = nastaveni urovne stability (0-255)
#define USB_fce_2_04 'I'                             // ... #I = detailni informace o kalibraci vnitrnich hodin
#define USB_fce_2_05 'A'                             // ... #A = nastaveni SLAVE adresy pro komunikaci (1-15)
#define USB_fce_2_06 'C'                             // ... #C zapnuti, nebo vypnuti kontroly CRC pro seriovou komunikaci (#C0=CRC se nekontroluje; #C1=CRC se kontroluje; #C2=jako #C1 ale s vypisem chyb do USB)
#define USB_fce_2_07 'O'                             // ... #O nastaveni oddelovacu
#define USB_fce_2_08     'o'                         // ...       #Oo = oddelovac polozek (nasleduje jeste jeden znak, ktery urcuje oddelovac: #Oo, ; #Oo; ;  #Oo_ ; #Oot)
#define USB_fce_2_09     ','                         // ...       #O, = carka jako desetinny oddelovac                 (#O,0 = vypnout carku, zapnout tecku   #O,1 = zapnout carku, vypnout tecku)
#define USB_fce_2_10     '"'                         // ...       #O" = uzavirat nebo neuzavirat polozky do uvozovek   (#O"1 = uzavirat                       #O"0 = neuzavirat)
#define USB_fce_2_11     'h'                         // ...       #Oh = zapisovat nebo nezapisovat hlavicku do souboru (#Oh0 = nezapisovat                    #Oh1 = zapisovat)
#define USB_fce_2_12 'F'                             // ... #F formatovani EEPROM (nasleduje jeste jeden znak pro urceni zpusobu mazani dat)
#define USB_fce_2_13         'S'                     // ...       #FS = SOFT format
#define USB_fce_2_14         'H'                     // ...       #FH = HARD format
#define USB_fce_2_15 'U'                             // ... #U = hranice jasu pro ukladani zaznamu do EEPROM 
#define USB_fce_2_16 'K'                             // ... #K zapis bodu kalibracni tabulky (nasleduje jeden znak pro urceni tabulky)
#define USB_fce_2_17             'J'                 // ...       kalibracni tabulka pro svetlo  (#KJ)
#define USB_fce_2_18             'T'                 // ...       kalibracni tabulka pro teplotu (#KT)
#define USB_fce_2_19 'E'                             // ... #E editace pozorovacich stanovist
#define USB_fce_2_20                 'T'             // ...      textove popisy
#define USB_fce_2_21                 'G'             // ...      graficke popisy na displeje
#define USB_fce_2_22 'N'                             // ... #N nastaveni naklonomeru
#define USB_fce_2_24                    'i'          // ...      informace o naklonomeru (#Ni)
#define USB_fce_2_23                    'k'          // ...      kalibrace naklonomeru (#Nk)
#define USB_fce_2_27 'X'                             // ... #X kalibrace luxmetru
#define USB_fce_2_28                        'a'      // ... #Xa     parametr 'a'
#define USB_fce_2_29                        'b'      // ... #Xb     parametr 'b'
#define USB_fce_2_30                        'c'      // ... #Xc     parametr 'c'
#define USB_fce_2_31 'M'                             // ... #M    Magnetometr (kompas)
#define USB_fce_2_32                            'k'  // ... #Mk     kalibrace
#define USB_fce_2_35                            'o'  // ... #Mo     offset kompasu (doladeni spatne nalepeneho cidla)
#define USB_fce_2_36                            'i'  // ... #Mi     informace o magnetometru
#define USB_fce_2_34 'D'                             // ... #D    nastaveni AfD
// pokracovat indexem 2_37   #define USB_fce_2_37

//  tajne funkce (se znackou '@')
//           v kazdem sloupci musi byt znak unikatni
#define USB_fce_3_01 'E'                      // ... @E = test postupneho prepinani cidel na expanzni desce
#define USB_fce_3_02 'X'                      // ... @X = nastaveni poctu vstupu na expanzni desce (1-7)
#define USB_fce_3_03 'R'                      // ... @R = prime cteni dat z EEPROM 
#define USB_fce_3_04 'W'                      // ... @W = primy zapis hodnoty do EEPROM 
#define USB_fce_3_05 'D'                      // ... @D obnoveni defaultnich hodnot (nasleduje 1 znak pro urceni toho, co se ma obnovit)
#define USB_fce_3_06     'S'                  //          @DS = obnoveni kalibracnich hodnot pro svetlo
#define USB_fce_3_07     'T'                  //          @DT = obnoveni kalibracnich hodnot pro teplotu
#define USB_fce_3_08     'P'                  //          @DP = obnoveni parametru systemu
#define USB_fce_3_10 '@'                      // ... @@ = reset
#define USB_fce_3_11 'G'                      // ... @G = vypis dat z GPS modulu
#define USB_fce_3_19          's'             // ... @Gs = nastaveni domaci zemepisne sirky (latitude) pro pozorovaci stanoviste (pouziva se pro vypocet elevace slunce a Mesice)
#define USB_fce_3_20          'd'             // ... @Gd = nastaveni domaci zemepisne delky (longitude) pro pozorovaci stanoviste (pouziva se pro vypocet elevace slunce a Mesice)
#define USB_fce_3_21          'z'             // ... @Gz = nastaveni domaci casove zony pro "zimni" cas
#define USB_fce_3_22          'l'             // ... @Gl = nastaveni domaci casove zony pro "letni" cas
#define USB_fce_3_23          'p'             // ... @Gp = nastaveni textovych popisku pro zimni a letni cas
#define USB_fce_3_12 'S'                      // ... @S = nastaveni rychlosti seriove komunikace
#define USB_fce_3_15 '>'                      // ... @> = vypis souboru z SD karty do seriove linky
#define USB_fce_3_16 'h'                      // ... @h = vypnuti zapisu informaci o nastaveni RTC obvodu do souboru "RTC_set.csv
#define USB_fce_3_63 'H'                      // ... @H = zapnuti zapisu informaci o nastaveni RTC obvodu do souboru "RTC_set.csv
#define USB_fce_3_18 '*'                      // ... @* nnn = nezdokumentovana servisni funkce pro vypis jednoho zaznamu z EEPROM v HEX formatu
#define USB_fce_3_24 'Z'                      // ... @Z zapsat systemove parametry na SD kartu
#define USB_fce_3_25 'O'                      // ... @O obnovit systemove parametry z SD karty
#define USB_fce_3_26 'T'                      // ... @T Funkce pro otestovani HW
#define USB_fce_3_27 'm'                      // ... @m vypnout modbus
#define USB_fce_3_28 'M'                      // ... @M zapnout modbus
#define USB_fce_3_33 'V'                      // ... @V verze SQM 
#define USB_fce_3_34              'd'         // ...       @Vd displejova verze
#define USB_fce_3_35              'l'         // ...       @Vl LED verze
#define USB_fce_3_36 'L'                      // ... @L blokovani/povolovani RGB LED 
#define USB_fce_3_37                  'b'     // ...       @Lb blokovani  RGB LED (blikani pri testu baterie)
#define USB_fce_3_38                  'B'     // ...       @LB povolovani RGB LED (blikani pri testu baterie)
#define USB_fce_3_39                  'm'     // ...       @Lm blokovani  RGB LED (blikani pri mereni)
#define USB_fce_3_40                  'M'     // ...       @LM povolovani RGB LED (blikani pri mereni)
#define USB_fce_3_41                  'e'     // ...       @Le blokovani  RGB LED (blikani pri chybe)
#define USB_fce_3_42                  'E'     // ...       @LE povolovani RGB LED (blikani pri chybe)
#define USB_fce_3_43                  'k'     // ...       @Lk blokovani  RGB LED (blikani pri komunikaci pres RS485)
#define USB_fce_3_44                  'K'     // ...       @LK povolovani RGB LED (blikani pri komunikaci pres RS485)
#define USB_fce_3_45                  'a'     // ...       @La blokovani  RGB LED (blikani pri AfD)
#define USB_fce_3_46                  'A'     // ...       @LA povolovani RGB LED (blikani pri AfD)
#define USB_fce_3_67                  't'     // ...       @Lt blokovani  RGB LED (GPS tracking)
#define USB_fce_3_68                  'T'     // ...       @LT povolovani RGB LED (GPS tracking)
#define USB_fce_3_17 'P'                      // ... @P = povolit/zakazat pipani
#define USB_fce_3_47                      'b' // ...       @Pb vypnout pipani pri zmene urovne napeti baterie
#define USB_fce_3_48                      'B' // ...       @PB zapnout pipani pri zmene urovne napeti baterie
#define USB_fce_3_49                      'm' // ...       @Pb vypnout pipani pri mereni
#define USB_fce_3_50                      'M' // ...       @PB zapnout pipani pri mereni
#define USB_fce_3_51                      'a' // ...       @Pa vypnout pipani pri alarmu (budiku) / odpoctu
#define USB_fce_3_52                      'A' // ...       @PA zapnout pipani pri alarmu (budiku) / odpoctu
#define USB_fce_3_53                      'e' // ...       @Pe vypnout pipani pri HW chybe
#define USB_fce_3_54                      'E' // ...       @PE zapnout pipani pri HW chybe
#define USB_fce_3_55                      't' // ...       @Pt vypnout pipani pri operacich s bocnim tlacitkem
#define USB_fce_3_56                      'T' // ...       @PT zapnout pipani pri operacich s bocnim tlacitkem 
#define USB_fce_3_57                      'o' // ...       @Po vypnout pipani pri mireni na objekt (RA-DEC)
#define USB_fce_3_58                      'O' // ...       @PO zapnout pipani pri mireni na objekt (RA-DEC)
#define USB_fce_3_59                      'v' // ...       @Pv vypnout pipani pri vystahach
#define USB_fce_3_60                      'V' // ...       @PV zapnout pipani pri vystrahach
#define USB_fce_3_61                      's' // ...       @Ps vypnout pipani pri stopkach
#define USB_fce_3_62                      'S' // ...       @PS zapnout pipani pri stopkach
#define USB_fce_3_65                      'z' // ...       @Pz vypnout pipnuti pri zapnuti
#define USB_fce_3_66                      'Z' // ...       @PZ zapnout pipnuti pri zapnuti
#define USB_fce_3_64 'N'                      // ... @N nn = nastaveni elevace Slunce pro definovatelny soumrak
#define USB_fce_3_69 'A'                      // ... @A l,i,f,o = zapinani a vypinani rezimu automatickeho odesialni dat do seriove linky
// pokracovat indexem 3_70 #define USB_fce_3_70


#define USB_fce_4_01 'I'                      // ... %I     ... detailni informace
#define USB_fce_4_02 'L'                      // ... %L     ... vypis poslednich 24 hod. do seriove linky
#define USB_fce_4_03      'h'                 // ... %Lh n  ... poslednich 'n' hod. do seriove linky
#define USB_fce_4_04      'a'                 // ... %La    ... kompletni vypis do seriove linky  
#define USB_fce_4_05      'z'                 // ... %Lz n  ... poslednich 'n' zaznamu do seriove linky  
#define USB_fce_4_07      'b'                 // ... %Lb    ... bajtovy vypis
#define USB_fce_4_06 'S'                      // ... %S     ... ulozeni na SD kartu
#define USB_fce_4_08 'P'                      // ... %P     ... pozastavit mereni
#define USB_fce_4_09 'R'                      // ... %R     ... pokracovat v mereni
#define USB_fce_4_10 'C'                      // ... %C n   ... nastaveni mericiho intervalu (10-255)
#define USB_fce_4_11 'F'                      // ... %F     ... formatovani pridavne EEPROM
#define USB_fce_4_12 'N'                      // ... %N     ... nastaveni na nulovou pozici
#define USB_fce_4_13 'W'                      // ... %W a,d ... primy zapis dat do pridavne EEPROM
#define USB_fce_4_14 'X'                      // ... %X     ... prime cteni dat z pridavne EEPROM


String head_byte[] =                   // pro vypis bajtove hlavicky 
    {
      "1x status 1",                   // [0]
      "4x sek1970" ,                   // [1]
      "2x jas" ,                       // [2]
      "1x status 2 " ,                 // [3]
      "2x infra" ,                     // [4]
      "2x full" ,                      // [5]
      "1x config" ,                    // [6]
      "2x teplota" ,                   // [7]
      "2x vlhkost" ,                   // [8]
      "2x tlak" ,                      // [9]
      "4x GEO lon" ,                   // [10]
      "4x GEO lat" ,                   // [11]
      "2x GEO alt" ,                   // [12]
      "2x naklon" ,                    // [13]
      "2x azimut" ,                    // [14]
      "1x elevace Slunce" ,            // [15]
      "1x elevace Mesic" ,             // [16]
      "1x osvetleni Mesice",           // [17]
      "2x rezerva 1" ,                 // [18]
      "2x rezerva 2" ,                 // [19]
      "2x rezerva 3" ,                 // [20]
      "2x rezerva 4" ,                 // [21]
      "1x stabilita"                   // [22]
      
    };
    

String dny[] = {"--", "Po" , "Ut" , "St" , "Ct" , "Pa" , "So" , "Ne"}; // pro vypis nazvu dni pro indexy 1 (pondeli) az 7 (nedele)  (0 se nepouzije)
String planety[] = {"------- ", "Merkur  " , "Venuse  " , "Mars    " , "Jupiter " , "Saturn  " , "Uran    " , "Neptun  ", "Pluto   "}; // pro vypis v simulaci slunecni soustavy (funkce "*s"). Vsechny retezce museji byt dlouhe 8 znaku



//          A=1
//        ######
//       #      #
//  F=32 #      #  B=2
//       # G=64 #
//        ######
//       #      #
//  E=16 #      #  C=4
//       #      #
//        ######   ## DP=128               priklady:
//         D=8     ##                        'A' = A + B + C + F + E + G = 1 + 2 + 4 + 32 + 16 + 64 = 119
//                                           'L' = F + E + D             = 32 + 16 + 8              = 56 
uint8_t dis_popisy[140][5] =            // graficke definice 5-znakovych textu na displej
    {
      // --- menu ---
      {  56,  48, 109, 120,   0} , // "LI5t "                  [0]
      { 119,  28, 120,  92,   0} , // "Auto "                  [1]
      {  91,  92,  84, 119,   0} , // "2onA "                  [2]
      {  57, 119,  56,  48, 124} , // "CALIb"                  [3]      
      {  94, 119, 120,  28,  55} , // "dAtuM"                  [4]
      {  57, 119, 109,   0,   0} , // "CA5  "                  [5]
      { 120, 121, 115,  56,   0} , // "tEPL "                  [6]
      {  62,  56, 118,   0,   0} , // "VLH  "                  [7]
      {  30, 119, 109,   0,   0} , // "JA5  "                  [8]
      { 118,  92,  64,  55,   4} , // "Ho-Mi"                  [9]
      { 115,  80,  28,  55,   0} , // "PruM "                 [10]
      {  56, 121,  94,   0,   0} , // "LEd  "                 [11]
      { 109, 120,  92, 115,   0} , // "StoP "                 [12] (stopky)
      { 125, 115, 109,   0,   0} , // "6P5  "                 [13] (GPS)
      { 109, 120, 119,  84,   0} , // "5tAn "                 [14] (Stan) Pozorovaci stanoviste      
      {  73, 109, 115, 121,  57} , // "#SPEC"                 [15] Specialni funkce (zap/vyp)
      {  73,   0,   4,  84, 120} , // "# int"                 [16] Prvni parametr specialnich funkci
      {  73, 115, 119,  80,  91} , // "#PAr2"                 [17] Druhy parametr specialnich funkci
      {  73, 115, 119,  80,  79} , // "#PAr3"                 [18] Treti parametr specialnich funkci      
      {  73, 113,  57, 121,   6} , // "#FCE1"                 [19] Spusteni prvniho podprogramu specialnich funkci
      {  73, 113,  57, 121,  91} , // "#FCE2"                 [20] Spusteni druheho podprogramu specialnich funkci
      {  73,  57,  92, 115, 110} , // "#CoPy"                 [21] Prenos dat ze specialni funkce na SD kartu
      {  28, 118, 121,  56,   0} , // "uHEL "                 [22] Naklonomer
      { 119, 109, 120,  80,  92} , // "A5tro"                 [23] Astro vypocty pro Slunce, Mesic a planety
      {  57,  92,  55, 115, 119} , // "CoMPA"                 [24] Kompas      
      {  80, 119,  64,  94, 121} , // "rA-De"                 [25] Rektascenze - Deklinace
      { 119,  56, 119,  80,  55} , // "ALArM"                 [26] Budik (zapina se i pri vypnutem hlavnim vypinaci)
      {  62,  92,  56, 120, 110} , // "VoLty"                 [27] mereni napeti na zdroji
      { 119, 113,  94,   0,   0} , // "AFd  "                 [28] Alarm for Darkness
      {  94, 109,  92,   0,   0} , // "d5o  "                 [29] (DSO) Viditelnosti zvolenych DSO

      
      // --- konec menu ---

      { 121,  80,  80,  64, 109} , // "Err-5"                 [30] chyba kalibracni tabulky svetla ("5" = "S")
      { 121,  80,  80,  64, 120} , // "Err-t"                 [31] chyba kalibracni tabulky teploty
      {  84,  92,  57,  92,  80} , // "noCor"                 [32] zobrazeni prubezneho jasu bez korekce
      {   0,  57,  92,  80,  80} , // " Corr"                 [33] zobrazeni prubezneho jasu s korekci
      {  80, 121, 113,   8,  62} , // "rEF_U"                 [34] test analogoveho vstupu pro referencni napeti
      {  62,   8, 124, 119, 120} , // "U_bAt"                 [35] test analogoveho vstupu pro napeti baterie
      {  64,  56,  92,  88,  64} , // "-Loc-"                 [36] bocni tlacitko zablokovano
      { 109,  94,  64, 121,  80} , // "5d-Er"                 [37] (chyba SD karty)
      { 109, 119,  62, 121,   0} , // "SAVE "                 [38]
      { 124,  28, 109, 109, 110} , // "bu55y"                 [39]
      { 121,  80,  80,  64,   4} , // "Err-i"                 [40] chyba I2C komunikace s cidlem svetla, EEPROM, nebo BME280
      { 121,  80,  80,  64, 125} , // "Err-6"                 [41] chyba GPS - je zasunuty, ale nekomunikuje ("6" = "G")
      {  94, 121, 113, 119,   0} , // "dEFA "                 [42] (obnoveni defaultnich parametru)
      {  73,  64,  92,  84,   0} , // "#-on "                 [43] specialni funkce zapnuty (# znaci tri pomlcky nad sebou)
      {  73,  64,  92, 113, 113} , // "#-oFF"                 [44] specialni funkce vypnuty
      {  84,  92,  64, 109,  94} , // "no-5d"                 [45] neni zasunuta SD karta (toto hlaseni se zobrazuje jen pri startu)
      {  80, 121, 109, 121, 120} , // "rE5Et"                 [46] Reset
      {  84,  64,  57, 119,  56} , // "n-CAL"                 [47] kalibrace naklonomeru
      { 128,   0,   0,   0,   0} , // ".    "                 [48] tecka na leve sedmisegmentovce siglalizuje mereni pomoci automatu
      {   1,   1,   1,   1,   1} , // "^^^^^"                 [49] Horni segmenty na vsech segmentovkach (cislo je pres rozsah)
      {  56,  28, 118,  55,   0} , // "LuHM "                 [50] prepnuto na rezim mereni luxu (LuXMeter) - 'X' se na displeji nepovedno, takze vypada jako 'H'
      { 109, 103,  55,   0,   0} ,  // "5qM  "                [51] prepnuto na puvodni rezim SQM - mereni mag/arcsec2
      {   0, 128,   0,   0,   0} , // " .   "                 [52] tecka na druhe sedmisegmentovce zleva siglalizuje mereni pomoci seriove linky
      { 119,  56, 119,  80,  55} , // "ALArM"                 [53]
      {  55, 121,  84,  28,   0} , // "MEnu "                 [54] zobrazi se pri vstupu do rezimu vypinani / zapinani polozek v menu
      { 121,  80,  80,  64,  84} , // "Err-n"                 [55] chyba naklonomeru pri mereni jasu
      {  55,   4,  64, 109, 121} , // "Mi-5E"                 [56] rezim zobrazeni casu "Minuty-Sekundy"
      {   0,  91,   4,  55, 119} , // " ZiMA"                 [57] (Zima - SEC)
      {   0,  56, 121, 120,  92} , // " LEto"                 [58] (Leto - SELC)
      {  57,  64,  57, 119,  56} , // "C-CAL"                 [59] kalibrace kompasu
      { 121,  80,  80,  64,  57} , // "Err-C"                 [60] chyba kalibrace
      { 121,  80,  80,  64,  80} , // "Err-r"                 [61] chyba RTC (rok je mensi nez 2020)
      { 118,  56, 121,  94, 119} , // "HLEdA"                 [62] mireni na zadane RA/Dec souradnice s pipanim
      { 121,  80,  80,  64, 113} , // "Err-F"                 [63] chyba formatovani (predcasne ukonceni tlacitkem)
      { 118,  92,  80,   4,   0} , // "Hori "                 [64] Horizontalne - pri kalibraci naklonomeru
      {  62, 121,  80, 120,   0} , // "UErt "                 [65] Vertikalne - pri kalibraci naklonomeru
      {   0,   0,   0,   0,   0} , // "     "                 [66] zhasnout displej
      { 118,   0,   0,  64,  64} , // "H  --"                 [67] pri nastavovani budiku se po nastaveni 24H budik zrusi (zobrazi se pomlcky)
      {  64,  64,  64,  64,  64} , // "-----"                 [68] 
      { 247, 222,  64,  64,  64} , // "A.d.---"               [69] funkce Alarm for Darknes vypnuta 
      {  64, 247, 241, 222,  64} , // "-A.F.d.-"              [70] jas je mensi (je vetsi tma), nez je nastaveny ve funkci Alarm for Darknes
      { 119,  56, 247,  92,  84} , // "ALA.on"                [71] po zapnuti se zobrazi znacka, ze je nejaky alarm v EEPROM
      { 115, 121,  80,   4, 113} , // "PEriF"                 [72] vypinani periferii
      {   0,   0, 124,  55, 121} , // "  bME"                 [73] BME280
      {   0,   0,  56, 109,  55} , // "  L5M"                 [74] LSM303
      {   0,   0,   0, 109,  94} , // "   Sd"                 [75] SD karta  
      {   0,   0,  80, 120,  88} , // "  rtc"                 [76] DS3231
      {   0,  80,  79,  63, 109} , // " r305"                 [77] rezim R30s      
      { 247, 241, 222,  92,  84} , // "A.F.d.on"              [78] funkce Alarm for Darknes zapnuta
      {   4,  84, 113,  80, 119} , // "inFrA"                 [79] pri prubeznem mereni jasu je mozne zobrazovat jen infra kanal

      {   0,  55, 121,  80,   0} , // " MEr "                 [80] Merkur
      {   0,  62, 121,  84,   0} , // " VEn "                 [81] Venuse
      {   0,  55, 119,  80,   0} , // " MAr "                 [82] Mars
      {   0,  30,  28, 115,   0} , // " JuP "                 [83] Jupiter
      {   0, 109, 119, 120,   0} , // " 2At "                 [84] Saturn
      {   0,  28,  80, 119,   0} , // " urA "                 [85] Uran
      {   0,  84, 121, 115,   0} , // " nEP "                 [86] Neptun
      {   0, 115,  56,  28,   0} , // " PLu "                 [87] Pluto 
      {   0,  55, 121, 109,   0} , // " ME5 "                 [88] Mesic 
      {   0, 109,  56,  28,   0} , // " SLu "                 [89] Slunce 
      {   0,  84,  92,  88,   0} , // " noc "                 [90] Astronomicka noc
      { 109,  92,  28,  55,  80} , // "SouMr"                 [91] uzivatelsky nastavitelny soumrak

      {  92,  94, 115,  92,  88} , // "odPoc"                 [92] Odpocet / casovac
      { 190,   0,  64,  64,  64} , // "U. ---"                [93] zobrazeni hlavniho napajeni pri neosazene referenci
      { 208,   0,  64,  64,  64} , // "r. ---"                [94] zobrazeni referencniho napeti pri neosazene referenci
      {   0,   0,   0,   0,   0} , // "     "                 [95] zruseno (rezerva)
      { 120, 208,  92, 113, 113} , // "tr.oFF"                [96] trasovani vypnuto
      { 120, 208,   0,  92,  84} , // "tr. on"                [97] trasovani zapnuto

      {  80, 121, 119,  94, 110} , // "rEAdy"                 [98] pripraveno na mereni smerove charakteristiky cidla
      {   0,  80,  28,  84,   0} , // " run "                 [99] zacatek mereni smerove charakteristiky (najeti na 45 stupnu)


      { 124,  92, 208,  72,   0} , // "bor.=  "              [100] Bortleho stupnice
      {  55, 118, 190,   0,   0} , // "MHV.  "               [101] zadavani MHV pri osobni kalibraci Bortleho stupnice
      { 124,  64,  57, 119,  56} , // "b-CAL"                [102] uzivatelska kalibrace Bortleho stupnice
      { 124,  92,  80, 120,  56} , // "bortL"                [103] prepnuti na Bortle v menu "Jas"

      { 115,  28, 109, 118,   0} , // "PuSH"                 [104] Pri autokalibraci signalizuje, ze se ma strcit do otocneho podstavce, aby se natocil na stredovou polohu

                                                            // podmenu DSO
      {  55, 121, 109, 109,   4} , // "MESSi"                [105] volba typu katalogu MESSIER ve funkci DSO
      {  84, 125,  57,   0,   0} , // "n6C  "                [106] volba typu katalogu NGC ve funkci DSO
      {   4,  57,   0,   0,   0} , // "iC   "                [107] volba typu katalogu IC ve funkci DSO
      {  92, 116,  84,   8,  92} , // "ohn_o"                [108] volba ohniska okularu
      { 109,  88, 116,  92, 115} , // "5choP"                [109] volba schopnosti pozorovatele
      {  80, 121, 241,   0, 158} , // "rEF. J."              [110] volba referencniho jasu oblohy pro vypocty viditelnosti otevrenych hvezdokup
      { 115,  80,  28,  55, 121} , // "PruME"                [111] volba prumeru dalekohledu
      {  92, 116,  84,   8,  94} , // "ohn_d"                [112] volba ohniska dalekohledu

      {  55, 119, 125,  84,   4} , // "MAGni"                [113] magnituda objektu
      {  55, 119,  30,  92,  80} , // "MAJor"                [114] delsi rozmer
      {  55,   4,  84,  92,  80} , // "Minor"                [115] kratsi rozmer
      { 120, 110, 115,   0,   0} , // "tyP  "                [116] typ objektu
      { 120,  62, 119,  80,   0} , // "tVAr "                [117] tvar objektu
      { 119,  91,   4,  55,   0} , // "A2iM "                [118] Azimut
      { 121,  56, 121,  62,   0} , // "ELEV "                [119] Elevace
      {  84,   8, 109, 120, 119} , // "n_5tA"                [120] pocet hvezd v hvezdokupe
      { 124,   8, 109, 120, 119} , // "b_5tA "               [121] nejjasnejsi hvezda v hvezokupe
      {  62,   4,  94,   4, 120} , // "Vidit"                [122] stupen viditelnosti

      {  84, 121, 124,  28,  56} , // "nEbuL"                [123] mlhovina  (bez rozliseni planetarni / emisni / zbytek supernovy)
      {  92, 115,   0,  88,  56} , // "oP cL"                [124] otevrena hvezdokupa
      { 125, 119,  56, 119, 118} , // "6ALAH"                [125] galaxie
      { 125,  56,   0,  88,  56} , // "6L cL"                [126] kulova hvezdokupa
      {  88, 184,  84, 121, 124} , // "cL.nEb"               [127] hvezdokupa s mlhovinou 
      { 109, 120, 119,  80,   0} , // "5tAr "                [128] (dvoj)hvezda
      { 121,  56, 121,  64,  63}   // "ELE-0"                [129] objekt neni videt, protoze je pod horizontem
       
    };


uint8_t test_popisy[30][5] =                                // graficke definice 5-znakovych textu na displej pro testovaci podprogramy
    {

      { 120, 121, 109, 120,   0} ,                       // "tESt "                  [0]
      {   0,  56, 121,  94,   0} ,                       // " LEd "                  [1] test LED
      {   0, 125, 115, 109,   0} ,                       // " 6P5 "                  [2] test GPS
      {  28, 118, 121,  56,   0} ,                       // "uHEL "                  [3] test naklonomeru
      { 121, 121, 115,  80,  92} ,                       // "EEPro"                  [4] test EEPROM
      { 124, 121, 121, 115,   0} ,                       // "bEEP "                  [5] test piskaku
      {   0,  30, 119, 109,   0} ,                       // " JA5 "                  [6] test svetlomeru (TSL)
      {   0, 124,  55, 121,   0} ,                       // " bME "                  [7] test BME280
      {   0, 119,  94,  57,   0} ,                       // " AdC "                  [8] test A/D prevodniku      
      { 120,  56, 119,  57,   0} ,                       // "tLAC "                  [9] test tlacitek
      {   0, 102, 127, 109,   0} ,                       // " 485 "                 [10] test RS485
      {   0,   4,  91,  57,   0} ,                       // " i2C "                 [11] sken I2C
      {   0, 109,  94,   0,   0} ,                       // " 5d  "                 [12] test SD karty
      {   0, 121, 118, 120,   0} ,                       // " EXt "                 [13] test externiho kontaktu
      { 121, 121,  64,  79,  63} ,                       // "EE-30"                 [14] test EEPROM 30
      {  57,  92,  55, 115, 119} ,                       // "CoMPA"                 [15] test kompasu LSM303 
      {   0,   0,   0,   0,   0} ,                       // "     "                 [16] rezerva
      {   0,   0,   0,   0,   0} ,                       // "     "                 [17] rezerva
      {   0,  80, 120,  88,   0} ,                       // " rtc "                 [18] test RTC
      {  94,  64,  92,  28, 120} ,                       // "d-out"                 [19] test bocniho digitalniho vystupu
      {  80, 120,  88,   0,  91}                         // "rtc 2"                 [20] test DS3231
    };


// prvni znaky na displeji, ktere jsou zobrazeny pri nastavovani, nebo zobrazovani polozek v podmenu (graficke definice znaku na 7-seg displeji)
uint8_t znak_V =       62      ;  // "V"   zobrazeni vlhkosti
uint8_t znak_A =      119      ;  // "A"   nastaveni automatu
uint8_t znak_P =      115      ;  // "P"   nastaveni prumerovani
uint8_t znak_d =       94      ;  // "d"   nastaveni dni
uint8_t znak_M =       55      ;  // "M"   nastaveni mesicu / minut
uint8_t znak_r =       80      ;  // "r"   nastaveni roku
uint8_t znak_H =      118      ;  // "H"   nastaveni hodin
uint8_t znak_L =       56      ;  // "L"   listovani v zaznamech

uint8_t znak_R1 =      80      ;  // "r"   prvni znak z polozky rektascenze (rA.)
uint8_t znak_R2 =     119 + 128;  // "A."  druhy znak z polozky rektascenze (rA.)
uint8_t znak_D1 =      94      ;  // "d"   prvni znak z polozky deklinace (dE.)
uint8_t znak_D2 =     121 + 128;  // "E."  druhy znak z polozky deklinace (dE.)

uint8_t znak_AD1 =    119 + 128;  // "A."  prvni znak z polozky Alarm for Darkness (A.d.)
uint8_t znak_AD2 =     94 + 128;  // "d."  druhy znak z polozky Alarm for Darkness (A.d.)



uint8_t znak_b_tecka =124 + 128;  // "b."  znak pro menu "budik" 
uint8_t znak_o_tecka = 92 + 128;  // "o."  znak pro menu "odpocet" 


uint8_t znak_u =       28      ;  // "u"   pro uhlovou vzdalenost

uint8_t znak_eS =     109      ;  // "S"   elevace nebo azimut Slunce
uint8_t znak_eM =      55      ;  // "M"   elevace, azimut, nebo osvetleni Mesice
uint8_t znak_oM =      92 + 128;  // "o."  osvetleni Mesice
uint8_t znak_Atecka = 119 + 128;  // "A."  azimut (Slunce nebo Mesice)
uint8_t znak_Etecka = 121 + 128;  // "E."  elevace (Slunce nebo Mesice)
uint8_t znak_C  =      57      ;  // "C"   kalibrace  (napis "CAL.")
uint8_t znak_Ltecka =  56 + 128;  // "L."  kalibrace  (napis "CAL.")
uint8_t znak_vecer  =  62 + 128;  // "V."  Znacka pro vecerni viditelnost Venuse
uint8_t znak_rano   =  80 + 128;  // "r."  Znacka pro ranni viditelnost Venuse


// defaultni popisy pozorovacich stanovist
String poz_st_txt[] = {"--", "Poz.Stan.1" , "Poz.Stan.2" , "Poz.Stan.3" , "Poz.Stan.4" , "Poz.Stan.5"}; // textovy popis prednastavenych pozorovacich stanovist (10 znaku)
uint8_t poz_st_gr[6][5] =                                                                                  // graficke definice 5-znakovych textu na displej
    {
      {  64,  64,  64,  64,  64} , // "-----"      [0]
      { 109, 120, 119, 212,   6} , // "5tAn.1"     [1]
      { 109, 120, 119, 212,  91} , // "5tAn.2"     [2]
      { 109, 120, 119, 212,  79} , // "5tAn.3"     [3]
      { 109, 120, 119, 212, 102} , // "5tAn.4"     [4]
      { 109, 120, 119, 212, 109}   // "5tAn.5"     [5]
    };


uint8_t znak_dso_okular_1   =    92+128  ;  // 'o.'     (Ohnskova vzdalenost okularu)      "o.o."
uint8_t znak_dso_okular_2   =    92+128  ;  // 'o.'

uint8_t znak_dso_schop_1    =    109+128 ;  // '5.'     (Schopnosti Pozorovatele)          "5.P."
uint8_t znak_dso_schop_2    =    115+128 ;  // 'P.' 

uint8_t znak_dso_ref_1      =    80 + 128;  // 'r.'     (referencni jas)                   "r.J."
uint8_t znak_dso_ref_2      =    30 + 128;  // 'J.' 

uint8_t znak_dso_prumer_1   =    94+128  ;  // 'd.'     (prumer dalekohledu)               "d."

uint8_t znak_dso_ohnisko_1  =    56+128  ;  // 'L.'     (ohniskova vzdalenost dalekohledu) "L."



String popisy3231[] = {               // popisy registru v RTC obvodu DS3231
    "Sekundy",                        // registr 0x00
    "Minuty" ,                        // registr 0x01
    "Hodiny" ,
    "Dny" ,
    "Den v mesici" ,
    "Mesic / Stoleti" ,
    "Rok" ,
    "Alarm 1 sekundy",
    "Alarm 1 minuty",
    "Alarm 1 hodiny", 
    "Alarm 1 dny v tydnu / mesici", 
    "Alarm 2 minuty", 
    "Alarm 2 hodiny", 
    "Alarm 2 dny v tydnu / mesici", 
    "Control        [EOSC | BBSQW | CONV | RS2 |   RS1   | INTCN | A2IE | A1IE]", 
    "Control/Status [ OSF |   0   |   0  |  0  | EN32kHz |  BSY  |  A2F |  A1F]", 
    "Aging Offset", 
    "MSB z teploty", 
    "LSB z teploty"                   // registr 0x12
    };


#endif






//===============================================================================================================================================
#ifdef EN_language
//   EN:
//----------------------------------------------
char hlavicka[]         = "  ind. ; EEPROM ;  sec_1970  ;    date    ;   time   ; wd ; zone ; bright; bright ;addr;sen.; descr.;  Un/Stab ; infra ;  full ; [ms];  gain ;temper ; temp 'C; humid ;humid % ; press. ;   GEO_lat  ;   GEO_long  ;  ALT  ;  elev. ; azm ; S_el ; M_el ; M_sv ; res.1 ; res.2 ; res.3 ; res.4 \0";
// example                " 00018 ; 000823 ; 1609232363 ; 29.12.2020 ; 08:59:23 ; Tu ;  CET ; 08881 ; 08,881 ; 01 ; 01 ; bt_DN ;  UNSTAB  ; 17989 ; 48311 ; 600 ;  428x ; 07196 ; +21,96 ; 05151 ; 051,51 ; 093410 ; +49,457154 ; +120,457156 ; -1234 ; +065.8 ; 123 ;  -45 ;  -27 ;  58  ; 65535 ; 65535 ; 65535 ; 65535 \0"

// output string for simulation of solar system (keep exact width of columns)
char sss_hlavicka[]     = "    date    ;  planet ; index ; elongation ; E/W ;  RA_planet ;  DEC_planet ;   RA_Sun   ;   DEC_Sun  ;   dist_P-E    ;    dist_S-P   ;    dist_E-S   ; phase angle ; illumination ;      planet mag      ;    ring mag   ;   ring + planet mag    ;\0";             
// example                " 28.12.2020 ; Jupiter ;   4   ; 023,725750 ;  ?  ; 303,258917 ; -20,310040  ; 277,872566 ; -77,872566 ;   10,861111   ;   09,968692   ;    0,983389   ;  xx2,274772 ;   0,999606   ;       1,392652       ;    0,791857   ;       0,600795         ;\0";                

char ssm_hlavicka[]     = "    date    ;   Sunset   ;  azim. ;  AD end  ;  DD begin  ;  DD end  ;  AD begin  ;   Sunrise  ;  azim. ;  max Sun elev. ;  elev.  ;  Sun distance [AU]  ;   Moonrise  ;  azim. ;   Moonset  ;  azim. ;  max Moon elev.  ;   elev. ;  Moon distance [km]  ; Moonphase ;\0";
// example                " 13.07.2025 ;    21:07   ;   306  ;   0:23   ;    0:23    ;   1:56   ;    1:56    ;    5:12    ;   54   ;      13:09     ;    62   ;       1.016536      ;    22:43    ;   110  ;    9:18    ;   254  ;       3:59       ;    28   ;        379996        ;     85    ;\0";

#define kod_jazyka    "EN"               // language code for automatic select of language in PC program

#define lng001    "Humidity: "
#define lng002    "Temperature with no correction: "
#define lng003    "Temperature with correction by correction table: "
#define lng004    "Absolute pressure (no recalculate to sea level): "
#define lng005    "Timezone "
#define lng006    "Out of range "
#define lng007    "(10-255)"
#define lng008    "Light value with NO callibration corrections: "
#define lng009    "Light value with callibration corrections:    "
#define lng010    "Automat OFF"
#define lng011    "Timer for automatic measure "
#define lng012    "Index of active light sensor: "
#define lng013    "Light value with NO callibration corrections: "
#define lng014    "Temperature value with NO callibration corrections: "
#define lng015    "Average "
#define lng016    " measures"
#define lng017    "(1-20)"
#define lng018    "Level for stabile light mark: "
#define lng019    "SLAVE address: "
#define lng020    "(1-15)"
#define lng021    "CRC is not tested"
#define lng022    "CRC is tested"
#define lng023    "CRC is tested, mismatches are sending to USB Serial Line"
#define lng024    "Write to EEPROM only values darkest than "
#define lng025    "(0-255)"
#define lng026    "Zadany pocet vstupu pro cidla na expanzni desce: "
#define lng027    "(1 - 7)"
#define lng028    " previous: EEPROM["
#define lng359    " previous: EEPROM2["
#define lng029    "Restore defaults for calibration table of light"
#define lng030    "Restore defaults for calibration table of temperature"
#define lng031    "Restore system parameters defaults"
#define lng036    "l        ... measure brightness  - NO save"
#define lng037    "v        ... measure humidity    - NO save"
#define lng038    "t        ... measure temperature - NO save"
#define lng039    "p        ... measure pressure    - NO save"
#define lng040    "z 0      ... switch to normal timezone - winter"
#define lng041    "z 1      ... switch to DST timezone - summer"
#define lng042    "a nnn    ... automatic measure runs every 0-255 minutes"
#define lng043    "e n      ... pri pouziti expanzni desky prepne na zvolene cidlo (1-7)"
#define lng044    "r        ... dump last 100 records from EEPROM"
#define lng045    "rp       ... dump last record from EEPROM"
#define lng046    "ra       ... dump all records from EEPROM"
#define lng047    "rz nnnnn ... dump one selected record"
#define lng048    "c        ... read current time from internal RTC"
#define lng049    "i        ... system info"
#define lng050    "h        ... show header for output list"
#define lng051    "m        ... run measure with record into EEPROM"
#define lng052    "kj       ... kalibracni mereni jasu (vystup bez korekce)"
#define lng053    "kt       ... kalibracni mereni teploty (vystup bez korekce)"
#define lng054    "#?       ... help for extended functions"
#define lng055    "#Tyyyymmddhhnnss ... set date and time"
#define lng056    "#P nn        ... set count of measures for averages (1-20)"
#define lng057    "#S nnn       ... set of stability mark level (0-255)"
#define lng058    "#I           ... detailed information about internal RTC"
#define lng059    "#A nn        ... setup SLAVE address (1-15)"
#define lng060    "#C 0         ... CRC check for RS485 - OFF"
#define lng061    "#C 1         ... CRC check for RS485 - ON"
#define lng062    "#O?          ... setup of dividers"
#define lng063    "#FS          ... SOFT format EEPROM"
#define lng064    "#FH          ... HARD format EEPROM"
#define lng065    "#U nnn       ... set level of dark for save record into EEPROM - in 0.1x mag/arcsec2 (0-255)"
#define lng066    "#KJ n;x;y*   ... zapis bodu do kalibracni tabulky jasu"
#define lng067    "#KT n;x;y*   ... zapis bodu do kalibracni tabulky teploty"
#define lng068    "@W addr,data ... direct write to EEPROM"
#define lng069    "@R addr      ... direct read from EEPROM"
#define lng070    "@E           ... test postupneho prepinani cidel na expanzni desce"
#define lng071    "@X n         ... nastaveni poctu vstupu na expanzni desce (1-7)"
#define lng072    "@DS          ... set default callibration for brightness sensor"
#define lng073    "@DT          ... set default callibration for temperature sensor"
#define lng074    "@DP          ... set default parameters of system"
#define lng076    "@@           ... software RESET"
#define lng077    "Divider of items: "
#define lng078    "space"
#define lng079    "comma"
#define lng080    "semicolon"
#define lng081    "tabulator"
#define lng082    "Decimal divider: "
#define lng083    "dot"
#define lng084    "Use quotes along values"
#define lng085    "Don't use quotes along values"
#define lng086    " "               // space at the end of previous sentences: "Use quotes along values" + " "; "Don't use quotes along values" + " "
#define lng087    "Header into CSV files: "
#define lng088    "write."
#define lng089    "don't write."
#define lng090    "#O,1 ... decimal divider: comma"
#define lng091    "#O,0 ... decimal divider: dot"
#define lng092    "#O\"1 ... don't use quotes along values"
#define lng093    "#O\"0 ... use quotes along values"
#define lng094    "#Oh1 ... write headers"
#define lng095    "#Oh0 ... don't write headers"
#define lng096    "#Oo_ ... divider of items: space"
#define lng097    "#Oo, ... divider of items: comma"
#define lng098    "#Oo; ... divider of items: semicolon"
#define lng099    "#Oot ... divider of items: tabulator"
#define lng100    "warning: divider of values and decimal separator - both are comma!"
#define lng102    "(merena hodnota max 24 mag/arcsec2 = 24000)"
#define lng103    "(skutecna hodnota max 24 mag/arcsec2 = 24000)"
#define lng104    "zadane kalibracni hodnoty pro plosny jas:"
#define lng105    "pozice - merena hodnota - (mag/arcsec2) - spravna hodnota - (mag/arcsec2)"
#define lng106    "(position 1 to 7)"
#define lng107    "(merena hodnota max. 50'C = 10000)"
#define lng108    "(zobrazena hodnota max. 50'C = 10000)"
#define lng109    "zadane kalibracni hodnoty pro teplotu:"
#define lng110    "pozice - merena hodnota ('C) - zobrazena hodnota ('C)"
#define lng111    "Casova zona: "
#define lng112    "\r\nDividers:"
#define lng113    " Polozky v souboru: "
#define lng114    " Decimal divider: "
#define lng115    " use quotes along values"
#define lng116    " don't use quotes along values"
#define lng117    " Insert header to every file"
#define lng118    " Don't insert header to every file"
#define lng119    "\r\nAutomat:"
#define lng120    " NO automatic trigger"
#define lng121    "  Runs every "
#define lng122    " minutes."
#define lng123    "\r\nAverage "
#define lng124    " Light measurement"
#define lng125    "Checksum byte in RS485 communication is not required."
#define lng126    "Checksum byte in RS485 communication is required."
#define lng127    "Rozhodovaci rozdil pro stabilni/nestabilni jas je "
#define lng128    "Trigger level for saving records to EEPROM - Surface darkness than "
#define lng129    "Unicate identification: "
#define lng130    "\r\nHardware:"
#define lng131    " Processor: "
#define lng132    " Light sensor: TSL2591"
#define lng032    "  "        // rezerva
#define lng033    "  "        // rezerva
#define lng034    "  "        // rezerva
#define lng035    "  "        // rezerva
#define lng133    "  "        // rezerva
#define lng134    "  "        // rezerva
#define lng211    "  "        // rezerva
#define lng231    "  "        // rezerva
#define lng235    "  "        // rezerva
#define lng247    "  "        // rezerva
#define lng075    "  "        // rezerva
#define lng135    " Sensor of humidity, temperature and pressure: BME280"
#define lng136    " SD card: "
#define lng137    "inserted"
#define lng138    "removed"
#define lng139    "disabled"
#define lng140    " Display: 5x7 segments (TM1637)"
#define lng141    " Just only signaliztion LED"
#define lng142    " Number of inputs of expansion sensor board: "
#define lng143    " Index number of light sensor:    "
#define lng144    "\r\nEEPROM:"
#define lng145    " Begin of records area: "
#define lng146    " End of records area:   "
#define lng147    " Next record will write to EEPROM address: "
#define lng148    " Lenght of one record in EEPROM is: "
#define lng149    " bytes"
#define lng150    " Contains these parts: "
#define lng151    "  - Surface brightness        YES"
#define lng152    "  - temperature               "
#define lng155    "  - pressure                  "
#define lng156    "  - humidity                  "
#define lng157    "  - infra part of light       "
#define lng158    "  - full part of light        "
#define lng159    "  - ctrl reg. of TSL2591      "
#define lng206    "  - GPS coordinates           "
#define lng228    "  - elevation angle           "
#define lng264    "  - azimuth                   "
#define lng236    "  - Sun and Moon elevat.      "
#define lng305    "  - reserve 1                 "    // reserve for other sensors (labels to "sys_info()")
#define lng306    "  - reserve 2                 "
#define lng307    "  - reserve 3                 "
#define lng308    "  - reserve 4                 "
#define lng153    "YES"
#define lng154    "NO"
#define lng160    "\r\nCallibration of temperature sensor"
#define lng161    "index ; measured  ; correct"
#define lng162    "\r\nCallibration of light sensor ["
#define lng163    "index ; measured ; correct"
#define lng164    "SOFT format of EEPROM ..."
#define lng165    "HARD format of EEPROM ..."
#define lng166    "EEPROM format finished."
#define lng167    "Selected record is OUT OF RANGE (1 - "
#define lng168    "NO record"
#define lng169    "CRC byte does not match:"
#define lng170    "   expected CRC: "
#define lng171    "   recieved CRC: "
#define lng172    "This function is not available for all devices (addr 127)"
#define lng173    "Universal request sent."
#define lng174    "Information about RTC"
#define lng175    "Time is "
#define lng176    "Time in internal RTC (with no corrections) (UTC): "
#define lng177    "Last setup of RTC (UTC): "
#define lng178    " ... in seonds from 1.1.1970: "
#define lng179    " ... in readable form: "
#define lng180    "Interval since last time setup: "
#define lng181    "Correction of time : "
#define lng182    " ... Number in EEPROM in HEX form: "
#define lng183    " ... one second is "
#define lng184    "substracted"
#define lng185    "added"
#define lng186    " every "
#define lng187    " seconds from last time set."
#define lng188    "           (it is about "
#define lng189    " hours)"
#define lng190    " days)"
#define lng191    " years)"
#define lng192    "Since last setup should be add / substract: "
#define lng193    " extra seconds."
#define lng194    "So corrected time for showing is: "
#define lng195    "Recieved: "
#define lng197    "Year: "
#define lng198    "Month: "
#define lng199    "Day: "
#define lng200    "Hour: "
#define lng201    "Minute: "
#define lng202    "Second: "
#define lng203    "Difference between time in RTC and current time [s]: "
#define lng204    "RTC ERROR (correction of seconds is needed more often than 5 minutes)."
#define lng205    "Automatic correction of RTC disabled."
#define lng207    "All records are written into EEPROM"
#define lng208    "@G           ... details of GPS module"
#define lng209    "@S n         ... serial baudrate (0=9600; 1=19200; 2=38400; 3=115200)"
#define lng210    "calibration point deleted"
#define lng212    "#E              ... print of observer sites"
#define lng213    "#ETn text       ... edit text of obs. sites"
#define lng214    "#EGn g,g,g,g,g  ... edit graph. of obs. sites"
#define lng215    "Write to 'RTC_set.csv' - ENABLE"
#define lng216    "Write to 'RTC_set.csv' - DISABLE"
#define lng217    "@> F,n,par   ... dump files from SD card to serial line"
#define lng218    "@h (H)       ... DISABLE (ENABLE) write to RTC_set.csv file"
#define lng219    "Big difference between time in RTC and current time"
#define lng220    "Short interval since last time setting (< 1 day)"
#define lng221    "Long interval since last time setting (> half year)"
#define lng222    "@P           ... enable/disable beeper"
#define lng223    "Beeper enabled"
#define lng224    "Beeper muted"
#define lng225    "rh nnnnn ... dump records within last 'nnnnn' hours"
#define lng226    "s        ... elevation (spirit level)"
#define lng227    "#Lc          ... level calibration"
#define lng229    " Elevation: "
#define lng230    "#Li          ... level info"
#define lng232    "Put SQM horizontally"
#define lng233    "\r\n\r\nPut SQM vertically"
#define lng234    "\r\n\r\nLevel calibration error - setting-up defaults"
#define lng237    "Coordinates for astronomical computings (longitude / latitude): "
#define lng238    "Latitude: "
#define lng239    "Longitude: "
#define lng240    "@Ga nnn      ... home latitude for astro computings"
#define lng241    "@Go nnn      ... home longitude for astro computings"
#define lng242    "@Gw nn       ... home timezone for winter"
#define lng243    "@Gs nn       ... home timezone for summer"
#define lng244    "UTC difference winter/summer: "
#define lng245    "x        ... measure illuminance (lux)    - NO save"
#define lng246    "@Gd ABCDEFHG ... text description for winter and summer timezone"
#define lng248    "#Xa n.nn     ... calibration of luxmeter - param. 'a'"
#define lng249    "#Xb n.nn     ... calibration of luxmeter - param. 'b'"
#define lng263    "#Xc n.nn     ... calibration of luxmeter - param. 'c'"
#define lng265    " Compass / inclinometer LSM303DLHC"
#define lng266    "#Mk          ... calibration of magnetometer (compass)"
#define lng267    "Compass callibration ERROR"
#define lng268    "d        ... direction (azimuth)"
#define lng269    "Azimuth: "
#define lng270    "@B           ... Backup params to SD"
#define lng271    "@O           ... Restore params form SD"
#define lng272    "@T           ... HW test menu"
#define lng273    "Last measured source voltage (5V-30V): "
#define lng274    "RTC Backup battery (3V):               "
#define lng275    "Reference (1V8):                       "
#define lng276    "Voltage of processor (3V3):            "
#define lng277    "#TG              ... set time by GPS"
#define lng278    "GPS time is not available"
#define lng279    "MODBUS ON"
#define lng280    "MODBUS OFF"
#define lng281    "@m   (M)     ... modbus OFF (ON)"
#define lng339    "@L           ... OFF/ ON RGB LED"   
#define lng282    "     @Lb (B) ... OFF (ON) LED for battery test"
#define lng283    "     @Lm (M) ... OFF (ON) LED during measuring"
#define lng284    "     @Le (E) ... OFF (ON) LED for errors"
#define lng285    "     @Lc (C) ... OFF (ON) LED for communication"
#define lng301    "     @La (A) ... OFF (on) LED Alarm for Darkness"
#define lng354    "     @Lt (T) ... vypnout (zapnout) LED pri GPS trasovani"
#define lng290    " RGB LED functions"
#define lng286    "   LED battery [B]:           "
#define lng287    "   LED measuring [M]:         "
#define lng288    "   LED error [E]:             "
#define lng289    "   LED communication [C]:     "
#define lng300    "   LED for AfD function [A]:  "
#define lng355    "   LED for GPS tracking [T]:  "
#define lng291    "I2C bus problem"
#define lng292    "SD card problem"
#define lng293    "SD card not inserted"
#define lng294    "RTC problem (backup battery)"
#define lng295    "Inclinometer problem"
#define lng296    " SQM display version"
#define lng297    " SQM LED version"
#define lng298    "@Vd          ... SQM display version"
#define lng299    "@Vl          ... SQM LED version"
#define lng302    "#D ff.f      ... level of AfD (13.0 - 25.4) other = OFF"
#define lng303    "AfD level: "
#define lng304    "AfD OFF"
#define lng309    "rd nnnnn ... dump one selected record in integer data format"
#define lng310    "b n hh:mm ... set one alarm"
#define lng311    "alarm no. "
#define lng312    " OFF"
#define lng313    "Turn the power OFF (disconnect USB cable and slide power switch to OFF). In 30 seconds the alarm should be active"
#define lng314    "Time in DS3231: "
#define lng315    "Registers of DS3231:"
#define lng316    "#Mo ff.f     ... set offset of compass (-12.7 ... +12.7 deg)"
#define lng317    "compass 'rotated' [deg]: "
#define lng318    "countdown  "
#define lng319    "o -nnn ... set countdown alarm [nnn=minutes]"
#define lng320    "\nFile open error / file doesn't exist"
#define lng321    "Backup file version mismatch - restoring impossible"
#define lng222    "@P           ... enable/disable beeper"
#define lng322    "     @Pb (B) ... disable (enable) beeper for battery level"
#define lng323    "     @Pm (M) ... disable (enable) beeper for meaasurement"
#define lng324    "     @Pa (A) ... disable (enable) beeper for alarms and countdown"
#define lng325    "     @Pe (E) ... disable (enable) beeper for HW errors"
#define lng326    "     @Pt (T) ... disable (enable) beeper for TimeStamp button"
#define lng327    "     @Po (O) ... disable (enable) beeper for approaching to Ra/Dec coordinates"
#define lng328    "     @Pw (W) ... disable (enable) beeper for other warnings"
#define lng329    "     @Ps (S) ... disable (enable) beeper for stopwatch functions"
#define lng352    "     @Pp (P) ... disable (enable) beeper for power ON"
#define lng330    "Beeper signals"
#define lng331    "   battery [B]:               "
#define lng332    "   measurement [M]:           "
#define lng333    "   alarms and countdown [A]:  "
#define lng334    "   HW problems [E]:           "
#define lng335    "   TS button operations [T]:  "
#define lng336    "   approaching to Ra-Dec [O]: "
#define lng337    "   next warnings [W]:         "
#define lng338    "   stopwatch [S]:             "
#define lng353    "   power ON [P]:              "
#define lng340    "Time set "
#define lng341    "#Mi          ... Info about magnetometer"
#define lng342    "     (lines: "
#define lng343    "*m           ... run measuing without serial output"
#define lng344    "*n           ... request for the index of the last record"
#define lng345    "*a           ... test for valid SLAVE addres"
#define lng346    "*f           ... dump of all serial functions characters"
#define lng347    "*r nnn       ... download selected record in the INT data format"
#define lng348    "@N nn        ... set elevation of Sun for user defined dusk/dawn"
#define lng349    "User defined dusk/dawn is when Sun is "       // whole sentence: "User defined dusk/dawn is when Sun is _______ degrees under horizon."
#define lng350    " degrees under horizon."
#define lng351    "*s p c d.m.y ... simulation of solar system"
#define lng356    "Tracking to file 'trck"
#define lng357    ".gpx' is ON."
#define lng358    "Tracking to GPX file is off."
#define lng383    "Count of GPS points in the GPX file: "
#define lng360    "GPS points:     "              //  Exactly 16 characters!
#define lng361    "averages "                    //  waypoint name in GPX file. Exactly 9 characters!
#define lng362    "@A l,i,f,o   ... automatic sendings records to any serial line (USB or 485) without request"
#define lng363    "   Line:    "
#define lng364    " USB"
#define lng365    " RS485"
#define lng366    "Automatic sendings records via RS485 switched OFF modbus"
#define lng367    " "
#define lng368    " "
#define lng369    "   Format:   "
#define lng370    " data"
#define lng371    " text"
#define lng372    "   Objects:    "
#define lng373    " date + bright "
#define lng374    "+ temperature + humidity + pressure "
#define lng375    "+ bright details "
#define lng376    "+ elevation + azimuth "
#define lng377    "+ Sun and Moon position "
#define lng378    "all data"
#define lng379    "Automatic sending records "
#define lng380    "switch OFF"
#define lng381    "Not available records for send"
#define lng382    "By turning on Modbus, automatic sending of records via RS485 was disabled."
#define lng384    "[V] (on USB now)"
#define lng385    "*e aaa       ... read 1 byte EEPROM"
// continue:  #define lng386




// text labels (menu) for test functions
#define lng_test_001     "a .... I2C scan"
#define lng_test_002     "b .... test of A/D converter"
#define lng_test_003     "c .... test of EEPROM 128k"
#define lng_test_004     "d .... test of extra EEPROM (r30s)"
#define lng_test_005     "e .... test of beeper"
#define lng_test_006     "f .... test of BME280"
#define lng_test_007     "g .... test of TSL2591"
#define lng_test_008     "h .... test of displej"
#define lng_test_009     "i .... test EXT contact and inserted SD card"
#define lng_test_010     "j .... test of buttons"
#define lng_test_011     "k .... RGB LED test"
#define lng_test_012     "l .... test of SD card"
#define lng_test_013     "m .... test of RS485"
#define lng_test_014     "n .... GPS test"
#define lng_test_015     "o .... test of LSM303 (compass and inclinometer)"
#define lng_test_016     "p .... GPS serial data listening"
#define lng_test_017     "r .... test of main RTC in STM32F4x1"
#define lng_test_018     "s .... random time set of RTC in STM32F4x1"
#define lng_test_019     "t .... external output to LOW"
#define lng_test_020     "T .... external output to HIGH"
#define lng_test_021     "u .... test of RTC DS3231"
#define lng_test_022     "U .... test of wake up in 30s"
#define lng_test_023     "x .... return"
#define lng_test_024     "Test of display"
#define lng_test_025     "End of display test"
#define lng_test_026     "Real voltage on Ref pin:  "
#define lng_test_027     "Real voltage on Ubat pin: "
#define lng_test_028     "Test of buttons."
#define lng_test_029     " Press and hold any button ... "
#define lng_test_030     "\r\nIn 10 seconds any button did not preesed - END OF TEST."
#define lng_test_031     "[UP]    "
#define lng_test_032     "[DOWN]  "
#define lng_test_033     "[OK]    "
#define lng_test_034     "[TS]    "
#define lng_test_035     "   Bounces = "
#define lng_test_036     "   Duration = "
#define lng_test_037     "  Release pressed button ... "
#define lng_test_038     "\r\nIn 10 seconds any button did not released - END OF TEST."
#define lng_test_041     "Sken of I2C\r\n|"            // posledni znak za odradkovanim musi byt svislitko '|' 
#define lng_test_042     "value on address [00000] = "
#define lng_test_043     "value on address [32768] = "
#define lng_test_044     "Capacity 64kB "
#define lng_test_045     "Capacity 32kB "
#define lng_test_046     "EEPROM error"
#define lng_test_047     "Test of beeper"
#define lng_test_048     "number of seconds in the main RTC: "
#define lng_test_049     "tt after saving: "
#define lng_test_050     "RTC set to: 21.10.2023 11:12:13 Su"
#define lng_test_051     "GPS not present (data missing)."
#define lng_test_052     "Connect by wire pin Tx on PCB SQM-BAS7 and pin GPS-OUT on PCB SQM-GPS6"
#define lng_test_053     "For end of test disconnect the wire and wait 5 seconds"
#define lng_test_054     "listenning of GPS ended"
#define lng_test_055     "I2C error... code: "
#define lng_test_056     "Temperature: "
#define lng_test_057     "      Humidity: "
#define lng_test_058     "      Pressure: "
#define lng_test_059     "SD card is not inserted - End of test"
#define lng_test_060     "testing file soubor: "
#define lng_test_061     "Random number: "
#define lng_test_062     "Test of writting some random number: "
#define lng_test_063     "end of writting"
#define lng_test_064     "Dump file: "
#define lng_test_065     "function 'soubor.open()' for reading failures"
#define lng_test_066     "function 'soubor.open()' for writting failures"
#define lng_test_067     "Side 6-pin connector  (pin Data_Ready): "
#define lng_test_068     "status: HIGH"
#define lng_test_069     "status: LOW"
#define lng_test_070     "SD holder insert contact: "
#define lng_test_073     "Test of RGB LED"
#define lng_test_074     "Green blinking..."
#define lng_test_075     "Red blinking..."
#define lng_test_076     "Blue blinking..."
#define lng_test_077     "15 seconds test of compass and inclnometer LSM303"
#define lng_test_078     "remain: "
#define lng_test_079     "          acceleration: aX= "
#define lng_test_080     "    aY= "
#define lng_test_081     "    aZ= "
#define lng_test_083     "          magnetometer: mX= "
#define lng_test_084     "    mY= "
#define lng_test_085     "    mZ= "
#define lng_test_086     "Send few characters from PC through RS485 and watch response."
#define lng_test_087     "  (any character on USB line will test end)"
#define lng_test_088     "RESPONSE: "
#define lng_test_089     "recieved / forwarding back: "
#define lng_test_090     "External OUTPUT on 6-pin connector set to: "
#define lng_test_091     "Found address: "
#define lng_test_092     "Voltage on processor:         "
#define lng_test_093     "Voltage on 3V coin cell:         "
#define lng_test_094     "Time is not correct (3V coin cell failures)"
#define lng_test_095     "Time seems to be OK (3V coin cell is OK)"
#define lng_test_096     "- .... Automenu OFF/ON"

#define lng30_001    "Status: "
#define lng30_002    "Interval: "
#define lng30_003    "End of dump"
#define lng30_004    "Stoped"
#define lng30_005    "Running"
#define lng30_006    "Starting point for search free cell in EEPROM: "
#define lng30_007    "Address for next record: "
#define lng30_008    "Extra EEPROM size: "
#define lng30_009    "Stoped"
#define lng30_010    "Running"
#define lng30_011    "previous interval: "
#define lng30_012    "new interval: "
#define lng30_013    "formating error"
#define lng30_014    "Next record to address 10."
#define lng30_015    "Copying records to SD card ... "
#define lng30_016    "SAVED"
#define lng30_017    "'file.open()' error"
#define lng30_018    "%I     ... detail information"
#define lng30_019    "%L     ... list of records durning last 24 hours to USB serial line"
#define lng30_020    "%Lh n  ... list of records durning last 'n' hours to USB serial line"
#define lng30_021    "%La    ... list of all records to USB serial line"
#define lng30_022    "%Lz n  ... list of last 'n' records to USB serial line"
#define lng30_023    "%S     ... save to SD  card"
#define lng30_024    "%Lb    ... bytes dump"
#define lng30_025    "%P     ... pause measurement"
#define lng30_026    "%R     ... continue measurement"
#define lng30_027    "%C n   ... set of measuring interval (10-255)"
#define lng30_028    "%F     ... formating extra EEPROM"
#define lng30_029    "%N     ... Next record to 'zero' position in EEPROM"
#define lng30_030    "%?     ... this help"
#define lng30_031    "%W a,d ... write data to EEPROM2"
#define lng30_032    "%X a   ... read data from EEPROM2"


//                           0000000000111111111122222222223333333333444444444455555555556666666666777777777788888888889999999999
//                           0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789
char hlavicka30[]         = "index ;  sec 1970  ;   date   ;  time  ;  WD ; temper.[C];  humid.[%]; pres[Pa] ;    light [lx]  ;\0";
//        example           "00044 ; 1631464959 ;12.09.2021;16:42:39;  Su ;   +26,48  ;   056,21  ;  095953  ;    78123,12    ;\0";

char hlavicka_E0[]  =       " addr. ; typ ;inter;block;PAUSE;  capacity ; inx ;     RESERVE     ;\0";
//        example           "     0 ; 100 ;  30 ;   0 ;   0 ; 255 ; 220 ; 001 ; 255 ; 255 ; 255 ;"  

char hlavicka_E10[] =       " addr. ;        sec 1970       ; temperat. ;  humidity ; pressure  ;       light     ;\0";
//        example           " 32500 ;  97 ;  62 ;  40 ; 154 ;  29 ; 213 ;  22 ;  35 ; 140 ; 128 ; 132 ; 123 ; 231 ;"  



// popisky do CSV zaznamu - NUTNO DODRZET PRESNY POCET ZNAKU (dlouhe texty zkratit, kratke doplnit mezerami)
// must to have exactly width (count of characters)
#define lng254    " STABLE "            // mark for stabile brightness                     (8 characters)
#define lng255    " UNSTAB "            // mark for unstabile brightness                   (8 characters)
#define lng256    "bt_UP"               // mark for start of measure by button "Up"        (5 characters)
#define lng257    "bt_DN"               // mark for start of measure by button "Down"      (5 characters)
#define lng258    "bt_OK"               // mark for start of measure by button "OK"        (5 characters)
#define lng259    "comm."               // mark for start of measure by communication command (RS485 or USB)         (5 characters)
#define lng260    " auto"               // mark for start of measure by automatic timer                              (5 characters)
#define lng261    "calib"               // mark for record without calibration corrections of light (raw data from sensor)     (5 characters)
#define lng262    "Ca-Av"               // mark for average last of 10 records without calibration correction of light         (5 characters)


// codes and characters for commands via USB (serial line)
//  ordinary functions
//           in every column must be unicate character
#define USB_fce_1_01 'v'             // ... show humidity                - no save
#define USB_fce_1_02 't'             // ... show temterature             - no save
#define USB_fce_1_03 'p'             // ... show pressure                - no save
#define USB_fce_1_04 'l'             // ... show light (mag/arcsec2)     - no save
#define USB_fce_1_05 'z'             // ... switch to winter CET ("z0") or summer CEST ("z1") timezone
#define USB_fce_1_06 'a'             // ... timer for automatic measure 0-255 minutes
#define USB_fce_1_07 'e'             // ... switch light sensor on extender board (1-7)
#define USB_fce_1_08 'r'             // ... show records from EEPROM (only 'r' shows 100 records)
#define USB_fce_1_09     'p'         // ...          "rp" ... show last record
#define USB_fce_1_10     'a'         // ...          "ra" ... show all records
#define USB_fce_1_11     'z'         // ...          "rz" ... show selected record
#define USB_fce_1_19     'h'         // ...          "rh" ... show records since last period (for exam.: last 24 hours)
#define USB_fce_1_23     'x'         // ...          "rx" ... last record for Unihedron software (experimental)
#define USB_fce_1_26     'd'         // ...          "rd" ... show selected record in data (not text) format
#define USB_fce_1_12 'c'             // ... curent date and time
#define USB_fce_1_13 'i'             // ... details system information
#define USB_fce_1_25     'x'         // ...          "ix" information about SQM device for Unihedron software (experimental)
#define USB_fce_1_14 'h'             // ... show only header without records
#define USB_fce_1_15 'm'             // ... run one measure with record into EEPROM
#define USB_fce_1_16 'k'             // ... calibration
#define USB_fce_1_17         'j'     // ...     calibration of light ("kj")
#define USB_fce_1_18         't'     // ...     calibration of temperature ("kt")
#define USB_fce_1_20 's'             // ... spirit level
#define USB_fce_1_21 'x'             // ... illuminance (lux)
#define USB_fce_1_22 'd'             // ... direction (azimuth)
#define USB_fce_1_24 'R'             // ... "R" ... last record for Unihedron software (experimental)
#define USB_fce_1_27 'b'             // ... set one of the 5 alarms "b 2 15:48"
#define USB_fce_1_28 'o'             // ... countdown


//  service functions (first character '#')
//           in every column must be unicate character
#define USB_fce_2_01 'T'                             // ... #T = setup of date and time
#define USB_fce_2_33     'G'                         // ...       #TG = set date and time by GPS
#define USB_fce_2_02 'P'                             // ... #P = setup of count of measures for averaging
#define USB_fce_2_03 'S'                             // ... #S = setup of level of stability mark (0-255)
#define USB_fce_2_04 'I'                             // ... #I = detail information about internal timer (RTC)
#define USB_fce_2_05 'A'                             // ... #A = setup of slave address for RS485 communication (1-15)
#define USB_fce_2_06 'C'                             // ... #C switch on / off for control byte (RS485 communication)
#define USB_fce_2_07 'O'                             // ... #O setup of separators
#define USB_fce_2_08     'O'                         // ...       #OO = next character determines separator of values   ("#OO;" = semicolon  "#OO," = comma  "#OOt" = tabulator    "#OO_" = space)
#define USB_fce_2_09     ','                         // ...       #O, = decimal point / comma separator    (#O,0 = coma off, point on        #O,1 = comma on, point off)
#define USB_fce_2_10     '"'                         // ...       #O" = quote separator of values  (#O"1 = on                   #O"0 = off)
#define USB_fce_2_11     'h'                         // ...       #Oh = write header into CSV files (#Oh0 = don't write          #Oh1 = write)
#define USB_fce_2_12 'F'                             // ... #F format of EEPROM 
#define USB_fce_2_13         'S'                     // ...       #FS = SOFT format
#define USB_fce_2_14         'H'                     // ...       #FH = HARD format
#define USB_fce_2_15 'U'                             // ... #U = level for light for save into EEPROM
#define USB_fce_2_16 'K'                             // ... #K write calibration point to calibration table in EEPROM
#define USB_fce_2_17             'J'                 // ...       calibration point for light  (#KJ)
#define USB_fce_2_18             'T'                 // ...       calibration point for temperature (#KT)
#define USB_fce_2_19 'E'                             // ... #E edit of observer sites
#define USB_fce_2_20                 'T'             // ...      #ET ... texts
#define USB_fce_2_21                 'G'             // ...      #EG ... grapgics symbols for 7-segments display
#define USB_fce_2_22 'L'                             // ... #L level angle
#define USB_fce_2_24                    'i'          // ...      level info (#Li)
#define USB_fce_2_23                    'c'          // ...      level calibration (#Lc)
#define USB_fce_2_27 'X'                             // ... #X calibration of luxmeter
#define USB_fce_2_28                        'a'      // ... #Xa     parameter 'a'
#define USB_fce_2_29                        'b'      // ... #Xb     parameter 'b'
#define USB_fce_2_30                        'c'      // ... #Xc     parameter 'c'
#define USB_fce_2_31 'M'                             // ... #M    Magnetometer (compass)
#define USB_fce_2_32                            'k'  // ... #Mk     calibration of magnetometer
#define USB_fce_2_35                            'o'  // ... #Mo     offset of compass (fine setup of NORTH)
#define USB_fce_2_36                            'i'  // ... #Mi     information about magnetometer
#define USB_fce_2_34 'D'                             // ... #D    set level of AfD
// continue at index 2_37:  #define USB_fce_2_37


//  "secret" functions (first character '@')
//           in every column must be unicate character
#define USB_fce_3_01 'E'                      // ... @E = test postupneho prepinani cidel na expanzni desce
#define USB_fce_3_02 'X'                      // ... @X = nastaveni poctu vstupu na expanzni desce (1-7)
#define USB_fce_3_03 'R'                      // ... @R = direct read from EEPROM 
#define USB_fce_3_04 'W'                      // ... @W = direct write to EEPROM 
#define USB_fce_3_05 'D'                      // ... @D restore defaults (following 1 chracter for specify)
#define USB_fce_3_06     'S'                  //          @DS = restore calibrations for light
#define USB_fce_3_07     'T'                  //          @DT = restore calibrations for temperature
#define USB_fce_3_08     'P'                  //          @DT = restore system parameters
#define USB_fce_3_10 '@'                      // ... @@ = reset
#define USB_fce_3_11 'G'                      // ... @G = detailed GPS info
#define USB_fce_3_19          'a'             // ... @Ga = setup for home coordinates (latitude) for astronomical computings
#define USB_fce_3_20          'o'             // ... @Go = setup for home coordinates (longitude) for astronomical computings
#define USB_fce_3_21          'w'             // ... @Gw = home timezone for winter
#define USB_fce_3_22          's'             // ... @Gs = home timezone for summer
#define USB_fce_3_23          'd'             // ... @Gd = text desctiption for winter and summer timzone ("@Gd CET CEST")
#define USB_fce_3_12 'S'                      // ... @S = serial baudrate (USB / RS485)
#define USB_fce_3_15 '>'                      // ... @> = dump of "RTC_set.csv" to USB serial bus
#define USB_fce_3_16 'h'                      // ... @h = Switch OFF for write data into file  "RTC_set.csv"
#define USB_fce_3_63 'H'                      // ... @H = Switch ON  for write data into file  "RTC_set.csv"
#define USB_fce_3_18 '*'                      // ... @* nnn = dump of single record from EEPROM in HEX format 
#define USB_fce_3_24 'B'                      // ... @B Backup system parameters to SD card
#define USB_fce_3_25 'O'                      // ... @O Restore system parameters from SD card
#define USB_fce_3_26 'T'                      // ... @T Funkce pro otestovani HW
#define USB_fce_3_27 'M'                      // ... @M switch modbus ON
#define USB_fce_3_28 'm'                      // ... @m switch modbus OFF
#define USB_fce_3_33 'V'                      // ... @V version of SQM 
#define USB_fce_3_34              'd'         // ...       @Vd display version
#define USB_fce_3_35              'l'         // ...       @Vl LED version
#define USB_fce_3_36 'L'                      // ... @L ON/OFF RGB LED 
#define USB_fce_3_37                  'b'     // ...       @Lb diasble RGB LED (battery test)
#define USB_fce_3_38                  'B'     // ...       @LB enable  RGB LED (battery test)
#define USB_fce_3_39                  'm'     // ...       @Lm diasble RGB LED (blinking during measuring)
#define USB_fce_3_40                  'M'     // ...       @LM enable  RGB LED (blinking during measuring)
#define USB_fce_3_41                  'e'     // ...       @Le diasble RGB LED (error blinking)
#define USB_fce_3_42                  'E'     // ...       @LE enable  RGB LED (error blinking)
#define USB_fce_3_43                  'c'     // ...       @Lk diasble RGB LED (RS485 communication blinking)
#define USB_fce_3_44                  'C'     // ...       @LK enable  RGB LED (RS485 communication blinking)
#define USB_fce_3_45                  'a'     // ...       @La diasble RGB LED (Alarm for Darkness)
#define USB_fce_3_46                  'A'     // ...       @LA enable  RGB LED (Alarm for Darkness)
#define USB_fce_3_67                  't'     // ...       @Lt diasble RGB LED (GPS tracking)
#define USB_fce_3_68                  'T'     // ...       @LT enable  RGB LED (GPS tracking)
#define USB_fce_3_17 'P'                      // ... @P = enable/disable beeper
#define USB_fce_3_47                      'b' // ...       @Pb beeper OFF - battery level
#define USB_fce_3_48                      'B' // ...       @PB beeper ON  - battery level
#define USB_fce_3_49                      'm' // ...       @Pb beeper OFF - measurement
#define USB_fce_3_50                      'M' // ...       @PB beeper ON  - measurement
#define USB_fce_3_51                      'a' // ...       @Pa beeper OFF - alarm
#define USB_fce_3_52                      'A' // ...       @PA beeper ON  - alarm
#define USB_fce_3_53                      'e' // ...       @Pe beeper OFF - HW err
#define USB_fce_3_54                      'E' // ...       @PE beeper ON  - HW err
#define USB_fce_3_55                      't' // ...       @Pt beeper OFF - TimeStamp button
#define USB_fce_3_56                      'T' // ...       @PT beeper ON  - TimeStamp button
#define USB_fce_3_57                      'o' // ...       @Po beeper OFF - approaching in RA-DEC function
#define USB_fce_3_58                      'O' // ...       @PO beeper ON  - approaching in RA-DEC function
#define USB_fce_3_59                      'w' // ...       @Pv beeper OFF - other warnings
#define USB_fce_3_60                      'W' // ...       @PV beeper ON  - other warnings
#define USB_fce_3_61                      's' // ...       @Ps beeper OFF - stopwatch
#define USB_fce_3_62                      'S' // ...       @PS beeper ON  - stopwatch
#define USB_fce_3_65                      'p' // ...       @Pp beeper OFF - power ON
#define USB_fce_3_66                      'P' // ...       @PP beeper ON  - power ON
#define USB_fce_3_64 'N'                      // ... @N nn = set elevation of Sun for user defined dusk/dawn"
#define USB_fce_3_69 'A'                      // ... @A l,i,f,o = automatic sendings records to any serial line (USB or RS485)
// continue by index 3_70 #define USB_fce_3_70


#define USB_fce_4_01 'I'                      // ... %I     ... detail information
#define USB_fce_4_02 'L'                      // ... %L     ... dump (list) of last 24 hours to serial line
#define USB_fce_4_03      'h'                 // ... %Lh n  ... dump (list) of last 'n' hours to serial line
#define USB_fce_4_04      'a'                 // ... %La    ... dump (list) of all records to serial line
#define USB_fce_4_05      'z'                 // ... %Lz n  ... dump (list) of last 'n' records to serial line
#define USB_fce_4_07      'b'                 // ... %Lb    ... dump (list) of all records to serial line in bytes format
#define USB_fce_4_06 'S'                      // ... %S     ... save to SD card
#define USB_fce_4_08 'P'                      // ... %P     ... pause recording
#define USB_fce_4_09 'R'                      // ... %R     ... run / continue recording
#define USB_fce_4_10 'C'                      // ... %C n   ... setup of recording interval (10-255)
#define USB_fce_4_11 'F'                      // ... %F     ... formating attached EEPROM
#define USB_fce_4_12 'N'                      // ... %N     ... reset next record to zero position in EEPROM
#define USB_fce_4_13 'W'                      // ... %W a,d ... Write data to EEPROM 2
#define USB_fce_4_14 'X'                      // ... %X     ... Read data from EEPROM 2
                         

String head_byte[] =                   // for print bytes header
    {
      "1x status 1",                   // [0]
      "4x sec1970" ,                   // [1]
      "2x bright" ,                    // [2]
      "1x status 2 " ,                 // [3]
      "2x infra" ,                     // [4]
      "2x full" ,                      // [5]
      "1x config" ,                    // [6]
      "2x tempreature" ,               // [7]
      "2x humidity" ,                  // [8]
      "2x pressure" ,                  // [9]
      "4x GEO lon" ,                   // [10]
      "4x GEO lat" ,                   // [11]
      "2x GEO alt" ,                   // [12]
      "2x elevation" ,                 // [13]
      "2x azimuth" ,                   // [14]
      "1x Sun elevat." ,               // [15]
      "1x Moon elevat." ,              // [16]
      "1x Moon illuminat.",            // [17]
      "2x reserve 1" ,                 // [18]
      "2x reserve 2" ,                 // [19]
      "2x reserve 3" ,                 // [20]
      "2x reserve 4" ,                 // [21]
      "1x stable"                      // [22]
    };


String dny[] = {"--", "Mo" , "Tu" , "We" , "Th" , "Fr" , "Sa" , "Su"}; // weekdays: index 1 = monday; ...  index 7 = sunday.  (index 0 not used)
String planety[] = {"------- ", "Mercury " , "Venus   " , "Mars    " , "Jupiter " , "Saturn  " , "Uranus  " , "Neptune ", "Pluto   "}; // for simulation of solar system (function "*s") keep 8 char width


//          A=1
//        ######
//       #      #
//  F=32 #      #  B=2
//       # G=64 #
//        ######
//       #      #
//  E=16 #      #  C=4
//       #      #
//        ######   ## DP=128               examples:
//         D=8     ##                        'A' = A + B + C + F + E + G = 1 + 2 + 4 + 32 + 16 + 64 = 119
//                                           'L' = F + E + D             = 32 + 16 + 8              = 56 
uint8_t dis_popisy[120][5] =            // definition of 5-characters for display
    {
      //---menu--
      {  56,  48, 109, 120,   0} , // "LI5t "                  [0]
      { 119,  28, 120,  92,   0} , // "Auto "                  [1]
      {  91,  92,  84, 121,   0} , // "2onE "                  [2]
      {  57, 119,  56,  48, 124} , // "CALIb"                  [3]      
      {  94, 119, 120, 121,   0} , // "dAtE "                  [4]
      { 120,   4,  55, 121,   0} , // "tiME "                  [5]
      { 120, 121,  55, 115,   0} , // "tEMP "                  [6]
      { 118,  28,  55,   4,  94} , // "HuMid"                  [7]
      { 124,  80,   4, 111, 118} , // "bri9H"                  [8]
      { 118,  92,  64,  55,   4} , // "Ho-Mi"                  [9]
      { 119,  62, 121,  80,   0} , // "AVer "                 [10]
      {  56, 121,  94,   0,   0} , // "LEd  "                 [11]      
      { 109, 120,  92, 115,   0} , // "StoP "                 [12] (StopWatch)
      { 125, 115, 109,   0,   0} , // "6P5  "                 [13] (GPS)
      { 109,   4, 120, 121,   0} , // "5itE "                 [14] Observer Site
      {  73, 109, 115, 121,  57} , // "#SPEC"                 [15] Specialni funkce (zap/vyp)
      {  73,   0,   4,  84, 120} , // "# int"                 [16] Prvni parametr specialnich funkci (interval rezimu 30s)
      {  73, 115, 119,  80,  91} , // "#PAr2"                 [17] Druhy parametr specialnich funkci (nepouzito)
      {  73, 115, 119,  80,  79} , // "#PAr3"                 [18] Treti parametr specialnich funkci (nepouzito)
      {  73, 113,  57, 121,   6} , // "#FCE1"                 [19] Spusteni prvniho podprogramu specialnich funkci (nepouzito)
      {  73, 113,  57, 121,  91} , // "#FCE2"                 [20] Spusteni druheho podprogramu specialnich funkci (nepouzito)
      {  73,  57,  92, 115, 110} , // "#CoPy"                 [21] Prenos dat ze specialni funkce na SD kartu
      {  56, 121,  62, 121,  56} , // "LEVEL"                 [22] Spirit Level
      { 119, 109, 120,  80,  92} , // "A5tro"                 [23] Astro computing for planets, Sun and Moon
      {  57,  92,  55, 115, 119} , // "CoMPA"                 [24] Compass      
      {  80, 119,  64,  94, 121} , // "rA-De"                 [25] Ra / Dec coordinates
      { 119,  56, 119,  80,  55} , // "ALArM"                 [26] Time alarm (zapina se i pri vypnutem hlavnim vypinaci)
      {  62,  92,  56, 120, 109} , // "VoLtS"                 [27] battery voltage measure
      { 119, 113,  94,   0,   0} , // "AFd  "                 [28] Alarm for Darkness
      {  94, 109,  92,   0,   0} , // "d5o  "                 [29] (DSO) visibility of selected Deep Space Object

     
      //---end of menu--
      
      { 121,  80,  80,  64,  56} , // "Err-L"                 [30] (Calibration table ERROR - for light)
      { 121,  80,  80,  64, 120} , // "Err-t"                 [31] (Calibration table ERROR - for temperature)
      {  84,  92,  57,  92,  80} , // "noCor"                 [32] show on display light with no correction
      {   0,  57,  92,  80,  80} , // " Corr"                 [33] show on display light with correction by callibration table
      {  80, 121, 113,   8,  62} , // "rEF_U"                 [34] test of analog in for reference voltage
      {  62,   8, 124, 119, 120} , // "U_bAt"                 [35] test of analog in for battery voltage
      {  64,  56,  92,  88,  64} , // "-Loc-"                 [36] timestamp button locked
      { 109,  94,  64, 121,  80} , // "5d-Er"                 [37] (SD card ERROR)
      { 109, 119,  62, 121,   0} , // "SAVE "                 [38]
      { 124,  28, 109, 109, 110} , // "bu55y"                 [39]
      { 121,  80,  80,  64,   4} , // "Err-i"                 [40] (I2C ERROR)
      { 121,  80,  80,  64, 125} , // "Err-6"                 [41] (GPS ERROR)("6" = "G")
      {  94, 121, 113, 119,   0} , // "dEFA "                 [42] (Reload defaults)
      {  73,  64,  92,  84,   0} , // "#-on "                 [43] specialni funkce zapnuty (# znaci tri pomlcky nad sebou)  (not used in this public program version)
      {  73,  64,  92, 113, 113} , // "#-oFF"                 [44] specialni funkce vypnuty                                  (not used in this public program version)
      {  84,  92,  64, 109,  94} , // "no-5d"                 [45] (SD card is not present)
      {  80, 121, 109, 121, 120} , // "rE5Et"                 [46] Reset
      {  84,  64,  57, 119,  56} , // "n-CAL"                 [47] calibration of inclinometer (level spirit)
      { 128,   0,   0,   0,   0} , // ".    "                 [48] left dot (automat measuring is running)
      {   1,   1,   1,   1,   1} , // "^^^^^"                 [49] Overload
      {  56,  28, 118,  55,   0} , // "LuHM "                 [50] switched to luxmeter
      { 109, 103,  55,   0,   0} , // "5qM  "                 [51] switched to Sky Quality Meter - measure mag/arcsec ("5" = "S")
      {   0, 128,   0,   0,   0} , // " .   "                 [52] second dot from left (serial measuring is running)
      { 119,  56, 119,  80,  55} , // "ALArM"                 [53]
      {  55, 121,  84,  28,   0} , // "MEnu "                 [54] for menu items show / hide
      { 121,  80,  80,  64,  84} , // "Err-n"                 [55] error of level spirit
      {  55,   4,  64, 109, 121} , // "Mi-5E"                 [56] show current "minutes - seconds" on display
      {   0,   0,  57, 121, 120} , // "  CEt"                 [57] winter timezone
      {   0,  57, 121, 109, 120} , // " CESt"                 [58] summer timezone
      {  57,  64,  57, 119,  56} , // "C-CAL"                 [59] calibration of compass
      { 121,  80,  80,  64,  57} , // "Err-C"                 [60] calibration error
      { 121,  80,  80,  64,  80} , // "Err-r"                 [61] (RTC ERROR)
      { 113,   4,  84,  94,   0} , // "Find "                 [62] finding object with Ra/Dec coordinates
      { 121,  80,  80,  64, 113} , // "Err-F"                 [63] formating of EEPROM error
      { 118,  92,  80,   4,   0} , // "Hori "                 [64] Horizontally - for calibration of spirit level
      {  62, 121,  80, 120,   0} , // "UErt "                 [65] Vertically - for calibration of spirit level
      {   0,   0,   0,   0,   0} , // "     "                 [66] zhasnout displej
      { 118,   0,   0,  64,  64} , // "H  --"                 [67] alarm clock is OFF (24H)
      {  64,  64,  64,  64,  64} , // "-----"                 [68]      
      { 247, 222,  64,  64,  64} , // "A.d.---"               [69] funtion Alarm for Darknes is OFF 
      {  64, 247, 241, 222,  64} , // "-A.F.d.-"              [70] Alarm for Darknes is running
      { 119,  56, 247,  92,  84} , // "ALA.on"                [71] some alarm is in EEPROM
      { 115, 121,  80,   4, 115} , // "PEriP"                 [72] cut-off peripherals
      {   0,   0, 124,  55, 121} , // "  bME"                 [73]     BME280
      {   0,   0,  56, 109,  55} , // "  L5M"                 [74]     LSM303
      {   0,   0,   0, 109,  94} , // "   Sd"                 [75]     SD card  
      {   0,   0,  80, 120,  88} , // "  rtc"                 [76]     DS3231
      {   0,  80,  79,  63, 109} , // " r305"                 [77]     R30s      
      { 247, 241, 222,  92,  84} , // "A.F.d.on"              [78] Alarm for Darknes "on"
      {   4,  84, 113,  80, 119} , // "inFrA"                 [79] infra channel      

      {   0,  55, 121,  80,   0} , // " MEr "                 [80] Mercury
      {   0,  62, 121,  84,   0} , // " VEn "                 [81] Venus
      {   0,  55, 119,  80,   0} , // " MAr "                 [82] Mars
      {   0,  30,  28, 115,   0} , // " JuP "                 [83] Jupiter
      {   0, 109, 119, 120,   0} , // " 2At "                 [84] Saturn
      {   0,  28,  80, 119,   0} , // " urA "                 [85] Uran
      {   0,  84, 121, 115,   0} , // " nEP "                 [86] Neptun
      {   0, 115,  56,  28,   0} , // " PLu "                 [87] Pluto 
      {   0,  55,  92,  92,   0} , // " Moo "                 [88] Moon 
      {   0, 109,  28,  84,   0} , // " Sun "                 [89] Sun 
      {  84,   4, 111, 116, 120} , // "ni9ht"                 [90] Astronomical night
      {  94, 119,  62,  62,  84} ,  // "dAVVn"                [91] user defined dusk (dawn)
      {  88,  92,  28,  84, 120} , // "count"                 [92] countdown
      { 190,   0,  64,  64,  64} , // "U. ---"                [93] battery voltage unknown
      { 208,   0,  64,  64,  64} , // "r. ---"                [94] ref voltage unknown
      {   0,   0,   0,   0,   0} , // "     "                 [95] rezerva
      { 120, 208,  92, 113, 113} , // "tr.oFF"                [96] tracking OFF
      { 120, 208,   0,  92,  84} , // "tr. on"                [97] tracking ON

      {  80, 121, 119,  94, 110} , // "rEAdy"                 [98] pripraveno na mereni smerove charakteristiky cidla
      {   0,  80,  28,  84,   0} , // " run "                 [99] zacatek mereni smerove charakteristiky (najeti na 45 stupnu)

      { 124,  92, 208,  72,   0} , // "bor.= "               [100] Bortle scale (only 4 characters) followed by single number
      {  84, 121, 184,   0,   0} , // "nEL.  "               [101] Naked Eye Limiting Magnitude (NELM) for calibration Bortle Scale (only 3 characters) followed by nuber with one decimal place
      { 124,  64,  57, 119,  56} , // "b-CAL"                [102] User calibration the Bortle Scale
      { 124,  92,  80, 120,  56} , // "bortL"                [103] switch to Bortle in menu "Bright"

      { 115,  28, 109, 118,   0} , // "PuSH"                 [104] For autocalibration device (Push to zero angle)

                                                            // submenu DSO
      {  55, 121, 109, 109,   4} , // "MESSi"                [105]  MESSIER
      {  84, 125,  57,   0,   0} , // "n6C  "                [106]  NGC
      {   4,  57,   0,   0,   0} , // "iC   "                [107]  IC
      { 113,  92,  88,   8, 121} , // "Foc_E"                [108]  focal lenght for eyepiece
      {  92, 124, 109, 121,  80} , // "ob5Er"                [109]  observer abilities
      {  84,  92,  80,  55, 119} , // "norMA"                [110]  normal (reference) sky brightness
      {  94,   4, 119,  55,   0} , // "diAM "                [111]  diameter of telecsope
      { 113,  92,  88,   8, 120} , // "Foc_t"                [112]  focal lenght of telescope

      {  55, 119, 125,  84,   4} , // "MAGni"                [113] Object magnitude
      {  55, 119,  30,  92,  80} , // "MAJor"                [114] Major axe
      {  55,   4,  84,  92,  80} , // "Minor"                [115] Minor axe
      { 120, 110, 115, 121,   0} , // "tyPE "                [116] Object type
      { 109, 118, 119, 115, 121} , // "5HAPE"                [117] Object shape
      { 119,  91,   4,  55,   0} , // "A2iM "                [118] Azimutt
      { 121,  56, 121,  62,   0} , // "ELEV "                [119] Elevation
      {  84,   8, 109, 120, 119} , // "n_5tA"                [120] number of stars in Open Cluster
      { 124,   8, 109, 120, 119} , // "b_5tA "               [121] Brightness star in Open Cluster
      {  62,   4, 109,   4, 124} , // "Vi5ib"                [122] Visibility grade
      
      {  84, 121, 124,  28,  56} , // "nEbuL"                [123] Nebula  (Planetary / emiting / SN remnant)
      {  92, 115,   0,  88,  56} , // "oP cL"                [124] open cluste
      { 125, 119,  56, 119, 118} , // "6ALAH"                [125] galaxy
      { 125,  56,   0,  88,  56} , // "6L cL"                [126] globular cluster
      {  88, 184,  84, 121, 124} , // "cL.nEb"               [127] cluster + nebula
      { 109, 120, 119,  80,   0} , // "5tAr "                [128] (double) star
      { 121,  56, 121,  64,  63}   // "ELE-0"                [129] object below the horizon (visibility = 0)
    };

uint8_t test_popisy[30][5] =                                // definition of 5-characters for display for testing subrutines
    {

      { 120, 121, 109, 120,   0} ,                       // "tESt "                  [0]
      {   0,  56, 121,  94,   0} ,                       // " LEd "                  [1] LED test
      {   0, 125, 115, 109,   0} ,                       // " 6P5 "                  [2] GPS test
      { 119,  84, 125,  56, 121} ,                       // "AnGLE"                  [3] inclinometer test
      { 121, 121, 115,  80,  92} ,                       // "EEPro"                  [4] main EEPROM test
      { 124, 121, 121, 115,   0} ,                       // "bEEP "                  [5] beeper test
      {   0,  30, 119, 109,   0} ,                       // " JA5 "                  [6] TSL test
      {   0, 124,  55, 121,   0} ,                       // " bME "                  [7] BME280 test
      {   0, 119,  94,  57,   0} ,                       // " AdC "                  [8] A/D Converter test
      { 124,  28, 120, 120,   0} ,                       // "butt "                  [9] test of buttons
      {   0, 102, 127, 109,   0} ,                       // " 485 "                 [10] RS485 test
      {   0,   4,  91,  57,   0} ,                       // " i2C "                 [11] scan of I2C
      {   0, 109,  94,   0,   0} ,                       // " 5d  "                 [12] SD card test
      {   0, 121, 118, 120,   0} ,                       // " EXt "                 [13] test of external contact (in 6-pin connector at side of the box)
      { 121, 121,  64,  79,  63} ,                       // "EE-30"                 [14] test of extended EEPROM for "r30s" functions
      {  57,  92,  55, 115, 119} ,                       // "CoMPA"                 [15] compass LSM303 
      {   0,   0,   0,   0,   0} ,                       // "     "                 [16] rezerva
      {   0,   0,   0,   0,   0} ,                       // "     "                 [17] rezerva
      {   0,  80, 120,  88,   0} ,                       // " rtc "                 [18] main RTC test (STM32F4x1)
      {  94,  64,  92,  28, 120} ,                       // "d-out"                 [19] output signal in 6-pin connector at side of the box
      {  80, 120,  88,   0,  91}                         // "rtc 2"                 [20] DS3231 test
    };




// first characters on display for submenus
uint8_t znak_V =       28      ;  // "u"   hUmidity       (CZ = Vlhkost)
uint8_t znak_A =      119      ;  // "A"   Automat 
uint8_t znak_P =       62      ;  // "V"   aVerage        (CZ = Prumer)
uint8_t znak_d =       94      ;  // "d"   Days
uint8_t znak_M =       55      ;  // "M"   Months / minutes
uint8_t znak_r =      110      ;  // "y"   Years          (CZ = Roky)
uint8_t znak_H =      118      ;  // "H"   Hours
uint8_t znak_L =       56      ;  // "L"   List

uint8_t znak_R1 =      80      ;  // "r"   first character of label  rektascenze (rA.)
uint8_t znak_R2 =     119 + 128;  // "A."  second character of label rektascenze (rA.)
uint8_t znak_D1 =      94      ;  // "d"   first character of label  deklinace (dE.)
uint8_t znak_D2 =     121 + 128;  // "E."  second character of label deklinace (dE.)

uint8_t znak_AD1 =    119 + 128;  // "A."  first character of label Alarm for Darkness (A.d.)
uint8_t znak_AD2 =     94 + 128;  // "d."  second character of label Alarm for Darkness (A.d.)



uint8_t znak_b_tecka =      119 + 128;  // "A."  Character for menu "budik" (A. = Alarm) 
uint8_t znak_o_tecka =       88 + 128;  // "c."  Character for menu "odpocet" (c. = Countdown)

uint8_t znak_u =       95      ;  // "a"   angle distance (CZ = uhlova vzdalenost)

uint8_t znak_eS =     109      ;  // "S"   elevation or azimuth of the Sun
uint8_t znak_eM =      55      ;  // "M"   elevation or azimuth of the Moon
uint8_t znak_oM =       4 + 128;  // "i."  illumination of Moon
uint8_t znak_Atecka  = 119 + 128;  // "A."  azimuth (Sun or Moon)
uint8_t znak_Etecka  = 121 + 128;  // "E."  elevation (Sun or Moon)
uint8_t znak_C       =  57      ;  // "C"   calibration (in label "CAL.")
uint8_t znak_Ltecka  =  56 + 128;  // "L."  calibration (in label "CAL.")
uint8_t znak_vecer   = 121 + 128;  // "E."  Venus is visible at Evening
uint8_t znak_rano    =  55 + 128;  // "M."  Venus is visible at Morning

// defaults descriprion of observer sites
String poz_st_txt[] = {"--", "Obs.Site 1" , "Obs.Site 2" , "Obs.Site 3" , "Obs.Site 4" , "Obs.Site 5"};      // text description of observer sites (10 characters)
uint8_t poz_st_gr[6][5] =                                                                                       // graphical description  of observer sites for display
    {
      {  64,  64,  64,  64,  64} , // "-----"     [0]
      { 109,   4, 120, 249,   6} , // "5itE.1"    [1]
      { 109,   4, 120, 249,  91} , // "5itE.2"    [2]
      { 109,   4, 120, 249,  79} , // "5itE.3"    [3]
      { 109,   4, 120, 249, 102} , // "5itE.4"    [4]
      { 109,   4, 120, 249, 109}   // "5itE.5"    [5]
    };

uint8_t znak_dso_okular_1   =    121     ;  // 'E'      (Eyepiece focal lenght)     "Ey."
uint8_t znak_dso_okular_2   =    110+128 ;  // 'y.'

uint8_t znak_dso_schop_1    =    119     ;  // 'A'      (observer abilities)        "Ab."
uint8_t znak_dso_schop_2    =    124+128 ;  // 'b.' 

uint8_t znak_dso_ref_1      =    80 + 128;  // 'r.'     (reference bright)          "r.b."
uint8_t znak_dso_ref_2      =    124+128 ;  // 'b.' 


uint8_t znak_dso_prumer_1   =    94+128  ;  // 'd.'     (diameter of telescope)     "d."

uint8_t znak_dso_ohnisko_1  =    56+128  ;  // 'L.'     (focal lenght of telescope) "L."


String popisy3231[] = {             // description of DS3231 registres
    "Seconds",                      // register 0x00
    "Minutes" ,                     // register 0x01
    "Hours" ,
    "Day" ,
    "Date" ,
    "Month/Century" ,
    "Year" ,
    "Alarm 1 Seconds",
    "Alarm 1 Minutes",
    "Alarm 1 Hours", 
    "Alarm 1 Day (WeekDay / MonthDay)", 
    "Alarm 2 Minutes", 
    "Alarm 2 Hours", 
    "Alarm 2 Day (WeekDay / MonthDay)", 
    "Control        [EOSC | BBSQW | CONV | RS2 |   RS1   | INTCN | A2IE | A1IE]", 
    "Control/Status [ OSF |   0   |   0  |  0  | EN32kHz |  BSY  |  A2F |  A1F]", 
    "Aging Offset", 
    "MSB of Temp", 
    "LSB of Temp"                   // register 0x12
    };


#endif
//======================= jazykove verze ======================
