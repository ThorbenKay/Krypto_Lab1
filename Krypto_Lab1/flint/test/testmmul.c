/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testmmul.c   Stand: 10.12.2000                                 */
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


#define MAXTESTLEN CLINTMAXBIT
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)
#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))
#define oddrand_l(n_l,bits) do { rand_l((n_l),(bits)); n_l[1] |= 1; } while (eqz_l(n_l))

static int komm_test (unsigned int nooftests);
static int ass_test (unsigned int nooftests);
static int dist_test (unsigned int nooftests);
static int muladd_test (unsigned int nooftests);
static int ummul_test (unsigned int nooftests);
static int mulmon_test (int);
static int check (CLINT a_l, CLINT b_l, int test, int line);
static void ldzrand_l (CLINT n_l, int bits);


int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  komm_test (1000);
  muladd_test (100);
  ass_test (1000);
  dist_test (1000);
  ummul_test (1000);
  mulmon_test (1000);

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int komm_test (unsigned int nooftests)
{
  unsigned long i = 1;

  /* Test mit 0 */

  setzero_l (r1_l);
  setzero_l (r2_l);
  if (0 == mmul_l (r1_l, r2_l, r1_l, r1_l))
    {
      fprintf (stderr, "Fehler bei mmul_l(): Reduzierung mit 0 nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  printf ("Kommutativitaetstest...\n");

  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  mmul_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  setzero_l (r1_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      mmul_l (r1_l, r2_l, r3_l, r10_l);
      mmul_l (r2_l, r1_l, r4_l, r10_l);
      check (r3_l, nul_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }


  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r1_l, 0);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r2_l, CLINTRNDLN);
      ldzrand_l (r10_l, CLINTMAXBIT / 2);
      mmul_l (r1_l, r2_l, r3_l, r10_l);
      mmul_l (r2_l, r1_l, r4_l, r10_l);
      check (r3_l, nul_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }


  /* Test mit 1 , neutrales Element */

  setone_l (r1_l);
  setone_l (r2_l);
  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  mmul_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, one_l, i++, __LINE__);

  setone_l (r1_l);
  setmax_l (r2_l);
  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  mmul_l (r1_l, r2_l, r3_l, r10_l);
  mmul_l (r2_l, r1_l, r4_l, r10_l);
  mod_l (r2_l, r10_l, r2_l);
  check (r3_l, r2_l, i++, __LINE__);
  check (r4_l, r2_l, i++, __LINE__);


  setone_l (r1_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      mmul_l (r1_l, r2_l, r3_l, r10_l);
      mmul_l (r2_l, r1_l, r4_l, r10_l);
      mod_l (r2_l, r10_l, r2_l);
      check (r3_l, r2_l, i, __LINE__);
      check (r4_l, r2_l, i, __LINE__);
    }


  /* Test mit 1 und fuehrenden Nullen beim anderen Faktor */
  ldzrand_l (r1_l, 1);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      mmul_l (r1_l, r2_l, r3_l, r10_l);
      mmul_l (r2_l, r1_l, r4_l, r10_l);
      mod_l (r2_l, r10_l, r2_l);
      check (r3_l, r2_l, i, __LINE__);
      check (r4_l, r2_l, i, __LINE__);
    }


  /* Akku-Betrieb */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      cpy_l (r2_l, r0_l);
      rand_l (r3_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      mmul_l (r1_l, r3_l, r1_l, r10_l);
      mmul_l (r3_l, r0_l, r0_l, r10_l);
      check (r1_l, r0_l, i, __LINE__);
      mmul_l (r2_l, r3_l, r10_l, r10_l);
      check (r1_l, r10_l, i, __LINE__);
    }


  /* Test mit Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  SETONE_L (r10_l);
  mmul_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i, __LINE__);


  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      mmul_l (r1_l, r2_l, r3_l, r10_l);
      mmul_l (r2_l, r1_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }

  return 0;
}


static int muladd_test (unsigned int nooftests)
{
  unsigned int i = 1, j, n;

  printf ("Test Multiplikation durch wiederholte Addition...\n");

  /* Test mit maximal vielen Stellen */
  rand_l (r1_l, CLINTMAXBIT);
  nzrand_l (r10_l, CLINTMAXBIT); /*lint !e666*/
  do
    {
      n = usrand64_l () % 1001;
    }
  while (n == 0);
  u2clint_l (r2_l, ( USHORT ) n);
  mmul_l (r1_l, r2_l, r3_l, r10_l); /* r3_l = r1_l*r2_l */
  mod_l (r1_l, r10_l, r4_l);
  for (j = 1; j < n; j++)
    {
      madd_l (r4_l, r1_l, r4_l, r10_l); /* r2_l = r1_l + ... + r1_l */
    }                                   /*        ----- n-mal ----  */
  check (r3_l, r4_l, i, __LINE__);

  /* Tests mit zufaelliger Stellenzahl */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      do
        {
          n = usrand64_l () % 1001;
        }
      while (n == 0);
      u2clint_l (r2_l, ( USHORT ) n);
      mmul_l (r1_l, r2_l, r3_l, r10_l); /* r3_l = r1_l*r2_l */
      mod_l (r1_l, r10_l, r4_l);
      for (j = 1; j < n; j++)
        {
          madd_l (r4_l, r1_l, r4_l, r10_l); /* r2_l = r1_l + ... + r1_l */
        }                                   /*        ----- n-mal ----  */

      check (r3_l, r4_l, i, __LINE__);
    }
  return 0;
}


