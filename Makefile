.PHONY: all

CPP = g++
BIN = ./bin/
MAIN = ./main/
LIB = ./lib/
INCLUDE = ./include/
CPPFLAGS = -Wall -Werror --std=c++17

all: main common

main: server client client2

common: $(LIB)libcommon.so

server: $(BIN)server

client: $(BIN)client

client2: $(BIN)client2

$(LIB)libcommon.so: $(LIB)common.cpp
	$(CPP) --shared -fPIC ${CPPFLAGS} -c $< -o $@ -I$(INCLUDE)

dep_server = $(MAIN)server.cpp $(LIB)server.cpp $(LIB)thread_pool.cpp
dep_client = $(MAIN)client.cpp  $(LIB)client.cpp
dep_client2 = $(MAIN)client2.cpp $(LIB)client.cpp

$(BIN)server: $(LIB)libcommon.so $(dep_server)
	$(CPP) $(dep_server) ${CPPFLAGS} -o $@ -I$(INCLUDE) -L$(LIB) -lcommon

$(BIN)client: $(LIB)libcommon.so $(dep_client)
	$(CPP) $(dep_client) ${CPPFLAGS}  -o $@ -I$(INCLUDE) -L$(LIB) -lcommon

$(BIN)client2: $(LIB)libcommon.so $(dep_client2)
	$(CPP) $(dep_client2) ${CPPFLAGS} -o $@ -I$(INCLUDE) -L$(LIB) -lcommon

clean:
	rm -f $(BIN)/* $(LIB)/*.so
