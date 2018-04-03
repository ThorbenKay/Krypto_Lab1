/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testripe.c   Stand: 14.06.2001                                 */
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "flint.h"
#include "ripemd.h"

#define TESTDATEI1 "test1.bin"
#define TESTDATEI2 "test2.bin"
#define TESTDATEI3 "test3.bin"

static int check (UCHAR *HashRef, UCHAR *HashRes, int error, int test);


/* Referenzwerte fuer RIPEMD-160 */

UCHAR clear_[] = { 0x9c,0x11,0x85,0xa5,0xc5,0xe9,0xfc,0x54,0x61,0x28,0x08,0x97,0x7e,0xe8,0xf5,0x48,0xb2,0x25,0x8d,0x31};
UCHAR clear_A_9[] = { 0xb0,0xe2,0x0b,0x6e,0x31,0x16,0x64,0x02,0x86,0xed,0x3a,0x87,0xa5,0x71,0x30,0x79,0xb2,0x1f,0x51,0x89};
UCHAR clear_a[] = { 0x0b,0xdc,0x9d,0x2d,0x25,0x6b,0x3e,0xe9,0xda,0xae,0x34,0x7b,0xe6,0xf4,0xdc,0x83,0x5a,0x46,0x7f,0xfe};
UCHAR clear_abc[] = { 0x8e,0xb2,0x08,0xf7,0xe0,0x5d,0x98,0x7a,0x9b,0x04,0x4a,0x8e,0x98,0xc6,0xb0,0x87,0xf1,0x5a,0x0b,0xfc};
UCHAR clear_a_z[] = { 0xf7,0x1c,0x27,0x10,0x9c,0x69,0x2c,0x1b,0x56,0xbb,0xdc,0xeb,0x5b,0x9d,0x28,0x65,0xb3,0x70,0x8d,0xbc};
UCHAR clear_1_0[] = { 0x9b,0x75,0x2e,0x45,0x57,0x3d,0x4b,0x39,0xf4,0xdb,0xd3,0x32,0x3c,0xab,0x82,0xbf,0x63,0x32,0x6b,0xfb};
UCHAR clear_M_a[] = { 0x52,0x78,0x32,0x43,0xc1,0x69,0x7b,0xdb,0xe1,0x6d,0x37,0xf9,0x7f,0x68,0xf0,0x83,0x25,0xdc,0x15,0x28};
UCHAR clear_bin1[] = { 0xea,0xe5,0xe8,0x49,0x85,0x03,0xe8,0x25,0xb2,0xa0,0xa7,0x4f,0x54,0xf2,0xad,0x6f,0x5f,0xb3,0xb1,0xd8};
UCHAR clear_bin2[] = { 0xe1,0x1f,0x59,0xa6,0x96,0x1d,0xde,0xa9,0xec,0x63,0xe7,0x65,0xbb,0x70,0xd3,0x7b,0x44,0x46,0x01,0x6f};
UCHAR clear_bin3[] = { 0x4c,0xb3,0x3d,0xb6,0x14,0x35,0x42,0xd9,0x2c,0x9b,0x6d,0xfc,0xfd,0xa8,0x1b,0x63,0xa9,0x23,0xb2,0x04};

