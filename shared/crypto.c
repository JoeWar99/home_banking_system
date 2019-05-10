#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "crypto.h"
#include "constants.h"

int gen_random_int() {
    int val = rand() % 0xff;
    val |= (rand() % 0xff) << 8;
    val |= (rand() % 0xff) << 16;
    val |= (rand() % 0xff) << 24;
    return val;
}

int gen_salt(char * buf, int buf_size, int salt_size) {

    if (salt_size >= buf_size) return -1;

    int iters = salt_size / sizeof(int) - 1;
    
    for (int i = 0; i < iters; i++) {
        sprintf(&buf[i*sizeof(int)], "%x", gen_random_int());
    }
    buf[buf_size] = '\0';
    return 0;
}

int gen_hash(const char * pwd, char * salt, char * hash) {
    FILE * fp;
    char full_cmd[ MAX_PASSWORD_LEN + SALT_LEN + 20 + 1];
    full_cmd[0] = '\0';
    
    strcat(full_cmd, "echo -n ");
    strcat(full_cmd, pwd);
    strcat(full_cmd, salt);
    strcat(full_cmd, " | sha256sum");

	if ((fp = popen(full_cmd, "r")) == NULL)
	{
		fprintf(stderr, "Error opening file pipe!\n");
		return -1;
	}

    fgets(hash, HASH_LEN+1, fp);

    if (pclose(fp)) {
        perror("pclose");
        return -1;
    }
/* TODO handle slow call interruption ??? */
/*
    int ret = pclose(fp);
	if (ret)
	{
		if (ret == -1)
		{
			perror("pclose");
			return -1;
		}
		else {
			return PIPE_CMD_ERR;
		}
	}
*/
    return 0;   
}