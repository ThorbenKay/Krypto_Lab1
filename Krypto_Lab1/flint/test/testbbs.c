/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testbbs.c    Stand: 10.12.2000                                 */
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
#define CLINTRNDLN (1 + ulrandBBS_l()% MAXTESTLEN)
#define CLINTRNDLNDIGIT (1 + ulrandBBS_l()% CLINTMAXDIGIT)
#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))


/*lint -esym(666,disperr_l)*/

static int chi_test (void);
static int rand_test (unsigned int nooftests);

float chisquare (ULONG, ULONG);

int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrandBBS_lt ();
  create_reg_l ();
  chi_test ();
  rand_test (1000);

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen :-)\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int chi_test ()
{
  ULONG seed, i, errcount = 0;
  float X;

  printf ("Chi-Quadrat-Test fuer usrandBBS_l()...\n");

  for (i = 0; i < 10; i++)
    {
      seed = ulrandBBS_l ();
      X = chisquare (0x50000, seed);
      if (X >= 66048) /* 2^16 + 2*sqrt(2^16) */
        {
          fprintf (stderr, "Warnung: Chisquare > r + 2*sqrt(r) :-|\n");
          ++errcount;
        }
      else
        {
          if (X <= 65024) /* 2^16 - 2*sqrt(2^16) */
            {
              fprintf (stderr, "Warnung: Chisquare < r - 2*sqrt(r) :-|\n");
              ++errcount;
            }
          else
            {
              fprintf (stderr, "Chisquare-Test Nr. %ld O.K. :-)\n", i + 1);
            }
        }

    }
  if (errcount > 3)
    {
      fprintf (stderr, "Mmm, zu viele Fehler im Chi-Quadrat-Test... :-(\n");
    }
  return errcount;
}


static int f[65536 ];

float chisquare (ULONG sample, ULONG seed)
{
  ULONG i, t;

  ulseedBBS_l (seed);
  for (i = 0; i < 65536; i++)
      f[i] = 0;

  for (i = 0; i < sample; i++)
      f[usrandBBS_l ()] ++;

  for (i = 0, t = 0; i < 65536; i++)
      t += f[i] * f[i];

  return (float) ((( float ) 65536 * ( float ) t / ( float ) sample ) - sample );
}


static int rand_test (unsigned int nooftests)
{
  unsigned i;
  unsigned r;

  printf ("Test randBBS_l ...\n");

  randBBS_l (r0_l, 0);
  if (!eqz_l (r0_l))
    {
      fprintf (stderr, "Fehler bei randBBS_l() in Zeile %d\n", __LINE__);
      disperr_l ("r0_l = ", r0_l);
      exit (-1);
    }

  randBBS_l (r0_l, 1);
  if (!equ_l (r0_l, one_l))
    {
      fprintf (stderr, "Fehler bei randBBS_l() in Zeile %d\n", __LINE__);
      disperr_l ("r0_l = ", r0_l);
      exit (-1);
    }

  for (i = 0; i < nooftests; i++)
    {
      r = ulrandBBS_l () % CLINTMAXDIGIT;
      randBBS_l (r0_l, r);

      if (ld_l (r0_l) != r)
        {
          fprintf (stderr, "Fehler bei randBBS_l() in Zeile %d\n", __LINE__);
          disperr_l ("r0_l = ", r0_l);
          fprintf (stderr, "r = %d\n", r);
          exit (-1);
        }
    }
  return 0;
}


