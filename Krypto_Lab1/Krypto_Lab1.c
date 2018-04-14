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



	// Bob acting:

	////Include this for keylength 128 Bit:
	//printf(" 1. Denfined keylength \t\t : 128 Bit\n");
	//printf("\n 2. Key Generation by Bob\n");
	//// Numbers and Keys from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	//char primeNumber_P_dec[] = "9446087950672310551"; // 64 Bit
	//char primeNumber_q_dec[] = "9270433997468153567"; // 64 Bit
	//char publicKey_e_dec[] = "65537"; // no further restirictions often =3 ; even more often 65537 (F4)
	//char privateKey_d_dec[] = "55217705478688103782924194069675715973"; // 128 Bit

	// Include this for keylength 256 Bit:
	printf(" 1. Denfined keylength \t\t : 256 Bit\n");
	printf("\n 2. Key Generation by Bob\n");
	// Numbers and Keys from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	char primeNumber_P_dec[] = "280150602340161058630637143798855285723"; // 128 Bit
	char primeNumber_q_dec[] = "184336280682452596243149253821987903701"; // 128 Bit
	char publicKey_e_dec[] = "65537"; // no further restirictions often =3 ; even more often 65537 (F4)
	char privateKey_d_dec[] = "23603187110593578184937184241008316352076378834332071822245298133461559552273"; // 256 Bit

	////Include this for keylength 512 Bit:
	//printf(" 1. Denfined keylength \t\t : 512 Bit\n");
	//printf("\n 2. Key Generation by Bob\n");
	//// Numbers and Keys from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	//char primeNumber_P_dec[] = "80824724804745807486283639664086432423365230305232460150306615994756662304441"; // 256 Bit
	//char primeNumber_q_dec[] = "67812425181959338416264932580167523465123963953968346505886858496583126663179"; // 256 Bit
	//char publicKey_e_dec[] = "65537"; // no further restirictions often =3 ; even more often 65537 (F4)
	//char privateKey_d_dec[] = "393935157415617435641075967003824583003670809962882715409854897001995601435763\n\
	//						   7774126753089038906823817465347676500676055668244992372968921008141312294913"; // 512 Bit

	////Include this for keylength 1024 Bit:
	//printf(" 1. Denfined keylength \t\t : 1024 Bit\n");
	//printf("\n 2. Key Generation by Bob\n");
	//// Numbers and Keys from https://www.mobilefish.com/services/rsa_key_generation/rsa_key_generation.php
	//char primeNumber_P_dec[] = "1206033059788433644552688637904417716272053869059200857175126056313481840397838\n\
	//							6912736067427117879855559258458167783174957432223334606379662379986299611719"; // 512 Bit
	//char primeNumber_q_dec[] = "9726819209556728347873630938097509771574194049064697946727269659843616448842286\n\
	//							033081467709147918351744068121655454084111833480777947376699905658879866239"; // 512 Bit
	//char publicKey_e_dec[] = "65537"; // no further restirictions often =3 ; even more often 65537 (F4)
	//char privateKey_d_dec[] =  "64923665068390268658296567278655330009459074958279308442529809930993219450248445\n\
	//							13466932679728828192155328015368511256237981548785279209473488848056648958210990\n\
	//							59281748496721535079831978406580718774440399919425051290444684362011693536669840\n\
	//							64571079334450609260019839823696598312405449566772381242720806119245"; // 1024 Bit

	startRSAstraightForward(&primeNumber_P_dec, &primeNumber_q_dec, &publicKey_e_dec, &privateKey_d_dec);

	startRSAchineseRemainderTheorem(&primeNumber_P_dec, &primeNumber_q_dec, &publicKey_e_dec, &privateKey_d_dec);
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

