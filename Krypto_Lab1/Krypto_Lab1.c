// Krypto_Lab1.c 
// Thorben Kay & Dominic Dietl
//

#include "flint.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "Krypto_Lab1.h"


int main(int argc, char* argv[]) {

	printf("--------------------------------------------------------------\n");
	printf("--------------------------------------------------------------\n");
	printf("--------------------THE GREAT RSA MACHINE---------------------\n");
	printf("-----------------------by Kay and Dietl-----------------------\n");
	printf("--------------------------------------------------------------\n\n");

 	int keylength = 512; // e.g 512, 1024 Bits
	
	printf("\n\n THE RSA STRAIGHT FORWARD =\n\n");
	printf(" 1. Denfined keylength \t\t : %i\n", keylength);

	CLINT privateKey_d, publicKey_e, publicKey_n, chiffre, encryptedMessage;





	// Bob acting:

	printf("\n 2. Key Generation by Bob\n");
	CLINT primeNumber_P, primeNumber_q, subtrahend, tempFactor_1, tempFactor_2, eulerPHI;

	// Numbers and Keys from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	char primeNumber_P_dec[] = "280150602340161058630637143798855285723"; // variable adjustable
	char primeNumber_q_dec[] = "184336280682452596243149253821987903701"; // variable adjustable
	char publicKey_e_dec[] = "65537"; // no further restirictions often =3 ; even more often 65537 (F4)
	char privateKey_d_dec[] = "23603187110593578184937184241008316352076378834332071822245298133461559552273";
	char subtrahend_dec[] = "1";
	char tempFactor_1_dec[] = "1";
	char tempFactor_2_dec[] = "1";
	int baseOfNumbers = 10;

	// initialize CLINT variable
	str2clint_l(primeNumber_P, primeNumber_P_dec, baseOfNumbers);
	str2clint_l(primeNumber_q, primeNumber_q_dec, baseOfNumbers);

	printf("	Prime number P  \t : %s\n", xclint2str_l(primeNumber_P, 10, 0));
	printf("	Prime number q  \t : %s\n", xclint2str_l(primeNumber_q, 10, 0));

	mul_l(primeNumber_P, primeNumber_q, publicKey_n);

	// initialize CLINT variable
	str2clint_l(subtrahend, subtrahend_dec, baseOfNumbers);
	sub_l(primeNumber_P, subtrahend, tempFactor_1);

	sub_l(primeNumber_q, subtrahend, tempFactor_2);

	mul_l(tempFactor_1, tempFactor_2, eulerPHI);

	str2clint_l(publicKey_e, publicKey_e_dec, baseOfNumbers);

	// *privateKey_d initialize =
	str2clint_l(privateKey_d, privateKey_d_dec, baseOfNumbers);
	// *publicKey_e mal *privateKey_d MOD euler_PHI = 1

	printf("	Private Key d \t\t : %s\n", xclint2str_l(privateKey_d, 10, 0));
	printf("	Public  Key e \t\t : %s\n", xclint2str_l(publicKey_e, 10, 0));
	printf("	Public  Key n \t\t : %s\n", xclint2str_l(publicKey_n, 10, 0));
	printf("	Euler PHI     \t\t : %s\n", xclint2str_l(eulerPHI, 10, 0));








	// Alice acting:

	printf("\n3. Encryption by ALice:\n");

	CLINT message;
	char message_dec[] = "1657089733347464174213";
	printf("	Message m  \t\t : %s\n", message_dec);
	str2clint_l(message, message_dec, 10);

	squareAndMultiply(message, publicKey_e, publicKey_n, &chiffre);

	printf("	Chiffre  \t\t : %s\n", xclint2str_l(chiffre, 10, 0));


	squareAndMultiply(message, publicKey_e, publicKey_n, &chiffre);
	
	printf("	--> sends chiffre to Bob\n\n");






	// Bob acting:

	printf(" 4. Bob decrypts recieved Message:\n");

	squareAndMultiply(chiffre, privateKey_d, publicKey_n, encryptedMessage);
	printf("	decrypted message \t : %s (should be %s) \n\n", xclint2str_l(encryptedMessage, 10, 0), xclint2str_l(message, 10, 0));

	if (xclint2str_l(encryptedMessage, 10, 0) == xclint2str_l(message, 10, 0)) {
		printf("---> Process Sucessful !!");
	}
	else(printf("---> Process failed !!"));

	printf("\n______________________________________________________________\n\n");

	/*printf("\n\n THE RSA with CHINES REMINDER THEOREM =\n\n");
	printf(" 1. Denfined keylength \t\t : %i\n", keylength);*/
	// isAlgorithmRSA_CRT = do the same for RSA plus CRT (when it comes to mod calcs.)
	
	//printf("Check CRT Squareding\n");

	//printf("	Result n \t\t : %s\n", exponentTest_str);


	return 0;
}


void squareAndMultiply(CLINT base, CLINT exponent, CLINT n, CLINT *result) {

	//Exponent in Binär
	char *exponent_bin = xclint2str_l(exponent, 2, 0);

	char res[] = "1";
	//CLINT res_l;
	int i = 0;
	str2clint_l(*result, res, 10);

	while (exponent_bin[i] != 0) {
		mmul_l(*result, *result, *result, n);

		if (exponent_bin[i] == '1') {
			mmul_l(*result, base, *result, n);
		}

		i++;
	}
}
	
