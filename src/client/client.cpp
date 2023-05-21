#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define LINE_LENGTH_LIMIT 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Cannot create socket.\n";
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9002),
        .sin_addr = INADDR_ANY};

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Cannot connect.\n";
        std::cerr << "errno: " << errno << std::endl;
        exit(1);
    }
    std::cerr << "Connected.\n";

    /* send */
    // char *msg = "Ground control to Major Tom.";
    // if (send(sockfd, msg, strlen(msg), 0) < 0) {
    char msg[LINE_LENGTH_LIMIT] = {0};
    std::cin.getline(msg, LINE_LENGTH_LIMIT);
    if (send(sockfd, msg, strlen(msg), 0) < 0) {
        std::cerr << "Cannot send message.\n";
        std::cerr << "errno: " << errno << std::endl;
        exit(1);
    }

    /* receive */
    char buff[1024] = {0};
    if (recv(sockfd, &buff, 1024, 0) < 0) {
        std::cerr << "Cannot receive message.\n";
        std::cerr << "errno: " << errno << std::endl;
        exit(1);
    }
    std::cerr << "Message from server:\n";
    std::cerr << buff << std::endl;

    std::cerr << "Shut down client.\n";
    close(sockfd);

    return 0;
}
