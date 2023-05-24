.PHONY: all

CPP = g++
BIN = ./bin/
LIBS = ./lib/
INCLUDE = ./include/
CFLAGS = -Wall -Werror -Wmissing-prototypes

all: server client

server: $(BIN)server

$(BIN)server: $(LIBS)/server.cpp
	$(CPP) $< -Wall --std=c++11 -o $@ -I$(INCLUDE)

client: $(BIN)client

$(BIN)client: $(LIBS)/client.cpp
	$(CPP) $< -Wall --std=c++11 -o $@ -I$(INCLUDE)

clean:
	rm -f $(BIN)/server $(BIN)/client
