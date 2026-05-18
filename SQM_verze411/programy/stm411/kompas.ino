// Rozsireni o naklonomer s kompasem LSM303DLHC
//================================================


int16_t ax, ay, az;                                                                    // tihove zrychleni v kazde ose
int16_t ax_0, ay_0, az_0;                                                              // hodnoty z horizontalni kalibrace naklonu
int16_t ax_90, ay_90, az_90;                                                           // hodnoty z vertikalni kalibrace naklonu

int16_t prum_ax, prum_ay, prum_az ; 

bool kolisa_x = false;
bool kolisa_y = false;
bool kolisa_z = false;

int16_t mx , my , mz ;                                                                 // magneticka sila v kazde ose 
int16_t mag_x_min, mag_y_min, mag_z_min;
int16_t mag_x_max, mag_y_max, mag_z_max;

float posledni_uhel;                                                                   // posledni zmereny uhel naklonu se zaznamenava jako desetinne cislo a pouzije se pro mereni azimutu
uint8_t last_status;                                                                      // pro kontrolu stavu I2C komunikace s modulem LSM303DLHC




//----------------------------------------------
// z naklonu a hodnot magnetometru zjisti azimut natoceni krabicky (0 az 359)
int16_t zjisti_azimut(void)
  {
    if (modul_LSM303DLHC == true)
      {
        nacti_3D_magnet();
    
        mx = map( mx , mag_x_min , mag_x_max , -100 , 100);                                // prevod na +/- 100 pomoci rozsahu zjisteneho pri kalibraci
        my = map( my , mag_y_min , mag_y_max , -100 , 100);
        mz = map( mz , mag_z_min , mag_z_max , -100 , 100);
    
    
        float vaha_my = cos(posledni_uhel * PI / 180);                                     // pri horizontalni poloze se uprednostnuje vypocet s magnetometrem v ose Y
        float vaha_mz = sin(posledni_uhel * PI / 180);                                     // pri vertikalni poloze se uprednostnuje vypocet s magnetometrem v ose Z
                                                                                           // v mezipoloze mezi horizontalnim a vertikalnim naklonem se pouzivani obe osy v "COS"/"SIN" pomeru
        float my_v = my * vaha_my;
        float mz_v = mz * vaha_mz;
         
    
        float azimut = atan2(mx,(my_v + mz_v));
    
        azimut = (180 * (azimut + PI) / PI);                                               // pootoceni azimutu (kvuli smeru ulozeni cidla) a prevod z radianu na stupne
    
        azimut = azimut - offset_kompasu;                                                  // jemne doladeni spatne nalepeneho cidla
        if (azimut > 359) azimut = azimut - 360;
        if (azimut < 0)   azimut = azimut + 360;
        
    
        azimut = constrain(mapfloat(azimut , 0 , 360 , 360 , 0), 0 , 359)  + 0.5;          // pro jistotu jeste omezit na rozsah 0 az 359 stupnu a zaokrouhlit
        return (int)azimut;                                                                // azimut nikdy nebude zaporny, tak staci oseknout desetinnou cast
      }
    else                                                                                   // kdyz je cidlo vypnute
      {
        return 65535;                                                                      // vraci se 65535
      }
  
  }
//----------------------------------------------



