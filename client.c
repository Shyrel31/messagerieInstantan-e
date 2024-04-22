#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

typedef struct {
    char nom[30];
    int age;
} User;

int main() {
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


    send(socketClient, &user, sizeof(user), 0);


    char message[100];
    printf("Entrez vos messages (tapez 'fin' pour terminer) :\n");
    do {
        scanf("%s", message);
        send(socketClient, message, strlen(message) + 1, 0);
    } while (strcmp(message, "fin") != 0);


    close(socketClient);
    return 0;
}
