#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define DB_FILE "users.txt"

void register_user(char *username, char *password, char *response)
{
    FILE *file = fopen(DB_FILE, "r");
    if (file != NULL)
    {
        char file_user[256], file_pass[256];
        int status;
        // Atualizado para ler os 3 campos
        while (fscanf(file, "%s %s %d", file_user, file_pass, &status) != EOF)
        {
            if (strcmp(username, file_user) == 0)
            {
                fclose(file);
                strcpy(response, "ERRO: O nome de utilizador ja existe. Escolhe outro.\n");
                return;
            }
        }
        fclose(file);
    }

    file = fopen(DB_FILE, "a");
    if (file == NULL)
    {
        strcpy(response, "ERRO: Nao foi possivel abrir a base de dados.\n");
        return;
    }
    // Grava o novo utilizador com status 0 (Pendente)
    fprintf(file, "%s %s 0\n", username, password);
    fclose(file);
    strcpy(response, "OK: Utilizador registado com sucesso. A aguardar aprovacao do Administrador.\n");
}

// Devolve: 0 (Falha), -1 (Pendente), 1 (Utilizador Normal), 2 (Administrador)
int login_user(char *username, char *password)
{
    FILE *file = fopen(DB_FILE, "r");
    if (file == NULL)
        return 0;

    char file_user[256], file_pass[256];
    int status;
    while (fscanf(file, "%s %s %d", file_user, file_pass, &status) != EOF)
    {
        if (strcmp(username, file_user) == 0 && strcmp(password, file_pass) == 0)
        {
            fclose(file);
            if (status == 0)
                return -1; // Conta existe, mas está pendente
            if (status == 2)
                return 2; // Login de Administrador
            return 1;     // Login de Utilizador normal
        }
    }
    fclose(file);
    return 0; // Credenciais erradas
}

// Função para o F6: Apagar um utilizador
void delete_user(char *target_user, char *response)
{
    FILE *file = fopen(DB_FILE, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!file || !temp)
    {
        strcpy(response, "ERRO: Falha ao aceder a base de dados.\n");
        if (file)
            fclose(file);
        if (temp)
            fclose(temp);
        return;
    }

    char file_user[256], file_pass[256];
    int status;
    int found = 0;

    // Copia todos os utilizadores para o ficheiro temporário, exceto o alvo
    while (fscanf(file, "%s %s %d", file_user, file_pass, &status) != EOF)
    {
        if (strcmp(target_user, file_user) == 0)
        {
            found = 1; // Encontrou e vai ignorar (não escreve no temp)
        }
        else
        {
            fprintf(temp, "%s %s %d\n", file_user, file_pass, status);
        }
    }

    fclose(file);
    fclose(temp);

    // Substitui o ficheiro antigo pelo novo
    remove(DB_FILE);
    rename("temp.txt", DB_FILE);

    if (found)
    {
        strcpy(response, "OK: Utilizador apagado com sucesso.\n"); // Envia confirmacao
    }
    else
    {
        strcpy(response, "ERRO: Utilizador nao encontrado.\n");
    }
}

// Função para o F5: Aprovar um novo utilizador
void approve_user(char *target_user, char *response)
{
    FILE *file = fopen(DB_FILE, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!file || !temp)
    {
        strcpy(response, "ERRO: Falha ao aceder a base de dados.\n");
        if (file)
            fclose(file);
        if (temp)
            fclose(temp);
        return;
    }

    char file_user[256], file_pass[256];
    int status;
    int found = 0;

    while (fscanf(file, "%s %s %d", file_user, file_pass, &status) != EOF)
    {
        if (strcmp(target_user, file_user) == 0)
        {
            found = 1;
            fprintf(temp, "%s %s 1\n", file_user, file_pass); // Muda o status de 0 para 1
        }
        else
        {
            fprintf(temp, "%s %s %d\n", file_user, file_pass, status);
        }
    }

    fclose(file);
    fclose(temp);

    remove(DB_FILE);
    rename("temp.txt", DB_FILE);

    if (found)
    {
        strcpy(response, "OK: Utilizador aprovado com sucesso.\n");
    }
    else
    {
        strcpy(response, "ERRO: Utilizador nao encontrado ou ja aprovado.\n");
    }
}