//----------------------------------------------
// zjisteni uhlu naklonu vcetne filtrovani nestabilnich hodnot
uint16_t uhel(void)
  {
    if (modul_LSM303DLHC == true)
      {
        long sumax = 0;
        long sumay = 0;
        long sumaz = 0;
    
        int16_t pole_prumeru_x[11];
        int16_t pole_prumeru_y[11];
        int16_t pole_prumeru_z[11];
        int16_t max_opakovani = 5;                                                             // pri kolisani nektereho vzorku v mereni se mohou provest jeste dalsi 4 overovaci mereni
    
        //--------------------------------------
        // filtr 
        bool merit_x = true;
        bool merit_y = true;
        bool merit_z = true;
        
        while (max_opakovani > 0)
          {
            max_opakovani --;
            sumax = 0;
            sumay = 0;
            sumaz = 0;
            for (uint8_t i = 0; i < 4 ; i ++)                                                 // nacteni 4 mereni do poli
              {
                nacti_3D_zrychleni();
                if (merit_x == true)  pole_prumeru_x[i] = ax;
                if (merit_y == true)  pole_prumeru_y[i] = ay;
                if (merit_z == true)  pole_prumeru_z[i] = az;    
                delay(60);
              }
            for (uint8_t i = 0; i < 4 ; i ++)                                                 // prumer 4 mereni naklonu
              {
                sumax = sumax + pole_prumeru_x[i];
                sumay = sumay + pole_prumeru_y[i];
                sumaz = sumaz + pole_prumeru_z[i];
              }
            if (merit_x == true) prum_ax = sumax / 4;
            if (merit_x == true) prum_ay = sumay / 4;
            if (merit_x == true) prum_az = sumaz / 4;
    
            kolisa_x = false;
            kolisa_y = false;
            kolisa_z = false;
    
            
            for (uint8_t i = 0; i < 4 ; i ++)                                                 // vyhodnoceni extremnich odchylek jednotlivych mereni od prumeru
              {            
                if (merit_x == true and pole_prumeru_x[i] - prum_ax >  200) kolisa_x = true;
                if (merit_x == true and pole_prumeru_x[i] - prum_ax < -200) kolisa_x = true;
                if (merit_y == true and pole_prumeru_y[i] - prum_ay >  200) kolisa_y = true;
                if (merit_y == true and pole_prumeru_y[i] - prum_ay < -200) kolisa_y = true;
                if (merit_z == true and pole_prumeru_z[i] - prum_az >  200) kolisa_z = true;
                if (merit_z == true and pole_prumeru_z[i] - prum_az < -200) kolisa_z = true;
              }
    
           if (kolisa_x == false) merit_x = false;
           if (kolisa_y == false) merit_y = false;
           if (kolisa_z == false) merit_z = false;
    
            if (kolisa_x == false and kolisa_y == false and kolisa_z == false)
              {
                break;
              }
            else
              {
                delay(50);
              }
              
          }
    
    
    
        if (max_opakovani == 0 and (kolisa_x == true or kolisa_y == true or kolisa_z == true))   stabilni_naklon = false;   // nestabilni mereni naklonu
        else                                                                                     stabilni_naklon = true;    // stabilni mereni naklonu
    
        // konec filtru    
        // vystupem jsou zprumerovane hodnoty 'prum_ax', 'prum_ay', 'prum_az', pri kterych se 4 po sobe jdouci vzorky neodchylily od prumeru o vic nez 200 cisel
        //     pokud by doslo k vetsi odchylce, provede se jeste dalsi 1 az maximalne 4 pokusy.
        //     kdyz ani po 5 merenich neni hodnota stabilni, vrati se to, co je k dispozici
        //     Zaroven se vraci jeste znacka do globalni promenne 'stabilni_naklon', ktera je vyuzita ve vodovaze (svitici tecky pri nestabilnim naklonu)
        //--------------------------------------
        
        
            
        int16_t aktx = prum_ax - ax_0;
        int16_t akty = prum_ay - ay_0;
        int16_t aktz = prum_az - az_90;
        
        float uhel_z = atan2(aktz, sqrt(square(aktx) + square(akty)))/(PI/180);
    
        uhel_z = mapfloat(uhel_z, 90, 0 , 0 , 90);
        if (akty > 0) uhel_z = -uhel_z;
    
        posledni_uhel = uhel_z;                                             // pro testovani stability mereni
    
        if (uhel_z < 0)   uhel_z = (uhel_z +  999.95) * 10.0;               // zaokrouhleni zaporneho cisla a prevod na kladne cislo (posun o 1000 stupnu)
        else              uhel_z = (uhel_z + 1000.05) * 10.0;               // zaokrouhleni kladneho cisla a posun o 1000 stupnu

        return uhel_z ; 
      }
    else                                                                                   // kdyz je modul zakazany, vraci se 65535
      {
        return 65535;
      }
  }
//----------------------------------------------



