#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#define MAX_CLIENTS 3
#define MAX_MESSAGES 100

typedef struct {
    char nom[30];
    int age;
    char messages[MAX_MESSAGES][100];
    int num_messages;
} User;

void *function(void *arg) {
    int socket = *(int *)arg;
    char msg[] = "Entrez votre nom et votre âge :";
    send(socket, msg, strlen(msg) + 1, 0);

    User user;
    recv(socket, &user, sizeof(user), 0);
    printf("Le client s'appelle %s et a %d ans\n", user.nom, user.age);

    char buffer[100];
    int message_count = 0;
    do {
        recv(socket, buffer, sizeof(buffer), 0);
        strcpy(user.messages[message_count++], buffer);
    } while (strcmp(buffer, "fin") != 0);

    printf("Messages de l'utilisateur %s:\n", user.nom);
    for (int i = 0; i < message_count - 1; i++) {
        printf("%s\n", user.messages[i]);
    }

    close(socket);
    free(arg);
    pthread_exit(NULL);
}

int main(void) {
    int socketServeur = socket(AF_INET, SOCK_STREAM, 0);
    if (socketServeur == -1) {
        printf("Erreur lors de la création du socket\n");
        return 1;
    }

    struct sockaddr_in addrserveur;
    if (inet_pton(AF_INET, "10.0.2.15", &addrserveur.sin_addr) <= 0) {
        printf("Adresse IP invalide\n");
        return 1;
    }

    addrserveur.sin_family = AF_INET;
    addrserveur.sin_port = htons(21000);

    bind(socketServeur, (const struct sockaddr *)&addrserveur, sizeof(addrserveur));
    listen(socketServeur, 5);
    printf("Serveur en écoute...\n");

    pthread_t thread[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);
        int socketClient = accept(socketServeur, (struct sockaddr *)&addrClient, &csize);
        printf("Nouvelle connexion reçue\n");

        int *arg = malloc(sizeof(int));
        *arg = socketClient;

        pthread_create(&thread[i], NULL, function, arg);
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(thread[i], NULL);
    }

    close(socketServeur);

    printf("Fermeture du serveur\n");
    return 0;
}
