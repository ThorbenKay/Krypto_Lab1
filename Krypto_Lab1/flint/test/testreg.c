/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testreg.c    Stand: 10.12.2000                                 */
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

static int reg_test (void);
static int regs_test (void);

#define MAXTESTLEN CLINTMAXBIT
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)
#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();

  reg_test ();
  regs_test ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}



static int reg_test ()
{
  unsigned int i;
  clint *r_l;
  if (NULL == ( r_l = create_l ()))
    {
      fprintf (stderr, "Fehler bei create_l(): NULL zurueck in Zeile %d\n", __LINE__);
      exit (-1);
    }
  rand_l (r_l, CLINTMAXDIGIT);
  purge_l (r_l);
  for (i = 0; i <= CLINTMAXDIGIT; i++)
    {
      if (r_l[i] != 0)
        {
          fprintf (stderr, "Fehler bei purge_l(): r_l[%d] != 0 in Zeile %d\n", i, __LINE__);
          exit (-1);
        }
    }
  free_l (r_l);
  return 0;
}


static int regs_test ()
{
  unsigned int i;
  CLINT a_l;

#ifndef FLINT_USEDLL
  create_reg_l ();
#endif

  for (i = 0; i < NOOFREGS; i++)
    {
      if (NULL == get_reg_l (i))
        {
          fprintf (stderr, "Fehler bei create_reg_l(): get_reg_l(%d) == NULL in Zeile %d\n", i, __LINE__);
          exit (-1);
        }
    }

  rand_l (r0_l, CLINTMAXDIGIT);
  cpy_l (a_l, r0_l);
  create_reg_l ();
  free_reg_l ();
  if (NULL == r0_l)
    {
      fprintf (stderr, "Fehler bei create_reg_l()/free_reg_l(): get_reg_l(0) == NULL in Zeile %d\n", __LINE__);
      exit (-1);
    }
  if (!equ_l (r0_l, a_l))
    {
      fprintf (stderr, "Fehler bei create_reg_l()/free_reg_l(): r_l != a_l in Zeile %d\n", __LINE__);
      disperr_l ("r0_l =", r0_l); /*lint !e666 */
      exit (-1);
    }

  purge_reg_l (0);
  for (i = 0; i <= CLINTMAXDIGIT; i++)
    {
      if (r0_l[i] != 0)
        {
          fprintf (stderr, "Fehler bei purge_reg_l(): r0_l[%d] != 0 in Zeile %d\n", i, __LINE__);
          exit (-1);
        }
    }

  set_noofregs_l (100);

  purgeall_reg_l ();
  free_reg_l ();
  if (NULL != r0_l)
    {
      fprintf (stderr, "Fehler bei create_reg_l()/free_reg_l(): get_reg_l(0) != NULL in Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (0 != create_reg_l ())
    {
      fprintf (stderr, "Fehler: create_reg_l() != 0 in Zeile %d\n", __LINE__);
      exit (-1);
    }

  for (i = 0; i < 100; i++)
    {
      if (NULL == get_reg_l (i))
        {
          fprintf (stderr, "Fehler bei create_reg_l(): get_reg_l(%d) == NULL in Zeile %d\n", i, __LINE__);
          exit (-1);
        }
    }

  if (get_reg_l (100) != NULL)
    {
      fprintf (stderr, "Fehler bei get_reg_l() get_reg_l(100) != NULL in Zeile %d\n", __LINE__);
      disperr_l ("r0_l =", r0_l); /*lint !e666 */
      exit (-1);
    }

  purgeall_reg_l ();

#ifndef FLINT_USEDLL
  free_reg_l ();
#endif

  return 0;
}