//----------------------------------------------
// kalibrace magnetometru nalezenim minimalnich a maximalnich hodnot ve vsech osach
// pro serlinka = true se odesilaji hodnoty pres seriovou linku
// pro serlinka = false se kalibrace provadi pres displej
//  v obou pripadech kalibrace konci v okamziku, kdy se po dobu 15 sekund nezmeni minimum a maximum z ani jedne osy 
void kalibrace_magnet(bool serlinka)
  {

    if (modul_LSM303DLHC == true)
      {
        mag_x_min = 20000;
        mag_y_min = 20000;
        mag_z_min = 20000;
           
        mag_x_max = -20000;
        mag_y_max = -20000;
        mag_z_max = -20000;
        
        bool nulujcasovac = true;
        bool predcasne_ukonceno = false;
        uint32_t stabcas = millis();
        uint16_t zbyva = 15;
        uint16_t posledni_zobrazena_sekunda = 0;
    
        while (millis() - stabcas < 10000)                                                 // 10 sekund se neobjevilo zadne nove minimum ani maximum
          {
            nacti_3D_magnet();
    
            if (mag_x_min > mx)
              {
                mag_x_min = mx;
                nulujcasovac = true; 
              }
            if (mag_x_max < mx)
              {
                mag_x_max = mx;
                nulujcasovac = true; 
              }
    
            if (mag_y_min > my)
              {
                mag_y_min = my;
                nulujcasovac = true; 
              }
            if (mag_y_max < my)
              {
                mag_y_max = my;
                nulujcasovac = true; 
              }
    
            if (mag_z_min > mz)
              {
                mag_z_min = mz;
                nulujcasovac = true; 
              }
            if (mag_z_max < mz)
              {
                mag_z_max = mz;
                nulujcasovac = true; 
              }
    
            if (nulujcasovac == true)
              {
                stabcas = millis();
                nulujcasovac = false;
    
                if (serlinka == true)
                  {
                    Serial.print(mag_x_min);
                    Serial.print(" ; ");
                    Serial.print(mag_y_min);
                    Serial.print(" ; ");
                    Serial.print(mag_z_min);
                    Serial.print(" ; ");
                    
                    Serial.print(mag_x_max);
                    Serial.print(" ; ");
                    Serial.print(mag_y_max);
                    Serial.print(" ; ");
                    Serial.println(mag_z_max);                           
                  }
              }
            else                                                                           // dobiha 10 sek. casovac
              {
                zbyva = 10 - ((millis() - stabcas) / 1000);
                if (zbyva != posledni_zobrazena_sekunda)                                   // kazdou sekundu se aktualizuje informace o zbyvajicim case do konce kalibrace
                  {
                    posledni_zobrazena_sekunda = zbyva;
                    if (serlinka == true)
                      {
                        Serial.print('*');
                        Serial.println(zbyva);
                      }
                    zobraz_cislo(zbyva, 16);
                  }
              }
            if (digitalRead(pin_tl_ok) == LOW)                                             // predcasne ukonceni kalibrace stiskem tlacitka [OK]
              {
                predcasne_ukonceno = true;
                break;
              }
            
            delay(200);
          }                                                                                // dobehlo 10 sekund, behem kterych se nezaznamenal zadny extrem
    
        if (serlinka == true)
          {
            Serial.print("mag_x_min: ");
            Serial.print(mag_x_min);
            Serial.print("    mag_x_max: ");
            Serial.println(mag_x_max);
    
            Serial.print("mag_y_min: ");
            Serial.print(mag_y_min);
            Serial.print("    mag_y_max: ");
            Serial.println(mag_y_max);
    
            Serial.print("mag_z_min: ");
            Serial.print(mag_z_min);
            Serial.print("    mag_z_max: ");
            Serial.println(mag_z_max);
          }
    
        bool chyba_kalibrace = false;
    
        if (predcasne_ukonceno == true) chyba_kalibrace = true;
        
        if (abs(mag_x_max - mag_x_min) < 100) chyba_kalibrace = true;                      // test, jestli je rozdil mezi minimem a maximem dostatecny
        if (abs(mag_y_max - mag_y_min) < 100) chyba_kalibrace = true;
        if (abs(mag_z_max - mag_z_min) < 100) chyba_kalibrace = true;
    
        if (chyba_kalibrace == true)
          {
            chyba(9);                                                                      // problem s kalibraci kompasu ("Err-C")
            if (serlinka == true)
              {
                Serial.println(lng267);                                                    //  "Chyba kalibrace kompasu"
              }
            kompas_setup();                                                                // Pri chybe se nactou puvodni konstanty z EEPROM.
          }
        else                                                                               // kalibrace kompasu probehla s prijatelnymi vysledky
          {
            zobraz_text(38);                                                               // "SAVE"
            tone_X(pin_bzuk,4000, 10 , 6);                                                 // po uspesne kalibraci bzukne (pokud je pipani povoleno)
            delay(300);
            EEPROM_write_int(eeaddr_kompas      , mag_x_min);                              // ulozeni kalibracnich magnetickych konstant do EEPROM
            EEPROM_write_int(eeaddr_kompas +  2 , mag_x_max);
            EEPROM_write_int(eeaddr_kompas +  4 , mag_y_min);
            EEPROM_write_int(eeaddr_kompas +  6 , mag_y_max);
            EEPROM_write_int(eeaddr_kompas +  8 , mag_z_min);
            EEPROM_write_int(eeaddr_kompas + 10 , mag_z_max);
          }
      }
    delay(100);    
  }
//----------------------------------------------



//----------------------------------------------
void naklon_setup(void)
  {
    naklon_write(CTRL_REG4_A, 0x08);                                                   // FS = 00 (+/- 2 g full scale); HR = 1 (high resolution enable)
    naklon_write(CTRL_REG1_A, 0x47);                                                   // ODR = 0100 (50 Hz ODR); LPen = 0 (normal mode); Zen = Yen = Xen = 1 (all axes enabled)

    ax_0  = (int16_t) EEPROM_read_int(eeaddr_naklon_horiz);                            // hodnoty naklonu ve vsech osach pro horizontalni polohu
    ay_0  = (int16_t) EEPROM_read_int(eeaddr_naklon_horiz + 2);
    az_0  = (int16_t) EEPROM_read_int(eeaddr_naklon_horiz + 4);

    ax_90 = (int16_t) EEPROM_read_int(eeaddr_naklon_vert);                             // hodnoty naklonu ve vsech osach pro vertikalni polohu
    ay_90 = (int16_t) EEPROM_read_int(eeaddr_naklon_vert + 2);
    az_90 = (int16_t) EEPROM_read_int(eeaddr_naklon_vert + 4);
  }
//----------------------------------------------



