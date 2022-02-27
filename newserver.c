#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pty.h>
#include <utmp.h>
#include <sysexits.h>
#include <signal.h>
#include <string.h>
#include <termios.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_LEN 256UL

void initialiser(int* master_fd, int* slave_fd, pid_t* child_pid, char** prog);
void envoyer(const char* msg, size_t msgLen);
void traiter(char* output);
void nettoyer();

// master and slave pty fd
int mfd, sfd;

// child process pid
pid_t c_pid;

char inputBuffer[BUFFER_LEN];
char outputBuffer[BUFFER_LEN];

/* 
gcc newserver.c -o newserver -lutil
./newserver ./echo
*/
int main(int argc, char* argv[])
{
    // initialiser(&mfd, &sfd, &c_pid, argv);

    // const char* msg = "salut\na\n\ntous";
    // envoyer(msg, strlen(msg));

    // traiter(outputBuffer);

    // printf(">%s<", outputBuffer);

    // nettoyer();

    // exit(EX_OK);


    

    /* create a socket */
    // AF_INET      IPv4 Internet protocols
    // SOCK_STREAM  Provides sequenced, reliable, two-way, connection-based byte streams
    // The protocol specifies a particular protocol to be used with the socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("socket() error");
        exit(EX_OSERR);
    }

    /* specify an address for the socket */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9191);

    /* bind the socket to our specified IP and port */
    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("bind() error");
        exit(EX_OSERR);
    }

    // initialize subprocess communication
    initialiser(&mfd, &sfd, &c_pid, argv);

    /* prepare to accept a single connection at a time */
    if(listen(sock, 1) == -1)
    {
        perror("listen() error");
        exit(EX_OSERR);
    }

    /* await a connection on the socket */
    int client_socket = accept(sock, NULL, NULL);
    if(client_socket == -1)
    {
        perror("accept() error");
        exit(EX_OSERR);
    }

    /* receive tcp message from client */
    if(recv(client_socket, inputBuffer, BUFFER_LEN, 0) == -1)
    {
        perror("recv() error");
        exit(EX_OSERR);
    }

    printf(">%s<\n", inputBuffer); //debug

    envoyer(inputBuffer, strlen(inputBuffer));
    traiter(outputBuffer);

    printf(">%s<", outputBuffer); //debug

    // /* send response back to client */
    // if(send(client_socket, outputBuffer, strlen(outputBuffer), 0) == -1)
    // {
    //     perror("send() error");
    //     exit(EX_OSERR);
    // }

    // close fds and kill subprocess
    nettoyer();

    /* close sockets */
    close(client_socket);
    close(sock);

    exit(EX_OK);
}

void initialiser(int* master_fd, int* slave_fd, pid_t* child_pid, char** prog)
{
    struct termios pty;
    if(openpty(master_fd, slave_fd, NULL, &pty, NULL) == -1)
    {
        perror("openpty() error");
        exit(EX_OSERR);
    }

    *child_pid = fork();

    switch(*child_pid)
    {
        /* error on fork() */
        case -1:
            close(*master_fd);
            close(*slave_fd);
            perror("fork() error");
            exit(EX_OSERR);

        /* child process */
        case 0:
            // on ferme le fd qu'on utilise pas
            close(*master_fd);
            
            /* on ouvre le pty slave dans le processus fils */
            if(login_tty(*slave_fd) != 0)
            {
                perror("login_tty() failed");
                exit(EX_OSERR);
            }

            // on execute le programme avec ses arguments
            execvp(prog[1], prog + 1);

            perror("execvp() failed");
            exit(EX_OSERR);

        /* parent process */
        default:
            ;
    }
}

void envoyer(const char* msg, size_t msgLen)
{
    if(write(mfd, msg, msgLen) == -1)
    {
        perror("write() error");
        exit(EX_IOERR);
    }
}

void traiter(char* output)
{
    /* on signale la fin d'input */
    if(write(mfd, "\x4", sizeof("\x4")) == -1)
    {
        perror("write() error");
        exit(EX_IOERR);
    }

    /* on copie la réponse du processus fils */
    if(read(sfd, output, BUFFER_LEN) == -1)
    {
        perror("read() error");
        exit(EX_IOERR);
    }
}

void nettoyer()
{
    close(mfd);
    close(sfd);
    kill(c_pid, SIGKILL);
}
