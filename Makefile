.PHONY: all

CC=g++
BIN=./bin/
SOURCE=./src/

all: server client

server: $(BIN)server

$(BIN)server: $(SOURCE)/server/server.cpp
	$(CC) $< -Wall --std=c++11 -o $@

client: $(BIN)client

$(BIN)client: $(SOURCE)/client/client.cpp
	$(CC) $< -Wall --std=c++11 -o $@

clean:
	rm -rf $(BIN)*
