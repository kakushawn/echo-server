#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <vector>

#define BACKLOG 10
#define BUFF_SIZE 1024

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cout << "Cannot create socket.\n";
        exit(1);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)) < 0) {
        std::cout << "Cannot setsockopt.\n";
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET, .sin_port = htons(9002), .sin_addr = INADDR_ANY};

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cout << "Cannot bind.\n";
        std::cout << "errno: " << errno << std::endl;
        return 1;
    }

    std::cout << "listening:\n";
    if (listen(sockfd, 1) == -1) {
        std::cout << "I am deaf.\n";
        return 1;
    }

    int id = 0;
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int sockfd_client;
    while (1) {
        std::cout << "[" << id << "]: accepting...\n";
        sockfd_client = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

        /* receive */
        std::cout << "[" << id << "]: receiving...\n";
        char buff[BUFF_SIZE] = {0};
        if (recv(sockfd_client, buff, BUFF_SIZE, 0) < 0) {
            std::cout << "Cannot receive message. errno: " << errno << std::endl;
        } else {
            std::cout << "[" << id << "]: message from client: " << buff << std::endl;
            /* send */
            std::string response(buff);
            if (send(sockfd_client, response.c_str(), response.size(), 0) < 0) {
                std::cout << "[" << id << "]: Could not send message. errno: " << errno << std::endl;
            } else {
                std::cout << "[" << id << "]: Finished\n\n";
            }
        }

        ++id;
    	close(sockfd_client);
    }

    close(sockfd);
    std::cout << "Shut down server.\n";

    return 0;
}
