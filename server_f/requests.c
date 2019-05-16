#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "requests.h"
#include "sync.h"
#include "sync_log.h"
#include "../shared/constants.h"
#include "../shared/utilities.h"
#include "../shared/account_utilities.h"
#include "../shared/com_protocol.h"

static int is_valid_create_request(const req_value_t * request_value, bank_account_t * accounts_database[]);

static int is_valid_transfer_request(const req_value_t * request_value, bank_account_t * accounts_database[]);

static int is_valid_balance_request(const req_value_t * request_value, bank_account_t * accounts_database[]);

static int is_valid_shutdown_request(const req_value_t * request_value, bank_account_t * accounts_database[]);

int create_request(const req_value_t * request_value, bank_account_t * accounts_database[], int id){
	int ret_code = RC_OK;
	uint32_t create_id = request_value->create.account_id;

	if(lock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, create_id) != 0)
		return RC_OTHER;
	
	usleep(request_value->header.op_delay_ms * 1000);
	
	if (syncLogSyncDelay(STDOUT_FILENO, id, create_id, request_value->header.op_delay_ms) < 0)
		fprintf(stderr, "syncLogSyncDelay error\n");

	if((ret_code = is_valid_create_request(request_value, accounts_database)) == RC_OK)
		if((ret_code = create_account(request_value->create.password, create_id, request_value->create.balance, accounts_database)) != RC_OK)
			fprintf(stderr, "create_account: error creating the account\n");

	if(unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, create_id) != 0)
		return RC_OTHER;

	return ret_code;
}

int transfer_request(const req_value_t * request_value, bank_account_t * accounts_database[], int id, uint32_t * final_balance){
	uint32_t orig_id = request_value->header.account_id;
	uint32_t dest_id = request_value->transfer.account_id;
	int first = min(orig_id, dest_id);
	int second = max(orig_id, dest_id);
	int ret_code = RC_OK;

	if(lock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, first) != 0)
		return RC_OTHER;
	
	usleep(request_value->header.op_delay_ms * 1000);

	if (syncLogSyncDelay(STDOUT_FILENO, id, orig_id, request_value->header.op_delay_ms) < 0)
		fprintf(stderr, "syncLogSyncDelay error\n");
	
	/* If the IDs are the same */
	if(first == second){
		/* Verify password - if it is wrong it must not give access to the balance */
		if(authenticate(request_value->header.password, accounts_database[orig_id]) != 0){
			if(unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, first) != 0)
				return RC_OTHER;
			
			return RC_LOGIN_FAIL;
		}

		/* Even though there was an error, return balance before operation */
		*final_balance = accounts_database[orig_id]->balance;

		/* Will return in the SAME_ID in the worst case */
		ret_code = is_valid_transfer_request(request_value, accounts_database);

		if(unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, first) != 0)
			return RC_OTHER;
		
		return ret_code;
	}
	
	if(lock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, second) != 0){
		unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, first);
		return RC_OTHER;
	}

	usleep(request_value->header.op_delay_ms * 1000);

	if (syncLogSyncDelay(STDOUT_FILENO, id, dest_id, request_value->header.op_delay_ms) < 0)
		fprintf(stderr, "syncLogSyncDelay error\n");
	
	/* Verify password - if it is wrong it must not give access to the balance */
	if(authenticate(request_value->header.password, accounts_database[orig_id]) != 0){
	
		if(unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, second) != 0){
			unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, first);
			return RC_OTHER;
		}

		if(unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, first) != 0)
			return RC_OTHER;
		
		return RC_LOGIN_FAIL;
	}

	if((ret_code = is_valid_transfer_request(request_value, accounts_database)) == RC_OK){
		accounts_database[orig_id]->balance -= request_value->transfer.amount;
		accounts_database[dest_id]->balance += request_value->transfer.amount;
	}

	/* Even though there was an error, return balance before operation */
	*final_balance = accounts_database[orig_id]->balance;

	if(unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, second) != 0){
		unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, first);
		return RC_OTHER;
	}

	if(unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, first) != 0)
		return RC_OTHER;

	return ret_code;
}

