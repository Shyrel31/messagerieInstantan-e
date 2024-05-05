#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

typedef struct {
    char nom[30];
    int age;
} User;

void sigusr1_handler(int signum) {
    if (signum == SIGUSR1) {
        printf("Interruption demandée. Envoi du message spécial au serveur...\n");
        // Code pour envoyer un message spécial au serveur indiquant l'interruption
    }
}

void *recevoir_messages(void *arg) {
    int socketClient = *((int *)arg);
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

int main() {
    signal(SIGUSR1, sigusr1_handler); 

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
    connect(socketClient, (const struct sockaddr *)&addrClient, sizeof(addrClient));
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
    scanf("%s %d", user.nom, &user.age);

    send(socketClient, &user, sizeof(user), 0);

    char message[100];
    printf("Entrez vos messages (tapez 'interrupt' pour demander une interruption et 'fin' pour terminer) :\n");
    while (1) {
        scanf("%s", message);
        send(socketClient, message, strlen(message) + 1, 0);

        // Vérifier si une interruption est demandée
        if (strcmp(message, "interrupt") == 0) {
            kill(getpid(), SIGUSR1); // Envoyer le signal SIGUSR1 (Ctrl+I) pour demander une interruption
        }

        if (strcmp(message, "fin") == 0 || strcmp(message, "exit") == 0 || strcmp(message, "quit") == 0) {
            break;
        }

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