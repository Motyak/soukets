#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

void init(int* ecrit, int* lit,int* server);
int Operation(char op,int n1,int n2);
void Nettoyer();

int ecrit,lit;
int server;	//valeur ne change pas

int main(int argc, char* argv[])
{
    
    init(&ecrit,&lit,&server);
    printf("%d\n",Operation('+',1,2));
    //~ printf("bonjour\n");
    printf("%d\n",Operation('+',4,2));
    Nettoyer();
}

void init(int* ecrit, int* lit,int* s)
{
    int status;
    pid_t c_pid;
    int f[2];
    int f2[2];
    pipe(f);
    pipe(f2);
    
    c_pid=fork();
    if(c_pid == 0) 
    { 
        //server
        *s=getpid();
         printf("I'm in the child %d, my parent is %d; server is %d\n",getpid(),getppid(),server);
        close(1);	//on libere stdout
        dup(f[1]);	//on duplique stdout vers pipe f
        close(0);	//on libere stdin
        dup(f2[0]);	//on duplique stdin dans pipe f2
        close(f[0]);
        close(f[1]);
        close(f2[0]);
        close(f2[1]);
        execlp("bc","bc",NULL);
    }
    else if(c_pid>0)
    {
        //client
        *ecrit=f2[1];
        *lit=f[0];
        close(f[1]);
        close(f2[0]);
    }
    else
    {
        perror("fork() error");
        exit(-1);
    }
}

int Operation(char op,int n1,int n2)
{
    //ecrit dans le tube (f2[1] puis lit le tube f[0])
    char com[20];
    sprintf(com,"%d%c%d\n",n1,op,n2);
    write(ecrit,com,strlen(com));
    read(lit,com,20);
    int res=atoi(com);
    return res;
}

void Nettoyer()
{
    //clean les fd puis sigkill le fils
    close(ecrit);
    close(lit);
    printf("%d\n",server);
    //~ kill(server,SIGKILL);
}
