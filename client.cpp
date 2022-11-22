#include "common.h"

int main () {
    net::ConnectingSocket sock;

    sock.send_message("Hello, world!");
    return 0;
}