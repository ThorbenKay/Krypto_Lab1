/******************************************************************************/
/*                                                                            */
/*  Arithmetik und Zahlentheorie-Funktionen fuer grosse Zahlen in C           */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei kmul.c       Stand: 09.11.2000                                 */
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

#include <memory.h>
#include "flint.h"
#include "kmul.h"

#define NO_ASSERTS

#ifdef FLINT_DEBUG
#undef NO_ASSERTS
#define ASSERT_LOG_AND_QUIT
#include "_assert.h"
#include "_alloc.h"
#ifdef COVERAGE
#include "utclog.h"
#endif
#endif

#ifdef NO_ASSERTS
#define Assert(a) (void)0
#endif


static void kmul (clint *, clint *, int, int, CLINT);
static void ksqr (clint *, int, CLINT);

static void shiftadd (CLINT a_l, CLINT b_l, int l, CLINT s_l);
static void addkar (clint *a_l, clint *b_l, int len_arg, CLINT s_l);

#define MUL_SCHWELLE 40
#define SQR_SCHWELLE 40

CLINTD tmp_l;

/******************************************************************************/
/*                                                                            */
/*  Funktion:  Schnittstellenfunktion fuer Karatsuba-Multiplikation           */
/*  Syntax:    void kmul_l (CLINT a_l, CLINT b_l, CLINT c_l);                 */
/*  Eingabe:   aa_l, bb_l (Faktoren)                                          */
/*  Ausgabe:   p_l (Produkt)                                                  */
/*  Rueckgabe: E_CLINT_OK falls alles O.K.                                    */
/*             E_CLINT_OFL bei Ueberlauf                                      */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
kmul_l (CLINT a_l, CLINT b_l, CLINT p_l)
{
  CLINT aa_l, bb_l;
  CLINTD pp_l;
  int OFL = E_CLINT_OK;

  cpy_l (aa_l, a_l);
  cpy_l (bb_l, b_l);

  kmul (LSDPTR_L(aa_l), LSDPTR_L(bb_l), DIGITS_L (aa_l), DIGITS_L (bb_l), pp_l);

  if (DIGITS_L (pp_l) > (USHORT) CLINTMAXDIGIT)                /* Overflow ? */
    {
      ANDMAX_L (pp_l);                            /* Reduziere modulo Nmax+1 */
      OFL = E_CLINT_OFL;
    }

  cpy_l (p_l, pp_l);

  ZEROCLINT_L (aa_l);
  ZEROCLINT_L (bb_l);
  ZEROCLINTD_L (pp_l);
  ZEROCLINTD_L (tmp_l);
  return OFL;
}


/******************************************************************************/
/*                                                                            */
/*  Funktion:  Schnittstellenfunktion fuer Karatsuba-Quadrierung              */
/*  Syntax:    void ksqr_l (CLINT a_l, CLINT c_l);                            */
/*  Eingabe:   aa_l (Faktor)                                                  */
/*  Ausgabe:   p_l (Quadrat)                                                  */
/*  Rueckgabe: E_CLINT_OK falls alles O.K.                                    */
/*             E_CLINT_OFL bei Ueberlauf                                      */
/*                                                                            */
/******************************************************************************/
int __FLINT_API
ksqr_l (CLINT a_l, CLINT p_l)
{
  CLINT aa_l;
  CLINTD pp_l;
  int OFL = E_CLINT_OK;

  cpy_l (aa_l, a_l);

  ksqr (LSDPTR_L(aa_l), DIGITS_L (aa_l), pp_l);

  if (DIGITS_L (pp_l) > (USHORT) CLINTMAXDIGIT)                /* Overflow ? */
    {
      ANDMAX_L (pp_l);                            /* Reduziere modulo Nmax+1 */
      OFL = E_CLINT_OFL;
    }

  cpy_l (p_l, pp_l);

  ZEROCLINT_L (aa_l);
  ZEROCLINTD_L (pp_l);
  ZEROCLINTD_L (tmp_l);
  return OFL;
}


