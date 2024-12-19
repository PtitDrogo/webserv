<?php

$messages = [
    "Bienvenue sur mon site web!",
    "Aujourd'hui est une belle journée!",
    "PHP est génial pour créer des scripts CGI!",
    "Continuez à coder et à apprendre.",
    "Vous êtes connecté depuis " . $_SERVER['REMOTE_ADDR']
];

$message = $messages[array_rand($messages)];

echo "Message Aléatoire: ";
echo "$message";
?>
