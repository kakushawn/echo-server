#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <thread>

#define BACKLOG 20
#define BUFF_SIZE 1024

void run(int id, const int &sockfd) {
    std::cout << "[" << id << "]: accepting...\n";
    struct sockaddr_storage their_addr;
    socklen_t talen = sizeof(their_addr);
    int sockfd_client = accept(sockfd, (struct sockaddr *)&their_addr, &talen);

    /* receive */
    std::cout << "[" << id << "]: receiving...\n";
    char buff[BUFF_SIZE] = {0};
    recv(sockfd_client, buff, BUFF_SIZE, 0);
    std::cout << "[" << id << "]: message from client: " << buff << std::endl;

    /* send */
    std::string response(buff);

    if (send(sockfd_client, response.c_str(), response.size(), 0) < 0) {
        std::cout << "[" << id << "]: Could not send. errno: " << errno << std::endl;
    } else {
        std::cout << "[" << id << "]: Finished\n\n";
    }
    close(sockfd_client);
}

int main() {
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

    int counter = 0;
    while (true) {
        std::thread worker(run, counter++, sockfd);
        // std::thread worker2(run, counter++, sockfd);
        // std::thread worker3(run, counter++, sockfd);
        worker.join();
        // worker2.join();
        // worker3.join();
    }

    std::cout << "Shut down.\n";
    shutdown(sockfd, SHUT_RDWR);

    return 0;
}
