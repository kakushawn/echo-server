.PHONY: all

CPP = g++
BIN = ./bin/
LIB = ./lib/
INCLUDE = ./include/
CPPFLAGS = -Wall -Werror --std=c++11

all: server client client2

common: $(LIB)libcommon.so

$(LIB)libcommon.so: $(LIB)common.cpp
	$(CPP) --shared -fPIC ${CPPFLAGS} -c $< -o $@ -I$(INCLUDE)

server: $(LIB)libcommon.so
	$(CPP) $(BIN)server.cpp $(LIB)server.cpp ${CPPFLAGS} -o $(BIN)server -lcommon -I$(INCLUDE) -L$(LIB)

client: $(LIB)libcommon.so
	$(CPP) $(BIN)client.cpp $(LIB)client.cpp ${CPPFLAGS} -o $(BIN)client -lcommon -I$(INCLUDE) -L$(LIB)
client2: $(LIB)libcommon.so
	$(CPP) $(BIN)client2.cpp $(LIB)client.cpp ${CPPFLAGS} -o $(BIN)client2 -lcommon -I$(INCLUDE) -L$(LIB)

clean:
	rm -f $(BIN)/*.o $(BIN)/server $(BIN)/client $(LIB)/*.so
