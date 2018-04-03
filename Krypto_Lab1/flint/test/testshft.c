/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testshft.c   Stand: 10.12.2000                                 */
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

static int shl_test (unsigned int nooftests);
static int shr_test (unsigned int nooftests);
static int shift_test (unsigned int nooftests);
static int check (CLINT a_l, CLINT b_l, int test, int line);


int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  shl_test (1000);
  shr_test (1000);
  shift_test (1000);

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int shl_test (unsigned int nooftests)
{
  unsigned int i = 1, j, k;

  printf ("Test shl_l()...\n");

  /* Test mit 0 */
  setzero_l (r1_l);
  shl_l (r1_l);
  check (r1_l, nul_l, i++, __LINE__);

  /* Test mit max_l */
  setmax_l (r0_l);
  setmax_l (r1_l);
  if (E_CLINT_OFL != shl_l (r0_l))
    {
      fprintf (stderr, "Fehler bei shl_l(): Ueberlauf nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }
  umul_l (r1_l, 2, r1_l);
  check (r0_l, r1_l, i++, __LINE__);

  /* Test Ueberschreitung einer Wortgrenze */
  u2clint_l (r1_l, BASEMINONE);
  cpy_l (r2_l, r1_l);
  shl_l (r2_l);
  if (ld_l (r2_l) != 17)
    {
      fprintf (stderr, "Fehler bei shl_l(): ld_l(r1_l) != 17 in Zeile %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l);        /*lint !e666*/
      disperr_l ("shl_l(r1_l) = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  mul_l (r1_l, two_l, r1_l);
  check (r2_l, r1_l, i++, __LINE__);

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      k = usrand64_l () % MAX (1, ( CLINTMAXBIT - ( digits_l (r2_l) << 4 ))); /*lint !e666*/
      for (j = 1; j <= k; j++)
        {
          shl_l (r1_l);
          mul_l (r2_l, two_l, r2_l);
        }
      check (r1_l, r2_l, i, __LINE__);
    }
  return 0;
}


static int shr_test (unsigned int nooftests)
{
  unsigned int i = 1, j, k;

  printf ("Test shr_l()...\n");

  /* Test mit 0 */
  setzero_l (r1_l);
  if (E_CLINT_UFL != shr_l (r1_l))
    {
      fprintf (stderr, "Fehler bei shr_l(): Unterlauf nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  /* Test mit 1 */
  setone_l (r1_l);
  shr_l (r1_l);
  check (r1_l, nul_l, i++, __LINE__);

  /* Test Unterschreitung einer Wortgrenze */
  setdigits_l (r1_l, 2);
  r1_l[1] = 0;
  r1_l[2] = 1;
  cpy_l (r2_l, r1_l);
  shr_l (r2_l);
  if (ld_l (r2_l) != 16 && digits_l (r2_l) != 1)
    {
      fprintf (stderr, "Fehler bei shr_l(): ld_l(r1_l) != 16 in Zeile %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l);        /*lint !e666*/
      disperr_l ("shr_l(r1_l) = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  udiv_l (r1_l, 2, r1_l, r0_l);
  check (r2_l, r1_l, i++, __LINE__);

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      k = usrand64_l () % MAX (1, ( CLINTMAXBIT - ( digits_l (r2_l) << 4 ))); /*lint !e666*/
      for (j = 1; j <= k; j++)
        {
          shr_l (r1_l);
          udiv_l (r2_l, 2, r2_l, r0_l);
        }
      check (r1_l, r2_l, i, __LINE__);
    }
  return 0;

}


static int shift_test (unsigned int nooftests)
{
  unsigned int i = 1, j, k;
  int sk;

  printf ("Test shift_l() nach links...\n");

  /* Test mit 0 */
  setzero_l (r1_l);
  shift_l (r1_l, 20);
  check (r1_l, nul_l, i++, __LINE__);

  rand_l (r1_l, CLINTRNDLN);
  cpy_l (r2_l, r1_l);
  shift_l (r1_l, 0);
  check (r1_l, r2_l, i++, __LINE__);

  /* Test mit max_l */
  setmax_l (r0_l);
  setmax_l (r1_l);
  if (E_CLINT_OFL != shift_l (r0_l, 1))
    {
      fprintf (stderr, "Fehler bei shift_l(): Ueberlauf nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }
  r1_l[1] &= (BASEMINONE - 1);
  check (r0_l, r1_l, i++, __LINE__);

  if (E_CLINT_OFL != shift_l (r0_l, CLINTMAXBIT - 1))
    {
      fprintf (stderr, "Fehler bei shift_l(): Ueberlauf nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }
  check (r0_l, nul_l, i++, __LINE__);

  /* Test Ueberschreitung einer Wortgrenze */
  u2clint_l (r1_l, BASEMINONE);
  cpy_l (r2_l, r1_l);
  shift_l (r2_l, 1);
  if (ld_l (r2_l) != 17)
    {
      fprintf (stderr, "Fehler bei shift_l(): ld_l(r1_l) != 17 in Zeile %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l);             /*lint !e666*/
      disperr_l ("shift_l(r1_l, 1) = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  mul_l (r1_l, two_l, r1_l);
  check (r2_l, r1_l, i++, __LINE__);

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      k = usrand64_l () % MAX (1, ( CLINTMAXBIT - ( digits_l (r2_l) << 4 ))); /*lint !e666*/
      shift_l (r1_l, k);
      for (j = 1; j <= k; j++)
        {
          mul_l (r2_l, two_l, r2_l);
        }
      check (r1_l, r2_l, i, __LINE__);
    }

  printf ("Test shift_l() nach rechts...\n");

  /* Test mit 1 */
  setone_l (r1_l);
  shift_l (r1_l, -1);
  check (r1_l, nul_l, i++, __LINE__);

  /* Test mit max_l */
  setmax_l (r0_l);
  setmax_l (r1_l);
  shift_l (r0_l, -1);
  r1_l[CLINTMAXDIGIT] &= (BASEMINONE - BASEDIV2);
  check (r0_l, r1_l, i++, __LINE__);

  shift_l (r0_l, -((int)( CLINTMAXBIT - 1 )));
  r1_l[CLINTMAXDIGIT] &= (BASEMINONE - BASEDIV2);
  check (r0_l, nul_l, i++, __LINE__);

  /* Test Unterschreitung einer Wortgrenze */
  setdigits_l (r1_l, 2);
  r1_l[1] = 0;
  r1_l[2] = 1;
  cpy_l (r2_l, r1_l);
  shift_l (r2_l, -1);
  if (ld_l (r2_l) != 16 && digits_l (r2_l) != 1)
    {
      fprintf (stderr, "Fehler bei shift_l(): ld_l(r1_l) != 16 in Zeile %d\n", __LINE__);
      disperr_l ("r1_l = ", r1_l);              /*lint !e666*/
      disperr_l ("shift_l(r1_l, -1) = ", r2_l); /*lint !e666*/
      exit (-1);
    }
  udiv_l (r1_l, 2, r1_l, r0_l);
  check (r2_l, r1_l, i++, __LINE__);

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      k = usrand64_l () % MAX (1, ( CLINTMAXBIT - ( digits_l (r2_l) << 4 ))); /*lint !e666*/
      sk = ( int ) ( 0 - k );
      shift_l (r1_l, sk);
      for (j = 1; j <= k; j++)
        {
          div_l (r2_l, two_l, r2_l, r0_l);
        }
      check (r1_l, r2_l, i, __LINE__);
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
      fprintf (stderr, "Fehler bei sh?_l() in Zeile %d\n", line);
      disperr_l ("a_l = ", a_l);
      disperr_l ("b_l = ", b_l);
      exit (-1);
    }
  return 0;
}

