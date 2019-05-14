#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <unistd.h>
#include <sys/types.h>

#define READ	0
#define WRITE	1

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

	char pass_salt[ MAX_PASSWORD_LEN + SALT_LEN + 1];
    pass_salt[0] = '\0';
    
    strcat(pass_salt, pwd);
    strcat(pass_salt, salt);

	int fd1[2];
	int fd2[2];

	pid_t pid;

	if(pipe(fd1) != 0){
		perror("pipe1");
		return -1;
	}
	if(pipe(fd2) != 0){
		perror("pipe2");
		return -1;
	}

	pid = fork();

	if(pid == -1){
		perror("fork");
		return -3;
	}
	else if(pid > 0){ // parent
		/* Close unnecessary descriptors */
		if (close(fd1[READ]) == -1) {
			perror("close fd1 read");
			return -1;
		}
		if (close(fd2[WRITE]) == -1) {
			perror("close fd2 write");
			return -1;
		}

		/* Write input for sha256sum */
		if (write(fd1[WRITE], pass_salt, strlen(pass_salt)) == -1) {
			perror("parent write");
			return -1;
		}

		if (close(fd1[WRITE]) == -1) {
			perror("close fd1 write");
			return -1;
		}

		/* Read output from sha256sum */
		if (read(fd2[READ], hash, HASH_LEN + 1) == -1) {
			perror("parent read");
			return -1;
		}
		
		if (close(fd2[READ]) == -1) {
			perror("close fd2 read");
			return -1;
		}

		hash[HASH_LEN] = '\0';

	}
	else{
		/* Close unnecessary descriptors */
		if (close(fd1[WRITE]) == -1) {
			perror("close fd1 write");
			return -1;
		}
		if (close(fd2[READ]) == -1) {
			perror("close fd2 read");
			return -1;
		}

		dup2(fd1[READ], STDIN_FILENO);
		dup2(fd2[WRITE], STDOUT_FILENO);
		execlp("sha256sum", "sha256sum", NULL);
	}

    return 0;   
}