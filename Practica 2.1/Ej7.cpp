#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <thread>
#define MAX_THREAD 2

class TCPThread
{
    public:
    TCPThread(int socket_s) : s(socket_s) {};

    void setup(char _host[NI_MAXHOST], char _serv[NI_MAXSERV])
    {
        host = _host;
        serv = _serv;
    }

    void message() 
    {
        char buffer[40];

        do{
            int bytes;
            int i = 0;
            bytes = recv(s, (void *) buffer , 39, 0);        
            buffer[bytes] = '\0';
            send(s, buffer, bytes + 1, 0);
            std::cout <<"Thread " << std::this_thread::get_id() << ": " << bytes << " bytes recibidos de " << host << ":" << serv << "\n";

        }while(true);
    };

    private:
    int s;
    char* host, *serv;
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
    std::cout << "Llega?\n";
    struct sockaddr client;
    socklen_t clientLen = sizeof(struct sockaddr);
    while(true)
    { 
        int client_s = accept(s, &client, &clientLen);
        std::cout << "Accept\n";
        getnameinfo(&client, clientLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV );
        std::cout << "Conexion desde " << host << ":" << serv << "\n";

        TCPThread* thread = new TCPThread(client_s);
        thread->setup(host, serv);
        std::thread([thread](){thread->message(); delete thread;}).detach();
    }
            
    std::cout << "Saliendo...\n";
    
    if(close(s) < 0)
    {
        std::cout << "[close] " << strerror(errno) << "\n";
        return -errno;
    }
    
    return 0;    
}