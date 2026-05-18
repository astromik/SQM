// Astronomicke vypocty
//==================================================================
//  (Zakladni vzorce opsane nekde z internetu a prevedeno z BASICu do WIRINGu)


//-----------------------------------------------
// posun z lokalniho casu do UTC (pro SEC je to 1, pro SELC je to 2)
// vstupem jsou globalni promenne "LOC_xxx"
// vysledky jsou v globalnich promennych "astro_UTC_xxx"
void z_LOC_na_Astro_UTC(int16_t offset)    
  {
    // pocet dni v mesici pro ruzne pocitani dni od zacatku roku (unor se pak pri prestupnem roce jeste upravuje)
    //                  x , Led, Uno, Bre, Dub, Kve, Cer, Cec, Srp, Zar, Rij, Lis, Pro
    int16_t  pomdny_d[] = { 0,   31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };
  
    if ((LOC_rok % 4) == 0)     pomdny_d[2] = 29;             // pri prestupnem roku se nastavuje unor na 29 dni
    if (LOC_rok == 2100)        pomdny_d[2] = 28;             // rok 2100 bude specialita. I kdyz je deliteny 4, tak NEBUDE prestupny.
    
    astro_UTC_min = LOC_min;                                  // na zacatku se UTC promenne nastavi na stejnou hodnotu jako mistni cas
    astro_UTC_rok = LOC_rok;
    astro_UTC_mes = LOC_mes;
    astro_UTC_den = LOC_den;
    astro_UTC_hod = LOC_hod - offset;                         // ... akorat od hodin se odecte casovy posun (1 hodina pro SEC, 2 hodiny pro SELC ...)
  
    if (astro_UTC_hod < 0)                                    // kontrola, jestli po odecteni casove zony, nedoslo k podteceni casu do predchozicho dne
      {
        astro_UTC_hod = astro_UTC_hod + 24;
        astro_UTC_den = astro_UTC_den - 1;
        if (astro_UTC_den < 1)                                // kontrola podteceni do predchoziho mesice
          {
            astro_UTC_mes = astro_UTC_mes - 1;
            if (astro_UTC_mes < 1)                            // kontrola podteceni do predchoziho roku
              {
                astro_UTC_rok = astro_UTC_rok - 1;
                astro_UTC_mes = 12;
              }
            astro_UTC_den = pomdny_d[astro_UTC_mes];          // cislo dne se nastavi na nejvyssi cislo dne v predchozim mesici
          }
      
      }
    
  }
//-----------------------------------------------
  


//-----------------------------------------------
//  vypocty pro polohy Mesice a Slunce (opsano nekde z internetu)
void astro_vypocty(void)
  {
    if (leto == true) casova_zona = letni_posun;
    else              casova_zona = zimni_posun;
    
    z_LOC_na_Astro_UTC(casova_zona);                          // z globalnich promennych pro casove udaje v mistni casove zone (LOC_xxx) vypocte UTC datum a cas
 
    double days = day2000(astro_UTC_rok, astro_UTC_mes, astro_UTC_den, astro_UTC_hod + astro_UTC_min/60.0);  
    double t = days / 36525.0;

    
    //  Data Slunce
    double L1   = rozsah(280.466 + 36000.8 * t);
    double M1   = rozsah(357.529+35999*t - 0.0001536* t*t + t*t*t/24490000.0);
    double C1   = (1.915 - 0.004817* t - 0.000014* t * t)* dsin(M1);   
           C1   = C1 + (0.01999 - 0.000101 * t)* dsin(2*M1);
           C1   = C1 + 0.00029 * dsin(3*M1);
    double V1   = M1 + C1;
    double EC1  = 0.01671 - 0.00004204 * t - 0.0000001236 * t*t;
    double R1   = 0.99972 / (1 + EC1 * dcos(V1));
    double Th1  = L1 + C1;
    double OM1  = rozsah(125.04 - 1934.1 * t);
    double LaM1 = Th1 - 0.00569 - 0.00478 * dsin(OM1);
    double Obl  = (84381.448 - 46.815 * t)/3600;
    double Ra1  = datan2(dsin(Th1) * dcos(Obl) - dtan(0)* dsin(Obl), dcos(Th1));
    double DEC1 = dasin(dsin(0)* dcos(Obl) + dcos(0)*dsin(Obl)*dsin(Th1));
    
    //  Data Mesic
    double F    = rozsah(93.2721 + 483202 * t - 0.003403 * t* t - t * t * t/3526000.0);
    double L2   = rozsah(218.316 + 481268 * t);
    double Om2  = rozsah(125.045 - 1934.14 * t + 0.002071 * t * t + t * t * t/450000.0);
    double M2   = rozsah(134.963 + 477199 * t + 0.008997 * t * t + t * t * t/69700.0);
    double D    = rozsah(297.85 + 445267 * t - 0.00163 * t * t + t * t * t/545900.0);
    double D2   = 2*D;
    double R2   = 1 + (-20954 * dcos(M2) - 3699 * dcos(D2 - M2) - 2956 * dcos(D2)) / 385000.0;
    double R3   = (R2 / R1) / 379.168831168831;
    
    double Bm   = 5.128 * dsin(F) + 0.2806 * dsin(M2 + F);
           Bm   = Bm + 0.2777 * dsin(M2 - F) + 0.1732 * dsin(D2 - F);
    double Lm   = 6.289 * dsin(M2) + 1.274 * dsin(D2 -M2) + 0.6583 * dsin(D2); 
           Lm   = Lm + 0.2136 * dsin(2*M2) - 0.1851 * dsin(M1) - 0.1143 * dsin(2 * F); 
           Lm   = Lm +0.0588 * dsin(D2 - 2*M2) ;
           Lm   = Lm + 0.0572* dsin(D2 - M1 - M2) + 0.0533* dsin(D2 + M2);
           Lm   = Lm + L2;
    double Ra2  = (datan2(dsin(Lm) * dcos(Obl) - dtan(Bm)* dsin(Obl), dcos(Lm))) / 15.0;
    double Dec2 = dasin(dsin(Bm)* dcos(Obl) + dcos(Bm)*dsin(Obl)*dsin(Lm));

    double HLm  = rozsah(LaM1 + 180 + (180/PI) * R3 * dcos(Bm) * dsin(LaM1 - Lm));
    double HBm  = R3 * Bm;

    double I    = 1.54242;
    double W    = Lm - Om2;
    double qY   = dcos(W) * dcos(Bm);
    double qX   = dsin(W) * dcos(Bm) * dcos(I) - dsin(Bm) * dsin(I);
    double A    = datan2(qX, qY);
//    double EL   = A - F;
//    double EB   = dasin(-dsin(W) * dcos(Bm) * dsin(I) - dsin(Bm) * dcos(I));
          
           W    = rozsah(HLm - Om2);
           qY   = dcos(W) * dcos(HBm);
           qX   = dsin(W) * dcos(HBm) * dcos(I) - dsin(HBm) * dsin(I);
           A    = datan2(qX, qY);
//    double SL   = rozsah(A - F);
//    double SB   = dasin(-dsin(W) * dcos(HBm) * dsin(I) - dsin(HBm) * dcos(I));
//    double Co   = 0;
//           if (SL < 90)  Co = 90 - SL;
//           else          Co = 450 - SL;

           A    = dcos(Bm) * dcos(Lm - LaM1);
    double Psi  = 90 - datan(A / sqrt(1- A*A));
           qX   = R1 * dsin(Psi);
           qY   = R3 - R1* A;
    double Il   = datan2(qX, qY);
    double K    = (1 + dcos(Il))/2;

    RaToAzim ( 1 , Ra1 / 15.0,  DEC1 , LOC_rok , LOC_mes , LOC_den , LOC_hod , LOC_min , casova_zona , GeoLon , GeoLat );       // Slunce
    RaToAzim ( 2 ,     Ra2   ,  Dec2 , LOC_rok , LOC_mes , LOC_den , LOC_hod , LOC_min , casova_zona , GeoLon , GeoLat );       // Mesic

    Mes_osvit   = zaokrouhlit(K*100);
    Mes_elevace = zaokrouhlit(Mes_elevace) + 90;              // do pameti se ukladaji jen kladna cisla, proto se elevace posouvaji o 90 stupnu vyse
    Slu_elevace = zaokrouhlit(Slu_elevace) + 90;              // do pameti se ukladaji jen kladna cisla, proto se elevace posouvaji o 90 stupnu vyse
    Mes_azimut  = zaokrouhlit(Mes_azimut);
    Slu_azimut  = zaokrouhlit(Slu_azimut);
  }
//-----------------------------------------------



//-----------------------------------------------
int16_t zaokrouhlit(double n)
  {
    if (n >= 0) n = n + 0.5;                                  // kladna cisla se zaokrouhluji prictenim 0.5 a oseknutim desetinne casti
    else        n = n - 0.5;                                  // zaporna cisla se zaokrouhluji odectenim 0.5 a oseknutim desetinne casti
    return (int)n;    
  }
//-----------------------------------------------



//-----------------------------------------------
// Ruzna aritmetika - opsano nekde z internetu (prevedeno z BASICu do PHP a pak sem do Wiringu)
double rozsah(double x)
  {
    double b = x / 360.0;
    double a = 360 * (b - ipart(b));
    if (a  < 0 )
      {
        a = a + 360;
      }
    return a;
  }

double ipart(double x)
  {
    double a;
    if (x > 0)
      {
        a = floor(x);
      }
    else
      {
        a = ceil(x);
      } 
    return a;
  }

double dsin(double x)
  {
    return sin(PI / 180.0 * x);
  }
  
double dcos(double x)
  {
    return cos(PI / 180.0 * x);
  }

double dtan(double x)
  {
    return tan(PI / 180.0 * x);
  }

//   pocet dnu od J2000
double day2000(int16_t y, int16_t m, int16_t d, double h)
  {
//    unsigned long greg = y*10000 + m*100 + d;
    if (m == 1 || m == 2)
      {
        y = y - 1;
        m = m + 12;
      }

    double a = floor(y/100);
    double b = 2 - a  + floor(a/4);
      
    int32_t c = floor(365.25 * y);
    int32_t d1 = floor(30.6001 * (m + 1));
    return ( b + c + d1 -730550.5 + d + h/24.0);
  } 

double datan2(double y, double x)
  {
    double a;
    if ((x == 0) && (y == 0))
      {
        return 0;
      }
    else
      {
        a = datan(y / x);
        if (x < 0)
          {
            a = a + 180; 
          }
        if (y < 0 && x > 0)
          {
            a = a + 360;
          }
        return a;
      }
  }

double dasin(double x)
  {
    return 180/ PI * asin(x);
  }

double dacos(double x)
  {
    return 180/ PI * acos(x);
  }
  
double datan(double x)
  {
    return 180/ PI * atan(x);
  }
//-----------------------------------------------



//-----------------------------------------------
//  prevede Rektascenzi a Deklinaci na Azimut a Elevaci (podle lokalniho casu, mistni casove zony a zemepisnych souradnic)
void RaToAzim( uint8_t teleso , double qRA, double qDECL, int16_t qrok, uint8_t qmes, uint8_t qden, uint8_t qhod, uint8_t qmin , int16_t qutcdif, double qlong, double qlat)
  {
    double qcas = qhod + (qmin / 60.0);
    double dj2000 = floor(day2000(qrok, qmes, qden, qhod +12)) + ((qcas - qutcdif) / 24.0) - 0.5;

    double lst = 100.46 + 0.985647 * dj2000 + qlong + 15.0 * (qcas - qutcdif);

  
    while (lst <= 0)   lst = lst + 360;
    
    while (lst >= 360) lst = lst - 360;
  
    double ha = lst - qRA * 15.0;
  
    while (ha <= 0)    ha = ha + 360;
    
    while (ha >= 360)  ha = ha - 360;

  
    double alt = dasin(dsin(qDECL) * dsin(qlat) + dcos(qDECL) * dcos(qlat) * dcos(ha));

    double azm = datan2(dsin(ha), dcos(ha) * dsin(qlat) - dtan(qDECL) * dcos(qlat)) + 180;
    if (azm >= 360)    azm = azm - 360;

    if (teleso == 1)                                          // teleso je Slunce
      {
        Slu_elevace = alt;
        Slu_azimut = azm;   
      }          

    if (teleso == 2)                                          // teleso je Mesic
      {
        Mes_elevace = alt;
        Mes_azimut = azm;
       }

    if (teleso == 3)                                          // teleso neni zadane (pri prepoctu Ra-Dec na Az-El z menu)
      {
        obecna_elevace = alt;
        obecny_azimut = azm;
       }
  }
