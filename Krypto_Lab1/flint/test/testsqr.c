/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testsqr.c    Stand: 10.12.2000                                 */
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

static int neu_test (void);
static int sqrmul_test (unsigned int nooftests);
static int pot_test (unsigned int nooftests);
static int binom_test (unsigned int nooftests);
static int dist_test (unsigned int nooftests);
static int overflow_test (void);
static int check (CLINT a_l, CLINT b_l, int test, int line);
static void ldzrand_l (CLINT n_l, int bits);


int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  neu_test ();
  sqrmul_test (1000);
  pot_test (1000);
  binom_test (1000);
  dist_test (1000);
  overflow_test ();

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int neu_test (void)
{
  /* Test mit 0 */
  setzero_l (r1_l);
  sqr_l (r1_l, r2_l);
  check (r2_l, nul_l, 1, __LINE__);

  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r1_l, 0);
  sqr_l (r1_l, r2_l);
  check (r2_l, nul_l, 2, __LINE__);


  printf ("Neutrales Element...\n");

  /* Test mit 1 , neutrales Element */
  setone_l (r1_l);
  sqr_l (r1_l, r2_l);
  check (r2_l, one_l, 3, __LINE__);

  return 0;
}


static int sqrmul_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test gegen Multiplikation...\n");

  /* Test mit max_l */
  setmax_l (r1_l);
  sqr_l (r1_l, r2_l);
  mul_l (r1_l, r1_l, r3_l);
  check (r3_l, r2_l, 1, __LINE__);

  /* Test gegen Multiplikation, Akkumulator-Betrieb */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r10_l, r1_l);
      sqr_l (r1_l, r1_l);
      mul_l (r10_l, r10_l, r3_l);
      check (r3_l, r1_l, i, __LINE__);
    }
  return 0;
}



static int pot_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Potenzregel-Test...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      sqr_l (r1_l, r3_l);
      sqr_l (r2_l, r4_l);
      mul_l (r3_l, r4_l, r4_l); /* r4_l = r1_l^2 * r2_l^2 */

      mul_l (r1_l, r2_l, r5_l);
      sqr_l (r5_l, r5_l); /* r5_l = (r1_l * r2_l)^2 */

      check (r5_l, r4_l, i, __LINE__);
    }
  return 0;
}



static int binom_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Binomial-Test...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      add_l (r1_l, r2_l, r3_l);
      sqr_l (r3_l, r3_l); /* r3_l = (r1_l + r2_l)^2 */

      sqr_l (r1_l, r4_l);
      sqr_l (r2_l, r5_l);
      mul_l (r1_l, r2_l, r6_l);
      mul_l (r6_l, two_l, r6_l);
      add_l (r4_l, r5_l, r4_l);
      add_l (r4_l, r6_l, r4_l); /* r4_l = r1_l^2 + 2*r1_l*r2_l + r2_l^2 */

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
      add_l (r0_l, r1_l, r2_l);
      mul_l (r0_l, r2_l, r4_l);

      sqr_l (r0_l, r5_l);
      mul_l (r0_l, r1_l, r6_l);
      add_l (r5_l, r6_l, r7_l);
      check (r4_l, r7_l, i, __LINE__);
    }
  return 0;
}


static int overflow_test (void)
{
  printf ("Test Overflow bei sqr_l()...\n");

  setmax_l (r0_l);

  if (E_CLINT_OFL != sqr_l (r0_l, r2_l)) /* max_l * max_l = 1 */
    {
      fprintf (stderr, "Fehler bei sqr_l: Overflow nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (one_l, r2_l))
    {
      fprintf (stderr, "Fehler bei Reduzierung mod 2^CLINTMAXBIT in Zeile %d\n", __LINE__);
      disperr_l ("r2_l = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  return 0;
}


static int check (CLINT a_l, CLINT b_l, int test, int line)
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
      fprintf (stderr, "Fehler bei sqr_l() in Zeile %d\n", line);
      disperr_l ("a_l = ", a_l);
      disperr_l ("b_l = ", b_l);
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


