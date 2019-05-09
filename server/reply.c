#include "reply.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/utilities.h"

int init_reply(tlv_reply_t * reply, tlv_request_t * request, int ret, bank_account_t * accounts_database[]){
	//TODO verify rep_size is correct
	uint32_t rep_size = 0;
	rep_size += sizeof(reply->length);

	/* Init type */
	reply->type = request->type;
	rep_size += sizeof(reply->type);


	/* Init value.header */
	reply->value.header.account_id = request->value.header.account_id;
	reply->value.header.ret_code = ret;
	rep_size += sizeof(rep_header_t);

	// TODO: verificar o que enviar em caso de erro
	/* Init value.union */
	if(ret == 0){
		switch(reply->type){
			case OP_TRANSFER:
				reply->value.transfer.balance = accounts_database[request->value.header.account_id]->balance;
				rep_size += sizeof(rep_transfer_t);
				break;
			case OP_BALANCE:
				reply->value.balance.balance = accounts_database[request->value.header.account_id]->balance;
				rep_size += sizeof(rep_balance_t);
				break;
			case OP_SHUTDOWN:
				// TODO: fazer isto
				reply->value.shutdown.active_offices = 69;
				rep_size += sizeof(rep_shutdown_t);
				printf("To be implemented\n");
		}
	}

	/* Init length */
	reply->length = rep_size;
	return 0;
}

int init_reply_fifo_name(char * fifo_name, pid_t pid){

	strcpy(fifo_name, USER_FIFO_PATH_PREFIX);
	fifo_name[strlen(USER_FIFO_PATH_PREFIX)] = '\0';

	char *string_pid = (char*) malloc(sizeof(char)*6);
	sprintf(string_pid, "%d", pid);
	string_pid[strlen(string_pid)] = '\0';

	concat(&fifo_name, string_pid, strlen(string_pid));
	return 0;
}