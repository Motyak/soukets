#include <stdio.h>
#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>

#define SOUKETS_HOST INADDR_ANY
#define SOUKETS_PORT 1337
#define SOUKETS_CHANNEL_SIZE 2
#define SOUKETS_MAX_CLIENTS 10

int SIZEOF_SOCKADDR_IN = sizeof(struct sockaddr_in);

// gcc soukets.c -o server
int main()
{
    int s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = SOUKETS_HOST;
    addr.sin_port = htons(SOUKETS_PORT);

    bind(s, (struct sockaddr*)&addr, sizeof(addr));

    listen(s, SOUKETS_MAX_CLIENTS);

    int nb_clients = 0;
    int clients_fd[SOUKETS_MAX_CLIENTS];
    struct sockaddr_in clients_info[SOUKETS_MAX_CLIENTS];

    // while(1)
    // {
        for(int i = 0; i < SOUKETS_CHANNEL_SIZE; ++i)
        {
            // clients_fd[nb_clients] = accept(s, 0, 0);
            clients_fd[nb_clients] = accept(s, (struct sockaddr*)&clients_info[nb_clients], &SIZEOF_SOCKADDR_IN);
            printf("client connected\n");
            ++nb_clients;
        }
        // je crée un subprocess qui envoie à chaque client
        // du channel un message de bienvenue
        char msg1[20] = "bonjour client 1\n";
        char msg2[20] = "bonjour client 2\n";
        send(clients_fd[0], &msg1, sizeof(msg1), 0);
        send(clients_fd[1], &msg2, sizeof(msg2), 0);
    // }

    close(s);

    return 0;
}
