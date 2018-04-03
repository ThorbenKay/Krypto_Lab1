/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testmul.c    Stand: 22.02.2001                                 */
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

#define oddrand_l(n_l,bits) do { rand_l((n_l),(bits)); n_l[1] |= 1; } while (eqz_l(n_l))
#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))

static int rsaXXX_test (void);
static int komm_test (unsigned int);
static int ass_test (unsigned int);
static int dist_test (unsigned int);
static int muladd_test (unsigned int);
static int umul_test (unsigned int);
static int overflow_test (void);
static int check (CLINT, CLINT, int, int);
static void ldzrand_l (CLINT, int);


int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  rsaXXX_test ();
  komm_test (1000);
  muladd_test (1000);
  ass_test (1000);
  dist_test (1000);
  umul_test (1000);
  overflow_test ();

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int rsaXXX_test (void)
{
  CLINT rsaXXX_l;

  printf ("Teste Faktoren von RSA-130 ...\n");

  str2clint_l (rsaXXX_l, "18070820886874048059516561644059055662781025167694013"
                         "49170127021450056662540244048387341127590812303371781"
                         "887966563182013214880557", 10);
  str2clint_l (r2_l, "39685999459597454290161126162883786067576449112810064"
                     "832555157243", 10);
  str2clint_l (r3_l, "45534498646735972188403686897274408864356301263205069"
                     "600999044599", 10);

  mul_l (r2_l, r3_l, r1_l);
  check (r1_l, rsaXXX_l, 1, __LINE__);


  printf ("Teste Faktoren von RSA-140 ...\n");

  str2clint_l (rsaXXX_l, "21290246318258757547497882016271517497806703963277216"
                         "27823338321538194998405649591136657385302191831678310"
                         "7387995317230889569230873441936471", 10);
  str2clint_l (r2_l, "33987174230284385545301236276138758356339864959695974"
                     "23490929302771479", 10);
  str2clint_l (r3_l, "62642001874012850961516549482644422193020371786235090"
                     "19111660653946049", 10);

  mul_l (r2_l, r3_l, r1_l);
  check (r1_l, rsaXXX_l, 1, __LINE__);


  printf ("Teste Faktoren von RSA-155 ...\n");

  str2clint_l (rsaXXX_l, "10941738641570527421809707322040357612003732945449205"
                         "99091384213147634998428893478471799725789126733249762"
                         "5752899781833797076537244027146743531593354333897", 10);
  str2clint_l (r2_l, "102639592829741105772054196573991675900716567808038066803"
                     "341933521790711307779", 10);
  str2clint_l (r3_l, "106603488380168454820927220360012878679207958575989291522"
                     "270608237193062808643", 10);

  mul_l (r2_l, r3_l, r1_l);
  check (r1_l, rsaXXX_l, 1, __LINE__);


  printf ("Teste Faktoren des Singh-Challenge ...\n");

  str2clint_l (rsaXXX_l, "10742788291266565907178411279942116612663921794753294"
                         "58887781721035546415098012187903383292623528109075067"
                         "2083504941996433143425558334401855808989426892463", 10);
  str2clint_l (r2_l, "128442051653810314916622590289775531989649843239158643682"
                     "16177647043137765477", 10);
  str2clint_l (r3_l, "836391832187606937820650856449710761904520026199724985596"
                     "729108812301394489219", 10);

  mul_l (r2_l, r3_l, r1_l);
  check (r1_l, rsaXXX_l, 1, __LINE__);

  return 0;
}