void list_users(char *response)
{
    FILE *file = fopen(DB_FILE, "r");
    if (file == NULL)
    {
        strcpy(response, "ERRO: Nao foi possivel aceder a base de dados.\n");
        return;
    }

    char file_user[256], file_pass[256];
    int status;

    strcpy(response, "--- LISTA DE UTILIZADORES ---\n");

    while (fscanf(file, "%s %s %d", file_user, file_pass, &status) != EOF)
    {
        // Concatenar cada utilizador à resposta (com cuidado para não exceder o BUFFER_SIZE)
        char line[300];
        if (status == 2)
        {
            snprintf(line, sizeof(line), "- %s (Admin)\n", file_user);
        }
        else if (status == 1)
        {
            snprintf(line, sizeof(line), "- %s (Aprovado)\n", file_user);
        }
        else
        {
            snprintf(line, sizeof(line), "- %s (Pendente)\n", file_user);
        }

        if (strlen(response) + strlen(line) < BUFFER_SIZE - 10)
        {
            strcat(response, line);
        }
        else
        {
            strcat(response, "... (lista truncada)\n");
            break;
        }
    }
    fclose(file);
}

// Função para enviar e armazenar a mensagem (F4)
void send_message(char *sender, char *receiver, char *msg, char *response)
{
    FILE *file = fopen("messages.txt", "a");
    if (!file)
    {
        strcpy(response, "ERRO: Nao foi possivel aceder a caixa de mensagens.\n");
        return;
    }
    // Grava no formato: Destinatario Remetente Mensagem
    fprintf(file, "%s %s %s\n", receiver, sender, msg);
    fclose(file);
    strcpy(response, "OK: Mensagem armazenada no servidor para quando o utilizador a solicitar.\n");
}