/******************************************************************************/
/*                                                                            */
/*  Funktion:  Karatsuba-Multiplikation zweier Faktoren a_l und b_l           */
/*             mit 2k Stellen zur Basis B                                     */
/*  Syntax:    void kmul (clint *aptr_l, clint *bptr_l,                       */
/*                        int len_a, int len_b, CLINT p_l);                   */
/*  Eingabe:   aptr_l (Zeiger auf niederstwertige Stelle von a_l)             */
/*             bptr_l (Zeiger auf niederstwertige Stelle von b_l)             */
/*             len_a (Stellenzahl von a_l)                                    */
/*             len_b (Stellenzahl von b_l)                                    */
/*  Ausgabe:   p_l (Produkt)                                                  */
/*  Rueckgabe: -                                                              */
/*                                                                            */
/******************************************************************************/
static void
kmul (clint *aptr_l, clint *bptr_l, int len_a, int len_b, CLINT p_l)
{
  CLINT c01_l, c10_l;
  clint c0_l[CLINTMAXSHORT + 2], c1_l[CLINTMAXSHORT + 2], c2_l[CLINTMAXSHORT + 2];
  clint *a1ptr_l, *b1ptr_l;
  int l2;

  if ((len_a == len_b) && (len_a >= MUL_SCHWELLE) && (0 == (len_a & 1)))
    {
      l2 = len_a/2;
      a1ptr_l = aptr_l + l2;
      b1ptr_l = bptr_l + l2;

      kmul (aptr_l, bptr_l, l2, l2, c0_l);
      kmul (a1ptr_l, b1ptr_l, l2, l2, c1_l);

      addkar (a1ptr_l, aptr_l, l2, c01_l);
      addkar (b1ptr_l, bptr_l, l2, c10_l);

      kmul (LSDPTR_L (c01_l), LSDPTR_L (c10_l), DIGITS_L (c01_l), DIGITS_L (c10_l), c2_l);

      sub (c2_l, c1_l, tmp_l);
      sub (tmp_l, c0_l, c2_l);

      shiftadd (c1_l, c2_l, l2, tmp_l);
      shiftadd (tmp_l, c0_l, l2, p_l);
    }

  else /* Rueckfall auf nichtrekursive Multiplikation */
    {
      memcpy (LSDPTR_L (c1_l), aptr_l, len_a * sizeof (clint));
      memcpy (LSDPTR_L (c2_l), bptr_l, len_b * sizeof (clint));
      SETDIGITS_L (c1_l, len_a);
      SETDIGITS_L (c2_l, len_b);
      mult (c1_l, c2_l, p_l);
    }

  ZEROCLINT_L (c01_l);
  ZEROCLINT_L (c10_l);
  ZEROCLINT_L (c0_l);
  ZEROCLINT_L (c1_l);
  ZEROCLINT_L (c2_l);
}


/******************************************************************************/
/*                                                                            */
/*  Funktion:  Karatsuba-Quadrierung eines Faktors a_l                        */
/*             mit 2k Stellen zur Basis B                                     */
/*  Syntax:    void kmul (clint *aptr_l, int len_a, CLINT p_l);               */
/*  Eingabe:   aptr_l (Zeiger auf niederstwertige Stelle von a_l)             */
/*             len_a (Stellenzahl von a_l)                                    */
/*  Ausgabe:   p_l (Quadrat)                                                  */
/*  Rueckgabe: -                                                              */
/*                                                                            */
/******************************************************************************/
static void
ksqr (clint *aptr_l, int len_a, CLINT p_l)
{
  CLINT c01_l;
  clint c0_l[CLINTMAXSHORT + 2], c1_l[CLINTMAXSHORT + 2], c2_l[CLINTMAXSHORT + 2];
  clint *a1ptr_l;
  int l2;

  if ((len_a >= SQR_SCHWELLE) && (0 == (len_a & 1)))
    {
      l2 = len_a/2;
      a1ptr_l = aptr_l + l2;

      ksqr (aptr_l, l2, c0_l);
      ksqr (a1ptr_l, l2, c1_l);

      addkar (a1ptr_l, aptr_l, l2, c01_l);

      ksqr (LSDPTR_L (c01_l), DIGITS_L (c01_l), c2_l);

      sub (c2_l, c1_l, tmp_l);
      sub (tmp_l, c0_l, c2_l);

      shiftadd (c1_l, c2_l, l2, tmp_l);
      shiftadd (tmp_l, c0_l, l2, p_l);
    }

  else /* Rueckfall auf nichtrekursive Quadrierung */
    {
      memcpy (LSDPTR_L (c1_l), aptr_l, len_a * sizeof (clint));
      SETDIGITS_L (c1_l, len_a);
      sqr (c1_l, p_l);
    }

  ZEROCLINT_L (c01_l);
  ZEROCLINT_L (c0_l);
  ZEROCLINT_L (c1_l);
  ZEROCLINT_L (c2_l);
}


