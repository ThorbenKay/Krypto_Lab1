/******************************************************************************/
/*                                                                            */
/*  Arithmetik- und Zahlentheorie-Funktionen fuer grosse Zahlen in C          */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei ripemd.c     Stand: 05.07.2001                                 */
/*                                                                            */
/*  Copyright (C) 1998-2001 by Michael Welschenbach                           */
/*  Copyright (C) 2001 by Springer-Verlag Berlin, Heidelberg                  */
/*                                                                            */
/*  All Rights Reserved                                                       */
/*                                                                            */
/*  Die Software darf fuer nichtkommerzielle Zwecke frei verwendet und        */
/*  veraendert werden, solange die folgenden Bedingungen anerkannt werden:    */
/*                                                                            */
/*  (1) Alle Aenderungen muessen als solche kenntlich gemacht werden, so dass */
/*      nicht der Eindruck erweckt wird, es handle sich um die Original-      */
/*      Software.                                                             */
/*                                                                            */
/*  (2) Die Copyright-Hinweise und die uebrigen Angaben in diesem Feld        */
/*      duerfen weder entfernt noch veraendert werden.                        */
/*                                                                            */
/*  (3) Weder der Verlag noch der Autor uebernehmen irgendeine Haftung fuer   */
/*      eventuelle Schaeden, die aus der Verwendung oder aus der Nicht-       */
/*      verwendbarkeit der Software, gleich fuer welchen Zweck, entstehen     */
/*      sollten, selbst dann nicht, wenn diese Schaeden durch Fehler verur-   */
/*      sacht wurden, die in der Software enthalten sein sollten.             */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "flint.h"
#include "ripemd.h"


/* Prototypen */

static void appetize (ULONG *);
static void swallow (ULONG *, ULONG *);
static void digest (ULONG *, UCHAR *, ULONG[]);

#ifdef FLINT_SECURE
static void zero_ul (ULONG *, ULONG *, ULONG *, ULONG *);
static void *local_memset (void *, int, size_t);
#endif

/* Macros */

#define ROL(X, N)   (((X) << (N)) | ((X) >> (32-(N))))

#define CHAIN(A, B, C, D, E, X, S) { \
          (A) = (E); (E) = (D); (D) = ROL((C), 10); (C) = (B); \
          (B) = ROL((X), (S)) + (A); \
        }

/* Konvertierung eines UCHAR-Vektors in einen ULONG-Wert */

#if defined LITTLE_ENDIAN
#define UC2UL(ucptr)  *(unsigned long *)((ucptr))
#else
#define UC2UL(ucptr) \
          (((unsigned long) *((ucptr)+3) << 24) | \
           ((unsigned long) *((ucptr)+2) << 16) | \
           ((unsigned long) *((ucptr)+1) <<  8) | \
           ((unsigned long) *(ucptr)))
#endif


/* Addition einer einstelligen Zahl b zu einer zweistelligen Zahl (a[1],a[0]) */
/* Falls bei Addition a[0]+=b ein Uebertrag auftritt, wird a[1] inkrementiert */

#define ADDC(a, b) { \
          if (((a)[0] + (b)) < (a)[0]) {(a)[1]++;} \
          (a)[0] += (b); \
        }


