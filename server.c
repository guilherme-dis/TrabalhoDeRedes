/*
** server.c -- a stream socket server demo
** Server side program that is able to wait for a connection and answer with a
** Hello World string to a client program
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MYPORT 3490      // the port users will be connecting to
#define MAXDATASIZE 1000 // max number of bytes we can get at once

#define BACKLOG 10 // how many pending connections queue will hold

void sigchld_handler(int s)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}
void deleteUserFromCredentials(char *user)
{
    FILE *fp;
    char line[255];
    fp = fopen("credenciais.txt", "r+");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, user, strlen(user)) == 0)
        {
            fseek(fp, -strlen(line), SEEK_CUR);
            int i;
            for (i = 1; i < strlen(line); i++)
            {
                fputc(' ', fp);
            }
            fputc('\n', fp);
            fclose(fp);
            return;
        }
    }
    fclose(fp);
}

void deleteUsersFromStatus(char *user)
{
    FILE *fp;
    char line[255];
    fp = fopen("status.txt", "r+");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, user, strlen(user)) == 0)
        {
            fseek(fp, -strlen(line), SEEK_CUR);
            int i;
            for (i = 1; i < strlen(line); i++)
            {
                fputc(' ', fp);
            }
            fputc('\n', fp);
            fclose(fp);
            return;
        }
    }
    fclose(fp);
}

void deleteUserFromMessages(char *user)
{
    FILE *fp;
    char line[255];
    fp = fopen("messages.txt", "r+");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, user, strlen(user)) == 0)
        {
            fseek(fp, -strlen(line), SEEK_CUR);
            int i;
            for (i = 1; i < strlen(line); i++)
            {
                fputc(' ', fp);
            }
            fputc('\n', fp);
            fclose(fp);
            return;
        }
    }
    fclose(fp);
}

int deleteUser(char *msg){
    //separar o comando da mensagem
    char *comando = strtok(msg, " ");
    char *user = strtok(NULL, " ");
    deleteUserFromCredentials(user);
    deleteUsersFromStatus(user);
    deleteUserFromMessages(user);
    return 0;
}


int checkStatus(char *receiver)
{
    // colocar um \0 no final da string
    receiver[strlen(receiver)] = '\0';
    // separar o comando da mensagem
    char *comando = strtok(receiver, " ");
    char *user = strtok(NULL, " ");
    printf("verificando status de %s \n", user);
    FILE *fp;
    fp = fopen("status.txt", "r+");
    if (fp == NULL)
    {
        printf("Erro ao abrir o arquivo status.txt\n");
        return -1;
    }
    char linha[100];
    while (fgets(linha, 100, fp) != NULL)
    {
        if (strstr(linha, user) != NULL)
        {
            char *status = strtok(linha, " ");
            status = strtok(NULL, " ");
            // remover o \n do final da string
            status[strlen(status) - 1] = '\0';
            if (!strcmp(status, "1"))
            {
                printf("status 1\n");
                fclose(fp);
                return 0;
            }
            else
            {
                printf("status 0\n");
                fclose(fp);
                return -1;
            }
        }
    }
    fclose(fp);
    printf("Usuario nao encontrado\n");
    return 1;
}

void store_message(char *receiver, char *sender, char *message)
{
    FILE *fp;
    fp = fopen("messages.txt", "r+");
    fprintf(fp, "%s %s \"%s\"\n", receiver, sender, message);
    fclose(fp);
}
char *checkMessage(char *message)
{
    char *comando = strtok(message, " ");
    char *user = strtok(NULL, " ");

    printf("verificando mensagens de %s \n", user);
    FILE *fp;
    char line[255];
    char *ret_message = "";
    fp = fopen("messages.txt", "r+");
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return "Error";
    }
    while (fgets(line, sizeof(line), fp))
    {
        if (strncmp(line, user, strlen(user)) == 0)
        {
            char *ret_message = strdup(line);
            printf("linha: %s", ret_message);
            fseek(fp, -strlen(line), SEEK_CUR);
            int i;
            for (i = 1; i < strlen(line); i++)
            {
                fputc(' ', fp);
            }
            fputc('\n', fp);
            fclose(fp);
            return ret_message;
        }
    }
    fclose(fp);
    return "no";
}

char *retrieve_message(char *receiver)
{
    FILE *fp;
    char line[255];
    char *ret_message = "";
    fp = fopen("messages.txt", "r"); // open the file in read mode
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return "Error";
    }
    while (fgets(line, sizeof(line), fp))
    {
        // check if the line starts with the receiver's name
        if (strncmp(line, receiver, strlen(receiver)) == 0)
        {
            if (ret_message[0] == '\0')
            {
                ret_message = strdup(line);
            }
            else
            {
                strcat(ret_message, line);
            }
        }
    }
    fclose(fp);
    return ret_message;
}

int saveMessage(char *mensagem)
{
    char *comando = strtok(mensagem, " ");
    char *sender = strtok(NULL, " ");
    char *receiver = strtok(NULL, " ");
    char *message = strtok(NULL, "\0");

    printf("receiver: %s sender: %s message: %s \n", receiver, sender, message);

    FILE *fp;
    fp = fopen("messages.txt", "r+");
    if (fp == NULL)
    {
        printf("Erro ao abrir o arquivo messages.txt");
        return -1;
    }

    char linha[100];
    while (fgets(linha, 100, fp) != NULL)
    {
    }
    fprintf(fp, "%s %s \"%s\"\n", receiver, sender, message);
    fclose(fp);
    return 0;
}

int changeStatus(char *argumento, int status)
{
    char *username = strtok(argumento, " ");
    FILE *fp;
    fp = fopen("status.txt", "r+");
    if (fp == NULL)
    {
        printf("Erro ao abrir o arquivo status.txt");
        return -1;
    }
    char linha[100];
    while (fgets(linha, 100, fp) != NULL)
    {
        if (strstr(linha, username) != NULL)
        {
            fseek(fp, -strlen(linha), SEEK_CUR);
            fprintf(fp, "%s %d\n", username, status);
            fclose(fp);
            return 0;
        }
    }
    fprintf(fp, "%s %d\n ", username, status);
    fclose(fp);
    return 0;
}

int getCredentials(char *argumento)
{
    FILE *fp;
    fp = fopen("credenciais.txt", "r");
    if (fp == NULL)
    {
        printf("Erro ao abrir o arquivo credenciais.txt");
        return -1;
    }
    char linha[100];
    while (fgets(linha, 100, fp) != NULL)
    {
        if (strstr(linha, argumento) != NULL)
        {
            fclose(fp);
            return 0;
        }
    }

    return -1;
}
void loggoutInterno(char *argumento)
{
    changeStatus(argumento, 0);
}

int logginInterno(char *argumento)
{
    if (getCredentials(argumento) == 0)
    {
        changeStatus(argumento, 1);
        return 0;
    }
    return -1;
}

int main(int argc, char **argv)
{
    int sockfd, new_fd;            // listen on sock_fd, new connection on new_fd
    struct sockaddr_in my_addr;    // my address information
    struct sockaddr_in their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    int myPort;
    int retVal;
    char buf[MAXDATASIZE];

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <Port Number>\n", argv[0]);
        exit(-1);
    }
    myPort = atoi(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(-2);
    }
    /*
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(-3);
        }
    */

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(myPort);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    while (1)
    { // main accept() loop
        printf("Accepting ....v\n");
        retVal = 0;
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
        if (!fork())
        { // this is the child process
            signal(SIGCHLD, SIG_IGN);
            sleep(5);
            do
            {
                /* Zeroing buffers */
                memset(buf, 0x0, MAXDATASIZE);
                /* Receives client message */
                int message_len;
                if ((message_len = recv(new_fd, buf, 1000, 0)) > 0)
                {
                    char aux[100];
                    strcpy(aux, buf);
                    // a primeira palavra do buffer é o comando
                    char *comando = strtok(buf, " ");
                    // o resto do buffer é o argumento
                    char *argumento = strtok(NULL, "\0");

                    // verificar se o comando é "loggin"
                    if (strcmp(comando, "loggin") == 0) // Funciona
                    {
                        if (logginInterno(argumento) == 0)
                        {
                            char user[200];
                            sprintf(user, "setusuario %s", argumento);
                            send(new_fd, user, strlen(user), 0);
                        }
                        else
                        {
                            send(new_fd, "error Usuario ou senha Nao encontrados", 30, 0);
                        }
                        // saveDns(argumento, inet_ntoa(their_addr.sin_addr));
                    }
                    else if (strcmp(comando, "loggout") == 0) // Funciona
                    {
                        loggoutInterno(argumento);
                        send(new_fd, "quit ", 17, 0);
                    }
                    else if (strcmp(comando, "sendMessage") == 0) // funciona
                    {
                        if (saveMessage(aux) == 0)
                        {
                            send(new_fd, "message enviado com sucesso", 30, 0);
                        }
                        else
                        {
                            send(new_fd, "message nao enviado", 20, 0);
                        }
                    }
                    else if (strcmp(comando, "checkStatus") == 0)
                    {
                        char resposta[100];
                        int res = checkStatus(aux);
                        if (res == 0)
                        {
                            sprintf(resposta, "message %s online", argumento);
                        }
                        else if (res == -1)
                        {
                            sprintf(resposta, "message %s offline", argumento);
                        }
                        else
                        {
                            sprintf(resposta, "error %s nao encontrado", argumento);
                        }
                        send(new_fd, resposta, strlen(resposta), 0);
                    }

                    else if (strcmp(comando, "checkMessageUpdate") == 0)
                    {
                        char *message = checkMessage(aux);
                        //se a mensagem estiver vazia, não retorna nada
                        // se a mensagem de resposta for 'no' não envia nada
                        printf("Mensagem: %s\n", message);
                        if (strcmp(message, "no") != 0)
                        {
                            //acressentar ao inicio da mensagem a palavra message
                            char aux2[100];
                            sprintf(aux2, "message %s", message);  
                            printf("Mensagem: %s\n", aux2);  
                            send(new_fd, aux2, strlen(aux2), 0);
                        }
                    }
                    else if(strcmp(comando,"deleteUser")==0){
                        if(deleteUser(aux)==0){
                            send(new_fd,"message Usuario deletado com sucesso",28,0);
                        }
                    }
                    else // Caso não seja nenhum dos comando acima
                    {
                        printf("Comando: %s\n", comando);
                        send(new_fd, "message comando invalido", 22, 0);
                    }
                }

                /* 'bye' message finishes the connection */
                if (strcmp(buf, "sair") == 0)
                {
                    send(new_fd, "Finalizando conexao", 19, 0);
                }

            } while (strcmp(buf, "sair"));
        }
        close(new_fd);
    }

    return 0;
}
