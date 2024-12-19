<?php
$a = isset($_GET['a']) ? (int)$_GET['a'] : 5;
$b = isset($_GET['b']) ? (int)$_GET['b'] : 7;
$sum = $a + $b;

echo "Calcul de la Somme\n";
echo "a = $a ";
echo "b = $b ";
echo "\nSomme : $sum";
?>
