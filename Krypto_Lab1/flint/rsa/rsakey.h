//*****************************************************************************/
//                                                                            */
//  Software zum Buch "Kryptographie in C und C++"                            */
//  Autor: Michael Welschenbach                                               */
//                                                                            */
//  Quelldatei rsakey.h     Stand: 19.05.2001                                 */
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

#ifndef __RSAKEYH__
#define __RSAKEYH__             // rsakey.h ist #included

#include "flintpp.h"
#include "ripemd.h"

#define BLOCKTYPE_SIGN 01
#define BLOCKTYPE_ENCR 02


typedef struct
{
    LINT pubexp, prvexp, mod, p, q, ep, eq, r;
    USHORT bitlen_mod;  // Binaerlaenge Modulus
    USHORT bytelen_mod; // Laenge Modulus in Byte
} KEYSTRUCT;


typedef struct
{
    LINT pubexp, mod;
    USHORT bitlen_mod;  // Binaerlaenge Modulus
    USHORT bytelen_mod; // Laenge Modulus in Byte
} PKEYSTRUCT;


class RSAkey
{
  public:
    inline RSAkey (void) {};
    RSAkey (const int);
    RSAkey (const int, const LINT&, const LINT& = 1);
    PKEYSTRUCT export_public (void) const;
    UCHAR* decrypt (const LINT&, int*);
    LINT sign (const UCHAR* const, const int);
    void purge (void);
    RSAkey& operator= (const RSAkey&);

    friend int operator== (const RSAkey&, const RSAkey&);
    friend int operator!= (const RSAkey&, const RSAkey&);
    friend fstream& operator<< (fstream&, const RSAkey&);
    friend fstream& operator>> (fstream&, RSAkey&);

  private:
    KEYSTRUCT key;

    // Hilfsfunktionen
    int makekey (const int, const LINT& = 1);
    int testkey (void);
    LINT fastdecrypt (const LINT&);
};


class RSApub
{
  public:
    inline RSApub (void) {};
    RSApub (const RSAkey&);
    LINT crypt (const UCHAR* const, const int);
    int verify (const UCHAR* const, const int, const LINT&);
    void purge (void);
    RSApub& operator= (const RSApub&);

    friend int operator== (const RSApub&, const RSApub&);
    friend int operator!= (const RSApub&, const RSApub&);
    friend fstream& operator<< (fstream&, const RSApub&);
    friend fstream& operator>> (fstream&, RSApub&);

  private:
    PKEYSTRUCT pkey;
};


///////////////////////////////////////////////////////////////////////////////
// Hilfsfunktionen ausserhalb der RSA-Klassen

// Funktion zur PKCS#1-Formatierung von Encryption Blocks
UCHAR* format_pkcs1 (const UCHAR*, const int, const UCHAR, const UCHAR*, const int);

// Parser fuer entschluesselten Encryption Block, PKCS#1-formatiert
UCHAR* parse_pkcs1 (const UCHAR*, int*);

#endif // __RSAKEYH__

