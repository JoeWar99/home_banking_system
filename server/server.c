#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include "../shared/utilities.h"
#include "../shared/constants.h"
#include "../shared/types.h"
#include "../shared/crypto.h"
#include "../shared/queue.h"
#include "../shared/account_utilities.h"
#include "requests.h"

#define SHARED 0

sem_t full, empty;
pthread_mutex_t mutex_accounts_database = PTHREAD_MUTEX_INITIALIZER;
/* Max Bank Account number + 1 (admin) */
bank_account_t *accounts_database[MAX_BANK_ACCOUNTS + 1];
queue_t *resquest_queue;

void *balconies(void *arg)
{

    sem_wait(&full);
    pthread_mutex_lock(&mutex_accounts_database);

    tlv_request_t *first_request = (tlv_request_t *)queue_front(resquest_queue);
    if (first_request == NULL)
    {
        perror("queue_front: error getting first queue element");
        exit(RC_OTHER);
    }

    if (queue_pop(resquest_queue) != 0)
    {
        perror("queue_pop: error removing first queue element");
        exit(RC_OTHER);
    }

    int balance = accounts_database[first_request->value.header.account_id]->balance;

    tlv_reply_t *request_reply = (tlv_reply_t *)malloc(sizeof(tlv_reply_t));
    int ret;
    if ((ret = is_valid_request(first_request, accounts_database)) == 0)
    {
        printf("Valid request. Return: %d\n", ret);

        switch (first_request->type)
        {
        case OP_BALANCE:
            request_reply->value.balance.balance = balance;
            break;

        case OP_TRANSFER:
            accounts_database[first_request->value.header.account_id]->balance = balance - first_request->value.transfer.amount;
            accounts_database[first_request->value.transfer.account_id]->balance =  accounts_database[first_request->value.transfer.account_id]->balance + first_request->value.transfer.amount;
            request_reply->value.transfer.balance =  accounts_database[first_request->value.header.account_id]->balance;
            break;
        }
    }
    else
    {
        printf("Invalid request. Return: %d\n", ret);
    }

    pthread_mutex_unlock(&mutex_accounts_database);
    sem_post(&empty);

    int user_fifo;

    request_reply->type = first_request->type;
    request_reply->value.header.account_id = first_request->value.header.account_id;
    request_reply->value.header.ret_code = ret;

     char *secure_fifo_name = (char *)malloc(sizeof(char) * (strlen(USER_FIFO_PATH_PREFIX) + 1));

    strcpy(secure_fifo_name, USER_FIFO_PATH_PREFIX);
    secure_fifo_name[strlen(USER_FIFO_PATH_PREFIX)] = '\0';

    char *string_pid = (char*) malloc(sizeof(char)*6);
    sprintf(string_pid, "%d", first_request->value.header.pid);
    string_pid[strlen(string_pid)] = '\0';

    concat(&secure_fifo_name, string_pid, strlen(string_pid));


    if ((user_fifo = open(secure_fifo_name, O_RDONLY)) == -1)
    {
        perror("open");
        exit(RC_USR_DOWN);
    }


    if(write(user_fifo, &request_reply, sizeof(tlv_reply_t))!=sizeof(tlv_reply_t)){
        perror("write: error writing to user fifo");
        exit(RC_OTHER);
    }


    if (close(user_fifo) != 0)
    {
        perror("close: error closing down server fifo");
        exit(RC_OTHER);
    }



    free(request_reply);
    free(string_pid);
    free(secure_fifo_name);
    free(request_reply);
    free(first_request);
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Usage: %s <n_threads> <admin_pwd>\n", argv[0]);
        exit(RC_OTHER);
    }

    int n_threads_console = atoi(argv[1]);

    if (n_threads_console <= 0)
    {
        printf("Error: The numbers of threads cannot be negative or zero\n");
        exit(RC_OTHER);
    }

    char *pwd = argv[2];

    if (!valid_pwd(pwd))
    {
        fprintf(stderr, "Password length must be between %d and %d\n", MIN_PASSWORD_LEN, MAX_PASSWORD_LEN);
        exit(RC_OTHER);
    }

    if ((resquest_queue = init_queue()) == NULL)
    {
        perror("init_queue: error initialing the queue");
        exit(RC_OTHER);
    }

    int n_threads = min(atoi(argv[1]), MAX_BANK_OFFICES);

    if (sem_init(&full, SHARED, 0) != 0)
    {
        perror("sem_init: erro initializing semaphore");
        exit(RC_OTHER);
    }

    if (sem_init(&empty, SHARED, n_threads) != 0)
    {
        perror("sem_init: erro initializing semaphore empty");
        exit(RC_OTHER);
    }

    int id;
    pthread_t thread_id[n_threads];

    for (int i = 0; i < n_threads; i++)
    {
        if (pthread_create(&thread_id[i], NULL, balconies, &id) != 0)
        {
            perror("pthread_create: error creating thread ");
            exit(RC_OTHER);
        }
    }

    srand(time(NULL));

    int secure_svr;

    if (mkfifo(SERVER_FIFO_PATH, 0660) < 0)
    {
        if (errno != EEXIST)
        {
            printf("Could not create fifo %s\n", SERVER_FIFO_PATH);
            exit(RC_OTHER);
        }
    }

    // CREATE METHOD TO ADD ACCOUNTS - store in a list???
    bank_account_t admin_account = {ADMIN_ACCOUNT_ID, "hash", "salt", 0};
    gen_salt(admin_account.salt, SALT_LEN + 1, SALT_LEN);
    gen_hash(pwd, admin_account.salt, admin_account.hash);

    accounts_database[admin_account.account_id] = &admin_account;

    if ((secure_svr = open(SERVER_FIFO_PATH, O_RDWR)) == -1)
    {
        perror("open");
        exit(RC_SRV_DOWN);
    }

    tlv_request_t request;

    while (1)
    {
        if (read(secure_svr, &request, sizeof(tlv_request_t)) != 0)
        {

            printf("header :   pid : %d , account_id %d , password %s, delay %d\n", request.value.header.pid, request.value.header.account_id, request.value.header.password, request.value.header.op_delay_ms);
            switch (request.type)
            {
            case OP_CREATE_ACCOUNT:
                printf("create:  accoutn_id %d, balance %d, password %s\n", request.value.create.account_id, request.value.create.balance, request.value.create.password);
                break;
            case OP_TRANSFER:
                printf("transfer:  accoutn_id %d, ammount %d\n", request.value.transfer.account_id, request.value.transfer.amount);
                break;
            case OP_BALANCE:
                printf("balace\n");
                break;
            case OP_SHUTDOWN:
                printf("shutdown\n");
                if (fchmod(secure_svr, 0444) != 0)
                {
                    perror("fchmod: error altering server fifo permissions");
                    exit(RC_OTHER);
                }
                break;
            }

            sem_wait(&empty);
            pthread_mutex_lock(&mutex_accounts_database);

            if (queue_push(resquest_queue, &request) != 0)
            {
                perror("queue_push: error pushing request to queue");
                exit(RC_OTHER);
            }

            pthread_mutex_unlock(&mutex_accounts_database);
            sem_post(&full);
        }
    }

    // Free allocated memory

    if (del_queue(resquest_queue) != 0)
    {
        perror("del_queue: error deleting queue");
        exit(RC_OTHER);
    }

    // TODO: fazer free do array

    if (close(secure_svr) != 0)
    {
        perror("close: error closing down server fifo");
        exit(RC_OTHER);
    }
    if (unlink(SERVER_FIFO_PATH) != 0)
    {
        perror("unlink: error unlinking user fifo");
        exit(RC_OTHER);
    }

    return RC_OK;
}