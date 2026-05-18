// Pripadne rozsireni mereni o dalsi 4 cidla, ktera mohou byt pripojena do bocniho konektoru a odesilat data pres I2C
//  


#ifdef cidlo_rezerva_1                                  // kdyz je cidlo pouzito (povoleno)
uint16_t rezervni_cidlo_1(void)
  {
    //-----------------------
    // tady cist data pres I2C z nejake adresy a registru
    //-----------------------
    return 11111;                                       // vrati zmerenou hodnotu (pro testovani je tu natvrdo hodnota 11111)
  }
#else
uint16_t rezervni_cidlo_1(void)                     // kdyz cidlo pouzito neni
  {
    return 0xFFFF;                                      // vraci hodnotu 0xFFFF
  }
#endif



#ifdef cidlo_rezerva_2                                  // kdyz je cidlo pouzito (povoleno)
uint16_t rezervni_cidlo_2(void)
  {
    //-----------------------
    // tady cist data pres I2C z nejake adresy a registru
    //-----------------------
    return 22222;                                       // vrati zmerenou hodnotu (pro testovani je tu natvrdo hodnota 22222)
  }
#else
uint16_t rezervni_cidlo_2(void)                     // kdyz cidlo pouzito neni
  {
    return 0xFFFF;                                      // vraci hodnotu 0xFFFF
  }
#endif


#ifdef cidlo_rezerva_3                                  // kdyz je cidlo pouzito (povoleno)
uint16_t rezervni_cidlo_3(void)
  {
    //-----------------------
    // tady cist data pres I2C z nejake adresy a registru
    //-----------------------
    return 33333;                                       // vrati zmerenou hodnotu (pro testovani je tu natvrdo hodnota 33333)
  }
#else
uint16_t rezervni_cidlo_3(void)                     // kdyz cidlo pouzito neni
  {
    //-----------------------
    // tady cist data pres I2C z nejake adresy a registru
    //-----------------------
    return 0xFFFF;                                      // vraci hodnotu 0xFFFF
  }
#endif



#ifdef cidlo_rezerva_4                                  // kdyz je cidlo pouzito (povoleno)
uint16_t rezervni_cidlo_4(void)
  {
    return 44444;                                       // vrati zmerenou hodnotu (pro testovani je tu natvrdo hodnota 44444)
  }
#else
uint16_t rezervni_cidlo_4(void)                     // kdyz cidlo pouzito neni
  {
    return 0xFFFF;                                      // vraci hodnotu 0xFFFF
  }
#endif
