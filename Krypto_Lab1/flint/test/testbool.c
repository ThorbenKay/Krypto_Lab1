/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testbool.c   Stand: 10.12.2000                                 */
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

static int and_test (unsigned int nooftests);
static int or_test (unsigned int nooftests);
static int xor_test (unsigned int nooftests);

static int check (CLINT a_l, CLINT b_l, int test, int line);

/*lint -esym(666,swap_l)*/

int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  and_test (1000);
  or_test (1000);
  xor_test (1000);

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int xor_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Tests xor_l()...\n");

  rand_l (r1_l, CLINTRNDLN);
  xor_l (nul_l, r1_l, r2_l);
  check (r2_l, r1_l, i, __LINE__);

  xor_l (r1_l, nul_l, r2_l);
  check (r2_l, r1_l, i, __LINE__);

  SETONE_L (r0_l);
  rand_l (r1_l, CLINTRNDLN);
  xor_l (r0_l, r1_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] ^= 1;
      check (r2_l, r1_l, i, __LINE__);
    }
  else
    {
      check (r2_l, one_l, i, __LINE__);
    }

  rand_l (r1_l, CLINTRNDLN);
  xor_l (r1_l, r0_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] ^= 1;
      check (r2_l, r1_l, i, __LINE__);
    }
  else
    {
      check (r2_l, one_l, i, __LINE__);
    }


  setmax_l (r0_l);
  xor_l (r0_l, r0_l, r0_l);
  check (r0_l, nul_l, i, __LINE__);

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      xor_l (r0_l, r0_l, r0_l);
      check (r0_l, nul_l, i, __LINE__);
      xor_l (r1_l, r0_l, r0_l);
      check (r0_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r0_l);
      cpy_l (r3_l, r1_l);
      swap_l (r0_l, r1_l);
      check (r0_l, r3_l, i, __LINE__);
      check (r1_l, r2_l, i, __LINE__);
    }
  return 0;
}


static int or_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Tests or_l()...\n");

  rand_l (r1_l, CLINTRNDLN);
  or_l (nul_l, r1_l, r2_l);
  check (r2_l, r1_l, i, __LINE__);

  or_l (r1_l, nul_l, r2_l);
  check (r2_l, r1_l, i, __LINE__);

  or_l (nul_l, nul_l, r2_l);
  check (r2_l, nul_l, i, __LINE__);

  or_l (one_l, nul_l, r2_l);
  check (r2_l, one_l, i, __LINE__);

  SETONE_L (r0_l);
  rand_l (r1_l, CLINTRNDLN);
  or_l (r0_l, r1_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] |= 1;
      check (r2_l, r1_l, i, __LINE__);
    }
  else
    {
      check (r2_l, one_l, i, __LINE__);
    }

  rand_l (r1_l, CLINTRNDLN);
  or_l (r1_l, r0_l, r2_l);
  if (GTZ_L (r1_l))
    {
      r1_l[1] |= 1;
      check (r2_l, r1_l, i, __LINE__);
    }
  else
    {
      check (r2_l, one_l, i, __LINE__);
    }

  setmax_l (r0_l);
  setmax_l (r1_l);
  or_l (r0_l, r0_l, r0_l);
  check (r0_l, r1_l, i, __LINE__);

  for (i = 1; i <= nooftests; i++)
    {
      setmax_l (r0_l);
      rand_l (r1_l, CLINTRNDLN);
      or_l (r0_l, r1_l, r1_l);
      check (r1_l, r0_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      or_l (r0_l, r0_l, r0_l);
      check (r0_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      setzero_l (r2_l);
      or_l (r0_l, r2_l, r0_l);
      check (r0_l, r1_l, i, __LINE__);
    }

  return 0;
}


static int and_test (unsigned int nooftests)
{
  unsigned int i = 1;

  printf ("Tests and_l()...\n");

  rand_l (r1_l, CLINTRNDLN);
  and_l (nul_l, r1_l, r2_l);
  check (r2_l, nul_l, i, __LINE__);

  and_l (r1_l, nul_l, r2_l);
  check (r2_l, nul_l, i, __LINE__);

  SETONE_L (r0_l);
  rand_l (r1_l, CLINTRNDLN);
  and_l (r0_l, r1_l, r2_l);
  if (GTZ_L (r1_l)) 
    {
      r1_l[1] &= 1;
      SETDIGITS_L (r1_l, 1);
    }
  check (r2_l, r1_l, i, __LINE__);

  rand_l (r1_l, CLINTRNDLN);
  and_l (r1_l, r0_l, r2_l);
  if (GTZ_L (r1_l)) 
    {
      r1_l[1] &= 1;
      SETDIGITS_L (r1_l, 1);
    }
  check (r2_l, r1_l, i, __LINE__);


  setmax_l (r0_l);
  setmax_l (r1_l);
  and_l (r0_l, r0_l, r0_l);
  check (r0_l, r1_l, i, __LINE__);

  for (i = 1; i <= nooftests; i++)
    {
      setmax_l (r0_l);
      rand_l (r1_l, CLINTRNDLN);
      and_l (r0_l, r1_l, r0_l);
      check (r1_l, r0_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      and_l (r0_l, r0_l, r0_l);
      check (r0_l, r1_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      setzero_l (r2_l);
      and_l (r0_l, r2_l, r0_l);
      check (r0_l, nul_l, i, __LINE__);
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
      fprintf (stderr, "Fehler in Test bei Zeile %d\n", line);
      disperr_l ("a_l = ", a_l);
      disperr_l ("b_l = ", b_l);
      exit (-1);
    }
  return 0;
}