/*****************************************************************************/
/* RIPEMD-160 API                                                            */
/*****************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Funktion:   Hashen einer Nachricht der Laenge length Byte in einem        */
/*              Schritt (Single Shot Mode)                                    */
/*  Syntax:     void ripemd160 (UCHAR *HashRes, UCHAR *clear, ULONG length);  */
/*  Eingabe:    UCHAR *clear (Zeiger auf Nachrichtenblock)                    */
/*              ULONG length (Laenge des Nachrichtenblocks in Byte)           */
/*  Ausgabe:    UCHAR *HashRes (Hashwert wird unter HashRes gespeichert,      */
/*                              Laenge 20 bytes)                              */
/*  Rueckgabe:  -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ripemd160 (UCHAR *HashRes, UCHAR *clear, ULONG length)
{
  RMDSTAT hws;

  ripeinit (&hws);
  ripefinish (HashRes, &hws, clear, length);

#ifdef FLINT_SECURE
  local_memset (&hws, 0, sizeof (hws));
#endif
}


/******************************************************************************/
/* Funktionen zum Hashen einer Nachricht in mehreren Bloecken                 */
/* Abfolge: Initialisierung mit ripeinit                                      */
/*          Hashen Block1, Block2,... ,Blockn mit ripehash                    */
/*          Abschluss mit ripefinish                                          */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Funktion:   Initialisierung der RIPEMD 160-Funktion                       */
/*  Syntax:     void ripeinit (RMDSTAT *hws);                                 */
/*  Eingabe:    RMDSTAT *hws (RIPEMD-Statuspuffer)                            */
/*  Ausgabe:    -                                                             */
/*  Rueckgabe:  -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ripeinit (RMDSTAT *hws)
{
  appetize (hws->stomach);
  hws->total[0] = 0;
  hws->total[1] = 0;
}


/******************************************************************************/
/*                                                                            */
/*  Funktion:   Hashen eines Nachrichtenblocks der Laenge length Byte         */
/*  Syntax:     int ripehash (RMDSTAT *hws, UCHAR *clear, ULONG length);      */
/*  Eingabe:    RMDSTAT *hws (RIPEMD-Statuspuffer)                            */
/*              UCHAR *clear (Zeiger auf Nachrichtenblock)                    */
/*              ULONG length (Laenge in Byte des Nachrichtenblocks = 0 mod 64)*/
/*  Ausgabe:    -                                                             */
/*  Rueckgabe:  E_CLINT_OK falls alles O.K.                                   */
/*              E_CLINT_RMD falls length != 0 mod 64                          */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
ripehash (RMDSTAT *hws, UCHAR *clear, ULONG length)
{
  ULONG   ULBlock[16];    /* aktueller 16-ULONG (512 Bit) Message-Block */
  ULONG   noofblocks;     /* Anzahl 16-ULONG- (512 Bit-) Bloecke        */
  ULONG   i, j;           /* Zaehler                                    */

  /* Existiert unvollstaendiger 64-Byte-Block? */
  if (length & 63)
    {
      return E_CLINT_RMD;          /* Falls ja: Rueckgabe Fehlercode  */
    }

  /* Anzahl von 64-Byte-Bloecken in clear */
  noofblocks = length >> 6;

  /* Verarbeite 64-Byte-Bloecke von clear */
  for (i = 0; i < noofblocks; i++)
    {
        for (j = 0; j < 16; j++)
          {
            ULBlock[j] = UC2UL (clear);
            clear += 4;
          }

        swallow (hws->stomach, ULBlock);
    }

  /* Addiere Laenge von clear zu hws->total */
  ADDC (hws->total, length);

#ifdef FLINT_SECURE
  /* Ueberschreibe Hilfsvariablen */
  zero_ul (&ULBlock[0], &ULBlock[1], &ULBlock[2], &ULBlock[3]);
  zero_ul (&ULBlock[4], &ULBlock[5], &ULBlock[6], &ULBlock[7]);
  zero_ul (&ULBlock[8], &ULBlock[9], &ULBlock[10], &ULBlock[11]);
  zero_ul (&ULBlock[12], &ULBlock[13], &ULBlock[14], &ULBlock[15]);
#endif

  return 0;
}


