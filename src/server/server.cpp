#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <vector>

#define PORT 9002
#define BACKLOG 10
#define BUFF_SIZE 1024

class Server
{
public:
    Server()
    {
        id = 0;
		buff_size = BUFF_SIZE;
		back_log = BACKLOG;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            ErrorLog("socket creation");
            exit(1);
        }

        int opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                       sizeof(opt)) < 0) {
            ErrorLog("socket option setting");
            exit(1);
        }

        struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(PORT),
            .sin_addr = INADDR_ANY};
        if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            ErrorLog("binding");
            exit(1);
        }

        if (listen(sockfd, 1) == -1) {
            ErrorLog("listening");
            exit(1);
        }
    }

    void Run()
    {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int sockfd_client;
        while (1) {
            ++id;

            sockfd_client = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
            if (sockfd_client < 0) {
                ErrorLog("accepting");
                break;
            }

			Echo(sockfd_client);

            close(sockfd_client);
        }
    }

private:
    int sockfd;
    unsigned int id;
	unsigned int buff_size;
	unsigned int back_log;

    void ErrorLog(const char *step)
    {
        std::cout << "Failed at " << step;
        std::cout << ". errno: " << errno;
        std::cout << " id: " << id << std::endl;
    }

	void Echo(int sockfd_client) {
		char buff[buff_size] = {0};
		if (recv(sockfd_client, buff, buff_size, 0) < 0) {
			ErrorLog("receiving");
		} else {
			std::cout << "[" << id << "]:";
			std::cout << " message from client: " << buff << std::endl;
			std::string response(buff);
			if (send(sockfd_client, response.c_str(), response.size(), 0) < 0) {
				ErrorLog("sending");
				std::cout << "[" << id << "]: Could not send message. errno: " << errno << std::endl;
			}
		}
	}
};

int main()
{
    Server server;
    server.Run();

    return 0;
}
