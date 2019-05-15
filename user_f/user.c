#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include "../shared/utilities.h"
#include "../shared/constants.h"
#include "../shared/types.h"
#include "../shared/com_protocol.h"
#include "../shared/sope.h"
#include "user_parse.h"

int user_fifo;
int secure_svr;

int opened_user_fifo = 0;
int opened_srv_fifo = 0;
int logged_request = 0;
int logged_reply = 0;

tlv_request_t full_request;
tlv_reply_t request_reply;
sem_t * ulog_sem;

void exit_user_process(ret_code_t ret)
{
    int pid = getpid();
    char secure_fifo_name[strlen(USER_FIFO_PATH_PREFIX) + WIDTH_PID + 1];
    init_secure_fifo_name(secure_fifo_name, pid);

    if (!logged_request) 
    {
		if(sem_wait(ulog_sem) != 0){
			perror("ulog_sem failure sem_wait");
			exit(RC_OTHER);
		}
        if (logRequest(STDOUT_FILENO, pid, &full_request) < 0)
        {
            fprintf(stderr, "logRequest: error writing request to stdout\n");
            exit(RC_OTHER);
        }
		if(sem_post(ulog_sem) != 0){
			perror("ulog_sem failure sem_post");
			exit(RC_OTHER);
		}
    }

    if (!logged_reply) 
    {
		if(sem_wait(ulog_sem) != 0){
			perror("ulog_sem failure sem_wait");
			exit(RC_OTHER);
		}
        if (logReply(STDOUT_FILENO, pid, &request_reply) < 0)
        {
            fprintf(stderr, "logRequest: error writing reply to stdout\n");
            exit(RC_OTHER);
        }
		if(sem_post(ulog_sem) != 0){
			perror("ulog_sem failure sem_post");
			exit(RC_OTHER);
		}
    }
   
    if (opened_srv_fifo)
    {
        if (close(secure_svr) != 0)
        {
            perror("close: error closing down server fifo");
            exit(RC_OTHER);
        }
    }

    if (opened_user_fifo) {
        if (close(user_fifo) != 0)
        {
            perror("close: error closing down user fifo");
            exit(RC_OTHER);
        }
    }

    /* Unlink user specific fifo */
    if (unlink(secure_fifo_name) != 0)
    {
        perror("unlink: error unlinking user fifo");
        exit(RC_OTHER);
    }

	if(sem_close(ulog_sem)){
		perror("ulog_sem failure sem_close");
		exit(RC_OTHER);
	}

    exit(ret);
}

void sigalarm_handler()
{
    // TODO faltam os frees quando sai por causa do alarme
    init_reply_error(&request_reply, &full_request, RC_SRV_TIMEOUT);
    exit_user_process(RC_SRV_TIMEOUT);
}

