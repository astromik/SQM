// Podprogramy pro praci s EEPROM
//================================================



//----------------------------------------------
// provedeni jednoho zaznamu do EEPROM (pocatecni adresa zaznamu je uvedena v promenne 'adr_dalsiho_zaznamu')
// na zaver (pokud je potreba) prepise bit7 nasledujici pametove bunky hodnotou '0' a tim pripravi prostor pro dalsi zaznam
void zaznam_EEPROM(void)
  {
    uint16_t pomprom = (256 * pole_EEPROM[5]) + pole_EEPROM[6];                               // zmerene svetlo v tisicinach mag/arcsec2
    
    if (pomprom / 100 >= hranice_ukladani)                                                        // do EEPROM se zaznam uklada jen v pripade, ze je vetsi tma, nez hodnota v promenne hranice_ukladani
      {
        hranice_ukladani = EEPROM_read(eeaddr_uroven_EEPROM);                                     // protoze se pri kalibraci ukladaji vsechny zaznamy nezavisle na nastavene hranici svetla 
                                                                                                  // (hranice se umele prepisuje na 0), je nutne po kazdem ulozeni nastavenou hranici zase obnovit
        adr_posl_zaznamu = adr_dalsiho_zaznamu;

        if ((pole_EEPROM[0] & 0b01100000) == 0)                                                   // jedna se o zaznam svetla (bity 5 a 6 jsou oba '0')
          {
            AUTOkom_posledni = adr_posl_zaznamu;                                                      // pro automaticke odesilani hodnot do seriove linky se zaznamena index posledniho zaznamu
          }

        for (uint8_t i = 0 ; i < delka_zaznamu ; i++)
          {
            EEPROM_write(adr_posl_zaznamu + i , pole_EEPROM[i]);
          }
        
        adr_dalsiho_zaznamu = adr_dalsiho_zaznamu + delka_zaznamu;
        if (adr_dalsiho_zaznamu >= eeaddr_MAX_EEPROM)      adr_dalsiho_zaznamu = eeaddr_MIN_EEPROM;     // pokud by dalsi zaznam zacinal nad adresou maximalni pouzitelne EEPROM, tak se vrati zpatky na zacatek
        uint8_t pomprom = EEPROM_read(adr_dalsiho_zaznamu);
        if ((pomprom & 0b10000000) > 0)  EEPROM_write(adr_dalsiho_zaznamu , (pomprom & 0b01111111));    // bit7 se nastavi na '0' 

        if (((adr_dalsiho_zaznamu-eeaddr_MIN_EEPROM) % (delka_zaznamu * 100)) == 0 )
          {
            EEPROM_update(eeaddr_10kB_znacka, (adr_dalsiho_zaznamu-eeaddr_MIN_EEPROM) / delka_zaznamu / 100UL );
          }
        priprav_1_zaznam(adr_posl_zaznamu);                                                           // pripraveni jednoho formatovaneho retezce se vsemi namerenymi udaji
      }
    else                                                                                              // jas je moc velky, takze se do EEPROM neuklada, ale textovy vystup se musi pripravit pro SD kartu
      {
        priprav_1_zaznam(0);                                                                          // pripraveni jednoho formatovaneho retezce (spec. verze bez adresy EEPROM)
      }
    SD_save();                                                                                        // pokus o ulozeni zaznamu na SD kartu (na SD kartu se zapisuji vsechny hodnoty nezavisle na urovni svetla)
   
    if (MOD_autoupdate == 1)  EEPROM_to_MOD(index_posledniho_zaznamu + 1);                            // kdyz se maji modbusove registry 30001 atz 30031 aktualizovat automaticky po kazdem mereni, tak se aktualizuji
                                                                                                      //    Novy index_posledniho_zaznamu se pocita v loopu z adresy, ale pro aktualizaci MODBUS registru se musi zvetsit okamzite
    
  }
//----------------------------------------------



//----------------------------------------------
//  prazdne misto se nemusi pokazde hledat od adresy 'eeaddr_MIN_EEPROM'.
//   pri ukladani dochazi po kazdem 100. zapisu do EEPROM k ulozeni "zachytneho bodu", od ktereho se pak pri zapnuti napajeni zacne hledat volne misto
uint32_t najdi_prazdne_misto(uint8_t start_hledani)
  {
    uint32_t adresa;

    for (adresa = eeaddr_MIN_EEPROM + (100UL * delka_zaznamu * start_hledani); adresa <= max_zaznam_EEPROM ; adresa = adresa + delka_zaznamu)
      {
        if ((EEPROM_read(adresa) & 0b10000000) == 0 ) break;                                      // prazdny blok bunek pripraveny pro zapis
      }

    if (adresa > max_zaznam_EEPROM)  adresa = eeaddr_MIN_EEPROM;                                  // kdyz se zadna volna adresa nenasla, tak se pro zapis pripravi prvni blok v EEPROM (na adrese 100)

    adr_posl_zaznamu = adresa - delka_zaznamu;
    if (adr_posl_zaznamu < eeaddr_MIN_EEPROM ) adr_posl_zaznamu = max_zaznam_EEPROM;

    if (EEPROM_read_int(adr_posl_zaznamu) == 0)                                                   // vypocteny posledni zaznam neobsahuje zadny cas
      {
        adr_posl_zaznamu = 0;                                                                     // specialni pripad, pri kterem se nevypisuje posledni zaznam
      }
    
    return adresa;
  }
//----------------------------------------------



