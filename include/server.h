#ifndef SERVER_H_
#define SERVER_H_

class Server
{
public:
    Server();

	// Setting up server socket.
	int Init();

	// A simple worker that waits connection and echo, 
	// and runs in a single thread.
	void Run();

private:
    int sock_fd;
    unsigned int connection_id;

    void ErrorLog(const char *step) ;
};

#endif
