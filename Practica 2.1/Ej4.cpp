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
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0)
    {
        std::cerr << "[getaddrinfo] " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int s = socket(res->ai_family, res->ai_socktype, 0);

    if(s == -1)
    {
        std::cout << "[socket] " << strerror(errno) << "\n";
    }
    
    if(bind(s, res->ai_addr, res->ai_addrlen) < 0)
    {
        std::cout << "[bind] " << strerror(errno) << "\n";
        return -errno;
    }

    if(listen(s, 0) < 0)
    {
        std::cout << "[listen] " << strerror(errno) << "\n";
        return -errno;
    }
    freeaddrinfo(res);

    char buffer[40];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr client;
    socklen_t clientLen = sizeof(struct sockaddr);
 
    int client_s = accept(s, &client, &clientLen);
    int bytes;
    
    getnameinfo(&client, clientLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
    std::cout << "Conexion desde " << host << ":" << serv << "\n";

    while(1)
    {
        int i = 0;
        bytes = recv(client_s, (void *) buffer , 39, 0);        
        if(bytes <= 0) break;
        buffer[bytes] = '\0';
        send(client_s, buffer, bytes + 1, 0);
    }

    std::cout << "Conexion Terminada\n";
    
    if(close(s) < 0)
    {
        std::cout << "[close] " << strerror(errno) << "\n";
        return -errno;
    }

    if(close(client_s) < 0)
    {
        std::cout << "[close] " << strerror(errno) << "\n";
        return -errno;
    }

    return 0;    
}