//-----------------------------------------------



//-----------------------------------------------
// ze dvou azimutu a elevaci vypocte uhlovou vzdalenost ve stupnich
float uhlova_vzdalenost(int16_t azimut1, int16_t elevace1, int16_t azimut2, int16_t elevace2)
  {
    float u_vzdal;
    u_vzdal = dacos((dcos(elevace1) * dcos(elevace2) * dcos(azimut1 - azimut2)) + (dsin(elevace1) * dsin(elevace2)));
    
    return u_vzdal;
  }
//-----------------------------------------------



//-----------------------------------------------
// ze zadanych paraetru vypocte pocet dni vctene casove casti dne od 1.1.2000
double xday2000(int16_t d2000_rok, int16_t d2000_mes, int16_t d2000_den, int16_t d2000_hod, int16_t d2000_min)
  {
    double a;
    int16_t  b;
    a = 10000.0 * d2000_rok + 100.0 * d2000_mes + d2000_den;
    if (d2000_mes <= 2)
      {
        d2000_mes = d2000_mes + 12;
        d2000_rok = d2000_rok - 1;
      }
    b = (d2000_rok / 400) - (d2000_rok / 100) + (d2000_rok / 4);
    a = 365.0 * d2000_rok - 730548.5;

    double floo = floor(30.6001 * (d2000_mes + 1));
    double hms = ((d2000_hod + (d2000_min / 60.0) ) / 24.0);
    double vystup = a + b + floo + d2000_den + hms;
    return  vystup;
  }
//-----------------------------------------------



//-----------------------------------------------
// pro zadanou planetu (parametr pnumber) vypocte pro nastaveny den (parametr d) Rektascenzi a Deklinaci (globalni promene 'globra' a 'globde')
// pnumber:  Merkur=1, Venuse=2, Mars=3 ..... Neptun=7,  (Pluto=8)
// d: den od 1.1.2000 vcetne jeho casti - vypocitava se v podprogramu xday2000()
void planet(double d, int16_t pnumber)
  {
    double X, Y , z, v , m , r , xe , ye , ze , s1, si , so , c1 , ci , co, ecl;
    element (d,pnumber);
    m = range2pi(pla_l - pla_p);
    v = kepler(m, pla_e, 8);
    r = pla_a * (1 - pla_e * pla_e) / (1 + pla_e * cos(v));
    s1 = sin(v + pla_p - pla_o);
    si = sin(pla_i);
    so = sin(pla_o);
    c1 = cos(v + pla_p - pla_o);
    ci = cos(pla_i);
    co = cos(pla_o);
    X = r * (co * c1 - so * s1 * ci);
    Y = r * (so * c1 + co * s1 * ci);
    z = r * (s1 * si);
    element (d,0);                               // vypocty pro Zemekouli (index 0)
    m = range2pi(pla_l - pla_p);
    v = kepler(m, pla_e, 8);
    r = pla_a * (1 - pla_e * pla_e) / (1 + pla_e * cos(v));
    s1 = sin(v + pla_p - pla_o);
    si = sin(pla_i);
    so = sin(pla_o);
    c1 = cos(v + pla_p - pla_o);
    ci = cos(pla_i);
    co = cos(pla_o);
    xe = r * (co * c1 - so * s1 * ci);
    ye = r * (so * c1 + co * s1 * ci);
    ze = r * (s1 * si);
    X = X - xe;
    Y = Y - ye;
    ecl = 23.429292 * rads;
    xe = X;
    ye = Y * cos(ecl) - z * sin(ecl);
    ze = Y * sin(ecl) + z * cos(ecl);
    
    globra = atan2(ye, xe) * degs;
    if (globra < 0)  globra = 360 + globra;

    globde = atan(ze / sqrt(xe * xe + ye * ye)) * degs;

    globvzd = sqrt(xe * xe + ye * ye + ze * ze);
  }
//-----------------------------------------------



//-----------------------------------------------
double kepler(double m, double ecc, double eps)
  {
    double delta, e , v ;
    e = m;                                          //            'first guess
    delta = 0.05;                                   //            'set delta equal to a dummy value
    while (abs(delta) >= pow(10 , -eps))            //            'converged?
      {
        delta = e - ecc * sin(e) - m;               //            'new error
        e = e - delta / (1 - ecc * cos(e));         //            'corrected guess
      }
    v = 2 * atan(   pow((   (1 + ecc) / (1 - ecc) ) , 0.5) * tan(0.5 * e));
    if (v < 0)  v = v + pla_tpi;
    return v;
  }
//-----------------------------------------------



//-----------------------------------------------
// podle zadaneho poctu dni od 1.1.2000 (parametr 'd') se do promennych ulozi elementy drahy planety s cislem 'pnum'
void element(double d, uint8_t pnum)
  {
    switch(pnum)
      {
        case 0:          //earth
            pla_i = (0.00005 - 0.000000356985 * d) * rads;
            pla_o = (-11.26064 - 0.00013863 * d) * rads;
            pla_p = (102.94719 + 0.00000911309 * d) * rads;
            pla_a = 1.00000011 - 1.36893E-12 * d;
            pla_e = 0.01671022 - 0.00000000104148 * d;
            pla_l = range2pi(rads * (100.46435 + 0.985609101 * d));
            break;        
        case 1:          // mercury
            pla_i = (7.00487 - 0.000000178797 * d) * rads;
            pla_o = (48.33167 - 0.0000033942 * d) * rads;
            pla_p = (77.45645 + 0.00000436208 * d) * rads;
            pla_a = 0.38709893 + 1.80698E-11 * d;
            pla_e = 0.20563069 + 0.000000000691855 * d;
            pla_l = range2pi(rads * (252.25084 + 4.092338796 * d));
            break;
        case 2:          //venus
            pla_i = (3.39471 - 0.0000000217507 * d) * rads;
            pla_o = (76.68069 - 0.0000075815 * d) * rads;
            pla_p = (131.53298 - 0.000000827439 * d) * rads;
            pla_a = 0.72333199 + 2.51882E-11 * d;
            pla_e = 0.00677323 - 0.00000000135195 * d;
            pla_l = range2pi(rads * (181.97973 + 1.602130474 * d));
            break;
        case 3:          //mars
            pla_i = (1.85061 - 0.000000193703 * d) * rads;
            pla_o = (49.57854 - 0.0000077587 * d) * rads;
            pla_p = (336.04084 + 0.00001187 * d) * rads;
            pla_a = 1.52366231 - 0.000000001977 * d;
            pla_e = 0.09341233 - 0.00000000325859 * d;
            pla_l = range2pi(rads * (355.45332 + 0.524033035 * d));
            break;
        case 4:          //jupiter
            pla_i = (1.3053 - 0.0000000315613 * d) * rads;
            pla_o = (100.55615 + 0.00000925675 * d) * rads;
            pla_p = (14.75385 + 0.00000638779 * d) * rads;
            pla_a = 5.20336301 + 0.0000000166289 * d;
            pla_e = 0.04839266 - 0.00000000352635 * d;
            pla_l = range2pi(rads * (34.40438 + 0.083086762 * d));
            break;
        case 5:          //saturn
            pla_i = (2.48446 + 0.0000000464674 * d) * rads;
            pla_o = (113.71504 - 0.0000121 * d) * rads;
            pla_p = (92.43194 - 0.0000148216 * d) * rads;
            pla_a = 9.53707032 - 0.0000000825544 * d;
            pla_e = 0.0541506 - 0.0000000100649 * d;
            pla_l = range2pi(rads * (49.94432 + 0.033470629 * d));
            break;
        case 6:          //uranus
            pla_i = (0.76986 - 0.0000000158947 * d) * rads;
            pla_o = (74.22988 + 0.0000127873 * d) * rads;
            pla_p = (170.96424 + 0.0000099822 * d) * rads;
            pla_a = 19.19126393 + 0.0000000416222 * d;
            pla_e = 0.04716771 - 0.00000000524298 * d;
            pla_l = range2pi(rads * (313.23218 + 0.011731294 * d));
            break;
        case 7:          //neptune
            pla_i = (1.76917 - 0.0000000276827 * d) * rads;
            pla_o = (131.72169 - 0.0000011503 * d) * rads;
            pla_p = (44.97135 - 0.00000642201 * d) * rads;
            pla_a = 30.06896348 - 0.0000000342768 * d;
            pla_e = 0.00858587 + 0.000000000688296 * d;
            pla_l = range2pi(rads * (304.88003 + 0.0059810572 * d));
            break;
        case 8:          //pluto (uz neni planeta)
            pla_i = (17.14175 + 0.0000000841889 * d) * rads;
            pla_o = (110.30347 - 0.0000002839 * d) * rads;
            pla_p = (224.06676 - 0.00000100578 * d) * rads;
            pla_a = 39.48168677 - 0.0000000210574 * d;
            pla_e = 0.24880766 + 0.00000000177002 * d;
            pla_l = range2pi(rads * (238.92881 + 3.97557152635181E-03 * d));
            break;

        // tady by mohly byt dalsi CASE napriklad pro asteroidy, komety, nebo dalsi trpaslici planety
        
        }
  }
//-----------------------------------------------



//-----------------------------------------------
double range2pi(double pla_X)
  {
    double r2pi =  pla_X - (pla_tpi * (int)(pla_X / pla_tpi));
    return r2pi;
  }
//-----------------------------------------------



