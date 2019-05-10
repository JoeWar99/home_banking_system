#include <stdio.h>
#include "../shared/types.h"
#include "../shared/account_utilities.h"
#include "server_parse.h"

int valid_args(int n_threads, char * pwd) {
    if (n_threads <= 0)
    {
        fprintf(stderr, "Error: The numbers of threads must be greater than 0\n");
        return 0;
    }

    if (!valid_pwd(pwd))
    {
        fprintf(stderr, "Password length must be between %d and %d\n", MIN_PASSWORD_LEN, MAX_PASSWORD_LEN);
        return 0;
    }
    
    return 1;
}