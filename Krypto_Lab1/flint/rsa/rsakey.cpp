//*****************************************************************************/
//                                                                            */
//  Software zum Buch "Kryptographie in C und C++"                            */
//  Autor: Michael Welschenbach                                               */
//                                                                            */
//  Quelldatei rsakey.cpp   Stand: 24.06.2001                                 */
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

#include <stdlib.h>
#include <string.h>
#include "rsakey.h"

#ifdef FLINT_TEST
static void cout_mess (const char* const, const int, const char* const);
#endif


// Member-Funktionen der Klasse RSAkey

// Konstruktor 1
RSAkey::RSAkey (const int bitlen)
{
  int done;
  seedBBS ((unsigned long)time (NULL));
  do
    {
      done = RSAkey::makekey (bitlen);
    }
  while (!done);
}


// Konstruktor 2 mit Vorgabe eines Startwerts in rnd fuer die Primzahlerzeugung
// Der Konstruktor erzeugt RSA-Schluessel vorgegebener Binaerlaenge, 
// Initialisierung des Zufallszahlengenerators randBBS() mit 
// vorgegebenem LINT-Argument und optionalem oeffentlichem Exponenten (ungerade)
// Falls oeffentlicher Exponent == 1 oder fehlend wird dieser
// zufaellig bestimmt. Falls der vorgegebene Exponent gerade ist, wird 
// durch die Funktion makekey() ein Fehlerstatus generiert, der 
// durch try() und catch() abgefangen werden kann, wenn die 
// Fehlerbehandlung mittels Exceptions aktiviert ist.

RSAkey::RSAkey (const int bitlen, const LINT& rnd, const LINT& PubExp)
{
  int done;
  seedBBS (rnd);
  do
    {
      done = RSAkey::makekey (bitlen, PubExp);
    }
  while (!done);
}


// Export oeffentliche Schluesselkomponente
PKEYSTRUCT RSAkey::export_public (void) const
{
  PKEYSTRUCT pktmp;

  pktmp.pubexp = key.pubexp;
  pktmp.mod = key.mod;
  pktmp.bitlen_mod = key.bitlen_mod;
  pktmp.bytelen_mod = key.bytelen_mod;

  return pktmp;
}


// Entschluesselungsfunktion
UCHAR* RSAkey::decrypt (const LINT& Ciph, int* LenMess)
{
  UCHAR* Mess = lint2byte (fastdecrypt (Ciph), LenMess);

#ifdef FLINT_TEST
  cout_mess ((const char*)Mess, key.bytelen_mod - 1, "Encryption Block nach Entschluesselung");
#endif

  // Parsing decrypted Encryption Block, PKCS#1-formatiert
  return parse_pkcs1 (Mess, LenMess);
}


// Signierfunktion
// Rueckgabe von 0: Fehler bei Formatierung, Nachricht zu lang
LINT RSAkey::sign (const UCHAR* const Mess, const int LenMess)
{

#ifdef FLINT_TEST
  cout << "Laenge Modulus = " << key.bytelen_mod << " Byte." << endl;
#endif
  UCHAR HashRes[RMDVER>>3];
  int LenEncryptionBlock = key.bytelen_mod - 1;
  UCHAR* EncryptionBlock = new UCHAR[LenEncryptionBlock];
  ripemd160 (HashRes, (UCHAR*)Mess, (ULONG)LenMess);

  if (NULL == format_pkcs1 (EncryptionBlock, LenEncryptionBlock, 
                              BLOCKTYPE_SIGN, HashRes, RMDVER >> 3))
    {
      delete [] EncryptionBlock;
      return LINT (0);             // Fehler bei Formatierung: Message zu lang
    }

#ifdef FLINT_TEST
  cout_mess ((const char*)EncryptionBlock, LenEncryptionBlock, "Encryption Block");
#endif

  // Wandle Encryption Block in LINT-Zahl (Konstruktor 3)
  LINT m = LINT (EncryptionBlock, LenEncryptionBlock);
  delete [] EncryptionBlock;

  return fastdecrypt (m);
}


// Loeschen eines Schluessels
void RSAkey::purge (void)
{
  key.pubexp.purge ();
  key.prvexp.purge ();
  key.mod.purge ();
  key.p.purge ();
  key.q.purge ();
  key.ep.purge ();
  key.eq.purge ();
  key.r.purge ();
  key.bitlen_mod = 0;
  key.bytelen_mod = 0;
}



