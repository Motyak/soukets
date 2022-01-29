#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
// #include <sys/wait.h>

#define OUTPUT_BUFFER_LEN 256UL

enum FileDesc { STDIN, STDOUT, STDERR };
enum PipeSide {OUT, IN};

void initialiser(pid_t* c_pid, int* inputFd, int* outputFd, char** prog);
void envoyer(const char* msg, size_t msgLen, char* output);
void nettoyer();

const char* EOT_CHAR = "" + (char)4;

pid_t subprocess;
int fd_ecriture, fd_lecture;
char outputBuffer[OUTPUT_BUFFER_LEN];

// ./newserver ./echo
int main(int argc, char* argv[])
{
    initialiser(&subprocess, &fd_ecriture, &fd_lecture, argv);

    // const char* msg = "bonjour\na\n\ntous\n";
    const char* msg = "bonjour";
    envoyer(msg, sizeof(msg), outputBuffer);

    printf(">%s<", outputBuffer);

    nettoyer();
}

void initialiser(pid_t* c_pid, int* inputFd, int* outputFd, char** prog)
{
    /* contient fd_ecriture en entrée et la stdin du child en sortie */
    int input_pipe[2];
    if(pipe(input_pipe) == -1)
    {
        perror("pipe() error");
        exit(-1);
    }

    /* contient la stdout du child en entrée et fd_lecture en sortie */
    int output_pipe[2];
    if(pipe(output_pipe) == -1)
    {
        perror("pipe() error");
        exit(-1);
    }
    
    *c_pid = fork();
    if(*c_pid == 0)
    {
        /* child process */

        /* on ferme les fd dont on ne se sert pas */
        close(input_pipe[IN]);
        close(output_pipe[OUT]);

        // on libere stdout pour qu'il soit pris dans le dup (lowest fd nb)
        close(STDOUT);
        // on duplique stdout en entrée du pipe output_pipe
        dup(output_pipe[IN]);
        // on libere stdin pour qu'il soit pris dans le dup (lowest fd nb)
        close(STDIN);
        // on duplique stdin en sortie du pipe input_pipe
        dup(input_pipe[OUT]);

        // on execute le programme avec ses arguments
        execvp(prog[1], &prog[1]);
    }
    else if(*c_pid > 0)
    {
        /* parent process */

        /* on ferme les fd dont on ne se sert pas */
        close(input_pipe[OUT]);
        close(output_pipe[IN]);

        // on associe l'entrée du pipe input au fd d'input
        *inputFd = input_pipe[IN];
        // on associe la sortie du pipe output au fd d'output
        *outputFd = output_pipe[OUT];
        
        // // on attend que le processus fils se termine
        // wait(NULL);
    }
    else
    {
        perror("fork() error");
        exit(-1);
    }
}

void envoyer(const char* msg, size_t msgLen, char* output)
{
    /* on redirige le message vers le processus fils */
    if(write(fd_ecriture, msg, msgLen) == -1)
    {
        perror("write() error");
        exit(-1);
    }
    close(fd_ecriture);

    /* ask to flush the input buffer without closing the file descriptor */
    // if(write(fd_ecriture, EOT_CHAR, sizeof(EOT_CHAR)) == -1)
    // {
    //     perror("write() error");
    //     exit(-1);
    // }
    
    // lseek(fd_ecriture, 0, SEEK_END);

    /* on copie la réponse du processus fils */
    if(read(fd_lecture, output, OUTPUT_BUFFER_LEN) == -1)
    {
        perror("read() error");
        exit(-1);
    }
}

void nettoyer()
{
    close(fd_ecriture);
    close(fd_lecture);
    kill(subprocess, SIGKILL);
}
