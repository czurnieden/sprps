#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

/* Adapt to your paths. */
#include "tommath.h"
#include "tommath_private.h"
static void print_word(mp_word n);

/* Print bigint with the given prefix and a Unix EOL character  */
static void mp_print(const char *s, const mp_int *a, int radix, FILE *stream)
{
   mp_err err;
   fputs(s, stream);
   err = mp_fwrite(a, radix, stream);
   if (err != MP_OKAY) {
      fprintf(stderr,"mp_fwrite in mp_print failed. error = %s\n", mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }
   fputc('\n',stream);
}

static bool is_prime(mp_int *n) {
   mp_digit bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
   mp_int b;
   mp_err err;
   bool result = false;

   if ( (err = mp_init(&b) ) != MP_OKAY) {
      fprintf(stderr,"mp_init(&b) failed because of %s\n", mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }
   for (int i = 0; i < 12; i++) {
      mp_set(&b,bases[i]);
      if( (err = mp_prime_miller_rabin(n, &b, &result) ) != MP_OKAY)        goto LTM_ERR;
      if (!result) {
         goto LTM_ERR;
      }
   }
   mp_clear(&b);
   return true;
LTM_ERR:
   mp_clear(&b);
   return false;
}


int main(int argc, char **argv)
{
   mp_int N, b;
   mp_word start_point = 0ull;
   mp_digit base = 0ul;
   mp_err err = MP_OKAY;
   bool result = false;


   if (argc == 3) {
      base = strtoul(argv[1], NULL, 10);
   } else {
      fprintf(stderr,"Usage: %s base startpoint\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   if ( (err = mp_init_multi(&N, &b, NULL) ) != MP_OKAY) {
      fprintf(stderr,"mp_init_multi failed because of %s\n", mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }

   if( (err = mp_read_radix(mp_int &N, argv[2], 10) ) != MP_OKAY)                     goto LTM_ERR;

   printf("base = %"PRIu64", startpoint = ", base);
   mp_print("", &N, 10, stdout);
   putchar('\n');

   mp_set(&b, base);

   if (mp_iseven(&N)){
      if( (err = mp_incr(&N) ) != MP_OKAY)                                            goto LTM_ERR;
   }

   for (uint64_t i = 0ul; i < UINT64_MAX; i++) {
      if( (err = mp_prime_miller_rabin(&N, &b, &result) ) != MP_OKAY)                 goto LTM_ERR;
      if (result) {
         if (!is_prime(&N)) {
            mp_print("", &N, 10, stdout);
         }
      }
      if( (err = mp_add_d(&N, 2, &N) ) != MP_OKAY)                                    goto LTM_ERR;
      if ((i % 10000000ul) == 0ul) {
         fprintf(stderr,"%"PRIu64": ",i);mp_print("", &N, 10, stdout);
      }
   }

   mp_clear_multi(&N, &b, NULL);
   exit(EXIT_SUCCESS);
LTM_ERR:
   mp_clear_multi(&N, &b, NULL);
   exit(EXIT_FAILURE);
}


/*
  clang -Weverything -O3 -g3 mr_bigint.c -o mr_bigint libtommath.a
*/


















