#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "utilities.h"
#include "constants.h"
#include "types.h"
#include "crypto.h"

int main(int argc, char * argv[]){

    if (argc != 3) {
        printf("Usage: %s <n_threads> <admin_pwd>\n", argv[0]);
        exit(RC_OTHER);
    }

    srand(time(NULL));

    int secure_svr;
    int n_threads;

    if (mkfifo(SERVER_FIFO_PATH, 0660) < 0 ) {
        if(errno!=EEXIST){
            printf("Could not create fifo %s\n", SERVER_FIFO_PATH);
            exit(RC_OTHER);
        }        
    }

    n_threads = min(atoi(argv[1]), MAX_BANK_OFFICES);

    // CREATE METHOD TO ADD ACCOUNTS - store in a list???
    struct bank_account admin_account = {ADMIN_ACCOUNT_ID , "hash", "salt", 0 };
    gen_salt(admin_account.salt, SALT_LEN+1, SALT_LEN);
    gen_hash(argv[2], admin_account.salt, admin_account.hash);


    if ((secure_svr = open(SERVER_FIFO_PATH, O_RDWR)) == -1) {
        perror("open");
        exit(RC_SRV_DOWN);
    }


    //MENSAGEM DE ENCERRAMENTO
    //int fchmod(int fd, mode_t mode)

    fchmod(secure_svr, 0444);


    close(secure_svr);
    unlink(SERVER_FIFO_PATH);

    return RC_OK;
}





