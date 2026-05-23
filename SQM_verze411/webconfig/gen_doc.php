<HTML>
<HEAD>
<TITLE></TITLE>
<meta http-equiv="Content-Type" content="text/html">
<base target="_self">


<meta charset="UTF-8"> </head>

<BODY>


<?php

//---------------------------------------------------
// Prihlasovaci udaje pro MySQLi:
//  mysqli_connect($mysql_SERVER , $mysql_JMENO , $mysql_HESLO , $mysql_DATABAZE);
$mysql_SERVER   = "****server****";
$mysql_JMENO    = "****jmeno****";
$mysql_HESLO    = "****heslo****";
$mysql_DATABAZE = "****databaze****";
// databaze obsahuje tabulku: "SQM_osazovacka".
//           Obsah tabulky ve formatu SQL je pripojen jako soubor "SQM_osazovacka.sql"
//---------------------------------------------------


include("zip.lib.php");

// --------- podprogramy pro vytvoreni ZIP archivu ----------------
function get_files($dir)
{
	$handle=opendir($dir);
	$i=0;
	while (false!==($file = readdir($handle))) 
	{
		if ($file!="."&&$file!=".."&&!is_dir($file))
		{       
          if ($file!="parts")       // podlozka '/parts/' se vyhodnocuje jako soubor, proto se preskakuje
            {
              $files[$i]=$file;
              $i++;            
            }
		} 
	}
	closedir($handle); 
	return $files;
}

function pack_it($dir,$files,$names)
{
	$zip = new zipfile();
	
	if (count($files)!=count($names)) $names = $files;
	for($i=0;$i<count($files);$i++)
	{
		$fp=fopen($dir.$files[$i],"rb");
		$part_content=fread($fp,filesize($dir.$files[$i]));
		fclose($fp);
		
		$zip->addFile($part_content,$names[$i]);
               
//                unlink ($dir.$files[$i]);

	}
	$content=$zip->file();
	return $content;
}


function download_it($content,$zip_name)
{

$fp = fopen($zip_name, "w+");
fwrite($fp, $content);
fclose ($fp);

}
// ----------------------------------------------------------------





$cislo_verze = 0;
$varovani_1 = false;           // neni osazen modul BME280, ale je pozadovano rozsireni R30s
$varovani_2 = false;           // neni osazen ani displej ani RGB LED

$vstup_BCKUP    = $_POST['IN_BCKUP'];
$vstup_DIS      = isset($_POST['IN_DIS'])      ? 1 : 0;
$vstup_SD       = isset($_POST['IN_SD'])       ? 1 : 0;
$vstup_BME      = isset($_POST['IN_BME'])      ? 1 : 0;
$vstup_BZUK     = isset($_POST['IN_BZUK'])     ? 1 : 0;
$vstup_LED      = isset($_POST['IN_LED'])      ? 1 : 0;
$vstup_LSM303   = isset($_POST['IN_LSM303'])   ? 1 : 0;
$vstup_R30S     = isset($_POST['IN_R30S'])     ? 1 : 0;
$vstup_DS3231   = isset($_POST['IN_DS3231'])   ? 1 : 0;
$vstup_EXT      = isset($_POST['IN_EXT'])      ? 1 : 0;
$vstup_UREF     = isset($_POST['IN_UREF'])     ? 1 : 0;
$vstup_GPS      = isset($_POST['IN_GPS'])      ? 1 : 0;
$vstup_RS485    = isset($_POST['IN_RS485'])    ? 1 : 0;


$copy_BAT_top      = false;
$copy_BAT_bot      = false;
$copy_SUPCAP_top   = false;
$copy_SUPCAP_bot   = false;
$copy_DIS_top      = false;
$copy_DIS_bot      = false;
$copy_PCB_DIS_top  = false;
$copy_PCB_DIS_bot  = false;
$copy_SD_top       = false;
$copy_SD_bot       = false;
$copy_BME_top      = false;
$copy_BME_bot      = false;
$copy_BZUK_top     = false;
$copy_BZUK_bot     = false;
$copy_LED_top      = false;
$copy_LED_bot      = false;
$copy_LSM303_top   = false;
$copy_LSM303_bot   = false;
$copy_R30S_top     = false;
$copy_R30S_bot     = false;
$copy_DS3231_top   = false;
$copy_DS3231_bot   = false;
$copy_EXT_top      = false;
$copy_EXT_bot      = false;
$copy_UREF_top     = false;
$copy_UREF_bot     = false;
$copy_GPS_top      = false;
$copy_GPS_bot      = false;
$copy_PCB_GPS_top  = false;
$copy_PCB_GPS_bot  = false;
$copy_RS485_top    = false;
$copy_RS485s_bot   = false;
$copy_RS485bez_top = false;
$copy_RS485bez_bot = false;
$copy_RS485s_top   = false;
$copy_LSM303_bot   = false;






$predos = 1;                                  // 1= 0b001 = SQM-BAS
                                              // 3= 0b011 = SQM-BAS + SQM-DIS
                                              // 5= 0b101 = SQM-BAS + SQM-GPS
                                              // 7= 0b111 = SQM-BAS + SQM-DIS + SQM-GPS

