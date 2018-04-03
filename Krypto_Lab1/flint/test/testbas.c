/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testbas.c    Stand: 10.12.2000                                 */
/*                                                                            */
/*  Copyright (C) 1998-2001 by Michael Welschenbach                           */
/*  Copyright (C) 1998-2001 by Springer-Verlag Berlin, Heidelberg             */
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
#include <stdlib.h>
#include <time.h>
#include "flint.h"

#define NO_ASSERTS

#ifdef FLINT_DEBUG
#undef NO_ASSERTS
#define ASSERT_LOG_AND_QUIT
#include "_alloc.h"
#endif

#include "_assert.h"


static void ldzrand_l (CLINT n_l, int bits);

static int vcheck_test (unsigned int nooftests);

static int ld_test (unsigned int nooftests);

static int cpy_test (unsigned int nooftests);
static int check_cpy (CLINT a_l, CLINT b_l, int test, int line);

static int equ_test (unsigned int nooftests);
static int check_equ (CLINT a_l, CLINT b_l, int eq, int i, int line);

static int mequ_test (unsigned int nooftests);
static int check_mequ (CLINT a_l, CLINT b_l, CLINT m_l, int eq, int i, int line);

static int cmp_test (unsigned int nooftests);
static int check_cmp (CLINT a_l, CLINT b_l, int eq, int i, int line);

static int str_test (unsigned int nooftests);
static int check_str (CLINT a_l, CLINT b_l, int i, int line);

static int byte_test (unsigned int nooftests);
static int check_byte (CLINT a_l, CLINT b_l, int i, int line);

static int ul2_test (unsigned int nooftests);
static int check_ul2 (CLINT a_l, clintd n, int islong, int line);

static int setmax_test (void);


#define MAXTESTLEN CLINTMAXBIT
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)
#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());

  Assert (sizeof (USHORT) == 2);
  Assert (sizeof (ULONG) == 4);
  Assert (sizeof (CLINT) == CLINTMAXBYTE);
  Assert (sizeof (CLINTD) == 2 + (CLINTMAXDIGIT << 2));
  Assert (sizeof (CLINTQ) == 2 + (CLINTMAXDIGIT << 3));

  initrand64_lt ();
  create_reg_l ();

  vcheck_test (1000);
  cpy_test (1000);
  ld_test (1000);
  equ_test (1000);
  mequ_test (1000);
  cmp_test (1000);
  ul2_test (2000);
  str_test (100);
  byte_test (1000);
  setmax_test ();

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int vcheck_test (unsigned int nooftests)
{
  unsigned int i;
  clint r_l[CLINTMAXDIGIT << 1 ];

  printf ("Test vcheck_l()... \n");

  ldzrand_l (r_l, CLINTMAXBIT);
  setdigits_l (r_l, CLINTMAXDIGIT + 1);
  r_l[digits_l (r_l)] = 1;
  if (vcheck_l (r_l) != E_VCHECK_OFL)
    {
      fprintf (stderr, "Fehler bei vcheck_l() in Zeile %d\n", __LINE__);
      fprintf (stderr, "vcheck_l(r_l)!= E_VCHECK_OFL, ld_l(r_l) = %d\n", ld_l (r_l));
      exit (-1);
    }

  if (vcheck_l (NULL) != E_VCHECK_MEM)
    {
      fprintf (stderr, "Fehler bei vcheck_l() in Zeile %d\n", __LINE__);
      fprintf (stderr, " vcheck_l(NULL) != E_VCHECK_MEM\n");
      exit (-1);
    }

  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r_l, CLINTRNDLN / 2);
      if (vcheck_l (r_l) != E_VCHECK_LDZ)
        {
          fprintf (stderr, "Fehler bei vcheck_l() in Test %d/Zeile %d\n", i, __LINE__);
          fprintf (stderr, " vcheck_l(r_l) != E_VCHECK_LDZ\n");
          disperr_l ("r_l = ", r_l);
          exit (-1);
        }
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r_l, CLINTRNDLN);
      if (vcheck_l (r_l) != 0)
        {
          fprintf (stderr, "Fehler bei vcheck_l() in Zeile %d\n", __LINE__);
          fprintf (stderr, " vcheck_l(r_l) != 0\n");
          disperr_l ("r_l = ", r_l);
          exit (-1);
        }
    }
  return 0;
}


