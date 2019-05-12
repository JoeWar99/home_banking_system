#pragma once

#include "../shared/types.h"

void init_request(tlv_request_t * full_request, int operation, int pid, int account_id, char * pwd, int op_delay, char ** req_args);

int init_reply(tlv_reply_t * reply, tlv_request_t * request, int ret, int n_threads, uint32_t balance);

int init_secure_fifo_name(char * fifo_name, pid_t pid);

int write_reply(int fd, tlv_reply_t * request_reply);

int read_reply(int fd, tlv_reply_t * request_reply);

int write_request(int fd, tlv_request_t * request);

tlv_request_t * read_request(int fd);