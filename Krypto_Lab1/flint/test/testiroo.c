/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testiroo.c   Stand: 10.12.2000                                 */
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

static int test_squares (unsigned int nooftests);
static int test_nonsquares (void);
static int test_iroot (unsigned int nooftests);

int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  test_squares (1000);
  test_nonsquares ();
  test_iroot (1000);

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int
test_squares (unsigned int nooftests)
{
  int i;

  printf ("Teste Funktion issqr_l()...\n");
  printf ("Triviale Tests mit kleinen Konstanten\n");

  u2clint_l (r0_l, 0);
  u2clint_l (r1_l, 0);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadrat nicht erkannt bei Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadratwurzel falsch bei Zeile %d\n", __LINE__);
      disperr_l ("Wurzel = ", r0_l); /*lint !e666*/
      disperr_l ("Berechneter Wert = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  u2clint_l (r0_l, 1);
  u2clint_l (r1_l, 1);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadrat nicht erkannt bei Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadratwurzel falsch bei Zeile %d\n", __LINE__);
      disperr_l ("Wurzel = ", r0_l); /*lint !e666*/
      disperr_l ("Berechneter Wert = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  u2clint_l (r0_l, 2);
  u2clint_l (r1_l, 4);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadrat nicht erkannt bei Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadratwurzel falsch bei Zeile %d\n", __LINE__);
      disperr_l ("Wurzel = ", r0_l); /*lint !e666*/
      disperr_l ("Berechneter Wert = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  u2clint_l (r0_l, 4);
  u2clint_l (r1_l, 16);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadrat nicht erkannt bei Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadratwurzel falsch bei Zeile %d\n", __LINE__);
      disperr_l ("Wurzel = ", r0_l); /*lint !e666*/
      disperr_l ("Berechneter Wert = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  u2clint_l (r0_l, 256);
  ul2clint_l (r1_l, 65536);

  if (!issqr_l (r1_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadrat nicht erkannt bei Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r0_l, r2_l))
    {
      fprintf (stderr, "Fehler bei issqr_l():\n");
      fprintf (stderr, "Quadratwurzel falsch bei Zeile %d\n", __LINE__);
      disperr_l ("Wurzel = ", r0_l); /*lint !e666*/
      disperr_l ("Berechneter Wert = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  printf ("Tests mit Quadraten...\n");

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (r0_l, CLINTRNDLN/2);
      sqr_l (r0_l, r1_l);

      if (!issqr_l (r1_l, r2_l))
        {
          fprintf (stderr, "Fehler bei issqr_l():\n");
          fprintf (stderr, "Quadrat nicht erkannt in Test %d bei Zeile %d\n", i, __LINE__);
          exit (-1);
        }

      if (!equ_l (r0_l, r2_l))
        {
          fprintf (stderr, "Fehler bei issqr_l():\n");
          fprintf (stderr, "Quadratwurzel falsch in Test %d bei Zeile %d\n", i, __LINE__);
          disperr_l ("Wurzel = ", r0_l); /*lint !e666*/
          disperr_l ("Berechneter Wert = ", r2_l); /*lint !e666*/
          exit (-1);
        }
    }

  return 0;
}

static int
test_nonsquares (void)
{
  int i;
  unsigned short p;

  printf ("Tests von issqr_l() mit Nicht-Quadraten...\n");

  i = 1;
  p = 2;
  do
    {
      ul2clint_l (r0_l, p);

      if (issqr_l (r0_l, r1_l))
        {
          fprintf (stderr, "Fehler bei issqr_l():\n");
          fprintf (stderr, "Quadrat erkannt in Test %d bei Zeile %d\n", i, __LINE__);
          exit (-1);
        }

      p += smallprimes[i];
    }
  while (++i <= NOOFSMALLPRIMES);

  for (i = 1; i <= 5; i++)
    {
      nzrand_l (r0_l, 256);

      if (ISEVEN_L (r0_l))
        {
          inc_l (r0_l);
        }

      while (!isprime_l (r0_l))
        {
          inc_l (r0_l);
          inc_l (r0_l);
        }

      if (issqr_l (r1_l, r2_l))
        {
          fprintf (stderr, "Fehler bei issqr_l():\n");
          fprintf (stderr, "Quadrat erkannt in Test %d bei Zeile %d\n", i, __LINE__);
          exit (-1);
        }
    }

  return 0;
}


static int
test_iroot (unsigned int nooftests)
{
  int i;

  printf ("Teste Funktion iroot_l()...\n");
  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (r0_l, CLINTRNDLN/2);
      sqr_l (r0_l, r1_l);

      iroot_l (r1_l, r2_l);

      if (!equ_l (r0_l, r2_l))
        {
          fprintf (stderr, "Fehler bei issqr_l():\n");
          fprintf (stderr, "Quadratwurzel falsch in Zeile %d\n", __LINE__);
          disperr_l ("Wurzel = ", r0_l); /*lint !e666*/
          disperr_l ("Berechneter Wert = ", r2_l); /*lint !e666*/
          exit (-1);
        }
    }

  return 0;
}
