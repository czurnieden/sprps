#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

/* Adapt to your paths. */
#include "tommath.h"

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



#ifndef BUFLEN
#define BUFLEN 4096
#endif

#include <errno.h>
int main(int argc, char **argv)
{
   mp_int N, b;
   mp_digit base = 0ul;
   mp_err err = MP_OKAY;
   FILE *input;
   uint64_t entry;
   char buffer[BUFLEN];
   bool result = false;


   if (argc >= 2) {
      base = strtoul(argv[1], NULL, 10);
   } else {
      fprintf(stderr,"Usage: %s base [filename]\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   if (argc == 3) {
      errno = 0;
      input = fopen(argv[2], "r");
      if (input == NULL) {
         fprintf(stderr,"Failed to open file \"%s\" because of %s \n", argv[2], strerror(errno));
         exit(EXIT_FAILURE);
      }
   } else {
      input = stdin;
   }

   if ( (err = mp_init_multi(&N, &b, NULL) ) != MP_OKAY) {
      fprintf(stderr,"mp_init_multi failed because of %s\n", mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }

   mp_set(&b, base);

   while(fgets(buffer, BUFLEN, input)) {
      /* faster than mp_to_radix? */
      entry = strtoull(buffer, NULL, 10);
      mp_set_u64(&N, entry);
      if( (err = mp_prime_miller_rabin(&N, &b, &result) ) != MP_OKAY)                 goto LTM_ERR;
      if (result) {
         if (!is_prime(&N)) {
            mp_print("", &N, 10, stdout);
         }
      }
   }


   mp_clear_multi(&N, &b, NULL);
   exit(EXIT_SUCCESS);
LTM_ERR:
   mp_clear_multi(&N, &b, NULL);
   exit(EXIT_FAILURE);
}

/* clang -Weverything -O3 -g3 miller_rabin.c -o miller_rabin libtommath.a */




































