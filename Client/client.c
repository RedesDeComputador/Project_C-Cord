#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void print_menu(int role)
{
    printf("\n--- COMANDOS DISPONIVEIS ---\n");
    if (role == 0)
    {
        printf(" - REGISTER <user> <pass>\n");
        printf(" - LOGIN <user> <pass>\n");
        printf(" - QUIT\n");
    }
    else if (role == 1)
    {
        printf(" - GET_INFO\n");
        printf(" - ECHO <mensagem>\n");
        printf(" - SEND_MSG <destinatario> <mensagem...>\n");
        printf(" - LIST_ALL\n");
        printf(" - CHECK_INBOX\n");
        printf(" - QUIT\n");
    }
    else if (role == 2)
    {
        printf(" - GET_INFO\n");
        printf(" - ECHO <mensagem>\n");
        printf(" - LIST_ALL\n");
        printf(" - APPROVE_USER <user>\n");
        printf(" - DELETE_USER <user>\n");
        printf(" - STATUS\n");
        printf(" - QUIT\n");
    }
    printf("----------------------------\n");
}

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE] = {0};

    int client_role = 0; // 0 = Pendente, 1 = Normal, 2 = Admin

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Falha na criacao do socket \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\n Endereco invalido / Nao suportado \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Falha na conexao ao servidor. \n");
        return -1;
    }

    printf("Conectado ao servidor C-Cord!\n");
    print_menu(client_role);

    while (1)
    {
        printf("\n> ");
        fgets(input, BUFFER_SIZE, stdin);
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0)
            continue;

        send(sock, input, strlen(input), 0);

        if (strcmp(input, "QUIT") == 0)
        {
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0)
        {
            printf("%s", buffer);

            if (strncmp(input, "LOGIN", 5) == 0)
            {
                if (strstr(buffer, "Bem-vindo Administrador!"))
                {
                    client_role = 2;
                    print_menu(client_role);
                }
                else if (strstr(buffer, "Login efetuado com sucesso"))
                {
                    client_role = 1;
                    print_menu(client_role);
                }
            }
        }
        else
        {
            printf("O servidor fechou a ligacao.\n");
            break;
        }
    }

    close(sock);
    return 0;
}
