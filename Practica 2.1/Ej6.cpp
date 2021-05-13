#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#define MAX_THREAD 2

class UDPThread
{
    public:
    UDPThread(int socket_s) : s(socket_s) {};

    void message() 
    {
        char buffer[40];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockaddr client;
        socklen_t clientLen = sizeof(struct sockaddr);

        do{
            int bytes = recvfrom(s,(void *) buffer,  40, 0, &client, &clientLen);
            sleep(2);

            time_t rawT;
            struct tm *info;
            time_t realT = time(&rawT);
            info = localtime(&realT);
            char * hour = (char*) malloc(sizeof(char) * 80);

            if(buffer[0] == 't')
            {
                int hourSize = strftime(hour, 80, "%X %p", info);
                sendto(s, hour, hourSize + 1, 0, &client, clientLen);
            }
            else if(buffer[0] == 'd')
            {
                int hourSize = strftime(hour, 80, "%Y - %m - %d", info);
                sendto(s, hour, hourSize, 0, &client, clientLen);
            }
            else
                std::cout << "Comando no soportado " << buffer[0] << "\n";

            
            getnameinfo(&client, clientLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
            std::cout <<"Thread " << std::this_thread::get_id() << ": " << bytes << " bytes recibidos de " << host << ":" << serv << "\n";

        }while(true);

    };

    private:
    int s;
};

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
    
    if(bind(s, res->ai_addr, res->ai_addrlen) < 0)
    {
        std::cout << "[bind] " << strerror(errno) << "\n";
        return -errno;
    }

    freeaddrinfo(res);



    for (size_t i = 0; i < MAX_THREAD; i++)
    {
        UDPThread* thread = new UDPThread(s);
        std::thread([&thread](){thread->message(); delete thread;}).detach();
    }
    

    char* exit;
    do
    { 
    std::cin >> exit;
    }while(strcmp(exit, "q\0"));
            
    std::cout << "Saliendo...\n";
    
    if(close(s) < 0)
    {
        std::cout << "[close] " << strerror(errno) << "\n";
        return -errno;
    }
    
    return 0;    
}