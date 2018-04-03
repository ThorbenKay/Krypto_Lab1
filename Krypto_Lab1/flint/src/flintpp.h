//*****************************************************************************/
//                                                                            */
//  Arithmetik- und Zahlentheorie-Funktionen fuer grosse Zahlen in C/C++      */
//  Software zum Buch "Kryptographie in C und C++"                            */
//  Autor: Michael Welschenbach                                               */
//                                                                            */
//  Quelldatei flintpp.h    Stand: 27.05.2001                                 */
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

#ifndef __FLINTPPH__
#define __FLINTPPH__            // flintpp.h ist #included

//lint -wlib(1)

#if defined FLINTPP_ANSI
#include <limits>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <new>
#include <algorithm>
#if !defined __WATCOMC__
using namespace std;
#endif // #!defined __WATCOMC__
#else
#include <limits.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <new.h>
#endif // #defined FLINTPP_ANSI

#ifndef __CPLUSPLUS__
#define __CPLUSPLUS__
#endif

#ifndef __cplusplus
#define __cplusplus
#endif

#define FLINTCPPHVMAJ   2 /* Major-Versionsnummer von flintpp.cpp */
#define FLINTCPPHVMIN   2 /* Minor-versionsnummer von flintpp.cpp */
#define FLINTCOMPATMAJ  2 /* Major-Vers. von flint.c benoetigt von flintpp.cpp */
#define FLINTCOMPATMIN  2 /* Minor-Vers. von flint.c benoetigt von flintpp.cpp */

//lint -wlib(4)


// Einbinden der FLINT/C C-Bibliothek

#include "flint.h"


// Versionskontrolle

#if ((FLINTCOMPATMAJ > FLINT_VERMAJ) || \
    ((FLINTCOMPATMAJ == FLINT_VERMAJ) && (FLINTCOMPATMIN > FLINT_VERMIN)))
#error Versionsfehler: FLINTPP.H inkompatibel zu FLINT.H
#endif


// LINT-Fehler

enum LINT_ERRORS {
        E_LINT_OK     = 0,      // Alles O.K.
        E_LINT_EOF    = 0x0010, // File-IO-Error
        E_LINT_DBZ    = 0x0020, // Division by zero
        E_LINT_NHP    = 0x0040, // Heap-Fehler: new ergibt NULL-Pointer
        E_LINT_OFL    = 0x0080, // Ueberlauf in Funktion/Operator
        E_LINT_UFL    = 0x0100, // Unterlauf in Funktion/Operator
        E_LINT_VAL    = 0x0200, // Argument von Funktion/Operator ist nicht initialisiert
        E_LINT_BOR    = 0x0400, // Ungueltige Basis
        E_LINT_MOD    = 0x0800, // Modulus gerade in mexp?m
        E_LINT_NPT    = 0x1000, // Null-Pointer als Argument
        E_LINT_ERR    = 0x2000  // Keine Loesung bei root oder chinrem,
                                // sonst: nicht naeher spezifizierter Fehler
};


// LINT-Exceptions

class LINT_Error                        // Abstrakte Basis-Klasse
{
 public:
  const char* function;
  int argno, lineno;
  virtual void debug_print (void) const = 0;  // Pure virtual function
  virtual ~LINT_Error() {function = 0;};
};

class LINT_DivByZero : public LINT_Error      // Division durch Null
{
 public:
  LINT_DivByZero (const char* const func, const int line);
  void debug_print (void) const;
};

class LINT_File : public LINT_Error     // Fehler bei Datei-IO
{
 public:
  LINT_File (const char* const func, const int line);
  void debug_print (void) const;
};

class LINT_Init : public LINT_Error     // Funktionsargument nicht initialisiert
{
 public:
  LINT_Init (const char* const func, const int arg, const int line);
  void debug_print (void) const;
};

