#pragma once

#include "types.h"

int valid_pwd(char * pwd);

int valid_user_account_id(int account_id);

int valid_account_id(int account_id);

int authenticate(const char * pwd, bank_account_t * bank_account);

int create_account(const char * pwd, uint32_t account_id, uint32_t balance, bank_account_t * accounts_database[]);