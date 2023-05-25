#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>

#include "common.h"

#define BACKLOG 10

static void handler(int signum)
{
    std::cout << "interruptted. signum: " << signum << std::endl;
}

Server::Server(uint32_t port, uint32_t buffer_size, uint32_t backlog)
    : port(port),
      buffer_size(buffer_size),
      backlog(backlog)
{
    std::cout << "port: " << port << std::endl;
    std::cout << "buffer_size: " << buffer_size << std::endl;
    std::cout << "backlog: " << backlog << std::endl;
}

int Server::Init()
{
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        ErrorLog("socket creation");
        return -1;
    }

    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &opt,
                   sizeof(opt)) < 0) {
        ErrorLog("socket option setting");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = INADDR_ANY};
    if (bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        ErrorLog("binding");
        return -1;
    }

    if (listen(sock_fd, BACKLOG) == -1) {
        ErrorLog("listening");
        return -1;
    }

    return 0;
}

// A simple server run in a single thread
void Server::Run()
{
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    bool interrupt = false;
    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
        interrupt = true;
		std::cout << "Cannot install signal handler. Aborted." << std::endl;
    }

    while (!interrupt) {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int sock_fd_client = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (sock_fd_client < 0) {
            ErrorLog("accepting");
            close(sock_fd_client);
            break;
        }

        std::string received;
        if (ReceiveMessage(sock_fd_client, received, buffer_size) < 0) {
            ErrorLog("receive");
            close(sock_fd_client);
            continue;
        }
        std::cout << "received message size: " << received.size() << std::endl;

        if (SendMessage(sock_fd_client, received, buffer_size) < 0) {
            ErrorLog("sending");
            close(sock_fd_client);
            continue;
        }
        std::cout << "sent message. size: " << received.length() << std::endl;

        close(sock_fd_client);
    }
    close(sock_fd);
    std::cout << "Server is closed." << std::endl;
}
