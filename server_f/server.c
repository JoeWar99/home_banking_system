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
#include "../shared/com_protocol.h"
#include "../shared/sope.h"
#include "server_parse.h"
#include "requests.h"

sem_t full, empty;

bank_account_t *accounts_database[MAX_BANK_ACCOUNTS];
pthread_mutex_t accounts_db_mutex[MAX_BANK_ACCOUNTS];

queue_t *request_queue;
pthread_mutex_t req_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void *balconies(void *arg)
{
    int id_thread = *(int *)arg;
    int pid_thread = pthread_self();
    int ret;
    int full_aux;
    int empty_aux;
    tlv_request_t *first_request;

    logBankOfficeOpen(STDOUT_FILENO, id_thread, pid_thread);

    /*
    * Passos:
    * 	1) Receber pedido
    *   2) Validar pedido
    * 	3) Se válido executar pedido
    * 	4) Responder ao cliente    
    */
    while (1)
    {
        if (sem_getvalue(&full, &full_aux) != 0)
        {
            perror("sem_get_value:");
            exit(RC_OTHER);
        }

        /* Wait full */
        logSyncMechSem(STDOUT_FILENO, id_thread, SYNC_OP_SEM_WAIT, SYNC_ROLE_CONSUMER, 0, full_aux);
        sem_wait(&full);

        /* Wait mutex */
        pthread_mutex_lock(&req_queue_mutex);
        logSyncMech(STDOUT_FILENO, id_thread, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_CONSUMER, 0);

        /* Take item */
        first_request = (tlv_request_t *)queue_front(request_queue);
        if (first_request == NULL)
        {
            fprintf(stderr, "queue_front: error getting first queue element\n");
            exit(RC_OTHER);
        }

        if (queue_pop(request_queue) != 0)
        {
            fprintf(stderr, "queue_pop: error removing first queue element\n");
        //    exit(RC_OTHER);
        }

        /* Signal mutex */
        pthread_mutex_unlock(&req_queue_mutex);
        logSyncMech(STDOUT_FILENO, id_thread, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_CONSUMER, first_request->value.header.pid);

        /* Signal empty */
        sem_post(&empty);
        if (sem_getvalue(&empty, &empty_aux) != 0)
        {
            perror("sem_get_value:");
            exit(RC_OTHER);
        }
        logSyncMechSem(STDOUT_FILENO, id_thread, SYNC_OP_SEM_POST, SYNC_ROLE_CONSUMER, first_request->value.header.pid, empty_aux);


        // TODO: change to TID later
        if (logRequest(STDOUT_FILENO, 6969, first_request) < 0)
        {
            fprintf(stderr, "logRequest: error writing request to stdout\n");
        //    exit(RC_OTHER);
        }

        if ((ret = is_valid_request(first_request, accounts_database)) == 0)
        {
            switch (first_request->type)
            {
            case OP_CREATE_ACCOUNT:
                if (create_request(&first_request->value, accounts_database) != 0)
                {
                    fprintf(stderr, "create_request: failed to create account.\n");
                    //   exit(RC_OTHER);
                }
                break;
            case OP_TRANSFER:
                // printf("transfer:  account_id %d, ammount %d\n", request.value.transfer.account_id, request.value.transfer.amount);
                transfer_request(&first_request->value, accounts_database);
                break;
            case OP_BALANCE:
                // printf("balance\n");
                break;
            case OP_SHUTDOWN:
                // printf("shutdown\n");
                /* if (fchmod(secure_svr, 0444) != 0)
                {
                    perror("fchmod: error altering server fifo permissions");*/
                //  exit(RC_OTHER);
                break;
            }
        }

        char secure_fifo_name[strlen(USER_FIFO_PATH_PREFIX) + WIDTH_PID + 1];
        init_secure_fifo_name(secure_fifo_name, first_request->value.header.pid);

        int user_fifo;
        if ((user_fifo = open(secure_fifo_name, O_WRONLY)) == -1)
        {
            perror("secure_fifo_name");
            //exit(RC_USR_DOWN);
        }

        tlv_reply_t request_reply;
        pthread_mutex_lock(&accounts_db_mutex[first_request->value.header.account_id]);
        init_reply(&request_reply, first_request, ret, accounts_database);
        pthread_mutex_unlock(&accounts_db_mutex[first_request->value.header.account_id]);

        // TODO: change to TID later
        if (logReply(STDOUT_FILENO, id_thread, &request_reply) < 0)
        {
            fprintf(stderr, "logRequest: error writing reply to stdout\n");
        }

        /* Send reply to user */
        if (write_reply(user_fifo, &request_reply) != 0)
        {
            fprintf(stderr, "write_reply: error writing reply to server\n");
        }

        /* Close user specific fifo */
        if (close(user_fifo) != 0)
        {
            perror("error closing down server fifo");
        }

        free(first_request);
    }

    logBankOfficeClose(STDOUT_FILENO, id_thread, pid_thread);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <n_threads> <admin_pwd>\n", argv[0]);
        exit(RC_OTHER);
    }

    /* Generate new random seed */
    srand(time(NULL));

    int n_threads = min(atoi(argv[1]), MAX_BANK_OFFICES);
    char *pwd = argv[2];
    
    if(!valid_args(n_threads, pwd)) {
        fprintf(stderr, "Invalid arguments given\n");
        exit(RC_OTHER);
    }

    /* Initialize requests queue */
    if ((request_queue = init_queue()) == NULL)
    {
        perror("init_queue: error initialing the queue");
        exit(RC_OTHER);
    }

    /* TODO what is this??? maybe define a macro */
    int id_main_thread = 0;

    /* Initialize FULL semaphore with 0 */
    if (sem_init(&full, SHARED_SEM, 0) != 0)
    {
        perror("sem_init: error initializing full semaphore");
        exit(RC_OTHER);
    }

    logSyncMechSem(STDOUT_FILENO, id_main_thread, SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, id_main_thread, 0);

    /* Initialize EMPTY semaphore with number of threads */
    if (sem_init(&empty, SHARED_SEM, n_threads) != 0)
    {
        perror("sem_init: error initializing empty semaphore");
        exit(RC_OTHER);
    }

    logSyncMechSem(STDOUT_FILENO, id_main_thread, SYNC_OP_SEM_INIT, SYNC_ROLE_PRODUCER, id_main_thread, n_threads);

    /* Create server fifo */
    if (mkfifo(SERVER_FIFO_PATH, 0660) < 0)
    {
        if (errno != EEXIST)
        {
            fprintf(stderr, "Could not create fifo %s\n", SERVER_FIFO_PATH);
            exit(RC_OTHER);
        }
    }

    /* Create balconies */
    int id[n_threads];
    pthread_t thread_tid[n_threads];
    for (int i = 0; i < n_threads; i++)
    {
        id[i] = i + 1;
        if (pthread_create(&thread_tid[i], NULL, balconies, &id[i]) != 0)
        {
            perror("pthread_create: error creating thread ");
            exit(RC_OTHER);
        }
    }

    /* Allocate accounts_database and initialize corresponding mutexes */
    for (uint32_t i = 0; i < MAX_BANK_ACCOUNTS; i++)
    {
        accounts_database[i] = (bank_account_t *)malloc(sizeof(bank_account_t));
        accounts_database[i]->account_id = EMPTY_ACCOUNT_ID;
        pthread_mutex_init(&accounts_db_mutex[i], NULL);
    }

    /* Create admin account */
    int ret;
    if ((ret = create_account(pwd, ADMIN_ACCOUNT_ID, 0, accounts_database)) != 0)
    {
        fprintf(stderr, "create_account: failed to create ADMIN_ACCOUNT. Error: %d\n", ret);
        exit(RC_OTHER);
    }

    /* Open server fifo */
    int secure_svr;
    if ((secure_svr = open(SERVER_FIFO_PATH, O_RDWR)) == -1)
    {
        perror("open");
        exit(RC_SRV_DOWN);
    }

    int full_aux;
    int empty_aux;
    while (1)
    {

        /* Produce item */
        tlv_request_t * request;
        if ((request=read_request(secure_svr)) == NULL)
        {
            fprintf(stderr, "read_request: error reading the request\n");
            break;
        }

        if (logRequest(STDOUT_FILENO, id_main_thread, request) < 0)
        {
            fprintf(stderr, "logRequest: error writing request to stdout\n");
            exit(RC_OTHER);
        }

        if (sem_getvalue(&empty, &empty_aux) != 0)
        {
            perror("sem_get_value:");
            exit(RC_OTHER);
        }

        /* Wait empty */
        logSyncMechSem(STDOUT_FILENO, id_main_thread, SYNC_OP_SEM_WAIT, SYNC_ROLE_PRODUCER, request->value.header.pid, empty_aux);
        sem_wait(&empty);

        /* Wait mutex */
        pthread_mutex_lock(&req_queue_mutex);
        logSyncMech(STDOUT_FILENO, id_main_thread, SYNC_OP_MUTEX_LOCK, SYNC_ROLE_ACCOUNT, request->value.header.pid);

        /* Append item */
        if (queue_push(request_queue, request) != 0)
        {
            fprintf(stderr, "queue_push: error pushing request to queue\n");
            exit(RC_OTHER);
        }

        /* Signal mutex */
        pthread_mutex_unlock(&req_queue_mutex);
        logSyncMech(STDOUT_FILENO, id_main_thread, SYNC_OP_MUTEX_UNLOCK, SYNC_ROLE_ACCOUNT, request->value.header.pid);

        /* Signal full */
        sem_post(&full);
        if (sem_getvalue(&full, &full_aux) != 0)
        {
            perror("sem_get_value:");
            exit(RC_OTHER);
        }
        logSyncMechSem(STDOUT_FILENO, id_main_thread, SYNC_OP_SEM_POST, SYNC_ROLE_PRODUCER, request->value.header.pid, full_aux);
    }

    /* Delete requests queue */
    if (del_queue(request_queue) != 0)
    {
        fprintf(stderr, "del_queue: error deleting queue\n");
        exit(RC_OTHER);
    }

    /* Free accounts database allocated memory */
    for (uint32_t i = 0; i < MAX_BANK_ACCOUNTS; i++)
        free(accounts_database[i]);

    /* Close server fifo descriptor */
    if (close(secure_svr) != 0)
    {
        perror("close: error closing down server fifo");
        exit(RC_OTHER);
    }

    /* Unlink server fifo */
    if (unlink(SERVER_FIFO_PATH) != 0)
    {
        perror("unlink: error unlinking user fifo");
        exit(RC_OTHER);
    }

    return RC_OK;
}