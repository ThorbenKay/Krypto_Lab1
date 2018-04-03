/******************************************************************************/
/*                                                                            */
/*  Software zum Buch "Kryptographie in C und C++"                            */
/*  Autor: Michael Welschenbach                                               */
/*                                                                            */
/*  Quelldatei testdiv.c    Stand: 29.04.2001                                 */
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

static int neu_test (unsigned int nooftests);
static int kleine_div_test (unsigned int nooftests);
static int divsub_test (unsigned int nooftests);
static int divrest_test (unsigned int nooftests);
static int rsaXXX_test (void);
static int S5_test (void);
static int mod_test (unsigned int nooftests);
static int check (CLINT a_l, CLINT b_l, int test, int line);

static void ldzrand_l (CLINT n_l, int bits);

int main ()
{
  printf ("Testmodul %s, compiliert fuer FLINT/C-Library Version %s\n", __FILE__, verstr_l ());
  initrand64_lt ();
  create_reg_l ();

  neu_test (1000);
  kleine_div_test (1000);
  rsaXXX_test ();
  S5_test ();
  divsub_test (1000);
  divrest_test (10000);
  mod_test (1000);

  free_reg_l ();

  printf ("Alle Tests in %s fehlerfrei durchlaufen.\n", __FILE__);

#ifdef FLINT_DEBUG
  MemDumpUnfreed ();
#endif

  return 0;
}


static int neu_test (unsigned int nooftests)
{
  unsigned long i = 1;

  /* Test Division durch 0 */

  rand_l (r1_l, CLINTRNDLN);
  setzero_l (r2_l);
  if (0 == div_l (r1_l, r2_l, r3_l, r4_l))
    {
      printf ("Fehler: Division durch 0 nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }


  /* Test Division 0 durch Wert > 0 */

  setzero_l (r1_l);
  rand_l (r2_l, CLINTRNDLN);
  div_l (r1_l, r2_l, r3_l, r4_l);
  check (r3_l, nul_l, i, __LINE__);
  check (r4_l, nul_l, i, __LINE__);


  printf ("Test mit 1, neutrales Element...\n");

  setone_l (r2_l);
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      div_l (r1_l, r2_l, r3_l, r4_l);
      check (r3_l, r1_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }


  /* Test mit 1 und fuehrenden Nullen */
  ldzrand_l (r2_l, 1);
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      div_l (r1_l, r2_l, r3_l, r4_l);
      check (r3_l, r1_l, i, __LINE__);
      check (r4_l, nul_l, i, __LINE__);
    }
  return 0;
}


static int kleine_div_test (unsigned int nooftests)
{
  unsigned int i;
  clint n;

  printf ("Test Division durch kleine Divisoren...\n");

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      do
        {
          u2clint_l (r2_l, usrand64_l ());
        }
      while (eqz_l (r2_l));
      div_l (r1_l, r2_l, r3_l, r4_l);
      mul_l (r2_l, r3_l, r5_l);
      add_l (r5_l, r4_l, r6_l);
      check (r6_l, r1_l, i, __LINE__);
    }


  /* Tests mit udiv_l() im Akkumulator-Betrieb */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r2_l, r1_l);
      cpy_l (r0_l, r1_l);
      do
        {
          n = usrand64_l ();
        }
      while (0 == n);
      u2clint_l (r3_l, n);

      udiv_l (r1_l, n, r1_l, r4_l);
      mul_l (r3_l, r1_l, r5_l);
      add_l (r5_l, r4_l, r5_l);
      check (r5_l, r0_l, i, __LINE__);

      udiv_l (r2_l, n, r5_l, r2_l);
      mul_l (r3_l, r5_l, r5_l);
      add_l (r5_l, r2_l, r6_l);
      check (r6_l, r0_l, i, __LINE__);
    }


  /* Euklidischer Test  a = b * q1 + r1, b = c * q2 + r2   */
  /*                ==> a = c * q1 * q2 + r3 * q1 + r1     */

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      do
        {
          u2clint_l (r2_l, usrand64_l ());
        }
      while (eqz_l (r2_l));
      do
        {
          u2clint_l (r3_l, usrand64_l ());
        }
      while (eqz_l (r3_l));

      div_l (r1_l, r2_l, r4_l, r5_l); /* r1_l = r2_l * r4_l + r5_l */
      div_l (r2_l, r3_l, r6_l, r7_l); /* r2_l = r3_l * r6_l + r7_l */

      mul_l (r3_l, r6_l, r8_l);
      mul_l (r8_l, r4_l, r8_l);
      mul_l (r7_l, r4_l, r9_l);
      add_l (r8_l, r9_l, r9_l);
      add_l (r9_l, r5_l, r9_l); /* r9_l = r3_l*r6_l*r4_l + r7_l*r4_l + r5_l */

      check (r9_l, r1_l, i, __LINE__);
    }
  return 0;
}


