#include "sync_log.h"
#include "sope.h"
#include "sync.h"

int syncLogRequest(int fd, int id, const tlv_request_t *request) {
    if(lock_log_mutex()!= 0){
        perror("lock_log_mutex: error locking log_mutex");
        return RC_OTHER; 
    }

    int ret = logRequest(fd, id, request);

    if(unlock_log_mutex()!=0){
        perror("unlock_log_mutex: error unlocking log_mutex");
        return RC_OTHER;   
    }

    return ret;
}

int syncLogReply(int fd, int id, const tlv_reply_t *request) {
    if(lock_log_mutex()!= 0){
        perror("lock_log_mutex: error locking log_mutex");
        return RC_OTHER; 
    }

    int ret = logReply(fd, id, request);

    if(unlock_log_mutex()!=0){
        perror("unlock_log_mutex: error unlocking log_mutex");
        return RC_OTHER;   
    }

    return ret;
}

int syncLogBankOfficeOpen(int fd, int id, pthread_t tid) {
    if(lock_log_mutex()!= 0){
        perror("lock_log_mutex: error locking log_mutex");
        return RC_OTHER; 
    }

    int ret = logBankOfficeOpen(fd, id, tid);

    if(unlock_log_mutex()!=0){
        perror("unlock_log_mutex: error unlocking log_mutex");
        return RC_OTHER;   
    }

    return ret;
}

int syncLogBankOfficeClose(int fd, int id, pthread_t tid) {
    if(lock_log_mutex()!= 0){
        perror("lock_log_mutex: error locking log_mutex");
        return RC_OTHER; 
    }

    int ret = logBankOfficeClose(fd, id, tid);

    if(unlock_log_mutex()!=0){
        perror("unlock_log_mutex: error unlocking log_mutex");
        return RC_OTHER;   
    }

    return ret;
}

int syncLogAccountCreation(int fd, int id, const bank_account_t *account) {
    if(lock_log_mutex()!= 0){
        perror("lock_log_mutex: error locking log_mutex");
        return RC_OTHER; 
    }

    int ret = logAccountCreation(fd, id, account);

    if(unlock_log_mutex()!=0){
        perror("unlock_log_mutex: error unlocking log_mutex");
        return RC_OTHER;   
    }

    return ret;
}

int syncLogSyncMech(int fd, int id, sync_mech_op_t smo, sync_role_t role, int sid) {
    if(lock_log_mutex()!= 0){
        perror("lock_log_mutex: error locking log_mutex");
        return RC_OTHER; 
    }

    int ret = logSyncMech(fd, id, smo, role, sid);

    if(unlock_log_mutex()!=0){
        perror("unlock_log_mutex: error unlocking log_mutex");
        return RC_OTHER;   
    }

    return ret;
}

int syncLogSyncMechSem(int fd, int id, sync_mech_op_t smo, sync_role_t role, int sid, int val) {
    if(lock_log_mutex()!= 0){
        perror("lock_log_mutex: error locking log_mutex");
        return RC_OTHER; 
    }

    int ret = logSyncMechSem(fd, id, smo, role, sid, val);

    if(unlock_log_mutex()!=0){
        perror("unlock_log_mutex: error unlocking log_mutex");
        return RC_OTHER;   
    }

    return ret;
}

int syncLogDelay(int fd, int id, uint32_t delay_ms) {
    if(lock_log_mutex()!= 0){
        perror("lock_log_mutex: error locking log_mutex");
        return RC_OTHER; 
    }

    int ret = logDelay(fd, id, delay_ms);

    if(unlock_log_mutex()!=0){
        perror("unlock_log_mutex: error unlocking log_mutex");
        return RC_OTHER;   
    }

    return ret;
}

int syncLogSyncDelay(int fd, int id, int sid, uint32_t delay_ms) {
    if(lock_log_mutex()!= 0){
        perror("lock_log_mutex: error locking log_mutex");
        return RC_OTHER; 
    }

    int ret = logSyncDelay(fd, id, sid, delay_ms);

    if(unlock_log_mutex()!=0){
        perror("unlock_log_mutex: error unlocking log_mutex");
        return RC_OTHER;   
    }

    return ret;
}