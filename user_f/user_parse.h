#pragma once

int valid_args(int account_id, char * account_pwd, int delay, int operation);

int parse_req_args(char * req_args_string, char * req_args[3], int * req_arg_count);

int valid_req_args(int operation, char ** req_args, int req_arg_count);