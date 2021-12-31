#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>

// gcc tcpserver.c -o server
int main()
{
    char msg[256] = "message\n";

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

    // bind the socket to our specified IP and port
    int status = bind(s, (struct sockaddr*)&addr, sizeof(addr));

    // n is the max number of connections
    listen(s, 5);

    int client_socket;
    client_socket = accept(s, 0, 0);

    // send the message
    send(client_socket, msg, sizeof(msg), 0);

    // close the socket's fd
    close(s);

    return 0;
}