static int cpy_test (unsigned int nooftests)
{
  unsigned int i = 1, j;

  printf ("Test cpy_l()...\n");

  /* Test mit 0 */
  setzero_l (r1_l);
  cpy_l (r2_l, r1_l);
  check_cpy (r2_l, r1_l, i++, __LINE__);


  /* Test mit max_l */
  setmax_l (r1_l);
  cpy_l (r2_l, r1_l);
  check_cpy (r2_l, r1_l, i++, __LINE__);


  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r1_l, CLINTRNDLN);
  cpy_l (r2_l, r1_l);
  check_cpy (r2_l, r1_l, i++, __LINE__);


  /* Test mit einer Stelle und fuehrenden Nullen */
  rand_l (r1_l, ulrand64_l () % ( BITPERDGT + 1 ));
  for (j = digits_l (r1_l) + 1; j <= CLINTMAXDIGIT; j++)
    {
      r1_l[j] = 0;
    }
  cpy_l (r2_l, r1_l);
  check_cpy (r2_l, r1_l, i++, __LINE__);


  /* Test Kopieren eines Operanden auf sich selbst */
  setzero_l (r1_l);
  cpy_l (r1_l, r1_l);
  check_cpy (r1_l, nul_l, i++, __LINE__);

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);

      for (j = 0; j <= digits_l (r1_l); j++)
        {
          r2_l[j] = r1_l[j];
        }

      cpy_l (r1_l, r1_l);
      check_cpy (r1_l, r2_l, i, __LINE__);
    }


  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      check_cpy (r2_l, r1_l, i, __LINE__);
    }
  return 0;
}


static int check_cpy (CLINT dest_l, CLINT src_l, int test, int line)
{
  unsigned int i;
  if (vcheck_l (dest_l))
    {
      fprintf (stderr, "Fehler bei vcheck_l(dest_l) in Zeile %d\n", line);
      fprintf (stderr, "vcheck_l(dest_l) == %d\n", vcheck_l (dest_l));
      disperr_l ("dest_l = ", dest_l);
      exit (-1);
    }


  RMLDZRS_L (src_l);
  if (vcheck_l (dest_l) > 0)
    {
      fprintf (stderr, "Fehler cpy_l: vcheck_l(dest_l) != 0 in Zeile %d\n", line);
      exit (-1);
    }

  if (digits_l (src_l) != digits_l (dest_l))
    {
      fprintf (stderr, "Fehler cpy_l: src_l und dest_l haben unterschiedliche Laenge in Zeile %d\n", line);
      exit (-1);
    }
  for (i = 1; i < digits_l (src_l); i++)
    {
      if (src_l[i] != dest_l[i])
        {
          fprintf (stderr, "Fehler cpy_l: src_l != dest_l in Stelle %d in Zeile %d\n", i, line);
          exit (-1);
        }
    }
  return 0;
}