class LINT_Heap : public LINT_Error     // Heap-Fehler bei new
{
 public:
  LINT_Heap (const char* const func, const int line);
  void debug_print (void) const;
};

class LINT_OFL : public LINT_Error      // Ueberlauf in Funktion
{
 public:
  LINT_OFL (const char* const func, const int line);
  void debug_print (void) const;
};

class LINT_UFL : public LINT_Error      // Unterlauf in Funktion
{
 public:
  LINT_UFL (const char* const func, const int line);
  void debug_print (void) const;
};

class LINT_Base : public LINT_Error     // Ungueltige Basis
{
 public:
  LINT_Base (const char* const func, const int line);
  void debug_print (void) const;
};

class LINT_Emod : public LINT_Error     // Modulus gerade bei mexp?m
{
 public:
  LINT_Emod (const char* const func, const int line);
  void debug_print (void) const;
};

class LINT_Nullptr : public LINT_Error  // Null-Pointer als Argument
{
 public:
  LINT_Nullptr (const char* const func, const int arg, const int line);
  void debug_print (void) const;
};

class LINT_Mystic : public LINT_Error   // Unbekannter Fehler ;-)
{
 public:
  LINT_Mystic (const char* const func, const int errcode, const int line);
  void debug_print (void) const;
};


class LintInit
{
 public:
  LintInit (void);
};

// Die Konstruktorfunktion LintInit () setzt den ios-internen Wert, der nach
// Initialisierung von LINT::flagsindex durch ios::iword (flagsindex)
// repraesentiert wird, auf die LINT-default-Werte. Damit wird der default-
// Modus fuer die stream-Ausgabe von LINT-Werten festgelegt. Im aufrufenden
// Programm kan dies durch LINT-Manipulatoren geaendert werden (vgl.
// Manipulatoren wie LINT::SetLintFlags (LINT::flags) weiter unten).


// Makros zur Internationalisierung

#define ggT            gcd
#define xggT           xgcd
#define kgV            lcm
#define chinrest       chinrem
#define primwurz       primroot
#define zweianteil     twofact


// Deklaration der Klasse LINT

class LINT
{
 public:

  // LINT-FRIENDS

  friend LintInit::LintInit (void);

  // Ueberladene Operatoren, implementiert als friend-Funktionen

  friend const LINT operator+ (const LINT&, const LINT&);
  friend const LINT operator- (const LINT&, const LINT&);
  friend const LINT operator* (const LINT&, const LINT&);
  friend const LINT operator/ (const LINT&, const LINT&);
  friend const LINT operator% (const LINT&, const LINT&);
  friend const LINT operator<< (const LINT&, const int);
  friend const LINT operator>> (const LINT&, const int);

  // Logische Funktionen

  friend const int operator== (const LINT&, const LINT&);
  friend const int operator!= (const LINT&, const LINT&);
  friend const int operator> (const LINT&, const LINT&);
  friend const int operator< (const LINT&, const LINT&);
  friend const int operator<= (const LINT&, const LINT&);
  friend const int operator>= (const LINT&, const LINT&);

  // Boole'sche Funktionen

  friend const LINT operator^ (const LINT&, const LINT&);
  friend const LINT operator| (const LINT&, const LINT&);
  friend const LINT operator& (const LINT&, const LINT&);

  // Arithmetik

  friend const LINT add (const LINT&, const LINT&);
  friend const LINT sub (const LINT&, const LINT&);
  friend const LINT mul (const LINT&, const LINT&);
  friend const LINT sqr (const LINT&);
  friend const LINT divr (const LINT&, const LINT&, LINT&);
  friend const LINT mod (const LINT&, const LINT&);
  friend const LINT mod2 (const LINT&, const USHORT);

  // Vertauschung

  friend void fswap (LINT&, LINT&);

  // Loeschen von LINT, Ruecksetzen auf Wert 0

  friend void purge (LINT&);

  // Modulare Arithmetik