static int ass_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Assoziativitaetstest...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      rand_l (r3_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      mmul_l (r1_l, r2_l, r4_l, r10_l);
      mmul_l (r3_l, r4_l, r5_l, r10_l);

      mmul_l (r2_l, r3_l, r4_l, r10_l);
      mmul_l (r1_l, r4_l, r6_l, r10_l);

      check (r5_l, r6_l, i, __LINE__);
    }
  return 0;
}


static int ummul_test (unsigned int nooftests)
{
  unsigned int i = 1;
  USHORT n;
  CLINT n_l;

  printf ("Test ummul_l()...\n");

  rand_l (r1_l, CLINTRNDLN);
  if (0 == ummul_l (r1_l, 2, r1_l, nul_l))
    {
      fprintf (stderr, "Fehler bei ummul_l(): Reduzierung mit 0 nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  /* Test mit 0 */
  rand_l (r1_l, CLINTRNDLN);
  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  n = 0;
  ummul_l (r1_l, n, r3_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Akkumulator-Test */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      cpy_l (r2_l, r1_l);
      n = usrand64_l ();
      u2clint_l (n_l, n);
      ummul_l (r1_l, n, r1_l, r10_l);
      mmul_l (r2_l, n_l, r3_l, r10_l);
      check (r1_l, r3_l, i, __LINE__);
    }

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      n = usrand64_l ();
      u2clint_l (n_l, n);
      ummul_l (r1_l, n, r3_l, r10_l);
      mmul_l (r1_l, n_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
    }
  return 0;
}


static int dist_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Distributionsstest...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r3_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      madd_l (r0_l, r1_l, r2_l, r10_l);
      mmul_l (r2_l, r3_l, r4_l, r10_l);

      mmul_l (r0_l, r3_l, r5_l, r10_l);
      mmul_l (r1_l, r3_l, r6_l, r10_l);
      madd_l (r5_l, r6_l, r7_l, r10_l);
      check (r4_l, r7_l, i, __LINE__);
    }
  return 0;
}


