#include <iostream>

#include "client.h"


int main(int argc, char **argv)
{
    if (argc > 2) {
        std::cout << "Invalid argument." << std::endl;
        return 1;
    }

    // Read message
    std::string msg;
    if (argc == 1) {
        std::getline(std::cin, msg);
    } else {
        msg = argv[1];
    }

    Client client;

    client.ToServer(msg);

    return 0;
}