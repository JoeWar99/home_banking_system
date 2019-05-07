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


int main(int argc, char * argv[]){

    if (argc != 6) {
        printf("Usage: %s <account_id> <account_pwd> <delay> <operation> <request_args>\n", argv[0]);
        exit(RC_OTHER);
    }

    int pid  = getpid();    
    char string_pid[64];
    int secure_svr;
    //int account_id, op_delay, operation;
    int operation;

    //acccount_id = atoi(argv[1]);
    //op_delay = atoi(argv[3]);
    operation = atoi(argv[4]);

    char * req_args[3];
    int req_arg_count;

    if (parse_req_args(argv[5], req_args, &req_arg_count) == -1) {
        fprintf(stderr, "Error parsing request arguments\n");
        exit(RC_OTHER);
    }

    if(valid_req_args(operation, req_args, req_arg_count) == -1) {
        fprintf(stderr, "Invalid request arguments\n");
        exit(RC_OTHER);
    }

    sprintf(string_pid, "%d", pid);
    

    if ((secure_svr = open(SERVER_FIFO_PATH, O_WRONLY)) == -1) {
        perror("open: server is not working 404 error");
        exit(RC_SRV_DOWN);
    }

    for (int i = 0; i < req_arg_count ; i++) free(req_args[i]);

    close(secure_svr);
    return RC_OK;

}

