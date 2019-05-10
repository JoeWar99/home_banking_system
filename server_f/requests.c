#include <stdio.h>
#include <pthread.h>
#include "requests.h"
#include "../shared/constants.h"
#include "../shared/account_utilities.h"

extern pthread_mutex_t accounts_db_mutex[MAX_BANK_ACCOUNTS]; 

static int is_valid_create_request(const req_value_t * request_value, bank_account_t * accounts_database[]);

static int is_valid_transfer_request(const req_value_t * request_value, bank_account_t * accounts_database[]);

static int is_valid_balance_request(const req_value_t * request_value, bank_account_t * accounts_database[]);

static int is_valid_shutdown_request(const req_value_t * request_value, bank_account_t * accounts_database[]);

int is_valid_request(tlv_request_t * request, bank_account_t * accounts_database[]){
	switch (request->type)
	{
		case OP_CREATE_ACCOUNT:
			return is_valid_create_request(&request->value, accounts_database);
		case OP_TRANSFER:
			return is_valid_transfer_request(&request->value, accounts_database);
		case OP_BALANCE:
			return is_valid_balance_request(&request->value, accounts_database);
		case OP_SHUTDOWN:
			return is_valid_shutdown_request(&request->value, accounts_database);
		default:
			return RC_OTHER;
	}
}

int create_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	int ret_code = 0;
	uint32_t create_id = request_value->create.account_id;

	pthread_mutex_lock(&accounts_db_mutex[create_id]);

	if(create_account(request_value->create.password, create_id, request_value->create.balance, accounts_database) != 0)
		ret_code = -1;

	pthread_mutex_unlock(&accounts_db_mutex[create_id]);

	return ret_code;;
}

int transfer_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	uint32_t orig_id = request_value->header.account_id;
	uint32_t dest_id = request_value->transfer.account_id;

	pthread_mutex_lock(&accounts_db_mutex[orig_id]);
	accounts_database[orig_id]->balance -= request_value->transfer.amount;
	pthread_mutex_unlock(&accounts_db_mutex[orig_id]);

	pthread_mutex_lock(&accounts_db_mutex[dest_id]);
	accounts_database[dest_id]->balance += request_value->transfer.amount;
	pthread_mutex_unlock(&accounts_db_mutex[dest_id]);
	return 0;
}

static int is_valid_create_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	int ret_code = RC_OK;
	uint32_t orig_id = request_value->header.account_id;
	uint32_t create_id = request_value->create.account_id;
	
	pthread_mutex_lock(&accounts_db_mutex[orig_id]);
	pthread_mutex_lock(&accounts_db_mutex[create_id]);

	/* Verify password */
	if(authenticate(request_value->header.password, accounts_database[orig_id]) != 0)
		ret_code = RC_LOGIN_FAIL;

	/* Testing if it is a admin request */
	else if(orig_id != ADMIN_ACCOUNT_ID)
		ret_code = RC_OP_NALLOW;

	/* Verify if account already exists */	
	else if(accounts_database[create_id]->account_id != EMPTY_ACCOUNT_ID)
		ret_code = RC_ID_IN_USE;

	/* Parameter verification done in user_parse */
	
	pthread_mutex_unlock(&accounts_db_mutex[create_id]);
	pthread_mutex_unlock(&accounts_db_mutex[orig_id]);

	return ret_code;
}

static int is_valid_transfer_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	int ret_code = RC_OK;
	uint32_t orig_id = request_value->header.account_id;
	uint32_t dest_id = request_value->transfer.account_id;

	pthread_mutex_lock(&accounts_db_mutex[orig_id]);
	pthread_mutex_lock(&accounts_db_mutex[dest_id]);

	/* Verify password */
	if(authenticate(request_value->header.password, accounts_database[orig_id]) != 0)
		ret_code = RC_LOGIN_FAIL;

	/* Return OP_NALLOW if the request is made by the admin */
	else if(orig_id == ADMIN_ACCOUNT_ID)
		ret_code = RC_OP_NALLOW;

	/* Verify if destination id exists */
	else if(accounts_database[dest_id]->account_id == EMPTY_ACCOUNT_ID)
		ret_code = RC_ID_NOT_FOUND;

	/* Verifies if both origin and destin account have the same id */
	else if(orig_id == dest_id)
		ret_code = RC_SAME_ID;
	
	/* Verifies if account has enough funds */
	else if(accounts_database[orig_id]->balance < request_value->transfer.amount)
		ret_code = RC_NO_FUNDS;

	/* Verify if the final amount is too large */
	else if(accounts_database[dest_id]->balance + request_value->transfer.amount > MAX_BALANCE)
		ret_code = RC_TOO_HIGH;

	pthread_mutex_unlock(&accounts_db_mutex[dest_id]);
	pthread_mutex_unlock(&accounts_db_mutex[orig_id]);

	return ret_code;
}

static int is_valid_balance_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	int ret_code = RC_OK;
	uint32_t orig_id = request_value->header.account_id;

	pthread_mutex_lock(&accounts_db_mutex[orig_id]);

	/* Verify password */
	if(authenticate(request_value->header.password, accounts_database[orig_id]) != 0)
		ret_code = RC_LOGIN_FAIL;

	/* Return OP_NALLOW if the request is made by the admin */
	else if(orig_id == ADMIN_ACCOUNT_ID)
		ret_code = RC_OP_NALLOW;

	pthread_mutex_unlock(&accounts_db_mutex[orig_id]);

	return ret_code;
}

static int is_valid_shutdown_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	int ret_code = RC_OK;
	uint32_t orig_id = request_value->header.account_id;

	pthread_mutex_lock(&accounts_db_mutex[orig_id]);

	/* Verify password */
	if(authenticate(request_value->header.password, accounts_database[orig_id]) != 0)
		ret_code = RC_LOGIN_FAIL;

	/* Return OP_NALLOW if the request is made by a client */
	else if(orig_id != ADMIN_ACCOUNT_ID)
		ret_code = RC_OP_NALLOW;

	pthread_mutex_unlock(&accounts_db_mutex[orig_id]);

	return ret_code;
}