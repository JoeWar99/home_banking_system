#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "constants.h"
#include "utilities.h"

void concat(char *str1[], char *str2, size_t n)
{
	const size_t len1 = strlen(*str1);
	const size_t len2 = n < strlen(str2) ? n : strlen(str2);
	*str1 = (char *)realloc(*str1, len1 + len2 + 1); // +1 for the null-terminator

	/* Check for errors */
	if (*str1 == NULL)
	{
		perror("realloc");
		exit(1);
	}

	/* Concatenate str2 */
	strncat(*str1, str2, len2);
}

int min(int a, int b) {
    return a < b ? a : b;
}

int max(int a, int b) {
    return a > b ? a : b;
}

long unsigned int str_to_lu(char * str) {
	char *ptr1;
	long unsigned int balance = strtoul(str, &ptr1, 10);
	if (strlen(ptr1) > 0) {
		fprintf(stderr, "Invalid long unsigned number\n");
		return INVALID_BALANCE;
	}
	return balance;
}

int str_to_int(char * str) {
	char *ptr1;
	int balance = strtol(str, &ptr1, 10);
	if (strlen(ptr1) > 0) {
		fprintf(stderr, "Invalid int number\n");
		return -1;
	}
	return balance;
}