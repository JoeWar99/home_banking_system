#pragma once

#include <pthread.h>
#include "types.h"

int syncLogRequest(int fd, int id, const tlv_request_t *request);

int syncLogReply(int fd, int id, const tlv_reply_t *request);

int syncLogBankOfficeOpen(int fd, int id, pthread_t tid);

int syncLogBankOfficeClose(int fd, int id, pthread_t tid);

int syncLogAccountCreation(int fd, int id, const bank_account_t *account);

int syncLogSyncMech(int fd, int id, sync_mech_op_t smo, sync_role_t role, int sid);

int syncLogSyncMechSem(int fd, int id, sync_mech_op_t smo, sync_role_t role, int sid, int val);

int syncLogDelay(int fd, int id, uint32_t delay_ms);

int syncLogSyncDelay(int fd, int id, int sid, uint32_t delay_ms);