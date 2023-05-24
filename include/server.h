#ifndef SERVER_H_
#define SERVER_H_

class Server
{
public:
    Server(int port = 9002, int backlog = 10, int buff_size = 1024);

    // Setting up server socket.
    int Init();

    // A simple worker that waits connection and echo,
    // and runs in a single thread.
    void Run();

private:
    int sock_fd;
    int port;
    int backlog;
    int buff_size;
    unsigned int connection_id;

	// Show log given a step string
    void ErrorLog(const char *step);
};

#endif
