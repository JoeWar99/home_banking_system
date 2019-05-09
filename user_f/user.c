#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "../shared/utilities.h"
#include "../shared/constants.h"
#include "../shared/types.h"
#include "../shared/com_protocol.h"
#include "user_parse.h"

#include "../shared/sope.h"

int main(int argc, char *argv[])
{

    if (argc != 6)
    {
        printf("Usage: %s <account_id> <account_pwd> <delay> <operation> <request_args>\n", argv[0]);
        exit(RC_OTHER);
    }

    int pid = getpid();
    int secure_svr;
    int account_id, op_delay, operation;

    account_id = atoi(argv[1]);
    char *pwd = argv[2];
    op_delay = atoi(argv[3]);
    operation = atoi(argv[4]);

    char *req_args[3];
    int req_arg_count = 0;

    if (!valid_args(account_id, pwd, op_delay, operation))
    {
        fprintf(stderr, "Invalid arguments given\n");
        exit(RC_OTHER);
    }

    if (parse_req_args(argv[5], req_args, &req_arg_count) == -1)
    {
        fprintf(stderr, "Error parsing request arguments\n");
        exit(RC_OTHER);
    }

    if (!valid_req_args(operation, req_args, req_arg_count))
    {
        fprintf(stderr, "Invalid request arguments\n");
        exit(RC_OTHER);
    }

    char secure_fifo_name[strlen(USER_FIFO_PATH_PREFIX) + WIDTH_PID + 1];
    init_secure_fifo_name(secure_fifo_name, pid);

    if (mkfifo(secure_fifo_name, 0660) < 0)
    {
        if (errno != EEXIST)
        {
            fprintf(stderr, "Could not create fifo %s\n", SERVER_FIFO_PATH);
            exit(RC_OTHER);
        }
    }

    tlv_request_t full_request;
    init_request(&full_request, operation, pid, account_id, pwd, op_delay, req_args);

    //Opening server fifo

    if ((secure_svr = open(SERVER_FIFO_PATH, O_WRONLY)) == -1)
    {
        perror("open: server is not working 404 error");
        exit(RC_SRV_DOWN);
    }
	if(write_request(secure_svr, &full_request) != 0){
		fprintf(stderr, "write: error writing to server\n");
		exit(RC_OTHER);
	}

    if (logRequest(STDOUT_FILENO, pid, &full_request) < 0)
    {
        fprintf(stderr, "logRequest: error writing request to stdout\n");
        exit(RC_OTHER);
    }

    // Waiting for server response or timeout

    int user_fifo;
    if ((user_fifo = open(secure_fifo_name, O_RDONLY)) == -1)
    {
        perror("open");
        exit(RC_USR_DOWN);
    }

    tlv_reply_t request_reply;

	if(read_reply(user_fifo, &request_reply) != 0){
		fprintf(stderr, "read_reply: error reading reply from fifo\n");
		exit(RC_OTHER);
	}

	if (logReply(STDOUT_FILENO, pid, &request_reply) < 0)
	{
		fprintf(stderr, "logRequest: error writing reply to stdout\n");
		exit(RC_OTHER);
	}
	

    /* if (read(user_fifo, &request_reply, sizeof(tlv_reply_t)) != 0)
    {
        // printf("header :   account_id %d , ret_code %d\n", request_reply.value.header.account_id, request_reply.value.header.ret_code);
        // switch (request_reply.type)
        // {
        // case OP_CREATE_ACCOUNT:
        // 	printf("create\n");
        // 	break;
        // case OP_TRANSFER:
        // 	printf("transfer: balance %d\n", request_reply.value.transfer.balance);
        // 	break;
        // case OP_BALANCE:
        // 	printf("balance: balance %d\n", request_reply.value.balance.balance);
        // 	break;
        // case OP_SHUTDOWN:
        // 	printf("shutdown: active offices %d\n", request_reply.value.shutdown.active_offices);
        // 	break;
        // }
        if (logReply(STDOUT_FILENO, pid, &request_reply) < 0)
        {
            fprintf(stderr, "logRequest: error writing reply to stdout\n");
            exit(RC_OTHER);
        }
    }*/

    // Free alocated memory
    for (int i = 0; i < req_arg_count; i++)
    {
        free(req_args[i]);
    }

    // Closing down fifos and unlinking user fifo

    if (close(secure_svr) != 0)
    {
        perror("close: error closing down server fifo");
        exit(RC_OTHER);
    }

    /* if (close(user_fifo) != 0)
    {
        perror("close: error closing down user fifo");
        exit(RC_OTHER);
    }*/

    if (unlink(secure_fifo_name) != 0)
    {
        perror("unlink: error unlinking user fifo");
        exit(RC_OTHER);
    }

    return RC_OK;
}