  friend const LINT madd (const LINT&, const LINT&, const LINT&);
  friend const LINT msub (const LINT&, const LINT&, const LINT&);
  friend const LINT mmul (const LINT&, const LINT&, const LINT&);
  friend const LINT msqr (const LINT&, const LINT&);
  friend const LINT mexp (const LINT&, const LINT&, const LINT&);
  friend const LINT mexp (const USHORT, const LINT&, const LINT&);
  friend const LINT mexp (const LINT&, const USHORT, const LINT&);
  friend const LINT mexp5m (const LINT&, const LINT&, const LINT&);
  friend const LINT mexpkm (const LINT&, const LINT&, const LINT&);
  friend const LINT mexp2 (const LINT&, const USHORT, const LINT&);
  friend const LINT shift (const LINT&, const int);

  // Zahlentheoretische friend-Funktionen

  friend const int isprime (const LINT&);
  friend const LINT issqr (const LINT&);
  friend const unsigned int ld (const LINT&);
  friend const LINT gcd (const LINT&, const LINT&);
  friend const LINT xgcd (const LINT&, const LINT&, LINT&, int&, LINT&, int&);
  friend const LINT inv (const LINT&, const LINT&);
  friend const LINT lcm (const LINT&, const LINT&);
  friend const int jacobi (const LINT&, const LINT&);
  friend const LINT root (const LINT&);
  friend const LINT root (const LINT&, const LINT&);
  friend const LINT root (const LINT&, const LINT&, const LINT&);
  friend const LINT primroot (const unsigned int, LINT**);
  friend const LINT chinrem (const unsigned int, LINT**);
  friend const int twofact (const LINT&, LINT&);
  friend const LINT findprime (const USHORT);
  friend const LINT findprime (const USHORT, const LINT&);
  friend const LINT findprime (const LINT&, const LINT&, const LINT&);
  friend const LINT nextprime (const LINT&, const LINT&);
  friend const LINT extendprime (const USHORT, const LINT&, const LINT&, const LINT&);
  friend const LINT extendprime (const LINT&, const LINT&, const LINT&, const LINT&, const LINT&);
  friend const LINT strongprime (const USHORT);
  friend const LINT strongprime (const USHORT, const LINT&);
  friend const LINT strongprime (const USHORT, const USHORT, const USHORT, const USHORT, const LINT&);
  friend const LINT strongprime (const LINT&, const LINT&, const LINT&);
  friend const LINT strongprime (const LINT&, const LINT&, const USHORT, const USHORT, const USHORT, const LINT&);
  friend const int iseven (const LINT&);
  friend const int isodd (const LINT&);
  friend const int mequ (const LINT&, const LINT&, const LINT&);

  // Pseudo-Zufallszahlen-Generatoren

  friend LINT randl (const int);
  friend LINT randl (const LINT&, const LINT&);
  friend void seedl (const LINT&);
  friend LINT randBBS (const int);
  friend LINT randBBS (const LINT&, const LINT&);
  friend int seedBBS (const LINT&);

  // Konvertierung

  friend char* lint2str (const LINT&, const USHORT, const int = 0);
  friend UCHAR* lint2byte (const LINT&, int*);