// RSAkey Hilfsfunktionen


// RSA-Schluesselerzeugung nach IEEE P1363, Annex A
// Eingabe Schluesselaenge und optional oeffentlicher Exponent
// Falls oeffentlicher Exponent == 1 oder fehlend wird ein Exponent der 
// halben Moduluslaenge zufaellig bestimmt.

int RSAkey::makekey (const int length, const LINT& PubExp)
{
  // Erzeuge Primzahl p 
  // mit 2^(m - r - 1) <= p < 2^(m - r), wobei
  // m = floor((length + 1)/2) und r zufaellig im Intervall 2 <= r < 15
  const USHORT m = (((const USHORT)length + 1) >> 1) - 2 - usrandBBS_l () % 13;

  key.p = findprime (m, PubExp);

  // Bestimme Intervallgrenzen qmin und qmax fuer Primzahl q 
  // Setze qmin = floor ((2^(length - 1))/p + 1)
  LINT qmin = LINT(0).setbit (length - 1)/key.p + 1;
  // Setze qmax = floor ((2^length)/p) 
  LINT qmax = LINT(0).setbit (length)/key.p;

  // Erzeuge Primzahl q > p
  // mit Laenge qmin <= q <= qmax
  key.q = findprime (qmin, qmax, PubExp);

  // Erzeuge Modulus p*q
  // mit Laenge 2^(length - 1) <= p*q < 2^length
  key.mod = key.p * key.q;

  // Berechne Euler'sche Phi-Funktion
  LINT phi_n = key.mod - key.p - key.q + 1;

  // Erzeuge oeffentlichen Exponenten, falls nicht in PubExp vorgegeben
  if (1 == PubExp)
    {
      key.pubexp = randBBS (length/2) | 1; // Halbe Stellenlaenge des Modulus
      while (gcd (key.pubexp, phi_n) != 1)
        {
          ++key.pubexp;
          ++key.pubexp;
        }
    }
  else
    {
      key.pubexp = PubExp;
    }

  // Erzeuge geheimen Exponenten
  key.prvexp = key.pubexp.inv (phi_n);

  // Erzeuge geheime Schluesselkomponenten fuer die schnelle Entschluesselung
  key.ep = key.prvexp % (key.p - 1);
  key.eq = key.prvexp % (key.q - 1);
  key.r = inv (key.p, key.q);     // r = p^(-1) mod q, alternativ ist
                                  // r = q^(-1) mod p moeglich

  // Notiere Schluessellaenge
  key.bitlen_mod = ld (key.mod);
  key.bytelen_mod = key.bitlen_mod >> 3;
  if ((key.bitlen_mod % 8) > 0)
    {
      ++key.bytelen_mod;
    }

#ifdef FLINT_TEST
  cout << "Modulus = " << key.mod << endl;
  cout << "Oeffentlicher Exponent e = " << key.pubexp << endl;
  cout << "Privater Exponent d = " << key.prvexp << endl;
  cout << "p = " << key.p << endl;
  cout << "q = " << key.q << endl;
  cout << "d mod p-1 = " << key.ep << endl;
  cout << "d mod q-1 = " << key.eq << endl;
  cout << "Inverses p mod q = " << key.r << endl;
#endif // FLINT_TEST

  return testkey ();
}


// Funktion fuer Schluesseltests
int RSAkey::testkey ()
{
  LINT mess = randBBS (ld (key.mod) >> 1);
  return (mess == fastdecrypt (mexpkm (mess, key.pubexp, key.mod)));
}


// Funktion fuer die schnelle Entschluesselung
LINT RSAkey::fastdecrypt (const LINT& mess)
{
  LINT m, w;                           // Bei Verwendung alternativer
  m = mexpkm (mess, key.ep, key.p);    // CRT-Schluesselkomponente 
  w = mexpkm (mess, key.eq, key.q);    // r = q^(-1) mod p:
  w.msub (m, key.q);                   // m.msub (w, key.p);
  w = w.mmul (key.r, key.q) * key.p;   // m = m.mmul (key.r, key.p) * key.q;
  return (w + m);
}


