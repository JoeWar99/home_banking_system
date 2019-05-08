#pragma once

#include "types.h"

int valid_pwd(char * pwd);

int valid_user_account_id(int account_id);

int valid_account_id(int account_id);

int authenticate(char * pwd, bank_account_t * bank_account);


int account_balance(int account_id, bank_account_t *accounts_database[], int tamanho);

int account_database_position(int account_id, bank_account_t *accounts_database[], int tamanho);