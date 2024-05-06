Serveur de messagerie

Ce programme est un serveur de messagerie multi-utilisateurs conçu pour gérer la communication entre plusieurs clients via des sockets.

Développement

Etape 1 : Création du serveur
Le serveur est conçu pour générer un thread dédié à chaque nouvelle connexion de client. La communication entre le serveur et les clients est réalisée à l'aide de sockets.
Etape 2 : Gestion des messages
La gestion des messages est réalisée en utilisant un tableau de chaînes de caractères. Cette matrice permet de stocker tous les messages envoyés par chaque utilisateur.
Etape 3 : Envoi de messages
Les clients peuvent envoyer des messages au serveur. Les messages sont toujours des chaînes de caractères.
Etape 4 : Réception et renvoi 
Dès qu'un message est reçu par le serveur, il est renvoyé à tous les clients connectés.
Etape 5 : Affichage des messages
Les messages sont affichés dès qu’ils sont reçus. Pour une afin d’avoir une messagerie en temps réel.
Etape 6 : Identification
Chaque message affiché par un client est accompagné du numéro du client qui l'a posté, permettant ainsi aux utilisateurs de savoir de qui provient le message.
Etape 7 : Demande de blocage pour écrire un message
Les utilisateurs peuvent demander un blocage pour écrire un message en envoyant une commande spéciale. Pendant ce blocage, les messages reçus par le serveur sont stockés et affichés ultérieurement une fois que l'utilisateur a terminé d'écrire son message.

Utilisation

La compilation : Compilez le programme serveur à l'aide de votre compilateur C préféré.
L’exécution : Exécutez le programme serveur sur la machine hôte.
Les Connexions : Les clients se connectent au serveur en utilisant l'adresse IP et le port spécifiés.
Les communications : Les utilisateurs peuvent envoyer des messages au serveur, qui seront ensuite redistribués à tous les clients connectés.
La déconnexion : Les clients peuvent se déconnecter du serveur à tout moment.

Préparation

Adresse IP : Spécifiez l'adresse IP du serveur dans le code source.
Port : Spécifiez le port sur lequel le serveur écoute les connexions dans le code source.
