// Krypto_Lab1.c 
// Thorben Kay & Dominic Dietl
//

#include "flint.h"
#include <stdio.h>
#include <time.h> // contains clock()
#include <string.h>
#include <stdbool.h>
#include "Krypto_Lab1.h"
typedef unsigned short clint;
typedef clint CLINT[CLINTMAXDIGIT + 1];


int main(int argc, char* argv[]) {

	int keyLength = 0;

	printf("--------------------------------------------------------------\n");
	printf("--------------------------------------------------------------\n");
	printf("--------------------THE GREAT RSA MACHINE---------------------\n");
	printf("-----------------------by Kay and Dietl-----------------------\n");
	printf("--------------------------------------------------------------\n\n");



	printf("\n\n THE RSA with STRAIGHT FORWARD =\n\n");
	// Bob acting:

	////Include this for keylength 128 Bit:
	//keyLength = 128;
	//printf(" 1. Denfined keylength \t\t : %i Bit\n", keyLength);
	//printf("\n 2. Key Generation by Bob\n");
	//// Numbers and Keys from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	//char primeNumber_P_dec[] = "9446087950672310551"; // 64 Bit
	//char primeNumber_q_dec[] = "9270433997468153567"; // 64 Bit
	//char publicKey_e_dec[] = "65537"; // no further restirictions often =3 ; even more often 65537 (F4)
	//char privateKey_d_dec[] = "55217705478688103782924194069675715973"; // 128 Bit

	//// Include this for keylength 256 Bit:
	//keyLength = 256;
	//printf(" 1. Denfined keylength \t\t : %i Bit\n", keyLength);
	//printf("\n 2. Key Generation by Bob\n");
	//// Numbers and Keys from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	//char primeNumber_P_dec[] = "280150602340161058630637143798855285723"; // 128 Bit
	//char primeNumber_q_dec[] = "184336280682452596243149253821987903701"; // 128 Bit
	//char publicKey_e_dec[] = "65537"; // no further restirictions often =3 ; even more often 65537 (F4)
	//char privateKey_d_dec[] = "23603187110593578184937184241008316352076378834332071822245298133461559552273"; // 256 Bit

	////Include this for keylength 512 Bit:
	//keyLength = 512;
	//printf(" 1. Denfined keylength \t\t : %i Bit\n", keyLength);
	//printf("\n 2. Key Generation by Bob\n");
	// Numbers and Keys from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	//char primeNumber_P_dec[] = "80824724804745807486283639664086432423365230305232460150306615994756662304441"; // 256 Bit
	//char primeNumber_q_dec[] = "67812425181959338416264932580167523465123963953968346505886858496583126663179"; // 256 Bit
	//char publicKey_e_dec[] = "65537"; // no further restirictions often =3 ; even more often 65537 (F4)
	//char privateKey_d_dec[] = "393935157415617435641075967003824583003670809962882715409854897001995601435763\n\
	//						   7774126753089038906823817465347676500676055668244992372968921008141312294913"; // 512 Bit

	//Include this for keylength 1024 Bit:
	keyLength = 1024;
	printf(" 1. Denfined keylength \t\t : %i Bit\n", keyLength);
	printf("\n 2. Key Generation by Bob\n");
	// Numbers and Keys from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	char primeNumber_P_dec[] = "1206033059788433644552688637904417716272053869059200857175126056313481840397838\n\
								6912736067427117879855559258458167783174957432223334606379662379986299611719"; // 512 Bit
	char primeNumber_q_dec[] = "9726819209556728347873630938097509771574194049064697946727269659843616448842286\n\
								033081467709147918351744068121655454084111833480777947376699905658879866239"; // 512 Bit
	char publicKey_e_dec[] = "65537"; // no further restirictions often =3 ; even more often 65537 (F4)
	char privateKey_d_dec[] =  "64923665068390268658296567278655330009459074958279308442529809930993219450248445\n\
								13466932679728828192155328015368511256237981548785279209473488848056648958210990\n\
								59281748496721535079831978406580718774440399919425051290444684362011693536669840\n\
								64571079334450609260019839823696598312405449566772381242720806119245"; // 1024 Bit

	
	startRSAstraightForward(&primeNumber_P_dec, &primeNumber_q_dec, &publicKey_e_dec, &privateKey_d_dec, keyLength);

	printf("\n______________________________________________________________\n\n");

	printf("\n\n THE RSA with CHINESE REMAINDER THEOREM = \n\n");
	printf(" 1. Denfined keylength \t\t : %i Bit\n", keyLength);
	printf("\n 2. Key Generation by Bob\n");


	startRSAchineseRemainderTheorem(&primeNumber_P_dec, &primeNumber_q_dec, &publicKey_e_dec, &privateKey_d_dec, keyLength);

	printf("\n______________________________________________________________\n\n");

	return 0;
}


