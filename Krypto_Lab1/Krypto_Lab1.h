typedef unsigned short clint;
typedef clint CLINT[CLINTMAXDIGIT + 1];

void squareAndMultiply(CLINT base, CLINT exponent, CLINT n, CLINT *result, int keyLength);
void chineseReminderTheorem(CLINT primeNumber_P, CLINT primeNumber_q, CLINT message, CLINT privateKey_d, CLINT publicKey_n, CLINT *sig, int keyLength);
void startRSAstraightForward(char *primeNumber_P_dec, char *primeNumber_q_dec, char *publicKey_e_dec, char *privateKey_d_dec, int keyLength);
void startRSAchineseRemainderTheorem(char *primeNumber_P_dec, char *primeNumber_q_dec, char *publicKey_e_dec, char *privateKey_d_dec, int keyLength);
