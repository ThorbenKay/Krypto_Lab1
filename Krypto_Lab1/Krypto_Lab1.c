// Krypto_Lab1.c : Defines the entry point for the console application.
//
#include "flint.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "Krypto_Lab1.h"


int main(int argc, char* argv[]) {

	/////////////// First Tests //////////////////
	//char input[] = "1010", dummy[] = "Test_String";
	//USHORT base = 2;
	//int test;

	//str2clint_l(fertigerClint, &input, base);
	//xclint2str_l(fertigerClint, base, NULL);

	//printf("Clint Variable oi hat den Wert : %s \n", dummy);

	// TEST !!!!!!!

	/////////////// RSA Starting //////////////////
	printf("--------------------------------------------------------------\n");
	printf("--------------------------------------------------------------\n");
	printf("--------------------THE GREAT RSA MACHINE---------------------\n");
	printf("-----------------------by Kay and Dietl-----------------------\n");
	printf("--------------------------------------------------------------\n\n");

	bool isAlgorithmRSAstraightForwardCorrect = false;
	char sendedMessage[] = "Ich bin ein exemplarischer Inhalt";
	char recievedMessage[] = "0";
 	int keylength = 512; // e.g 512, 1024 Bits
	printf(" 1. Denfined keylength \t\t : %i\n", keylength);

	isAlgorithmRSAstraightForwardCorrect = startRSAstraightForward(sendedMessage, recievedMessage, keylength);
	// isAlgorithmRSA_CRT = do the same for RSA plus CRT (when it comes to mod calcs.)

	printf(" 1. Bob recieves a message \t : %s\n", recievedMessage);
	
	//printf("Check CRT Squareding\n");
	//squareAndMultiply(baseTest, exponentTest);

	if (isAlgorithmRSAstraightForwardCorrect == true) {
		printf("\n-> Sucessful run...");
	}
	else {
		printf("\n-> run NOT Sucessful...");
	}

	return 0;
}

//CLINT squareAndMultiply(CLINT base_X , CLINT exponent_k ) {
//	CLINT result;
//	int exponentBinary;
//	//clint2byte_l(exponent_k, exponentBinary);
//
//	return result;
//}


bool startRSAstraightForward(char sendedMessage, char recievedMessage, int keyLength) {
	
	CLINT privateKey_d, publicKey_e, publicKey_n, chiffre;

	// Bob acting:
	keyGeneration(keyLength, &privateKey_d, &publicKey_e, &publicKey_n); // & damit Werte "überschrieben werden können"
	printf(" 2. Key Generation by Bob\n");
	printf("	Private Key d \t\t : %c\n", privateKey_d);
	printf("	Public  Key e \t\t : %c\n", publicKey_e);
	printf("	Public  Key n \t\t : %s\n", xclint2str_l(publicKey_n, 10, 0));

	// Alice acting:
	encrypt(chiffre, sendedMessage, publicKey_e, publicKey_n);
	printf(" 3. Encryption by ALice:\n");
	printf("	Message m  \t\t : %c\n", sendedMessage);
	printf("	chiffre c  \t\t : %c\n", chiffre);

	// Bob acting:
	decrypt(recievedMessage, chiffre, publicKey_e, publicKey_n, privateKey_d);

	if (sendedMessage == recievedMessage) {
		return true;
	}
	else {
		return false;
	}
}

void keyGeneration(int keyLength, CLINT *privateKey_d, CLINT *publicKey_e, CLINT *publicKey_n) { //  *damit Werte "überschrieben werden können"
	
	CLINT seed, primeNumber_P, primeNumber_q, subtrahend, tempFactor_1, tempFactor_2, eulerPHI;

	// Number from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	char primeNumber_P_dec[] = "280150602340161058630637143798855285723"; // variable adjustable
	char primeNumber_q_dec[] = "184336280682452596243149253821987903701"; // variable adjustable
	char subtrahend_dec[] = "1";
	char tempFactor_1_dec[] = "1";
	char tempFactor_2_dec[] = "1";
	int baseOfNumbers = 10;

	// initialize CLINT variable
	str2clint_l(primeNumber_P, primeNumber_P_dec, baseOfNumbers);
	str2clint_l(primeNumber_q, primeNumber_q_dec, baseOfNumbers);
	
	// show decimal Value of CLINT variable 
	printf("	Prime number P  \t :%s\n", xclint2str_l(primeNumber_P, 10, 0));
	printf("	Prime number q  \t :%s\n", xclint2str_l(primeNumber_q, 10, 0));


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



	// *publicKey_e teilerfremd zu Euler_PHI finden (also GGT = 1)
	// *privateKey_d finden = 
	// *publicKey_e mal *privateKey_d MOD euler_PHI = 1



}

void encrypt(CLINT chiffre, char sendedMessage, CLINT *publicKey_e, CLINT *publicKey_n) {
	// to be filled
	// use squareAndMultiply()
}

void decrypt(char recievedMessage, CLINT chiffre, CLINT publicKey_e, CLINT publicKey_n, CLINT privateKey_d) {
	// to be filled
	// use squareAndMultiply()
}