//-----------------------------------------------
// vyzaduje korektni globalni promenne 'casova_zona', 'GeoLon', 'GeoLat'
//  parametr 'sim_planeta' je poradove cislo planety od Slunce Zeme se preskakuje (1=Merkur, 2=Venuse, 3=Mars, ... 7=Neptun, 8=Pluto.
//                                                              Dalsi objekty jsou 9=Mesic; 10=Slunce, 11=astro noc, 12=nastavitelny soumrak)
void simulace_planet(uint8_t sim_planeta)
  {
    
    bool start_elevace = false;
    int16_t plusmin;
    if (leto == true) casova_zona = letni_posun;
    else              casova_zona = zimni_posun;
    double korekce_horizontu = 0;                                    // pro planety se pocita vychod/zapad v okamziku kdy stred planety prechazi pres elevaci 0 stupnu
                                                                     // pro Slunce a Mesic se pro vychod/zapad pocita okamzik, kdy je videt horni okraj (0,83 stupnu pod horizontem)
                                                                     // pro astronomickou noc je Slunce 18 stupnu pod horizontem 

    SIM_rok = LOC_rok;                                               // prekopirovani aktualniho mistniho datumu a casu do simulacnich promennych
    SIM_mes = LOC_mes;
    SIM_den = LOC_den;
    SIM_hod = LOC_hod;
    SIM_min = LOC_min;

    double pladen2000 = xday2000(SIM_rok, SIM_mes, SIM_den, SIM_hod, SIM_min);  // pocet dni od 1.1.2000 vcetne desetinne casove casti

    for (plusmin = 0; plusmin < 1440 ; plusmin ++)                   // smycka pro simulaci 24 hodin od aktualniho casu po kroku 1 minuty
      {
        if (sim_planeta < 9)
          {
            element(pladen2000,sim_planeta);                             // vypocet elementu drahy pro zvolenou planetu a nastaveny cas
            planet(pladen2000, sim_planeta);
            RaToAzim( 3 , globra / 15, globde, SIM_rok, SIM_mes, SIM_den , SIM_hod, SIM_min , casova_zona, GeoLon, GeoLat);  // vypocte azimut a elevaci planety pro nastaveny den a cas
            pladen2000 = pladen2000 + (1/1440.0);                        // pri kazde smycce se vypocteny 'pladen2000' zvysi o 1 minutu (1440 minut = 1 den)
            korekce_horizontu = 0;
          }

        
        if (sim_planeta == 9)                                            // pro Mesic jsou specialni vypocty
          {
            double days = day2000(SIM_rok, SIM_mes, SIM_den, SIM_hod + SIM_min/60.0);  
            double t = days / 36525.0;            

            //  Data Slunce
            double L1   = rozsah(280.466 + 36000.8 * t);
            double M1   = rozsah(357.529+35999*t - 0.0001536* t*t + t*t*t/24490000.0);
            double C1   = (1.915 - 0.004817* t - 0.000014* t * t)* dsin(M1);   
                   C1   = C1 + (0.01999 - 0.000101 * t)* dsin(2*M1);
                   C1   = C1 + 0.00029 * dsin(3*M1);
            double V1   = M1 + C1;
            double EC1  = 0.01671 - 0.00004204 * t - 0.0000001236 * t*t;
            double R1   = 0.99972 / (1 + EC1 * dcos(V1));
            double Th1  = L1 + C1;
            double OM1  = rozsah(125.04 - 1934.1 * t);
            double LaM1 = Th1 - 0.00569 - 0.00478 * dsin(OM1);
            double Obl  = (84381.448 - 46.815 * t)/3600;
            double Ra1  = datan2(dsin(Th1) * dcos(Obl) - dtan(0)* dsin(Obl), dcos(Th1));
            double DEC1 = dasin(dsin(0)* dcos(Obl) + dcos(0)*dsin(Obl)*dsin(Th1));
            RaToAzim( 1 , Ra1 / 15, DEC1, SIM_rok, SIM_mes, SIM_den , SIM_hod, SIM_min , casova_zona, GeoLon, GeoLat);  // vypocte azimut a elevaci Mesic pro nastaveny den a cas
            
                    
            
            //  Data Mesic
            double F    = rozsah(93.2721 + 483202 * t - 0.003403 * t* t - t * t * t/3526000.0);
            double L2   = rozsah(218.316 + 481268 * t);
            double Om2  = rozsah(125.045 - 1934.14 * t + 0.002071 * t * t + t * t * t/450000.0);
            double M2   = rozsah(134.963 + 477199 * t + 0.008997 * t * t + t * t * t/69700.0);
            double D    = rozsah(297.85 + 445267 * t - 0.00163 * t * t + t * t * t/545900.0);
            double D2   = 2*D;
            double R2 = 1 + (-20954 * dcos(M2) - 3699 * dcos(D2 - M2) - 2956 * dcos(D2)) / 385000.0;
            double r2km   = R2 * 385000.0;
            double R3   = (R2 / R1) / 379.168831168831;
            globvzd = r2km/1000.0;                                  // vzdalenost v tisich km
            double Bm   = 5.128 * dsin(F) + 0.2806 * dsin(M2 + F);
                   Bm   = Bm + 0.2777 * dsin(M2 - F) + 0.1732 * dsin(D2 - F);
            double Lm   = 6.289 * dsin(M2) + 1.274 * dsin(D2 -M2) + 0.6583 * dsin(D2); 
                   Lm   = Lm + 0.2136 * dsin(2*M2) - 0.1851 * dsin(M1) - 0.1143 * dsin(2 * F); 
                   Lm   = Lm +0.0588 * dsin(D2 - 2*M2) ;
                   Lm   = Lm + 0.0572* dsin(D2 - M1 - M2) + 0.0533* dsin(D2 + M2);
                   Lm   = Lm + L2;


            double Ra2  = (datan2(dsin(Lm) * dcos(Obl) - dtan(Bm)* dsin(Obl), dcos(Lm)));
            double Dec2 = dasin(dsin(Bm)* dcos(Obl) + dcos(Bm)*dsin(Obl)*dsin(Lm));

            double HLm  = rozsah(LaM1 + 180 + (180/PI) * R3 * dcos(Bm) * dsin(LaM1 - Lm));
            double HBm  = R3 * Bm;
        
            double I    = 1.54242;
            double W    = Lm - Om2;
            double qY   = dcos(W) * dcos(Bm);
            double qX   = dsin(W) * dcos(Bm) * dcos(I) - dsin(Bm) * dsin(I);
            double A    = datan2(qX, qY);
        //    double EL   = A - F;
        //    double EB   = dasin(-dsin(W) * dcos(Bm) * dsin(I) - dsin(Bm) * dcos(I));
                  
                   W    = rozsah(HLm - Om2);
                   qY   = dcos(W) * dcos(HBm);
                   qX   = dsin(W) * dcos(HBm) * dcos(I) - dsin(HBm) * dsin(I);
                   A    = datan2(qX, qY);
            double SL   = rozsah(A - F);
        //    double SB   = dasin(-dsin(W) * dcos(HBm) * dsin(I) - dsin(HBm) * dcos(I));
            double Co   = 0;
                   if (SL < 90)  Co = 90 - SL;
                   else          Co = 450 - SL;
        
                   A    = dcos(Bm) * dcos(Lm - LaM1);
            double Psi  = 90 - datan(A / sqrt(1- A*A));
                   qX   = R1 * dsin(Psi);
                   qY   = R3 - R1* A;
            double Il   = datan2(qX, qY);
            double K    = (1 + dcos(Il))/2;
            Mes_osvit   = zaokrouhlit(K*100);
            RaToAzim( 3 , Ra2 / 15, Dec2, SIM_rok, SIM_mes, SIM_den , SIM_hod, SIM_min , casova_zona, GeoLon, GeoLat);  // vypocte azimut a elevaci Mesic pro nastaveny den a cas
            
            if (plusmin == 0)                                                // informace o terminatoru, uhlove vzdalenosti od Slunce a colongitudu se ukladaji jen pro aktualni cas 
              {
                Mes_colong = Co;  
    
                if (Mes_colong < 90)                             Mes_term =  Mes_colong;
                if (Mes_colong >= 90 and Mes_colong < 270 )      Mes_term =  Mes_colong - 180;
                if (Mes_colong >= 270)                           Mes_term =  Mes_colong - 360;

                Mes_term = -Mes_term;                                        // dodatecne otoceni znamenka u terminatoru
   
                if (Mes_colong >  270 and Mes_colong < 360)    P_east_elo = 1;         // Mesic je videt chvili jen vecer, zobrazuje se i uhlova vzdalenost od Slunce
                if (Mes_colong <= 270 and Mes_colong > 180)    P_east_elo = 0;         // Mesic je videt chvili jen rano, zobrazuje se i uhlova vzdalenost od Slunce
                   
                P_elongace = uhlova_vzdalenost(obecny_azimut, obecna_elevace, Slu_azimut , Slu_elevace);
              }

            korekce_horizontu = -0.833;                                      // vychod/zapad horniho okraje Mesice je o polovinu uhloveho prumeru pod horizontem (je tam i nejaka korekce refrakce)
          }


        if (sim_planeta == 10 or sim_planeta == 11 or sim_planeta == 12)     // pro Slunce, astronomickou noc a volitelny soumrak jsou specialni vypocty
          {
            double days = day2000(SIM_rok, SIM_mes, SIM_den, SIM_hod + SIM_min/60.0);  
            double t = days / 36525.0;
             
            //  Data Slunce
            double L1   = rozsah(280.466 + 36000.8 * t);
            double M1   = rozsah(357.529+35999*t - 0.0001536* t*t + t*t*t/24490000.0);
            double C1   = (1.915 - 0.004817* t - 0.000014* t * t)* dsin(M1);   
                   C1   = C1 + (0.01999 - 0.000101 * t)* dsin(2*M1);
                   C1   = C1 + 0.00029 * dsin(3*M1);
            double V1   = M1 + C1;
            double EC1  = 0.01671 - 0.00004204 * t - 0.0000001236 * t*t;
            double R1   = 0.99972 / (1 + EC1 * dcos(V1));
            
            double Th1  = L1 + C1;
//            double OM1  = rozsah(125.04 - 1934.1 * t);
//            double LaM1 = Th1 - 0.00569 - 0.00478 * dsin(OM1);
            double Obl  = (84381.448 - 46.815 * t)/3600;
            double Ra1  = datan2(dsin(Th1) * dcos(Obl) - dtan(0)* dsin(Obl), dcos(Th1));
            double DEC1 = dasin(dsin(0)* dcos(Obl) + dcos(0)*dsin(Obl)*dsin(Th1));   
            RaToAzim( 3 , Ra1 / 15, DEC1, SIM_rok, SIM_mes, SIM_den , SIM_hod, SIM_min , casova_zona, GeoLon, GeoLat);  // vypocte azimut a elevaci Mlunce pro nastaveny den a cas
            globvzd = R1;
            if (sim_planeta == 10) korekce_horizontu = -0.833;            // vychod / zapad horniho okraje Slunce je o polovinu uhloveho prumeru pod horizontem (je tam i nejaka korekce refrakce)
            if (sim_planeta == 11) korekce_horizontu = -18;               // astronomicka noc je pri stredu Slunce 18 stupnu pod horizontem
            if (sim_planeta == 12) korekce_horizontu = -elevace_soumrak;  // pri indexu 12 se pocita uzivatelsky nastavitelny soumrak
          }

        if (plusmin == 0)                                                 // v prvnim kroku smycky (aktualni cas) se zaznamena, jestli je planeta nad nebo pod horizontem   
          {
            SIM_aktel = (int)round(obecna_elevace);                       // aktualni elevace, azimut a vzdalenost se poznamenaji do pomocnych globalnich promennych
            SIM_aktaz = (int)(obecny_azimut + 0.5);
            SIM_osvit = Mes_osvit;
            SIM_aktvzd_presne = globvzd;
            if (sim_planeta != 9)   SIM_aktvz = (int)((globvzd * 100.0)+0.5);       // vzdalenost vsech planet a Slunce se zaznamenava jako INTEGER v setinach AU
            else                    SIM_aktvz = (int)((globvzd *  10.0)+0.5);       // krome Mesice (sim_planeta==9), ktery se zaznamenava v tisicich km na jedno des. misto
                                                                                    // pro astronomickou noc (sim_planeta==11) se ani vzdalenost nezobrazuje
                      
            if (obecna_elevace < korekce_horizontu) start_elevace = false;           // aktualne je planeta pod horizontem a bude se hledat nasledujici vychod nad horizont 
            else                                    start_elevace = true;            // aktualne je planeta nad horizontem a bude se hledat nasledujici zapad pod horizont 
          }

        
        
        if (start_elevace == false and obecna_elevace > korekce_horizontu)  break;   // planeta pri simulaci prave vysla nad horizont, vypadne se ze smycky
        if (start_elevace == true  and obecna_elevace < korekce_horizontu)  break;   // planeta pri simulaci prave zapadla pod horizont, vypadne se ze smycky
        SIM_plus_minuta();                                                           // simulacni promenne se posunou o jednu minutu dal (s kontrolou preteceni minut, hodin, dni a mesicu)
      }
    
    if (plusmin == 1440)                                             // pri simulaci v trvani 24 hodin se planeta nedostala nad nebo pod horizont
      {
        SIM_vyza  = 0;                                               // k zadne udalosti nedojde
        SIM_azim  = 9999;                                            // azimut a ostatni vysledky vypoctu se nastavi na nesmysly
        SIM_zamin = 9999;
        SIM_cas_h = 9999;
        SIM_cas_m = 9999;        
      }
    else
      {
        if (obecna_elevace > korekce_horizontu)       SIM_vyza = 1;  // pristi udalost bude vychod planety
        if (obecna_elevace < korekce_horizontu)       SIM_vyza = 2;  // pristi udalost bude zapad planety

        SIM_azim = obecny_azimut;                                    // do globalni promenne se nastavi azimut na kterem k udalosti dojde
        SIM_zamin = plusmin;                                         // do globalni promenne se nastavi za kolik minut k udalosti dojde
        SIM_cas_h = SIM_hod;                                         // konkretni cas udalosti se kopiruje do globalnich promennych 'SIM_cas_h', 'SIM_cas_m'
        SIM_cas_m = SIM_min;
      }                                                              


    if (sim_planeta > 0 and sim_planeta < 9)                         // pro Merkur a Venusi se pocita elongace, pro ostatni planety jen magnituda
      {
        vypocty_elongace(LOC_rok, LOC_mes, LOC_den, LOC_hod, LOC_min, sim_planeta); // do globalnich promennych P_faze a P_elongace ulozi vypoctene hodnoty pro aktualni cas
      }

    if (sim_planeta == 11)           
      {
        astro_noc(18);                                               // pro astronomickou noc se spousti jeste dalsi podprogram pro vypocty
      }

    if (sim_planeta == 12)                                           // pro uzivatelsky nastavitelny soumrak se spusti stejny podprogram jako pro astro noc
      {                                                              //          akorat misto pevne nastavene elevace -18 stupnu se pouzije uzivatelsky definovana elevace
        astro_noc(elevace_soumrak);                                    
      }

  
  }
