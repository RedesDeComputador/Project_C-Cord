C-Cord Server - Fase 2

Este repositório contém a segunda iteração do servidor de comunicações C-Cord. Construído sobre a arquitetura sequencial desenvolvida na Fase 1, esta versão expande as capacidades da rede introduzindo um sistema de mensagens assíncronas, um diretório público e mecanismos de moderação de contas através de hierarquia de privilégios.

O QUE HÁ DE NOVO NA FASE 2?

Nesta fase, o servidor deixou de ser apenas um sistema de autenticação e passou a suportar comunicação real e gestão de acessos:

Mensagens Offline (Assíncronas): Os utilizadores podem agora enviar mensagens para destinatários que não estão online. As mensagens ficam retidas no servidor até o destinatário solicitar a sua leitura (consumo destrutivo).

Diretório de Utilizadores: Adicionada a capacidade de consultar todos os membros registados na plataforma e os seus respetivos estados (Pendente, Aprovado, Admin).

Gestão de Contas (Perfil Admin): A segurança foi reforçada. Novos registos entram num estado de "Pendente" e necessitam da aprovação manual de um Administrador. Os Administradores ganharam também o poder de apagar contas permanentemente da base de dados.

Manipulação Segura de Ficheiros: Implementação de um sistema de reescrita com ficheiros temporários (temp.txt) para garantir a integridade da base de dados ao aprovar, apagar contas ou consumir mensagens.

ARQUITETURA E ARMAZENAMENTO

Rede: Sockets TCP/IP em C (Modelo Sequencial / Bloqueante).

Base de Dados Principal: Ficheiro de texto "users.txt" formatado como:   .

Caixa de Correio: Ficheiro de texto "messages.txt" formatado como:   .

COMO COMPILAR E EXECUTAR

Certifica-te de que tens os ficheiros do servidor e do cliente separados e o compilador GCC instalado.

Compilar os ficheiros:
gcc server.c -o server
gcc client.c -o client

Iniciar o Servidor:
Executa o servidor no teu terminal principal. Ele iniciará na porta 8080.
./server

Iniciar a Sessão do Cliente:
Abre um novo terminal e executa o cliente. (Lembrando que o servidor opera de forma sequencial, logo atende um cliente de cada vez).
./client

GUIA DE COMANDOS (Fase 2)

Nível 0: Visitante (Sem Login)

REGISTER   : Regista uma nova conta (fica pendente).

LOGIN   : Autentica no sistema. Requer aprovação prévia.

QUIT : Encerra a ligação com o servidor.

Nível 1: Utilizador Normal

LIST_ALL : Consulta a lista de todos os utilizadores registados.

SEND_MSG  <mensagem...> : Envia uma mensagem para a caixa de correio de outro membro.

CHECK_INBOX : Verifica, lê e apaga as mensagens destinadas a ti no servidor.

GET_INFO : Retorna o estado e versão do servidor.

ECHO  : Teste de latência/eco com o servidor.

QUIT : Termina a sessão e liberta o servidor para o próximo cliente.

Nível 2: Administrador

Possui acesso a todos os comandos do Utilizador Normal.

APPROVE_USER  : Muda o estado de um utilizador de "Pendente" (0) para "Aprovado" (1).

DELETE_USER  : Apaga permanentemente o registo de um utilizador do servidor.
