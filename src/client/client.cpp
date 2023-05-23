#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#define PORT 9002
#define BUFF_SIZE 1024

class Client
{
public:
    Client()
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            ErrorLog("socket creation");
            return;
        }
    }

    void ToServer(const std::string &msg)
    {
        struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(PORT),
            .sin_addr = INADDR_ANY};
        if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            ErrorLog("connet");
            return;
        }

        if (send(sockfd, msg.c_str(), msg.length(), 0) < 0) {
            ErrorLog("send");
            return;
        }

        char buff[1024] = {0};
        if (recv(sockfd, &buff, 1024, 0) < 0) {
            ErrorLog("recv");
            return;
        }
        std::cout << "Message from server: " << buff << std::endl;
    }

    ~Client()
    {
        close(sockfd);
    }

private:
    int port;
    int sockfd;
    void ErrorLog(const char *step)
    {
        std::cout << "Failed at " << step << ". errno: " << errno << std::endl;
    }
};

int main(int argc, char **argv)
{
    if (argc > 2) {
        std::cout << "Invalid argument." << std::endl;
        return 1;
    }

	std::string msg;
    if (argc == 1) {
   		std::getline(std::cin, msg);
    } else {
		msg = argv[1];
    }

    Client client;

    client.ToServer(msg);

    return 0;
}