//-----------------------------------------------



//-----------------------------------------------
//  Hledani casu astronomicke noci, nebo uzivatelsky definovaneho soumraku.
//   Soumrak se pocita jako okamzik, kdy Slunce poklesne nastavenou hodnotu pod horizont (nastavuje se funkci "@N")
//   Simuluje se vypocet RA a DEC pro Slunce v case 12:00 az 12:00 nasledujiciho dne.
//   Z RA a DEC se pak vypocitava elevace Slunce.
//   Kdyz elevace klesne pod hranicni elevaci, zaznamena se cas zacatku noci (soumraku), kdyz pak elelvace vyroste nad hranicni elevaci, zaznamena se konec noci (soumraku)
void astro_noc(uint8_t hranicni_elevace)
  {
    zobraz_RTC(false);

    if (leto == true) casova_zona = letni_posun;
    else              casova_zona = zimni_posun;
    

    SIM_rok = LOC_rok;                                                           // prekopirovani aktualniho mistniho datumu a casu do simulacnich promennych
    SIM_mes = LOC_mes;
    SIM_den = LOC_den;
    SIM_hod = LOC_hod;
    SIM_min = LOC_min;

    double days = day2000(SIM_rok, SIM_mes, SIM_den, SIM_hod + SIM_min/60.0);    // v prvnim kroku se zjisti, jestli v aktualnim case je nebo neni astronomicka noc
    double t = days / 36525.0;
    ra_dec_slunce(t);        
    RaToAzim ( 1 , globra / 15.0,  globde , SIM_rok , SIM_mes , SIM_den , SIM_hod , SIM_min , casova_zona , GeoLon , GeoLat );       // Slunce

    if (Slu_elevace < -hranicni_elevace)                                         // prave ted JE astronomicka noc, zacatek simulace se posune zpatky
      {
        if (SIM_hod < 12)                                                        // je noc po pulnoci, simulace se posune na poledne predchoziho dne
          {
            SIM_minus_den();
          }                                                                      // kdyz je noc pred pulnoci, se dnem se nehyba a zacatek simulace se nastavi na dnesni poledne 
      }
                                                                                 // kdyz prave ted noc NENI, nastavi se zacatek simulace na dnesni poledne
    SIM_hod = 12;                                                                // poledne
    SIM_min = 0;


    astro_noc_zacatek = 9999;                                        // pro pripad, ze by v zadany den noc nenastala, nastavi se promenne na nesmyslnou hodnotu
    astro_noc_konec = 9999;

    int16_t plusminuty = 0;
    uint8_t event = 0;                                                  // predpokalda se, ze v poledne astronomicka noc nebude, takze prvni udalost bude zacatek noci
    while (plusminuty < 1440)                                        // simuluje se 24 hodin od poledne do nasledujiciho poledne
      {
        double days = day2000(SIM_rok, SIM_mes, SIM_den, SIM_hod + SIM_min/60.0);  
        double t = days / 36525.0;
        ra_dec_slunce(t);
        RaToAzim ( 1 , globra / 15.0,  globde , SIM_rok , SIM_mes , SIM_den , SIM_hod , SIM_min , casova_zona , GeoLon , GeoLat );       // Slunce
   
        if (Slu_elevace < -hranicni_elevace and event == 0)
          {
            astro_noc_zacatek = (SIM_hod * 60) + SIM_min;            // pocet minut od pulnoci
            event = 1;
          }
    
        if (Slu_elevace > -hranicni_elevace and event == 1)
          {
            astro_noc_konec =  (SIM_hod * 60) + SIM_min;             // pocet minut od pulnoci
            if (astro_noc_konec < 9999 and astro_noc_zacatek < 9999)
              {
                astro_noc_trvani = (1440 - astro_noc_zacatek) + astro_noc_konec;          // trvani je pocet minut od zacatku do pulnoci a pocet minut od pulnoci do konce
                if (astro_noc_trvani > 1440) astro_noc_trvani = astro_noc_trvani - 1440;  // v obdobi nekolika letnich dni, kdy noc zacina i konci po lokalni pulnoci se musi interval opravit
              }
            else
              {
                astro_noc_trvani = 0;                                // kdyz neni astronomicka noc bezpecne ohranicena obema casy, nepocita se delka jejiho trvani
              }                                                      // (za poladnim kruhem se muze stat, ze astronomicka noc bude trvat dele nez 24 hodin, ale i v tom pripade to pocita 0)
            event = 2;
            break;                                                   // po nalezeni konce noci uz dalsi simulace neni potreba a ze smycky se vypadne
          }
        
        plusminuty++;                                                // pricitaji se minuty od pulnoci
        SIM_plus_minuta();                                           // zaroven se pricitaji SIM_minuty s osetrenim prechodu do nasledujicich SIM_hodin , SIM_dni ....

      }

  
  }
//-----------------------------------------------



//-----------------------------------------------
// z promenne 't' (cas od 1.1.2000) vypocte RA a DEC pro Slunce
// vysledek ulozi do globalnich promennych 'globra' a 'globde'
void ra_dec_slunce(double t)
  {
    //  Data Slunce
    double L1   = rozsah(280.466 + 36000.8 * t);
    double M1   = rozsah(357.529+35999*t - 0.0001536* t*t + t*t*t/24490000.0);
    double C1   = (1.915 - 0.004817* t - 0.000014* t * t)* dsin(M1);   
           C1   = C1 + (0.01999 - 0.000101 * t)* dsin(2*M1);
           C1   = C1 + 0.00029 * dsin(3*M1);
    double V1   = M1 + C1;
    double EC1  = 0.01671 - 0.00004204 * t - 0.0000001236 * t*t;
    double R1   = 0.99972 / (1 + EC1 * dcos(V1));
    double Th1  = L1 + C1;
//    double OM1  = rozsah(125.04 - 1934.1 * t);
//    double LaM1 = Th1 - 0.00569 - 0.00478 * dsin(OM1);
    double Obl  = (84381.448 - 46.815 * t)/3600;
    globra  = datan2(dsin(Th1) * dcos(Obl) - dtan(0)* dsin(Obl), dcos(Th1));
    globde = dasin(dsin(0)* dcos(Obl) + dcos(0)*dsin(Obl)*dsin(Th1));
    globvzd = R1;
  }




//-----------------------------------------------
// Od globalni casove promenne pro simulaci 'SIM_den' odecte 1 den.
//   Kdyz dojde k podteceni na predchozi mesic nebo rok, nastavi se spravne i odpovidajici promenne 'SIM_mes' a 'SIM_rok'
void SIM_minus_den(void)
  {
    int16_t  pomdny_d[] = { 0,   31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };
  
    if ((SIM_rok % 4) == 0)     pomdny_d[2] = 29;             // pri prestupnem roku se nastavuje unor na 29 dni
    if (SIM_rok == 2100)        pomdny_d[2] = 28;             // rok 2100 bude specialita. I kdyz je deliteny 4, tak NEBUDE prestupny.
 
    SIM_den --;
    if (SIM_den == 0)                 // doslo k podteceni dni do predchoziho mesice
      {
        SIM_mes --;
        SIM_den = pomdny_d[SIM_mes];
        if (SIM_mes == 0)             // doslo k podteceni mesicu do predchoziho roku 
          {
            SIM_den = 31;
            SIM_mes = 12;
            SIM_rok --;
          }
      }
  }
//-----------------------------------------------



//-----------------------------------------------
// Ke globalnim casovym promennym pro simulaci pricte 1 minutu.
//   Kdyz dojde k preteceni nejake promenne, nastavi se na minimalni hodnotu a zvysi se nasledujici polozka
void SIM_plus_minuta(void)
  {
    int16_t  pomdny_d[] = { 0,   31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };
  
    if ((SIM_rok % 4) == 0)     pomdny_d[2] = 29;             // pri prestupnem roku se nastavuje unor na 29 dni
    if (SIM_rok == 2100)        pomdny_d[2] = 28;             // rok 2100 bude specialita. I kdyz je deliteny 4, tak NEBUDE prestupny.

    
    SIM_min ++;
    if (SIM_min == 60)                        // doslo k preteceni minut do nasledujici hodiny
      {
        SIM_min = 0;
        SIM_hod ++;
        if (SIM_hod == 24)                    // doslo k preteceni hodin do nasledujiciho dne
          {
            SIM_hod = 0;
            SIM_den ++;
            if (SIM_den > pomdny_d[SIM_mes])  // doslo k preteceni dni aktualniho mesice do dalsiho mesice
              {
                SIM_den = 1;
                SIM_mes ++;
                if (SIM_mes == 13)            // doslo k preteceni mesicu do nasledujiciho roku 
                  {
                    SIM_mes = 1;
                    SIM_rok ++;
                  }
              }
          }
      }
  }


