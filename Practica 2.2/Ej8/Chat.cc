#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    char* tmp = _data;

    memcpy(tmp, &type, 1);
    tmp++;
        
    memcpy(tmp, nick.c_str(), 8);
    tmp += 8;

    memcpy(tmp, message.c_str(), 80);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char* tmp = _data;

    memcpy(&type, tmp, 1);
    tmp++;
        
    nick = tmp;
    tmp += 8;

    message = tmp;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{

    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        ChatMessage msg;
        Socket* s;
        socket.recv(msg, s);
        s->bind();

        switch (msg.type)
        {
            case ChatMessage::LOGIN:
            {    
                std::cout << "Conectado: " << msg.nick << std::endl;
                clients.push_back(std::move(std::make_unique<Socket>(*s)));
                break;
            }
            case ChatMessage::LOGOUT:
            {
                std::cout << "Desconectando a: " << msg.nick << std::endl;
                auto it = clients.begin();
                while (it != clients.end() && it->get() != s) it++;

                if(it != clients.end())
                {
                    std::cout << "Desconectado a: " << msg.nick << std::endl;
                    clients.erase(it);
                    it->release();
                }

                break;
            }
            case ChatMessage::MESSAGE:
            {
                std::cout << "Mensaje de: " << msg.nick << std::endl;
                auto it = clients.begin();
                while (it != clients.end())
                {
                    if(!(*(it->get()) == *s)) socket.send(msg, *it->get());
                    it++;
                }
                
                break;
            }
        }

    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        std::string msg;
        std::getline(std::cin, msg);
        ChatMessage em(nick, msg);
        em.type = ChatMessage::MESSAGE;

        // Enviar al servidor usando socket
        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        ChatMessage msg;
        socket.recv(msg);

        std::cout << msg.nick << ": " << msg.message << "\n";
    }
}

