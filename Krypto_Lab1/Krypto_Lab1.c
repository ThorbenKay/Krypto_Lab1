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

	bool isAlgorithmRSAstraightForwardCorrect = false;
	char sendedMessage[] = "123456789";
 	int keylength = 512; // e.g 512, 1024 Bits
	
	printf("\n\n THE RSA STRAIGHT FORWARD =\n\n");
	printf(" 1. Denfined keylength \t\t : %i\n", keylength);

	isAlgorithmRSAstraightForwardCorrect = startRSAstraightForward(&sendedMessage, keylength);
	printf("\n______________________________________________________________\n\n");

	printf("\n\n THE RSA with CHINES REMINDER THEOREM =\n\n");
	printf(" 1. Denfined keylength \t\t : %i\n", keylength);
	// isAlgorithmRSA_CRT = do the same for RSA plus CRT (when it comes to mod calcs.)
	
	//printf("Check CRT Squareding\n");
	//squareAndMultiply(baseTest, exponentTest);


	return 0;
}

//CLINT squareAndMultiply(CLINT base_X , CLINT exponent_k ) {
//	CLINT result;
//	int exponentBinary;
//	//clint2byte_l(exponent_k, exponentBinary);
//
//	return result;
//}


bool startRSAstraightForward(char *sendedMessage, int keyLength) {
	
	CLINT privateKey_d, publicKey_e, publicKey_n, chiffre;
	char recievedMessage[] = "--------------------";

	// Bob acting:
	keyGeneration(keyLength, &privateKey_d, &publicKey_e, &publicKey_n); // & damit Werte "überschrieben werden können"
	printf(" 2. Key Generation by Bob\n");
	printf("	Private Key d \t\t : %s\n", xclint2str_l(privateKey_d, 10, 0));
	printf("	Public  Key e \t\t : %s\n", xclint2str_l(publicKey_e, 10, 0));
	printf("	Public  Key n \t\t : %s\n", xclint2str_l(publicKey_n, 10, 0));

	// Alice acting:

	encrypt(&chiffre, &sendedMessage, publicKey_e, publicKey_n);
	printf(" 3. Encryption by ALice:\n");
	printf("	Message m  \t\t : %s\n", sendedMessage);
	printf("	chiffre c  \t\t : %s\n\n", xclint2str_l(chiffre, 10, 0));
	printf("	--> sends chiffre to Bob\n\n");

	// Bob acting:
	decrypt(recievedMessage, chiffre, publicKey_e, publicKey_n, privateKey_d);
	printf(" 4. Bob decrypts recieved Message:\n");
	printf("	decrypted message \t : %s (should be 123456789) \n", recievedMessage);

}

void keyGeneration(int keyLength, CLINT *privateKey_d, CLINT *publicKey_e, CLINT *publicKey_n) { //  *damit Werte "überschrieben werden können"
	
	CLINT seed, primeNumber_P, primeNumber_q, subtrahend, tempFactor_1, tempFactor_2, eulerPHI;

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
	
	// show decimal Value of CLINT variable 
	printf("	Prime number P  \t : %s\n", xclint2str_l(primeNumber_P, 10, 0));
	printf("	Prime number q  \t : %s\n", xclint2str_l(primeNumber_q, 10, 0));


	//*publicKey_n = P mal Q (512 mal 512 ergibt 1024?)
	mul_l(primeNumber_P, primeNumber_q, publicKey_n);

	// euler_PHI = (primeNumber_P-1) mal (primeNumber_Q-1);
	// initialize CLINT variable
	str2clint_l(subtrahend, subtrahend_dec, baseOfNumbers);
	sub_l(primeNumber_P, subtrahend, tempFactor_1);
	//printf("	temp factor \t :%s\n", xclint2str_l(tempFactor_1, 10, 0)); //is correct !
	sub_l(primeNumber_q, subtrahend, tempFactor_2);

	mul_l(tempFactor_1, tempFactor_2, eulerPHI);
	//printf("	eulerPHI \t :%s\n", xclint2str_l(eulerPHI, 10, 0)); //is correct !



	// *publicKey_e teilerfremd zu Euler_PHI finden (also GGT = 1) used 65537
	str2clint_l(publicKey_e, publicKey_e_dec, baseOfNumbers);

	// *privateKey_d initialize =
	str2clint_l(privateKey_d, privateKey_d_dec, baseOfNumbers);
	// *publicKey_e mal *privateKey_d MOD euler_PHI = 1



}

void encrypt(CLINT *chiffre, char *sendedMessage, CLINT publicKey_e, CLINT publicKey_n) {
	// to be filled
	// use squareAndMultiply()
	// result should be (according to Homepage):
	char chiffre_dec[] = "2948701374227544971298766725833993016114019032452990736886112158895628361146";
	str2clint_l(chiffre, chiffre_dec, 10);
}

void decrypt(char recievedMessage, CLINT chiffre, CLINT publicKey_e, CLINT publicKey_n, CLINT privateKey_d) {
	// to be filled
	// use squareAndMultiply()
	// result should be (according to Homepage):
	// recievedMessage = "123456789";
}