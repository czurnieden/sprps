#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>


typedef unsigned long xuint128_t __attribute__((mode(TI)));

static void strrev(char *str)
{
   char *end = str + strlen(str) - 1;
   while (str < end) {
      *str ^= *end;
      *end ^= *str;
      *str ^= *end;
      str++;
      end--;
   }
}


/* base 10 and ASCII only */
static char *u128tostr(xuint128_t n)
{
   /* Yes, I know, but it is simple */
   static char s[40];
   int digit = 0, i = 0;
   for (int j = 0; j < 40; j++) {
      s[j] = 0;
   }
   while (n > 0ull) {
      digit = (int)(n % 10ull);
      s[i++] = (char)digit + 0x30;
      n /= 10ull;
   }
   strrev(s);
   return s;
}



/* base 10 and ASCII only */
static xuint128_t strtou128(const char *s)
{
   xuint128_t res = 0u;

   for (size_t i = 0u; i < strlen(s);  i++) {
      res *= 10;
      res += (xuint128_t)(s[i] - 0x30);
   }

   return res;
}

typedef struct {
   uint64_t a;
   uint64_t b;
   uint64_t c;
   uint64_t d;
} ranctx;

static ranctx jenkins_x;

#define rot(x,k) (((x)<<(k))|((x)>>(64-(k))))
static uint64_t jenkins_val(void)
{
   uint64_t e = jenkins_x.a - rot(jenkins_x.b, 7);
   jenkins_x.a = jenkins_x.b ^ rot(jenkins_x.c, 13);
   jenkins_x.b = jenkins_x.c + rot(jenkins_x.d, 37);
   jenkins_x.c = jenkins_x.d + e;
   jenkins_x.d = e + jenkins_x.a;
   return jenkins_x.d;
}

static void jenkins_init(uint64_t seed)
{
   int i;
   jenkins_x.a = 0xF1EA5EEDuL;
   jenkins_x.b = jenkins_x.c = jenkins_x.d = seed;
   for (i = 0; i < 20; ++i) {
      (void)jenkins_val();
   }
}


static xuint128_t rand128(void)
{
   xuint128_t res = 0ull;
   uint64_t high, low;

   high = jenkins_val();
   low = jenkins_val();

   res = high;
   res <<= 64ull;
   res |= low;

   return res;
}

#ifdef MULMOD_FAST
/* Needs log2(A*B) < 128 */
static xuint128_t mulmod(const xuint128_t A, const xuint128_t B, const xuint128_t mod)
{
   return ((A % mod) * (B % mod)) % mod;
}
#else
static xuint128_t mulmod(const xuint128_t A, const xuint128_t B, const xuint128_t mod)
{
   xuint128_t res = 0, c;
   xuint128_t a = A, b = B;


   b %= mod;
   a %= mod;
   /* Slow "bit-banging" modulus but does not overflow */
   while (a != 0ull) {
      /* Three additional additions for every set bit */
      if ((a & 0x1ull) == 1ull) {
         if (b >= (mod - res)) {
            res -= mod;
         }
         res += b;
      }
      a >>= 1ull;
      c = b;
      if (c >= (mod - b)) {
         c -= mod;
      }
      b += c;
   }
   return res % mod;
}
#endif

static int lsb(const xuint128_t N)
{
   int count = 0ull;
   xuint128_t n = N;
   if (N == 0ull) {
      return 0;
   }
   while ((n & 1ull) == 0ull) {
      n >>= 1ull;
      count++;
   }
   return count;
}

#define MIN(x,y) (((x) < (y)) ? (x) : (y))

static xuint128_t gcd(xuint128_t u, xuint128_t v)
{
   int u_lsb, v_lsb, k;

   if (u == 0ull) {
      return v;
   }

   if (v == 0ull) {
      return u;
   }

   /* reduce by 2^k */
   u_lsb = lsb(u);
   v_lsb = lsb(v);
   k     = MIN(u_lsb, v_lsb);

   if (k > 0) {
      u >>= (xuint128_t)k;
      v >>= (xuint128_t)k;
   }

   if (u_lsb != k) {
      u >>= (xuint128_t)(u_lsb - k);
   }

   if (v_lsb != k) {
      v >>= (xuint128_t)(v_lsb - k);
   }

   while (v != 0ull) {
      if (u > v) {
         xuint128_t t = u;
         u = v;
         v = t;
      }
      v = v - u;
      if (v == 0ull) {
         break;
      }
      v_lsb = lsb(v);
      v >>= (xuint128_t)v_lsb;
   }

   u <<= (xuint128_t)k;

   return u;
}



