
FLAGS = -Wall
LIBS  = -lncurses
DEBUG = -g
CC    = g++
OBJ   = ConexaoRawSocket.o utils.o

# ==============================================================================

all: server client

debug: $(FLAGS) += $(DEBUG) 
debug: server client 

# ==============================================================================

server: main-server.cpp functions-server.o $(OBJ)
	$(CC) $(FLAGS) $(OBJ) main-server.cpp functions-server.o -o server $(LIBS)

functions-server.o: functions-server.cpp functions-server.hpp  
	$(CC) $(FLAGS) -c functions-server.cpp $(LIBS)

# ==============================================================================

client: main-client.cpp functions-client.o $(OBJ)
	$(CC) $(FLAGS) $(OBJ) functions-client.o main-client.cpp -o client $(LIBS)

functions-client.o: functions-client.cpp functions-client.hpp 
	$(CC) $(FLAGS) -c functions-client.cpp $(LIBS)

# ==============================================================================

ConexaoRawSocket.o: ConexaoRawSocket.c ConexaoRawSocket.h
	$(CC) $(FLAGS) -c ConexaoRawSocket.c

utils.o: utils.cpp utils.hpp
	$(CC) $(FLAGS) -c utils.cpp $(LIBS)

# ==============================================================================

clean:
	rm -f *.o

purge: clean
	rm -f server client