#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define DB_FILE "users.txt"

// Função para registar um utilizador no ficheiro local
void register_user(char *username, char *password, char *response) {
    // 1. Verificar se o utilizador já existe
    FILE *file = fopen(DB_FILE, "r");
    if (file != NULL) {
        char file_user[256], file_pass[256];
        while (fscanf(file, "%s %s", file_user, file_pass) != EOF) {
            if (strcmp(username, file_user) == 0) {
                fclose(file);
                strcpy(response, "ERRO: O nome de utilizador já existe. Escolhe outro.\n");
                return;
            }
        }
        fclose(file);
    }

    // 2. Se não existe (ou o ficheiro ainda não foi criado), registar o novo utilizador
    file = fopen(DB_FILE, "a");
    if (file == NULL) {
        strcpy(response, "ERRO: Não foi possível abrir a base de dados.\n");
        return;
    }
    fprintf(file, "%s %s\n", username, password);
    fclose(file);
    strcpy(response, "OK: Utilizador registado com sucesso.\n");
}

// Função para verificar se o utilizador existe no ficheiro local
int login_user(char *username, char *password) {
    FILE *file = fopen(DB_FILE, "r");
    if (file == NULL) return 0;

    char file_user[256], file_pass[256];
    while (fscanf(file, "%s %s", file_user, file_pass) != EOF) {
        if (strcmp(username, file_user) == 0 && strcmp(password, file_pass) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};

    // 1. Criar o socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar o socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 2. Bind do socket à porta
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Falha no bind");
        exit(EXIT_FAILURE);
    }

    // 3. Listen (aguardar conexões)
    if (listen(server_fd, 3) < 0) {
        perror("Falha no listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor C-Cord (Fase 1) a correr na porta %d...\n", PORT);

    // 4. Ciclo principal bloqueante (um cliente de cada vez)
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Falha no accept");
            continue;
        }

        printf("Novo cliente conectado.\n");
        int is_logged_in = 0;

        // Loop de comunicação com o cliente ativo
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            memset(response, 0, BUFFER_SIZE);
            
            int valread = read(new_socket, buffer, BUFFER_SIZE - 1);
            if (valread <= 0) {
                printf("Cliente desconectado.\n");
                break;
            }

            buffer[strcspn(buffer, "\r\n")] = 0; 

            printf("[LOG] Recebido do cliente: %s\n", buffer);
            
            char command[256], arg1[256], arg2[256];
            int parsed = sscanf(buffer, "%s %s %s", command, arg1, arg2);

            if (strcmp(command, "REGISTER") == 0 && parsed >= 3) {
                register_user(arg1, arg2, response);
            } 
            else if (strcmp(command, "LOGIN") == 0 && parsed >= 3) {
                if (login_user(arg1, arg2)) {
                    is_logged_in = 1;
                    strcpy(response, "OK: Login efetuado com sucesso.\n");
                } else {
                    strcpy(response, "ERRO: Credenciais inválidas.\n");
                }
            } 
            else if (strcmp(command, "GET_INFO") == 0) {
                if (is_logged_in) {
                    strcpy(response, "INFO: C-Cord Server v1.0 | Uptime: 100s | Modo: Sequencial\n");
                } else {
                    strcpy(response, "ERRO: Precisas de fazer LOGIN primeiro.\n");
                }
            } 
            else if (strcmp(command, "ECHO") == 0) {
                if (is_logged_in) {
                    snprintf(response, BUFFER_SIZE, "ECHO: %s\n", buffer + 5); // +5 ignora "ECHO "
                } else {
                    strcpy(response, "ERRO: Precisas de fazer LOGIN primeiro.\n");
                }
            } 
            else if (strcmp(command, "QUIT") == 0) {
                strcpy(response, "A fechar ligação. Adeus!\n");
                send(new_socket, response, strlen(response), 0);
                break;
            }
            else {
                strcpy(response, "ERRO: Comando desconhecido ou formato inválido.\n");
            }

            send(new_socket, response, strlen(response), 0);
        }
        close(new_socket);
    }
    return 0;
}