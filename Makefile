CC=gcc
CFLAGS=-Wextra -pthread

all: user_f server_f

user_f: user.o user_parse.o crypto.o utilities.o account_utilities.o com_protocol.o log.o
	$(CC) user.o user_parse.o utilities.o crypto.o account_utilities.o com_protocol.o log.o -o user $(CFLAGS)

server_f: server.o crypto.o utilities.o queue.o requests.o account_utilities.o com_protocol.o log.o
	$(CC) server.o utilities.o crypto.o queue.o requests.o account_utilities.o com_protocol.o log.o -o server $(CFLAGS)

server.o: server_f/server.c
	$(CC) -c server_f/server.c $(CFLAGS)

requests.o: server_f/requests.c shared/account_utilities.c
	$(CC) -c server_f/requests.c shared/account_utilities.c $(CFLAGS)

com_protocol.o: shared/com_protocol.c shared/utilities.c
	$(CC) -c shared/com_protocol.c shared/utilities.c $(CFLAGS)

user.o: user_f/user.c
	$(CC) -c user_f/user.c $(CFLAGS)

user_parse.o: user_f/user_parse.c shared/utilities.c
	$(CC) -c user_f/user_parse.c shared/utilities.c $(CFLAGS)

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
	rm *.o user server

clean_fifo:
	rm -rf /tmp/secure_*