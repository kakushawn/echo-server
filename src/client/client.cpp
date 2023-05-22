#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define LINE_LENGTH_LIMIT 1024

int main(int argc, char **argv) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cout << "Cannot create socket.\n";
        exit(1);
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9002),
        .sin_addr = INADDR_ANY};

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cout << "Cannot connect.\n";
        std::cout << "errno: " << errno << std::endl;
        exit(1);
    }
    std::cout << "Connected.\n";

    /* send */
    char msg[LINE_LENGTH_LIMIT] = {0};
    if (argc==1) {
        std::cin.getline(msg, LINE_LENGTH_LIMIT);
    } else {
        unsigned int c = 0;
        while(c<1023 && argv[1][c] != '\0') {
            msg[c] = argv[1][c];
            ++c;
        }
        msg[c] = '\0';
    }
    if (send(sockfd, msg, strlen(msg), 0) < 0) {
        std::cout << "Cannot send message.\n";
        std::cout << "errno: " << errno << std::endl;
        exit(1);
    }

    /* receive */
    char buff[1024] = {0};
    if (recv(sockfd, &buff, 1024, 0) < 0) {
        std::cout << "Cannot receive message.\n";
        std::cout << "errno: " << errno << std::endl;
        exit(1);
    }
    std::cout << "Message from server: " << buff << std::endl;

    std::cout << "Shut down client.\n";
    close(sockfd);

    return 0;
}
