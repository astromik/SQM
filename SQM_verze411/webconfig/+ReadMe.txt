Generátor výrobní dokumentace pro SQM.
----------------------------------------------

Online verze je dostupná zde:
     https://astromik.org/raspi/sqm/verze411/webconfig/

Pomocí této webové aplikace je možné zvolit konkrétní typy rozšíření pro SQM.
Například neosazovat držák SD karty, nebo čidlo náklonu.

Výsledkem je pak ZIP balík dokumentace s konkrétními osazovačkami a rozpiskami.
Také se automaticky vygenerují úpravy zdrojového kódu pro STM32F411.

Zde jsou k dispozici podklady pro zprovoznění generátoru dokumentace na jiném serveru.


V souboru "gen_doc.php" je nutné zadat správné přihlašovací údaje pro databázi:
//---------------------------------------------------
// Prihlasovaci udaje pro MySQLi:
//  mysqli_connect($mysql_SERVER , $mysql_JMENO , $mysql_HESLO , $mysql_DATABAZE);
$mysql_SERVER   = "****server****";
$mysql_JMENO    = "****jmeno****";
$mysql_HESLO    = "****heslo****";
$mysql_DATABAZE = "****databaze****";



Kompletní databázová tabulka se nachází v souboru "SQM_osazovacka.sql" a do serverové MySQLi databáze je nutné ji naimportovat.



