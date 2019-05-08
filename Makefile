CC=gcc
CFLAGS=-Wextra -pthread

all: user server

user: user.o user_parse.o crypto.o utilities.o
	$(CC) user.o user_parse.o utilities.o crypto.o -o user_exe $(CFLAGS)

server: server.o crypto.o utilities.o queue.o vetor.o
	$(CC) server.o utilities.o crypto.o queue.o vetor.o user_parse.o -o server_exe $(CFLAGS)

server.o: server/server.c
	$(CC) -c server/server.c $(CFLAGS)

user.o: user/user.c
	$(CC) -c user/user.c $(CFLAGS)

user_parse.o: user/user_parse.c
	$(CC) -c user/user_parse.c $(CFLAGS)

crypto.o: shared/crypto.c
	$(CC) -c shared/crypto.c $(CFLAGS)

utilities.o: shared/utilities.c
	$(CC) -c shared/utilities.c $(CFLAGS)

log.o: shared/log.c
	$(CC) -c shared/log.c $(CFLAGS)

queue.o: shared/queue.c
	$(CC) -c shared/queue.c $(CFLAGS)

vetor.o: shared/vetor.c
	$(CC) -c shared/vetor.c $(CFLAGS)

clean: 
	rm *.o user_exe server_exe