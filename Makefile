CC=gcc
CFLAGS=-Wextra -pthread

all: user server

user: user.o user_parse.o crypto.o utilities.o account_utilities.o com_protocol.o
	$(CC) user.o user_parse.o utilities.o crypto.o account_utilities.o com_protocol.o -o user_exe $(CFLAGS)

server: server.o crypto.o utilities.o queue.o requests.o account_utilities.o com_protocol.o
	$(CC) server.o utilities.o crypto.o queue.o requests.o account_utilities.o com_protocol.o -o server_exe $(CFLAGS)

server.o: server/server.c
	$(CC) -c server/server.c $(CFLAGS)

requests.o: server/requests.c shared/account_utilities.c
	$(CC) -c server/requests.c shared/account_utilities.c $(CFLAGS)

com_protocol.o: shared/com_protocol.c shared/utilities.c
	$(CC) -c shared/com_protocol.c shared/utilities.c $(CFLAGS)

user.o: user/user.c
	$(CC) -c user/user.c $(CFLAGS)

user_parse.o: user/user_parse.c shared/utilities.c
	$(CC) -c user/user_parse.c shared/utilities.c $(CFLAGS)

crypto.o: shared/crypto.c
	$(CC) -c shared/crypto.c $(CFLAGS)

account_utilities.o: shared/account_utilities.c shared/utilities.c shared/crypto.c
	$(CC) -c shared/account_utilities.c shared/utilities.c shared/crypto.c $(CFLAGS)

utilities.o: shared/utilities.c
	$(CC) -c shared/utilities.c $(CFLAGS)

log.o: shared/log.c
	$(CC) -c shared/log.c $(CFLAGS)

queue.o: shared/queue.c
	$(CC) -c shared/queue.c $(CFLAGS)

clean: 
	rm *.o user_exe server_exe