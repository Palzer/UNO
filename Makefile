CC=g++
CFLAGS= -g -Wall

SERVER_SRCS= server.cpp serverdata.cpp deck.cpp
SERVER_OBJS= ${SERVER_SRCS:.cpp=.o}

CLIENT_SRCS= client.cpp clientdata.cpp deck.cpp
CLIENT_OBJS= ${CLIENT_SRCS:.cpp=.o}

server/client: $(SERVER_OBJS) $(CLIENT_OBJS) 
	${CC} -o server $(SERVER_OBJS)
	${CC} -o client $(CLIENT_OBJS)	

server: $(SERVER_OBJS) 
	${CC} -o server $(SERVER_OBJS)

client: $(CLIENT_OBJS) 
	${CC} -o client $(CLIENT_OBJS)

clean:
	rm -r server $(SERVER_OBJS)
	rm -r client $(CLIENT_OBJS)
	
$(OBJS): serverdata.h deck.h clientdata.h
