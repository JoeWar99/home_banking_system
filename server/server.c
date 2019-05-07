#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../shared/vetor.h"
#include "../shared/utilities.h"
#include "../shared/constants.h"
#include "../shared/types.h"
#include "../shared/crypto.h"
#include "../shared/queue.h"

void *balconies (void *arg){











}

int main(int argc, char * argv[]){

    if (argc != 3) {
        printf("Usage: %s <n_threads> <admin_pwd>\n", argv[0]);
        exit(RC_OTHER);
    }

    int n_threads_console = atoi(argv[1]);

    if(n_threads_console <= 0){
        printf("Error: The numbers of threads cannot be negative or zero\n");
        exit(RC_OTHER);
    }

    srand(time(NULL));

    int secure_svr;
    int n_threads;
    
    

    // VAO TER DE SER GLOBAIS PARA PODEREM SER ACESSIVEIS A TODOS OS THREADS ??
    vetor* accounts_database = vetor_novo();
    queue* resquest_queue = init_queue();


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


    vetor_insere(accounts_database, &admin_account, -1);

    

    if ((secure_svr = open(SERVER_FIFO_PATH, O_RDWR)) == -1) {
        perror("open");
        exit(RC_SRV_DOWN);
    }

    
    
    while (1)
    {
        /* code */
    }
    
    if(fchmod(secure_svr, 0444) != 0){
        perror("fchmod: error altering server fifo permissions");
        exit(RC_OTHER);
    }





    // Free allocated memory

    if(empty_queue(resquest_queue) != 0){
        perror("empt_queue: error emptying queue");
        exit(RC_OTHER);
    }

    free(resquest_queue);

    if(vetor_free(accounts_database)!= 0){
        perror("vetor_free: error emptying vector");
        exit(RC_OTHER);
    }

    free(accounts_database);


    if(close(secure_svr)!= 0){ 
        perror("close: error closing down server fifo");
        exit(RC_OTHER);
    }
    if(unlink(SERVER_FIFO_PATH)!= 0){
        perror("unlink: error unlinking user fifo");
        exit(RC_OTHER);
    }

    return RC_OK;
}