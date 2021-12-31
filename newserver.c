#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define OUTPUT_BUFFER_LEN 1024

enum FileDesc { STDIN, STDOUT, STDERR };

void initialiser(int* inputFd, int* outputFd, char** prog);
void envoyer(const char* input, char** output);
void nettoyer();

int inputFd, outputFd;
char outputBuffer[OUTPUT_BUFFER_LEN];

int main(int argc, char* argv[])
{
	initialiser(&inputFd, &outputFd, argv);

    envoyer("test", (char**)&outputBuffer);

    printf("%s", outputBuffer); //debug

    nettoyer(); // on arrive jamais ici si le prog a une boucle infinie
}

void initialiser(int* inputFd, int* outputFd, char** prog)
{
    int parentPipe[2];
    pipe(parentPipe);

    int childPipe[2];
    pipe(childPipe);
    
	pid_t c_pid = fork();
    if(c_pid == 0) 
    { 
        /* child process */

        // on libere stdout pour qu'il soit pris dans le dup (lowest fd nb)
		close(STDOUT);
        // on duplique stdout vers le stdout du processus parent
		dup(parentPipe[STDOUT]);
        // on libere stdin pour qu'il soit pris dans le dup (lowest fd nb)
		close(STDIN);
        // on duplique stdin vers le stdin du processus enfant
		dup(childPipe[STDIN]);

        /* on libère les file descriptors */
		close(parentPipe[STDIN]);
		close(parentPipe[STDOUT]);
		close(childPipe[STDIN]);
		close(childPipe[STDOUT]);

        // on lance le programme avec ses arguments
		execvp(prog[1], &prog[1]);
	}
    else if(c_pid > 0)
    {
		/* parent process */

        // on affecte la stdout du processus enfant à l'input fd
		*inputFd = childPipe[STDOUT];
        // on affecte la stdin du processus parent à l'output fd
		*outputFd = parentPipe[STDIN];

        /* on libère les file descriptors */
		close(parentPipe[STDOUT]);
		close(childPipe[STDIN]);
    }
    else
    {
		perror("fork() error");
		exit(-1);
    }
}

void envoyer(const char* input, char** output)
{
    // on écrit l'input dans le file desc correspondant
    write(inputFd, input, strlen(input));
    // on lit l'output depuis le file desc correspondant
	read(outputFd, output, OUTPUT_BUFFER_LEN);
}

void nettoyer()
{
    /* on libère les file descriptors */
    close(inputFd);
	close(outputFd);
}
