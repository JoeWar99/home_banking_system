#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "com_protocol.h"
#include "utilities.h"
#include "../server_f/sync.h"


void init_request(tlv_request_t * full_request, int operation, int pid, int account_id, char * pwd, int op_delay, char ** req_args) {

    uint32_t req_size = 0;

    full_request->type = operation;

    req_value_t * request_value = &(full_request->value);

    req_header_t * user_header = &(request_value->header);
    user_header->pid = pid;
    user_header->account_id = account_id;
    strcpy(user_header->password, pwd);
    user_header->op_delay_ms = op_delay;
    req_size += sizeof(req_header_t);

    req_create_account_t * user_create;
    req_transfer_t * user_transfer;
    switch (operation)
    {
        case OP_CREATE_ACCOUNT:
            user_create = &(request_value->create);
            user_create->account_id = atoi(req_args[0]);
            user_create->balance = atoi(req_args[1]);
            strcpy(user_create->password, req_args[2]);
            req_size += sizeof(req_create_account_t);
            break;

        case OP_TRANSFER:
            user_transfer = &(request_value->transfer);
            user_transfer->account_id = atoi(req_args[0]);
            user_transfer->amount = atoi(req_args[1]);
            req_size += sizeof(req_transfer_t);
            break;
		default:
			break;
    }

    full_request->length = req_size;
}

int init_reply(tlv_reply_t * reply, tlv_request_t * request, int ret, int active_offices, uint32_t balance){

	uint32_t rep_size = 0;

	/* Init type */
	reply->type = request->type;

	/* Init value.header */
	reply->value.header.account_id = request->value.header.account_id;
	reply->value.header.ret_code = ret;
	rep_size += sizeof(rep_header_t);

	/* Init value.union */
	if(ret == 0){
		switch(reply->type){
			case OP_BALANCE:				
				reply->value.balance.balance = balance;
				rep_size += sizeof(rep_balance_t);
				break;

			case OP_SHUTDOWN:
				reply->value.shutdown.active_offices = active_offices - 1;
				rep_size += sizeof(rep_shutdown_t);
				break;

			default:
				break;
		}
	}
	if(reply->type == OP_TRANSFER){
		reply->value.transfer.balance = balance;
		rep_size += sizeof(rep_transfer_t);
	}

	/* Init length */
	reply->length = rep_size;
	return 0;
}

int init_reply_error(tlv_reply_t * reply, tlv_request_t * request, int ret) {
	uint32_t rep_size = 0;

	/* Init type */
	reply->type = request->type;

	/* Init value.header */
	reply->value.header.account_id = request->value.header.account_id;
	reply->value.header.ret_code = ret;
	rep_size += sizeof(rep_header_t);

	/* Init length */
	reply->length = rep_size;
	return 0;
}

int init_secure_fifo_name(char * fifo_name, pid_t pid){

	size_t prefix_size = strlen(USER_FIFO_PATH_PREFIX);
	strcpy(fifo_name, USER_FIFO_PATH_PREFIX);
	sprintf(&fifo_name[prefix_size], "%d", pid);
	fifo_name[prefix_size+WIDTH_PID] = '\0';
	return 0;
}

int write_reply(int fd, const tlv_reply_t * request_reply){
	int total_size = request_reply->length + sizeof(op_type_t) + sizeof(uint32_t);
	if (write(fd, request_reply, total_size) != total_size)
		return -1;
	return 0;
}

int read_reply(int fd, tlv_reply_t * request_reply){
	if (read(fd, &(request_reply->type), sizeof(op_type_t)) <= 0)
		return -1;
	if (read(fd, &(request_reply->length), sizeof(uint32_t)) <= 0)
		return -1;
	if (read(fd, &(request_reply->value), request_reply->length) <= 0)
		return -1;
	return 0;
}

int write_request(int fd, const tlv_request_t * request){
	int total_size = request->length + sizeof(op_type_t) + sizeof(uint32_t);
	if (write(fd, request, total_size) != total_size)
		return -1;
	
	return 0;
}

int read_request(int fd, tlv_request_t * request){
	op_type_t type;
	uint32_t req_length;
	int read_val;

	if ((read_val = read(fd, &type, sizeof(op_type_t))) <= 0) 
		return read_val;

	if ((read_val = read(fd, &req_length, sizeof(uint32_t)))  <= 0) 
		return read_val;

	request->type = type;
	request->length = req_length;

	if ((read_val = read(fd, &(request->value), request->length)) <= 0) {
		return read_val;
	}

	return RC_OK;
}