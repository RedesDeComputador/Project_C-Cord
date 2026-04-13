#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Falha na criação do socket \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n Endereço inválido / Não suportado \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Falha na conexão ao servidor. O servidor está a correr? \n");
        return -1;
    }

    printf("Conectado ao servidor C-Cord!\n");
    printf("Comandos disponíveis:\n");
    printf(" - REGISTER <user> <pass>\n");
    printf(" - LOGIN <user> <pass>\n");
    printf(" - GET_INFO\n");
    printf(" - ECHO <mensagem>\n");
    printf(" - QUIT\n");

    while (1) {
        printf("\n> ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) continue;

        send(sock, input, strlen(input), 0);

        if (strcmp(input, "QUIT") == 0) {
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0) {
            printf("%s", buffer);
        } else {
            printf("O servidor fechou a ligação.\n");
            break;
        }
    }

    close(sock);
    return 0;
}