/******************************************************************************/
/*                                                                            */
/*  Funktion:   Abschluss der Hashfunktion RIPEMD 160                         */
/*  Syntax:     void ripefinish (UCHAR *HashRes, RMDSTAT *hws, UCHAR *clear,  */
/*                                                             ULONG length); */
/*  Eingabe:    RMDSTAT *hws (RIPEMD-Statuspuffer)                            */
/*              UCHAR *clear (Zeiger auf letzten Nachrichtenblock)            */
/*              ULONG length (Laenge des Nachrichtenblocks in Byte)           */
/*  Ausgabe:    UCHAR *HashRes (Hashwert wird unter HashRes gespeichert,      */
/*                              Laenge 20 bytes)                              */
/*  Rueckgabe:  -                                                             */
/*                                                                            */
/******************************************************************************/
void __FLINT_API
ripefinish (UCHAR *HashRes, RMDSTAT *hws, UCHAR *clear, ULONG length)
{
  unsigned i;

  /* Anzahl Bytes in vollstaendigen 512-Bit-Bloecken */
  ULONG blength = (length >> 6) << 6;

  /* Verarbeitung der vollstaendigen Bloecke in clear */
  ripehash (hws, clear, blength);

  /* Uebertrage Laenge der gesamten Nachricht in hws->total */
  ADDC (hws->total,length - blength);

  /* Verarbeitung des letzten unvollstaendigen Blocks, Padding und Laenge */
  digest (hws->stomach, clear + blength, hws->total);

  for (i = 0; i < (RMDVER >> 3); i += 4)
    {
      *(HashRes+i)   = (UCHAR)hws->stomach[i>>2];
      *(HashRes+i+1) = (UCHAR)(hws->stomach[i>>2] >>  8); /*lint !e661 !e662 */
      *(HashRes+i+2) = (UCHAR)(hws->stomach[i>>2] >> 16); /*lint !e661 !e662 */
      *(HashRes+i+3) = (UCHAR)(hws->stomach[i>>2] >> 24); /*lint !e661 !e662 */
    }

  return;
}


/******************************************************************************/
/* RIPEMD-160 Kernfunktionen                                                  */
/******************************************************************************/

/* Auswahl der Message-Worte */
static const int
r1[] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
         7,  4, 13,  1, 10,  6, 15,  3, 12,  0,  9,  5,  2, 14, 11,  8,
         3, 10, 14,  4,  9, 15,  8,  1,  2,  7,  0,  6, 13, 11,  5, 12,
         1,  9, 11, 10,  0,  8, 12,  4, 13,  3,  7, 15, 14,  5,  6,  2,
         4,  0,  5,  9,  7, 12,  2, 10, 14,  1,  3,  8, 11,  6, 15, 13};

/* ... fuer parallele Runden */
static const int
r2[] = { 5, 14,  7,  0,  9,  2, 11,  4, 13,  6, 15,  8,  1, 10,  3, 12,
         6, 11,  3,  7,  0, 13,  5, 10, 14, 15,  8, 12,  4,  9,  1,  2,
        15,  5,  1,  3,  7, 14,  6,  9, 11,  8, 12,  2, 10,  0,  4, 13,
         8,  6,  4,  1,  3, 11, 15,  0,  5, 12,  2, 13,  9,  7, 10, 14,
        12, 15, 10,  4,  1,  5,  8,  7,  6,  2, 13, 14,  0,  3,  9, 11};

/* Laengen der Rotationen */
static const int
s1[] = {11, 14, 15, 12,  5,  8,  7,  9, 11, 13, 14, 15,  6,  7,  9,  8,
         7,  6,  8, 13, 11,  9,  7, 15,  7, 12, 15,  9, 11,  7, 13, 12,
        11, 13,  6,  7, 14,  9, 13, 15, 14,  8, 13,  6,  5, 12,  7,  5,
        11, 12, 14, 15, 14, 15,  9,  8,  9, 14,  5,  6,  8,  6,  5, 12,
         9, 15,  5, 11,  6,  8, 13, 12,  5, 12, 13, 14, 11,  8,  5,  6};

/* ... fuer parallele Runden */
static const int
s2[] = { 8,  9,  9, 11, 13, 15, 15,  5,  7,  7,  8, 11, 14, 14, 12,  6,
         9, 13, 15,  7, 12,  8,  9, 11,  7,  7, 12,  7,  6, 15, 13, 11,
         9,  7, 15, 11,  8,  6,  6, 14, 12, 13,  5, 14, 13, 13,  7,  5,
        15,  5,  8, 11, 14, 14,  6, 14,  6,  9, 12,  9, 12,  5, 15,  8,
         8,  5, 12,  9, 12,  5, 14,  6,  8, 13,  6,  5, 15, 13, 11, 11};