static int ld_test (unsigned int nooftests)
{
  unsigned int i = 1, k, l;

  printf ("Test ld_l()...\n");

  /* Test mit 0 */
  setzero_l (r1_l);
  if (ld_l (r1_l) != 0)
    {
      fprintf (stderr, "Fehler bei ld_l() in Zeile %d\n", __LINE__);
      fprintf (stderr, " ld_l(r1_l) != 0\n");
      exit (-1);
    }

  /* Test mit max_l */
  setmax_l (r1_l);
  if (ld_l (r1_l) != CLINTMAXBIT)
    {
      fprintf (stderr, "Fehler bei ld_l() in Zeile %d\n", __LINE__);
      fprintf (stderr, " ld_l(r1_l) != %d\n", CLINTMAXBIT);
      exit (-1);
    }

  /* Test mit 0 und fuehrenden Nullen */

  for (i = 1; i < nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      l = ld_l (r1_l);
      k = (( l % BITPERDGT ) > 0 ) ? 1 : 0;

      if ((( l >> LDBITPERDGT ) + k ) != digits_l (r2_l))
        {
          fprintf (stderr, "Fehler bei ld_l() in Zeile %d\n", __LINE__);
          fprintf (stderr, " ld_l(r1_l)/%ld + 1 != digits_l(r2_l)\n", BITPERDGT);
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          disperr_l ("r2_l = ", r2_l); /*lint !e666*/
          exit (-1);
        }
      if (( l != 0 ) && ( r1_l[ ( l >> LDBITPERDGT ) + k] & ( 1 << (( l - 1 ) % BITPERDGT ))) == 0)
        {
          fprintf (stderr, "Fehler bei ld_l() in Zeile %d\n", __LINE__);
          fprintf (stderr, " Angegebenes hoechstes Bit in r1_l nicht gesetzt\n");
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          exit (-1);
        }
      if (( k > 0 ) && (( r1_l[ ( l >> LDBITPERDGT ) + k] >> ( l % BITPERDGT )) > 0 ))
        {
          fprintf (stderr, "Fehler bei ld_l() in Zeile %d\n", __LINE__);
          fprintf (stderr, "ld_l(r1_l) < ld(r1_l) + 1\n");
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          exit (-1);
        }
    }


  /* Allgemeine Tests */

  for (i = 1; i < nooftests; i++)
    {
      nzrand_l (r1_l, CLINTRNDLN); /*lint !e666*/
      l = ld_l (r1_l);
      k = (( l % BITPERDGT ) > 0 ) ? 1 : 0;

      if ((( l >> LDBITPERDGT ) + k ) != digits_l (r1_l))
        {
          fprintf (stderr, "Fehler bei ld_l() in Zeile %d\n", __LINE__);
          fprintf (stderr, " ld_l(r1_l)/%ld + 1 != digits_l(r1_l)\n", BITPERDGT);
          exit (-1);
        }
      if (( r1_l[ ( l >> LDBITPERDGT ) + k] & ( 1 << (( l - 1 ) % BITPERDGT ))) == 0)
        {
          fprintf (stderr, "Fehler bei ld_l() in Zeile %d\n", __LINE__);
          fprintf (stderr, " Angegebenes hoechstes Bit in r1_l nicht gesetzt\n");
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          exit (-1);
        }
      if (( k > 0 ) && (( r1_l[ ( l >> LDBITPERDGT ) + k] >> ( l % BITPERDGT )) > 0 ))
        {
          fprintf (stderr, "Fehler bei ld_l() in Zeile %d\n", __LINE__);
          fprintf (stderr, "ld_l(r1_l) < ld(r1_l) + 1\n");
          disperr_l ("r1_l = ", r1_l); /*lint !e666*/
          exit (-1);
        }
    }
  return 0;
}


static int equ_test (unsigned int nooftests)
{
  unsigned int i = 1, j;

  printf ("Test equ_l()...\n");

  /* Test mit 0 */
  setzero_l (r1_l);
  setzero_l (r2_l);
  check_equ (r1_l, r2_l, 1, i++, __LINE__);


  /* Tests mit max_l */
  setmax_l (r1_l);
  setzero_l (r2_l);
  check_equ (r1_l, r2_l, -1, i++, __LINE__);

  setzero_l (r1_l);
  setmax_l (r2_l);
  check_equ (r1_l, r2_l, -1, i++, __LINE__);

  setmax_l (r1_l);
  setmax_l (r2_l);
  check_equ (r1_l, r2_l, 1, i++, __LINE__);


  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r1_l, 0);
  ldzrand_l (r2_l, 0);
  check_equ (r1_l, r2_l, 1, i++, __LINE__);


  /* Test mit fuehrenden Nullen */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      ZCPY_L (r2_l, r1_l); /*lint !e666 */
      for (j = digits_l (r2_l) + 1; j <= CLINTMAXDIGIT; j++)
        {
          r2_l[j] = 0;
        }
      j = digits_l (r2_l);
      setdigits_l (r2_l, j + ( ulrand64_l () % ( CLINTMAXDIGIT - j + 1 )));
      check_equ (r1_l, r2_l, 1, i, __LINE__);
    }


  /* Allgemeine Tests: Gleichheit */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      check_equ (r1_l, r2_l, 1, i, __LINE__);
    }


  /* Allgemeine Tests: Ungleichheit */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      setbit_l (r2_l, usrand64_l() % CLINTMAXBIT);
      xor_l (r1_l, r2_l, r2_l);
      check_equ (r1_l, r2_l, -1, i, __LINE__);
    }
  return 0;
}


