#pragma once

#include "../shared/types.h"

int init_reply(tlv_reply_t * reply, tlv_request_t * request, int ret, bank_account_t * accounts_database[]);

int init_reply_fifo_name(char * fifo_name, pid_t pid);