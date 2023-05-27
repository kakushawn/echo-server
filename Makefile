.PHONY: all

CPP = g++
BIN = ./bin/
LIB = ./lib/
INCLUDE = ./include/
CPPFLAGS = -Wall -Werror --std=c++17

all: server client client2

common: $(LIB)libcommon.so

$(LIB)libcommon.so: $(LIB)common.cpp
	$(CPP) --shared -fPIC ${CPPFLAGS} -c $< -o $@ -I$(INCLUDE)

server: $(BIN)server

$(BIN)server: $(LIB)libcommon.so $(BIN)server.cpp $(LIB)server.cpp $(LIB)thread_pool.cpp
	$(CPP) $(BIN)server.cpp $(LIB)server.cpp $(LIB)thread_pool.cpp ${CPPFLAGS} -o $(BIN)server -lcommon -I$(INCLUDE) -L$(LIB)

client: $(BIN)client

$(BIN)client: $(LIB)libcommon.so $(BIN)client.cpp $(LIB)client.cpp
	$(CPP) $(BIN)client.cpp $(LIB)client.cpp ${CPPFLAGS} -o $(BIN)client -lcommon -I$(INCLUDE) -L$(LIB)

client2: $(BIN)client2

$(BIN)client2: $(LIB)libcommon.so $(BIN)client2.cpp $(LIB)client.cpp
	$(CPP) $(BIN)client2.cpp $(LIB)client.cpp ${CPPFLAGS} -o $(BIN)client2 -lcommon -I$(INCLUDE) -L$(LIB)

thread_pool: $(LIB)thread_pool.so

$(LIB)thread_pool.so: $(LIB)thread_pool.cpp
	$(CPP) --shared -fPIC ${CPPFLAGS} -c $< -o $@ -I$(INCLUDE)

clean:
	rm -f $(BIN)/*.o $(BIN)/server $(BIN)/client $(LIB)/*.so
