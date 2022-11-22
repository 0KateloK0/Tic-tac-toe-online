#include "common.h"
#include "Game.h"

int main() {
    net::AcceptingSocket as;

    std::thread accept_thread([&](){
        while (true) {
            int con = as.accept_connection();
            if (con == -1)
                continue;
            std::string msg = as.get_message(con);
            std::cout << msg << std::endl;
            // handle game logic
        }
    });

    accept_thread.join();

    return 0;
}