static int divsub_test (unsigned int nooftests)
{
  unsigned int i;

  printf ("Test Division durch wiederholte Subtraktion...\n");

  for (i = 1; i <= nooftests; i++)
    {
      do
        {
          rand_l (r1_l, CLINTRNDLN);
          rand_l (r2_l, ( digits_l (r1_l) * BITPERDGT ) - ( ucrand64_l () % 11 ));
        }
      while (eqz_l (r2_l));

      div_l (r1_l, r2_l, r3_l, r4_l); /* r1_l = r2_l*r3_l + r4_l */

      setzero_l (r5_l);
      while (ge_l (r1_l, r2_l))
        {
          sub_l (r1_l, r2_l, r1_l); /* r1_l = r1_l - r2_l - ... - r2_l */
                                    /*            ----- r5_l-mal ----  */
          add_l (r5_l, one_l, r5_l);
        }

      check (r3_l, r5_l, i, __LINE__);
      check (r4_l, r1_l, i, __LINE__);
    }
  return 0;
}


static int rsaXXX_test (void)
{
  CLINT rsaXXX_l;

  printf ("Teste mit RSA-130 ...\n");

  str2clint_l (rsaXXX_l, "18070820886874048059516561644059055662781025167694013"
                         "49170127021450056662540244048387341127590812303371781"
                         "887966563182013214880557", 10);
  str2clint_l (r2_l, "39685999459597454290161126162883786067576449112810064"
                     "832555157243", 10);
  str2clint_l (r3_l, "45534498646735972188403686897274408864356301263205069"
                     "600999044599", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 1, __LINE__);
  check (r11_l, nul_l, 1, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 2, __LINE__);
  check (r11_l, nul_l, 2, __LINE__);

  printf ("Teste mit RSA-140 ...\n");

  str2clint_l (rsaXXX_l, "21290246318258757547497882016271517497806703963277216"
                         "27823338321538194998405649591136657385302191831678310"
                         "7387995317230889569230873441936471", 10);
  str2clint_l (r2_l, "33987174230284385545301236276138758356339864959695974"
                     "23490929302771479", 10);
  str2clint_l (r3_l, "62642001874012850961516549482644422193020371786235090"
                     "19111660653946049", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 3, __LINE__);
  check (r11_l, nul_l, 3, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 4, __LINE__);
  check (r11_l, nul_l, 4, __LINE__);

  printf ("Teste mit RSA-155 ...\n");

  str2clint_l (rsaXXX_l, "10941738641570527421809707322040357612003732945449205"
                         "99091384213147634998428893478471799725789126733249762"
                         "5752899781833797076537244027146743531593354333897", 10);
  str2clint_l (r2_l, "102639592829741105772054196573991675900716567808038066803"
                     "341933521790711307779", 10);
  str2clint_l (r3_l, "106603488380168454820927220360012878679207958575989291522"
                     "270608237193062808643", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 3, __LINE__);
  check (r11_l, nul_l, 3, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 4, __LINE__);
  check (r11_l, nul_l, 4, __LINE__);

  printf ("Test mit Modulus und Faktoren des Singh-Challenge ...\n");

  str2clint_l (rsaXXX_l, "10742788291266565907178411279942116612663921794753294"
                         "58887781721035546415098012187903383292623528109075067"
                         "2083504941996433143425558334401855808989426892463", 10);
  str2clint_l (r2_l, "128442051653810314916622590289775531989649843239158643682"
                     "16177647043137765477", 10);
  str2clint_l (r3_l, "836391832187606937820650856449710761904520026199724985596"
                     "729108812301394489219", 10);

  div_l (rsaXXX_l, r2_l, r10_l, r11_l);
  check (r10_l, r3_l, 3, __LINE__);
  check (r11_l, nul_l, 3, __LINE__);

  div_l (rsaXXX_l, r3_l, r10_l, r11_l);
  check (r10_l, r2_l, 4, __LINE__);
  check (r11_l, nul_l, 4, __LINE__);

  return 0;
}


