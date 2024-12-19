<?php
echo "Variables d'environnement\n";
foreach ($_SERVER as $key => $value) {
    echo "$key: $value\n";
}
?>
