#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <iostream>
#include <string>

#include "common.h"
#include "thread_pool.h"

static void SignalHandler(int signum)
{
    std::cout << "interruptted. signum: " << signum << std::endl;
}

// Job event for echoing message
void EchoEvent(int sock_fd, uint32_t buffer_size)
{
    std::string received_message;
    ReceiveMessageNonblocking(sock_fd, received_message, buffer_size);

    if (received_message.size() == 0) {
        perror("ReceiveMessageNonblocking");
    } else {
        uint32_t sending_size = received_message.size();
        if (SendMessage2(sock_fd, received_message, sending_size) < 0) {
            perror("SendMessage2");
        }
    }
}

// Job event for accepting new connection.
void ConnectionEvent(int sock_fd, int epoll_fd)
{
    int sock_fd_client;
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    while ((sock_fd_client = accept(sock_fd, (struct sockaddr *)&client_addr, &client_addr_len)) > 0) {
        if (sock_fd_client == -1) {
            perror("accept");
            break;
        }
        SetNonblocking(sock_fd_client);
        if (EpollCtlAdd(epoll_fd, sock_fd_client, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP) < 0) {
            perror("epoll_ctl: conn_sock");
            break;
        }
    }
}

Server::Server(uint32_t port, uint32_t buffer_size, uint32_t backlog, uint32_t max_events)
    : port(port),
      buffer_size(buffer_size),
      backlog(backlog),
      max_events(max_events)
{
    std::cout << "port: " << port << std::endl;
    std::cout << "buffer_size: " << buffer_size << std::endl;
    std::cout << "backlog: " << backlog << std::endl;
    std::cout << "max_events: " << max_events << std::endl;
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

void Server::RunMultiThreaded(int num_workers, int num_server_workers)
{
    std::cout << "num_workers: " << num_workers << std::endl;

    ThreadPool pool(num_workers);
    pool.Start();
    std::cout << "Created " << num_workers << " workers in pool." << std::endl;

    ThreadPool pool_server(num_server_workers);
    pool_server.Start();
    std::cout << "Created " << num_server_workers << " workers in server pool." << std::endl;

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return;
    }
    SetNonblocking(sock_fd);
    if (EpollCtlAdd(epoll_fd, sock_fd, EPOLLIN | EPOLLOUT | EPOLLET) < 0) {
        perror("epoll_ctl: listen_sock");
        return;
    }
    struct epoll_event events[max_events];
    while (true) {
        int num_fds = epoll_wait(epoll_fd, events, max_events, 1000);
        if (num_fds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int n = 0; n < num_fds; ++n) {
            int fd = events[n].data.fd;
            if (events[n].events & (EPOLLRDHUP | EPOLLHUP)) {
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                continue;
            }
            if (fd == sock_fd) {
                pool_server.Push(std::bind(ConnectionEvent, fd, epoll_fd));
            } else if (events[n].events & EPOLLIN) {
                pool.Push(std::bind(EchoEvent, fd, buffer_size));
            }
        }
    }
    pool.Stop();
    pool_server.Stop();
    close(epoll_fd);
    std::cout << "epoll_fd is closed." << std::endl;
}

Server::~Server()
{
    close(sock_fd);
    std::cout << "sock_fd is closed." << std::endl;
}