static int S5_test (void)
{

  printf ("Test von Schritt 5 in Divisionsalgorithmus ...\n");

  str2clint_l (r1_l, "e37d3abc904baba7a2ac4b6d8f782b2bf84919d2917347690d9e93d"
        "cdd2b91cee9983c564cf1312206c91e74d80ba479064c8f42bd70aaaa689f80d435a"
        "fc997ce853b465703c8edca", 16);

  str2clint_l (r2_l, "80b0987b72c1667c30c9156a6674c2e73e61a1fd527d4e78b3f1505"
        "603c566658459b83ccfd587ba9b5fcbdc0ad09152e0ac265", 16);

  str2clint_l (r3_l, "1c48a1c798541ae0b9eb2c6327b1fffff4fe5c0e2723", 16);

  str2clint_l (r4_l, "ca2312fbb3f4c23add7655e94c3410b15c6064bd48a4e5fcc33ddf5"
        "53e7cb829bf66fbfd61b4667f5ed6b387ec47c5272cf6fb", 16);

  div_l (r1_l, r2_l, r5_l, r6_l);
  check (r5_l, r3_l, 1, __LINE__);
  check (r6_l, r4_l, 1, __LINE__);

#if !(CLINTMAXDIGIT < 256)
  str2clint_l (r1_l, "100620a6461fc335d8d5be9d9c4039c9d6925da2f9aedea05185ead8"
        "d039a84e5be1ee1858079d45afbdf46045c011b561555ac364d7706eb32a1c4d8f025"
        "ab872ef4a3e6484bc35d114bf9907b7917aa5e4e3cd7adf283d672999460eddee6ca6"
        "b303d534795858191cfd6a3437f1600283732ae7001d385167a951aa7affaed8f5fd8"
        "e6460cf6b31fbb840494f1c1fbeda55cb4fbd78fbf467ee6da98b8e8d30bd59a87223"
        "802740314567d98d6d4a1f67c6ed1b3fb2f6e77c6d14863854e857132c0863a5d3b22"
        "d191b120141e1b0f7c3cd9679cdad3d19cc07b7baba609a8c0cf18af4f73fd9c8a691"
        "cf942c2c3b5bea32d7f1bdfc9b40cb236b2c364ee923e4f2dafa24469b9a97d11919f"
        "59b10dbb7130ddd8a68ea89f1a18e186dd9f09d06da60b14ee7fffbc1ac93d8111249"
        "1c19574da97cffb4696e935d5ef9fb9896e8e51fe0405e97c485de8c53305f325b7d8"
        "833effebd93e889dbefd29a847647dcf0d68ed219b92db7aa669dfa911b4c1eaf4796"
        "b88c7ceec50960b14865ad5a3a0741ab4f76314888364e384c870659b51af1815ce81"
        "a5685d6383a56cefc1e9296d5c0b", 16);

  str2clint_l (r2_l, "bc2adf0cec6fb67ee7b51f89c69e6ac4a8bc1493c15087b0c7b89558"
        "1f6c9369bcd8ef94a5e7caf906da3949ce5a8cb04df692b293e400cc479128e5045e1"
        "2d860ead1c634f2efe031", 16);

  str2clint_l (r3_l, "15cce78a7a83c62ea91adf7f4775543f5edb0cdf8f3efe09aed6073b"
        "7e55178d7fc577c6582675fc939117e52293036e590beb42d2c10812eae3aa1c47afc"
        "7b72aeacc157dd3bb8fc7210b60494c12afe4de4e4c2c08e5f9f9df408d387bac6103"
        "7a9f213c607293675693cb6662103ba09aa629af48918dc8eaa0a351909962aa1a6c1"
        "80021670a39eadc74988483e4b71d3c66affff99b0105495f00c87ffd13749da4bbb2"
        "7cf864597bf2ea71b3fbc15649d35d066e515f60b664577760422d09546d9819b1cd4"
        "289d6b6e706fcc511a8eb0a7e55cbd806d9f99d5959b79675183b878605031ca3bea4"
        "665cca977b344174d4a9b8c00ec5b4a11796f52a8e8ea10373ba497947c0fa4576366"
        "a3276d272895106436a7cbb607ae18115552c1e55cfebfc38b068324b67d9306e4f89"
        "8ed9afc3c557f56f9efe71261a90fbc9fc27095f165c1107448a41cacd7e1831e73aa"
        "2d08b6e239e313ec2301", 16);

  str2clint_l (r4_l, "48114bc17b6310f96a10965127fa5600ecd2f565e2389fe78f51a91f"
        "4bf9e94128eafcc566f05322604ec7a6830b2a01ea9315da71b3e3e8c5a10c37aa8ee"
        "a3557984c47441aaac8da", 16);

  div_l (r1_l, r2_l, r5_l, r6_l);

  check (r5_l, r3_l, 1, __LINE__);
  check (r6_l, r4_l, 1, __LINE__);
#endif /* !(CLINTMAXDIGIT < 256) */

  str2clint_l (r1_l, "7b952b50cba1bd5573a8e0d8ea33e3b7fa5322ece49a3663d72ef457"
        "30dbaf36f1d36156c000c5614586e0debbf6cbf4913417e11f621a3845d6804c8db3b"
        "2cc01b37f198016bf8f3540facf5cadc9059e969e813bf5d", 16);

  str2clint_l (r2_l, "288f8a2d3ccaf166904edea8f67aa27d540bb215d1ea0f50a7311fa7"
        "d07db3b95b947fbc5e", 16);

  str2clint_l (r3_l, "30bfedf9dd29106a454370be597dd621e9d3c65731021de3ef135837"
        "fb5aa548fffff09dc1cd172c95d26735eeb6e1ed9e08", 16);

  str2clint_l (r4_l, "1aa7fd2df02cafd036533325274e90705ea9249d24f121c92357853f"
        "f579a38b426ccdd86d", 16);

  div_l (r1_l, r2_l, r5_l, r6_l);

  check (r5_l, r3_l, 1, __LINE__);
  check (r6_l, r4_l, 1, __LINE__);


  /* Spezialfall: S5 wird ausgefuehrt bei Division mit 32 Bit-Wortbreite */

  str2clint_l (r1_l, "1f1bf7045d135fbc248134dcb1bb953a548d16e3578b7300", 16);

  str2clint_l (r2_l, "07c6fdc11744d7ef09204d372c6ee54ed37552c7978d7131", 16);

  str2clint_l (r4_l, "7c6fdc11744d7ef09204d372c6ee54dda2d1e8c90e31f6d", 16);

  str2clint_l (r3_l, "3", 16);

  div_l (r1_l, r2_l, r5_l, r6_l);

  check (r5_l, r3_l, 1, __LINE__);
  check (r6_l, r4_l, 1, __LINE__);

  return 0;
}