void chineseReminderTheorem(CLINT primeNumber_P, CLINT primeNumber_q, CLINT message, CLINT privateKey_d, CLINT publicKey_n, CLINT *sig)
{
	CLINT  d_p, d_q, minus1, qMinus1, pMinus1, u, v, sig_p, sig_q, greatestCommonFactor, u_p_product, v_q_product , u_p_sigQ_product, v_q_sigP_product ;
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
	printf("	v\t\t\t : %s \n",  xclint2str_l(v, 10, 0));

	squareAndMultiply(message, d_p, primeNumber_P, sig_p);
	squareAndMultiply(message, d_q, primeNumber_q, sig_q);

	//combine
	mul_l(u, primeNumber_P, u_p_product);
	mul_l(v, primeNumber_q, v_q_product);
	mul_l(u_p_product, sig_q, u_p_sigQ_product);
	mul_l(v_q_product, sig_p, v_q_sigP_product);
	
	str2clint_l(minus1, minus1_dec, baseOfNumbers);

	if ((signForU[0] == -1) && ((signForV[0] == -1))) {
		add_l(u_p_sigQ_product, v_q_sigP_product, sig);
		mul_l(sig, minus1, sig);
	}
	if ((signForU[0] == 1) && ((signForV[0] == 1))) {
		add_l(u_p_sigQ_product, v_q_sigP_product, sig);
	}
	if ((signForU[0] == -1) && ((signForV[0] == 1))) {
		sub_l(v_q_sigP_product, u_p_sigQ_product, sig);
	}
	if ((signForU[0] == 1) && ((signForV[0] == -1))) {
		sub_l(u_p_sigQ_product, v_q_sigP_product, sig);
	}

	// Approach of keep the negativ sign in mind failed !!!!!!
	//str2clint_l(minus1, minus1_dec, baseOfNumbers);

	//if (signForU[0] == -1) {
	//	
	//	mul_l(u, minus1, u);
	//}

	//if (signForV[0] == -1) {

	//	mul_l(v, minus1, v);
	//}

}


void startRSAstraightForward(char *primeNumber_P_dec, char *primeNumber_q_dec, char *publicKey_e_dec, char *privateKey_d_dec)
{

	CLINT privateKey_d, publicKey_e, publicKey_n, chiffre, encryptedMessage;
	CLINT primeNumber_P, primeNumber_q, subtrahend, tempFactor_1, tempFactor_2, eulerPHI;

	char subtrahend_dec[] = "1";
	char tempFactor_1_dec[] = "1";
	char tempFactor_2_dec[] = "1";
	int baseOfNumbers = 10;

	// initialize CLINT variable
	str2clint_l(primeNumber_P, primeNumber_P_dec, baseOfNumbers);
	str2clint_l(primeNumber_q, primeNumber_q_dec, baseOfNumbers);

	printf("\n\n THE RSA with STRAIGHT FORWARD =\n\n");
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

	printf("	--> sends chiffre to Bob\n\n");






	// Bob acting:

	printf(" 4. Bob decrypts recieved Message:\n");

	squareAndMultiply(chiffre, privateKey_d, publicKey_n, encryptedMessage);
	printf("	decrypted message \t : %s (should be %s) \n\n", xclint2str_l(encryptedMessage, 10, 0), message_dec);

	//if (xclint2str_l(encryptedMessage, 10, 0) == xclint2str_l(message,10,0)) {  // Need to be improved
	//	printf("---> Process Sucessful !!");
	//}
	//else(printf("---> Process failed !!"));

	printf("\n______________________________________________________________\n\n");

}

void startRSAchineseRemainderTheorem(char *primeNumber_P_dec, char *primeNumber_q_dec, char *publicKey_e_dec, char *privateKey_d_dec)
{

	CLINT privateKey_d, publicKey_e, publicKey_n, chiffre, encryptedMessage, sig;
	CLINT primeNumber_P, primeNumber_q, subtrahend, tempFactor_1, tempFactor_2, eulerPHI;

	char subtrahend_dec[] = "1";
	char tempFactor_1_dec[] = "1";
	char tempFactor_2_dec[] = "1";
	int baseOfNumbers = 10;

	// initialize CLINT variable
	str2clint_l(primeNumber_P, primeNumber_P_dec, baseOfNumbers);
	str2clint_l(primeNumber_q, primeNumber_q_dec, baseOfNumbers);

	printf("\n\n THE RSA with CHINESE REMAINDER THEOREM = \n\n");
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

	printf("\n3. Encryption (creating signiture) by ALice:\n");
	CLINT message;
	char message_dec[] = "1657089733347464174213";
	printf("	Message m  \t\t : %s\n", message_dec);
	str2clint_l(message, message_dec, 10);

	chineseReminderTheorem(primeNumber_P, primeNumber_q, message, privateKey_d, publicKey_n, &sig);
	printf("	Signature  \t\t : %s\n", xclint2str_l(sig, 10, 0));

	printf("\t--> sends signiture to Bob\n\n");








	// Bob acting:

	printf(" 4. Bob decrypts recieved Message:\n");

	squareAndMultiply(sig, publicKey_e, publicKey_n, encryptedMessage);
	printf("	decrypted message \t : %s (should be %s) \n\n", xclint2str_l(encryptedMessage, 10, 0), message_dec);

	////if (xclint2str_l(encryptedMessage, 10, 0) == xclint2str_l(message,10,0)) {  // Need to be improved
	////	printf("---> Process Sucessful !!");
	////}
	////else(printf("---> Process failed !!"));

	printf("\n______________________________________________________________\n\n");

}