int balance_request(const req_value_t * request_value, bank_account_t * accounts_database[], int id, uint32_t * final_balance){
	uint32_t orig_id = request_value->header.account_id;
	int ret_code;

	if(lock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, orig_id) != 0)
		return RC_OTHER;

	usleep(request_value->header.op_delay_ms * 1000);

	if (syncLogSyncDelay(STDOUT_FILENO, id, orig_id, request_value->header.op_delay_ms) < 0)
		fprintf(stderr, "syncLogSyncDelay error\n");
	
	if((ret_code = is_valid_balance_request(request_value, accounts_database)) == RC_OK)	
		*final_balance = accounts_database[orig_id]->balance;

	if(unlock_accounts_db_mutex(id, SYNC_ROLE_ACCOUNT, orig_id) != 0)
		return RC_OTHER;

	return ret_code;
}

int shutdown_request(const req_value_t * request_value, bank_account_t * accounts_database[], int id, int * balcony_open, int secure_srv, int dummy_descriptor) {
	int ret;

	usleep(request_value->header.op_delay_ms * 1000);

	if (syncLogDelay(STDOUT_FILENO, id, request_value->header.op_delay_ms) < 0) 
		fprintf(stderr, "syncLogDelay error\n");

	if((ret = is_valid_shutdown_request(request_value, accounts_database)) != RC_OK)
		return ret;

	/* No more requests allowed */
	*balcony_open = 0;

	/* Close dummy connection in write mode, to close reading FIFO */
	if (close(dummy_descriptor) != 0)
    {
        perror("close: error closing down server fifo");
        return RC_OTHER;
    }

	/* Change server permissions to read only */
	if (fchmod(secure_srv, 0444) != 0)
	{
		perror("fchmod: error altering server fifo permissions");
		return RC_OTHER;
	}

	return RC_OK;
}

static int is_valid_create_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	uint32_t orig_id = request_value->header.account_id;
	uint32_t create_id = request_value->create.account_id;
	
	/* Verify password */
	if(authenticate(request_value->header.password, accounts_database[orig_id]) != 0)
		return RC_LOGIN_FAIL;

	/* Testing if it is a admin request */
	if(orig_id != ADMIN_ACCOUNT_ID)
		return RC_OP_NALLOW;

	/* Verify if account already exists */	
	if(accounts_database[create_id]->account_id != EMPTY_ACCOUNT_ID)
		return RC_ID_IN_USE;

	/* Parameter verification done in user_parse */
	
	return RC_OK;
}

static int is_valid_transfer_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	uint32_t orig_id = request_value->header.account_id;
	uint32_t dest_id = request_value->transfer.account_id;
		
	/* Verify password - already done */

	/* Return OP_NALLOW if the request is made by the admin */
	if(orig_id == ADMIN_ACCOUNT_ID)
		return RC_OP_NALLOW;

	/* Verify if destination id exists */
	if(accounts_database[dest_id]->account_id == EMPTY_ACCOUNT_ID)
		return RC_ID_NOT_FOUND;

	/* Verifies if both origin and destin account have the same id */
	if(orig_id == dest_id)
		return RC_SAME_ID;
	
	/* Verifies if account has enough funds */
	if(accounts_database[orig_id]->balance < request_value->transfer.amount)
		return RC_NO_FUNDS;

	/* Verify if the final amount is too large */
	if(accounts_database[dest_id]->balance + request_value->transfer.amount > MAX_BALANCE)
		return RC_TOO_HIGH;

	return RC_OK;
}

static int is_valid_balance_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	uint32_t orig_id = request_value->header.account_id;

	/* Verify password */
	if(authenticate(request_value->header.password, accounts_database[orig_id]) != 0)
		return RC_LOGIN_FAIL;

	/* Return OP_NALLOW if the request is made by the admin */
	if(orig_id == ADMIN_ACCOUNT_ID)
		return RC_OP_NALLOW;

	return RC_OK;
}

static int is_valid_shutdown_request(const req_value_t * request_value, bank_account_t * accounts_database[]){
	uint32_t orig_id = request_value->header.account_id;

	/* Verify password */
	if(authenticate(request_value->header.password, accounts_database[orig_id]) != 0)
		return RC_LOGIN_FAIL;

	/* Return OP_NALLOW if the request is made by a client */
	if(orig_id != ADMIN_ACCOUNT_ID)
		return RC_OP_NALLOW;

	return RC_OK;
}