  // Ueberladen von ostream insert Operator << fuer Ausgabe von LINT-Objekten
  //
  //  Folgende Modi zur Ausgabe von LINT-Werten sind verfuegbar:
  //  1.  Basis fuer die Darstellung
  //      Die Default-Basis ist 16 fuer die den LINT-Wert und 10 fuer die Laenge
  //      Diese Vorgabe fuer LINT-Werte kann durch den Aufruf von
  //          LINT::setf (LINT::base);
  //      fuer den Standard-Ausgabestrom cout und fuer den allgemeinen Fall von
  //          LINT::setf (ostream, LINT::base);
  //      mit base = linthex, lintdec, lintoct oder lintbin auf die angegebene
  //      Basis eingestellt werden, die Vorgabe fuer die Laengenangabe mit der
  //      Funktion ios::setf (ios::iosbase) mit iosbase = hex, dec oder oct
  //
  //  2.  Anzeige in Grossbuchstaben
  //      Standardmaessig erfolgt die Anzeige von Hex-Ziffern und die des
  //      Praefixes fuer die Hexadezimaldarstellung in Kleinbuchstaben:
  //      0xfafafa. Durch den Aufruf von
  //          LINT::setf (LINT::lintuppercase);
  //          LINT::setf (ostream, LINT::lintuppercase);
  //      kann dies geaendert werden, so dass Grossbuchstaben angezeigt werden:
  //      0XFAFAFA.
  //
  //  3.  Anzeige der Laenge eines LINT-Typs
  //      Standardmaessig wird die Binaerlaenge in eines LINT-Typs angezeigt.
  //      Durch den Aufruf
  //          LINT::unsetf (ostream, LINT::lintshowlength);
  //      wird dies geaendert, so dass keine Anzeige der Laenge erfolgt.
  //
  //  4.  Anzeige des Praefixes fuer die Zahldarstellung
  //      Standardmaessig werden LINT-Typen mit Praefix fuer die Zahldarstellung
  //      angezeigt. Durch Aufruf von
  //              LINT::unsetf (LINT::lintshowbase)
  //              LINT::unsetf (ostream, LINT::lintshowbase)
  //      wird dies geaendert.
  //
  //  Alternativ zu LINT::setf () und LINT::unsetf () koennen die Manipulatoren
  //  SetLintFlags () und ResetLintFlags () verwendet werden:
  //
  //      cout << ResetLintFlags (LINT::flag) << ... //Ausschalten
  //
  //      cout << SetLintFlags (LINT::flag) << ...   //Einschalten
  //
  //   Weitere Manipulatoren:
  //
  //      LintHex
  //      LintBin
  //      LintDec
  //      LintOct
  //      LintUpr
  //      LintLwr
  //      LintShowbase
  //      LintNobase
  //      LintShowlength


  friend ostream& operator<< (ostream& s, const LINT& ln);

  // Ueberladen von fstream/istream/ofstream insert Operatoren >>,<<
  // fuer file-IO von LINT-Objekten

  friend fstream& operator<< (fstream& s, const LINT& ln);
  friend ofstream& operator<< (ofstream& s, const LINT& ln);

  friend fstream& operator>> (fstream& s, LINT& ln);
  friend ifstream& operator>> (ifstream& s, LINT& ln);


  // LINT-MEMBERS

  // Konstruktoren

  LINT (void);                               // Konstruktor  1
  LINT (const char* const, const int);       // Konstruktor  2
  LINT (const UCHAR* const, const int);      // Konstruktor  3
  LINT (const char* const);                  // Konstruktor  4
  LINT (const LINT&);                        // Konstruktor  5  (Copy)
  LINT (const signed int);                   // Konstruktor  6
  LINT (const signed long);                  // Konstruktor  7
  LINT (const unsigned char);                // Konstruktor  8
  LINT (const unsigned short);               // Konstruktor  9
  LINT (const unsigned int);                 // Konstruktor 10
  LINT (const unsigned long);                // Konstruktor 11
  LINT (const CLINT);                        // Konstruktor 12

  //  Destruktor

  ~LINT (void)
    {
#ifdef FLINT_SECURE
      purge_l (n_l);
#endif // FLINT_SECURE
      delete [] n_l;
    }


  // Ueberladene Operatoren, implementiert als member Funktionen
  // linkes (implizites) Argument ist immer vom Typ LINT

  // Assignment

  const LINT& operator= (const LINT&);

  // Arithmetik

  const LINT& operator++ (void);
  const LINT operator++ (int);
  const LINT& operator-- (void);
  const LINT operator-- (int);
  const LINT& operator+= (const LINT&);
  const LINT& operator-= (const LINT&);
  const LINT& operator*= (const LINT&);
  const LINT& operator/= (const LINT&);
  const LINT& operator%= (const LINT&);

