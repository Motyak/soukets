// #include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// #include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

// gcc tcpclient.c
int main()
{
    /* create a socket */
    // AF_INET      IPv4 Internet protocols
    // SOCK_STREAM  Provides sequenced, reliable, two-way, connection-based byte streams
    // The protocol specifies a particular protocol to be used with the socket
    int s = socket(AF_INET, SOCK_STREAM, 0);

    /* specify an address for the socket */
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1337);
    addr.sin_addr.s_addr = INADDR_ANY;

    int status = connect(s, (struct sockaddr*)&addr, sizeof(addr));

    if(status == -1)
    {
        perror("Erreur connexion");
        return -1;
    }

    // receive data from the server
    char response[256];
    recv(s, &response, sizeof(response), 0);

    // print out the response
    printf("%s", response);

    // and then close the socket
    close(s);

    return 0;
}