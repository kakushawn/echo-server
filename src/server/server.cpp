#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define BACKLOG 2

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Cannot create socket.\n";
        exit(1);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)) < 0) {
        std::cerr << "Cannot setsockopt.\n";
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET, .sin_port = htons(9002), .sin_addr = INADDR_ANY};

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Cannot bind.\n";
        std::cerr << "errno: " << errno << std::endl;
        return 1;
    }

    std::cerr << "listening:\n";
    if (listen(sockfd, BACKLOG) == -1) {
        std::cerr << "I am deaf.\n";
        return 1;
    }

    int counter = 0;
    struct sockaddr_storage their_addr;
    socklen_t talen = sizeof(their_addr);
    while (true) {
        std::cout << "[" << counter << "]:\n";
        std::cerr << "accepting...\n";
        int sockfd_client = accept(sockfd, (struct sockaddr *)&their_addr, &talen);

        /* receive */
        std::cerr << "receiving...\n";
        char buff[1024] = {0};
        recv(sockfd_client, buff, 1024, 0);
        std::cerr << "message from client: " << buff << std::endl;

        /* send */
        // std::string response = "Bye!";
        std::string response(buff);

        if (send(sockfd_client, response.c_str(), response.size(), 0) < 0) {
            std::cerr << "Could not send. errno: " << errno << std::endl;
        } else {
            std::cerr << "[" << counter << "]" << "Finished\n\n";
        }
        ++counter;
        close(sockfd_client);
    }

    std::cerr << "Shut down.\n";
    shutdown(sockfd, SHUT_RDWR);

    return 0;
}
