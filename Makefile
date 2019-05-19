CC=gcc
CFLAGS = -Wall -Werror -Wextra
LBLIBS = -pthread -lrt

SRC_SERVER = server_f/server.c server_f/server_parse.c server_f/active_offices.c server_f/requests.c server_f/sync_log.c server_f/sync.c shared/crypto.c shared/utilities.c shared/queue.c shared/account_utilities.c shared/com_protocol.c shared/log.c
SRC_USER = user_f/user.c user_f/user_parse.c shared/crypto.c shared/utilities.c shared/account_utilities.c shared/com_protocol.c shared/log.c server_f/sync.c server_f/sync_log.c server_f/active_offices.c
OBJ_USER = $(SRC_USER:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)
EXEC1 = server
EXEC2 = user
DEPS = $(patsubst %.c,%.d,$(wildcard *.c))

.PHONY: all clean

all: $(EXEC1) $(EXEC2) log

log:
	touch ulog.txt slog.txt

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(EXEC1): $(OBJ_SERVER)
	$(CC) $(CFLAGS) -o $@ $(OBJ_SERVER) $(LBLIBS)

$(EXEC2): $(OBJ_USER)
	$(CC) $(CFLAGS) -o $@ $(OBJ_USER) $(LBLIBS)

clean:
	rm -rf $(EXEC1) $(EXEC2) *.o *.d *_f/*.o *_f/*.d shared/*.o shared/*.d *log.txt

clean_fifo:
	rm -rf /tmp/secure_*

-include $(DEPS)