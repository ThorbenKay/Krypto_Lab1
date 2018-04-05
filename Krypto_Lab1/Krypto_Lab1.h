
//CLINT squareAndMultiply(CLINT base_X, CLINT exponent_k);

bool startRSAstraightForward(char sendedMessage, int keyLength);

void keyGeneration(int keyLength, CLINT privateKey_d, CLINT publicKey_e, CLINT publicKey_n);
void encrypt(CLINT chiffre, char sendedMessage, CLINT publicKey_e, CLINT publicKey_n);
void decrypt(char recievedMessage, CLINT chiffre, CLINT publicKey_e, CLINT publicKey_n, CLINT privateKey_d);