$pouzity_blok = array(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

$pouzita_soucastka = array_fill(0, 150, false);

$popistypu = array("0=nezobrazuje se", "Integrované obvody", "Polovodiče",
                    "Optoelektronika" , "Odpory" , "Kondenzátory",
                    "Ostatní součástky", "Přepínače, tlačítka", "Konektory, držáky, patice", 
                    "Plošné spoje" , "Mechanika  (pro ruční verzi - nezohledňuje zvolená rozšíření)" , "Ostatní materiál");

$popisbloku = array("povinný základ desky SQM-BAS-7", "zálohování RTC baterií", "zálohování RTC superkapacitorem",
                    "rozšíření o displej" , "rozšíření o SD kartu" , "rozšíření o měření teploty, tlaku a vlhkosti",
                    "rozšíření o zvukovou signalizaci", "rozšíření o LED signalizaci" , "rozšíření o kompas a náklonoměr",
                    "rozšíření o režim 'r30s'", "rozšíření o možnost probuzení v nastaveném čase", "konektor pro externí periferie",
                    "rozšíření o měření napětí zdroje","rozšíření o GPS", "rozšíření o komunikaci RS485", "--",
                    "deska SQM-GPS-6","deska SQM-DIS-3","Mechanika a ostatní části (pro ruční verzi - nezohledňuje zvolená rozšíření)"," "," ");


$prg_config = "";

$top_osaz =  imagecreatefromgif('zaklad_top.gif');      // zaklad je pro vsechny varianty stejny
$bot_osaz =  imagecreatefromgif('zaklad_bot.gif');      // zaklad je pro vsechny varianty stejny

// podle zaskrtnutych bloku se pripoji dalsi bloky soucastek
if ($vstup_BCKUP == 'BAT')                              // nikdy nebude zaroven BAT a SUPCAP (ve vstupnim formu se navzajem prepinaji)
  {
    $cislo_verze = $cislo_verze + 1;
    $copy_BAT_top     = true;
    $pouzity_blok[1] = 1;
    $plus_img =  imagecreatefromgif('baterie_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
  }

if ($vstup_BCKUP == 'SUPCAP')                           // nikdy nebude zaroven BAT a SUPCAP (ve vstupnim formu se navzajem prepinaji)
  {
    $cislo_verze = $cislo_verze + 2;
    $copy_SUPCAP_top  = true;
    $copy_SUPCAP_bot  = true;
    $pouzity_blok[2] = 1;
    $plus_img =  imagecreatefromgif('supcap_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $plus_img =  imagecreatefromgif('supcap_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
  }


if ($vstup_DIS == 1)
  {
    $prg_config = $prg_config . "bool osazeno_DISPLEJ = true;      // pro zobrazeni je pouzity displej\n";
    $cislo_verze = $cislo_verze + 4;
    $copy_DIS_top      = true;
    $copy_PCB_DIS_top  = true;
    $copy_PCB_DIS_bot  = true;
    $pouzity_blok[3] = 1;
    $pouzity_blok[17] = 1;
    $plus_img =  imagecreatefromgif('displej_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $predos = $predos + 2;
  }
else
  {
    $prg_config = $prg_config . "bool osazeno_DISPLEJ = false;     // neni pouzity displej\n";
  }

if ($vstup_DIS == 0 && $vstup_LED == 0)
  {
    $varovani_2 = true;   // "POZOR: V teto variante neni zvolena zadna signalizace (ani displej ani RGB LED)."
  }

if ($vstup_SD == 1)
  {
    $prg_config = $prg_config . "bool osazeno_SD_card = true;      // je osazena SD karta\n";  
    $cislo_verze = $cislo_verze + 8;
    $copy_SD_top      = true;
    $copy_SD_bot      = true;
    $pouzity_blok[4] = 1;
    $plus_img =  imagecreatefromgif('sd_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $plus_img =  imagecreatefromgif('sd_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
  }
else
  {
    $prg_config = $prg_config . "bool osazeno_SD_card = false;     // neni osazena SD karta\n";
  }


if ($vstup_BME == 0 && $vstup_R30S == 1)
  {
    $varovani_1 = true;   // "Pro režim 'R30s' je nutné osadit modul BME280, který ale nebyl v předchozím formuláři zaškrtnutý. Došlo tedy k automatickému zaškrtnutí rozšíření BME280."
    $vstup_BME = 1;
  }


if ($vstup_BME == 1)
  {
    $prg_config = $prg_config . "bool osazeno_BME280  = true;      // je osazene cidlo pro mereni teploty, tlaku a vlhkosti\n";
    $cislo_verze = $cislo_verze + 16;
    $copy_BME_top      = true;
    $copy_BME_bot      = true;
    $pouzity_blok[5] = 1;
    $plus_img =  imagecreatefromgif('bme280_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $plus_img =  imagecreatefromgif('bme280_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
  }
else
  {
    $prg_config = $prg_config . "bool osazeno_BME280  = false;     // neni osazene cidlo pro mereni teploty, tlaku a vlhkosti\n";
  }

if ($vstup_BZUK == 1)
  {
    $cislo_verze = $cislo_verze + 32;
    $copy_BZUK_top     = true;
    $copy_BZUK_bot     = true;
    $pouzity_blok[6] = 1;
    $plus_img =  imagecreatefromgif('bzuk_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $plus_img =  imagecreatefromgif('bzuk_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
  }


if ($vstup_LED == 1)
  {
    $cislo_verze = $cislo_verze + 64;
    $copy_LED_top      = true;
    $pouzity_blok[7] = 1;
    $plus_img =  imagecreatefromgif('led_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
  }



if ($vstup_LSM303 == 1)
  {
    $prg_config = $prg_config . "bool osazeno_LSM303  = true;      // je osazeno cidlo naklonu s kompasem\n";
    $cislo_verze = $cislo_verze + 128;
    $copy_LSM303_top   = true;
    $pouzity_blok[8] = 1;
    $plus_img =  imagecreatefromgif('lsm303_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
  }
else
  {
    $prg_config = $prg_config . "bool osazeno_LSM303  = false;     // neni osazeno cidlo naklonu s kompasem\n";
  }


if ($vstup_R30S == 1)
  {
    $prg_config = $prg_config . "bool osazeno_r30s    = true;      // je osazena pridavna EEPROM pro rychle (30-sekundove) zaznamy teploty, tlaku a vlhkosti\n";  
    $cislo_verze = $cislo_verze + 256;
    $copy_R30S_bot     = true;
    $pouzity_blok[9] = 1;
    $plus_img =  imagecreatefromgif('r30s_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
  }
else
  {
    $prg_config = $prg_config . "bool osazeno_r30s    = false;     // neni osazena pridavna EEPROM pro rychle (30-sekundove) zaznamy teploty, tlaku a vlhkosti\n";
  }


if ($vstup_DS3231 == 1)
  {
    $prg_config = $prg_config . "bool osazeno_DS3231  = true;      // je osazen pridavny RTC obvod pro spousteni SQM i pri vypnutem napajeni\n";
    $cislo_verze = $cislo_verze + 512;
    $copy_DS3231_top   = true;
    $copy_DS3231_bot   = true;
    $pouzity_blok[10] = 1;
    $plus_img =  imagecreatefromgif('ds3231_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $plus_img =  imagecreatefromgif('ds3231_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
  }
else
  {
    $prg_config = $prg_config . "bool osazeno_DS3231  = false;     // neni osazen pridavny RTC obvod pro spousteni SQM i pri vypnutem napajeni\n";
  }

if ($vstup_EXT == 1)
  {
    $cislo_verze = $cislo_verze + 1024;
    $copy_EXT_top      = true;
    $pouzity_blok[11] = 1;
    $plus_img =  imagecreatefromgif('ext_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
  }


if ($vstup_UREF == 1)
  {
    $prg_config = $prg_config . "bool osazeno_uref    = true;      // jsou osazeny casti pro mereni napeti hlavniho zdroje\n";    $cislo_verze = $cislo_verze + 2048;
    $copy_UREF_top     = true;
    $copy_UREF_bot     = true;
    $pouzity_blok[12] = 1;
    $plus_img =  imagecreatefromgif('uref_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $plus_img =  imagecreatefromgif('uref_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
  }
else
  {
    $prg_config = $prg_config . "bool osazeno_uref    = false;     // nejsou osazeny casti pro mereni napeti hlavniho zdroje\n";
  }

if ($vstup_GPS == 1)
  {
    $prg_config = $prg_config . "bool osazeno_gps     = true;      // je osazena vnitrni deska SQM-GPS-6\n";  
    $cislo_verze = $cislo_verze + 4096;
    $copy_GPS_top      = true;
    $copy_GPS_bot      = true;
    $copy_PCB_GPS_top  = true;
    $copy_PCB_GPS_bot  = true;
    $pouzity_blok[13] = 1;
    $pouzity_blok[16] = 1;
    $plus_img =  imagecreatefromgif('gps_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $plus_img =  imagecreatefromgif('gps_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
    $predos = $predos + 4;
  }
else
  {
    $prg_config = $prg_config . "bool osazeno_gps     = false;     // neni osazena vnitrni deska SQM-GPS-6\n";
  }

if ($vstup_RS485 == 1 && $vstup_GPS == 0)               // bez osazene SQM-GPS-6 se osazuji zhora svorky
  {
    $cislo_verze = $cislo_verze + 8192;
    $copy_RS485bez_top = true;
    $copy_RS485bez_bot = true;
    $pouzity_blok[14] = 1;
    $pouzity_blok[15] = 1;
    $plus_img =  imagecreatefromgif('rs485_bezgps_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $plus_img =  imagecreatefromgif('rs485_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
  }

if ($vstup_RS485 == 1 && $vstup_GPS == 1)               // s osazenou SQM-GPS-6 se svorky neosazuji a komunikace a napajeni se pripojuje do SQM-GPS-6
  {
    $cislo_verze = $cislo_verze + 16384;
    $copy_RS485s_top   = true;
    $copy_RS485s_bot   = true;
    $pouzity_blok[14] = 1;
    $plus_img =  imagecreatefromgif('rs485_sgps_top.gif');
    imagecopymerge($top_osaz, $plus_img, 0, 0, 0, 0, 1040, 1500, 100);
    $plus_img =  imagecreatefromgif('rs485_bot.gif');
    imagecopymerge($bot_osaz, $plus_img, 0, 0, 0, 0, 860, 1460, 100);
  }


$pouzity_blok[18] = 1;               // ostatni mechanicke prvky (krabicka, sloupky, sroubky ...)

$hex_verze = dechex($cislo_verze);                            // verze bude vzdycky 5-znakovy hexa kod (doplnuji se uvodni nuly)
$pred_nuly = "";                                              // retezec s uvodnimi nulami
if ($cislo_verze < 0x10000) $pred_nuly = "0";                 // tahle nula by tu mela byt vzdycky - je to jen jako rezerva pro pripadne dalsi rozsirovani
if ($cislo_verze < 0x1000)  $pred_nuly = "00";
if ($cislo_verze < 0x100)   $pred_nuly = "000";
if ($cislo_verze < 0x10)    $pred_nuly = "0000";
$hex_verze = $pred_nuly . $hex_verze;


echo "<br><H2><br>Zvolená varianta: 0x" . strtoupper($hex_verze) . "</H2><br><br><br>";


// vytvoreni slozky a prekopirovani pouzitych grafickych souboru
if (file_exists("ver_" . $hex_verze))
  {
//    echo "(Slozka uz existuje)<br>";
  }
else
  {
// doplneni verze osazovacky do obrazku
    $verze_barva = imagecolorallocate($top_osaz, 0, 0, 0);
    imagestring($top_osaz, 5, 800, 10, "ver: 0x" . strtoupper($hex_verze) , $verze_barva);
    $verze_barva = imagecolorallocate($bot_osaz, 0, 0, 0);
    imagestring($bot_osaz, 5, 600, 10, "ver: 0x" . strtoupper($hex_verze) , $verze_barva);

//    echo "Vytvarim novou slozku<br>";
    mkdir("ver_" . $hex_verze, 0777);
    chmod("ver_" . $hex_verze, 0777);

    mkdir("ver_" . $hex_verze . "/parts", 0777);
    chmod("ver_" . $hex_verze . "/parts", 0777);


    imagegif($top_osaz, "ver_" . $hex_verze . "/+sqm-bas-7_top.gif");
    imagegif($bot_osaz, "ver_" . $hex_verze . "/+sqm-bas-7_bot.gif");

    copy("zaklad_top.gif"          , "ver_" . $hex_verze . "/parts/part_zaklad_top.gif");
    copy("zaklad_bot.gif"          , "ver_" . $hex_verze . "/parts/part_zaklad_bot.gif");
    copy("propoj_jack_bas.gif"     , "ver_" . $hex_verze . "/propoj_jack_bas.gif");
    copy("fot_ts_propoj.jpg"       , "ver_" . $hex_verze . "/fot_ts_propoj.jpg");
    copy("ts_tl_delky.gif"         , "ver_" . $hex_verze . "/ts_tl_delky.gif");
    copy("propoj_ts_bas.gif"       , "ver_" . $hex_verze . "/propoj_ts_bas.gif");

    copy("eagle_sqm-bas-7.zip"     , "ver_" . $hex_verze . "/eagle_sqm-bas-7.zip");      


    copy("spoj_mat.gif"            , "ver_" . $hex_verze . "/spoj_mat.gif");

    if ($copy_BAT_top      == true)    copy("baterie_top.gif"      , "ver_" . $hex_verze . "/parts/part_baterie_top.gif");
    if ($copy_SUPCAP_top   == true)    copy("supcap_top.gif"       , "ver_" . $hex_verze . "/parts/part_supcap_top.gif");
    if ($copy_DIS_top      == true)    copy("displej_top.gif"      , "ver_" . $hex_verze . "/parts/part_displej_top.gif");
    if ($copy_PCB_DIS_top  == true)    copy("sqm-dis-3_top.gif"    , "ver_" . $hex_verze . "/+sqm-dis-3_top.gif");
    if ($copy_SD_top       == true)    copy("sd_top.gif"           , "ver_" . $hex_verze . "/parts/part_sd_top.gif");
    if ($copy_BME_top      == true)    copy("bme280_top.gif"       , "ver_" . $hex_verze . "/parts/part_bme280_top.gif");
    if ($copy_BZUK_top     == true)    copy("bzuk_top.gif"         , "ver_" . $hex_verze . "/parts/part_bzuk_top.gif");
    if ($copy_LED_top      == true)    copy("led_top.gif"          , "ver_" . $hex_verze . "/parts/part_led_top.gif");
    if ($copy_LSM303_top   == true)    copy("lsm303_top.gif"       , "ver_" . $hex_verze . "/parts/part_lsm303_top.gif");
    if ($copy_R30S_top     == true)    copy("r30s_top.gif"         , "ver_" . $hex_verze . "/parts/part_r30s_top.gif");
    if ($copy_DS3231_top   == true)    copy("ds3231_top.gif"       , "ver_" . $hex_verze . "/parts/part_ds3231_top.gif");
    if ($copy_EXT_top      == true)    copy("ext_top.gif"          , "ver_" . $hex_verze . "/parts/part_ext_top.gif");
    if ($copy_UREF_top     == true)    copy("uref_top.gif"         , "ver_" . $hex_verze . "/parts/part_uref_top.gif");
    if ($copy_GPS_top      == true)    copy("gps_top.gif"          , "ver_" . $hex_verze . "/parts/part_gps_top.gif");
    if ($copy_PCB_GPS_top  == true)    copy("sqm-gps-6_top.gif"    , "ver_" . $hex_verze . "/+sqm-gps-6_top.gif");
    if ($copy_RS485bez_top == true)    copy("rs485_bezgps_top.gif" , "ver_" . $hex_verze . "/parts/part_rs485_bezgps_top.gif");
    if ($copy_RS485s_top   == true)    copy("rs485_sgps_top.gif"   , "ver_" . $hex_verze . "/parts/part_rs485_sgps_top.gif");
    if ($copy_BAT_bot      == true)    copy("bat_bot.gif"          , "ver_" . $hex_verze . "/parts/part_bat_bot.gif");
    if ($copy_SUPCAP_bot   == true)    copy("supcap_bot.gif"       , "ver_" . $hex_verze . "/parts/part_supcap_bot.gif");
    if ($copy_DIS_bot      == true)    copy("displej_bot.gif"      , "ver_" . $hex_verze . "/parts/part_displej_bot.gif");
    if ($copy_PCB_DIS_bot  == true)    copy("sqm-dis-3_bot.gif"    , "ver_" . $hex_verze . "/+sqm-dis-3_bot.gif");
    if ($copy_SD_bot       == true)    copy("sd_bot.gif"           , "ver_" . $hex_verze . "/parts/part_sd_bot.gif");
    if ($copy_BME_bot      == true)    copy("bme280_bot.gif"       , "ver_" . $hex_verze . "/parts/part_bme280_bot.gif");
    if ($copy_BZUK_bot     == true)    copy("bzuk_bot.gif"         , "ver_" . $hex_verze . "/parts/part_bzuk_bot.gif");
    if ($copy_LED_bot      == true)    copy("led_bot.gif"          , "ver_" . $hex_verze . "/parts/part_led_bot.gif");
    if ($copy_LSM303_bot   == true)    copy("lsm303_bot.gif"       , "ver_" . $hex_verze . "/parts/part_lsm303_bot.gif");
    if ($copy_R30S_bot     == true)    copy("r30s_bot.gif"         , "ver_" . $hex_verze . "/parts/part_r30s_bot.gif");
    if ($copy_DS3231_bot   == true)    copy("ds3231_bot.gif"       , "ver_" . $hex_verze . "/parts/part_ds3231_bot.gif");
    if ($copy_EXT_bot      == true)    copy("ext_bot.gif"          , "ver_" . $hex_verze . "/parts/part_ext_bot.gif");
    if ($copy_UREF_bot     == true)    copy("uref_bot.gif"         , "ver_" . $hex_verze . "/parts/part_uref_bot.gif");
    if ($copy_GPS_bot      == true)    copy("gps_bot.gif"          , "ver_" . $hex_verze . "/parts/part_gps_bot.gif");
    if ($copy_PCB_GPS_bot  == true)    copy("sqm-gps-6_bot.gif"    , "ver_" . $hex_verze . "/+sqm-gps-6_bot.gif");
    if ($copy_RS485bez_bot == true)    copy("rs485_bot.gif"        , "ver_" . $hex_verze . "/parts/part_rs485_bot.gif");
    if ($copy_RS485s_bot   == true)    copy("rs485_bot.gif"        , "ver_" . $hex_verze . "/parts/part_rs485_bot.gif");


    if ($copy_PCB_GPS_bot  == true)    copy("fot_spoj_gps_bas.jpg" , "ver_" . $hex_verze . "/fot_spoj_gps_bas.jpg");
    if ($copy_PCB_GPS_bot  == true)    copy("fot_gps_propoj.jpg"   , "ver_" . $hex_verze . "/fot_gps_propoj.jpg");
    if ($copy_BME_top      == true)    copy("fot_bme_cin.jpg"      , "ver_" . $hex_verze . "/fot_bme_cin.jpg");
    if ($copy_LSM303_top   == true)    copy("fot_propoj_lsm.jpg"   , "ver_" . $hex_verze . "/fot_propoj_lsm.jpg");
    if ($copy_RS485bez_top == true)    copy("propoj_485_bas.gif"   , "ver_" . $hex_verze . "/propoj_485_bas.gif");            
    if ($copy_RS485s_top   == true)    copy("propoj_485_gps.gif"   , "ver_" . $hex_verze . "/propoj_485_gps.gif");            
    if ($copy_LSM303_top   == true)    copy("propoj_lsm_bas.gif"   , "ver_" . $hex_verze . "/propoj_lsm_bas.gif");
    
    if ($copy_PCB_DIS_top  == true)
      {
        copy("propoj_tl_dis.gif"     , "ver_" . $hex_verze . "/propoj_tl_dis.gif");      
        copy("fot_propoj_tl_dis.jpg" , "ver_" . $hex_verze . "/fot_propoj_tl_dis.jpg");      
        copy("eagle_sqm-dis-3.zip"   , "ver_" . $hex_verze . "/eagle_sqm-dis-3.zip");      
        copy("tisk_3d.zip"   , "ver_" . $hex_verze . "/tisk_3d.zip");                      // pri pouziti displeje se predpoklada, ze to je rucni verze s krabickou
      }
    else
      {
        copy("propoj_tl_bas.gif"     , "ver_" . $hex_verze . "/propoj_tl_bas.gif");    
        copy("pevne_schema.gif"     , "ver_" . $hex_verze . "/pevne_schema.gif");            
      }

    if ($copy_PCB_DIS_top  == true and $copy_LED_top == true)                            // v pripade osazeni RGB lED v rucni verzi krabicky 
      {
        copy("fot_rgb_krabicka.jpg"     , "ver_" . $hex_verze . "/fot_rgb_krabicka.jpg");      // umísteni LED nad USB konektorem
      }



    if ($copy_PCB_GPS_top  == true)
      {
        copy("propoj_neo_gps.gif"    , "ver_" . $hex_verze . "/propoj_neo_gps.gif");      
        copy("propoj_bat_gps.gif"    , "ver_" . $hex_verze . "/propoj_bat_gps.gif");      
        copy("eagle_sqm-gps-6.zip"   , "ver_" . $hex_verze . "/eagle_sqm-gps-6.zip");      
      }
    else
      {
        copy("propoj_bat_bas.gif"    , "ver_" . $hex_verze . "/propoj_bat_bas.gif");            
      }

    $soubor_predos = fopen("ver_" . $hex_verze . "/pred_osazenim.txt", 'a+');
    $predos_plus = file_get_contents("predos_bas.txt");       // text pro bas bude v souboru "pred_osazenim.txt" vzdycky
    fwrite($soubor_predos, $predos_plus);
    
    if ($predos == 3 or $predos == 7)                         // ve verzich s displejem se prida jeste text pro displej
      {
        $predos_plus = file_get_contents("predos_dis.txt");
        fwrite($soubor_predos, $predos_plus);
      }

    if ($predos == 5 or $predos == 7)                         // ve verzich s GPS se prida jeste text pro GPS
      {
        $predos_plus = file_get_contents("predos_gps.txt");
        fwrite($soubor_predos, $predos_plus);
      }

    fclose($soubor_predos);

  }

echo "<h2>Úprava konfigurace v bloku #doc#01 ve zdrojovém kódu (soubor 'stm411.ino')</h2><br>";
echo "<textarea cols='150' rows='8'>" . $prg_config . "</textarea><br><br><br><br>";


// konfigurace programu se uklada do zvlastniho souboru
$soubor=fopen("ver_" . $hex_verze . "/prg_config.txt", "w+");
fwrite($soubor,  $prg_config);        
fclose($soubor);



// vygenerovani tabulky rozpisky z SQL databaze
$soubor=fopen("ver_" . $hex_verze . "/rozpiska.htm", "w+");
$soubor2=fopen("ver_" . $hex_verze . "/rozpiska_tisk.htm", "w+");

fwrite($soubor, "<HTML><HEAD><meta charset='UTF-8'></HEAD><BODY>");
fwrite($soubor, "<H2>Rozpiska pro SQM:</H2> <br>");
fwrite($soubor, "<table border='1'>");

fwrite($soubor2, "<HTML><HEAD><meta charset='UTF-8'></HEAD><BODY>");
fwrite($soubor2, "<H2>Zjednodušená rozpiska pro SQM:</H2> <br>");
fwrite($soubor2, "<table border='1'>");


$spojeni = mysqli_connect($mysql_SERVER , $mysql_JMENO , $mysql_HESLO , $mysql_DATABAZE); 


echo "<H2>Rozpiska pro SQM:</H2> <br>";


echo "<table border='1'>";

for ($i = 0; $i < 21 ; $i++)
  {
    if ($pouzity_blok[$i] == 1)
      {
        if ($i != 15)                     // komunikace RS485 s GPS a bez GPS je sloucena do jednoho bloku, proto se hlavicka tiskne jen 1x
          {
            echo "<tr bgcolor=YELLOW>";
            echo "      <td colspan='6' align='center'><B>". $popisbloku[$i] ."</B></td>";
            echo "</tr>\n";


            echo "<tr bgcolor=SILVER>";
            echo "      <td align='left'><B>pozice</B></td>";
            echo "      <td align='left'><B>hodnota</B></td>";
            echo "      <td align='left'><B>pouzdro</B></td>";
            echo "      <td align='left'><B>popis</B></td>";
            echo "      <td align='left'><B>odkaz</B></td>";
            echo "      <td align='left'><B>detail</B></td>";
            echo "</tr>\n";


            // plna rozpiska k zobrazeni
            fwrite($soubor, "<tr bgcolor=YELLOW>");
            fwrite($soubor, "      <td colspan='6' align='center'><B>". $popisbloku[$i] ."</B></td>");
            fwrite($soubor, "</tr>\n");
            fwrite($soubor, "<tr bgcolor=SILVER>");
            fwrite($soubor, "      <td align='left'><B>pozice</B></td>");
            fwrite($soubor, "      <td align='left'><B>hodnota</B></td>");
            fwrite($soubor, "      <td align='left'><B>pouzdro</B></td>");
            fwrite($soubor, "      <td align='left'><B>popis</B></td>");
            fwrite($soubor, "      <td align='left'><B>odkaz</B></td>");
            fwrite($soubor, "      <td align='left'><B>detail</B></td>");
            fwrite($soubor, "</tr>\n");

            // rozpiska k tisku (chybi odkaz a detail, navic je zaskrtavaci policko "sklad")
            fwrite($soubor2, "<tr bgcolor=YELLOW>");
            fwrite($soubor2, "      <td colspan='5' align='center'><B>". $popisbloku[$i] ."</B></td>");
            fwrite($soubor2, "</tr>\n");
            fwrite($soubor2, "<tr bgcolor=SILVER>");
            fwrite($soubor2, "      <td align='left'><B>sklad</B></td>");
            fwrite($soubor2, "      <td align='left'><B>pozice</B></td>");
            fwrite($soubor2, "      <td align='left'><B>hodnota</B></td>");
            fwrite($soubor2, "      <td align='left'><B>pouzdro</B></td>");
            fwrite($soubor2, "      <td align='left'><B>popis</B></td>");
            fwrite($soubor2, "</tr>\n");



          }

        $sql = "SELECT * FROM SQM_osazovacka WHERE blok = " . $i . " ORDER BY inx ASC;";
        $vysledek = MySQLi_Query($spojeni,$sql);
        $pom=0;
        
        while($zaznam1 = MySQLi_Fetch_Array($vysledek)):
        
          $pouzita_soucastka[$zaznam1[0]] = true;
        
          echo "<tr>";
          echo "      <td align='left'>" . $zaznam1[2] . " </td>";
          echo "      <td align='left'>" . $zaznam1[3] . " </td>";
          echo "      <td align='left'>" . $zaznam1[4] . " </td>";
          echo "      <td align='left'>" . $zaznam1[7] . " </td>";
          if ($zaznam1[8] != "") 
            {
              echo "      <td align='left'><A href='" . $zaznam1[8] . "' target=_blank>" . substr($zaznam1[8], 0, 20) . "...</A> </td>";
            }
          else
            {
              echo "      <td align='left'>&nbsp;</td>";
            }

          echo "      <td align='left'>" . $zaznam1[9] . " </td>";


          echo "</tr>\n";        
        
          
          // plna rozpiska k zobrazeni
          fwrite($soubor,  "<tr>");
          fwrite($soubor,  "      <td align='left'>" . $zaznam1[2] . " </td>");
          fwrite($soubor,  "      <td align='left'>" . $zaznam1[3] . " </td>");
          fwrite($soubor,  "      <td align='left'>" . $zaznam1[4] . " </td>");
          fwrite($soubor,  "      <td align='left'>" . $zaznam1[7] . " </td>");
          if ($zaznam1[8] != "") 
            {
              fwrite($soubor,  "      <td align='left'><A href='" . $zaznam1[8] . "' target=_blank>" . substr($zaznam1[8], 0, 20) . "...</A> </td>");
            }
          else
            {
              fwrite($soubor,  "      <td align='left'>&nbsp;</td>");
            }

          fwrite($soubor,  "      <td align='left'>" . $zaznam1[9] . " </td>");
          fwrite($soubor, "</tr>\n");        


          // rozpiska k tisku (chybi odkaz a detail, navic je zaskrtavaci policko "sklad")
          if (strlen($zaznam1[7]) > 40)
            {
              $zaznam1[7] = substr($zaznam1[7] , 0, 39) . "...";      // dlouhy popis se zkrati a doplni tremi teckami (kvuli UTF8  musi byt sudy pocet orezanych znaku)      
            }

          fwrite($soubor2,  "<tr>");
          fwrite($soubor2,  "      <td align='left'><input type='checkbox'></td>");
          fwrite($soubor2,  "      <td align='left'>" . $zaznam1[2] . " </td>");
          fwrite($soubor2,  "      <td align='left'>" . $zaznam1[3] . " </td>");
          fwrite($soubor2,  "      <td align='left'>" . $zaznam1[4] . " </td>");
          fwrite($soubor2,  "      <td align='left'>" . $zaznam1[7] . " </td>");
          fwrite($soubor2, "</tr>\n");        

        endwhile;


      }
  }
echo "</table><br> ";



fwrite($soubor,  "</table><br></BODY></HTML>\n");
fwrite($soubor2, "</table><br></BODY></HTML>\n");

fclose($soubor);
fclose($soubor2);

//        tabulka souctu vsech soucastek na vsech trech deskach

$soubor3=fopen("ver_" . $hex_verze . "/soucty_soucastek.htm", "w+");
fwrite($soubor3, "<HTML><HEAD><meta charset='UTF-8'></HEAD><BODY>\n");
fwrite($soubor3, "<H2>Celkové součty součástek pro objednání:</H2>\n");
fwrite($soubor3, "<table border='1'>");



echo "<H2>Celkové součty součástek pro objednání:</H2>";
echo "<table border='1'>";

for ($typ_souc = 1; $typ_souc < 11 ; $typ_souc++)
  {
  
        $tisknout_hlavicku = false;
        $sql = "SELECT * FROM SQM_osazovacka WHERE typ = " . $typ_souc . " ORDER BY hodnota ASC;";
        $vysledek = MySQLi_Query($spojeni,$sql);
        
        while($zaznam1 = MySQLi_Fetch_Array($vysledek)):
          if ($pouzita_soucastka[$zaznam1[0]] == true)   $tisknout_hlavicku = true;
        endwhile;
   
        if ($tisknout_hlavicku == true)
          {
            echo "<tr bgcolor=YELLOW><td align='left' colspan='5'><B> " . $popistypu[$typ_souc] . "</B></td></tr>\n";
            echo "<tr bgcolor=SILVER><td><B> počet </B></td><td><B>hodnota</B></td><td><B>pouzdro</B></td><td><B>popis</B></td><td><B>odkaz</B></td></tr>\n";
            
            fwrite($soubor3, "<tr bgcolor=YELLOW><td align='left' colspan='5'><B> " . $popistypu[$typ_souc] . "</B></td></tr>\n");
            fwrite($soubor3, "<tr bgcolor=SILVER><td><B> počet </B></td><td><B>hodnota</B></td><td><B>pouzdro</B></td><td><B>popis</B></td><td><B>odkaz</B></td></tr>\n");
          }        
        
        
        $sql = "SELECT * FROM SQM_osazovacka WHERE typ = " . $typ_souc . " ORDER BY hodnota ASC;";
        $vysledek = MySQLi_Query($spojeni,$sql);
        
        while($zaznam1 = MySQLi_Fetch_Array($vysledek)):
       
          if ($pouzita_soucastka[$zaznam1[0]] == true)
            {
              $pocet_soucastek = 1;
              $hledany_shluk = $zaznam1[6];
              $hodnota       = $zaznam1[3];
              $pouzdro       = $zaznam1[4];
              $popis         = $zaznam1[7];
              $odkaz         = $zaznam1[8];

              if ($odkaz != "") 
                {
                  $odkaz = "<A href='" . $zaznam1[8] . "' target=_blank>" . substr($zaznam1[8], 0, 20) . "...</A>";
                }
              else
                {
                  $odkaz = "&nbsp;";
                }


              
              $pouzita_soucastka[$zaznam1[0]] = false;        
              echo "<tr>";
              fwrite($soubor3, "<tr>"); 

              if ($hledany_shluk != 0)
                {
                  $sql2 = "SELECT * FROM SQM_osazovacka WHERE shluk = " . $hledany_shluk . " ORDER BY inx ASC;";
                  $vysledek2 = MySQLi_Query($spojeni,$sql2);
                  
                  while($zaznam2 = MySQLi_Fetch_Array($vysledek2)):
                    if ($pouzita_soucastka[$zaznam2[0]] == true)
                      {
                        $pocet_soucastek = $pocet_soucastek + 1; 
                        $pouzita_soucastka[$zaznam2[0]] = false;        
                      }
                  endwhile;
               }
        
            echo "<td> " . $pocet_soucastek . "</td><td> " . $hodnota . "</td><td> " . $pouzdro . "</td><td> " . $popis . "</td><td> " . $odkaz . "</td></tr>\n";  
            fwrite($soubor3, "<td> " . $pocet_soucastek . "</td><td> " . $hodnota . "</td><td> " . $pouzdro . "</td><td> " . $popis . "</td><td> " . $odkaz . "</td></tr>\n"); 
            }
        endwhile;
        if ($tisknout_hlavicku == true)
          {
            echo "<tr><td align='left' colspan='5'>&nbsp;</td></tr>\n";     // prazdna radka mezi typy soucastek
            fwrite($soubor3, "<tr><td align='left' colspan='5'>&nbsp;</td></tr>\n"); 
          }
  }

echo "</table><br> ";
fwrite($soubor3, "</table><br>\n "); 

fclose($soubor3);


MySQLi_close($spojeni);



echo "<br><br><hr>";
echo "<H2>Osazovačka pro desku SQM-BAS-7:</H2> <br>";

echo "<center>";
echo "<img src='ver_" . $hex_verze . "/+sqm-bas-7_top.gif" . "'><br>";
echo "SQM-BAS-7  TOP strana<br>";
echo "(+sqm-bas-7_top.gif)<br>";

if ($copy_BME_top      == true)
  {
    echo "<br><br>";
    echo "<img src='ver_" . $hex_verze . "/fot_bme_cin.jpg" . "'><br>";
    echo "Detail propojení modulu BME280<br>";
    echo "(fot_bme_cin.jpg)<br>";
  
  }

echo "<br><br><hr width=50%>";
echo "<img src='ver_" . $hex_verze . "/+sqm-bas-7_bot.gif" . "'><br>";
echo "SQM-BAS-7  BOT strana<br>";
echo "(+sqm-bas-7_bot.gif)<br>";
echo "</center>";


if ($pouzity_blok[17] == 1)           // kdyz je pouzita deska displeje, zobrazi se obrazek plosnaku SQM-DIS-3 z obou stran
  {
    echo "<br><br><hr>";
    echo "<H2>Osazovačka pro desku SQM-DIS-3:</H2> <br>";
    echo "<center>";
    echo "<img src='ver_" . $hex_verze . "/+sqm-dis-3_top.gif" . "'><br>";
    echo "SQM-DIS-3  TOP strana<br>";
    echo "(+sqm-dis-3_top.gif)<br>";
    echo "<br><br><hr width=50%>";
    echo "<img src='ver_" . $hex_verze . "/+sqm-dis-3_bot.gif" . "'><br>";
    echo "SQM-DIS-3  BOT strana<br>";
    echo "(+sqm-dis-3_bot.gif)<br>";
    echo "</center>";
  }



if ($pouzity_blok[16] == 1)           // kdyz je pouzita vnitrni GPS, zobrazi se obrazek plosnaku SQM-GPS-6 z obou stran
  {
    echo "<br><br><hr>";
    echo "<H2>Osazovačka pro desku SQM-GPS-6:</H2> <br>";
    echo "<center>";
    echo "<img src='ver_" . $hex_verze . "/+sqm-gps-6_top.gif" . "'><br>";
    echo "SQM-GPS-6  TOP strana<br>";
    echo "(+sqm-gps-6_top.gif)<br>";
    echo "<br><br><hr width=50%>";
    echo "<img src='ver_" . $hex_verze . "/+sqm-gps-6_bot.gif" . "'><br>";
    echo "SQM-GPS-6  BOT strana<br>";
    echo "(+sqm-gps-6_bot.gif)<br>";

    echo "<br><br>";
    echo "<img src='ver_" . $hex_verze . "/fot_spoj_gps_bas.jpg" . "'><br>";
    echo "Detaily osazení<br>";
    echo "(fot_spoj_gps_bas.jpg)<br>";
    echo "</center>";
  }



echo "<br><br><hr>";
echo "<H2>Okolní propoje:</H2> <br>";
echo "<center>";

if ($copy_PCB_DIS_top  == true)                           // propojeni tlacitek na displejovou desku, nebo primo na desku BAS
  {
    echo "<img src='ver_" . $hex_verze . "/propoj_tl_dis.gif" . "'><br>";
    echo "(propoj_tl_dis.gif)<br><br>";

    echo "<img src='ver_" . $hex_verze . "/fot_propoj_tl_dis.jpg" . "'><br>";
    echo "(fot_propoj_tl_dis.jpg)<br><br> <hr width=80%>";

  }
else
  {
    echo "<img src='ver_" . $hex_verze . "/propoj_tl_bas.gif" . "'><br>";
    echo "(propoj_tl_bas.gif)<br><br> <hr width=80%>";
  }

echo "<img src='ver_" . $hex_verze . "/propoj_ts_bas.gif" . "'><br>";
echo "(propoj_ts_bas.gif)<br><br>";

echo "<img src='ver_" . $hex_verze . "/fot_ts_propoj.jpg" . "'><br>";
echo "(fot_ts_propoj.jpg)<br><br>";


echo "<img style='max-width: 800px; height: auto;' src='ver_" . $hex_verze . "/ts_tl_delky.gif" . "'><br>";
echo "(ts_tl_delky.gif)<br><br> <hr width=80%>";


if ($copy_PCB_DIS_top  == true and $copy_LED_top == true)                            // v pripade osazeni RGB lED v rucni verzi krabicky 
  {
    echo "Sice není plánováno, že by měla být RGB LED osazena v ruční verzi, ale pokud bude tato kombinace použita,<br>"; 
    echo "musí se RGB LED vyvést na drátech do nějaké pozice, kde nebude překážet bočnímu tlačítku.<br><br>";
    echo "Ve vývojové verzi SQM jsem ji nalepil nad USB konektor a v boku krabičky jsem vyvrtal díru.<br><br>";
    echo "<img src='ver_" . $hex_verze . "/fot_rgb_krabicka.jpg" . "'><br>";
    echo "(fot_rgb_krabicka.jpg)<br><br> <hr width=80%>";
  }




if ($copy_PCB_GPS_top  == true)                           // propojeni modulu GPS k desce SQM-GPS-6
  {
    echo "<img src='ver_" . $hex_verze . "/propoj_neo_gps.gif" . "'><br>";
    echo "(propoj_neo_gps.gif)<br><br> ";

    echo "<img src='ver_" . $hex_verze . "/fot_gps_propoj.jpg" . "'><br>";
    echo "(fot_gps_propoj.jpg)<br><br> <hr width=80%>";


  }


if ($copy_PCB_GPS_top  == true)                           // propojeni modulu GPS k desce SQM-GPS-6
  {
    echo "<img src='ver_" . $hex_verze . "/propoj_bat_gps.gif" . "'><br>";
    echo "(propoj_bat_gps.gif)<br><br> <hr width=80%>";
  }
else
  {
    echo "<img src='ver_" . $hex_verze . "/propoj_bat_bas.gif" . "'><br>";  
    echo "(propoj_bat_bas.gif)<br><br> <hr width=80%>";
  }


if ($copy_RS485bez_top == true)                           // pripojeni komunikace do svorek na desku BAS
  {
    echo "<img src='ver_" . $hex_verze . "/propoj_485_bas.gif" . "'><br>";  
    echo "(propoj_485_bas.gif)<br><br> <hr width=80%>";
  }

if ($copy_RS485s_top == true)                             // pripojeni komunikace na desku GPS
  {
    echo "<img src='ver_" . $hex_verze . "/propoj_485_gps.gif" . "'><br>";  
    echo "(propoj_485_gps.gif)<br><br> <hr width=80%>";
  }

if ($copy_LSM303_top == true)                             // pripojeni kompasu a naklonomeru
  {
    echo "<img src='ver_" . $hex_verze . "/propoj_lsm_bas.gif" . "'><br>";  
    echo "(propoj_lsm_bas.gif)<br><br>";

    echo "<img src='ver_" . $hex_verze . "/fot_propoj_lsm.jpg" . "'><br>";  
    echo "(fot_propoj_lsm.jpg)<br><br> <hr width=80%>";

  }


echo "<img src='ver_" . $hex_verze . "/propoj_jack_bas.gif" . "'><br>";  
echo "(propoj_jack_bas.gif)<br><br>";



echo "</center>";

echo "<br><br><hr>";
echo "<H2>Mechanika:</H2> <br>";
echo "<center>";


if ($copy_PCB_DIS_top  == true)                           // pri pouziti displeje se predpoklada rucni verze SQM s krabickou
  {
    echo "<img src='ver_" . $hex_verze . "/spoj_mat.gif" . "'><br>";  
    echo "(spoj_mat.gif)<br><br>";
  }
else                                                      // pro verzi bez displeje si uzivatel resi mechaniku sam
  {
    echo "Pro verzi bez displeje se předpokládá, že se jedná o trvale nainstalovanou verzi.<br>";
    echo "Pro tuto variantu si musíte mechaniku zpracovat sami.<br>";
    echo "Schématické znázornění rozložení čidel:<br>";
    echo "<img src='ver_" . $hex_verze . "/pevne_schema.gif" . "'><br>";  

  }

echo "</center>";



// ----------- vytvoreni archivu se vsemi soubory v prave vytvorene slozce

// jednotlive funkcni casti osazovacek desky SQM-BAS se bali do samostatneho archivu "parts.zip", aby neprekazely v korenove slozce 
$dir="ver_" . $hex_verze . "/parts/";
$zip_name= "ver_" . $hex_verze . "/parts.zip";
if (is_file ($zip_name) == false)              // archiv se ve slozce vytvari jen v pripade, ze jeste neexistuje
  {
    $files=get_files($dir);
    $names=$files;
    $zip_content=pack_it($dir,$files,$names);
    download_it($zip_content,$zip_name);
  }



$dir="ver_" . $hex_verze . "/";
$zip_name= "ver_" . $hex_verze . "/sqm_archiv_" . $hex_verze . ".zip";

if (is_file ($zip_name) == false)              // archiv se ve slozce vytvari jen v pripade, ze jeste neexistuje
  {
    $files=get_files($dir);
    $names=$files;
    $zip_content=pack_it($dir,$files,$names);
    download_it($zip_content,$zip_name);
  }

echo "<br><br><HR><br><br><br>";

if ($varovani_1 == true)
  {
   echo "<br><br><font color=red><H3><B>POZOR: Pro režim 'R30s' je nutné osadit modul BME280, který ale nebyl v předchozím formuláři zaškrtnutý.<br>&nbsp;&nbsp;&nbsp;Došlo tedy k automatickému zaškrtnutí rozšíření BME280.</B></H3></font><br>";
  }

if ($varovani_2 == true)
  {
   echo "<br><br><font color=red><H3><B>POZOR: V této variantě není zvolena žádná signalizace (ani displej ani RGB LED).</B></H3></font><br>";
  }


echo "<center><H2><A href='" . $zip_name .  "'>Stáhnout archiv s vygenerovanými dokumenty</A></H2></center><br> <br> <br> ";





?>

</BODY>
</HTML>
