#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTS 3
#define MAX_MESSAGES 3

typedef struct {
    char nom[30];
    int age;
    int id; 
    char messages[MAX_MESSAGES][100];
    int num_messages;
} User;

typedef struct {
    int socket;
    struct sockaddr_in addr;
    User user;
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t id_mutex = PTHREAD_MUTEX_INITIALIZER;
int next_id = 1;


int get_next_id() {
    pthread_mutex_lock(&id_mutex);
    int id = next_id++;
    pthread_mutex_unlock(&id_mutex);
    return id;
}

void *handle_client(void *arg) {
    int index = *(int *)arg;
    int socket = clients[index].socket;
    User *user = &clients[index].user;

   
    user->id = get_next_id();

    char msg[10000];
    sprintf(msg, "Bienvenue! Vous êtes le client #%d", user->id);
    send(socket, msg, strlen(msg) + 1, 0);

    char buffer[100];
    do {
        recv(socket, buffer, sizeof(buffer), 0);
        printf("Message reçu de %s (#%d): %s\n", user->nom, user->id, buffer);

        
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (i != index && clients[i].socket != -1) {
                sprintf(msg, "[%s (#%d)]: %s", user->nom, user->id, buffer);
                send(clients[i].socket, msg, strlen(msg) + 1, 0);
            }
        }
        pthread_mutex_unlock(&mutex);
    } while (strcmp(buffer, "fin") != 0 && strcmp(buffer, "exit") != 0 && strcmp(buffer, "quit") != 0);

    close(socket);

    pthread_mutex_lock(&mutex);
    clients[index].socket = -1;
    pthread_mutex_unlock(&mutex);

    free(arg);
    pthread_exit(NULL);
}

int main(void) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket = -1;
    }

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
    int index[MAX_CLIENTS];
    
    while (1) {
        struct sockaddr_in addrClient;
        socklen_t csize = sizeof(addrClient);
        int socketClient = accept(socketServeur, (struct sockaddr *)&addrClient, &csize);

        if (socketClient == -1) {
            printf("Erreur lors de l'acceptation de la connexion\n");
            continue;
        }

        printf("Nouvelle connexion reçue\n");

        pthread_mutex_lock(&mutex);
        int j;
        for (j = 0; j < MAX_CLIENTS; j++) {
            if (clients[j].socket == -1) {
                clients[j].socket = socketClient;
                clients[j].addr = addrClient;
                index[j] = j;
                pthread_create(&thread[j], NULL, handle_client, &index[j]);
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        if (j == MAX_CLIENTS) {
            printf("Nombre maximal de clients atteint. Fermeture du serveur.\n");
            break;
        }
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket != -1) {
            pthread_join(thread[i], NULL);
        }
    }

    close(socketServeur);

    printf("Fermeture du serveur\n");
    return 0;
}