/* Test der Montgomery-Multiplikation */
static int mulmon_test (int nooftests)
{
  USHORT logB_r, nprime, i = 0;
  clint *rptr_l;

  printf ("Test mulmon_l()...\n");

  if (( rptr_l = ( clint * ) malloc (2 + CLINTMAXBYTE)) == NULL)
    {
      fprintf (stderr, "Fehler bei malloc in Zeile %d\n", __LINE__);
      exit (-1);
    }

  rand_l (r0_l, CLINTMAXBIT);
  rand_l (r2_l, CLINTMAXBIT);

  do
    {
      oddrand_l (r10_l, CLINTMAXBIT);         /* n ungerade */
    }
  while (GE_L (r2_l, r10_l));                 /* n > b */

  logB_r = DIGITS_L (r10_l);
  SETZERO_L (rptr_l);
  setbit_l (rptr_l, logB_r << LDBITPERDGT);

  Assert (DIGITS_L (rptr_l) <= 1 + CLINTMAXDIGIT);

  if (DIGITS_L (rptr_l) > CLINTMAXDIGIT)      /* rptr_l < r10_l <= Nmax */
    {
      mod_l (rptr_l, r10_l, rptr_l);
    }

  Assert (DIGITS_L (rptr_l) <= CLINTMAXDIGIT);

  mmul_l (r0_l, rptr_l, r1_l, r10_l); /* a' = a*r mod n */
  nprime = invmon_l (r10_l);          /* n^2 > a'*b     */

  mulmon_l (r1_l, r2_l, r10_l, nprime, logB_r, r3_l); /* a'x b */

  mmul_l (r0_l, r2_l, r4_l, r10_l);   /* a * b mod n */

  check (r3_l, r4_l, i, __LINE__);


  for (i = 1; i < nooftests; i++)
    {
      rand_l (r0_l, usrand64_l () % CLINTMAXBIT);
      rand_l (r2_l, usrand64_l () % CLINTMAXBIT);

      do
        {
          oddrand_l (r10_l, usrand64_l () % CLINTMAXBIT); /* n ungerade */
        }
      while (GE_L (r2_l, r10_l));                         /* n > b */

      logB_r = DIGITS_L (r10_l);
      SETZERO_L (rptr_l);
      setbit_l (rptr_l, logB_r << LDBITPERDGT);

      Assert (DIGITS_L (rptr_l) <= 1 + CLINTMAXDIGIT);

      if (DIGITS_L (rptr_l) > CLINTMAXDIGIT)      /* rptr_l < r10_l <= MaxB */
        {
          mod_l (rptr_l, r10_l, rptr_l);
        }

      Assert (DIGITS_L (rptr_l) <= CLINTMAXDIGIT + 1);

      mmul_l (r0_l, rptr_l, r1_l, r10_l); /* a' = a*r mod n */
      nprime = invmon_l (r10_l);          /* n^2 > a'*b     */

      mulmon_l (r1_l, r2_l, r10_l, nprime, logB_r, r3_l); /* a'x b */

      mmul_l (r0_l, r2_l, r4_l, r10_l); /* a * b mod n */

      check (r3_l, r4_l, i, __LINE__);
    }

  free (rptr_l);
  return (0);
}


/* Hilfsfunktionen */

static int check (CLINT a_l, CLINT b_l, int test, int line)
{
  if (vcheck_l (a_l))
    {
      fprintf (stderr, "Fehler bei vcheck_l(a_l) in Test%d/Zeile %d\n", test, line);
      fprintf (stderr, "vcheck_l(a_l) == %d\n", vcheck_l (a_l));
      disperr_l ("a_l = ", a_l);
      exit (-1);
    }


  if (!equ_l (a_l, b_l))
    {
      fprintf (stderr, "Fehler bei mmul_l() in Test %d/Zeile %d\n", test, line);
      disperr_l ("a_l = ", a_l);
      disperr_l ("b_l = ", b_l);
      disperr_l ("r1_l = ", r1_l);   /*lint !e666*/
      disperr_l ("r2_l = ", r2_l);   /*lint !e666*/
      disperr_l ("r10_l = ", r10_l); /*lint !e666*/
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