// Operatoren =, ==, != der Klasse RSAkey

RSAkey& RSAkey::operator= (const RSAkey &k)
{
  if ((&k != this)) // Kopiere Objekt nicht auf sich selbst
    {
      key.pubexp      = k.key.pubexp;
      key.prvexp      = k.key.prvexp;
      key.mod         = k.key.mod;
      key.p           = k.key.p;
      key.q           = k.key.q;
      key.ep          = k.key.ep;
      key.eq          = k.key.eq;
      key.r           = k.key.r;
      key.bitlen_mod  = k.key.bitlen_mod;
      key.bytelen_mod = k.key.bytelen_mod;
    }
  return *this;
}


int operator== (const RSAkey& k1, const RSAkey& k2)
{
  if (&k1 == &k2)       //lint !e506
    {
      return 1;
    }

  return (k1.key.pubexp      == k2.key.pubexp      &&
          k1.key.prvexp      == k2.key.prvexp      &&
          k1.key.mod         == k2.key.mod         &&
          k1.key.p           == k2.key.p           &&
          k1.key.q           == k2.key.q           &&
          k1.key.ep          == k2.key.ep          &&
          k1.key.eq          == k2.key.eq          &&
          k1.key.r           == k2.key.r           &&
          k1.key.bitlen_mod  == k2.key.bitlen_mod  &&
          k1.key.bytelen_mod == k2.key.bytelen_mod);

  // Operator == gibt 1 zurueck bei Gleichheit, 0 bei Ungleichheit
}


int operator!= (const RSAkey& k1, const RSAkey& k2)
{
  if (&k1 == &k2)       //lint !e506
    {
      return 0;
    }

  return (k1.key.pubexp      != k2.key.pubexp      ||
          k1.key.prvexp      != k2.key.prvexp      ||
          k1.key.mod         != k2.key.mod         ||
          k1.key.p           != k2.key.p           ||
          k1.key.q           != k2.key.q           ||
          k1.key.ep          != k2.key.ep          ||
          k1.key.eq          != k2.key.eq          ||
          k1.key.r           != k2.key.r           ||
          k1.key.bitlen_mod  != k2.key.bitlen_mod  ||
          k1.key.bytelen_mod != k2.key.bytelen_mod);

  // Operator != gibt 1 zurueck bei Ungleichheit, 0 bei Gleichheit
}


fstream& operator<< (fstream& s, const RSAkey& k)
{
  s << k.key.pubexp << k.key.prvexp << k.key.mod << k.key.p << k.key.q
    << k.key.ep << k.key.eq << k.key.r;

  write_ind_ushort (s, k.key.bitlen_mod);
  write_ind_ushort (s, k.key.bytelen_mod);

  return s;
}


