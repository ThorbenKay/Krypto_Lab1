/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testgcd.c    Stand: 28.05.2001                                 */
/*                                                                            */
/*  Copyright (C) 1998-2001 by Michael Welschenbach                           */
/*  Copyright (C) 1998-2001 by Springer Verlag, Heidelberg                    */
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
#include <time.h>
#include <stdlib.h>

#include "flint.h"

#define MAXTESTLEN (CLINTMAXBIT)
#define CLINTRNDLN (1 + ulrand64_l() % MAXTESTLEN)

#define CLINTRNDLNDIGIT (1 + ulrand64_l() % CLINTMAXDIGIT)

#define disperr_l(S,A) fprintf(stderr,"%s%s\n%u bit\n\n",(S),hexstr_l(A),ld_l(A))
#define nzrand_l(n_l,bits) do { rand_l((n_l),(bits)); } while (eqz_l(n_l))

static int trivial_tests (void);
static int three_integers_tests (int);
static int distributive_tests (int);
static int general_tests (int);
static int check (CLINT, CLINT, CLINT, CLINT, CLINT, int, CLINT, int, int, int);
static int check_inv (CLINT, CLINT, CLINT, CLINT, CLINT, int, int);


int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();

  trivial_tests ();
  three_integers_tests (1000);
  distributive_tests (1000);
  general_tests (1000);

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

  return 0;
}


static int
trivial_tests (void)
{
  CLINTD a_l, b_l, c1_l, c2_l, u_l, v_l;
  int i = 1, su, sv;

  printf ("Triviale Tests mit kleinen Konstanten ...\n");

  setzero_l (a_l);
  setone_l (b_l);
  gcd_l (a_l, b_l, c1_l);
  xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
  check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i++);
  inv_l (a_l, b_l, u_l, v_l);
  check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);
  
  setone_l (a_l);
  setone_l (b_l);
  gcd_l (a_l, b_l, c1_l);
  xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
  check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i++);
  inv_l (a_l, b_l, u_l, v_l);
  check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);

  setone_l (a_l);
  setzero_l (b_l);
  gcd_l (a_l, b_l, c1_l);
  xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
  check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i++);
  inv_l (a_l, b_l, u_l, v_l);
  check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);

  settwo_l (a_l);
  settwo_l (b_l);
  gcd_l (a_l, b_l, c1_l);
  xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
  check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i++);
  inv_l (a_l, b_l, u_l, v_l);
  check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);

  return 0;
}


static int
three_integers_tests (int nooftests)
{
  CLINT a_l, b_l, c_l, t1_l, t2_l, abc_l;
  int i;

  printf ("Teste ggT gegen kgV mit drei Argumenten ...\n");

  /* Es gilt ggT (a, b, c) * kgV (ab, ac, bc) = a*b*c */

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (a_l, CLINTRNDLN/6);     /* a */
      nzrand_l (b_l, CLINTRNDLN/6);     /* b */
      nzrand_l (c_l, CLINTRNDLN/6);     /* c */

      mul_l (a_l, b_l, t1_l);           /* a*b   */

      mul_l (t1_l, c_l, abc_l);         /* a*b*c */

      mul_l (a_l, c_l, t2_l);
      lcm_l (t1_l, t2_l, t1_l);         /* kgV (ab, ac)                */

      mul_l (b_l, c_l, t2_l);           /* kgV (kgV (ab, ac), bc)) =   */
      lcm_l (t1_l, t2_l, t1_l);         /* kgV (ab, ac, bc)            */

      gcd_l (a_l, b_l, t2_l);           /* ggT (a, b)                  */
      gcd_l (t2_l, c_l, t2_l);          /* ggT (ggT (a, b), c)         */

      mul_l (t1_l, t2_l, t2_l);         /* kgV (ab, ac, bc)*ggT (a, b, c) */

      if (!equ_l (abc_l, t2_l))         /* = a*b*c                     */
        {
          printf ("Fehler in Test #%d nahe Zeile %d\n", i, __LINE__);
          exit (-1);
        }
    }

  return 0;
}


