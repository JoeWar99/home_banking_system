#include "account_utilities.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "utilities.h"
#include "crypto.h"

int valid_account_id(int account_id){
	return account_id >= 0 && account_id < MAX_BANK_ACCOUNTS;
}

int valid_user_account_id(int account_id) {
	return valid_account_id(account_id) && account_id != 0;
}

int valid_pwd(char * pwd) {
    return strlen(pwd) >= MIN_PASSWORD_LEN && strlen(pwd) <= MAX_PASSWORD_LEN;
}

int authenticate(char * pwd, bank_account_t * bank_account){
	if (bank_account->account_id == EMPTY_ACCOUNT_ID) return 1;
	char * hash = (char*) malloc(strlen(bank_account->hash));
	gen_hash(pwd, bank_account->salt, hash);
	int comp = strcmp(hash, bank_account->hash);
	return comp;
}


int create_account(char * pwd, uint32_t account_id, uint32_t balance, bank_account_t * accounts_database[]){
	if(gen_salt(accounts_database[account_id]->salt, SALT_LEN + 1, SALT_LEN) != 0)
		return -1;
    if(gen_hash(pwd, accounts_database[account_id]->salt, accounts_database[account_id]->hash) != 0)
		return -2;
	accounts_database[account_id]->account_id = account_id;
	accounts_database[account_id]->balance = balance;
	return 0;
}