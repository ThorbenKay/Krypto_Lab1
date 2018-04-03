/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testset.c    Stand: 10.12.2000                                 */
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

static int setbit_test (unsigned int nooftests);
static int check (CLINT a_l, CLINT b_l, int test, int line);


int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  setbit_test (10000);

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int setbit_test (unsigned int nooftests)
{
  unsigned int i = 1;
  USHORT k, l;

  printf ("Tests setbit_(), testbit_l(), clearbit_l()...\n");

  setzero_l (r0_l);
  k = CLINTMAXBIT - 1; /* hoechswertige Bitposition eines CLINT-Objektes */
  if (0 != setbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler setbit_l()!= 0 in Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (0 == setbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler setbit_l()== 0 in Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (!testbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler bei setbit_l(), testbit_l() in Zeile %d\n", __LINE__);
      exit (-1);
    }
  check (r0_l, r0_l, i, __LINE__);

  if (0 == clearbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler: clearbit_l() == 0 in Zeile %d\n", __LINE__);
      exit (-1);
    }
  if (gtz_l (r0_l))
    {
      fprintf (stderr, "Fehler bei clearbit_l() in Zeile %d\n", __LINE__);
      exit (-1);
    }
  check (r0_l, r0_l, i, __LINE__);

  setzero_l (r0_l);
  k = 0;
  setbit_l (r0_l, k);
  if (!testbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler bei setbit_l(), testbit_l() in Zeile %d\n", __LINE__);
      exit (-1);
    }
  check (r0_l, one_l, i, __LINE__);
  if (0 == clearbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler: clearbit_l() == 0 in Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (0 != clearbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler: clearbit_l() != 0 in Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (gtz_l (r0_l))
    {
      fprintf (stderr, "Fehler bei clearbit_l() in Zeile %d\n", __LINE__);
      exit (-1);
    }
  check (r0_l, r0_l, i, __LINE__);

  setzero_l (r0_l);
  k = 1;
  setbit_l (r0_l, k);
  if (!testbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler bei setbit_l(), testbit_l() in Zeile %d\n", __LINE__);
      exit (-1);
    }
  check (r0_l, two_l, i, __LINE__);
  if (0 == clearbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler: clearbit_l() == 0 in Zeile %d\n", __LINE__);
      exit (-1);
    }
  if (0 != clearbit_l (r0_l, k))
    {
      fprintf (stderr, "Fehler: clearbit_l() != 0 in Zeile %d\n", __LINE__);
      exit (-1);
    }
  if (gtz_l (r0_l))
    {
      fprintf (stderr, "Fehler bei clearbit_l() in Zeile %d\n", __LINE__);
      exit (-1);
    }
  check (r0_l, r0_l, i, __LINE__);

  for (i = 1; i <= nooftests; i++)
    {
      setzero_l (r0_l);
      setone_l (r1_l);
      k = usrand64_l () % CLINTMAXBIT;
      setbit_l (r0_l, k);
      if (0 == setbit_l (r0_l, k))
        {
          fprintf (stderr, "Fehler setbit_l()== 0 in Zeile %d\n", __LINE__);
          exit (-1);
        }

      if (0 < k)
        {
          shift_l (r1_l, k);
        }
      if (!testbit_l (r0_l, k))
        {
          fprintf (stderr, "Fehler bei setbit_l(), testbit_l() in Zeile %d\n", __LINE__);
          exit (-1);
        }
      check (r0_l, r1_l, i, __LINE__);
      if (0 == clearbit_l (r0_l, k))
        {
          fprintf (stderr, "Fehler: clearbit_l() == 0 in Zeile %d\n", __LINE__);
          exit (-1);
        }
      if (0 != clearbit_l (r0_l, k))
        {
          fprintf (stderr, "Fehler: clearbit_l() != 0 in Zeile %d\n", __LINE__);
          exit (-1);
        }
      if (gtz_l (r0_l))
        {
          fprintf (stderr, "Fehler bei clearbit_l() in Zeile %d\n", __LINE__);
          exit (-1);
        }
      check (r0_l, r0_l, i, __LINE__);
    }

  for (i = 1; i <= nooftests; i++)
    {
      do
        {
          nzrand_l (r0_l, CLINTRNDLN); /*lint !e666*/
          cpy_l (r1_l, r0_l);
          l = ld_l (r0_l);
          k = usrand64_l () % l;
        }
      while (testbit_l (r0_l, k));
      if (0 != setbit_l (r0_l, k))
        {
          fprintf (stderr, "Fehler setbit_l() != 0 in Zeile %d\n", __LINE__);
          exit (-1);
        }
      if (!testbit_l (r0_l, k))
        {
          fprintf (stderr, "Fehler bei setbit_l(), testbit_l() in Zeile %d\n", __LINE__);
          exit (-1);
        }
      check (r0_l, r0_l, i, __LINE__);
      if (0 == clearbit_l (r0_l, k))
        {
          fprintf (stderr, "Fehler: clearbit_l() == 0 in Zeile %d\n", __LINE__);
          exit (-1);
        }
      if (0 != clearbit_l (r0_l, k))
        {
          fprintf (stderr, "Fehler: clearbit_l() != 0 in Zeile %d\n", __LINE__);
          exit (-1);
        }
      if (!equ_l (r0_l, r1_l))
        {
          fprintf (stderr, "Fehler bei clearbit_l() in Zeile %d\n", __LINE__);
          exit (-1);
        }
      check (r0_l, r0_l, i, __LINE__);
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