//-----------------------------------------------
// vypocte fazi planety (Venuse nebo Merkuru) a jeji elongaci pro zadany den mesic a rok
//   Vysledky necha v globalnich double promennych P_faze a P_elongace
void vypocty_elongace(int16_t plarok, int16_t plamesic, int16_t pladen, int16_t plahodina, int16_t plaminuta, uint8_t plaindex)
  {
    SIM_rok = plarok; 
    SIM_mes = plamesic;
    SIM_den = pladen;
    SIM_hod = plahodina;
    SIM_min = plaminuta;
    P_mag_SAT = 99.99;
    R_mag     = 99.99;

    double pladen2000 = xday2000(SIM_rok, SIM_mes, SIM_den, SIM_hod, SIM_min);  // pocet dni od 1.1.2000 vcetne desetinne casove casti

    element(pladen2000,plaindex);                             // vypocet elementu drahy pro Merkur (index 1), nebo pro Venusi (index 2)
    planet(pladen2000, plaindex);

    double raP = globra;
    double deP = globde;
    double vzdal_ZP = globvzd;


    P_ra = raP;
    P_de = deP;
    P_vzdalenost = vzdal_ZP;
    
    double days = day2000(SIM_rok, SIM_mes, SIM_den, SIM_hod + SIM_min/60.0);    //Slunce
    double t = days / 36525.0;
    ra_dec_slunce(t);        


    double raS = globra;
    double deS = globde;
    double vzdal_ZS = globvzd;
    dist_ZS = globvzd;                           // jsete jedna globalni kopie pro simulacni tabulku

    S_ra = raS;
    S_de = deS;
        
    double xs = dcos(deS) * dcos(raS);           // kartezske souradnice pro Slunce
    double ys = dcos(deS) * dsin(raS);
    double zs = dsin(deS);
    
    double xp = dcos(deP) * dcos(raP);          // kartezske souradnice pro planetu
    double yp = dcos(deP) * dsin(raP);
    double zp = dsin(deP);

    // vyhodnoceni zapadni nebo vychodni elongace podle rektascenze Slunce a Venuse (Merkuru)
    //   Ve vyhodnoceni se vyuziva toho, ze se Slunce a Venuse nikdy nevzdali o vic nez 48 stupnu  (Merkur o 28 stupnu) 
    //      (rozdil rektascenzi muze byt v nekterych pripadech i o neco vetsi, takze tu hranici mam pro Venusi s rezervou 70 stupnu).
    //   Existuji pouze 4 typy vzajemneho usporadani objektu (S=Slunce, 0=nulova rektascenze, P=Planeta):
    
    //           >>>>  >>>>  >>>>  smer zvysovani rektascenze  >>>>  >>>>  >>>>
    //       A)    S 0 P        S 0 P        S 0 P        S 0 P        S 0 P         (Slunce je pred nulovou hranici rektascenze, Planeta je za nulovou hranici rektascenze)
    //       B)    P 0 S        P 0 S        P 0 S        P 0 S        P 0 S         (Planeta je pred nulovou hranici rektascenze, Slunce je za nulovou hranici rektascenze)
    //       C)      0    P  S    0    P  S    0    P  S    0    P  S    0    P  S   (Slunce je pred Planetou (vpravo od Planety), neni mezi nimi nulova hranice rektascenze)
    //       D)      0    S  P    0    S  P    0    S  P    0    S  P    0    S  P   (Planeta je pred Sluncem (vpravo od Slunce), neni mezi nimi nulova hranice rektascenze)
    
    if (plaindex == 1 or plaindex == 2)         // vychodni a zapadni elongace se urcuje jen planet Merkur a Venuse
      {
        if ((raS-raP) > 70)                     // priklad A): Ra Slunce = 390, Ra Planety = 10 (mezi objekty je hranice nulove rektasscenze)
          {
            P_east_elo = 1;                     //   jednoznacne urceni vzajemneho usporadani - dalsi testy uz se neprovadi
          }
        else
          {
            if ((raP-raS) > 70)                 // priklad B): Ra Planety = 390, Ra Slunce = 10 (mezi objekty je hranice nulove rektasscenze)
              {
                P_east_elo = 0;                 //   opet jednoznacne urceni vzajemeho usporadani - tentokrat je vysledek opacny nez v A)
              }
            else                                // zbyvaji dve verze usporadani, pri kterych oba objekty lezi na ose vedle sebe mimo hranici nulove rektascenze
              {
                if ((raP-raS) < 0)              // priklad C): Ra Planety = 10, Ra Slunce = 30
                  {
                    P_east_elo = 0;
                  }
                else                            // Vsechno ostatni musi byt posledni typ usporadani D): Ra Slunce = 10, Ra Planety = 30
                  {
                    P_east_elo = 1;
                  }
              }
          }
        
      }
    else                                        // u ostatnich objektu se vychodni / zapadni elongace nastavuje na 2 (neurcita)
      {
        P_east_elo = 2;      
      }


    double pomprom = xs*xp + ys*yp + zs*zp;       // skalarni soucin
    P_elongace = dacos(pomprom);                  // uhlova vzdalenost mezi Sluncem a planetou pri pohledu ze Zeme

    double vzdal_SP2 = (vzdal_ZS * vzdal_ZS) + (vzdal_ZP * vzdal_ZP) - (2 * vzdal_ZS * vzdal_ZP * dcos(P_elongace));
    double vzdal_SP = sqrt(vzdal_SP2);            // Vzdalenost Slunce - Planeta ...
    dist_SP = vzdal_SP;                           //  ... se jeste ulozi do globalni promenne pro pripadne dalsi zpracovani 

    
                                                  // vzdal_SP  ..... vzdalenost Slunce_Planeta
                                                  // vzdal_ZP  ..... vzdalenost Zeme_Planeta
                                                  // vzdal_ZS  ..... vzdalenost Zeme_Slunce


    fazovy_uhel = dacos((pow(vzdal_ZP,2) + pow(vzdal_SP,2) - pow(vzdal_ZS,2)) / (2 * vzdal_ZP * vzdal_SP)); // uhel mezi Zemi a Sluncem s vrcholem v planete
    fazovy_uhel = abs(fazovy_uhel);

    P_faze = (pow(vzdal_SP + vzdal_ZP,2) - pow(vzdal_ZS,2) ) / (4 * vzdal_SP * vzdal_ZP);



    // vypocet magnitud pro planety je prevzaty z kodu programu "Cartes du Ciel" (SkyChart)
    //    https://github.com/pchev/skychart/blob/master/skychart/cu_planet.pas

    double pomuhel;

    switch (plaindex)
      {
        case 1:                                               // Merkur
          P_mag = -0.42 + (5 * log10(vzdal_SP * vzdal_ZP)) + (0.038 * fazovy_uhel) + (- 0.000273 * pow(fazovy_uhel,2)) + (0.000002 * pow(fazovy_uhel,3));
          break;

        case 2:                                               // Venuse
          P_mag = -4.4 + (5 * log10(vzdal_SP * vzdal_ZP)) + (0.0009 * fazovy_uhel) + (0.000239 * pow(fazovy_uhel,2)) + (-0.00000065 * pow(fazovy_uhel,3));
          break;

        case 3:                                               // Mars
          P_mag = -1.52 + (5 * log10(vzdal_SP * vzdal_ZP)) +  (0.016 * fazovy_uhel);
          break;

        case 4:                                               // Jupiter
          P_mag = -9.40 + (5 * log10(vzdal_SP * vzdal_ZP)) +  (0.005 * fazovy_uhel);
          break;

        case 5:                                               // Saturn
          P_mag_SAT = -8.88 + (5 * log10(vzdal_SP * vzdal_ZP)) +  (0.044 * fazovy_uhel);   // Jen samotna planeta Saturn bez prstencu
          P_mag = P_mag_SAT;                                                               // pro pripad, ze by se uz nepocitaly prstence, tak by byla zobrazovana magnituda jen pro planetu
 
          // ===========================================================================================================
          // **      TAHLE CAST BY SE MOHLA JESTE UPRAVIT PRO PRESNEJSI MAGNITUDU, ALE NA ME UZ JE TO MOC SLOZITE
          //  dostupne promenne:
          // P_ra, P_de   ... rektascenze Saturnu, deklinace Saturnu ve stupnich
          // S_ra, S_de   ... rektascenze Slunce, deklinace Slunce ve stupnich
          // P_mag_SAT    ... zdanliva magnituda samotneho Saturnu bez prstence 
          // P_faze       ... osvetlena cast planety (0 az 1 = 0% az 100%)
          // fazovy_uhel  ... uhel mezi Zemi a Sluncem s vrcholem v Saturnu (uhlova vzdalenost Zeme-Slunce pri pohledu ze Saturnu) ve stupnich
          // vzdal_SP     ... vzdalenost Slunce-Saturn v astronomickych jednotkach
          // dist_ZS      ... vzdalenost Zeme-Slunce v astronomickych jednotkach
          // P_vzdalenost ... vzdalenost Zeme-Saturn v astronomickych jednotkach
          // pladen2000   ... pocet dni od 1.1.2000 vcetne desetinne casti pro hodiny a minuty
          // t            ... julianske stoleti od 1.1.2000 ( t = pladen2000 / 35625.0)
          // pla_i        ...  ???
          // pla_o        ...  ???
          // pla_p        ...  ???
          // pla_a        ...  ???
          // pla_e        ...  ???
          // pla_l        ...  ???
          //    priklad pro 15.8.2025 12:00
          //          P_ra         =    1.616351
          //          P_de         =   -1.946678
          //          S_ra         =  145.273892
          //          S_de         =   13.870893
          //          P_mag_SAT    =    0.893799
          //          P_faze       =    0.998943
          //          fazovy_uhel  =    3.726468
          //          vzdal_SP     =    9.559966
          //          dist_ZS      =    1.012764
          //          P_vzdalenost =    8.739982
          //          pladen2000   = 9358.000000
          //          t            =    0.256208
          //          pla_i        =   -0.000057
          //          pla_o        =   -0.219177
          //          pla_p        =    1.798256
          //          pla_a        =    1.000000
          //          pla_e        =    0.016700
          //          pla_l        =    5.651277


          pomuhel = (P_ra*pla_tpi)/360;                       // rektascenze planety Saturn v radianech         
          R_mag = abs(sin(pomuhel)*0.88) + 0.056;             // hruby odhad magnitudy pro samotne prstence v zavislosti na rektascenzi          
          P_mag = P_mag_SAT - R_mag;                          // jas samotnych prstencu se prida k magnitude planety (odectenim hodnoty se vysledny jas zvysuje)
          // ===========================================================================================================
          break;

        case 6:                                               // Uran
          P_mag = -7.19 + (5 * log10(vzdal_SP * vzdal_ZP));
          break;

        case 7:                                               // Neptun
          P_mag = -6.87 + (5 * log10(vzdal_SP * vzdal_ZP));
          break;
      
        case 8:                                               // Pluto
          P_mag = -1 + (5 * log10(vzdal_SP * vzdal_ZP));
          break;

        default:                                              // vsechno ostatni se nastavi na nesmyslnou hodnotu
          P_mag = 99.99;
      
      }
    
  }





//-----------------------------------------------
//  Pro simulaci vypoctu polohy planet slunecni soustavy
//-----------------------------------------------
// Ke globalnim casovym promennym pro simulaci pricte 1 den.
//   Kdyz dojde k preteceni nejake promenne, nastavi se na minimalni hodnotu a zvysi se nasledujici polozka
void SIM_plus_den(void)
  {
    int16_t  pomdny_d[] = { 0,   31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31 };
  
    if ((SIM_rok % 4) == 0)     pomdny_d[2] = 29;             // pri prestupnem roku se nastavuje unor na 29 dni
    if (SIM_rok == 2100)        pomdny_d[2] = 28;             // rok 2100 bude specialita. I kdyz je deliteny 4, tak NEBUDE prestupny.

    SIM_den ++;
    if (SIM_den > pomdny_d[SIM_mes])                          // doslo k preteceni dni aktualniho mesice do dalsiho mesice
      {
        SIM_den = 1;
        SIM_mes ++;
        if (SIM_mes == 13)                                    // doslo k preteceni mesicu do nasledujiciho roku 
          {
            SIM_mes = 1;
            SIM_rok ++;
          }
      }
  }
    



//-----------------------------------------------
//  Simulace slunecni soustavy po prikazu "*s"
//  Vystupem je tabulka hodnot oddelena strednikem do seriove linky
//  Pri parametru planety = 0 se pocita specialni minutova simulace pro vychody a zapady Slunce a Mesice a pro soumraky
//  Vstupni parametry uz musi byt prekontrolovane
void simulace(uint8_t ss_planeta, uint16_t ss_pocet_kroku , uint16_t ss_den, uint16_t ss_mes , uint16_t ss_rok)
  {
    if (ss_planeta == 0)                                                   // pro index planety se zobrazuje simulace vychodu a zapadu Slunce a Mesice a soumraky 
      {
        sim_SM( ss_pocet_kroku , ss_den , ss_mes , ss_rok);
      }
    else
      {
        sim_SS(ss_planeta , ss_pocet_kroku , ss_den , ss_mes, ss_rok);  
      }

  }
//-----------------------------------------------