void squareAndMultiply(CLINT base, CLINT exponent, CLINT n, CLINT *result, int keyLength) {


	char res[] = "1";
	int i = 0;
	str2clint_l(*result, res, 10);

	for (i= (keyLength-1); i >= 0; i--) {
		// mod quad
		msqr_l(*result, *result, n);
		// if exponent[index] equals 1
		if (testbit_l(exponent, i)) {
			// mod multiply
			mmul_l(*result, base, *result, n);
		}
	}
}

void chineseReminderTheorem(CLINT primeNumber_P, CLINT primeNumber_q, CLINT chiffre, CLINT privateKey_d, CLINT publicKey_n, CLINT *mes, int keyLength)
{
	CLINT  d_p, d_q, minus1, qMinus1, pMinus1, u, v, m_p, m_q, greatestCommonFactor, u_p_product, v_q_product , u_p_mq_product, v_q_mP_product ;
	char minus1_dec[] = "-1";
	int signForU[1], signForV[1];
	int baseOfNumbers = 10;

	usub_l(primeNumber_P, 1, pMinus1);
	usub_l(primeNumber_q, 1, qMinus1);

	mod_l(privateKey_d, pMinus1, d_p);
	mod_l(privateKey_d, qMinus1, d_q);

	printf("	d_p\t\t\t : %s\n", xclint2str_l(d_p, 10, 0));
	printf("	d_q\t\t\t : %s\n", xclint2str_l(d_q, 10, 0));

	/*  Funktion:  Groesster gemeinsamer Teiler d = ggT(a,b) und Darstellung des  */
	/*             ggT d = au + bv (Erweiterter Euklidischer Algorithmus)         */
	/*  Syntax:    void xgcd_l (CLINT a_l, CLINT b_l, CLINT d_l, CLINT u_l,       */
	/*                                      int *sign_u, CLINT v_l, int *sign_v); */
	/*  Eingabe:   a_l, b_l (Operanden)                                           */
	/*  Ausgabe:   d_l (ggT von a_l und b_l)                                      */
	/*             u_l, v_l (Faktoren der Darstellung des ggT von a_l und b_l     */
	/*             mit Vorzeichen sign_u und sign_v)                              */
	/*  Rueckgabe: -                                                              */
	xgcd_l(primeNumber_P, primeNumber_q, greatestCommonFactor, u, signForU, v, signForV);
	
	printf("        Found u and v fulfilling 1 = up+uq using Euclidean algorithm:\t\t\t \n");
	printf("	u\t\t\t : %s \n",  xclint2str_l(u, 10, 0));
	printf("	sign u\t\t\t : %i \n", signForU[0]);
	printf("	v\t\t\t : %s \n",  xclint2str_l(v, 10, 0));
	printf("	sign v\t\t\t : %i \n", signForV[0]);


	keyLength = keyLength / 2; // manipulate keylength for CRT
	squareAndMultiply(chiffre, d_p, primeNumber_P, m_p, keyLength);
	squareAndMultiply(chiffre, d_q, primeNumber_q, m_q, keyLength);

	//combine
	mul_l(u, primeNumber_P, u_p_product);
	mul_l(v, primeNumber_q, v_q_product);
	mul_l(u_p_product, m_q, u_p_mq_product);
	mul_l(v_q_product, m_p, v_q_mP_product);
	
	str2clint_l(minus1, minus1_dec, baseOfNumbers);

	if ((signForU[0] == -1) && ((signForV[0] == -1))) {
		add_l(u_p_mq_product, v_q_mP_product, mes);
		mul_l(mes, minus1, mes);
	}
	if ((signForU[0] == 1) && ((signForV[0] == 1))) {
		add_l(u_p_mq_product, v_q_mP_product, mes);
	}
	if ((signForU[0] == -1) && ((signForV[0] == 1))) {
		sub_l(v_q_mP_product, u_p_mq_product, mes);
	}
	if ((signForU[0] == 1) && ((signForV[0] == -1))) {
		sub_l(u_p_mq_product, v_q_mP_product, mes);
	}
	mod_l(mes, publicKey_n, mes);
}