//----------------------------------------------
void kompas_setup(void)
  {
    magnet_write(CRA_REG_M, 0x0C);                                                     // DO = 011 (7.5 Hz ODR)
    magnet_write(CRB_REG_M, 0x20);                                                     // GN = 001 (+/- 1.3 gauss full scale)
    magnet_write(MR_REG_M , 0x00);                                                     // MD = 00 (continuous-conversion mode)

    mag_x_min = (int16_t) EEPROM_read_int(eeaddr_kompas);                              // z EEPROM se nactou minima a maxima z posledni kalibrace kompasu
    mag_x_max = (int16_t) EEPROM_read_int(eeaddr_kompas +  2);
    mag_y_min = (int16_t) EEPROM_read_int(eeaddr_kompas +  4);
    mag_y_max = (int16_t) EEPROM_read_int(eeaddr_kompas +  6);
    mag_z_min = (int16_t) EEPROM_read_int(eeaddr_kompas +  8);
    mag_z_max = (int16_t) EEPROM_read_int(eeaddr_kompas + 10);    

    offset_kompasu = (EEPROM_read(eeaddr_kompas_offset) - 128) / 10.0;                 // doladeni spatne nalepeneho cidla (odecitani uhlu)
  }                                                                                    //   EEPROM =   0 ... offset = -12.8
                                                                                       //   EEPROM = 127 ... offset = - 0.1
                                                                                       //   EEPROM = 128 ... offset = + 0.0
                                                                                       //   EEPROM = 255 ... offset = +12.7
//----------------------------------------------



//----------------------------------------------
void default_naklonomer(void)
  {
    EEPROM_write_int(eeaddr_naklon_horiz      ,  -3216);                               // prvni nastrel nejakych realnych hodnot. Urcite bude ale nutne provest uvodni kalibraci naklonu.
    EEPROM_write_int(eeaddr_naklon_horiz +  2 ,    638);
    EEPROM_write_int(eeaddr_naklon_horiz +  4 ,  18627);
    EEPROM_write_int(eeaddr_naklon_vert       ,  -4073);
    EEPROM_write_int(eeaddr_naklon_vert  +  2 , -16203);
    EEPROM_write_int(eeaddr_naklon_vert  +  4 ,   1311);
    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce        
    ax_0  = (int16_t) EEPROM_read_int(eeaddr_naklon_horiz);                            // hodnoty naklonu ve vsech osach pro horizontalni polohu
    ay_0  = (int16_t) EEPROM_read_int(eeaddr_naklon_horiz + 2);
    az_0  = (int16_t) EEPROM_read_int(eeaddr_naklon_horiz + 4);

    ax_90 = (int16_t) EEPROM_read_int(eeaddr_naklon_vert);                             // hodnoty naklonu ve vsech osach pro vertikalni polohu
    ay_90 = (int16_t) EEPROM_read_int(eeaddr_naklon_vert + 2);
    az_90 = (int16_t) EEPROM_read_int(eeaddr_naklon_vert + 4);
  }
//----------------------------------------------



//----------------------------------------------
void default_kompas(void)
  {
    EEPROM_write_int(eeaddr_kompas     ,-20000);                                       // prednastavena minima a maxima pro vsechny 3 osy magmetometru. Urcite se ale bude muset provest uvodni kalibrace
    EEPROM_write_int(eeaddr_kompas +  2, 20000);
    EEPROM_write_int(eeaddr_kompas +  4,-20000);
    EEPROM_write_int(eeaddr_kompas +  6, 20000);
    EEPROM_write_int(eeaddr_kompas +  8,-20000);
    EEPROM_write_int(eeaddr_kompas + 10, 20000);
    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    

    mag_x_min = (int16_t) EEPROM_read_int(eeaddr_kompas);                              // z EEPROM se nactou minima a maxima, ktera tam byla ulozena v predchozich radkach
    mag_x_max = (int16_t) EEPROM_read_int(eeaddr_kompas +  2);
    mag_y_min = (int16_t) EEPROM_read_int(eeaddr_kompas +  4);
    mag_y_max = (int16_t) EEPROM_read_int(eeaddr_kompas +  6);
    mag_z_min = (int16_t) EEPROM_read_int(eeaddr_kompas +  8);
    mag_z_max = (int16_t) EEPROM_read_int(eeaddr_kompas + 10);  
    kolecko();                                             // vyckavaci kolecko na prostredni sedmisegmentovce    
    
  }
//----------------------------------------------



//----------------------------------------------
// Servisni informace o aktualnich offsetech a krajnich kalibracnich bodech
// Pri parametru 'true' vypise jeste 10x obsah vsech akcelaracnich registru
void naklonomer_info(bool test)
  {
     
    Serial.print("   aX0:");
    srovnej_pod_sebe(ax_0,6);
    Serial.print("   aY0:");
    srovnej_pod_sebe(ay_0,6);
    Serial.print("   aZ0:");
    srovnej_pod_sebe(az_0,6); 
    Serial.print("\r\n");

    Serial.print("  aX90:");
    srovnej_pod_sebe(ax_90,6);
    Serial.print("  aX90:");
    srovnej_pod_sebe(ay_90,6);
    Serial.print("  aX90:");
    srovnej_pod_sebe(az_90,6); 
    Serial.print("\r\n");



    if (test == true)
      {
        for (uint8_t i = 0; i < 10 ; i++)
          {        
            nacti_3D_zrychleni();
            Serial.print("AX: ");
            srovnej_pod_sebe(ax,6);
            Serial.print("      AY: ");
            srovnej_pod_sebe(ay,6);
            Serial.print("      AZ: ");
            srovnej_pod_sebe(az,6);
            Serial.print("\r\n");
            delay(200);
          }
      }
  }
//----------------------------------------------