static int komm_test (unsigned int nooftests)
{
  unsigned long i = 1;

  printf ("Kommutativitaetstest...\n");

  /* Test mit 0 */
  setzero_l (r1_l);
  setzero_l (r2_l);
  mul_l (r1_l, r2_l, r3_l);
  check (r3_l, nul_l, i++, __LINE__);

  setzero_l (r1_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r2_l, CLINTRNDLN);
      mul_l (r1_l, r2_l, r3_l);
      mul_l (r2_l, r1_l, r4_l);
      check (r3_l, nul_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }


  /* Test mit 0 und fuehrenden Nullen */
  ldzrand_l (r1_l, 0);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r2_l, CLINTRNDLN);
      mul_l (r1_l, r2_l, r3_l);
      mul_l (r2_l, r1_l, r4_l);
      check (r3_l, nul_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }


  /* Test mit 1 , neutrales Element */
  setone_l (r1_l);
  setone_l (r2_l);
  mul_l (r1_l, r2_l, r3_l);
  check (r3_l, one_l, i++, __LINE__);

  setone_l (r1_l);
  setmax_l (r2_l);
  mul_l (r1_l, r2_l, r3_l);
  mul_l (r2_l, r1_l, r4_l);
  check (r3_l, r2_l, i++, __LINE__);
  check (r4_l, r2_l, i++, __LINE__);


  setone_l (r1_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r2_l, CLINTRNDLN);
      mul_l (r1_l, r2_l, r3_l);
      mul_l (r2_l, r1_l, r4_l);
      check (r3_l, r2_l, i, __LINE__);
      check (r4_l, r2_l, i, __LINE__);
    }


  /* Test mit 1 und fuehrenden Nullen beim anderen Faktor*/
  ldzrand_l (r1_l, 1);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r2_l, CLINTRNDLN);
      mul_l (r1_l, r2_l, r3_l);
      mul_l (r2_l, r1_l, r4_l);
      check (r3_l, r2_l, i, __LINE__);
      check (r4_l, r2_l, i, __LINE__);
    }

  /* Akkumulator-Test */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      mul_l (r1_l, r1_l, r1_l);
      mul_l (r2_l, r2_l, r3_l);
      check (r1_l, r3_l, i, __LINE__);

      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      cpy_l (r10_l, r1_l);
      mul_l (r1_l, r2_l, r1_l);
      mul_l (r10_l, r2_l, r3_l);
      check (r1_l, r3_l, i, __LINE__);

      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      cpy_l (r10_l, r2_l);
      mul_l (r1_l, r2_l, r2_l);
      mul_l (r1_l, r10_l, r3_l);
      check (r2_l, r3_l, i, __LINE__);
    }

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      rand_l (r2_l, CLINTRNDLN);
      mul_l (r1_l, r2_l, r3_l);
      mul_l (r2_l, r1_l, r4_l);
      check (r3_l, r4_l, i, __LINE__);
    }
  return 0;
}


static int muladd_test (unsigned int nooftests)
{
  unsigned int i, j, n;

  printf ("Test Multiplikation durch wiederholte Addition...\n");

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      do
        {
          n = usrand64_l () % 1001;
        }
      while (n == 0);
      u2clint_l (r2_l, ( USHORT ) n);
      mul_l (r1_l, r2_l, r3_l); /* r3_l = r1_l*r2_l */
      cpy_l (r2_l, r1_l);

      for (j = 1; j < n; j++)
        {
          add_l (r2_l, r1_l, r2_l); /* r2_l = r1_l + ... + r1_l */
        }                           /*        ----- n-mal ----- */

      check (r2_l, r3_l, i, __LINE__);
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
      mul_l (r1_l, r2_l, r4_l);
      mul_l (r3_l, r4_l, r5_l);

      mul_l (r2_l, r3_l, r4_l);
      mul_l (r1_l, r4_l, r6_l);

      check (r5_l, r6_l, i, __LINE__);
    }
  return 0;
}


static int umul_test (unsigned int nooftests)
{
  unsigned int i = 1;
  USHORT n;
  CLINT n_l;

  printf ("Test umul_l()...\n");

  /* Test mit 0 */
  rand_l (r1_l, CLINTRNDLN);
  n = 0;
  umul_l (r1_l, n, r3_l);
  check (r3_l, nul_l, i++, __LINE__);

  /* Akkumulator-Test */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      n = usrand64_l ();
      u2clint_l (n_l, n);
      umul_l (r1_l, n, r1_l);
      mul_l (r2_l, n_l, r3_l);
      check (r1_l, r3_l, i, __LINE__);
    }

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      n = usrand64_l ();
      u2clint_l (n_l, n);
      umul_l (r1_l, n, r3_l);
      mul_l (r1_l, n_l, r4_l);
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
      add_l (r0_l, r1_l, r2_l);
      mul_l (r2_l, r3_l, r4_l);

      mul_l (r0_l, r3_l, r5_l);
      mul_l (r1_l, r3_l, r6_l);
      add_l (r5_l, r6_l, r7_l);
      check (r4_l, r7_l, i, __LINE__);
    }
  return 0;
}


static int overflow_test (void)
{
  printf ("Test Overflow bei mul_l()...\n");

  setmax_l (r0_l);
  sub_l (r0_l, one_l, r1_l);

  if (E_CLINT_OFL != mul_l (r0_l, two_l, r2_l)) /* max_l * 2 = max_l - 1 */
    {
      fprintf (stderr, "Fehler bei mul_l: Overflow nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  if (!equ_l (r1_l, r2_l))
    {
      fprintf (stderr, "Fehler bei Reduzierung mod 2^CLINTMAXBIT in Zeile %d\n", __LINE__);
      disperr_l ("r2_l = ", r2_l); /*lint !e666*/
      exit (-1);
    }


  if (E_CLINT_OFL != mul_l (r0_l, r0_l, r2_l)) /* max_l * max_l = 1 */
    {
      fprintf (stderr, "Fehler bei mul_l: Overflow nicht erkannt in Zeile %d\n", __LINE__);
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



/* Hilfsfunktionen */

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
      fprintf (stderr, "Fehler bei mul_l() in Test %d bei Zeile %d\n", test, line);
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

