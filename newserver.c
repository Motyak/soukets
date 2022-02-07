#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pty.h>
#include <utmp.h>
#include <sysexits.h>
#include <signal.h>

#define OUTPUT_BUFFER_LEN 256UL

enum FileDesc { STDIN, STDOUT, STDERR };
enum PipeSide {OUT, IN};

void initialiser(int* master_fd, int* slave_fd, pid_t* child_pid, char** prog);
void envoyer(const char* msg, size_t msgLen, char* output);
void nettoyer();

// master and slave pty fd
int mfd, sfd;
// child process pid
pid_t c_pid;
char outputBuffer[OUTPUT_BUFFER_LEN];

/* ./newserver ./echo
*  gcc newserver.c -o newserver -lutil */
int main(int argc, char* argv[])
{
    initialiser(&mfd, &sfd, &c_pid, argv);

    const char* msg = "salut\na\n\ntous\n\x4";
    envoyer(msg, sizeof(msg), outputBuffer);

    printf(">%s<", outputBuffer);

    nettoyer();
}

void initialiser(int* master_fd, int* slave_fd, pid_t* child_pid, char** prog)
{
    if(openpty(master_fd, slave_fd, NULL, NULL, NULL) == -1)
    {
        perror("openpty() error");
        exit(EX_OSERR);
    }

    *child_pid = fork();

    if(*child_pid == -1)
    {
        perror("fork() error");
        exit(EX_OSERR);
    }

    if(*child_pid == 0)
    {
        /* child process */

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
    }

    {
        /* parent process */
    }
}

void envoyer(const char* msg, size_t msgLen, char* output)
{
    /* on redirige le message vers le processus fils */
    if(write(mfd, msg, msgLen) == -1)
    {
        perror("write() error");
        exit(EX_IOERR);
    }

    /* on copie la réponse du processus fils */
    if(read(sfd, output, OUTPUT_BUFFER_LEN) == -1)
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