//----------------------------------------------
// Servisni informace o zmerenych extremech magneticke sily ve vsech osach
// Pri parametru 'true' vypise jeste 10x obsah vsech magnetickych registru 
void magnetometr_info(bool test)
  {
    Serial.print("   mag_x_min:");
    srovnej_pod_sebe(mag_x_min,6);
    Serial.print("   mag_y_min:");
    srovnej_pod_sebe(mag_y_min,6);
    Serial.print("   mag_z_min:");
    srovnej_pod_sebe(mag_z_min,6); 
    Serial.print("\r\n");

    Serial.print("   mag_x_max:");
    srovnej_pod_sebe(mag_x_max,6);
    Serial.print("   mag_y_max:");
    srovnej_pod_sebe(mag_y_max,6);
    Serial.print("   mag_z_max:");
    srovnej_pod_sebe(mag_z_max,6); 
    Serial.print("\r\n");
    
    Serial.print(lng317);                       //  "Kompas pootocen o [stupne]: "
    Serial.println(offset_kompasu,1); 


    if (test == true)
      {
        for (uint8_t i = 0; i < 10 ; i++)
          {        
            nacti_3D_magnet();
            Serial.print("MX: ");
            srovnej_pod_sebe(mx,6);
            Serial.print("      MY: ");
            srovnej_pod_sebe(my,6);
            Serial.print("      MZ: ");
            srovnej_pod_sebe(mz,6);
            Serial.print("\r\n");
            delay(200);
          }
      }    
  }
//----------------------------------------------


// zarovna kladne nebo zaporne cislo na zadany pocet mist.
// pocet mist je vcetne znamenka.
// sirka musi byt minimalne 2 mista
void srovnej_pod_sebe(int16_t vstup, uint8_t sirka)
  {
    uint32_t mocnina;
    uint8_t fs;
    char znamenko = ' ';
    if (vstup < 0)
      {
        znamenko = '-';
        vstup = - vstup;
      }
    
    mocnina = 1;
    for (fs = 1 ; fs < sirka ; fs++)
      {
        mocnina = 10 * mocnina;
      }

    for (fs = sirka ; fs > 2; fs--)
      {
        mocnina = mocnina / 10;
        if ((uint32_t)vstup < mocnina)
          {
            Serial.print(' ');
          }
      }
    Serial.print(znamenko);
    Serial.print(vstup);
  }


 



//----------------------------------------------
// Automaticka kalibrace naklonomeru
// Probiha ve dvou krocich
//     - zjisteni zrychleni vsech 3 os akcelerometru pri horizontalni poloze krabicky
//     - zjisteni zrychleni vsech 3 os akcelerometru pri vertikalni poloze krabicky
//  Vsech 6 16-bitovych hodnot se ulozi do EEPROM na adresy 612 az 623
//  Pri zapnuti napajeni jsou hodnoty z teto pameti vybrany a pouzity pro vypocty.
void kalibrace_naklon(void)
  {
    if (modul_LSM303DLHC == true)
      {
        zobraz_text(47);                                                                   // "n-CAL" (kalibrace naklonomeru)
        
        Serial.println(lng232);                                                            // "Poloz SQM horizontalne"
        delay(2000);
        zobraz_text(64);                                                                   // "Hori" (horizontalni kalibrace naklonomeru)
        while(Serial.available()) Serial.read();
        Serial.println(">>");
        while (Serial.available() == 0 and digitalRead(pin_tl_ok) == HIGH)  delay(100);    // ceka na odklepnuti horizontove polohy, nebo stisk OK
    
        long sumax = 0;
        long sumay = 0;
        long sumaz = 0;
        
        for (uint8_t i = 0; i < 10 ; i ++)
          {
            nacti_3D_zrychleni();
            sumax = sumax + ax;
            sumay = sumay + ay;
            sumaz = sumaz + az;
            delay(100);
          }
        ax_0 = sumax / 10;
        ay_0 = sumay / 10;
        az_0 = sumaz / 10;
        
        Serial.println(lng233);                                                            // "Postav SQM svisle"
        zobraz_text(65);                                                                   // "Vert" (vertikalni kalibrace naklonomeru)
        delay(2000);
        Serial.println(">>");
        while(Serial.available()) Serial.read();
    
        while (Serial.available() == 0 and digitalRead(pin_tl_ok) == HIGH)  delay(100);    // ceka na odklepnuti horizontove polohy, nebo stisk OK
        while(Serial.available()) Serial.read() ;
    
        sumax = 0;
        sumay = 0;
        sumaz = 0;
        
        for (uint8_t i = 0; i < 10 ; i ++)
          {
            nacti_3D_zrychleni();
            sumax = sumax + ax;
            sumay = sumay + ay;
            sumaz = sumaz + az;
            delay(100);
          }
        ax_90 = sumax / 10;
        ay_90 = sumay / 10;
        az_90 = sumaz / 10;
    
        bool chyba_kalibrace = false;
        if (abs(ax_90 - ax_0) >  7000)  chyba_kalibrace = true;                            // tato osa by se mela pri naklonu menit malo
        if (abs(ay_90 - ay_0) < 10000)  chyba_kalibrace = true;                            // tato osa by se mela pri naklonu menit hodne
        if (abs(az_90 - az_0) < 10000)  chyba_kalibrace = true;                            // tato osa by se mela pri naklonu menit hodne
    
        if (chyba_kalibrace == true)                                                       // cidlo naklonu je spatne umistene
          {
            naklonomer_info(true);
    
            Serial.println(lng234);                                                        // "Chyba kalibrace naklonu - nastavuji defaultni hodnoty"
            delay(2000);
            while(Serial.available()) Serial.read();
            Serial.println(">>");
            while (Serial.available() == 0)  delay(100);                                   // ceka na odklepnuti
            while(Serial.available()) Serial.read();
    
            default_naklonomer();                                                          // nastaveni defaultnich hodnot
          }
        else                                                                               // cilo naklonu je umistene dobre
          {
            zobraz_text(38);                                                               // "SAVE"
            delay(400);
            EEPROM_write_int(eeaddr_naklon_horiz     ,  ax_0);                                                  // zprumerovane hodnoty se ulozi do EEPROM
            EEPROM_write_int(eeaddr_naklon_horiz + 2 ,  ay_0);
            EEPROM_write_int(eeaddr_naklon_horiz + 4 ,  az_0);
            EEPROM_write_int(eeaddr_naklon_vert      ,  ax_90);
            EEPROM_write_int(eeaddr_naklon_vert + 2  ,  ay_90);
            EEPROM_write_int(eeaddr_naklon_vert + 4  ,  az_90);
            zobraz_text(66);                                                               // "     " (displej zhasne)
            Serial.println("\r\nOK\r\n");        
          }
      }
  }
