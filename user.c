#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "utilities.h"

int main(){

    int pid  = getpid();    
    char string_pid[64];
    int secure_svr;
    
    sprintf(string_pid, "%d", pid);
    

    if ((secure_svr = open("/tmp/secure_svr", O_WRONLY)) == -1) {
        perror("open: server is not working 404 error");
        exit(1);
    }


    close(secure_svr);
    return(0);

}

