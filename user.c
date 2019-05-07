#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "utilities.h"
#include "constants.h"
#include "types.h"

int main(int argc, char * argv[]){

    if (argc != 6) {
        printf("Usage: %s <account_id> <account_pwd> <delay> <operation> <request_args>\n", argv[0]);
        exit(RC_OTHER);
    }

    int pid  = getpid();    
    char string_pid[64];
    int secure_svr;
    //int acccount_id, op_delay, operation;
    int operation;

    //acccount_id = atoi(argv[1]);
    //op_delay = atoi(argv[3]);
    operation = atoi(argv[4]);

    if (operation < 0 || operation > 3) {
        printf("Invalid operation specified\n");
        exit(RC_OTHER);
    }

    char * req_args[3];
    int req_arg_count;
    const char delim[2] = " ";
    char *token = strtok(argv[5], delim);
    for (req_arg_count = 0; req_arg_count <= 3; req_arg_count++) {

        if (token == NULL) break;

        if (req_arg_count == 3) {
            printf("Invalid number of request arguments\n");
            exit(RC_OTHER);
        }

        req_args[req_arg_count] = (char *) malloc(sizeof(char) * (strlen(token)+1));
        strcpy(req_args[req_arg_count], token);
        token = strtok(NULL, delim);
    }

    // TODO check arguments values
    int expected_args = 0;
    if (operation == OP_CREATE_ACCOUNT)
        expected_args = 3;
    else if (operation == OP_TRANSFER)
        expected_args = 2;
        
    if (expected_args != req_arg_count) {
        printf("Invalid number of arguments for requested operation\n");
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