// Função para ler as mensagens e retirá-las da caixa de entrada (F4)
void check_inbox(char *current_user, char *response)
{
    FILE *file = fopen("messages.txt", "r");
    FILE *temp = fopen("temp_msgs.txt", "w");

    if (!file || !temp)
    {
        strcpy(response, "INFO: A tua caixa de entrada esta vazia (ficheiro inexistente).\n");
        if (file)
            fclose(file);
        if (temp)
            fclose(temp);
        return;
    }

    char receiver[256], sender[256], msg[512];
    char line[1024];
    int found = 0;

    strcpy(response, "--- CAIXA DE ENTRADA ---\n");

    // Lemos linha a linha para capturar a mensagem inteira com espaços
    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%s %s %[^\n]", receiver, sender, msg) == 3)
        {
            if (strcmp(current_user, receiver) == 0)
            {
                found = 1;
                char inbox_line[800]; // Aumentado para 800 para evitar truncamento
                snprintf(inbox_line, sizeof(inbox_line), "De %s: %s\n", sender, msg);

                // Evitar overflow do buffer de resposta
                if (strlen(response) + strlen(inbox_line) < BUFFER_SIZE - 10)
                {
                    strcat(response, inbox_line);
                }
                // Como é para este utilizador, NÃO gravamos no ficheiro temporário (mensagem consumida)
            }
            else
            {
                fprintf(temp, "%s", line); // Mensagem para outro utilizador, mantemos no ficheiro
            }
        }
    }

    fclose(file);
    fclose(temp);
    remove("messages.txt");
    rename("temp_msgs.txt", "messages.txt");

    if (!found)
    {
        strcpy(response, "INFO: Nao tens mensagens novas.\n");
    }
    else
    {
        strcat(response, "------------------------\n");
    }
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Falha ao criar o socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Falha no bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("Falha no listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor C-Cord (Fase 1) a correr na porta %d...\n", PORT);

    // 4. Ciclo principal bloqueante (um cliente de cada vez)
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Falha no accept");
            continue;
        }

        printf("Novo cliente conectado.\n");

        int user_role = 0;            // 0 = Nao logado, 1 = Normal, 2 = Admin
        char current_user[256] = {0}; // NOVO: Para guardar quem tem a sessão iniciada

        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            memset(response, 0, BUFFER_SIZE);

            int valread = read(new_socket, buffer, BUFFER_SIZE - 1);
            if (valread <= 0)
            {
                printf("Cliente desconectado.\n");
                close(new_socket);
                break;
            }

            buffer[strcspn(buffer, "\r\n")] = 0;
            printf("[LOG] Recebido do cliente: %s\n", buffer);

            char command[256], arg1[256], arg2[256];
            int parsed = sscanf(buffer, "%s %s %s", command, arg1, arg2);

            if (strcmp(command, "REGISTER") == 0 && parsed >= 3)
            {
                register_user(arg1, arg2, response);
            }
            else if (strcmp(command, "LOGIN") == 0 && parsed >= 3)
            {
                int login_status = login_user(arg1, arg2);
                if (login_status == 1)
                {
                    user_role = 1;
                    strcpy(current_user, arg1); // Guarda o nome do utilizador
                    strcpy(response, "OK: Login efetuado com sucesso.\n");
                }
                else if (login_status == 2)
                {
                    user_role = 2;
                    strcpy(current_user, arg1); // Guarda o nome do administrador
                    strcpy(response, "OK: Bem-vindo Administrador!\n");
                }
                else if (login_status == -1)
                {
                    strcpy(response, "ERRO: A tua conta precisa de ser aprovada por um administrador.\n");
                }
                else
                {
                    strcpy(response, "ERRO: Credenciais invalidas.\n");
                }
            }
            else if (strcmp(command, "SEND_MSG") == 0 && parsed >= 3)
            {
                if (user_role > 0)
                {
                    // Magia com ponteiros para extrair toda a frase após o username
                    char *msg_ptr = strchr(buffer, ' '); // primeiro espaço
                    if (msg_ptr)
                    {
                        msg_ptr++;
                        msg_ptr = strchr(msg_ptr, ' '); // segundo espaço (antes da mensagem)
                        if (msg_ptr)
                        {
                            msg_ptr++; // avança o espaço, msg_ptr agora aponta para a mensagem inteira
                            send_message(current_user, arg1, msg_ptr, response);
                        }
                        else
                        {
                            strcpy(response, "ERRO: Mensagem vazia.\n");
                        }
                    }
                }
                else
                {
                    strcpy(response, "ERRO: Precisas de fazer LOGIN primeiro.\n");
                }
            }
            else if (strcmp(command, "CHECK_INBOX") == 0)
            {
                if (user_role > 0)
                {
                    check_inbox(current_user, response);
                }
                else
                {
                    strcpy(response, "ERRO: Precisas de fazer LOGIN primeiro.\n");
                }
            }
            else if (strcmp(command, "GET_INFO") == 0)
            {
                if (user_role > 0)
                {
                    strcpy(response, "INFO: C-Cord Server v1.0 | Uptime: 100s | Modo: Sequencial\n");
                        }
                else
                {
                    strcpy(response, "ERRO: Precisas de fazer LOGIN primeiro.\n");
                }
            }
            else if (strcmp(command, "ECHO") == 0)
            {
                if (user_role > 0)
                {
                    snprintf(response, BUFFER_SIZE, "ECHO: %s\n", buffer + 5);
                }
                else
                {
                    strcpy(response, "ERRO: Precisas de fazer LOGIN primeiro.\n");
                }
            }
            else if (strcmp(command, "APPROVE_USER") == 0 && parsed >= 2)
            {
                if (user_role == 2)
                {
                    approve_user(arg1, response);
                }
                else
                {
                    strcpy(response, "ERRO: Acesso negado. Apenas para Administradores.\n");
                }
            }
            else if (strcmp(command, "DELETE_USER") == 0 && parsed >= 2)
            {
                if (user_role == 2)
                {
                    delete_user(arg1, response);
                }
                else
                {
                    strcpy(response, "ERRO: Acesso negado. Apenas para Administradores.\n");
                }
            }
            else if (strcmp(command, "LIST_ALL") == 0)
            {
                if (user_role > 0)
                { 
                    list_users(response);
                }
                else
                {
                    strcpy(response, "ERRO: Precisas de fazer LOGIN primeiro.\n");
                }
            }
            else if (strcmp(command, "STATUS") == 0)
            {
                if (user_role == 2)
                {
                    strcpy(response, "STATUS: Servidor ONLINE. Carga atual: BAIXA (Modo Sequencial).\n");
                }
                else
                {
                    strcpy(response, "ERRO: Acesso negado. Apenas para Administradores.\n");
                }
            }
            else if (strcmp(command, "QUIT") == 0)
            {
                strcpy(response, "A fechar ligacao. Adeus!\n");
                send(new_socket, response, strlen(response), 0);
                close(new_socket); // Garante que a socket é fechada
                break;
            }
            else
            {
                strcpy(response, "ERRO: Comando desconhecido ou formato invalido.\n");
            }

            send(new_socket, response, strlen(response), 0);
        }
    }
    return 0;
}