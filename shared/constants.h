#pragma once

#define MAX_BANK_OFFICES 99
#define MAX_BANK_ACCOUNTS 4096
#define MIN_BALANCE 1UL
#define MAX_BALANCE 1000000000UL
#define ADMIN_ACCOUNT_ID 0
#define MAIN_THREAD_ID 0

#define MIN_PASSWORD_LEN 8
#define MAX_PASSWORD_LEN 20
#define HASH_LEN 64
#define SALT_LEN 64

#define MAX_OP_DELAY_MS 99999

#define WIDTH_ID 5
#define WIDTH_DELAY 5
#define WIDTH_ACCOUNT 4
#define WIDTH_BALANCE 10
#define WIDTH_OP 8
#define WIDTH_RC 12
#define WIDTH_HASH 5
#define WIDTH_TLV_LEN 3
#define WIDTH_STARTEND 5

#define SERVER_LOGFILE "slog.txt"
#define USER_LOGFILE "ulog.txt"

#define SERVER_FIFO_PATH "/tmp/secure_srv"
#define USER_FIFO_PATH_PREFIX "/tmp/secure_"
#define USER_FIFO_PATH_LEN (sizeof(USER_FIFO_PATH_PREFIX) + WIDTH_ID + 1)


/*#define OK 1               // operação bem-sucedida;
#define SRV_DOWN  2        // não é possível abrir o FIFO /tmp/secure_srv (servidor indisponível);
#define SRV_TIMEOUT 3      // o pedido expirou (a resposta não foi recebida dentro do tempo limite);
#define USR_DOWN 4         //não é possível abrir o FIFO /tmp/secure_XXXXX (utilizador indisponível);
#define LOGIN_FAIL 5       //a autenticação falhou porque o par (ID da conta, senha) não é válido;
#define OP_NALLOW 6        //a operação não é permitida ao utilizador (cliente ou administrador);
#define ID_IN_USE  7       // o ID da conta já está em uso (corresponde a uma conta já criada);
#define ID_NOT_FOUND 8     // a conta com o ID especificado não existe;
#define SAME_ID 9          // as contas de origem e de destino são a mesma;
#define NO_FUNDS 10        // a operação não pode ser realizada porque o saldo corrente é insuficiente;
#define TOO_HIGH 11        // a operação não pode ser realizada porque o saldo final é demasiado elevado;
#define OTHER 12           //qualquer outro erro que não tenha sido especificado
*/
#define FIFO_TIMEOUT_SECS 30