//----------------------------------------------



//----------------------------------------------
// dvouosa vodovaha v horizontalni pozici
void vodovaha_hor(void)
  {
    uint8_t maskaX[] = {0,0,0,0,0};
    uint8_t maskaY[] = {0,0,0,0,0};
    
    uhel();

    int16_t pasmo_vyrovnani = 80;                                                          // v jak sirokem pasmu hodnot zrychleni se bude hlasit vyrovnany stav

    if ((prum_ax - ax_0) >   pasmo_vyrovnani)       maskaX[3] = 48;                    // nakloneno moc vpravo      
    if ((prum_ax - ax_0) < - pasmo_vyrovnani)       maskaX[1] =  6;                    // nakloneno moc vlevo

    if (((prum_ax - ax_0) >= - pasmo_vyrovnani) and ((prum_ax - ax_0) <= pasmo_vyrovnani))    // vyrovnano v ose vlevo / vpravo
      {
        maskaX[3] = 48;     
        maskaX[1] =  6;     
      }
    
    if ((prum_ay - ay_0) >  pasmo_vyrovnani)                                           // nakloneno moc dopredu
      {
        maskaY[3] = 8;
        maskaY[2] = 8;
        maskaY[1] = 8;
      }
    if ((prum_ay - ay_0) <  - pasmo_vyrovnani)                                         // nakloneno moc dozadu
      {
        maskaY[3] =  1;
        maskaY[2] =  1;
        maskaY[1] =  1;
      }

    if (((prum_ay - ay_0) >= - pasmo_vyrovnani) and ((prum_ay - ay_0) <= pasmo_vyrovnani))    // vyrovnano v ose dopredu / dozadu
      {
        maskaY[3] = 9;        
        maskaY[2] = 9;
        maskaY[1] = 9;
      }

      
    for (i = 0; i < 5 ; i++)
      {
        D_pamet[i] = maskaX[i] | maskaY[i] ;
      }

    if (stabilni_naklon == false)
      {
        D_pamet[4] = D_pamet[4] | 128 ;                                                // nestabilni naklon rozsviti tecku na vsech zobrazovacich jednotkach
        D_pamet[3] = D_pamet[3] | 128 ;
        D_pamet[2] = D_pamet[2] | 128 ;
        D_pamet[1] = D_pamet[1] | 128 ;
        D_pamet[0] = D_pamet[0] | 128 ;  
      }

    aktualizuj_displej();
    
    delay(20);
    
  }
//----------------------------------------------



//----------------------------------------------
// dvouosa vodovaha v zenitove pozici
void vodovaha_zen(void)
  {
    uint8_t maskaX[] = {0,0,0,0,0};
    uint8_t maskaZ[] = {0,0,0,0,0};
    int16_t pasmo_vyrovnani = 80;                                                          // v jak sirokem pasmu hodnot zrychleni se bude hlasit vyrovnany stav
    
    uhel();

    if (prum_ax >  ax_90 + pasmo_vyrovnani)        maskaX[4] = 33;                     // nakloneno moc vpravo
    if (prum_ax <  ax_90 - pasmo_vyrovnani)        maskaX[0] =  3;                     // nakloneno moc vlevo        

    if (prum_ax >= ax_90 - pasmo_vyrovnani and prum_ax <= ax_90 + pasmo_vyrovnani)     // osa vlevo / vpravo vyrovnana
      {
        maskaX[4] = 33;     
        maskaX[0] =  3;     
      }

    if (prum_az > az_90 - pasmo_vyrovnani)                       maskaZ[2] = 28;       // nakloneno moc dozadu (k sobe)(znak 'u')       
    if (prum_az < az_90 + pasmo_vyrovnani)                       maskaZ[2] = 35;       // nakloneno moc dopredu (od sebe) (znak '^')

    if (prum_az >= az_90 - pasmo_vyrovnani and prum_az <= az_90 + pasmo_vyrovnani)  maskaZ[2] = 63;  // osa dopredu / dozadu vyrovnana
   
    for (i = 0; i < 5 ; i++)
      {
        D_pamet[i] = maskaX[i] | maskaZ[i] ;
      }

    if (stabilni_naklon == false)
      {
        D_pamet[4] = D_pamet[4] | 128 ;                                                // nestabilni naklon rozsviti tecku na vsech zobrazovacich jednotkach
        D_pamet[3] = D_pamet[3] | 128 ;
        D_pamet[2] = D_pamet[2] | 128 ;
        D_pamet[1] = D_pamet[1] | 128 ;
        D_pamet[0] = D_pamet[0] | 128 ;  
      }

    aktualizuj_displej();
    
    delay(20);
  }
