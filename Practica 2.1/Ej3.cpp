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
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], "1243", &hints, &res);

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

    struct addrinfo * res2;

    int rt = getaddrinfo(argv[1], argv[2], &hints, &res2);

    if(rt != 0)
    {
        std::cerr << "[getaddrinfo] " << gai_strerror(rt) << std::endl;
        return -1;
    }
    char buffer[40];

    sendto(s, argv[3], 1, 0, res2->ai_addr, res2->ai_addrlen);
    
    int bytes = recvfrom(s,(void *) buffer,  40, 0, res2->ai_addr, &res2->ai_addrlen);
    if(bytes < 0) return -1;

    close(s);
    
    freeaddrinfo(res2);
    std::cout << buffer << "\n";


    return 0;    
}