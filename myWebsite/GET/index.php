<?php
if (isset($_GET["fullName"]))
    $tmp = $_GET["fullName"];
else
    $tmp = '';

echo '<html><head><title>CGI Test</title></head><body>';
echo '<p>Your full name is : <strong>' .($tmp) . '</strong></p>';
echo '</body></html>';

?>
