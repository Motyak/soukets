#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
//#include <sys/types.h>
#include <string.h>

int NbFichier2(char* repertoire);

int main(int argc, char* argv[])
{
	printf("res commande = %d\n",NbFichier2(argv[1]));
	printf("bonjour\n");
	
}

int NbFichier2(char* repertoire)
{
	int status;
    pid_t c_pid1,c_pid2;
    int f[2];
    int f2[2];

    pipe(f2);
    c_pid1=fork();
    if(c_pid1==0)
    {
        pipe(f);
        c_pid2=fork();
        if(c_pid2==0)
        {
            //fils2
            
            // redirection de sortie std vers pipe f
            close(1);
            dup(f[1]);

            close(f[0]);
            close(f[1]);
            close(f2[0]);
            close(f2[1]);
            execlp("ls","ls",repertoire,NULL);
        }
        else if(c_pid2>0)
        {
            // fils1

            // wait que le fils2 est terminé
            wait(&status);
            // redirection pipe f vers input
            close(0);
            dup(f[0]);

            // redirection sortie std vers pipe f2
            close(1);
            dup(f2[1]);

            close(f[0]);
            close(f[1]);
            close(f2[0]);
            close(f2[1]);
            execlp("wc","wc","-l",NULL);
        }
        else
        {
            perror("2nd fork() error");
            exit(-1);
        }
        
    }
    else if(c_pid1>0)
    {
        //parent

        // wait fils1
        wait(&status);

        // lecture du pipe f2[1] dans une variable
        char buf[20];
        read(f2[0],buf,20);

        close(f[0]);
        close(f[1]);
        close(f2[0]);
        close(f2[1]);

        return atoi(buf);
    }
    else
    {
        perror("1st fork() error");
        exit(-1);
    }
}