  // Bitweise und Boole'sche Funktionen

  const LINT& operator<<= (const int);
  const LINT& operator>>= (const int);
  const LINT& operator^= (const LINT&);
  const LINT& operator|= (const LINT&);
  const LINT& operator&= (const LINT&);

  // Member Funktionen
  // Arithmetik

  const LINT& add (const LINT&);
  const LINT& sub (const LINT&);
  const LINT& mul (const LINT&);
  const LINT& sqr (void);
  const LINT& divr (const LINT&, LINT&);
  const LINT& mod (const LINT&);
  const LINT& mod2 (const USHORT);

  // Modulare Arithmetik

  const LINT& madd (const LINT&, const LINT&);
  const LINT& msub (const LINT&, const LINT&);
  const LINT& mmul (const LINT&, const LINT&);
  const LINT& msqr (const LINT&);
  const LINT& mexp (const LINT&, const LINT&);
  const LINT& mexp (const USHORT, const LINT&);
  const LINT& mexp5m (const LINT&, const LINT&);
  const LINT& mexpkm (const LINT&, const LINT&);
  const LINT& mexp2 (const USHORT, const LINT&);
  const LINT& shift (const int);

  // Zahlentheoretische member Funktionen

  const LINT gcd (const LINT&) const;
  const LINT xgcd (const LINT&, LINT&, int&, LINT&, int&) const;
  const LINT inv (const LINT&) const;
  const LINT lcm (const LINT&) const;
  const int jacobi (const LINT&) const;
  const LINT root (void) const;
  const LINT root (const LINT&) const;
  const LINT root (const LINT&, const LINT&) const;
  const int twofact (LINT&) const;
  const unsigned int ld (void) const;
  const int isprime (void) const;
  const LINT issqr (void) const;
  const int mequ (const LINT&, const LINT&) const;
  const LINT chinrem (const LINT&, const LINT&, const LINT&) const;
  const int iseven (void) const;
  const int isodd (void) const;


  // Bit-Zugriffe

  const LINT& setbit (const unsigned int);
  const LINT& clearbit (const unsigned int);
  const int testbit (const unsigned int) const;

  // Vertauschung

  LINT& fswap (LINT&);

  // Loeschen von LINT, Ruecksetzen auf Wert 0

  void purge (void);


  // Konvertierung & Ein-/Ausgabe

  // Zur Erlaeuterung dieser Deklarationen siehe oben bei class LintInit

  enum {
      lintdec        = 0x010,
      lintoct        = 0x020,
      linthex        = 0x040,
      lintshowbase   = 0x080,
      lintuppercase  = 0x100,
      lintbin        = 0x200,
      lintshowlength = 0x400
  };

  static long flags (void);
  static long flags (ostream&);
  static long setf (long int);
  static long setf (ostream&, long int);
  static long unsetf (long int);
  static long unsetf (ostream&, long int);
  static long restoref (long int);
  static long restoref (ostream&, long int);


  // LINT-nach-String Konvertierungsfunktionen

  inline char* hexstr (void) const
    {
      return lint2str (16);
    }
  inline char* decstr (void) const
    {
      return lint2str (10);
    }
  inline char* octstr (void) const
    {
      return lint2str (8);
    }
  inline char* binstr (void) const
    {
      return lint2str (2);
    }

  char* lint2str (const USHORT, const int = 0) const;
  UCHAR* lint2byte (int*) const;

  inline void disp (char* str)
    {
      cout << str << lint2str (16) << endl << ld () << " bit\n";
    }


  // Setze benutzerspezifischen Error-Handler

  static void Set_LINT_Error_Handler (void (*)(LINT_ERRORS, const char* const, const int, const int));

  //Get Error/Warning-Status

