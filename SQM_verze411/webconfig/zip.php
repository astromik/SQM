<?PHP
ignore_user_abort();


include("zip.lib.php");

$cislo_verze="TEST";

$dir="ver_" . $cislo_verze . "/";


$zip_name= "ver_" . $cislo_verze . "/archiv_" . $cislo_verze . ".zip";



echo "DIR: " . $dir . "<br>";
echo "ZIP: " . $zip_name . "<br>";

function get_files($dir)
{
	$handle=opendir($dir);
	$i=0;
	while (false!==($file = readdir($handle))) 
	{
		if ($file!="."&&$file!=".."&&!is_dir($file))
		{       
      $files[$i]=$file;
      echo $file . "<br>";
      $i++;
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



//------------------------
$files=get_files($dir);
$names=$files;
echo "<br>1<br>";

$zip_content=pack_it($dir,$files,$names);
echo "<br>2<br>";

download_it($zip_content,$zip_name);
echo "<br>3<br>";


?>




