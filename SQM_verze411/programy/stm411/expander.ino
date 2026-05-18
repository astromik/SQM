// Podprogramy pro praci s expanderem MCP23017 pro prepinani vice cidel
//================================================


//----------------------------------------------
// prepnuti napajeni na zadane cidlo 0 az 7  (na portech PA0 a PB0 ale neni cidlo pripojeno, takze se pri 0 vsechna cidla odpoji)
//  Kladne napajeni je z portu B, zaporne z portu A
// ostatni cidla budou vypnuta (vystupy expanderu prepnute na INPUT)
void expa_switch(uint8_t cislo)
  {
    if ((pocet_cidel_svetla > 1))                                         // kdyz je nastaveno vice cidel svetla
      {
         if (EXPA_test() == true)                                         // test, jestli se i2c adrese expanderu vubec neco hlasi
          {
            EXPA_write(0x00 , 255);                                       // IODIR_A na vstup
            EXPA_write(0x0C ,   0);                                       // GPPUA vypnout vsechny Pull-Upy
            EXPA_write(0x01 , 255);                                       // IODIR_B na vstup
            EXPA_write(0x0D ,   0);                                       // GPPUB vypnout vsechny Pull-Upy
        
            if (cislo < 8)                                                // pro cisla 0 az 7 se prepinaji napajeci piny, pro cislo 8 nebo vic zustavaji vsechny napajeci piny vypnute
              {
                                                                          // pak zvoleny pin na poru A prepnout na '0' a stejny pin na portu B prepnout na '1'
                uint8_t pomprom = ~(1 << cislo);                             // prevod cisla na nulovy bit (0=bit0, 1=bit1, 5=bit5 ....) ostatni bity zustavaji v '1'
                EXPA_write(0x00 , pomprom);                               // zvoleny bit IODIR_A na vystup
                EXPA_write(0x01 , pomprom);                               // zvoleny bit IODIR_B na vystup
            
                EXPA_write(0x12 ,   0);                                   // vsechny bity v GPIO_A na '0' (ve skutecnosti se ale nastavi jen ten jeden bit, ktery je vystupni)
                EXPA_write(0x13 , 255);                                   // vsechny bity v GPIO_B na '1' (ve skutecnosti se ale nastavi jen ten jeden bit, ktery je vystupni)
              } 
          }
      }
  }
//----------------------------------------------



//----------------------------------------------
//  zapis do expanderu MCP23017 pres I2C komunikaci
void EXPA_write(uint8_t registr, uint8_t exdata)
  {
    delay(5);
    Wire.beginTransmission(I2C_ADDR_EXPA);                                // zacatek komunikace s expanderem
    Wire.write(registr);                                                  // cislo registru
    delay(5);
    Wire.write(exdata);                                                   // zapis hodnoty
    uint8_t err = Wire.endTransmission();                                    // konec komunikace
    if (err > 0) chyba(1);
    else         bitClear(err_bit,0);                                     // komunikace I2C je v poradku, maze se pripadny chybovy bit v promenne 'err_bit'

  }
//----------------------------------------------



//----------------------------------------------
// test prevzaty z I2C skeneru, jestli se na I2C adrese expanderu vubec neco hlasi
bool EXPA_test(void)
  {
    Wire.beginTransmission (I2C_ADDR_EXPA);
    if (Wire.endTransmission () == 0)  return true;
    else                               return false;
     
  }
