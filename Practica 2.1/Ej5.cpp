#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        std::cout << "Formato incorrecto \n";
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo * res;

    memset((void *) & hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int s = socket(AF_INET, SOCK_STREAM, 0);

    if(s == -1)
    {
        std::cout << "[socket] " << strerror(errno) << "\n";
    }

    char buffer[40];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr server;
    socklen_t serverLen = sizeof(struct sockaddr);
    getaddrinfo(argv[1],argv[2], &hints, &res);
    connect(s, res->ai_addr, res->ai_addrlen);
    int bytes;

    do
    {
        std::cin >> buffer;
        int i = 0;
        send(s, buffer, strlen(buffer), 0);
        bytes = recv(s, (void *) buffer , 39, 0);        
        if(bytes == 0) break;
        else if(bytes < 0) std::cout << "[recv] " << strerror(errno) << "\n";
        std::cout << buffer <<"\n";
    } while(strcmp(buffer, "Q\0"));

    if(close(s) < 0)
    {
        std::cout << "[close] " << strerror(errno) << "\n";
        return -errno;
    }

    return 0;    
}