static int divrest_test (unsigned int nooftests)
{
  unsigned int i;
  CLINTD d_l;

  printf ("Tests der Division mit Multiplikation und Addition...\n");

  /* Test mit max_l */
  setmax_l (r1_l);
  rand_l (r2_l, CLINTMAXBIT);
  div_l (r1_l, r2_l, r3_l, r4_l);
  mul_l (r2_l, r3_l, r5_l);
  add_l (r5_l, r4_l, r6_l);
  check (r6_l, r1_l, 1, __LINE__);

  /* Test mit doppelt langem Dividenden */
  rand_l (d_l, CLINTMAXBIT << 1);
  rand_l (r2_l, CLINTMAXBIT);
  div_l (d_l, r2_l, r3_l, r4_l);
  mul_l (r2_l, r3_l, r5_l);
  add_l (r5_l, r4_l, r6_l);
  mod2_l (d_l, CLINTMAXBIT, d_l);
  check (r6_l, d_l, 1, __LINE__);

  /* Allgemeine Tests mit fuehrenden Nullen */
  for (i = 1; i <= nooftests; i++)
    {
      ldzrand_l (r1_l, CLINTRNDLN);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
      div_l (r1_l, r2_l, r3_l, r4_l);
      mul_l (r2_l, r3_l, r5_l);
      add_l (r5_l, r4_l, r6_l);
      check (r6_l, r1_l, i, __LINE__);
    }

  /* Allgemeine Tests, Akkumulator-Betrieb */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      cpy_l (r7_l, r1_l);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
      cpy_l (r8_l, r2_l);
      div_l (r1_l, r2_l, r1_l, r2_l);
      mul_l (r8_l, r1_l, r5_l);
      add_l (r5_l, r2_l, r6_l);
      check (r6_l, r7_l, i, __LINE__);
    }

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/

      div_l (r1_l, r2_l, r3_l, r4_l);
      mul_l (r2_l, r3_l, r5_l);
      add_l (r5_l, r4_l, r6_l);
      check (r6_l, r1_l, i, __LINE__);
    }


  /* Euklidischer Test  a = b * q1 + r1, b = c * q2 + r2   */
  /*                ==> a = c * q1 * q2 + r3 * q1 + r1     */

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/
      nzrand_l (r3_l, CLINTRNDLN); /*lint !e666*/

      div_l (r1_l, r2_l, r4_l, r5_l); /* r1_l = r2_l * r4_l + r5_l */
      div_l (r2_l, r3_l, r6_l, r7_l); /* r2_l = r3_l * r6_l + r7_l */

      mul_l (r3_l, r6_l, r8_l);
      mul_l (r8_l, r4_l, r8_l);
      mul_l (r7_l, r4_l, r9_l);
      add_l (r8_l, r9_l, r9_l);
      add_l (r9_l, r5_l, r9_l); /* r9_l = r3_l*r6_l*r4_l + r7_l*r4_l + r5_l */

      check (r9_l, r1_l, i, __LINE__);
    }
  return 0;
}