static int check_equ (CLINT a_l, CLINT b_l, int eq, int test, int line)
{
  switch (eq)
    {
      case 1:
        if (!equ_l (a_l, b_l))
          {
            fprintf (stderr, "Fehler bei equ_l(): r1_l != r2_l in Zeile %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;
      case -1:
        if (equ_l (a_l, b_l))
          {
            fprintf (stderr, "Fehler bei equ_l(): r1_l == r2_l in Zeile %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;
      default:
        fprintf (stderr, "Fehler in Testbedingung bei equ_test() in Zeile %d\n", line);
        exit (-1);
    }
  return 0;
}


static int mequ_test (unsigned int nooftests)
{
  unsigned int i = 1, j;

  printf ("Test mequ_l()...\n");

  /* Test mit 0 */
  setzero_l (r1_l);
  setzero_l (r2_l);
  if (E_CLINT_DBZ != mequ_l (one_l, one_l, r1_l))
    {
      fprintf (stderr, "Fehler: Reduzierung mit 0 von mod_l() nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  /* Tests mit max_l */
  setmax_l (r1_l);
  setzero_l (r2_l);
  do
    {
      nzrand_l (r10_l, CLINTRNDLN);  /*lint !e666*/
      mod_l (r1_l, r10_l, r3_l);
    }
  while (EQZ_L (r3_l));

  check_mequ (r1_l, r2_l, r10_l, -1, i++, __LINE__);

  setzero_l (r1_l);
  setmax_l (r2_l);
  do
    {
      nzrand_l (r10_l, CLINTRNDLN);  /*lint !e666*/
      mod_l (r2_l, r10_l, r3_l);
    }
  while (EQZ_L (r3_l));

  check_mequ (r1_l, r2_l, r10_l, -1, i++, __LINE__);

  setmax_l (r1_l);
  setmax_l (r2_l);
  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  check_mequ (r1_l, r2_l, r10_l, 1, i++, __LINE__);


  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r1_l, 0);
  ldzrand_l (r2_l, 0);
  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  check_mequ (r1_l, r2_l, r10_l, 1, i++, __LINE__);


  /* Test mit fuehrenden Nullen */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      cpy_l (r2_l, r1_l);
      for (j = digits_l (r2_l) + 1; j <= CLINTMAXDIGIT; j++)
        {
          r2_l[j] = 0;
        }
      j = digits_l (r2_l);
      setdigits_l (r2_l, j + ( ulrand64_l () % ( CLINTMAXDIGIT - j + 1 )));
      check_mequ (r1_l, r2_l, r10_l, 1, i, __LINE__);
    }


  /* Test Modulus = 1 */
  nzrand_l (r1_l, CLINTRNDLN); /*lint !e666*/
  nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
  check_mequ (r1_l, r2_l, one_l, 1, i, __LINE__);


  /* Allgemeine Tests: Gleichheit */
  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (r1_l, CLINTRNDLN); /*lint !e666*/
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
      add_l (r1_l, r2_l, r3_l);
      check_mequ (r3_l, r2_l, r1_l, 1, i, __LINE__);
      check_mequ (r3_l, r1_l, r2_l, 1, i, __LINE__);
    }


  /* Allgemeine Tests: Ungleichheit */
  for (i = 1; i <= nooftests; i++)
    {
      do
        {
          rand_l (r1_l, CLINTRNDLN);
          rand_l (r2_l, CLINTRNDLN);
          nzrand_l (r3_l, CLINTRNDLN); /*lint !e666*/
          mod_l (r1_l, r3_l, r10_l);
          mod_l (r2_l, r3_l, r11_l);
        }
      while (equ_l (r10_l, r11_l));
      check_mequ (r1_l, r2_l, r3_l, -1, i, __LINE__);
    }
  return 0;
}


static int check_mequ (CLINT a_l, CLINT b_l, CLINT m_l, int eq, int test, int line)
{
  switch (eq)
    {
      case 1:
        if (!mequ_l (a_l, b_l, m_l))
          {
            fprintf (stderr, "Fehler bei mequ_l(): r1_l == r2_l in Zeile %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;

      case -1:
        if (mequ_l (a_l, b_l, m_l))
          {
            fprintf (stderr, "Fehler bei mequ_l(): r1_l != r2_l in Zeile %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;
      default:
        fprintf (stderr, "Fehler in Testbedingung bei mequ_test() in Zeile %d\n", line);
        exit (-1);
    }
  return 0;
}


static int cmp_test (unsigned int nooftests)
{
  unsigned int i = 1, j;

  printf ("Test cmp_l()...\n");

  /* Test mit 0 */
  setzero_l (r1_l);
  setzero_l (r2_l);
  check_cmp (r1_l, r2_l, 1, i++, __LINE__);


  /* Tests mit max_l */
  setmax_l (r1_l);
  setmax_l (r2_l);
  check_cmp (r1_l, r2_l, 1, i++, __LINE__);

  setmax_l (r1_l);
  setzero_l (r2_l);
  check_cmp (r1_l, r2_l, -1, i++, __LINE__);

  setzero_l (r1_l);
  setmax_l (r2_l);
  check_cmp (r1_l, r2_l, -1, i++, __LINE__);


  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r1_l, 0);
  ldzrand_l (r2_l, 0);
  check_cmp (r1_l, r2_l, 1, i++, __LINE__);


  /* Test auf Gleichheit mit fuehrenden Nullen */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      ZCPY_L (r2_l, r1_l); /*lint !e666 */
      for (j = digits_l (r2_l) + 1; j <= CLINTMAXDIGIT; j++)
        {
          r2_l[j] = 0;
        }
      j = digits_l (r2_l);
      setdigits_l (r2_l, j + ( ulrand64_l () % ( CLINTMAXDIGIT - j + 1 )));

      check_cmp (r1_l, r2_l, 1, i, __LINE__);
    }


  /* Test auf Ungleichheit mit fuehrenden Nullen */
  for (i = 1; i <= nooftests; i++)
    {
      do
        {
          ldzrand_l (r1_l, CLINTRNDLN);
          ldzrand_l (r2_l, CLINTRNDLN);
        }
      while (equ_l (r1_l, r2_l));
      check_cmp (r1_l, r2_l, -1, i, __LINE__);
    }

  setzero_l (r1_l);
  setone_l (r2_l);
  check_cmp (r1_l, r2_l, -1, i++, __LINE__);

  setzero_l (r2_l);
  setone_l (r1_l);
  check_cmp (r1_l, r2_l, -1, i++, __LINE__);


  /* Allgemeine Tests: Gleichheit */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      check_cmp (r1_l, r2_l, 1, i, __LINE__);
    }


  /* Allgemeine Tests: Ungleichheit */
  for (i = 1; i <= nooftests; i++)
    {
      do
        {
          rand_l (r1_l, CLINTRNDLN);
          rand_l (r2_l, CLINTRNDLN);
        }
      while (equ_l (r1_l, r2_l));
      check_cmp (r1_l, r2_l, -1, i, __LINE__);
    }
  return 0;
}


static int check_cmp (CLINT a_l, CLINT b_l, int eq, int test, int line)
{
  unsigned int i;
  int v = 0;
  switch (eq)
    {
      case 1:
        if (cmp_l (a_l, b_l))
          {
            fprintf (stderr, "Fehler bei cmp_l(): r1_l != r2_l in Zeile %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        break;

      case -1:
        if (!cmp_l (a_l, b_l))
          {
            fprintf (stderr, "Fehler bei cmp_l(): r1_l == r2_l in Zeile %d\n", line);
            disperr_l ("a_l = ", a_l);
            disperr_l ("b_l = ", b_l);
            exit (-1);
          }
        else
          {
            ZCPY_L (r3_l, a_l); /*lint !e666*/
            ZCPY_L (r4_l, b_l); /*lint !e666*/
            if (( digits_l (r3_l) < digits_l (r4_l)) && ( 1 == cmp_l (a_l, b_l)))
              {
                fprintf (stderr, "Fehler bei cmp_l(): r1_l > r2_l in Zeile %d\n", line);
                disperr_l ("a_l = ", a_l);
                disperr_l ("b_l = ", b_l);
                exit (-1);
              }
            else
              {
                if (( digits_l (r3_l) > digits_l (r4_l)) && ( -1 == cmp_l (a_l, b_l)))
                  {
                    fprintf (stderr, "Fehler bei cmp_l(): r1_l < r2_l in Zeile %d\n", line);
                    disperr_l ("a_l = ", a_l);
                    disperr_l ("b_l = ", b_l);
                    exit (-1);
                  }
                else
                  {
                    if (digits_l (r3_l) == digits_l (r4_l))
                      {
                        i = digits_l (r3_l);
                        while (a_l[i] == b_l[i] && i > 0)
                          {
                            --i;
                          }
                        if (a_l[i] > b_l[i])
                          {
                            v = 1;
                          }
                        else
                          {
                            if (a_l[i] < b_l[i])
                              {
                                v = -1;
                              }
                          }
                        if (v != cmp_l (a_l, b_l)) /*lint !e644*/
                          {
                            fprintf (stderr, "Fehler bei cmp_l(): cmp_l(r1_l, r2_l) != %d in Zeile %d\n", v, line);
                            disperr_l ("a_l = ", a_l);
                            disperr_l ("b_l = ", b_l);
                            exit (-1);
                          }
                      }
                  }
              }
          }
        break;

      default:
        fprintf (stderr, "Fehler in Testbedingung bei cmp_test() in Zeile %d\n", line);
        exit (-1);
    }
  return 0;
}


static int ul2_test (unsigned int nooftests)
{
  unsigned long i;
  clint n1;
  clintd n2;

  printf ("Test u2clint_l(), ul2clint_l()...\n");

  for (i = 1; i <= nooftests; i++)
    {
      n1 = usrand64_l ();
      u2clint_l (r1_l, n1);
      check_ul2 (r1_l, (clintd)n1, 0, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      n2 = ulrand64_l ();
      ul2clint_l (r1_l, n2);
      check_ul2 (r1_l, n2, 1, __LINE__);
    }
  return 0;
}


static int check_ul2 (CLINT n_l, clintd n, int islong, int line)
{
  if (vcheck_l (n_l))
    {
      fprintf (stderr, "Fehler bei vcheck_l(n_l) in Zeile %d\n", line);
      fprintf (stderr, "vcheck_l(n_l) == %d\n", vcheck_l (n_l));
      disperr_l ("n_l = ", n_l);
      exit (-1);
    }

  switch (islong)
    {
      case 0:
        if (( digits_l (n_l) > 0 ) && ( digits_l (n_l) != 1 || n_l[1] != n ))
          {
            fprintf (stderr, "Fehler in u2clint_l() in Zeile %d\nn = %lu\n", line, n);
            disperr_l ("n_l = ", n_l);
            exit (-1);
          }
        break;

      case 1:
        if ((( digits_l (n_l) == 1 ) && ( n_l[1] != ( n & BASEMINONE ))) ||
             (( digits_l (n_l) == 2 ) && (( n_l[1] != ( n & BASEMINONE )) || ( n_l[2] != ( n >> BITPERDGT ) ))))
          {
            fprintf (stderr, "Fehler in ul2clint_l() in Zeile %d\nn = %lu\n", line, n);
            disperr_l ("n_l = ", n_l);
            exit (-1);
          }
        break;

      default:
        break;
    }
  return 0;
}


static int str_test (unsigned int nooftests)
{
  unsigned int i = 1;
  char *ofl;

  printf ("Test str2clint_l(), xclint2str_l()...\n");

  /* Test mit Zahl > max_l */
  if ((ofl = (char*) malloc (sizeof (char) * (CLINTMAXDIGIT * 4 + 2))) == NULL)
    {
      fprintf (stderr, "Fehler bei malloc");
      exit (-1);
    }

/* Erzeuge string "1000...0" mit CLINTMAXDIGIT*BITPERDGT/4 Nullen     */
  ofl[0] = '1';                  
  ofl[CLINTMAXDIGIT * BITPERDGT/4 + 1] = '\0'; 
  for (i = 1; i <= CLINTMAXDIGIT * BITPERDGT/4; i++)
    {
      ofl[i] = '0';
    }

  if (E_CLINT_OFL != str2clint_l (r2_l, ofl, 16))
    {
      fprintf (stderr, "Fehler in str2clint_l bei Zeile %d: Overflow nicht erkannt.\n", __LINE__);
      exit (-1);
    }

  free (ofl);


  /* Test mit 0 */
  setzero_l (r1_l);
  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* Test mit max_l */
  setmax_l (r1_l);
  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r1_l, 0);
  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* Test mit 1 */
  setone_l (r1_l);
  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* Test mit fuehrenden Nullen */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
      check_str (r2_l, r1_l, i, __LINE__);
    }


  /* Test mit Grossbuchstaben */

  str2clint_l (r1_l, "1234567890ABCDEF", 16);
  str2clint_l (r2_l, "1234567890abcdef", 16);
  check_str (r1_l, r2_l, i++, __LINE__);


  /* Test mit Binaerziffern */

  str2clint_l (r1_l, "10101010", 2);
  str2clint_l (r2_l, "aa", 16);
  check_str (r1_l, r2_l, i++, __LINE__);


  /* Allgemeine Tests mit Makro clint2str_l */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, CLINT2STR_L (r1_l, 2), 2);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, CLINT2STR_L (r1_l, 8), 8);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, CLINT2STR_L (r1_l, 10), 10);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, CLINT2STR_L (r1_l, 16), 16);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  /* Allgemeine Tests xclint2str */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 2, 0), 2);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 8, 0), 8);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 10, 0), 10);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 2, 1), 2);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 8, 1), 8);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 10, 1), 10);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      str2clint_l (r2_l, xclint2str_l (r1_l, 16, 1), 16);
      check_str (r2_l, r1_l, i, __LINE__);
    }

  return 0;
}


