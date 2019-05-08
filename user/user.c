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
#include "user_parse.h"

int main(int argc, char *argv[])
{

    if (argc != 6)
    {
        printf("Usage: %s <account_id> <account_pwd> <delay> <operation> <request_args>\n", argv[0]);
        exit(RC_OTHER);
    }

    int pid = getpid();
    char string_pid[64];
    int secure_svr;
    int account_id, op_delay, operation;

    account_id = atoi(argv[1]);
    char * pwd = argv[2];
    op_delay = atoi(argv[3]);
    operation = atoi(argv[4]);

    char *req_args[3];
    int req_arg_count = 0;

    if (!valid_args(account_id,pwd, op_delay, operation)) {
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


    int user_fifo;
    char *secure_fifo_name = (char *)malloc(sizeof(char) * (strlen(USER_FIFO_PATH_PREFIX) + 1));

    strcpy(secure_fifo_name, USER_FIFO_PATH_PREFIX);
    secure_fifo_name[strlen(USER_FIFO_PATH_PREFIX) + 1] = '\0';
    //printf("%s\n", secure_fifo_name);
    sprintf(string_pid, "%d", pid);
    string_pid[strlen(string_pid) + 1] = '\0';

    concat(&secure_fifo_name, string_pid, strlen(string_pid));

    if (mkfifo(secure_fifo_name, 0660) < 0)
    {
        if (errno != EEXIST)
        {
            fprintf(stderr, "Could not create fifo %s\n", SERVER_FIFO_PATH);
            exit(RC_OTHER);
        }
    }

    if ((user_fifo = open(secure_fifo_name, O_RDONLY)) == -1)
    {
        perror("open");
        exit(RC_USR_DOWN);
    }


    // TODO use full struct
    req_value_t request_value;

    req_header_t * user_header = &request_value.header;
    user_header->pid = pid;
    user_header->account_id = account_id;
    strcpy(user_header->password, pwd);
    user_header->op_delay_ms = op_delay;

    req_create_account_t * user_create;
    req_transfer_t * user_transfer;
    switch (operation)
    {
        case OP_CREATE_ACCOUNT:
            user_create = &request_value.create;
            user_create->account_id = atoi(req_args[0]);
            user_create->balance = atoi(req_args[1]);
            strcpy(user_create->password, req_args[2]);
            break;

        case OP_TRANSFER:
            user_transfer = &request_value.transfer;
            user_transfer->account_id = atoi(req_args[0]);
            user_transfer->amount = atoi(req_args[1]);
            break;
    }

    //Opening server fifo

    if ((secure_svr = open(SERVER_FIFO_PATH, O_WRONLY)) == -1)
    {
        perror("open: server is not working 404 error");
        exit(RC_SRV_DOWN);
    }

    if (write(secure_svr, &request_value, sizeof(req_value_t) * 1) != sizeof(req_value_t))
    {
        perror("write: error writing to server");
        exit(RC_OTHER);
    }

    // Waiting for server response or timeout


  //  while(read (user_fifo,  ,   ) != 0     ||  FIFO_TIMEOUT_SECS  ){





  //  }

    // Free alocated memory

    for (int i = 0; i < req_arg_count; i++)
    {
        free(req_args[i]);
    }

    char string_fifo_user_aux[strlen(secure_fifo_name) + 1];
    strcpy(string_fifo_user_aux, secure_fifo_name);
    free(secure_fifo_name);

    // Closing down fifos and unlinking user fifo

    if (close(secure_svr) != 0)
    {
        perror("close: error closing down server fifo");
        exit(RC_OTHER);
    }
    if (close(user_fifo) != 0)
    {
        perror("close: error closing down user fifo");
        exit(RC_OTHER);
    }

    if (unlink(string_fifo_user_aux) != 0)
    {
        perror("unlink: error unlinking user fifo");
        exit(RC_OTHER);
    }

    return RC_OK;
}
