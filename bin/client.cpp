#include "client.h"

#include <iostream>

int main(int argc, char **argv)
{
    Client client(9002, 32);
    if (client.Init() < 0) {
        std::cout << "Failed to initialize connection." << std::endl;
        return 1;
    }

    // Read message
    if (argc == 1) {
        std::string msg;
        while (std::getline(std::cin, msg)) {
            if(client.EchoNonblocking(msg)<0) {
                std::cout << "Error occured when echoing" << std::endl;
                return -1;
            }
            std::cout << "echoed: " << msg << std::endl;
        }
    } else {
        for (int32_t i = 1; i < argc; ++i) {
            std::string msg = argv[i];
            if (client.EchoNonblocking(msg) < 0) {
                std::cout << "Error occured when echoing" << std::endl;
                return -1;
            }
            std::cout << "echoed: " << msg << std::endl;
        }
    }

    return 0;
}