//----------------------------------------------
// zapis jednoho bajtu do externi EEPROM
void EEPROM_write(uint32_t adresa, uint8_t wrdata)
  {
    uint8_t stara_hodnota = EEPROM_read(adresa);

    if (adresa > 65535UL)                                                                         // pro adresy vetsi nez 65535 se meni I2C adresy EEPROM 
      {
        adresa = adresa & 0xFFFF;
        I2C_ADDR = I2C_ADDR_EEPROM_RW_1;
      }
    else
      {
        I2C_ADDR = I2C_ADDR_EEPROM_RW_0;
      }

    if (stara_hodnota != wrdata)                                                                  // k zapisu dochazi jen v pripade, ze se ukladana data lisi od puvodni hodnoty
      {
        delay(5);
        Wire.beginTransmission(I2C_ADDR);                                                         // zacatek komunikace s EEPROM
        Wire.write(uint8_t(adresa >> 8));                                                            // MSB z adresy
        Wire.write(uint8_t(adresa % 256));                                                           // LSB z adresy
        delay(1);
        Wire.write(wrdata);                                                                       // ulozeni hodnoty
        uint8_t err = Wire.endTransmission();                                                        // konec komunikace
        if (err > 0) chyba(1);
        else         bitClear(err_bit,0);                                                         // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
        delay(5);

        if (adresa < eeaddr_MIN_EEPROM)  SD_log(adresa , wrdata);                                 // zmeny v systemove casti EEPROM se ukladaji do zvlastniho logovaciho souboru na SD karte
      }
    
  }
//----------------------------------------------



//----------------------------------------------
// zapis 2 bajtu do externi EEPROM
void EEPROM_write_int(uint32_t adresa, uint16_t wrdata)
  {
    EEPROM_write(adresa    , wrdata >> 8);
    EEPROM_write(adresa + 1, wrdata % 256);
  }
//----------------------------------------------



//----------------------------------------------
// zapis 4 bajtu do externi EEPROM
void EEPROM_write_long(uint32_t adresa, uint32_t wrdata)
  {
    EEPROM_write(adresa     ,  (((uint32_t)wrdata >> 24) & 0xFFUL));
    EEPROM_write(adresa + 1 ,  (((uint32_t)wrdata >> 16) & 0xFFUL));
    EEPROM_write(adresa + 2 ,  (((uint32_t)wrdata >>  8) & 0xFFUL));
    EEPROM_write(adresa + 3 ,  (((uint32_t)wrdata      ) & 0xFFUL));    
  }
//----------------------------------------------



//----------------------------------------------
// cteni 1 bajtu z externi EEPROM
uint8_t EEPROM_read(uint32_t addr)
  {
    if (addr > 65535UL)                                                                           // pro adresy vetsi nez 65535 se meni I2C adresy EEPROM 
      {
        addr = addr & 0xFFFF;
        I2C_ADDR = I2C_ADDR_EEPROM_RW_1;
      }
    else
      {
        I2C_ADDR = I2C_ADDR_EEPROM_RW_0;
      }
    
    delayMicroseconds(7);
    Wire.beginTransmission(I2C_ADDR);                                                             // zacatek komunikace s EEPROM
    Wire.write(uint8_t(addr >> 8));                                                               // MSB z adresy
    Wire.write(uint8_t(addr % 256));                                                              // LSB z adresy
    uint8_t err = Wire.endTransmission();                                                         // konec komunikace
    if (err > 0) chyba(1);
    else         bitClear(err_bit,0);                                                             // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'
    delayMicroseconds(2);
    Wire.requestFrom((uint8_t)I2C_ADDR, (uint8_t)1);
    return Wire.read();
  }
//----------------------------------------------



//----------------------------------------------
// cteni 2 bajtu z externi EEPROM
uint16_t EEPROM_read_int(uint32_t adresa)
  {
    return (256 * EEPROM_read(adresa))  +   EEPROM_read(adresa + 1);
  }
//----------------------------------------------



//----------------------------------------------
// cteni 4 bajtu z externi EEPROM
uint32_t EEPROM_read_long(uint32_t adresa)
  {
    uint8_t bajt3 = EEPROM_read(adresa);         // MSB
    uint8_t bajt2 = EEPROM_read(adresa + 1);
    uint8_t bajt1 = EEPROM_read(adresa + 2);
    uint8_t bajt0 = EEPROM_read(adresa + 3);    // LSB

    return   (((uint32_t)bajt3 << 24))  | (((uint32_t)bajt2 << 16)) | (((uint32_t)bajt1 << 8)) | ((uint32_t)bajt0);
  }
//----------------------------------------------



//----------------------------------------------
//  zapisuje do EEPROM, ale jen v pripade, ze se data v EEPROM lisi od vkladanych dat
//   (zpomaluje se tim sice zapis, ale prodluzuje se tim trvanlivost bunek)
void EEPROM_update(uint32_t addr, uint8_t wrdata)
  {
    uint8_t pomprom = EEPROM_read(addr);
    if (pomprom != wrdata)   EEPROM_write(addr,wrdata);
  }
//----------------------------------------------



//----------------------------------------------
// Test pridavne (puvodne zasouvaci) EEPROM pro specialni rezimy.
//  I2C adresa pridavne EEPROM je vzdycky 0b1010111 (= 0x57 = 87).
uint8_t test_zasunuti(void)
  { 
    delayMicroseconds(7);
    Wire.beginTransmission(87);                                                                   // zacatek komunikace s EEPROM
    Wire.write(0);                                                                                // MSB z adresy
    Wire.write(0);                                                                                // LSB z adresy
    delay(1);
    Wire.endTransmission();                                                                       // konec komunikace
    delayMicroseconds(2);
    Wire.requestFrom(87, 1);
    return Wire.read();                     
  }
//----------------------------------------------

  
