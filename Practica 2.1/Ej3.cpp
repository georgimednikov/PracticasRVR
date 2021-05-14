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

    int s = socket(AF_INET, SOCK_DGRAM, 0);

    if(s == -1)
    {
        std::cout << "[socket] " << strerror(errno) << "\n";
    }

    struct addrinfo * res;
    struct addrinfo hints;

    memset((void *) & hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rt = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rt != 0)
    {
        std::cerr << "[getaddrinfo] " << gai_strerror(rt) << std::endl;
        return -1;
    }
    char buffer[40];

    sendto(s, argv[3], 1, 0, res->ai_addr, res->ai_addrlen);
    
    int bytes = recvfrom(s,(void *) buffer,  40, 0, res->ai_addr, &res->ai_addrlen);
    if(bytes < 0) return -1;

    if(close(s) < 0)
    {
        std::cout << "[close] " << strerror(errno) << "\n";
        return -errno;
    }
    
    freeaddrinfo(res);
    std::cout << buffer << "\n";

    return 0;    
}