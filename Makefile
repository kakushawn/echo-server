.PHONY: all

CC=g++
BIN=./bin/
SOURCE=./src/

all: server client

server: $(BIN)/server

$(BIN)/server: $(SOURCE)server.cpp
	$(CC) $< -Wall --std=c++11 -o $@ $(LIBS)

client: $(BIN)/client

$(BIN)/client: $(SOURCE)client.cpp
	$(CC) $< -Wall --std=c++11 -o $@

clean:
	rm -rf $(BIN)
