//*****************************************************************************/
//                                                                            */
//  Software zum Buch "Kryptographie in C und C++"                            */
//  Autor: Michael Welschenbach                                               */
//                                                                            */
//  Quelldatei rsademo.cpp  Stand: 25.05.2001                                 */
//                                                                            */
//  Copyright (C) 1998-2001 by Michael Welschenbach                           */
//  Copyright (C) 1998-2001 by Springer-Verlag Berlin, Heidelberg             */
//                                                                            */
//  All Rights Reserved                                                       */
//                                                                            */
//  Die Software darf fuer nichtkommerzielle Zwecke frei verwendet und        */
//  veraendert werden, solange die folgenden Bedingungen anerkannt werden:    */
//                                                                            */
//  (1) Alle Aenderungen muessen als solche kenntlich gemacht werden, so dass */
//      nicht der Eindruck erweckt wird, es handle sich um die Original-      */
//      Software.                                                             */
//                                                                            */
//  (2) Die Copyright-Hinweise und die uebrigen Angaben in diesem Feld        */
//      duerfen weder entfernt noch veraendert werden.                        */
//                                                                            */
//  (3) Weder der Verlag noch der Autor uebernehmen irgendeine Haftung fuer   */
//      eventuelle Schaeden, die aus der Verwendung oder aus der Nicht-       */
//      verwendbarkeit der Software, gleich fuer welchen Zweck, entstehen     */
//      sollten, selbst dann nicht, wenn diese Schaeden durch Fehler verur-   */
//      sacht wurden, die in der Software enthalten sein sollten.             */
//                                                                            */
//*****************************************************************************/

#if defined FLINT_ANSI
#include <string>
#if !defined __WATCOMC__
using namespace std;
#endif // #!defined __WATCOMC__
#else
#include <string.h>
#endif // #defined FLINT_ANSI

#include "rsakey.h"

static void svkey (RSAkey&, RSApub&);
static void rdkey (RSAkey&, RSApub&);
static UCHAR* rndmess (const int);
static void cout_bytes (const char*, const int, const char* const);

// Schluessellaenge in Bit
#define KEYLENGTH 1024

// Laenge Testnachricht fuer Signierung in Byte
#define MESSLNGTH 1000

// Laenge Testnachricht fuer Verschluesselung in Byte, maximale Blocklaenge
// in Byte fuer PKCS#1-Format
#define BLCKLNGTH (((KEYLENGTH + 7) >> 3) - 11)

int main()
{
  int dlen;
  initrand64_lt();
  initrandBBS_lt();

  cout << "Testmodul " << __FILE__
       << " compiliert fuer FLINT/C-Library Version "
       << verstr_l () << endl;

  // Erzeuge RSA-Schluessel

  cout << "Erzeuge "<< KEYLENGTH << " Bit RSA-Schluessel..." << flush;
  RSAkey my_key (KEYLENGTH);
  cout << endl << "RSA-Schluesselsatz erzeugt." << endl;

  // Extrahiere Public Key
  RSApub my_public_key (my_key);
  cout << "Oeffentlichen RSA-Schluessel extrahiert." << endl;

  // Erzeuge Nachrichtenblock Message_1
  const UCHAR* const mess1 = rndmess (BLCKLNGTH);
  cout << "Zufallstext erzeugt." << endl;
  cout_bytes ((const char*)mess1, BLCKLNGTH, "Message_1");
  cout << endl;

  // Nachricht Message_1 verschluesseln
  LINT cipher = my_public_key.crypt (mess1, BLCKLNGTH);
  cout << "Message_1 verschluesselt." << endl;
  cout << "Chiffretext = " << cipher << endl;

  // Erzeuge Nachricht Message_2
  const UCHAR* const mess2 = rndmess (MESSLNGTH);
  cout << "Zufallstext erzeugt." << endl;
  cout_bytes ((const char*)mess2, MESSLNGTH, "Message_2");
  cout << endl;

  // Message_2 hashen und signieren
  LINT signature = my_key.sign (mess2, MESSLNGTH);
  UCHAR HashRes[RMDVER>>3];
  ripemd160 (HashRes, (UCHAR*)mess2, MESSLNGTH);

  cout << "Signatur zu Message_2 erzeugt." << endl;
  cout_bytes ((char *)HashRes, RMDVER >> 3, "Hashwert zu Message_2");
  cout << "Signatur = " << signature << endl;

  // Chiffretest entschluesseln und pruefen
  UCHAR* decrypted_mess = my_key.decrypt (cipher, &dlen);
  if (dlen == BLCKLNGTH && !memcmp (mess1, decrypted_mess, BLCKLNGTH))
      cout << "Ver-/Entschluesselung OK" << endl;
  else
      cout << "Fehler bei Ver-/Entschluesselung!" << endl;

  // Signatur pruefen
  if (my_public_key.verify (mess2, MESSLNGTH, signature))
      cout << "Verifikation OK" << endl;
  else
      cout << "Fehler in Signatur!" << endl;


  // Schreib-/Lese-Test

  RSAkey testk1;  // Nicht initialisierter RSA-Schluessel
  RSApub testk2;  // Nicht initialisierter Public Key

  // Speichern der RSA-Schluessel
  svkey (my_key, my_public_key);

  // Lesen der RSA-Schluessel
  rdkey (testk1, testk2);

  // Vergleich von Original und Duplikat
  if (testk1 == my_key)
      cout << "Schreib-/Lesetest RSAkey OK" << endl;
  else
      cout << "Fehler in Schreib-/Lesetest fuer RSAkey!" << endl;

  if (testk2 == my_public_key)
      cout << "Schreib-/Lesetest RSApub OK" << endl;
  else
      cout << "Fehler in Schreib-/Lesetest fuer RSApub!" << endl;

  delete [] (UCHAR*)mess1;
  delete [] (UCHAR*)mess2;

  return 0;
}


// Hilfsfunktionen zum Schreiben und Lesen

static void svkey (RSAkey &prvk, RSApub &pubk)
{
  fstream fout1 ("rsakey.prv",ios::out | ios::binary);
  fout1 << prvk;
  fout1.close ();

  fstream fout2 ("rsakey.pub",ios::out | ios::binary);
  fout2 << pubk;
  fout2.close ();
}


static void rdkey (RSAkey &prvk, RSApub &pubk)
{
  fstream fin1 ("rsakey.prv",ios::in | ios::binary);
  fin1 >> prvk;
  fin1.close ();

  fstream fin2 ("rsakey.pub",ios::in | ios::binary);
  fin2 >> pubk;
  fin2.close ();
}


// Hilfsfunktion

// Erzeugung von Zufalls-Messages der Laenge len UCHAR
static UCHAR* rndmess (const int len )
{
  UCHAR* mess = new UCHAR[sizeof(UCHAR) * (len + 1)];
  if (NULL == mess)
    {
      return NULL;
    }

  UCHAR* tmp = mess;
  for (int i = 0; i < len; i++)
    {
      *tmp++ = ucrand64_l ();
    }
  *tmp = '\0';

  return mess;
}


// Byteweise Ausgabe von Nachrichten als Hexwerte
static void cout_bytes (const char* mess, const int len, const char* const titel)
{
  cout << titel << " = " << hex
       << setfill ('0') << resetiosflags (ios::showbase);

  for  (int i = 0; i < len; i++)
    {
      cout << setw (2) << (((unsigned)*mess++) & 0xff);
    }

  cout << setfill (' ') << resetiosflags (ios::hex) << endl;
}

