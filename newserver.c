#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define OUTPUT_BUFFER_LEN 256

enum FileDesc { STDIN, STDOUT, STDERR };
enum PipeSide {OUT, IN};

void initialiser(int* inputFd, int* outputFd, char** prog);
void envoyer(const char* msg, int msgLen, char* output);

int fd_ecriture, fd_lecture;
char outputBuffer[OUTPUT_BUFFER_LEN];

// ./newserver ./echo
int main(int argc, char* argv[])
{
	initialiser(&fd_ecriture, &fd_lecture, argv);
    // envoyer("test\n" + (char)4, strlen("test\n" + (char)4), outputBuffer);
    envoyer("test\n", strlen("test\n"), outputBuffer);
    printf("%s\n", outputBuffer);
}

void initialiser(int* inputFd, int* outputFd, char** prog)
{
    /* contient fd_ecriture en entrée et la stdin du child en sortie */
    int input_pipe[2];
    pipe(input_pipe);

    /* contient la stdout du child en entrée et fd_lecture en sortie */
    int output_pipe[2];
    pipe(output_pipe);
    
	pid_t c_pid = fork();
    if(c_pid == 0)
    {
        /* child process */

        // on libere stdout pour qu'il soit pris dans le dup (lowest fd nb)
		close(STDOUT);
        // on duplique stdout en entrée du pipe output_pipe
        dup(output_pipe[IN]);
        // on libere stdin pour qu'il soit pris dans le dup (lowest fd nb)
		close(STDIN);
        // on duplique stdin en sortie du pipe input_pipe
        dup(input_pipe[OUT]);

        /* on libère les file descriptors */
        close(input_pipe[0]);
		close(input_pipe[1]);
        close(output_pipe[0]);
        close(output_pipe[1]);

        // on execute le programme avec ses arguments
		execvp(prog[1], &prog[1]);
	}
    else if(c_pid > 0)
    {
		/* parent process */

        // on associe la sortie du pipe input au fd d'input
        *inputFd = input_pipe[IN];
        // on associe l'entrée du pipe output au fd d'output
        *outputFd = output_pipe[OUT];

        /* on libère les file descriptors */
		close(input_pipe[OUT]);
        close(output_pipe[IN]);

        // // on attend que le processus fils se termine
        // wait(NULL);
    }
    else
    {
		perror("fork() error");
		exit(-1);
    }
}

void envoyer(const char* msg, int msgLen, char* output)
{
	write(fd_ecriture, msg, msgLen);
    close(fd_ecriture);
	read(fd_lecture, output, OUTPUT_BUFFER_LEN);
}
