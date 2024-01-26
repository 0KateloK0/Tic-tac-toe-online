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
        TCPConnectionSocket(TCPConnectionSocket const&) = default;
        TCPConnectionSocket& operator=(TCPConnectionSocket const&) = default;

        void send_message(std::string const&) const;
        std::string get_message() const;
    };

    // Protocol for server to report to players in format: STATUS\nMESSAGE
    enum app_status {
        PLAYER1_CONNECTED,
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
