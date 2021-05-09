#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>

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
    hints.ai_socktype = SOCK_DGRAM;

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
    
    bind(s, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    char buffer[1];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr client;
    socklen_t clientLen = sizeof(struct sockaddr);

    int bytes = recvfrom(s,(void *) buffer,  40, 0, &client, &clientLen);
    if(bytes < 0) return -1;
 

    while(buffer[0] != 'q')
    {   
        time_t rawT;
        struct tm *info;
        time_t realT = time(&rawT);
        info = localtime(&realT);
        char * hour = (char*) malloc(sizeof(char) * 80);

        if(buffer[0] == 'd')
        {
            int hourSize = strftime(hour, 80, "%X %p", info);
            sendto(s, hour, hourSize + 1, 0, &client, clientLen);
        }
        else if(buffer[0] == 't')
        {
            int hourSize = strftime(hour, 80, "%Y - %m - %d", info);
            sendto(s, hour, hourSize, 0, &client, clientLen);
        }
       else
            sendto(s, "Comando no soportado " + buffer[0], 23, 0, &client, clientLen);

        
        getnameinfo(&client, clientLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
        std::cout << bytes << " bytes recibidos de " << host << " " << serv << "\n";

        bytes = recvfrom(s,(void *) buffer,  40, 0, &client, &clientLen);
        if(bytes < 0) return -1;
    }
    std::cout << "Saliendo...\n";
    
    return 0;    
}