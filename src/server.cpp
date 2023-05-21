#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <cstring>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Cannot create socket.\n";
        exit(1);
    }

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        std::cerr << "Cannot setsockopt.\n";
        return 1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9002),
        .sin_addr = INADDR_ANY};

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Cannot bind.\n";
        std::cerr << "errno: " << errno << std::endl;
        return 1;
    }

    std::cerr << "listening:\n";
    int backlog = 20;
    if (listen(sockfd, backlog) == -1) {
        std::cerr << "I am deaf.\n";
        return 1;
    }

    std::cerr << "accepting:\n";
    struct sockaddr_storage their_addr;
    socklen_t talen = sizeof(their_addr);
    int sockfd2 = accept(sockfd, (struct sockaddr*)&their_addr, &talen);
    // std::cerr << "addr.sin"

    /* receive */
    std::cerr << "receiving:\n";
    char buff[1024] = {0};
    recv(sockfd2, buff, 1024, 0);
    std::cerr << "message from client:\n";
    std::cerr << buff << std::endl;

    /* send */
    std::string response = "Bye!";
    if (send(sockfd2, response.c_str(), response.size(), 0) < 0) {
        std::cerr << "Cannot send.\n";
        std::cerr << "errno: " << errno << std::endl;
    }

    std::cerr << "Shut down.\n";
    close(sockfd2);
    shutdown(sockfd, SHUT_RDWR);

    return 0;
}