//-----------------------------------------------
//   Simulace planet slunecni soustavy (Ra, Dec, vzdalenosti, faze, fazovy uhel, elongace, magnitudy)
void sim_SS( uint16_t ss_planeta,  uint16_t ss_pocet_kroku , uint16_t ss_den, uint16_t ss_mes , uint16_t ss_rok)
  {
    char sss_uvozovky = ' ';
    char sss_desod = '.';
    char sss_oddelovac = ';';

    for (int16_t znaky = 0; znaky <244 ; znaky ++)
      {
        vystupni_retezec[znaky] = ' ';
      }
    
    SIM_den = ss_den;
    SIM_mes = ss_mes;
    SIM_rok = ss_rok;

    if (ss_pocet_kroku > 0)                                            // hlavicka se tiskne jen v pripade, ze nejedna o PC vystup
      {
        switch(oddelovac_polozek)                                      // pri vystupu simulace do seriove linky se oddelovac polozek prebira z nastaveni funkce #Oo
          {
            case 0:
              sss_oddelovac = ' ';
              break;
            case 1:
              sss_oddelovac = ',';
              break;
            case 2:
              sss_oddelovac = ';';
              break;
            case 3:
              sss_oddelovac = '\t';   
          } 
        sss_uvozovky = uvozovky;                                       // stejne tak i pripadne uzavirani hodnot do uvozovek se prebira z funkce #O"
        sss_desod = desod;                                             // oddelovac desetinnych mist se prebira z funkce #O,
        
        sss_hlavicka[12] = sss_oddelovac;                              // V hlavicce se nastavi spravne oddelovace polozek.
        sss_hlavicka[22] = sss_oddelovac;                              //      ( V hlavicce se ale nenastavuje uzavirani do uvozovek)       
        sss_hlavicka[30] = sss_oddelovac;
        sss_hlavicka[43] = sss_oddelovac;
        sss_hlavicka[49] = sss_oddelovac;
        sss_hlavicka[62] = sss_oddelovac;
        sss_hlavicka[76] = sss_oddelovac;
        sss_hlavicka[89] = sss_oddelovac;
        sss_hlavicka[102] = sss_oddelovac;
        sss_hlavicka[118] = sss_oddelovac;
        sss_hlavicka[134] = sss_oddelovac;
        sss_hlavicka[150] = sss_oddelovac;
        sss_hlavicka[164] = sss_oddelovac;
        sss_hlavicka[179] = sss_oddelovac;
        sss_hlavicka[202] = sss_oddelovac;
        sss_hlavicka[218] = sss_oddelovac;
        sss_hlavicka[243] = sss_oddelovac;
        Serial.println(sss_hlavicka);
 
      }
    else                                                               // parametr pro cykly bal nastaven na 0, takze se jedna o vystup pro PC program
      {
        ss_pocet_kroku = 1;                                            // vystup pro PC program ma parametr cyklu 0, tak se zmeni na 1, aby alespon 1 cyklus probehnul        
        sss_uvozovky = ' ';                                            // pri vystupu do PC programu se nikdy polozky do uvozovek neuzaviraji
        sss_oddelovac = ';';                                           // oddelovac polozek je vzdycky strednik
        sss_desod = '.';                                               // vystup pro PC program pouziva vzdycky jako desetinny oddelovac tecku
      }
    
    for (uint16_t dni = 1; dni <= ss_pocet_kroku ; dni++)          // vypis dat tabulky, kazdy den na novy radek    
      {
        vypocty_elongace(SIM_rok,SIM_mes,SIM_den, 12 ,0, ss_planeta);  // vypocty pro 12:00 UT. Vsechny hodnoty se vraci v globalnich promennych

        formatuj_ulong(SIM_den ,   2, 2);                     // den           
        vystupni_retezec[3] = '.';                            // tecka za dnem
        formatuj_ulong(SIM_mes ,   5, 2);                     // mesic
        vystupni_retezec[6] = '.';                            // tecka za mesicem
        formatuj_ulong(SIM_rok ,  10, 4);                     // rok
      
        formatuj_string(planety[ss_planeta] , 14, 7);         // nazev planety
        formatuj_ulong(ss_planeta ,  26, 1);                  // index planety
      
        formatuj_float(P_elongace , 31,  34, 3 , 6);          // elongace
        vystupni_retezec[31] = ' ';                           // pripadne znamenko se hned maze
        
        if (P_east_elo == 0) vystupni_retezec[46] = 'W';      // zapadni / vychodni elongace
        if (P_east_elo == 1) vystupni_retezec[46] = 'E';
        if (P_east_elo == 2) vystupni_retezec[46] = '?';
      
        formatuj_float(P_ra , 51,  53, 3 , 6);                // rektascenze planety
        vystupni_retezec[50] = ' ';                           // pripadne znamenko se hned maze
      
        formatuj_float(P_de , 64,  66, 2 , 6);                // deklinace planety vcetne znamenka
          
      
        formatuj_float(S_ra , 77,  80, 3 , 6);                // rektascenze Slunce
        vystupni_retezec[77] = ' ';                           // pripadne znamenko se hned maze
      
        formatuj_float(S_de , 91,  93, 2 , 6);                // deklinace Slunce vcetne znamenka
      
        formatuj_float(P_vzdalenost , 105,  107, 2 , 6);      // vzdalenost planeta - Zeme
        vystupni_retezec[105] = ' ';                          // pripadne znamenko se hned maze
      
        formatuj_float(dist_SP , 121,  123, 2 , 6);           // vzdalenost Slunce - planeta
        vystupni_retezec[121] = ' ';                          // pripadne znamenko se hned maze
      
        formatuj_float(dist_ZS , 138,  139, 1 , 6);           // vzdalenost Zeme - Slunce
        vystupni_retezec[138] = ' ';                          // pripadne znamenko se hned maze
      
        formatuj_float(fazovy_uhel , 152,  155, 3 , 6);       // fazovy uhel (Zeme-planeta-Slunce)
        vystupni_retezec[152] = ' ';                          // pripadne znamenko se hned maze
      
        formatuj_float(P_faze , 167,  168, 1 , 6);            // osvetlena faze planety
        vystupni_retezec[167] = ' ';                          // pripadne znamenko se hned maze

        if (ss_planeta == 5)                                  // jen pro Saturn
          {
            formatuj_float(P_mag_SAT , 185,  187, 2 , 6);     // magnituda samotne planety bez prstence
            formatuj_float(R_mag     , 205,  207, 2 , 6);     // magnituda samotnych prstencu
          }
        else                                                  // pro ostatni planety mimo Saturnu
          {
            formatuj_float(P_mag     , 185,  187, 2 , 6);     // magnituda samotne planety bez prstence je stejna jako celkova magnituda planety
            formatuj_string("------------" , 205, 10);        // magnituda prstencu se proskrtne
          }
        
        formatuj_float(P_mag     , 224,  226, 2 , 6);         // celkova magnituda planety (u Satrnu soucet magnitud planety a prstencu)
      
        vystupni_retezec[0]  = sss_uvozovky;
        vystupni_retezec[11] = sss_uvozovky;
        vystupni_retezec[13] = sss_uvozovky;
        vystupni_retezec[21] = sss_uvozovky;
        vystupni_retezec[25] = sss_uvozovky;
        vystupni_retezec[27] = sss_uvozovky;
        
        vystupni_retezec[31] = sss_uvozovky;
        vystupni_retezec[42] = sss_uvozovky;
        vystupni_retezec[45] = sss_uvozovky;
        vystupni_retezec[47] = sss_uvozovky;
        vystupni_retezec[50] = sss_uvozovky;
        vystupni_retezec[61] = sss_uvozovky;
        vystupni_retezec[63] = sss_uvozovky;
        vystupni_retezec[74] = sss_uvozovky;
        vystupni_retezec[77] = sss_uvozovky;
        vystupni_retezec[88] = sss_uvozovky;
        vystupni_retezec[90] = sss_uvozovky;
        vystupni_retezec[101] = sss_uvozovky;
        vystupni_retezec[105] = sss_uvozovky;
        vystupni_retezec[115] = sss_uvozovky;
        vystupni_retezec[121] = sss_uvozovky;
        vystupni_retezec[131] = sss_uvozovky;
        vystupni_retezec[138] = sss_uvozovky;
        vystupni_retezec[147] = sss_uvozovky;
        vystupni_retezec[152] = sss_uvozovky;
        vystupni_retezec[163] = sss_uvozovky;
        vystupni_retezec[167] = sss_uvozovky;
        vystupni_retezec[176] = sss_uvozovky;
        vystupni_retezec[184] = sss_uvozovky;
        vystupni_retezec[195] = sss_uvozovky;
        vystupni_retezec[204] = sss_uvozovky;         // pro Saturn
        vystupni_retezec[215] = sss_uvozovky;
        vystupni_retezec[223] = sss_uvozovky;
        vystupni_retezec[234] = sss_uvozovky;

        
        vystupni_retezec[12] = sss_oddelovac;
        vystupni_retezec[22] = sss_oddelovac;
        vystupni_retezec[30] = sss_oddelovac;
        
        vystupni_retezec[43] = sss_oddelovac;
        vystupni_retezec[49] = sss_oddelovac;
        vystupni_retezec[62] = sss_oddelovac;
        vystupni_retezec[76] = sss_oddelovac;
        vystupni_retezec[89] = sss_oddelovac;
        vystupni_retezec[102] = sss_oddelovac;
        vystupni_retezec[118] = sss_oddelovac;
        vystupni_retezec[134] = sss_oddelovac;
        vystupni_retezec[150] = sss_oddelovac;
        vystupni_retezec[164] = sss_oddelovac;
        vystupni_retezec[179] = sss_oddelovac;
        vystupni_retezec[202] = sss_oddelovac;
        vystupni_retezec[218] = sss_oddelovac;
        vystupni_retezec[243] = sss_oddelovac;    

        vystupni_retezec[35]  = sss_desod;                               // desetinny oddelovac pro elongaci
        vystupni_retezec[54]  = sss_desod;                               // desetinny oddelovac pro RA planety
        vystupni_retezec[67]  = sss_desod;                               // desetinny oddelovac pro DEC planety
        vystupni_retezec[81]  = sss_desod;                               // desetinny oddelovac pro RA Slunce
        vystupni_retezec[94]  = sss_desod;                               // desetinny oddelovac pro DEC Slunce
        vystupni_retezec[108] = sss_desod;                               // desetinny oddelovac pro vzdalenost Planeta - Zeme
        vystupni_retezec[124] = sss_desod;                               // desetinny oddelovac pro Slunce - Planeta
        vystupni_retezec[140] = sss_desod;                               // desetinny oddelovac pro Zeme - Slunce
        vystupni_retezec[156] = sss_desod;                               // desetinny oddelovac pro fazovy uhel
        vystupni_retezec[169] = sss_desod;                               // desetinny oddelovac pro osvetlenou fazi planety
        vystupni_retezec[188] = sss_desod;                               // desetinny oddelovac pro magnitudu samotne planety
        if (ss_planeta == 5)  vystupni_retezec[208] = sss_desod;         // pro Saturn se do magnitudy prstencu nastavuje desetinny oddelovac (ostatni planety maji bunku preskrtnutou a oddelovac se neziskne)
        vystupni_retezec[227] = sss_desod;                               // desetinny oddelovac pro magnitudu planety a prstencu dohromady
        
        vystupni_retezec[244] = '\0';

        Serial.println (vystupni_retezec);


        if (dni < ss_pocet_kroku)                                        // predcasne ukonceni je mozne jen v pripade, ze se nejedna o posledni smycku
          {                                                              //        (tim se vylouci nechtene predcasne ukonceni pro PC funkci, kde se vraci jen jedna radka)
            if (digitalRead(pin_tl_ok) == LOW or Serial.available())     // moznost preruseni dlouheho vypoctu pomoci tlacitka OK, nebo odeslanim ibovolneho znaku do seriove linky
              {
                Serial.println("\n ... break\n");
                delay(10);
                while (Serial.available()) Serial.read();                // vymazani dat, ktere zpusobily preruseni, ze serioveho bufferu
                break;                                                   // vypadnuti ze smycky for...
              }
            
          }


        
        SIM_plus_den();
      }         
  }



