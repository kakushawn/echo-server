#ifndef SERVER_H_
#define SERVER_H_

class Server
{
public:
    Server(unsigned int port = 9002);

    // Setting up server socket.
    int Init();

    // A simple worker that waits connection and echo,
    // and runs in a single thread.
    void Run();
private:
    int sock_fd;
    unsigned int port;
    unsigned int backlog;
    unsigned int connection_id;

    // Show log given a step string
    void ErrorLog(const char *step);
};

#endif