static  xuint128_t pollard_rho(xuint128_t n)
{
   xuint128_t y = 0ull, ys = 0ull, c = 0ull, m = 0ull;
   xuint128_t g = 1ull, r = 1ull, q = 1ull;
   xuint128_t k = 0ull, t, x;

   y = rand128();
   c = rand128();
   m = rand128();

   do {
      x = y;
      for (xuint128_t i = 1ull; i < r; i++) {
         y = (mulmod(y,y, n) + c) % n;
      }
      k = 0ull;
      while ((k < r) && (g == 1ull)) {
         ys = y;
         for (xuint128_t j = 1ull; j < MIN(m, r - k); j++) {
            y = (mulmod(y,y, n) + c) % n;
            if (x < y) {
               t = y - x;
            } else {
               t = x - y;
            }
            q = mulmod(q, t, n);
         }
         g = gcd(q, n);
         k = k + m;
      }
      r <<= 1ull;
   } while (g == 1ull);
   if (g == n) {
      for (;;) {
         ys = (mulmod(ys, ys, n) + c) % n;
         if (x < ys) {
            t = y - x;
         } else {
            t = x - ys;
         }
         g = gcd(t, n);
         if (g > 1ull) {
            break;
         }
      }
   }
   return g;
}


#define xuint128_t_NUM_BITS 128
static xuint128_t isqrt(const xuint128_t N)
{
   xuint128_t s, rem, root, n;

   if (N < 1ull) {
      return 0ull;
   }
   n = N;
   /* highest power of four <= n */
   s = ((xuint128_t) 1) << (xuint128_t)(xuint128_t_NUM_BITS - 2);

   rem = n;
   root = 0ull;
   while (s > n) {
      s >>= 2ull;
   }
   while (s != 0ull) {
      if (rem >= (s | root)) {
         rem -= (s | root);
         root >>= 1ull;
         root |= s;
      } else {
         root >>= 1ull;
      }
      s >>= 2ull;
   }
   return (xuint128_t)root;
}

#define PRIME_TAB_LENGTH 54
static uint8_t prime_tab[54] = {
   2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47,
   53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107,
   109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167,
   173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
   233, 239, 241, 251
};

#define FACTOR_ARRAY_LENGTH 129
static int factor(xuint128_t n, xuint128_t *factors)
{
   xuint128_t p, t, residue, N = n;
   int idx = 0;

   if (n < 2ull) {
      factors[0] = 0;
      return 0;
   }

   for (int i = 0; i < PRIME_TAB_LENGTH; i++) {
      p = (xuint128_t)prime_tab[i];
      residue = n % p;
      if (residue == 0ull) {
         factors[idx++] = p;
         if (idx == FACTOR_ARRAY_LENGTH) {
            return 0;
         }
         for (;;) {
            n /= p;
            if ((n > 1ull) && ((n % p) == 0ull)) {
               factors[idx++] = p;
               if (idx == FACTOR_ARRAY_LENGTH) {
                  return 0;
               }
            } else {
               break;
            }
         }
      }
      /* early out */
      if (n == 1ull) {
         goto CLEAR_REST;
      }
   }

   /* TODO: could be a perfect square that is n = t^m with m>=2 */
   t = (xuint128_t)isqrt(n);
   if ((t * t) == n) {
      factors[idx++] = t;
      factors[idx] = t;
      return 1;
   }

   while (n > 1ull) {
      t = n;
      while (true) {
         p = pollard_rho(t);
         if ((t == p) && (t != N)) {
            break;
         }
         t = p;
      }
      factors[idx++] = p;
      if (idx == FACTOR_ARRAY_LENGTH) {
         return 0;
      }
      for (;;) {
         n /= p;
         if ((n > 1ull) && ((n % p) == 0ull)) {
            factors[idx++] = p;
            if (idx == FACTOR_ARRAY_LENGTH) {
               return 0;
            }
         } else {
            break;
         }
      }
   }
   /* Let's check if we found all of them */
   t = 1ull;
   for (int i = 0; i < idx; i++) {
      t *= factors[i];
   }
   if (t != N) {
      return 0;
   }
CLEAR_REST:
   for (int i = idx; i < FACTOR_ARRAY_LENGTH; i++) {
      factors[i] = 0;
   }

   return 1;
}




int main(int argc, char **argv)
{
   xuint128_t n = 0u;
   xuint128_t factors[20];
   int err, i;


   if (argc == 2) {
      n = strtou128(argv[1]);
   } else {
      fprintf(stderr,"Usage: %s, n<2^128\n",argv[0]);
      exit(EXIT_FAILURE);
   }

   jenkins_init(0xdeadbeef);

   err = factor(n, factors);
   if (err == 0) {
      fprintf(stderr,"something broke in factor()\n");
      exit(EXIT_FAILURE);
   }

   printf("%s: ", u128tostr(n));

   for (i = 0; i < FACTOR_ARRAY_LENGTH; i++) {
      if (factors[i+1] == 0ull) {
         printf("%s", u128tostr(factors[i]));
         break;
      } else {
         printf("%s ", u128tostr(factors[i]));
      }
   }
   putchar('\n');

   exit(EXIT_SUCCESS);
}

/* clang -Weverything -O3 -g3 factoring.c -o factoring  */