//-----------------------------------------------
//   Simulace Slunce, Mesice a soumraku (vychody a zapady, maximalni elevace, vzdaenosti, faze Mesice)
void sim_SM( uint16_t ss_pocet_kroku , uint16_t ss_den, uint16_t ss_mes , uint16_t ss_rok)
  {

    char sss_uvozovky = ' ';
    char sss_desod = '.';
    char sss_oddelovac = ';';

    for (int16_t znaky = 0; znaky <267 ; znaky ++)
      {
        vystupni_retezec[znaky] = ' ';
      }

    if (ss_pocet_kroku > 0)                                            // hlavicka se tiskne jen v pripade, ze nejedna o PC vystup
      {
        switch(oddelovac_polozek)                                      // pri vystupu simulace do seriove linky se oddelovac polozek prebira z nastaveni funkce #Oo
          {
            case 0:
              sss_oddelovac = ' ';
              break;
            case 1:
              sss_oddelovac = ',';
              break;
            case 2:
              sss_oddelovac = ';';
              break;
            case 3:
              sss_oddelovac = '\t';   
          } 
        sss_uvozovky = uvozovky;                                       // stejne tak i pripadne uzavirani hodnot do uvozovek se prebira z funkce #O"
        sss_desod = desod;                                             // oddelovac desetinnych mist se prebira z funkce #O,
        
        ssm_hlavicka[12] = sss_oddelovac;                              // V hlavicce se nastavi spravne oddelovace polozek.
        ssm_hlavicka[25] = sss_oddelovac;                              //      ( V hlavicce se ale nenastavuje uzavirani do uvozovek)
        ssm_hlavicka[34] = sss_oddelovac;
        ssm_hlavicka[45] = sss_oddelovac;
        ssm_hlavicka[58] = sss_oddelovac;
        ssm_hlavicka[69] = sss_oddelovac;
        ssm_hlavicka[82] = sss_oddelovac;
        ssm_hlavicka[95] = sss_oddelovac;
        ssm_hlavicka[104] = sss_oddelovac;
        ssm_hlavicka[121] = sss_oddelovac;
        ssm_hlavicka[131] = sss_oddelovac;
        ssm_hlavicka[153] = sss_oddelovac;
        ssm_hlavicka[167] = sss_oddelovac;
        ssm_hlavicka[176] = sss_oddelovac;
        ssm_hlavicka[189] = sss_oddelovac;
        ssm_hlavicka[198] = sss_oddelovac;
        ssm_hlavicka[217] = sss_oddelovac;
        ssm_hlavicka[227] = sss_oddelovac;
        ssm_hlavicka[250] = sss_oddelovac;
        ssm_hlavicka[262] = sss_oddelovac;
        Serial.println(ssm_hlavicka);
 
      }
    else
      {
        ss_pocet_kroku = 1;                                            // vystup pro PC program ma parametr cyklu 0, tak se zmeni na 1, aby alespon 1 cyklus probehnul        
        sss_uvozovky = ' ';                                            // pri vystupu do PC programu se nikdy polozky do uvozovek neuzaviraji
        sss_oddelovac = ';';                                           // oddelovac polozek je vzdycky strednik
        sss_desod = '.';                                               // vystup pro PC program pouziva vzdycky jako desetinny oddelovac tecku
      }

    
    SIM_rok = ss_rok;                                                  // pocatecni datum pro simulaci
    SIM_mes = ss_mes;
    SIM_den = ss_den;
    SIM_break = false;                                                 // zrusi pripadne predchozio breaknuti vypisu

    for (uint16_t dni = 1; dni <= ss_pocet_kroku ; dni++)          // vypis dat tabulky, kazdy den na novy radek  
      {
        SIM_hod = 12;                                                  // vypocty probihaji od poledne nastaveneho dne a trvaji 24 hodin (do poledne nasledujiciho dne)
        SIM_min = 0;
        d1_simulace_SM(SIM_rok, SIM_mes, SIM_den);


        if (SIM_break == true and dni < ss_pocet_kroku)                // v minutovem simulacnim podprogramu 'd1_simulace_SM()' doslo k predcasnemu ukonceni vypisu
          {                                                            //         preruseni je ale mozne jen v pripade, ze se nejedna o posledni simulacni den
            Serial.println("\n ... break\n");
            break;                                                     // vypadnuti ze smycky for...
          }
        vystupni_retezec[0]  = sss_uvozovky;
        vystupni_retezec[11] = sss_uvozovky;
        vystupni_retezec[16] = sss_uvozovky;
        vystupni_retezec[22] = sss_uvozovky;
        vystupni_retezec[28] = sss_uvozovky;
        vystupni_retezec[32] = sss_uvozovky;
        vystupni_retezec[36] = sss_uvozovky;
        vystupni_retezec[42] = sss_uvozovky;
        vystupni_retezec[48] = sss_uvozovky;
        vystupni_retezec[54] = sss_uvozovky;
        vystupni_retezec[60] = sss_uvozovky;
        vystupni_retezec[66] = sss_uvozovky;
        vystupni_retezec[72] = sss_uvozovky;
        vystupni_retezec[78] = sss_uvozovky;
        vystupni_retezec[85] = sss_uvozovky;
        vystupni_retezec[91] = sss_uvozovky;
        vystupni_retezec[97] = sss_uvozovky;
        vystupni_retezec[110] = sss_uvozovky;
        vystupni_retezec[116] = sss_uvozovky;
        vystupni_retezec[125] = sss_uvozovky;
        vystupni_retezec[128] = sss_uvozovky;
        vystupni_retezec[138] = sss_uvozovky;
        vystupni_retezec[147] = sss_uvozovky;
        vystupni_retezec[157] = sss_uvozovky;
        vystupni_retezec[163] = sss_uvozovky;
        vystupni_retezec[170] = sss_uvozovky;
        vystupni_retezec[174] = sss_uvozovky;
        vystupni_retezec[179] = sss_uvozovky;
        vystupni_retezec[185] = sss_uvozovky;
        vystupni_retezec[192] = sss_uvozovky;
        vystupni_retezec[196] = sss_uvozovky;
        vystupni_retezec[204] = sss_uvozovky;
        vystupni_retezec[210] = sss_uvozovky;
        vystupni_retezec[221] = sss_uvozovky;
        vystupni_retezec[224] = sss_uvozovky;
        vystupni_retezec[235] = sss_uvozovky;
        vystupni_retezec[242] = sss_uvozovky;
        vystupni_retezec[254] = sss_uvozovky;
        vystupni_retezec[258] = sss_uvozovky;

        vystupni_retezec[12] = sss_oddelovac;                          // spravne oddelovace polozek se nastavi i ve vystupnim retezci
        vystupni_retezec[25] = sss_oddelovac;
        vystupni_retezec[34] = sss_oddelovac;
        vystupni_retezec[45] = sss_oddelovac;
        vystupni_retezec[58] = sss_oddelovac;
        vystupni_retezec[69] = sss_oddelovac;
        vystupni_retezec[82] = sss_oddelovac;
        vystupni_retezec[95] = sss_oddelovac;
        vystupni_retezec[104] = sss_oddelovac;
        vystupni_retezec[121] = sss_oddelovac;
        vystupni_retezec[131] = sss_oddelovac;
        vystupni_retezec[153] = sss_oddelovac;
        vystupni_retezec[167] = sss_oddelovac;
        vystupni_retezec[176] = sss_oddelovac;
        vystupni_retezec[189] = sss_oddelovac;
        vystupni_retezec[198] = sss_oddelovac;
        vystupni_retezec[217] = sss_oddelovac;
        vystupni_retezec[227] = sss_oddelovac;
        vystupni_retezec[250] = sss_oddelovac;
        vystupni_retezec[262] = sss_oddelovac;
           
        vystupni_retezec[140]  = sss_desod;                            // desetinny oddelovac pro vzdalenost Slunce v AU
        
        vystupni_retezec[263]  = '\0';                                 // zakonceni retezce
        Serial.println (vystupni_retezec);
      }

    if (SIM_break == true) 
      {
        delay(10);
        while (Serial.available()) Serial.read();                      // vymazani dat, ktere zpusobily preruseni, ze serioveho bufferu
      }  
  
  }





