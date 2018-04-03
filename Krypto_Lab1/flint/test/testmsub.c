/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testmsub.c   Stand: 10.12.2000                                 */
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

static int neu_test (unsigned int nooftests);
static int msubadd_test (unsigned int nooftests);
static int ass_test (unsigned int nooftests);
static int umsub_test (unsigned int nooftests);
static int check (CLINT a_l, CLINT b_l, int test, int line);
static void ldzrand_l (CLINT n_l, int bits);

int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  neu_test (1000);
  msubadd_test (1000);
  ass_test (1000);
  umsub_test (1000);

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int neu_test (unsigned int nooftests)
{
  unsigned long i = 1;

  printf ("Test mit 0, neutrales Element...\n");

  /* Test mit 0 */

  setzero_l (r1_l);
  setzero_l (r2_l);
  if (0 == msub_l (r1_l, r2_l, r1_l, r1_l))
    {
      fprintf (stderr, "Fehler bei msub_l(): Reduzierung mit 0 nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  msub_l (r1_l, r2_l, r3_l, r10_l);
  msub_l (r2_l, r1_l, r4_l, r10_l);
  check (r3_l, nul_l, i++, __LINE__);
  check (r4_l, nul_l, i++, __LINE__);

  setmax_l (r1_l);
  setzero_l (r2_l);
  msub_l (r1_l, r2_l, r3_l, r10_l);
  mod_l (r1_l, r10_l, r1_l);
  check (r3_l, r1_l, i++, __LINE__);

  setzero_l (r2_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      msub_l (r1_l, r2_l, r3_l, r10_l);
      mod_l (r1_l, r10_l, r1_l);
      check (r3_l, r1_l, i, __LINE__);
    }


  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r2_l, 0);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      msub_l (r1_l, r2_l, r3_l, r10_l);
      mod_l (r1_l, r10_l, r1_l);
      check (r3_l, r1_l, i, __LINE__);
    }


  /* Test mit Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  rand_l (r2_l, CLINTRNDLN);
  SETONE_L (r10_l);
  msub_l (r1_l, r2_l, r3_l, r10_l);
  check (r3_l, nul_l, i, __LINE__);

  return 0;
}


static int msubadd_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test mit Addition und Subtraktion als inverse Operationen...\n");

  /* Allgemeine Tests, Akku-Betrieb */
  for (i = 1; i <= nooftests / 3; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r0_l, r1_l);
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      msub_l (r1_l, r2_l, r1_l, r10_l);
      madd_l (r1_l, r2_l, r3_l, r10_l);
      mod_l (r0_l, r10_l, r1_l);
      check (r1_l, r3_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests / 3; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      cpy_l (r0_l, r2_l);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      msub_l (r1_l, r2_l, r2_l, r10_l);
      madd_l (r2_l, r0_l, r3_l, r10_l);
      mod_l (r1_l, r10_l, r1_l);
      check (r1_l, r3_l, i, __LINE__);
    }


  for (i = 1; i <= nooftests / 3; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      cpy_l (r0_l, r10_l);
      msub_l (r1_l, r2_l, r10_l, r10_l);
      madd_l (r10_l, r2_l, r3_l, r0_l);
      mod_l (r1_l, r0_l, r1_l);
      check (r3_l, r1_l, i, __LINE__);
    }

  /* Test mit maximal vielen Stellen */
  rand_l (r1_l, CLINTMAXBIT);
  rand_l (r2_l, CLINTMAXBIT);
  nzrand_l (r10_l, CLINTMAXBIT); /*lint !e666*/
  msub_l (r1_l, r2_l, r3_l, r10_l);
  madd_l (r3_l, r2_l, r3_l, r10_l);
  mod_l (r1_l, r10_l, r1_l);
  check (r3_l, r1_l, i, __LINE__);

  /* Allgemeine Tests mit zufaelliger Stellenzahl */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      msub_l (r1_l, r2_l, r3_l, r10_l);
      madd_l (r3_l, r2_l, r3_l, r10_l);
      mod_l (r1_l, r10_l, r1_l);
      check (r3_l, r1_l, i, __LINE__);

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
      nzrand_l (r10_l, CLINTRNDLN);     /*lint !e666*/
      madd_l (r2_l, r3_l, r4_l, r10_l); /* r4_l = r2_l + r3_l mod r10_l */
      madd_l (r4_l, r1_l, r5_l, r10_l); /* r5_l = r1_l + r2_l + r3_l mod r10_l */

      msub_l (r5_l, r3_l, r6_l, r10_l); /* r6_l = r5_l - r3_l mod r10_l */

      /* Nun kommt der eigentliche Test */
      msub_l (r6_l, r2_l, r7_l, r10_l); /* r7_l = (r5_l - r3_l) - r2_l mod r10_l */

      msub_l (r5_l, r4_l, r8_l, r10_l); /* r8_l = r5_l - (r3_l + r2_l) mod r10_l */

      check (r7_l, r8_l, i, __LINE__);
    }
  return 0;
}


static int umsub_test (unsigned int nooftests)
{
  unsigned int i = 1, k;
  USHORT n;
  CLINT n_l;

  printf ("Test umsub_l()...\n");

  rand_l (r1_l, CLINTRNDLN);
  if (0 == umsub_l (r1_l, 2, r1_l, nul_l))
    {
      fprintf (stderr, "Fehler bei umsub_l(): Reduzierung mit 0 nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  /* Test mit 0 */
  rand_l (r1_l, CLINTRNDLN);
  nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
  n = 0;
  umsub_l (r1_l, n, r3_l, r10_l);
  mod_l (r1_l, r10_l, r1_l);
  check (r3_l, r1_l, i++, __LINE__);

  /* Akkumulator-Test */
  for (i = 1; i <= nooftests / 3; i++)
    {
      n = usrand64_l ();
      u2clint_l (n_l, n);

      k = CLINTRNDLN;
      rand_l (r1_l, MAX (( ld_l (n_l) + 1 ), k)); /*lint !e666*/
      nzrand_l (r10_l, CLINTRNDLN);               /*lint !e666*/
      cpy_l (r2_l, r1_l);
      umsub_l (r1_l, n, r1_l, r10_l);
      msub_l (r2_l, n_l, r3_l, r10_l);
      check (r1_l, r3_l, i, __LINE__);
    }


  /* Akkumulator-Test */
  for (i = 1; i <= nooftests / 3; i++)
    {
      n = usrand64_l ();
      u2clint_l (n_l, n);

      k = CLINTRNDLN;
      rand_l (r1_l, MAX (( ld_l (n_l) + 1 ), k)); /*lint !e666*/
      nzrand_l (r10_l, CLINTRNDLN);               /*lint !e666*/
      cpy_l (r0_l, r10_l);
      umsub_l (r1_l, n, r10_l, r10_l);
      msub_l (r1_l, n_l, r3_l, r0_l);
      check (r10_l, r3_l, i, __LINE__);
    }


  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r10_l, CLINTRNDLN); /*lint !e666*/
      n = usrand64_l ();
      u2clint_l (n_l, n);
      umsub_l (r1_l, n, r3_l, r10_l);
      msub_l (r1_l, n_l, r4_l, r10_l);
      check (r3_l, r4_l, i, __LINE__);
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
      fprintf (stderr, "Fehler bei msub_l() in Test Nr %d/Zeile %d\n", test, line);
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


