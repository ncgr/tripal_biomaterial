<?php
//Launches JNLP 
$file=$_POST['file'];
header('Content-Description: File Transfer');
header('Content-Type: application/x-java-jnlp-file');
header( 'Content-Disposition: attachment; filename="start.jnlp"' );
ob_clean();
flush();
readfile($file);
echo $file;
?>
