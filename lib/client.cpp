#include "client.h"

#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "common.h"

Client::Client(uint32_t port, uint32_t buffer_size)
    : port(port),
      buffer_size(buffer_size) {}

int Client::Init()
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket creation");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = INADDR_ANY};
    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connet");
        return -1;
    }

    return 0;
}

// Send message over socket to server and show the echoed message.
int Client::Echo(const std::string &msg, std::string &echoed)
{
    if (SendMessage(sock_fd, msg, buffer_size) < 0) {
        perror("sending");
        return -1;
    }

    if (ReceiveMessage(sock_fd, echoed, buffer_size) < 0) {
        perror("receiving");
        return -1;
    }

    return 0;
}

int Client::EchoNonblocking(const std::string &msg, std::string &echoed)
{
    SetNonblocking(sock_fd);
    if (SendMessage2(sock_fd, msg) < 0) {
        return -1;
    }

    do {
        if (ReceiveMessageNonblocking(sock_fd, echoed, buffer_size) < 0) {
            ErrorLog("ReceiveMessageNonblocking");
            return -1;
        }
    } while (echoed.size() != msg.size() && errno == EAGAIN);

    if (echoed.size() != msg.size()) {
        ErrorLog("echoed size");
        return -1;
    }

    return 0;
}

Client::~Client()
{
    close(sock_fd);
}