#include <stdio.h> // printf(), perror()
#include <stdlib.h> // exit()
#include <unistd.h> // close(), fork(), read()
#include <string.h> // bzero()

#include <netinet/in.h>

#define SOUKETS_HOST INADDR_ANY
#define SOUKETS_PORT 9191
// #define SOUKETS_CHANNEL_SIZE 2
#define SOUKETS_MAX_CLIENTS 10 //as an example

// int SIZEOF_SOCKADDR_IN = sizeof(struct sockaddr_in);

void error(const char* msg)
{
    perror(msg);
    exit(1);
}

// gcc multiserver.c -o multiserver
int main()
{
    char buffer[256];

    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("ERROR opening socket");
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = SOUKETS_HOST;
    serv_addr.sin_port = htons(SOUKETS_PORT);
    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    if(listen(sockfd, SOUKETS_MAX_CLIENTS) < 0)
        error("ERROR on listening");

    int newsockfd;
    pid_t pid;
    struct sockaddr_in cli_addr;
    socklen_t cli_addr_size;
    while(1)
    {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_addr_size);
        if(newsockfd < 0)
            error("ERROR on accept");

         pid = fork();
         if(pid < 0)
              error("ERROR in new process creation");

        /* child process */
         if(pid == 0)
        {
            close(sockfd);
            sockfd = -1;

            while(1)
            {
                if(recv(newsockfd, buffer, sizeof(buffer), 0) < 0)
                    error("ERROR reading from socket");
                if(strcmp(buffer, "\\\n") == 0)
                    break;
                printf("%s", buffer);
                bzero(buffer, sizeof(buffer));
            }
        }

        /* parent process */
        else // if pid > 0
        {
            close(newsockfd);
            newsockfd = -1;
        }
    }

    return 0;
}