//-----------------------------------------------
// jednodenni simulace Slunce a Mesice
// vyzaduje korektni globalni promenne 'casova_zona', 'GeoLon', 'GeoLat'
void d1_simulace_SM(int16_t ss_rok, int16_t ss_mes, int16_t ss_den)
  {
    
    int16_t   SIM_slu_vy_t    = 9999;       // vychod Slunce v minutach od poledne zadaneho dne
    int16_t   SIM_slu_za_t    = 9999;       // zapad Slunce v minutach od poledne zadaneho dne
    float SIM_slu_vy_a    =  999;       // azimut vychodu Slunce
    float SIM_slu_za_a    =  999;       // azimut zapadu Slunce
    float SIM_slu_maxel   = -999;       // maximalni elevace Slunce
    int16_t   SIM_slu_maxel_t = 9999;       // cas maximalni elevace Slunce v minutach od poledne zadaneho dne
    float SIM_slu_vzd     =    0;       // vzdalenost Slunce v [AU] v poledne simulovaneho dne

    int16_t   SIM_mes_vy_t    = 9999;       // vychod Mesice v minutach od poledne zadaneho dne
    int16_t   SIM_mes_za_t    = 9999;       // zapad Mesice v minutach od poledne zadaneho dne
    float SIM_mes_vy_a    =  999;       // azimut vychodu Mesice
    float SIM_mes_za_a    =  999;       // azimut zapadu Mesice
    float SIM_mes_maxel   = -999;       // maximalni elevace Mesice
    int16_t   SIM_mes_maxel_t = 9999;       // cas maximalni elevace Mesice v minutach od poledne zadaneho dne
    float SIM_mes_vzd     =    0;       // vzdalenost Mesice v tisicich km v poledne simulovaneho dne



    int16_t   SIM_asou_za_t   = 9999;       // zacatek astronomickeho soumraku v minutach od poledne zadaneho dne
    int16_t   SIM_asou_ko_t   = 9999;       // konec astronomickeho soumraku v minutach od poledne zadaneho dne

    int16_t   SIM_dsou_za_t   = 9999;       // zacatek definovaneho soumraku v minutach od pulnoci (Slunce je pod horizontem takovou elevaci, ktera je nastavena v 'elevace_soumrak')
    int16_t   SIM_dsou_ko_t   = 9999;       // konec definovaneho soumraku v minutach od pulnoci


    double korekce_horizontu = -0.833;                              // vychod / zapad horniho okraje Slunce je o polovinu uhloveho prumeru pod horizontem (je tam i nejaka korekce refrakce)

    bool nad_pod = true;                                            // Slunce je prave nad/pod horizontem
    bool nad_pod_as = true;                                         // Slunce je prave nad/pod hranici astronomickeho soumraku
    bool nad_pod_ds = true;                                         // Slunce je prave nad/pod hranici definovatelneho soumraku
    bool nad_pod_ms = true;                                         // Mesic je prave nad/pod horizontem

    int16_t plusmin;


    if (leto == true) casova_zona = letni_posun;
    else              casova_zona = zimni_posun;

    for (plusmin = 0; plusmin < 1440 ; plusmin ++)                  // smycka pro simulaci 24 hodin od poledne po kroku 1 minuty
      {
        double days = day2000(SIM_rok, SIM_mes, SIM_den, SIM_hod + SIM_min/60.0);  
        double t = days / 36525.0;
         
        //  Data Slunce
        double L1   = rozsah(280.466 + 36000.8 * t);
        double M1   = rozsah(357.529+35999*t - 0.0001536* t*t + t*t*t/24490000.0);
        double C1   = (1.915 - 0.004817* t - 0.000014* t * t)* dsin(M1);   
               C1   = C1 + (0.01999 - 0.000101 * t)* dsin(2*M1);
               C1   = C1 + 0.00029 * dsin(3*M1);
        double V1   = M1 + C1;
        double EC1  = 0.01671 - 0.00004204 * t - 0.0000001236 * t*t;
        double R1   = 0.99972 / (1 + EC1 * dcos(V1));
        
        double Th1  = L1 + C1;
        double OM1  = rozsah(125.04 - 1934.1 * t);
        double LaM1 = Th1 - 0.00569 - 0.00478 * dsin(OM1);
        double Obl  = (84381.448 - 46.815 * t)/3600;
        double Ra1  = datan2(dsin(Th1) * dcos(Obl) - dtan(0)* dsin(Obl), dcos(Th1));
        double DEC1 = dasin(dsin(0)* dcos(Obl) + dcos(0)*dsin(Obl)*dsin(Th1));   
        RaToAzim( 3 , Ra1 / 15, DEC1, SIM_rok, SIM_mes, SIM_den , SIM_hod, SIM_min , casova_zona, GeoLon, GeoLat);  // vypocte azimut a elevaci Slunce pro nastaveny den a cas


        //  Data Mesic
        double F    = rozsah(93.2721 + 483202 * t - 0.003403 * t* t - t * t * t/3526000.0);
        double L2   = rozsah(218.316 + 481268 * t);
        double Om2  = rozsah(125.045 - 1934.14 * t + 0.002071 * t * t + t * t * t/450000.0);
        double M2   = rozsah(134.963 + 477199 * t + 0.008997 * t * t + t * t * t/69700.0);
        double D    = rozsah(297.85 + 445267 * t - 0.00163 * t * t + t * t * t/545900.0);
        double D2   = 2*D;
        double R2 = 1 + (-20954 * dcos(M2) - 3699 * dcos(D2 - M2) - 2956 * dcos(D2)) / 385000.0;
        double r2km   = R2 * 385000.0;
        double R3   = (R2 / R1) / 379.168831168831;
        double Bm   = 5.128 * dsin(F) + 0.2806 * dsin(M2 + F);
               Bm   = Bm + 0.2777 * dsin(M2 - F) + 0.1732 * dsin(D2 - F);
        double Lm   = 6.289 * dsin(M2) + 1.274 * dsin(D2 -M2) + 0.6583 * dsin(D2); 
               Lm   = Lm + 0.2136 * dsin(2*M2) - 0.1851 * dsin(M1) - 0.1143 * dsin(2 * F); 
               Lm   = Lm +0.0588 * dsin(D2 - 2*M2) ;
               Lm   = Lm + 0.0572* dsin(D2 - M1 - M2) + 0.0533* dsin(D2 + M2);
               Lm   = Lm + L2;


        double Ra2  = (datan2(dsin(Lm) * dcos(Obl) - dtan(Bm)* dsin(Obl), dcos(Lm)));
        double Dec2 = dasin(dsin(Bm)* dcos(Obl) + dcos(Bm)*dsin(Obl)*dsin(Lm));

        double HLm  = rozsah(LaM1 + 180 + (180/PI) * R3 * dcos(Bm) * dsin(LaM1 - Lm));
        double HBm  = R3 * Bm;
    
        double I    = 1.54242;
        double W    = Lm - Om2;
        double qY   = dcos(W) * dcos(Bm);
        double qX   = dsin(W) * dcos(Bm) * dcos(I) - dsin(Bm) * dsin(I);
        double A    = datan2(qX, qY);
    //    double EL   = A - F;
    //    double EB   = dasin(-dsin(W) * dcos(Bm) * dsin(I) - dsin(Bm) * dcos(I));
              
               W    = rozsah(HLm - Om2);
               qY   = dcos(W) * dcos(HBm);
               qX   = dsin(W) * dcos(HBm) * dcos(I) - dsin(HBm) * dsin(I);
               A    = datan2(qX, qY);
    //    double SL   = rozsah(A - F);
    //    double SB   = dasin(-dsin(W) * dcos(HBm) * dsin(I) - dsin(HBm) * dcos(I));
    //    double Co   = 0;
    //           if (SL < 90)  Co = 90 - SL;
    //           else          Co = 450 - SL;
    
               A    = dcos(Bm) * dcos(Lm - LaM1);
        double Psi  = 90 - datan(A / sqrt(1- A*A));
               qX   = R1 * dsin(Psi);
               qY   = R3 - R1* A;
        double Il   = datan2(qX, qY);
        double K    = (1 + dcos(Il))/2;            
       
        RaToAzim( 2 , Ra2 / 15, Dec2, SIM_rok, SIM_mes, SIM_den , SIM_hod, SIM_min , casova_zona, GeoLon, GeoLat);  // vypocte azimut a elevaci Mesic pro nastaveny den a cas                     

        if (plusmin == 720)                                                 // osvetleni Mesice a jeho vzdalenost se pocita k pulnoci po zadanem datumu
          {
            Mes_osvit   = zaokrouhlit(K*100);
            SIM_mes_vzd = r2km;                                            // vzdalenost Mesice v km            
          }



        if (plusmin == 0)                                                   // prvni smycka testuje, jestli je v poledne (astronomicky) soumrak, nebo Slunce pod horizontem
          {
            if (obecna_elevace < -18)                  nad_pod_as = false;
            else                                       nad_pod_as = true;   // v poledne (v prvnim kroku) by za normalnich okolnosti nemel nastat astronomicky soumrak

            if (obecna_elevace < -elevace_soumrak)     nad_pod_ds = false;
            else                                       nad_pod_ds = true;   // v poledne (v prvnim kroku) by za normalnich okolnosti nemel nastat definovatelny soumrak

            if (obecna_elevace < -korekce_horizontu)   nad_pod = false;
            else                                       nad_pod = true;      // v poledne (v prvnim kroku) by za normalnich okolnosti melo byt Slunce nad horizontem

            if (Mes_elevace < -korekce_horizontu)      nad_pod_ms = false;  // v poledne (v prvnim kroku) se zjisti, jestli je Mesic nad nebo pod horizontem
            else                                       nad_pod_ms = true;      


            SIM_slu_vzd = R1;                                               // v poledne zadaneho dne se zaznamena vzdalenost Slunce od Zeme
          
          }

        if (obecna_elevace > SIM_slu_maxel)                                 // zaznamenana prubezne maximalni elevevace Slunce
          {
            SIM_slu_maxel = obecna_elevace;
            SIM_slu_maxel_t = plusmin;
          }

        if (nad_pod_ms == false and Mes_elevace > korekce_horizontu)        // horni okraj Mesice prave vysel nad horizont
          {
            SIM_mes_vy_t = plusmin;
            SIM_mes_vy_a = Mes_azimut;
            nad_pod_ms = true;
          }

        if (nad_pod_ms == true and Mes_elevace < korekce_horizontu)         // horni okraj Slunce prave zapadnul pod horizont
          {
            SIM_mes_za_t = plusmin;
            SIM_mes_za_a = Mes_azimut;
            nad_pod_ms = false;
          }


        if (Mes_elevace > SIM_mes_maxel)                                    // zaznamenana prubezne maximalni elevevace Mesice
          {
            SIM_mes_maxel = Mes_elevace;
            SIM_mes_maxel_t = plusmin;
          }



        if (nad_pod == false and obecna_elevace > korekce_horizontu)        // horni okraj Slunce prave vysel nad horizont
          {
            SIM_slu_vy_t = plusmin;
            SIM_slu_vy_a = obecny_azimut;
            nad_pod = true;
          }

        if (nad_pod == true and obecna_elevace < korekce_horizontu)         // horni okraj Slunce prave zapadnul pod horizont
          {
            SIM_slu_za_t = plusmin;
            SIM_slu_za_a = obecny_azimut;
            nad_pod = false;
          }

        if (nad_pod_as == false and obecna_elevace > -18)                   // stred Slunce prave vysel nad hranici -18 stupnu (skoncila astronomicka noc)
          {
            SIM_asou_ko_t = plusmin;
            nad_pod_as = true;
          }

        if (nad_pod_as == true and obecna_elevace < -18)                    // stred Slunce prave klesl pod hranici -18 stupnu (zacala astronomicka noc)
          {
            SIM_asou_za_t = plusmin;
            nad_pod_as = false;
          }


        if (nad_pod_ds == false and obecna_elevace > -elevace_soumrak)      // stred Slunce prave vysel nad hranici definovatelneho soumraku
          {
            SIM_dsou_ko_t = plusmin;
            nad_pod_ds = true;
          }

        if (nad_pod_ds == true and obecna_elevace < -elevace_soumrak)       // stred Slunce prave klesl pod hranici definovatelneho soumraku
          {
            SIM_dsou_za_t = plusmin;
            nad_pod_ds = false;
          }

        if (digitalRead(pin_tl_ok) == LOW or Serial.available())     // moznost preruseni dlouheho vypoctu pomoci tlacitka OK, nebo odeslanim ibovolneho znaku do seriove linky
          {
            SIM_break = true;                                        // prerusi se i nadrazena smycka pro dny
            break;                                                   // vypadnuti ze smycky for...
          }
        

            
        SIM_plus_minuta();                                           // simulacni promenne se posunou o jednu minutu dal (s kontrolou preteceni minut, hodin, dni a mesicu)
      }
    
    formatuj_ulong(ss_den ,   2, 2);                      // den           
    vystupni_retezec[3] = '.';                            // tecka za dnem
    formatuj_ulong(ss_mes ,   5, 2);                      // mesic
    vystupni_retezec[6] = '.';                            // tecka za mesicem
    formatuj_ulong(ss_rok ,  10, 4);                      // rok

    min_na_hm(17,SIM_slu_za_t);                           // zapad Slunce (cas)
    formatuj_ulong(SIM_slu_za_a + 0.5 ,  31, 3);          // zapad Slunce (azimut)
  
    min_na_hm(37, SIM_asou_za_t);                         // konec astronomickeho soumraku, zacatek astronomicke noci
    min_na_hm(49, SIM_dsou_za_t);                         // zacatek definovatelneho soumraku

    min_na_hm(61, SIM_dsou_ko_t);                         // konec definovatelneho soumraku
    min_na_hm(73, SIM_asou_ko_t);                         // zacatek astronomickeho soumraku, konec astronomicke noci

    min_na_hm(86, SIM_slu_vy_t);                          // vychod Slunce (cas)
    formatuj_ulong(SIM_slu_vy_a + 0.5 , 100, 3);          // vychod Slunce (azimut)

    min_na_hm(111,SIM_slu_maxel_t);                       // maximalni elevace Slunce (cas)
    formatuj_ulong(SIM_slu_maxel + 0.5 , 127, 2);         // maximalni elevace Slunce (zaokrouhlena elevace)

    formatuj_float(SIM_slu_vzd , 138,  139, 1 , 6);       // vzdalenost Slunce v poledne (v prvnim kroku simulace)

    min_na_hm(158, SIM_mes_vy_t);                         // vychod Mesice (cas)
    formatuj_ulong(SIM_mes_vy_a + 0.5 , 173, 3);          // vychod Mesice (azimut)

    min_na_hm(180, SIM_mes_za_t);                         // zapad Mesice (cas)
    formatuj_ulong(SIM_mes_za_a + 0.5 , 195, 3);          // zapad Mesice (azimut)

    min_na_hm(205, SIM_mes_maxel_t);                      // maximalni elevace Mesice (cas)
    formatuj_ulong(SIM_mes_maxel + 0.5 , 223, 2);         // maximalni elevace Mesice (elevace)

    formatuj_ulong(SIM_mes_vzd + 0.5 , 241, 6);           // vzdalenost Mesice [km]
    
    formatuj_ulong(Mes_osvit , 257, 3);                   // osvetleni Mesice [%]


  
  }
//-----------------------------------------------



void min_na_hm (int16_t pozice_dhod, int16_t vstupni_minuty)
  {
    if (vstupni_minuty < 9999)
      {        
        vstupni_minuty = vstupni_minuty - 720;              // odecist poledne (12 hodin)
        
        if (vstupni_minuty < 0) vstupni_minuty = vstupni_minuty + 1440;      // kdyz k udalosti dojde pred polednem, znamenalo by to zaporny cas, proto pricist 1 den
        int16_t hodiny = vstupni_minuty / 60;
        int16_t minuty = vstupni_minuty % 60;

        vystupni_retezec[pozice_dhod] = (hodiny/10) + 48;
        vystupni_retezec[pozice_dhod + 1] = (hodiny%10) + 48;       
        if(hodiny < 10)  vystupni_retezec[pozice_dhod] = ' ';
        vystupni_retezec[pozice_dhod + 2] = ':';
        vystupni_retezec[pozice_dhod + 3] = (minuty/10) + 48;
        vystupni_retezec[pozice_dhod + 4] = (minuty%10) + 48;       
      }
    else
      {
        vystupni_retezec[pozice_dhod]     = '-';
        vystupni_retezec[pozice_dhod + 1] = '-';
        vystupni_retezec[pozice_dhod + 2] = '-';
        vystupni_retezec[pozice_dhod + 3] = '-';
        vystupni_retezec[pozice_dhod + 4] = '-';
      }
  }