static void
appetize (ULONG *stomach)
{
  stomach[0] = 0x67452301UL;
  stomach[1] = 0xefcdab89UL;
  stomach[2] = 0x98badcfeUL;
  stomach[3] = 0x10325476UL;
  stomach[4] = 0xc3d2e1f0UL;

  return;
}


static void
swallow (ULONG *stomach, ULONG *ULBlock)
{
  int round, rol;
  ULONG x;

  ULONG a1 = stomach[0];
  ULONG b1 = stomach[1];
  ULONG c1 = stomach[2];
  ULONG d1 = stomach[3];
  ULONG e1 = stomach[4];

  ULONG a2 = stomach[0];
  ULONG b2 = stomach[1];
  ULONG c2 = stomach[2];
  ULONG d2 = stomach[3];
  ULONG e2 = stomach[4];

  /*lint -e123 Keine Lint-Warnung "Makros ... defined with arguments" */

  /* Runden/parallele Runden 0-15 */
  for (round = 0; round < 16; round++)
    {
      rol = s1[round];
      x = a1 + (b1 ^ c1 ^ d1) + ULBlock[round];
      CHAIN (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + (b2 ^ (c2 | ~d2)) + ULBlock[r2[round]] + 0x50a28BE6UL;
      CHAIN (a2, b2, c2, d2, e2, x, rol);
    }

  /* Runden/parallele Runden 16-31 */
  for (round = 16; round < 32; round++)
    {
      rol = s1[round];
      x = a1 + (d1 ^ (b1 & (c1 ^ d1))) + ULBlock[r1[round]] + 0x5A827999UL;
      CHAIN (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + (c2 ^ (d2 & (b2 ^ c2))) + ULBlock[r2[round]] + 0x5C4Dd124UL;
      CHAIN (a2, b2, c2, d2, e2, x, rol);
    }

  /* Runden/parallele Runden 32-47 */
  for (round = 32; round < 48; round++)
    {
      rol = s1[round];
      x = a1 + ((b1 | ~c1) ^ d1) + ULBlock[r1[round]] + 0x6ED9EBA1UL;
      CHAIN (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + ((b2 | ~c2) ^ d2) + ULBlock[r2[round]] + 0x6D703EF3UL;
      CHAIN (a2, b2, c2, d2, e2, x, rol);
    }

  /* Runden/parallele Runden 48-63 */
  for (round = 48; round < 64; round++)
    {
      rol = s1[round];
      x = a1 + (c1 ^ (d1 & (b1 ^ c1))) + ULBlock[r1[round]] + 0x8F1BBCDCUL;
      CHAIN (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + (d2 ^ (b2 & (c2 ^ d2))) + ULBlock[r2[round]] + 0x7A6D76E9UL;
      CHAIN (a2, b2, c2, d2, e2, x, rol);
    }

  /* Runden/parallele Runden 64-79 */
  for (round = 64; round < 80; round++)
    {
      rol = s1[round];
      x = a1 + (b1 ^ (c1 | ~d1)) + ULBlock[r1[round]] + 0xA953FD4EUL;
      CHAIN (a1, b1, c1, d1, e1, x, rol);

      rol = s2[round];
      x = a2 + (b2 ^ c2 ^ d2) + ULBlock[r2[round]];
      CHAIN (a2, b2, c2, d2, e2, x, rol);
    }

  /* Ergebnis in stomach */
  d2 += c1 + stomach[1];
  stomach[1] = stomach[2] + d1 + e2;
  stomach[2] = stomach[3] + e1 + a2;
  stomach[3] = stomach[4] + a1 + b2;
  stomach[4] = stomach[0] + b1 + c2;
  stomach[0] = d2;

#ifdef FLINT_SECURE
  /* Ueberschreibe Hilfsvariablen */
  zero_ul (&a1, &b1, &c1, &d1);
  zero_ul (&e1, &a2, &b2, &c2);
  zero_ul (&d2, &e2, &x, &a1);
#endif  

  return;
}


static void
digest (ULONG *stomach, UCHAR *clear, ULONG total[])
{
  ULONG i,j, rest;
  ULONG ULBlock[16];

  memset (ULBlock, 0, sizeof (ULONG) << 4);

  /* Padding-Verfahren zur Erreichung der Blocklaenge von 512 Bit     */
  /* am Beispiel der Nachricht "abc" der Laenge l = 24 Bit:           */
  /* 1. An das Ende der Nachricht wird ein Bit "1" angehaengt.        */
  /* 2. Es werden k "0"-Bits angehaengt, k ist die kleinste nicht-    */
  /*    negative Loesung von l+1+k = 448 mod 512, also im Beispiel    */
  /*    448-1-24 = 423, das sind 7 Bit nach der angehaengten "1" plus */
  /*    13 ULONGs (416 Bit) in den Positionen ULBlock[1]...[13].      */
  /* 3. Die Laenge der Nachricht wird in 64 Bit (2 ULONGs) angehaengt.*/
  /*                                                                  */
  /*    01100001 01100010 01100011 1 000...00 00...011000             */
  /*       "a"      "b"      "c"   1+ 423Bit   64Bit (Laenge)         */

  /* Messagelaenge modulo 64-ULONG-Bloecke (512 Bit) */
  rest = total[0] & 0x3f;

  /* Ganze ULONGs in ULBlock einsortieren */
  for (i = 0; i < (rest >> 2); i++)
    {
      ULBlock[i] = UC2UL (clear);
      clear += 4;
    }

  /* Restliche UCHARs in ULBLock einsortieren; hier ist 0 <= i <= 15 */
  for (j = i << 2; j < rest; j++)
    {
      ULBlock[i] |= (ULONG)*clear++ << ((j & 3) << 3);
    }

  /* 10000000 = 0x80 in ULBlock anhaengen: Mindestens ein Byte ist noch frei */
  ULBlock[i] |= (ULONG)0x80 << ((j & 3) << 3);

  if (rest > 55) /* Kein Platz mehr zum Anhaengen der Laenge (8 Byte), */
    {            /* daher Laenge in naechsten Block                    */
      swallow (stomach, ULBlock);
      memset (ULBlock, 0, sizeof (ULONG) << 4);
    }

  /* Laenge in Bit anhaengen */
  ULBlock[14] = total[0] << 3;
  ULBlock[15] = (total[0] >> 29) | (total[1] << 3);
  swallow (stomach, ULBlock);

#ifdef FLINT_SECURE
  /* Ueberschreibe Hilfsvariablen */
  zero_ul (&ULBlock[0], &ULBlock[1], &ULBlock[2], &ULBlock[3]);
  zero_ul (&ULBlock[4], &ULBlock[5], &ULBlock[6], &ULBlock[7]);
  zero_ul (&ULBlock[8], &ULBlock[9], &ULBlock[10], &ULBlock[11]);
  zero_ul (&ULBlock[12], &ULBlock[13], &ULBlock[14], &ULBlock[15]);
#endif

  return;
}


#ifdef FLINT_SECURE

/******************************************************************************/
/*                                                                            */
/* Ueberschreiben von vier ULONG-Variablen mit 0.                             */
/*                                                                            */
/******************************************************************************/
static void zero_ul (ULONG *a, ULONG *b, ULONG *c, ULONG *d)
{
  *a = 0;
  *b = 0;
  *c = 0;
  *d = 0;
}


/******************************************************************************/
/*                                                                            */
/* Eigene memset-Funktion, macht das gleiche wie memset                       */
/* wird benoetigt, damit Compiler trotz Optimierung Variablen loescht.        */
/*                                                                            */
/******************************************************************************/
static void *local_memset (void *ptr, int val, size_t len)
{
    return memset (ptr, val, len);
}

#endif