int main(int argc, char *argv[])
{

    if (argc != 6)
    {
        printf("Usage: %s <account_id> <account_pwd> <delay> <operation> <request_args>\n", argv[0]);
        exit(RC_OTHER);
    }

    struct sigaction alarm_aux;
    alarm_aux.sa_handler = sigalarm_handler;
    sigemptyset(&alarm_aux.sa_mask);
    alarm_aux.sa_flags = 0;
    sigaction(SIGALRM, &alarm_aux, NULL);

    int pid = getpid();
    int account_id, op_delay, operation;

    account_id = atoi(argv[1]);
    char *pwd = argv[2];
    op_delay = atoi(argv[3]);
    operation = atoi(argv[4]);

    char *req_args[3];
    int req_arg_count = 0;

    if (!valid_args(account_id, pwd, op_delay, operation))
    {
        fprintf(stderr, "Invalid arguments given\n");
        exit(RC_OTHER);
    }

    if (parse_req_args(argv[5], req_args, &req_arg_count) == -1)
    {
        fprintf(stderr, "Error parsing request arguments\n");
        exit(RC_OTHER);
    }

    if (!valid_req_args(operation, req_args, req_arg_count))
    {
        fprintf(stderr, "Invalid request arguments\n");
        exit(RC_OTHER);
    }

    /* Open log file and point STDOUT to it */
    int logfile;
    if ((logfile = open(USER_LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1) {
        perror("Open user log file");
        exit(RC_OTHER);
    }
    dup2(logfile, STDOUT_FILENO);

	if((ulog_sem = sem_open(ULOG_SEM_NAME, 0, 0600, 0)) == SEM_FAILED){
		perror("ulog_sem failure in sem_open()");     
		exit(RC_OTHER); 
	}

    /* Create user specific fifo */
    char secure_fifo_name[strlen(USER_FIFO_PATH_PREFIX) + WIDTH_PID + 1];
    init_secure_fifo_name(secure_fifo_name, pid);
    if (mkfifo(secure_fifo_name, 0660) < 0)
    {
        if (errno != EEXIST)
        {
            fprintf(stderr, "Could not create fifo %s\n", SERVER_FIFO_PATH);
            exit(RC_OTHER);
        }
    }

    init_request(&full_request, operation, pid, account_id, pwd, op_delay, req_args);

    /* Open server fifo */
    if ((secure_svr = open(SERVER_FIFO_PATH, O_WRONLY | O_NONBLOCK)) == -1)
    {
        /* SERVER DOWN */
        init_reply_error(&request_reply, &full_request, RC_SRV_DOWN);

        for (int i = 0; i < req_arg_count; i++)
        {
            free(req_args[i]);
        }

        exit_user_process(RC_SRV_DOWN);
    }
    opened_srv_fifo = 1;

    /* Write request */
    if (write_request(secure_svr, &full_request) != 0)
    {
        fprintf(stderr, "write_request: error writing request to server\n");
        exit(RC_OTHER);
    }

	// TODO: isto nunca e usado!! ver melhor
    //int error_due_to_alarm = 0;

    /* Set alarm */
    alarm(FIFO_TIMEOUT_SECS);

	if(sem_wait(ulog_sem) != 0){
		perror("ulog_sem failure sem_wait");
		exit(RC_OTHER);
	}
    if (logRequest(STDOUT_FILENO, pid, &full_request) < 0)
    {
        fprintf(stderr, "logRequest: error writing request to stdout\n");
        exit(RC_OTHER);
    }
	if(sem_post(ulog_sem) != 0){
		perror("ulog_sem failure sem_post");
		exit(RC_OTHER);
	}
    logged_request = 1;

    /* Open user specific fifo */
    if ((user_fifo = open(secure_fifo_name, O_RDONLY)) == -1)
    {
        /* USER DOWN */
        init_reply_error(&request_reply, &full_request, RC_USR_DOWN);

        for (int i = 0; i < req_arg_count; i++)
        {
            free(req_args[i]);
        }

        exit_user_process(RC_USR_DOWN);
    }
    opened_user_fifo = 1;

    /* Read server reply */
    if (read_reply(user_fifo, &request_reply) != 0)
    {
        fprintf(stderr, "read_reply: error reading reply from fifo\n");
        exit(RC_OTHER);
    }
    
    alarm(0);

	if(sem_wait(ulog_sem) != 0){
		perror("ulog_sem failure sem_wait");
		exit(RC_OTHER);
	}
    if (logReply(STDOUT_FILENO, pid, &request_reply) < 0)
    {
        fprintf(stderr, "logRequest: error writing reply to stdout\n");
        exit(RC_OTHER);
    }
	if(sem_post(ulog_sem) != 0){
		perror("ulog_sem failure sem_post");
		exit(RC_OTHER);
	}
    logged_reply = 1;

    /* Free alocated memory */
    for (int i = 0; i < req_arg_count; i++)
    {
        free(req_args[i]);
    }

    exit_user_process(RC_OK);
}