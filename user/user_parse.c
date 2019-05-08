#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "user_parse.h"
#include "../shared/constants.h"
#include "../shared/types.h"
#include "../shared/utilities.h"

int valid_user_account_id(int account_id) {
    return account_id >= 0 && account_id < MAX_BANK_ACCOUNTS;
}

int valid_pwd(char * pwd) {
    return strlen(pwd) >= MIN_PASSWORD_LEN && strlen(pwd) <= MAX_PASSWORD_LEN;
}

int valid_balance(long unsigned int balance) {
    return balance >= MIN_BALANCE && balance <= MAX_BALANCE;
}

int valid_delay(int delay) {
    return delay >= 0 && delay <= MAX_OP_DELAY_MS;
}

int valid_args(int account_id, char * account_pwd, int delay, int operation) {

    if (!valid_user_account_id(account_id)) {
        fprintf(stderr, "Invalid account ID: %d\n", account_id);
        return 0;
    }

    if (!valid_pwd(account_pwd)) {
        fprintf(stderr, "Password length must be between %d and %d\n", MIN_PASSWORD_LEN, MAX_PASSWORD_LEN);
        return 0;
    }

    if (!valid_delay(delay)) {
        fprintf(stderr, "Operation delay must be between 0 and %d\n", MAX_OP_DELAY_MS);
        return 0;
    }

    if (operation < 0 || operation > 3) {
        fprintf(stderr, "Invalid operation specified\n");
        return 0;
    }

    return 1;
}

int parse_req_args(char * req_args_string, char * req_args[3], int * req_arg_count) {

    const char delim[2] = " ";
    char *token = strtok(req_args_string, delim);
    for (*req_arg_count = 0; *req_arg_count <= 3; (*req_arg_count)++) {

        if (token == NULL) break;

        if (*req_arg_count == 3) {
            fprintf(stderr, "Invalid number of request arguments\n");
            return -1;
        }

        req_args[*req_arg_count] = (char *) malloc(sizeof(char) * (strlen(token)+1));
        strcpy(req_args[*req_arg_count], token);
        token = strtok(NULL, delim);
    }
    return 0;
}

int valid_req_args(int operation, char ** req_args, int req_arg_count) {

    switch(operation) {
        case OP_CREATE_ACCOUNT:        
            if (req_arg_count != 3) {
                fprintf(stderr, "Create account operation requires 3 arguments\n");
                return 0;
            }

            int account_id = atoi(req_args[0]);
            long unsigned int balance = str_to_lu(req_args[1]);
            char * pwd = req_args[2];

            if (!valid_user_account_id(account_id)) {
                fprintf(stderr, "Invalid account ID: %d\n", account_id);
                return 0;
            }

            if (!valid_balance(balance)) {
                fprintf(stderr, "Invalid balance value: %lu\n", balance);
                return 0;
            }

            if (!valid_pwd(pwd)) {
                fprintf(stderr, "Password length must be between %d and %d\n", MIN_PASSWORD_LEN, MAX_PASSWORD_LEN);
                return 0;
            }
            break;

        case OP_BALANCE:
            if (req_arg_count != 0) {
                fprintf(stderr, "Balance check operation requires no arguments\n");
                return 0;
            }
            break;

        case OP_TRANSFER:
            if (req_arg_count != 2) {
                fprintf(stderr, "Transfer operation requires 2 arguments\n");
                return 0;
            }

            int dest_account_id = atoi(req_args[0]);
            long unsigned int amount = str_to_lu(req_args[1]);
            
            if (!valid_user_account_id(dest_account_id)) {
                fprintf(stderr, "Invalid account ID: %d\n", dest_account_id);
                return 0;
            }

            if (!valid_balance(amount)) {
                fprintf(stderr, "Invalid amount value: %lu\n", amount);
                return 0;
            }

            break;

        case OP_SHUTDOWN:
            if (req_arg_count != 0) {
                fprintf(stderr, "Shutdown account operation requires no arguments\n");
                return 0;
            }
            break;
    }
    return 1;
}
