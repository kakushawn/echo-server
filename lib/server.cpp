#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>

#include "common.h"

#define MAX_EVENTS 10
#define EPOLL_WAIT_TIME_OUT 1000  // ms

static void SignalHandler(int signum)
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

    if (listen(sock_fd, backlog) == -1) {
        ErrorLog("listening");
        return -1;
    }

    struct sigaction sa;
    sa.sa_handler = SignalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
        std::cout << "Cannot install signal handler. Aborted." << std::endl;
        close(sock_fd);
        return -1;
    }

    return 0;
}

// A simple server run in a single thread
void Server::Run()
{
    while (true) {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int sock_fd_client = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (sock_fd_client < 0) {
            ErrorLog("accepting");
            break;
        }

        std::string received;
        while (ReceiveMessage(sock_fd_client, received, buffer_size) == 0) {
            std::cout << "received message size: " << received.size() << std::endl;

            if (SendMessage(sock_fd_client, received, buffer_size) < 0) {
                ErrorLog("sending");
                break;
            }
            std::cout << "sent message. size: " << received.length() << std::endl;
        }

        close(sock_fd_client);
        std::cout << "Connection is closed." << std::endl;
    }
    close(sock_fd);
    std::cout << "Server is closed." << std::endl;
}

void Server::RunNonblocking()
{
    int epoll_fd;
    struct epoll_event events[MAX_EVENTS];

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        std::cout << "epoll_create1" << std::endl;
        return;
    }
    if (EpollCtlAdd(epoll_fd, sock_fd, EPOLLIN | EPOLLOUT | EPOLLET) == -1) {
        std::cout << "epoll_ctl: listen_sock" << std::endl;
        return;
    }

    while (true) {
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, EPOLL_WAIT_TIME_OUT);
        if (num_fds == -1) {
            ErrorLog("epoll_wait");
            break;
        }

        for (int i = 0; i < num_fds; ++i) {
            if (events[i].data.fd == sock_fd) {
                struct sockaddr_storage client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int sock_fd_client = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len);
                if (sock_fd_client == -1) {
                    ErrorLog("accept");
                    continue;
                }

                if (SetNonblocking(sock_fd_client) < 0) {
                    ErrorLog("SetNonblocking");
                    continue;
                }

                if (EpollCtlAdd(epoll_fd, sock_fd_client, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP) < 0) {
                    std::cout << "epoll_ctl: sock_fd_client" << std::endl;
                    continue;
                }
            } else if (events[i].events & EPOLLIN) {
                std::string received_message;
                if (ReceiveMessageNonblocking(events[i].data.fd, received_message, buffer_size) < 0) {
                    ErrorLog("ReceiveMessageNonblocking");
                } else if (received_message.size() == 0) {
                    std::cout << "client fd " << events[i].data.fd << " hung up." << std::endl;
                } else {
                    uint32_t sending_size = received_message.size();
                    if (SendMessageNonblocking(events[i].data.fd, received_message, sending_size) < 0) {
                        ErrorLog("SendMessageNonblocking");
                    }
                }
                if (events[i].events & (EPOLLHUP)) {
                    std::cout << "client fd: " << events[i].data.fd << " disconnected." << std::endl;
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
                    close(events[i].data.fd);
                }
            } else {
                std::cout << "Unexcpeted error occured." << std::endl;
            }
        }
    }
    close(epoll_fd);
    std::cout << "epoll_fd is closed." << std::endl;
}

Server::~Server()
{
    close(sock_fd);
    std::cout << "sock_fd is closed." << std::endl;
}