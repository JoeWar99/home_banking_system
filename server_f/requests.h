#pragma once

#include "../shared/types.h"

int is_valid_request(tlv_request_t * request, bank_account_t * accounts_database[]);

int create_request(const req_value_t * request_value, bank_account_t * accounts_database[]);

int transfer_request(const req_value_t * request_value, bank_account_t * accounts_database[]);