int main ()
{
  int error = 0;
  unsigned i, nbytes;
  RMDSTAT hws;
  UCHAR HashRes[RMDVER>>3];
  UCHAR data[1024];
  FILE *mf;

  printf ("Testmodul %s, compiliert fuer ripemd.c als Modul der FLINT/C-Library\n", __FILE__);

  /* Test #1: Leere Zeichenkette */
  ripemd160 (HashRes, ( UCHAR * ) "", 0);
  check (clear_, HashRes, error, 1);

  /* Test #2: String "a" */
  ripemd160 (HashRes, ( UCHAR * ) "a", 1);
  check (clear_a, HashRes, error, 2);

  /* Test #3: String "abc" */
  ripemd160 (HashRes, ( UCHAR * ) "abc", 3);
  check (clear_abc, HashRes, error, 3);

  /* Test #4: String "a-z" */
  ripemd160 (HashRes, ( UCHAR * ) "abcdefghijklmnopqrstuvwxyz", 26);
  check (clear_a_z, HashRes, error, 4);

  /* Test #5: String "A .. Za .. z0 .. 9" */
  ripemd160 (HashRes, ( UCHAR * ) "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", 62);
  check (clear_A_9, HashRes, error, 5);

  /* Test #6: 8 mal "1234567890" in ripehash() und ripefinish() */
  ripeinit (&hws);
  ripehash (&hws, ( UCHAR * ) "1234567890123456789012345678901234567890123456789012345678901234", 64);
  ripefinish (HashRes, &hws, ( UCHAR * ) "5678901234567890", 16);
  check (clear_1_0, HashRes, error, 6);

  /* Test #7: 1 Million mal 'a' in ripehash() und ripefinish() */
  memset (data, 'a', 1024);
  ripeinit (&hws);
  for (i = 0; i < 976; i++)
    {
      error = ripehash (&hws, data, 1024);
    }
  ripefinish (HashRes, &hws, data, 576);
  check (clear_M_a, HashRes, error, 7);

  /* Test #8: Testdatei 1 */
  printf ("Testfile %s ...\n", TESTDATEI1);
  ripeinit (&hws);

  if (( mf = fopen (TESTDATEI1, "rb")) == NULL)
    {
      fprintf (stderr, "Fehler: %s kann Datei %s nicht oeffnen.\n", __FILE__, TESTDATEI1);
      exit (-1);
    }

  while ((( nbytes = fread (data, 1, 1024, mf)) != 0 ) && ( nbytes % 64 == 0 ))
    {
      if (!error)
        {
          error = ripehash (&hws, data, nbytes);
        }
      else
        {
          ripehash (&hws, data, nbytes);
        }
    }
  ripefinish (HashRes, &hws, data, nbytes);
  check (clear_bin1, HashRes, error, 8);
  fclose (mf);

  /* Testfile "test2.bin" */
  printf ("Testfile %s ...\n", TESTDATEI2);
  ripeinit (&hws);

  if (( mf = fopen (TESTDATEI2, "rb")) == NULL)
    {
      fprintf (stderr, "Fehler: %s kann Datei %s nicht oeffnen.\n", __FILE__, TESTDATEI2);
      exit (1);
    }

  while ((( nbytes = fread (data, 1, 1024, mf)) != 0 ) && ( nbytes % 64 == 0 ))
    {
      if (!error)
        {
          error = ripehash (&hws, data, nbytes);
        }
      else
        {
          ripehash (&hws, data, nbytes);
        }
    }

  ripefinish (HashRes, &hws, data, nbytes);
  check (clear_bin2, HashRes, error, 9);
  fclose (mf);

  /* Testfile "test3.bin" */
  printf ("Testfile %s ...\n", TESTDATEI3);
  ripeinit (&hws);

  if (( mf = fopen (TESTDATEI3, "rb")) == NULL)
    {
      fprintf (stderr, "Fehler: %s kann Datei %s nicht oeffnen.\n", __FILE__, TESTDATEI3);
      exit (1);
    }

  while ((( nbytes = fread (data, 1, 1024, mf)) != 0 ) && ( nbytes % 64 == 0 ))
    {
      if (!error)
        {
          error = ripehash (&hws, data, nbytes);
        }
      else
        {
          ripehash (&hws, data, nbytes);
        }
    }

  ripefinish (HashRes, &hws, data, nbytes);
  check (clear_bin3, HashRes, error, 10);
  fclose (mf);

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

  return 0;
}


static int check (UCHAR *HashRef, UCHAR *HashRes, int error, int test)
{
  int i;
  if (error)
    {
      fprintf (stderr, "Fehler bei RIPEMD in Test %d: Fehlercode %d\n", test, error);
    }

  if (memcmp (HashRef, HashRes, 20))
    {
      fprintf (stderr, "Fehler bei RIPEMD in Test %d: Message Digest falsch\n", test);
      for (i = 0; i < 20; i++)
        {
          fprintf (stderr, "%2x ", HashRes[i] & 0x00ff);
        }
      fprintf (stderr, "\n");
      exit (1);
    }

  fprintf (stderr, "Test #%d O.K.\n", test);

  return 0;
}