fstream& operator>> (fstream& s, RSAkey& k)
{
  s >> k.key.pubexp >> k.key.prvexp >> k.key.mod >> k.key.p >> k.key.q
    >> k.key.ep >> k.key.eq >> k.key.r;

  read_ind_ushort (s, &k.key.bitlen_mod);
  read_ind_ushort (s, &k.key.bytelen_mod);

  return s;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// Member-Funktionen der Klasse RSApub

// Konstruktor
RSApub::RSApub (const RSAkey& k)
{
  pkey = k.export_public ();
}


// Verschluesselungsfunktion
LINT RSApub::crypt (const UCHAR* const Mess, const int LenMess)
{
  int LenEncryptionBlock = pkey.bytelen_mod - 1;
  UCHAR* EncryptionBlock = new UCHAR[LenEncryptionBlock];

  // Formatierung des Encryption Block nach PKCS#1
  if (NULL == format_pkcs1 (EncryptionBlock,
                            LenEncryptionBlock,
                            BLOCKTYPE_ENCR,
                            Mess,
                            (ULONG)LenMess))
    {
      delete [] EncryptionBlock;
      return LINT (0);             // Fehler bei Formatierung: Message zu lang
    }

#ifdef FLINT_TEST
  cout_mess ((const char*)EncryptionBlock, LenEncryptionBlock, "Encryption Block vor Verschluesselung");
#endif

  // Wandle Encryption Block in LINT-Zahl (Konstruktor 3)
  LINT m = LINT (EncryptionBlock, LenEncryptionBlock);
  delete [] EncryptionBlock;

  return (mexpkm (m, pkey.pubexp, pkey.mod));
}


// Funktion zur Verifikation digitaler Signaturen
int RSApub::verify (const UCHAR* const Mess, const int LenMess, const LINT& Signature)
{
  int length, verification = 0;
  UCHAR HashRes1[RMDVER>>3];
  UCHAR* HashRes2 = lint2byte (mexpkm (Signature, pkey.pubexp, pkey.mod), &length);
  ripemd160 (HashRes1, (UCHAR*)Mess, (ULONG)LenMess);

#ifdef FLINT_TEST
  cout_mess ((const char*)HashRes2, pkey.bytelen_mod - 1, "Encryption Block bei Verifikation der Signatur");
#endif

  // Entnehme Daten aus entschluesseltem Encryption Block, PKCS#1-formatiert
  HashRes2 = parse_pkcs1 (HashRes2, &length);

  if (length == (RMDVER >> 3))
    {
      verification = !memcmp ((char *)HashRes1, (char*)HashRes2, RMDVER >> 3);
    }

#ifdef FLINT_TEST
  cout_mess ((const char*)HashRes2, length, "Hash-Wert bei Verifikation der Signatur");
  cout << "Verification = " << verification << endl;
#endif

  return verification;
}


// Loeschen eines oeffentlichen RSA-Schluessels
void RSApub::purge (void)
{
  pkey.pubexp.purge ();
  pkey.mod.purge ();
  pkey.bitlen_mod = 0;
  pkey.bytelen_mod = 0;
}


// Operatoren =, ==, != der Klasse RSApub

RSApub& RSApub::operator= (const RSApub &k)
{
  if ((&k != this)) // Kopiere Objekt nicht auf sich selbst
    {
      pkey.pubexp      = k.pkey.pubexp;
      pkey.mod         = k.pkey.mod;
      pkey.bitlen_mod  = k.pkey.bitlen_mod;
      pkey.bytelen_mod = k.pkey.bytelen_mod;
    }
  return *this;
}


int operator== (const RSApub& k1, const RSApub& k2)
{
  if (&k1 == &k2)       //lint !e506
    {
      return 1;
    }

  return (k1.pkey.pubexp      == k2.pkey.pubexp      &&
          k1.pkey.mod         == k2.pkey.mod         &&
          k1.pkey.bitlen_mod  == k2.pkey.bitlen_mod  &&
          k1.pkey.bytelen_mod == k2.pkey.bytelen_mod);

  // Operator == gibt 1 zurueck bei Gleichheit, 0 bei Ungleichheit
}


int operator!= (const RSApub& k1, const RSApub& k2)
{
  if (&k1 == &k2)       //lint !e506
    {
      return 0;
    }

  return (k1.pkey.pubexp      != k2.pkey.pubexp      ||
          k1.pkey.mod         != k2.pkey.mod         ||
          k1.pkey.bitlen_mod  != k2.pkey.bitlen_mod  ||
          k1.pkey.bytelen_mod != k2.pkey.bytelen_mod);

  // Operator != gibt 1 zurueck bei Ungleichheit, 0 bei Gleichheit
}


fstream& operator<< (fstream& s, const RSApub& k)
{
  s << k.pkey.pubexp << k.pkey.mod;

  write_ind_ushort (s, k.pkey.bitlen_mod);
  write_ind_ushort (s, k.pkey.bytelen_mod);

  return s;
}


fstream& operator>> (fstream& s, RSApub& k)
{
  s >> k.pkey.pubexp >> k.pkey.mod;

  read_ind_ushort (s, &k.pkey.bitlen_mod);
  read_ind_ushort (s, &k.pkey.bytelen_mod);

//s >> k.pkey.bitlen_mod >> k.pkey.bytelen_mod;

  return s;
}


// Funktion fuer die Formatierung von Encryption Blocks (EB) nach PKCS#1
// EB    = BT||PS1...PSl||00||DATA1...DATAk
// BT    = Block Typ Public Key Operation: 01 Signieren, 02 Verschluesselung
// PSi   = BT 01: Werte FF (hex), mindestens 8 Byte
//         BT 02: Zufaellige Werte ungleich Null als Fueller, mindestens 8 Byte
// DATAi = Datenbytes, k <= Bytelaenge des Modulus - 10, Parameter data zeigt
//         auf Daten, Laenge k wird im Parameter LenData uebergeben
// Puffer fuer den Encryption Block ist von aufrufender Funktion zu allokieren
// Puffergroesse muss der Bytelaenge des Modulus entsprechen (Mindestgroesse)
// Der Parameter EB zeigt auf den allokierten Speicher

UCHAR* format_pkcs1 (const UCHAR* EB, const int LenEB, const UCHAR BlockType, const UCHAR* data, const int LenData)
{
  // Berechne die Laenge lps des Padding-Blocks PS
  int lps = LenEB - 2 - LenData;

  if (lps < 8)                     // PKCS#1: Laenge Padding-Block >= 8
    {
      return NULL;                 // Rueckgabe NULL-Pointer ist Fehlerstatus
    }

  UCHAR* hlp = (UCHAR*)EB;

  switch (BlockType)
    {
      case 01: 
        *hlp++ = 0x01;             // Block Typ 01: Private Key Operation
        while (lps-- > 0)
          {
            *hlp++ = 0xff;         // 8 <= lps Werte FF (hex)
          }
        break;
      case 02: *hlp++ = 0x02;      // Block Typ 02: Public Key Operation
        while (lps-- > 0)
          {
            do
              {  
                *hlp = ucrandBBS_l ();   
              }                    // 8 <= lps Zufallsbytes ungleich Null
            while (*hlp == 0);
            ++hlp;
          }
        break;
      default:  
        return NULL;               // Undefinierter Block Typ in BlockType
    }         

  *hlp++ = 0x00;                   // Begrenzungsbyte

  for (int l = 1; l <= LenData; l++)
    {
      *hlp++ = *data++;
    }

  return (UCHAR*)EB;
}


// Parser fuer entschluesselten Encryption Block, PKCS#1-formatiert
// Gibt Zeiger auf die im EB enthaltenen Daten zurueck, 
// Speichert Laenge der enthaltenen Daten in LenData
// Rueckgabe von NULL bei Formatfehler

UCHAR* parse_pkcs1 (const UCHAR* eb, int* LenData)
{
  UCHAR* hlp = (UCHAR*)eb;
  UCHAR BlockType;
  int error = 0, noofpadc = 0;

  if ((BlockType = *hlp) > 2) // Kontrolle Block Typ: 00, 01, 02 sind gueltig
    {
      return (UCHAR*)NULL;
    }

  ++hlp;
  --*LenData;
  switch (BlockType)
    {
      case 0:
      case 1:                 // Block Typ 00 oder 01: Signaturanwendung
        while (*hlp != 0 && *LenData > 0)  // Analysiere Padding-String PS
          {
            if (*hlp != 0xff) 
              {
                error = 1;    // Pruefe Padding == 0xFF bei Block Type 01
                break;        // Setze Fehlerflag error auf 1 bei Wert != 0xFF
              }
            ++hlp;
            --*LenData;
            ++noofpadc;
          };
        break;
      case 2:                 // Block Typ 02: Verschluesselungsanwendung
        while (*hlp != 0 && *LenData > 0)  // Suche Ende des Padding-String PS
          {
            ++hlp;
            --*LenData;
            ++noofpadc;
          };
        break;
      default:
        return (UCHAR*)NULL;  // Falscher Block Typ
    }

  if (noofpadc < 8 || error || *LenData == 0)    
    {                         // Fehler bei Pruefung des Padding oder
      return (UCHAR*)NULL;    // Null vor Daten des EB-Formates vorhanden?
    }
  else
    {
      ++hlp;                  // Separationsbyte 00 uebergehen
      --*LenData;             // Laenge der Payload in LenData
    }

  return hlp;                 // Gebe Zeiger auf Payload zurueck
}


#ifdef FLINT_TEST
// Byteweise Ausgabe von Nachrichten als Hexwerte
static void cout_mess (const char* mess, const int len, const char* const titel)
{
  cout << titel << " = " << hex
       << setfill ('0') << resetiosflags (ios::showbase);

  for  (int i = 0; i < len; i++)
    {
      cout << setw (2) << (((unsigned)*mess++) & 0xff);
    }

  cout << setfill (' ') << resetiosflags (ios::hex) << endl;
}
#endif

