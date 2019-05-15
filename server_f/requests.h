#pragma once

#include "../shared/types.h"

int create_request(const req_value_t * request_value, bank_account_t * accounts_database[], int id);

int transfer_request(const req_value_t * request_value, bank_account_t * accounts_database[], int id, uint32_t * final_balance);

int balance_request(const req_value_t * request_value, bank_account_t * accounts_database[], int id, uint32_t * final_balance);

int shutdown_request(const req_value_t * request_value, bank_account_t * accounts_database[], int id, int * balcony_open, int secure_srv, int dummy_desc);