//----------------------------------------------



//----------------------------------------------
// zapis jednoho bajtu do naklonomeru
void naklon_write(uint8_t registr, uint8_t nakdata)
  {
    Wire.beginTransmission(I2C_ADDR_NAKLON);
    Wire.write(registr);
    Wire.write(nakdata);
    last_status = Wire.endTransmission();
  }
//----------------------------------------------



//----------------------------------------------
// zapis jednoho bajtu do magnetometru
void magnet_write(uint8_t registr, uint8_t magdata)
  {
    Wire.beginTransmission(I2C_ADDR_KOMPAS);
    Wire.write(registr);
    Wire.write(magdata);
    last_status = Wire.endTransmission();
  }
//----------------------------------------------



//----------------------------------------------
// precte z naklonomeru hodnotu zrychleni ve vsech osach a ulozi je do globalnich promennych ax, ay, az 
void nacti_3D_zrychleni(void)
  {
    if (modul_LSM303DLHC == true)
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
      
        ax = (int16_t)(xha << 8 | xla);
        ay = (int16_t)(yha << 8 | yla);
        az = (int16_t)(zha << 8 | zla);
      }
  }
//----------------------------------------------



//----------------------------------------------
// precte z magletometru hodnotu magnetickeho pole ve vsech osach a ulozi je do globalnich promennych mx, my, mz 
void nacti_3D_magnet(void)
  {
    if (modul_LSM303DLHC == true)
      {
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
      
        mx = (int16_t)(xhm << 8 | xlm);
        my = (int16_t)(yhm << 8 | ylm);
        mz = (int16_t)(zhm << 8 | zlm);
      }
  }
//----------------------------------------------



//----------------------------------------------
// druha mocnina pro velka cisla
int32_t square(int16_t vstup)
  {
    return  1UL * vstup * vstup;
  }
//----------------------------------------------



//----------------------------------------------
// nahrada funkce map() pro desetinna cisla
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
  {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  }
//----------------------------------------------



//----------------------------------------------
// jen testovaci rychle (a nepresne) zjisteni naklonu
uint16_t rychly_naklon(void)
  {
    if (modul_LSM303DLHC == true)
      {
        nacti_3D_zrychleni();
    
        int16_t aktx = ax - ax_0;
        int16_t akty = ay - ay_0;
        int16_t aktz = az - az_90;
        
        float uhel_z = atan2(aktz, sqrt(square(aktx) + square(akty)))/(PI/180);
    
        uhel_z = mapfloat(uhel_z, 90, 0 , 0 , 90);
        if (akty > 0) uhel_z = -uhel_z;
    
        posledni_uhel = uhel_z;
    
        uhel_z = (uhel_z + 0.05 + 1000.0) * 10.0;                                          // prevod na kladne cislo (posun o 1000 stupnu), zaokrouhleni na 1 desetinu a prevod na cele cislo
    
        return uhel_z ;    
      }
    else
      {
        return 65535;
      }
  }
