#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "utilities.h"



int main(){

    int secure_svr;

    if (mkfifo("/tmp/secure_svr", 0660) < 0 ) {
        if(errno!=EEXIST){
            printf("ERRO - could not create fifo /tmp/secure_svr\n");
            exit(1);
        } 
        
    }

    if ((secure_svr = open("/tmp/secure_svr", O_RDWR)) == -1) {
        perror("open");
        exit(1);
    }


    
    close(secure_svr);
    unlink("/tmp/secure_svr");

    return 0;
}





