#include "client.h"

#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

Client::Client(int port, int buff_size)
    : port(port),
      buff_size(buff_size)
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ErrorLog("socket creation");
        return;
    }
}

void Client::ToServer(const std::string &msg)
{
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = INADDR_ANY};
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ErrorLog("connet");
        return;
    }

    if (send(sockfd, msg.c_str(), msg.length(), 0) < 0) {
        ErrorLog("send");
        return;
    }

    char buff[buff_size] = {0};
    if (recv(sockfd, &buff, buff_size, 0) < 0) {
        ErrorLog("recv");
        return;
    }
    std::cout << "Message from server: " << buff << std::endl;
    close(sockfd);
}

void Client::ErrorLog(const char *step)
{
    std::cout << "Failed at " << step << ". errno: " << errno << std::endl;
}
