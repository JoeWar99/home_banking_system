#include "requests.h"
#include "../shared/constants.h"

static int is_valid_create_request(req_value_t request_value, bank_account_t * accounts_database[]);

static int is_valid_transfer_request(req_value_t request_value, bank_account_t * accounts_database[]);

static int is_valid_balance_request(req_value_t request_value);

static int is_valid_shutdown_request(req_value_t request_value);

int is_valid_request(tlv_request_t * request, bank_account_t * accounts_database[]){
	switch (request->type)
	{
		case OP_CREATE_ACCOUNT:
			return is_valid_create_request(request->value, accounts_database);
		case OP_TRANSFER:
			return is_valid_transfer_request(request->value, accounts_database);
		case OP_BALANCE:
			return is_valid_balance_request(request->value);
		case OP_SHUTDOWN:
			return is_valid_shutdown_request(request->value);
	}
}

static int is_valid_create_request(req_value_t request_value, bank_account_t * accounts_database[]){
	// TODO: escolher se primeiro testar login correto ou op_noallowed
	// TODO: Testar login

	/* Testing if it is a admin request */
	if(request_value.header.account_id != 0)
		return OP_NALLOW;

	/* Verify if account already exists */	
	if(accounts_database[request_value.create.account_id] != NULL)
		return ID_IN_USE;

	/* Parameter verification done in user_parse */
	
	return OK;
}

static int is_valid_transfer_request(req_value_t request_value, bank_account_t * accounts_database[]){
	// TODO: Testar login

	/* Return OP_NALLOW if the request is made by the admin */
	if(request_value.header.account_id == 0)
		return OP_NALLOW;

	/* Verify if destination id exists */
	if(accounts_database[request_value.transfer.account_id] == NULL)
		return ID_NOT_FOUND;

	/* Verifies if both origin and destin account have the same id */
	if(request_value.header.account_id == request_value.transfer.account_id)
		return SAME_ID;
	
	/* Verifies if account has enough funds */
	if(accounts_database[request_value.header.account_id]->balance < request_value.transfer.amount)
		return NO_FUNDS;

	/* Verify if the final amount is too large */
	if(accounts_database[request_value.transfer.account_id]->balance + request_value.transfer.amount > MAX_BALANCE)
		return TOO_HIGH;

	return OK;
}

static int is_valid_balance_request(req_value_t request_value){
	// TODO: Testar login

	/* Return OP_NALLOW if the request is made by the admin */
	if(request_value.header.account_id == 0)
		return OP_NALLOW;

	return OK;
}

static int is_valid_shutdown_request(req_value_t request_value){
	// TODO: Testar login

	/* Return OP_NALLOW if the request is made a client */
	if(request_value.header.account_id != 0)
		return OP_NALLOW;

	return OK;
}