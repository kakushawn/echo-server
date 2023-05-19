.PHONY: all

all: server client

server: server
	g++ server.cpp -Wall --std=c++11 -o server

client: client
	g++ client.cpp -Wall --std=c++11 -o client

clean:
	rm -r bin