  LINT_ERRORS Get_Warning_Status (void);

 private:
  // Zeiger auf CLINT-Typ
  clint* n_l;

  // Maximale Stellenzahl des CLINT-Typs
  int maxlen;

  // Initialisierungsstatus
  int init;

  // Status nach einer Operation auf einem LINT-Objekt
  LINT_ERRORS status;

  // LINT::Default-Error-Handler
  static void panic (LINT_ERRORS, const char* const, const int, const int);

  // Dummy-Objekt der Klasse LintInit
  static LintInit setup;

  // Zeiger auf zu allokierendes ios-Flag-Register
   static long flagsindex;
};

// CLASS LINT endet hier
///////////////////////////////////////////////////////////////////////////////




// Noch ein paar Hilfskonstrukte fuer die LINT-Manipulatoren

// ostream-Formatierungsfunktionen
ostream& _SetLintFlags (ostream& , int);
ostream& _ResetLintFlags (ostream& , int);

// Container-Klasse LINT_omanip
template <class T>
class LINT_omanip
{
 public:
  LINT_omanip (ostream&(*g)(ostream&, T), T j): i(j), f(g) {}
  friend ostream& operator <<(ostream& os, const LINT_omanip<T>& m)
    {
      return (*m.f) (os, m.i);
    }

 protected:
  T i;
  ostream& (*f)(ostream&, T);
}; //lint !e1712  Jammere nicht ueber fehlenden default-Konstruktor


// Manipulatoren fuer das Ausgabeformat von LINT-Objekten

LINT_omanip <int> SetLintFlags (int);
LINT_omanip <int> ResetLintFlags (int);

ostream& LintHex (ostream&);
ostream& LintDec (ostream&);
ostream& LintOct (ostream&);
ostream& LintBin (ostream&);
ostream& LintUpr (ostream&);
ostream& LintLwr (ostream&);
ostream& LintShowbase (ostream&);
ostream& LintNobase (ostream&);
ostream& LintShowlength (ostream&);
ostream& LintNolength (ostream&);


// Templates zum plattformunabhaengigen Schreiben und Lesen von USHORT-Werten

template <class T>
int read_ind_ushort (T& s, clint* dest)
{
  UCHAR buff[sizeof (clint)];
  unsigned i;
  s.read ((char*)buff, sizeof (clint));

  if (!s)
    {
      return -1;
    }
  else
    {
      *dest = 0;
      for (i = 0; i < sizeof (clint); i++)
        {
          *dest |= ((clint)buff[i]) << (i << 3);
        }
      return 0;
    }
}


template <class T>
int write_ind_ushort (T& s, clint src)
{
  UCHAR buff[sizeof (clint)];
  unsigned i, j;
  for (i = 0, j = 0; i < sizeof (clint); i++, j = i << 3)
    {
      buff[i] = (UCHAR)((src & (0xff << j)) >> j);
    }
  s.write ((const char*)buff, sizeof (clint));
  if (!s)
    {
      return -1;
    }
  else
    {
      return 0;
    }
}


// min, max als Template-inline-Funktionen
// (Nach Scott Meyers,"Effective C++", 2nd. Ed. Addison-Wesley 1998)
// min und max sollten in der C++-Standard Template Library enthalten sein,
// sind es aber wohl noch nicht ueberall

#if !defined FLINTPP_ANSI || (defined __IBMCPP__ && (__IBMCPP__ <=300)) || (defined _MSC_VER && (_MSC_VER <= 1200)) || defined __WATCOMC__

#ifndef min
template <class T>
inline const T& min (const T& a, const T& b)
{ return a < b ? a : b; }
#endif // min

#ifndef max
template <class T>
inline const T& max (const T& a, const T& b)
{ return a > b ? a : b; }
#endif // max
#endif // !(defined __IBMCPP__ || defined FLINTPP_ANSI)


#endif // __FLINTPPH__







