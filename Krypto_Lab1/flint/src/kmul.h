/******************************************************************************/
/*                                                                            */
/*  Arithmetik und Zahlentheorie-Funktionen fuer grosse Zahlen in C           */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei kmul.h       Stand: 28.05.2000                                 */
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

/* Lese kmul.h nur einmal */

#ifndef __KMULH__
#define __KMULH__

#ifdef  __cplusplus
extern "C" {
#endif


/***********************************************************/
/* Funktions-Prototypen                                    */
/***********************************************************/

#ifndef __FLINT_API
#ifdef FLINT_USEDLL
#define __FLINT_API                   __cdecl
#else
#define __FLINT_API                   /**/
#endif /* FLINT_USEDLL */
#endif /* !defined __FLINT_API */

#if !defined __FLINT_API_A
#if defined __GNUC__ && !defined __cdecl
#define __FLINT_API_A                 /**/
#else
#define __FLINT_API_A                 __cdecl
#endif /* !defined __GNUC__ */
#endif /* !defined __FLINT_API_A */


/* Falls das FLINT/C-Paket unter MS Visual C/C++ als DLL eingesetzt wird,    */
/* muessen Module, die von aussen auf die Daten nul_l, one_l, two_l und      */
/* smallprimes zugreifen, mit -D__FLINT_API_DATA=__declspec(dllimport)       */
/* uebersetzt werden.                                                        */

#ifndef __FLINT_API_DATA
#if (defined _MSC_VER && _MSC_VER >= 11) && defined FLINT_USEDLL
#define __FLINT_API_DATA              __declspec(dllimport)
#else
#define __FLINT_API_DATA              /**/
#endif /* MSC_VER && FLINT_USEDLL */
#endif /* !defined __FLINT_API_DATA */


extern int __FLINT_API ksqr_l (clint *, clint *);
extern int __FLINT_API kmul_l (clint *, clint *, clint *);


#ifdef  __cplusplus
}
#endif

#endif /* defined __KMULH__ */
