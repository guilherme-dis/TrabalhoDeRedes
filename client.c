/*
** client.c -- a stream socket client demo
** Client side program that is able to establish a connection with a server
** receive a message and print the received string.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 3490 // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

struct paramRotina
{
    int sockfd;
    int kill;
};

enum t_state
{
    IDLE,
    CONNECTED,
    HHHH
} state;

int sockfd, numbytes;

void sigControlC()
{
    printf("Control C pressed\n");
    close(sockfd);
    exit(0);
}

void sigHandler()
{
    fprintf(stderr, "*** Pegou um sinal ***\n");
}

void setUser(const char *username)
{
    FILE *fp;

    fp = fopen("user.txt", "w");
    if (fp == NULL)
    {
        perror("Erro ao abrir arquivo");
        return;
    }

    fprintf(fp, "%s", username);

    fclose(fp);
}

int userIsSet()
{
    FILE *fp;
    int c;

    fp = fopen("user.txt", "r");
    if (fp == NULL)
    {
        return 0;
    }

    c = fgetc(fp);
    if (c == EOF || c == ' ' || c == '\n')
    {
        fclose(fp);
        return 0;
    }
    else
    {
        fclose(fp);
        return 1;
    }
}

void getUser(char *user)
{
    FILE *file;
    file = fopen("user.txt", "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    fgets(user, MAXDATASIZE, file);
    fclose(file);
}

void parse(char *input, char *comando, char *argumento)
{
    char *pt;

    strcpy(comando, "");
    strcpy(argumento, "");

    pt = strtok(input, " ");
    strcpy(comando, pt);

    pt = strtok(NULL, "\0");
    strcpy(argumento, pt);

    strcpy(input, "");
}

int checkInput(char *comando, char *argumento)
{
    if (strcmp(comando, "quit") == 0)
    {
        printf("\nDeslogando...\n");
        setUser("");
        return 1;
    }
    else if (strcmp(comando, "setusuario") == 0)
    {
        printf("\nSetando usuario\n");
        setUser(argumento);
        return 0;
    }
    else if (strcmp(comando, "message") == 0)
    {
        printf("Mensagem recebida: %s\n", argumento);
        return 0;
    }

    else if (strcmp(comando, "message") == 0)
    {
        printf("Error: %s\n", argumento);

        return 0;
    }
    else
    {
        // Comando inválido
        printf("Primitiva recebida invalida\n");
        return 0;
    }
}

int *rotina_mensagem(struct paramRotina *param)
{
    char str[MAXDATASIZE];
    char usuario[MAXDATASIZE];

    char resposta[MAXDATASIZE];
    strcpy(resposta, "");

    char comando[MAXDATASIZE], argumento[MAXDATASIZE];

    while (1)
    {
        if ((numbytes = read(sockfd, resposta, MAXDATASIZE)) > 0)
        {
            parse(resposta, comando, argumento);
            param->kill = checkInput(comando, argumento);
        }

        sleep(1);

        if (param->kill == 0 && userIsSet())
        {
            getUser(usuario);
            strcpy(str, "checkMessageUpdate ");
            strcat(str, usuario);
            write(param->sockfd, str, strlen(str));
        }
        else
        {
            pthread_exit(0);
        }
    }
}

int main(int argc, char *argv[])
{

    char iBuf[MAXDATASIZE + 1];
    char oBuf[MAXDATASIZE + 1];
    struct hostent *he;
    struct sockaddr_in their_addr; // connector's address information
    int myPort;

    signal(1, sigHandler);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigControlC);

    state = IDLE;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <Hostname> <Port Number>\n", argv[0]);
        exit(1);
    }

    if ((he = gethostbyname(argv[1])) == NULL)
    { // get the host info
        perror("gethostbyname");
        exit(1);
    }

    myPort = atoi(argv[2]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;     // host byte order
    their_addr.sin_port = htons(myPort); // short, network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(their_addr.sin_zero), '\0', 8); // zero the rest of the struct

    printf("Going to be connected ....\n");

    if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        fprintf(stderr, "connect: ERRNO %d\n", errno);
        exit(1);
    }

    printf("Connected ....\n");

    pthread_t rotina;
    struct paramRotina param;
    param.kill = 0;
    param.sockfd = sockfd;
    int final;

    pthread_create(&rotina, NULL, rotina_mensagem, &param);
    char comando[MAXDATASIZE], argumento[MAXDATASIZE];
    while (1)
    {
        memset(oBuf, 0, MAXDATASIZE);
        printf("Input: ");
        fgets(oBuf, MAXDATASIZE - 1, stdin);
        oBuf[strlen(oBuf) - 1] = 0;

        write(sockfd, oBuf, strlen(oBuf));

        printf("Waiting....\n");

        sleep(3);
        if (strcmp(oBuf, "loggout") == 0)
        {
            printf("Fechando..\n");
            pthread_cancel(rotina);
            close(sockfd);
            break;
        }
    }
}
