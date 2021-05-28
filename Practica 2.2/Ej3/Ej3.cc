#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

static const size_t MAX_NAME = 20;

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t data_size = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);

        char* tmp = _data;

        memcpy(tmp, name, MAX_NAME * sizeof(char));
        tmp += MAX_NAME * sizeof(char);
        
        memcpy(tmp, &x, sizeof(int16_t));
        tmp +=  sizeof(int16_t);

        memcpy(tmp, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        char* tmp = data;
        int32_t data_size = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);

        memcpy(name, tmp, MAX_NAME * sizeof(char));
        tmp += MAX_NAME * sizeof(char);

        memcpy(&x, tmp, sizeof(int16_t));
        tmp +=  sizeof(int16_t);
        
        memcpy(&y, tmp, sizeof(int16_t));
        
        return 0;
    }


public:

    char name[MAX_NAME];
    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);
    int fd = open("./playerdata.data", O_CREAT|O_TRUNC|O_RDWR, 0666);
    if(fd < 0)
    {
        std::cout << "[open] " << strerror(errno) << "\n";
        return -errno;
    }

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    // 2. Escribir la serialización en un fichero
    int written = write(fd, one_w.data(), one_w.size());
        
    if(written < 0)
    {
        std::cout << "[write] " << strerror(errno) << "\n";
        return -errno;
    }

    int e = close(fd);

    if(e < 0)
    {
        std::cout << "[close] " << strerror(errno) << "\n";
        return -errno;
    }

    fd =  open("./playerdata.data", O_RDONLY , 0444);
    if(fd < 0)
    {
        std::cout << "[open] " << strerror(errno) << "\n";
        return -errno;
    }

    char* tmp = (char*) malloc(written * sizeof(char));

    e = read(fd, tmp, written);
    if(e < 0)
    {
        std::cout << "[read] " << strerror(errno) << "\n";
        return -errno;
    }

    one_r.from_bin(tmp);
    free(tmp);
    std::cout << "Name: " << one_r.name << "\nPos: " << one_r.x << ", " << one_r.y << "\n";
    return 0;
}

