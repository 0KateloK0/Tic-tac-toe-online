#ifndef HW_SOCKET_COMMON_H
#define HW_SOCKET_COMMON_H

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <iostream>

namespace net {
    class Socket {
    public:
        Socket();
        ~Socket();
    protected:
        bool is_created;
        int socket_fd;
    };

    class AcceptingSocket : public Socket {
    public:
        AcceptingSocket();
        ~AcceptingSocket() = default;
        int accept_connection();
        std::string get_message (int accepted_socket);
    protected:

    };

    class ConnectingSocket : public Socket {
    public:
        ConnectingSocket();
        ~ConnectingSocket() = default;
        void send_message(std::string const& msg);
    protected:

    };
    static const in_port_t port = htons(12345);
}


#endif //HW_SOCKET_COMMON_H
