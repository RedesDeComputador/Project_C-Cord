C-Cord: Sistema Cliente-Servidor em C
Um sistema cliente-servidor simples e interativo implementado na linguagem C utilizando Sockets TCP/IP. Este projeto (Fase 1) implementa um servidor sequencial com um sistema básico de autenticação, onde os utilizadores podem registar-se, fazer login e comunicar com o servidor.

✨ Funcionalidades
Comunicação via Sockets TCP: Conexão fiável entre cliente e servidor na porta 8080.

Sistema de Autenticação: Registo e Login de utilizadores.

Persistência de Dados (Local): As credenciais dos utilizadores são guardadas num ficheiro de texto (users.txt).

Proteção de Rotas: Comandos como GET_INFO e ECHO requerem que o utilizador tenha o login efetuado.

Interface CLI Simples: O cliente possui um terminal interativo para enviar comandos e ler as respostas do servidor em tempo real.

🛠️ Pré-requisitos
Para compilar e executar este projeto, precisas de um ambiente do tipo Unix (Linux, macOS, ou WSL no Windows) com o compilador GCC instalado.

GCC (GNU Compiler Collection)

🚀 Como Compilar e Executar
1. Compilação
Abre o teu terminal na pasta onde tens os ficheiros de código e corre os seguintes comandos para compilar o servidor e o cliente:
# Compilar o servidor
gcc server.c -o server

# Compilar o cliente
gcc client.c -o client

2. Execução
O servidor tem de estar a correr antes do cliente se conseguir conectar.

No Terminal 1 (Servidor):./server

(Deverás ver a mensagem "Servidor C-Cord (Fase 1) a correr na porta 8080...")

No Terminal 2 (Cliente):./client

📜 Comandos DisponíveisPodes interagir com o servidor usando os seguintes comandos no terminal do cliente:
ComandoDescriçãoExemplo de Uso
REGISTER Regista um novo utilizador na base de dados (users.txt).
REGISTER joao password123LOGINAutentica um utilizador existente.LOGIN joao password123GET_INFORetorna informações sobre o servidor (Requer Login).GET_INFOECHOO servidor devolve a mensagem enviada (Requer Login).ECHO Olá Servidor!QUITEncerra a ligação com o servidor e fecha o cliente.QUIT

Arquitetura e Limitações (Fase 1)
Sendo esta a primeira iteração (Fase 1) do projeto, existem algumas características técnicas a ter em conta:

Servidor Sequencial (Bloqueante): Atualmente, o servidor atende um cliente de cada vez. O ciclo principal usa um accept() bloqueante e entra num loop dedicado a esse cliente até ele fazer QUIT ou desconectar-se.

Segurança de Palavras-passe: As passwords são guardadas em plain-text (texto limpo) no ficheiro users.txt. Numa aplicação real, estas devem ser sempre encriptadas (ex: usando hashes SHA-256).

Limites de Buffer: As mensagens estão limitadas a 1024 bytes (definido em BUFFER_SIZE).