static int
distributive_tests (int nooftests)
{
  CLINT a_l, b_l, c_l, t1_l, t2_l, t3_l;
  int i;

  printf ("Teste ggT gegen kgV mit \"Distributivgesetzen\" ...\n");

  /* Es gilt kgV(c, ggT(a, b)) = ggT(kgV(a, c), kgV(b, c)) */

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (a_l, CLINTRNDLN/3);     /* a */
      nzrand_l (b_l, CLINTRNDLN/3);     /* b */
      nzrand_l (c_l, CLINTRNDLN/3);     /* c */

      gcd_l (a_l, b_l, t1_l);           /* ggT(a, b)                  */
      lcm_l (c_l, t1_l, t1_l);          /* kgv(c, ggT(a, b))          */

      lcm_l (a_l, c_l, t2_l);
      lcm_l (b_l, c_l, t3_l);

      gcd_l (t2_l, t3_l, t2_l);         /* ggT(kgV(a, c), kgV(b, c))  */

      if (!equ_l (t1_l, t2_l))
        {
          printf ("Fehler in Test #%d nahe Zeile %d\n", i, __LINE__);
          exit (-1);
        }
    }


  /* Es gilt ggT(c, kgV(a, b)) = kgV(ggT(a, c), ggT(b, c)) */

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (a_l, CLINTRNDLN/3);     /* a */
      nzrand_l (b_l, CLINTRNDLN/3);     /* b */
      nzrand_l (c_l, CLINTRNDLN/3);     /* c */

      lcm_l (a_l, b_l, t1_l);           /* kgV (a, b)                  */
      gcd_l (c_l, t1_l, t1_l);          /* ggT(c, kgV(a, b))           */

      gcd_l (a_l, c_l, t2_l);
      gcd_l (b_l, c_l, t3_l);

      lcm_l (t2_l, t3_l, t2_l);         /* kgV(ggT(a, c), ggT(b, c))  */

      if (!equ_l (t1_l, t2_l))
        {
          printf ("Fehler in Test #%d nahe Zeile %d\n", i, __LINE__);
          exit (-1);
        }
    }

  return 0;
}


static int
general_tests (int nooftests)
{
  CLINTD a_l, b_l, c1_l, c2_l, u_l, v_l;
  int i, su, sv;

  printf ("Allgemeine Tests mit zufaelligen Parametern ...\n");

  for (i = 1; i <= nooftests; i++)
    {
      nzrand_l (a_l, CLINTRNDLN);
      nzrand_l (b_l, CLINTRNDLN);
      gcd_l (a_l, b_l, c1_l);
      xgcd_l (a_l, b_l, c2_l, u_l, &su, v_l, &sv);
      check (a_l, b_l, c1_l, c2_l, u_l, su, v_l, sv, __LINE__, i);
      inv_l (a_l, b_l, u_l, v_l);
      check_inv (a_l, b_l, c1_l, u_l, v_l, __LINE__, i++);
    }

  return 0;
}


static int
check (CLINT a_l,
       CLINT b_l,
       CLINT c1_l,
       CLINT c2_l,
       CLINT u_l, int su,
       CLINT v_l, int sv,
       int line,
       int test)
{
  int error = 0;
  CLINT uu_l, vv_l;

  if (!equ_l (c1_l, c2_l))
    {
      error = 1;
      printf ("Fehler bei ggT-Berechnung in Test #%d nahe Zeile %d\n", test, line);
      disp_l ("ggT:", c1_l);
      disp_l ("\nxggT:", c2_l);
    }

  mul_l (u_l, a_l, uu_l);
  mul_l (v_l, b_l, vv_l);
  su = sadd (uu_l, su, vv_l, sv, uu_l);
  if (( su != 1 ) || ( !equ_l (uu_l, c2_l)))
    {
      error = 1;
      printf ("Fehler bei Berechnung der Faktoren u und v in Test #%d nahe Zeile %d\n", test, line);
      printf ("sign_u = %d, sign_v = %d\n", su, sv);
      disp_l ("u = \n", u_l);
      disp_l ("v = \n", v_l);
      disp_l ("Linear kombinierter ggT  = \n", uu_l);
      disp_l ("Referenz-ggT  = \n", c1_l);
    }

  if (error)
      exit (-1);
  else
      return 0;
}


static int
check_inv (CLINT a_l,
           CLINT b_l,
           CLINT c1_l,
           CLINT g_l,
           CLINT i_l,
           int line,
           int test)
{
  int error = 0;
  CLINT n_l;

  if (!equ_l (c1_l, g_l))
    {
      error = 1;
      printf ("Fehler bei ggT-Berechnung mit inv_l() in Test #%d nahe Zeile %d\n", test, line);
      disp_l ("a:", a_l);
      disp_l ("\nb:", b_l);
      disp_l ("\nggT:", c1_l);
      disp_l ("\ninv_l:", g_l);
    }

  if (EQONE_L (g_l) && GT_L (b_l, one_l) )
    {
      mmul_l (a_l, i_l, n_l, b_l);
      if (!EQONE_L (n_l))
        {
          error = 1;
          printf ("Fehler bei Berechnung des Inversen mit inv_l() in Test #%d nahe Zeile %d\n", test, line);
          disp_l ("a:", a_l);
          disp_l ("\nb:", b_l);
          disp_l ("\ni:", i_l);
          disp_l ("\na * i mod b:", n_l);
        }
    }

  if (error)
      exit (-1);
  else
      return 0;
}

