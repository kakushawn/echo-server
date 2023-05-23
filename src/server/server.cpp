#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#define BACKLOG 10
#define BUFF_SIZE 1024
#define WORKER_NUM 4;

struct arg_struct {
        int id;
        int sockfd;
};

void *run(void *args)
{
	struct arg_struct *arguments = (struct arg_struct *)args;
        int id = arguments->id;
        int sockfd = arguments->sockfd;
        std::cout << "[" << id << "]: accepting...\n";
        struct sockaddr_storage their_addr;
        socklen_t talen = sizeof(their_addr);
        while (1) {
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
                }
                else {
                        std::cout << "[" << id << "]: Finished\n\n";
                }

                close(sockfd_client);
        }
}

void signal_handler(int signum)
{
        std::cout << "Receied signal: " << signum << std::endl;
        exit(signum);
}

int main()
{
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

        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        int counter = 0;
        int worker_num = WORKER_NUM;
        std::vector<pthread_t> workers(worker_num);
        std::vector<struct arg_struct> args(worker_num);

        for (int i = 0; i < worker_num; ++i) {
                args[i].sockfd = sockfd;
                args[i].id = counter++;
                pthread_create(&workers[i], NULL, &run, (void *)&args[i]);
        }
        for (int i = 0; i < worker_num; ++i) {
                pthread_join(workers[i], NULL);
        }

        std::cout << "Shut down server.\n";
        close(sockfd);

        return 0;
}
