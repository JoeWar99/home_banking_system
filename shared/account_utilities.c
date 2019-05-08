#include "account_utilities.h"

#include <string.h>


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
	char hash[strlen(bank_account->hash)];
	gen_hash(pwd, bank_account->salt, hash);
	return strcmp(hash, bank_account->hash);
}

/*int account_balance(int account_id, bank_account_t *accounts_database, int tamanho){

	for(int i = 0; i< tamanho;i++){
		if(account_id == accounts_database[i].account_id){
			return accounts_database[i].balance;
		}
	}
}

int account_database_position(int account_id, bank_account_t *accounts_database, int tamanho){
	for(int i = 0; i< tamanho;i++){
		if(account_id == accounts_database[i].account_id){
			return i;
		}
	}
}*/

