#pragma once

int gen_random_int();

int gen_salt(char * buf, int buf_size, int salt_size);

int gen_hash(char * pwd, char * salt, char * hash);