/******************************************************************************/
/*                                                                            */
/*  Funktion:  Addition                                                       */
/*             Uebergabe von Zeigern auf LSD der Summanden                    */
/*  Syntax:    void addkar (clint *a_l, clint *b_l, int dgts, CLINT s_l);     */
/*  Eingabe:   a_l, b_l (Summanden mit gleicher Stellenzahl dgts)             */
/*  Ausgabe:   s_l (Summe)                                                    */
/*  Rueckgabe: -                                                              */
/*                                                                            */
/******************************************************************************/
static void
addkar (clint *a_l, clint *b_l, int dgts, CLINT s_l)
{
  clint *msdptra_l;
  clint *aptr_l, *bptr_l, *sptr_l = LSDPTR_L (s_l);
  ULONG carry = 0L;

  aptr_l = a_l;
  bptr_l = b_l;
  msdptra_l = a_l + dgts - 1;
  SETDIGITS_L (s_l, dgts);

  while (aptr_l <= msdptra_l)
    {
      *sptr_l++ = (USHORT) (carry = (ULONG) * aptr_l++ + (ULONG) * bptr_l++
                            + (ULONG) (USHORT) (carry >> BITPERDGT));
    }

  if (carry & BASE)
    {
      *sptr_l = 1;
      INCDIGITS_L (s_l);
    }
}


/******************************************************************************/
/*                                                                            */
/*  Funktion:  Addition mit Shift des ersten Summanden                        */
/*             Ohne Kontrolle fuehrender Nullen                               */
/*  Syntax:    void shiftadd (CLINT a_l, CLINT b_l, int dgts, CLINT s_l);     */
/*  Eingabe:   a_l, b_l (Summanden)                                           */
/*             dgts (Exponent zur Basis 2)                                    */
/*             Annahme: DIGITS_L (a_l) + dgts >= DIGITS_L (b_l))              */
/*  Ausgabe:   s_l (Summe = (2^dgts)*a_l + b_l)                               */
/*  Rueckgabe: -                                                              */
/*                                                                            */
/******************************************************************************/
static void
shiftadd (CLINT a_l, CLINT b_l, int dgts, CLINT s_l)
{
  clint *msdptra_l, *msdptrb_l;
  clint *aptr_l, *bptr_l, *sptr_l = LSDPTR_L (s_l);
  ULONG carry = 0L;

  aptr_l = LSDPTR_L (a_l);
  bptr_l = LSDPTR_L (b_l);
  msdptra_l = MSDPTR_L (a_l);
  msdptrb_l = MSDPTR_L (b_l);
  SETDIGITS_L (s_l, DIGITS_L (a_l) + dgts);

  while (bptr_l <= MIN (msdptrb_l, b_l + dgts))
    {
      *sptr_l++ = *bptr_l++;
    }

  while (sptr_l <= s_l + dgts)
    {
      *sptr_l++ = 0;
    }

  while (bptr_l <= msdptrb_l)
    {
      *sptr_l++ = (USHORT) (carry = (ULONG) * aptr_l++ + (ULONG) * bptr_l++
                            + (ULONG) (USHORT) (carry >> BITPERDGT));
    }

  while (aptr_l <= msdptra_l)
    {
      *sptr_l++ = (USHORT) (carry = (ULONG) * aptr_l++
                            + (ULONG) (USHORT) (carry >> BITPERDGT));
    }

  if (carry & BASE)
    {
      *sptr_l = 1;
      INCDIGITS_L (s_l);
    }

  RMLDZRS_L (s_l);
}

