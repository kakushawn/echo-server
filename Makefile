.PHONY: all

CPP = g++
BIN = ./bin/
LIB = ./lib/
INCLUDE = ./include/
CPPFLAGS = -Wall -Werror --std=c++11

all: server client

server: $(LIB)libserver.so
	$(CPP) $(BIN)serve.cpp ${CPPFLAGS} -o $(BIN)serve -lserver -I$(INCLUDE) -L$(LIB)

$(LIB)libserver.so: $(LIB)server.cpp
	$(CPP) --shared -fPIC ${CPPFLAGS} -c $< -o $@ -I$(INCLUDE)

client: $(LIB)libclient.so
	$(CPP) $(BIN)echo.cpp ${CPPFLAGS} -o $(BIN)echo -lclient -I$(INCLUDE) -L$(LIB)

$(LIB)libclient.so: $(LIB)/client.cpp
	$(CPP) --shared -fPIC ${CPPFLAGS} -c $< -o $@ -I$(INCLUDE)

clean:
	rm -f $(BIN)/*.o $(BIN)/serve $(BIN)/echo $(LIB)/*.so