static int check_str (CLINT a_l, CLINT b_l, int test, int line)
{
  if (vcheck_l (a_l))
    {
      fprintf (stderr, "Fehler bei vcheck_l(a_l) in Zeile %d\n", line);
      fprintf (stderr, "vcheck_l(a_l) == %d\n", vcheck_l (a_l));
      disperr_l ("a_l = ", a_l);
      exit (-1);
    }

  if (!equ_l (a_l, b_l))
    {
      fprintf (stderr, "Fehler bei str2clint_l() oder xclint2str_l() in Zeile %d\n", line);
      fprintf (stderr, "xclint2str_l(a_l, 16, 0) = %s\n", xclint2str_l (a_l, 16, 0));
      exit (-1);
    }
  return 0;
}


static int byte_test (unsigned int nooftests)
{
  unsigned int i = 1;
  UCHAR* ofl;
  UCHAR* N;
  int len;

  printf ("Test byte2clint_l(), clint2byte_l()...\n");

  /* Test mit Zahl > max_l */
  if (( ofl = (UCHAR*) malloc (sizeof (char) * (CLINTMAXBYTE + 2))) == NULL)
    {
      fprintf (stderr, "Fehler bei malloc");
      exit (-1);
    }

  ofl[0] = '1';                         /* Erzeuge Bytevektor "1000...0" mit */
  for (i = 1; i <= CLINTMAXBYTE; i++)   /* CLINTMAXBYTE Nullen               */
    {
      ofl[i] = '0';
    }

  if (E_CLINT_OFL != byte2clint_l (r2_l, ofl, (int)CLINTMAXBYTE + 1))
    {
      fprintf (stderr, "Fehler in byte2clint_l bei Zeile %d: Overflow nicht erkannt.\n", __LINE__);
      exit (-1);
    }


  /* Test mit Byte-Vektor der Laenge 0 */
  N = clint2byte_l (one_l, &len);
  byte2clint_l (r2_l, N, 0);
  check_byte (r2_l, nul_l, i++, __LINE__);


  /* Test mit 0 */
  setzero_l (r1_l);
  N = clint2byte_l (r1_l, &len);
  byte2clint_l (r2_l, N, len);
  check_byte (r2_l, r1_l, i++, __LINE__);


  /* Test Konstante 10945 */
  u2clint_l (r1_l, 10945);
  N = clint2byte_l (r1_l, &len);
  if ((len != 2) || (*N != 0x2A) || (*(N+1) != 0xC1))
    {
      fprintf (stderr, "Fehler in byte2clint_l bei Zeile %d: Konstante nicht korrekt verarbeitet.\n", __LINE__);
      exit (-1);
    }

  ofl[0] = 0x00;  
  ofl[1] = 0x2A;
  ofl[2] = 0xC1;
  byte2clint_l (r2_l, ofl, 3);
  check_byte (r2_l, r1_l, i++, __LINE__);


  /* Test mit max_l */
  setmax_l (r1_l);
  N = clint2byte_l (r1_l, &len);
  byte2clint_l (r2_l, N, len);
  check_byte (r2_l, r1_l, i++, __LINE__);


  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r1_l, 0);
  N = clint2byte_l (r1_l, &len);
  byte2clint_l (r2_l, N, len);
  check_byte (r2_l, r1_l, i++, __LINE__);


  /* Test mit 1 */
  setone_l (r1_l);
  N = clint2byte_l (r1_l, &len);
  byte2clint_l (r2_l, N, len);
  check_byte (r2_l, r1_l, i++, __LINE__);

  str2clint_l (r2_l, xclint2str_l (r1_l, 16, 0), 16);
  check_str (r2_l, r1_l, i++, __LINE__);


  /* Test mit fuehrenden Nullen */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      N = clint2byte_l (r1_l, &len);
      byte2clint_l (r2_l, N, len);
      check_byte (r2_l, r1_l, i++, __LINE__);
    }


  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      N = clint2byte_l (r1_l, &len);
      byte2clint_l (r2_l, N, len);
      check_byte (r2_l, r1_l, i++, __LINE__);
    }

  free (ofl);
  return 0;
}