void startRSAstraightForward(char *primeNumber_P_dec, char *primeNumber_q_dec, char *publicKey_e_dec, char *privateKey_d_dec, int keyLength)
{
	double timeRSAstraightForward = 0.0, timeRSAstraightForwardSTART;      // time measurment variables

	CLINT privateKey_d, publicKey_e, publicKey_n, chiffre, encryptedMessage;
	CLINT primeNumber_P, primeNumber_q, subtrahend, tempFactor_1, tempFactor_2, eulerPHI;

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
	timeRSAstraightForwardSTART = clock(); // start time measurement


	printf("\n 3. Encryption by ALice:\n");

	CLINT message;
	char message_dec[] = "1657089733347464174213"; // in mobilefish enter Plaintext "123456789"
	printf("	Message m  \t\t : %s\n", message_dec);
	str2clint_l(message, message_dec, 10);
	squareAndMultiply(message, publicKey_e, publicKey_n, &chiffre, keyLength);
	printf("	Chiffre  \t\t : %s\n", xclint2str_l(chiffre, 10, 0));

	printf("	--> sends chiffre to Bob\n\n");






	// Bob acting:

	printf(" 4. Bob decrypts recieved chiffre:\n");

	squareAndMultiply(chiffre, privateKey_d, publicKey_n, encryptedMessage, keyLength);
	printf("	decrypted message \t : %s (should be %s) \n\n", xclint2str_l(encryptedMessage, 10, 0), message_dec);

	timeRSAstraightForward += clock() - timeRSAstraightForwardSTART;    // stop time measurement
	timeRSAstraightForward = timeRSAstraightForward / CLOCKS_PER_SEC;  // rescale to seconds

	printf(" Measured time \t\t : %f seconds \n", timeRSAstraightForward);

}

void startRSAchineseRemainderTheorem(char *primeNumber_P_dec, char *primeNumber_q_dec, char *publicKey_e_dec, char *privateKey_d_dec, int keyLength)
{
	double timeRSAchineseRemainderTheorem = 0.0, timeRSAchineseRemainderTheoremSTART;

	CLINT privateKey_d, publicKey_e, publicKey_n, chiffre, encryptedMessage, mes;
	CLINT primeNumber_P, primeNumber_q, subtrahend, tempFactor_1, tempFactor_2, eulerPHI;

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

	timeRSAchineseRemainderTheoremSTART = clock(); // start time measurement

	printf("\n 3. Encryption by ALice:\n");
	CLINT message;
	char message_dec[] = "1657089733347464174213"; // in mobilefish enter Plaintext "123456789"
	printf("	Message m  \t\t : %s\n", message_dec);
	str2clint_l(message, message_dec, 10);

	squareAndMultiply(message, publicKey_e, publicKey_n, &chiffre, keyLength);
	printf("	Chiffre  \t\t : %s\n", xclint2str_l(chiffre, 10, 0));

	printf("	--> sends chiffre to Bob\n\n");








	// Bob acting:

	printf(" 4. Bob decrypts recieved chiffre:\n");

	chineseReminderTheorem(primeNumber_P, primeNumber_q, chiffre, privateKey_d, publicKey_n, &mes, keyLength); 
	printf("	decrypted message \t : %s (should be %s) \n\n", xclint2str_l(mes, 10, 0), message_dec);

	timeRSAchineseRemainderTheorem += clock() - timeRSAchineseRemainderTheoremSTART;    // stop time measurement
	timeRSAchineseRemainderTheorem = timeRSAchineseRemainderTheorem / CLOCKS_PER_SEC;  // rescale to seconds

	printf(" Measured time \t\t : %f seconds \n", timeRSAchineseRemainderTheorem);



}

