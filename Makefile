CC=gcc
CFLAGS=-Wextra

all: user server

user: user.o crypto.o utilities.o
	$(CC) user.o utilities.o crypto.o -o user_exe $(CFLAGS)

server: server.o crypto.o utilities.o
	$(CC) server.o utilities.o crypto.o -o server_exe $(CFLAGS)

server.o: server/server.c
	$(CC) -c server/server.c $(CFLAGS)

user.o: user/user.c
	$(CC) -c user/user.c $(CFLAGS)

crypto.o: shared/crypto.c
	$(CC) -c shared/crypto.c $(CFLAGS)

utilities.o: shared/utilities.c
	$(CC) -c shared/utilities.c $(CFLAGS)

log.o: shared/log.c
	$(CC) -c shared/log.c $(CFLAGS)

clean: 
	rm *.o user_exe server_exe