// teplota, vlhkost a tlak verze s BME
//=============================================


//----------------------------------------------
// teplota z cidla BME
uint16_t teplota(bool korekce)
  {
    uint16_t teplota_BME;
    if (senzor_BME == true)
      {
        float cidlo_teploty = cidlo_BME.readTemperature();
        teplota_BME  = 5000 + (100 * cidlo_teploty);
        if (korekce == true)
          {
           teplota_BME = korekce_teploty(teplota_BME);
          }
      }
    else
      {
        teplota_BME = 65535; 
      }
    return teplota_BME;
  }
//----------------------------------------------



//----------------------------------------------
// vlhkost z cidla BME
uint16_t vlhkost(void)
  {
    uint16_t vlhkost_BME;
    if (senzor_BME == true)
      {
        float cidlo_vlhkosti = cidlo_BME.readHumidity();
        vlhkost_BME = cidlo_vlhkosti * 100;
      }
    else
      {
        vlhkost_BME = 65535;
      }
    return vlhkost_BME;
  }
//----------------------------------------------



//----------------------------------------------
// tlak z cidla BME
uint16_t tlak(void)
  {
    uint16_t tlak_BME;
    if (senzor_BME == true)
      {
        uint32_t cidlo_tlaku = cidlo_BME.readPressure();
        tlak_BME = cidlo_tlaku - posun_tlaku;                               // aby se tlak vesel do uint16_t rozsahu, odecita se 60000 Pa
      }
    else
      {
        tlak_BME = 65535;
      }
    return tlak_BME;
  }
//----------------------------------------------



//----------------------------------------------
// prevede hodnotu teploty, ktera se uklada do EEPROM na normalni teplotu
float citelna_teplota(uint16_t necitelna_teplota)
  {
    return ((necitelna_teplota - 5000.0) / 100.0);
  }
//----------------------------------------------



//----------------------------------------------
// prevede hodnotu vlhkosti, ktera se uklada do EEPROM na normalni vlhkost
float citelna_vlhkost(uint16_t necitelna_vlhkost)
  {
    return (necitelna_vlhkost / 100.0);
  }
//----------------------------------------------



//----------------------------------------------
// prevede hodnotu tlaku, ktery se uklada do EEPROM a normalni (absolutni) tlak
uint32_t citelny_tlak(uint16_t necitelny_tlak)
  {
    return (necitelny_tlak + posun_tlaku);
  }
//----------------------------------------------
