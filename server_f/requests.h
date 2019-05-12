#pragma once

#include "../shared/types.h"

int is_valid_request(tlv_request_t * request, bank_account_t * accounts_database[]);

int create_request(const req_value_t * request_value, bank_account_t * accounts_database[], int id);

int transfer_request(const req_value_t * request_value, bank_account_t * accounts_database[], uint32_t * final_balance, int id);

int balance_request(const req_value_t * request_value, bank_account_t * accounts_database[], uint32_t * final_balance, int id);