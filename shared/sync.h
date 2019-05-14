#pragma once

#include "types.h"
#include <sys/types.h>
#include <stdint.h>

int lock_log_mutex();

int unlock_log_mutex();

int init_sync(uint32_t n_threads);

int del_sync();

int get_value_sem_empty(int * result);

int wait_sem_empty(pid_t sid);

int post_sem_empty(int balcony_id, pid_t sid);

int get_value_sem_full(int * result);

int wait_sem_full(int balcony_id);

int post_sem_full(pid_t sid);

int lock_queue_mutex(int balcony_id, sync_role_t role, pid_t sid);

int unlock_queue_mutex(int balcony_id, sync_role_t role, pid_t sid);

int lock_accounts_db_mutex(uint32_t muttex_id);

int unlock_accounts_db_mutex(uint32_t muttex_id);