static int check_byte (CLINT a_l, CLINT b_l, int test, int line)
{
  if (vcheck_l (a_l))
    {
      fprintf (stderr, "Fehler bei vcheck_l(a_l) in Zeile %d\n", line);
      fprintf (stderr, "vcheck_l(a_l) == %d\n", vcheck_l (a_l));
      disperr_l ("a_l = ", a_l);
      exit (-1);
    }

  if (!equ_l (a_l, b_l))
    {
      fprintf (stderr, "Fehler bei byte2clint_l() oder clint2byte_l() in Zeile %d\n", line);
      disperr_l ("a_l = ", a_l);      
      disperr_l ("b_l = ", b_l);
      exit (-1);
    }
  return 0;
}


static int setmax_test (void)
{
  unsigned int i;

  printf ("Test setmax_l()...\n");

  /* Setze r0_l auf den groesstmoeglichen Wert */
  for (i = 1; i <= CLINTMAXDIGIT; i++)
    {
      r0_l[i] = BASEMINONE;
    }
  setdigits_l (r0_l, CLINTMAXDIGIT);

  setmax_l (r1_l);

  if (!equ_l (r0_l, r1_l))
    {
      fprintf (stderr, "Fehler bei setmax_l: r0_l != r1_l in Zeile %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l); /*lint !e666*/
      exit (-1);
    }
  return 0;
}


static void ldzrand_l (CLINT n_l, int bits)
{
  unsigned int i;
  rand_l (n_l, bits);
  if (digits_l (n_l) == CLINTMAXDIGIT)
    {
      decdigits_l (n_l);
    }

  for (i = digits_l (n_l) + 1; i <= CLINTMAXDIGIT; i++)
    {
      n_l[i] = 0;
    }
  i = digits_l (n_l);
  do
    {
      setdigits_l (n_l, digits_l (n_l) + ( ulrand64_l () % ( CLINTMAXDIGIT - i + 1 )));
    }
  while (vcheck_l (n_l) != E_VCHECK_LDZ);
}

