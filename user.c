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

int valid_request_arguments(int operation, char * req_args, int req_arg_count) {
    switch(operation) {
        case OP_CREATE_ACCOUNT:
            if (req_arg_count != 3) {
                fprintf(STDERR_FILENO, "Create account operation requires 3 arguments\n");
                return -1;
            }

            int account_id = atoi(req_args[0]);
            int balance = atoi(req_args[1]);
            char * pwd = req_args[2];

            if (account_id <= 0 || account_id >= MAX_BANK_ACCOUNTS) {
                fprintf(STDERR_FILENO, "Invalid account ID: %d\n", account_id);
                return -1;
            }

            if (balance <= 0 || balance > MAX_BALANCE) {
                fprintf(STDERR_FILENO, "Invalid balance value: %d\n", balance);
                return -1;
            }

            if (strlen(pwd) < MIN_PASSWORD_LEN || strlen(pwd) > MAX_PASSWORD_LEN) {
                fprintf(STDERR_FILENO, "Password length must be between %d and %d\n", MIN_PASSWORD_LEN, MAX_PASSWORD_LEN);
                return -1;
            }          

            break;
        case OP_BALANCE:
            if (req_arg_count != 0) {
                fprintf(STDERR_FILENO, "Balance check operation requires no arguments\n");
                return -1;
            }
            break;
        case OP_TRANSFER:
            if (req_arg_count != 2) {
                fprintf(STDERR_FILENO, "Transfer operation requires 2 arguments\n");
                return -1;
            }
            int dest_account_id = atoi(req_args[0]);
            int amount = atoi(req_args[1]);
            
            if (dest_account_id <= 0 || dest_account_id >= MAX_BANK_ACCOUNTS) {
                fprintf(STDERR_FILENO, "Invalid account ID: %d\n", dest_account_id);
                return -1;
            }

            if (amount <= 0 || amount > MAX_BALANCE) {
                fprintf(STDERR_FILENO, "Invalid amount value: %d\n", amount);
                return -1;
            }

            break;
        case OP_SHUTDOWN:
            if (req_arg_count != 0) {
                fprintf(STDERR_FILENO, "Shutdown account operation requires no arguments\n");
                return -1;
            }
            break;
    }
    return 0;
}

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

    if(valid_request_arguments(operation, req_args, req_arg_count) == -1) {
        fprintf(STDERR_FILENO, "Invalid request arguments\n");
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