static int mod_test (unsigned int nooftests)
{
  unsigned int i, j;
  clint n;
  clintd k;
  printf ("Test Reduzierung mit mod_l, mod2_l, und umod_l...\n");


  /* Test von mod_l */

  if (0 == mod_l (one_l, nul_l, r1_l))
    {
      fprintf (stderr, "Fehler: Reduzierung mit 0 von mod_l() nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  /* Test mit Modulus = 1 */
  rand_l (r1_l, CLINTRNDLN);
  mod_l (r1_l, one_l, r3_l);
  check (r3_l, nul_l, 0, __LINE__);

  /* Allgemeine Tests */
  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r1_l, CLINTRNDLN);
      nzrand_l (r2_l, CLINTRNDLN); /*lint !e666*/

      div_l (r1_l, r2_l, r3_l, r4_l);
      mod_l (r1_l, r2_l, r5_l);
      check (r5_l, r4_l, i, __LINE__);
    }


  /* Test von mod2_l */

  /* Test von mod2_l mit 0 */
  mod2_l (nul_l, 0, r0_l);
  mod_l (nul_l, one_l, r4_l);
  check (r0_l, nul_l, i, __LINE__);
  check (r4_l, nul_l, i, __LINE__);

  k = 0;
  rand_l (r0_l, CLINTRNDLN);
  cpy_l (r1_l, r0_l);
  cpy_l (r2_l, one_l);
  mod2_l (r0_l, k, r0_l);
  mod_l (r1_l, one_l, r4_l);
  check (r0_l, r4_l, i, __LINE__);


  /* Test von mod2_l im Akku-Betrieb */
  for (i = 1; i <= nooftests; i++)
    {
      k = ulrand64_l () % MAXTESTLEN;
      rand_l (r0_l, CLINTRNDLN);
      cpy_l (r1_l, r0_l);
      cpy_l (r2_l, one_l);
      for (j = 1; j <= k; j++)
        {
          mul_l (r2_l, two_l, r2_l);
        }
      mod2_l (r0_l, k, r0_l);
      mod_l (r1_l, r2_l, r4_l);
      check (r0_l, r4_l, i, __LINE__);
    }


  /* Test von umod_l() */

  /* Reduzierung mit 0 */
  if (BASEMINONE != umod_l (one_l, 0))
    {
      fprintf (stderr, "Fehler: Reduzierung mit 0 von umod_l() nicht erkannt in Zeile %d\n", __LINE__);
      exit (-1);
    }

  /* Modulus = 1 */
  if (0 != umod_l (two_l, 1))
    {
      fprintf (stderr, "Fehler: Fehler bei Reduzierung mod 1 bei umod_l() in Zeile %d\n", __LINE__);
      exit (-1);
    }

  for (i = 1; i <= nooftests; i++)
    {
      rand_l (r0_l, CLINTRNDLN);
      n = usrand64_l ();
      u2clint_l (r1_l, n);

      u2clint_l (r2_l, umod_l (r0_l, n));
      if (!mequ_l (r0_l, r2_l, r1_l))
        {
          fprintf (stderr, "Fehler bei umod_l() in Test %d/Zeile %d\n", i, __LINE__);
          exit (-1);
        }
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
      fprintf (stderr, "Fehler bei div_l() in Test %d/Zeile %d\n", test, line);
      disperr_l ("Dividend = ", r1_l); /*lint !e666 */
      disperr_l ("Divisor = ", r2_l);  /*lint !e666 */
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


