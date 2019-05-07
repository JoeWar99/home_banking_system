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

int gen_random_int() {
    int val = rand() % 0xff;
    val |= (rand() % 0xff) << 8;
    val |= (rand() % 0xff) << 16;
    val |= (rand() % 0xff) << 24;
    return val;
}

int gen_salt(char * buf, int buf_size, int salt_size) {

    if (salt_size >= buf_size) return -1;

    int iters = salt_size / sizeof(int);
    
    for (int i = 0; i < iters; i++) {
        sprintf(&buf[i*sizeof(int)], "%x", gen_random_int());
    }
    buf[salt_size] = '\0';

    return 0;
}

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
            printf("ERRO - could not create fifo %s\n", SERVER_FIFO_PATH);
            exit(RC_OTHER);
        }        
    }

    n_threads = min(atoi(argv[1]), MAX_BANK_OFFICES);

    // account id ; hash ; salt ; balance
    struct bank_account admin_account = {0 , "hash", "salt", 0 };

    //printf("%d %s %s %d\n", admin_account.account_id, admin_account.hash, admin_account.salt, admin_account.balance);

    gen_salt(admin_account.salt, SALT_LEN+1, SALT_LEN);
    printf("%s\n", admin_account.salt);


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





