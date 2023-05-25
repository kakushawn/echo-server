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
        ErrorLog("socket creation");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = INADDR_ANY};
    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ErrorLog("connet");
        return -1;
    }

    return 0;
}

// Send message over socket to server and show the echoed message.
int Client::Echo(const std::string &msg)
{
    if (SendMessage(sock_fd, msg, buffer_size) < 0) {
        ErrorLog("sending");
        return -1;
    }

	std::cout << "receiving msg:" << msg << std::endl;
    std::string echoed;
    if (ReceiveMessage(sock_fd, echoed, buffer_size) < 0) {
        ErrorLog("receiving");
        return -1;
    }
    std::cout << "Message from server: " << echoed << std::endl;

    return 0;
}

Client::~Client()
{
    close(sock_fd);
}