#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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