//----------------------------------------------
// z 'pole_azimutu[]' vytvori prumerny azimut zaokrouhleny na cele stupne.
// neprumeruje se cele pole, ale jen pocet vzorku, ktery je uveden v globalni promenne 'velikost_pole_azimutu'. Povoleny interval 1 az 20.
// Bere se ohled i na pripadne kolisani azimutu kolem severniho smeru (mezi 359 a 0 stupni).
// Kdyz jsou hodnoty v poli stabilni, (jejich rozdil neprekrocil hodnotu v globalni promenne 'povoleny_rozptyl_azimutu'), je vysledkem prumerovana hodnota pole.
// Pri nestabilnm mereni se neprumeruje, ale jen vezme se posledni hodnotu v poli (pole_azimutu[velikost_pole_azimutu-1]) a pricte se k ni cislo 500. 
uint16_t prumeruj_azimuty(void) 
  {
    float suma = 0;                              // soucet vsech hodnot pro vytvoreni prumeru
    float prumer = 0;                            // vysledek prumerovani azimutu
    int16_t posun_stupnice = 0;                      // kdyz se hodnoty nachazi pobliz severu, stupnice se kvuli vypoctum pootoci o 90 stupnu
    bool stabil = false;                         // znacka nalezni takoveho posunu, kdy je rozdil mezi hodnotami mensi nez povoleny rozptyl


    if (rozdily_v_poli() == true)                                 // rozdily v merenich po posunu stupnice jsou v toleranci
      {
        stabil = true;
      }
    else                                                          // kdyz je rozdil mezi minimem a maximem moc velky, muze se jednat o prechod pres nulovy azimut
      {
        for (uint8_t i = 0; i < velikost_pole_azimutu; i++)          // ke kazdemu azimutu v poli ...
          {
            pole_azimutu[i] = pole_azimutu[i] + 90;               //  ...  se v tom pripade pricte +90 stupnu
            if (pole_azimutu[i] > 359) pole_azimutu[i] =  pole_azimutu[i] - 360;      // pri prekroceni hranice 359 stupnu se azimut pocita od nuly
          }
        posun_stupnice = 90;
        if (rozdily_v_poli() == true)                             // rozdily v merenich po posunu stupnice jsou v toleranci
          {
            stabil = true;
          }
      }

    if (stabil == true)                                               // kdyz se podarilo takove posunuti vsech hodnot, ze je mezi nimi rozdil do 10 stupnu, provede se normalni prumerovani
      {
        suma = 0;
        for (uint8_t i = 0; i < velikost_pole_azimutu; i++)              // spocte se normalni prumer
          {
            suma += pole_azimutu[i];                                  // soucet vsech (upravenych) azimutu
          }
        
        prumer = suma / velikost_pole_azimutu;
        prumer -= posun_stupnice;                                     // od prumeru se ale jeste musi odecist hodnota posunu stupnice
        if (prumer < 0) prumer += 360;                                // zaporna hodnota vysledneho prumeru se pripadne pootoci do rozsahu 0 az 360 stupnu

        if (posun_stupnice == 90)                                     // kdyz bylo nutne pro urceni stability posunout stupnici o 90 stupnu ...
          {
            for (uint8_t i = 0; i < velikost_pole_azimutu; i++)          // ... vrati vsechny hodnoty v poli na puvodni hodnoty
              {
                pole_azimutu[i] = pole_azimutu[i] - 90;
                if (pole_azimutu[i] < 0) pole_azimutu[i] = 360 + pole_azimutu[i];      // pri podlezeni hranice 0 stupnu se azimut pocita od 360
              }
          }
        prumer =(uint16_t) (prumer +  0.5);                       // zaokrouhleni na vele stupne (hodnota je vzdycky kladna, tak staci pricist 0.5 a oseknout desetinnou cast)
        return prumer;
      }
    else                                                              // ani po posunu stupnice nebyl rozdil mezi merenimi mensi nez 10 stupnu
      {
        for (uint8_t i = 0; i < velikost_pole_azimutu; i++)              // vsechny hodnoty v poli se vrati na puvodni hodnoty
          {
            pole_azimutu[i] = pole_azimutu[i] - 90;
            if (pole_azimutu[i] < 0) pole_azimutu[i] = 360 + pole_azimutu[i];      // pri podlezeni hranice 0 stupnu se azimut pocita od 360
          }
        return pole_azimutu[velikost_pole_azimutu-1] + 500;           // pricteni cisla 500 znamena, ze se prumer nepocital, protoze mereni azimutu nebylo stabilni
      }  
  }


//-------------------------------------------------
// vstupem jsou:
//    globalni pole zmerenych hodnot int16_t 'pole_azimutu[]'
//    globalni promenna 'velikost_pole_azimutu', ktera udava pocet prvku v poli 
//    globalni promenna 'povoleny_rozptyl_azimutu', ze ktere se urcuje stabilni / nestabilni mereni
// vystupem je:
//    TRUE v pripade, ze je rozdil mezi minimem a maximem mensi nez povoleny rozptyl (stabilni mereni)
//    FALSE v pripade, ze je rozdil mezi minimem a maximem vetsi nez povoleny rozptyl (nestabilni mereni, nebo hodnoty kolem azimutu 0 stupnu)
bool rozdily_v_poli(void)
  {
    int16_t min_azim = 999;                                                 // minimum nastavit nerealne vysoko (pri prvnim vzorku v poli se prepise)
    int16_t max_azim = 0;                                                   // maximum nastavit velice nizko (pri prvnim nenulovem azimutu se prepise)
    
    for (uint8_t i = 0; i < velikost_pole_azimutu; i++)
      {
        if (pole_azimutu[i] > max_azim ) max_azim = pole_azimutu[i];    // nalezeni maximalni hodnoty v poli
        if (pole_azimutu[i] < min_azim ) min_azim = pole_azimutu[i];    // nalezeni minimalni hodnoty v poli
      }
    if ((max_azim - min_azim) < povoleny_rozptyl_azimutu) return true;  // rozdil mezi krajnimi hodnotami je maly (mereni je v poradku)
    else                                                 return false;  // rozdil mezi krajnimi hodnotami je velky (mereni je nestabilni, nebo se jedna o prechod kolem nulteho azimutu)
  }
//-------------------------------------------------
    
