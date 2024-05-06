#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> // for close() function

#define MAX_MESSAGES 3
#define MAX_STOCK 100

int ecriture_en_cours = 1; // Variable globale pour suivre l'état de l'écriture des messages
char **Messagesliste;
int nbMessage;


typedef struct {
    char nom[30];
    int age;
} User;

void *recevoir_messages(void *arg) {
    int socketClient = *((int *)arg); // Use local variable socketClient

    char message[100];
    while (1) {
        if (recv(socketClient, message, sizeof(message), 0) == -1) {
            printf("Erreur lors de la réception d'un message du serveur\n");
            break;
        }
        printf("Message reçu du serveur: %s\n", message);
    }

    return NULL;
}

void stopMess(int signum) {
   ecriture_en_cours = 1;
  
    if (nbMessage != 0) {
        for (int i = 0 ; i < nbMessage ; i++) {
            printf("%s", Messagesliste[i]);
        }

        for (int i = 0 ; i < nbMessage ; i++) {
            free(Messagesliste[i]);
        }
        free(Messagesliste);
        nbMessage = 0;
        Messagesliste = (char **)malloc(MAX_STOCK * sizeof(char *));
    }
   ecriture_en_cours = 0;
}


int main() {
   
    if (signal(SIGINT, stopMess) == SIG_ERR) {
        perror("Erreur lors de la configuration du gestionnaire de signal");
        return 1;
    }

 
    int socketClient = socket(AF_INET, SOCK_STREAM, 0); 

    if (socketClient == -1) {
        printf("Erreur lors de la création du socket\n");
        return 1;
    }

    struct sockaddr_in addrClient;
    if (inet_pton(AF_INET, "10.0.2.15", &addrClient.sin_addr) <= 0) {
        printf("Adresse IP invalide\n");
        return 1;
    }

    addrClient.sin_family = AF_INET;
    addrClient.sin_port = htons(21000);

    if (connect(socketClient, (const struct sockaddr *)&addrClient, sizeof(addrClient)) == -1) {
        printf("Erreur lors de la connexion au serveur\n");
        return 1;
    }

    printf("Connecté\n");

    pthread_t thread;
    pthread_create(&thread, NULL, recevoir_messages, (void *)&socketClient);

    char msg[100];
    if (recv(socketClient, msg, sizeof(msg), 0) == -1) {
        printf("Erreur lors de la réception du message du serveur\n");
        close(socketClient);
        return 1;
    }

    printf("%s\n", msg);

    User user;
    printf("Entrez votre nom et votre âge : ");
    scanf("%s %d", user.nom, &user.age);

    // Envoi du message formaté au serveur
    sprintf(msg, "L'utilisateur %s qui a %d ans s'est connecté", user.nom, user.age);
    send(socketClient, msg, strlen(msg) + 1, 0);

    char message[100];
    printf("Entrez vos messages :\n");
    while (1) {
        scanf(" %[^\n]", message);
        send(socketClient, message, strlen(message) + 1, 0);
        printf("En attente d'une réponse du serveur...\n");
        if (recv(socketClient, msg, sizeof(msg), 0) == -1) {
            printf("Erreur lors de la réception du message du serveur\n");
            break;
        }
        printf("Réponse du serveur: %s\n", msg);
    }

    close(socketClient);

    pthread_join(thread, NULL);

    return 0;
}
