#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user_parse.h"
#include "../shared/constants.h"
#include "../shared/types.h"

int parse_req_args(char * req_args_string, char * req_args[3], int * req_arg_count) {

    const char delim[2] = " ";
    char *token = strtok(req_args_string, delim);
    for (*req_arg_count = 0; *req_arg_count <= 3; (*req_arg_count)++) {

        if (token == NULL) break;

        if (*req_arg_count == 3) {
            printf("Invalid number of request arguments\n");
            return -1;
        }

        req_args[*req_arg_count] = (char *) malloc(sizeof(char) * (strlen(token)+1));
        strcpy(req_args[*req_arg_count], token);
        token = strtok(NULL, delim);
    }
    return 0;
}

int valid_req_args(int operation, char ** req_args, int req_arg_count) {

    if (operation < 0 || operation > 3) {
        printf("Invalid operation specified\n");
        return -1;
    }

    switch(operation) {
        case OP_CREATE_ACCOUNT:
            if (req_arg_count != 3) {
                fprintf(stderr, "Create account operation requires 3 arguments\n");
                return -1;
            }

            int account_id = atoi(req_args[0]);
            char *ptr1;
            long unsigned int balance = strtoul(req_args[1], &ptr1, 10);
            char * pwd = req_args[2];

            if (account_id <= 0 || account_id >= MAX_BANK_ACCOUNTS) {
                fprintf(stderr, "Invalid account ID: %d\n", account_id);
                return -1;
            }

            if (balance <= 0 || balance > MAX_BALANCE) {
                fprintf(stderr, "Invalid balance value: %lu\n", balance);
                return -1;
            }

            if (strlen(pwd) < MIN_PASSWORD_LEN || strlen(pwd) > MAX_PASSWORD_LEN) {
                fprintf(stderr, "Password length must be between %d and %d\n", MIN_PASSWORD_LEN, MAX_PASSWORD_LEN);
                return -1;
            }
            break;

        case OP_BALANCE:
            if (req_arg_count != 0) {
                fprintf(stderr, "Balance check operation requires no arguments\n");
                return -1;
            }
            break;

        case OP_TRANSFER:
            if (req_arg_count != 2) {
                fprintf(stderr, "Transfer operation requires 2 arguments\n");
                return -1;
            }
            int dest_account_id = atoi(req_args[0]);
            char *ptr2;
            long unsigned int amount = strtoul(req_args[1], &ptr2, 10);
            
            if (dest_account_id <= 0 || dest_account_id >= MAX_BANK_ACCOUNTS) {
                fprintf(stderr, "Invalid account ID: %d\n", dest_account_id);
                return -1;
            }

            if (amount <= 0 || amount > MAX_BALANCE) {
                fprintf(stderr, "Invalid amount value: %lu\n", amount);
                return -1;
            }

            break;

        case OP_SHUTDOWN:
            if (req_arg_count != 0) {
                fprintf(stderr, "Shutdown account operation requires no arguments\n");
                return -1;
            }
            break;
    }
    return 0;
}
