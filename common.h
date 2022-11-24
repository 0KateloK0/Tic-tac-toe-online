#ifndef HW_SOCKET_COMMON_H
#define HW_SOCKET_COMMON_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <iostream>
#include <sstream>

namespace net {
    static const size_t buffer_size = 1024; // it's going be same everywhere anyway
    using buffer_t = std::array<char, buffer_size + 1>; // +1 is for \0

    class Socket {
    public:
        Socket();
        explicit Socket(int sock);
        ~Socket();
        operator int() const;
    protected:
        int sock;
        bool is_open;

        // every connection in this app will be done through this address
        // you can even say that I'm creating a protocol here)))
        static const in_port_t port;
        static sockaddr_in address;
    };

    class AcceptingSocket : public Socket {
    public:
        AcceptingSocket();
        ~AcceptingSocket() = default;
        int accept_connection() const;
    };

    class TCPConnectionSocket : public Socket {
    public:
        TCPConnectionSocket();
        explicit TCPConnectionSocket(int sock);
        ~TCPConnectionSocket() = default;
        void send_message(std::string const&) const;
        std::string get_message() const;
    };

    // Okay, protocol for server to report to players in format STATUS\nMESSAGE
    enum app_status {
        PLAYER1_CONNECTED, // maybe it'll work?
        PLAYER2_CONNECTED,
        PLAYER1_DISCONNECTED,
        PLAYER2_DISCONNECTED,
        GAME_STARTED,
        INCORRECT_MOVE,
        PLAYER1_MADE_MOVE,
        PLAYER2_MADE_MOVE,
        PLAYER1_WON,
        PLAYER2_WON,
        GAME_DRAWN,
        GAME_ENDED,
        SERVER_BROKE,
    };
    // Enum TO String
    std::string etos(app_status status);
}


#endif //HW_SOCKET_COMMON_H
