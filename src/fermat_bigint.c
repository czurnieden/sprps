#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>

/* Adapt to your paths. */
#include "/home/czurnieden/GITHUB/libtommath/tommath.h"
#include "/home/czurnieden/GITHUB/libtommath/tommath_private.h"

#if (MP_DIGIT_BIT < 31)
#error "Needs larger limbs, 28 bit at least"
#endif

#define SIEVE_SIZE 512
/*
static mp_digit primes[SIEVE_SIZE] = {
   0x0002, 0x0003, 0x0005, 0x0007, 0x000B, 0x000D, 0x0011, 0x0013,
   0x0017, 0x001D, 0x001F, 0x0025, 0x0029, 0x002B, 0x002F, 0x0035,
   0x003B, 0x003D, 0x0043, 0x0047, 0x0049, 0x004F, 0x0053, 0x0059,
   0x0061, 0x0065, 0x0067, 0x006B, 0x006D, 0x0071, 0x007F, 0x0083,
   0x0089, 0x008B, 0x0095, 0x0097, 0x009D, 0x00A3, 0x00A7, 0x00AD,
   0x00B3, 0x00B5, 0x00BF, 0x00C1, 0x00C5, 0x00C7, 0x00D3, 0x00DF,
   0x00E3, 0x00E5, 0x00E9, 0x00EF, 0x00F1, 0x00FB, 0x0101, 0x0107,
   0x010D, 0x010F, 0x0115, 0x0119, 0x011B, 0x0125, 0x0133, 0x0137,
   0x0139, 0x013D, 0x014B, 0x0151, 0x015B, 0x015D, 0x0161, 0x0167,
   0x016F, 0x0175, 0x017B, 0x017F, 0x0185, 0x018D, 0x0191, 0x0199,
   0x01A3, 0x01A5, 0x01AF, 0x01B1, 0x01B7, 0x01BB, 0x01C1, 0x01C9,
   0x01CD, 0x01CF, 0x01D3, 0x01DF, 0x01E7, 0x01EB, 0x01F3, 0x01F7,
   0x01FD, 0x0209, 0x020B, 0x021D
};
*/

static mp_digit primes[SIEVE_SIZE] = {
   2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53,
   59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
   127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
   191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251,
   257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
   331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397,
   401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
   467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557,
   563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619,
   631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701,
   709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787,
   797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863,
   877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953,
   967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 1031,
   1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093,
   1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171,
   1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237,
   1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303,
   1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409,
   1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471,
   1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543,
   1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607,
   1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669,
   1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747, 1753,
   1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847,
   1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907, 1913,
   1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997, 1999,
   2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069, 2081,
   2083, 2087, 2089, 2099, 2111, 2113, 2129, 2131, 2137, 2141,
   2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221, 2237, 2239,
   2243, 2251, 2267, 2269, 2273, 2281, 2287, 2293, 2297, 2309,
   2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377, 2381,
   2383, 2389, 2393, 2399, 2411, 2417, 2423, 2437, 2441, 2447,
   2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539, 2543, 2549,
   2551, 2557, 2579, 2591, 2593, 2609, 2617, 2621, 2633, 2647,
   2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693, 2699,
   2707, 2711, 2713, 2719, 2729, 2731, 2741, 2749, 2753, 2767,
   2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833, 2837, 2843,
   2851, 2857, 2861, 2879, 2887, 2897, 2903, 2909, 2917, 2927,
   2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011, 3019,
   3023, 3037, 3041, 3049, 3061, 3067, 3079, 3083, 3089, 3109,
   3119, 3121, 3137, 3163, 3167, 3169, 3181, 3187, 3191, 3203,
   3209, 3217, 3221, 3229, 3251, 3253, 3257, 3259, 3271, 3299,
   3301, 3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347, 3359,
   3361, 3371, 3373, 3389, 3391, 3407, 3413, 3433, 3449, 3457,
   3461, 3463, 3467, 3469, 3491, 3499, 3511, 3517, 3527, 3529,
   3533, 3539, 3541, 3547, 3557, 3559, 3571, 3581, 3583, 3593,
   3607, 3613, 3617, 3623, 3631, 3637, 3643, 3659, 3671
};


/* TODO: read precomputed sieve from file */
/* p * n where p is a small prime and n is the smallest number such that base^n is congruent to 1 mod p */
/*
static mp_digit sieve_pn_2[SIEVE_SIZE] = {
   0, 6, 20, 21, 110, 156, 136, 342, 253, 812, 155, 1332, 820, 602,
   1081, 2756, 3422, 3660, 4422, 2485, 657, 3081, 6806, 979, 4656,
   10100, 5253, 11342, 3924, 3164, 889, 17030, 9316, 19182, 22052,
   2265, 8164, 26406, 13861, 29756, 31862, 32580, 18145, 18528, 38612,
   19701, 44310, 8251, 51302, 17404, 6757, 28441, 5784, 12550, 4112,
   34453, 72092, 36585, 25484, 19670, 26602, 85556, 31314, 48205,
   48828, 100172, 9930, 7077, 120062, 121452, 31064, 64261, 67161,
   138756, 143262, 73153, 150932, 17468, 80200, 83436, 175142, 176820,
   18533, 31176, 32047, 195806, 100576, 34732, 212060, 106953, 217622,
   114481, 118341, 240590, 82834, 126253, 258572, 135460, 273006, 292140
};
*/

static mp_digit sieve_pn_2[SIEVE_SIZE] = {
   0, 6, 20, 21, 110, 156, 136, 342, 253, 812, 155, 1332, 820, 602, 1081, 2756,
   3422, 3660, 4422, 2485, 657, 3081, 6806, 979, 4656, 10100, 5253, 11342, 3924,
   3164, 889, 17030, 9316, 19182, 22052, 2265, 8164, 26406, 13861, 29756, 31862,
   32580, 18145, 18528, 38612, 19701, 44310, 8251, 51302, 17404, 6757, 28441,
   5784, 12550, 4112, 34453, 72092, 36585, 25484, 19670, 26602, 85556, 31314,
   48205, 48828, 100172, 9930, 7077, 120062, 121452, 31064, 64261, 67161, 138756,
   143262, 73153, 150932, 17468, 80200, 83436, 175142, 176820, 18533, 31176, 32047,
   195806, 100576, 34732, 212060, 106953, 217622, 114481, 118341, 240590, 82834,
   126253, 258572, 135460, 273006, 292140, 298662, 309692, 316406, 161596, 65094,
   83088, 343982, 87764, 179101, 15025, 183921, 375156, 95018, 382542, 28395, 41024,
   137602, 208981, 425756, 433622, 436260, 32304, 457652, 15026, 158930, 490700,
   501972, 258121, 87967, 178852, 181794, 275653, 281625, 572292, 289180, 295296,
   596756, 618582, 634412, 326836, 218970, 673220, 338253, 683102, 686412, 351541,
   726756, 366796, 737022, 371953, 768252, 48455, 778806, 392941, 821742, 82901,
   140607, 431056, 109629, 884540, 895862, 64804, 467061, 188374, 476776, 482653,
   490545, 331004, 508536, 93196, 1037342, 347140, 530965, 266514, 539241, 274838,
   367850, 1124660, 564453, 380564, 590241, 1189190, 397852, 300578, 31987, 1228772,
   1246572, 1260006, 636756, 661825, 332064, 193058, 1370070, 278716, 1407782, 355514,
   360300, 1470156, 184984, 747253, 1509212, 757065, 1528932, 194844, 1583822,
   1629452, 817281, 1644806, 207529, 1665390, 840456, 1691300, 848253, 1706942,
   869221, 79260, 293267, 925480, 933661, 1883756, 1905780, 325967, 991936, 337251,
   2034902, 120036, 256507, 1034641, 1046181, 2103950, 2109756, 709074, 360395, 547970,
   2197806, 1104841, 1107816, 2227556, 2245502, 1140805, 2318006, 2342430, 1189653,
   2397852, 301282, 1214461, 1226961, 2466470, 830554, 1252153, 849604, 640400,
   1290421, 323409, 83876, 2619542, 2626020, 881834, 2678132, 152444, 1381953, 2777222,
   2783892, 2864556, 1439056, 961634, 416996, 370015, 989002, 3001556, 3029340,
   3050262, 255938, 1546161, 131498, 1588653, 3191582, 1066244, 45025, 655582, 1660753,
   558455, 1704781, 3461460, 3483822, 1749385, 1753128, 3521252, 1764381, 891608,
   3611900, 3634742, 457207, 3726830, 1244852, 3796652, 1902225, 3890756, 3914462,
   3946182, 1985028, 3986012, 665667, 572858, 808422, 677712, 4106702, 4114812,
   2077741, 4212756, 2126953, 4278692, 2164240, 4336806, 2176741, 60581, 4403702,
   2227105, 92972, 1132628, 4539030, 2282316, 4581740, 109293, 2316628, 2333880,
   1581954, 1617002, 2434321, 4895156, 4930620, 5001932, 2505441, 5028806, 1688250,
   5137022, 5146092, 1291064, 433390, 871347, 5255556, 2636956, 5329172, 2669205,
   5440556, 5468582, 1825980, 1835354, 110497, 5553092, 5619270, 2823876, 1133356,
   946051, 5704932, 1431014, 2876401, 1162102, 2919736, 2934253, 5936532, 744505,
   2992681, 6044222, 6083622, 1528314, 6133052, 3131253, 3176460, 6403430, 6443982,
   3232153, 6494852, 3252525, 6535692, 6648662, 3355345, 210033, 3402136, 3423036,
   6867020, 3465028, 3501981, 441062, 7067622, 3544453, 1188595, 7163652, 7195806,
   212273, 602336, 7249556, 7281902, 7325142, 3673405, 3678828, 3695121, 3722356,
   71006, 7510340, 2518084, 3788128, 1275587, 3854476, 7775732, 1297815, 7820412,
   3921400, 7854006, 7943942, 334294, 8045732, 8079806, 8125350, 291414, 8182460,
   4142881, 4165941, 4194856, 4212253, 8459372, 2835324, 4282201, 8634782, 1452876,
   8740892, 8776406, 1101499, 326810, 4495501, 4501500, 9063110, 9111342, 4567753,
   9220332, 4622320, 2323338, 624444, 9403422, 4738581, 9501806, 2384708, 1380396,
   4862521, 486876, 2459408, 3333802, 5013361, 5019696, 3371860, 10153782, 175505,
   10256006, 5147236, 2586468, 2074324, 3474404, 2113150, 10578756, 1325599, 3539274,
   1782695, 10880102, 2178660, 10932942, 2743164, 5506221, 11039006, 5539456, 739482,
   1862051, 11199062, 5639761, 564648, 11360270, 3791252, 1640276, 383183, 5802121,
   11645156, 5891028, 1486519, 1991232, 11975060, 1998151, 12016622, 12030492, 12183590,
   12239502, 6161805, 12365772, 6218101, 3112578, 12478556, 12520982, 835676, 12577662,
   12648692, 6331461, 12748470, 12819980, 6417153, 6453028, 2167807, 13050156, 6539536,
   6561253, 2196755, 13224132, 13267806, 13384622, 6736285
};

/*
static mp_digit sieve_pn_7[SIEVE_SIZE] = {
   0, 0, 0, 0, 110, 156, 272, 57, 506, 203, 465, 333, 1640, 258, 1081,
   1378, 1711, 3660, 4422, 4970, 1752, 6162, 3403, 7832, 9312, 10100,
   5253, 11342, 2943, 1582, 16002, 8515, 9316, 9591, 11026, 22650,
   8164, 26406, 13861, 29756, 31862, 2172, 1910, 4632, 19306, 19701,
   44310, 8251, 25651, 52212, 27028, 56882, 57840, 31375, 65792, 68906,
   72092, 36585, 38226, 5620, 39903, 85556, 46971, 9641, 32552, 50086,
   36410, 18872, 120062, 121452, 11296, 128522, 22387, 23126, 143262,
   73153, 37733, 157212, 80200, 9816, 7961, 29470, 185330, 187056,
   32047, 195806, 50288, 52098, 212060, 71302, 108811, 114481, 78894,
   240590, 248502, 126253, 258572, 270920, 136503, 48690
};
*/


#define START_PRIME 1
#ifndef MAX_SIEVE
#define MAX_SIEVE SIEVE_SIZE
#endif

#ifndef MAX_TRIAL
#define MAX_TRIAL MAX_SIEVE
#endif

/*
   See proposition 3 (p. 1007) in :
   C. Pomerance, J. L. Selfridge, and S. S. Wagstaﬀ, Jr.
   “The Pseudoprimes to 25*10^9” Math. Comp. 29 (1980), 1003–1026.
   https://www.ams.org/mcom/1980-35-151/S0025-5718-1980-0572872-7/S0025-5718-1980-0572872-7.pdf
*/

static mp_err is_not_psp(const mp_int *a, bool *result)
{
   mp_digit t;
   mp_err err = MP_OKAY;
   int i=0;

   *result = false;

   for (i = START_PRIME; i < MAX_SIEVE; i++) {
      if ((err = mp_div_d(a,primes[i], NULL, &t)) != MP_OKAY)        goto LTM_ERR;
      if (t == 0u) {
         if ((err = mp_div_d(a, sieve_pn_2[i], NULL, &t)) != MP_OKAY)        goto LTM_ERR;
         if (t != primes[i]) {
            *result = true;
            break;
         }
      }
   }

LTM_ERR:
   return err;
}

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


static bool trial(const mp_int *a)
{
   mp_err err;
   for (int i = 0; i < MAX_TRIAL; i++) {
      mp_digit res;
      if ((err = mp_mod_d(a, primes[i], &res)) != MP_OKAY) {
         fprintf(stderr,"mp_mod_d in \"trial\" failed. error = %s\n", mp_error_to_string(err));
         exit(EXIT_FAILURE);
      }
      if (res == 0u) {
         return true;
      }
   }
   return false;
}

static bool is_prime(mp_int *n)
{
   mp_digit bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
   mp_int b;
   mp_err err;
   bool result = false;

   if ((n->used == 1) && (n->dp[0] < 38)) {
      mp_digit ln = n->dp[0];
      for (int i = 0; i < 12; i++) {
         if (ln == bases[i]) {
            return true;
         }
      }
   } else if (trial(n)){
      return false;
   }

   if ((err = mp_init(&b)) != MP_OKAY) {
      fprintf(stderr,"mp_init(&b) failed because of %s\n", mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }
   for (int i = 0; i < 12; i++) {
      mp_set(&b,bases[i]);
      if ((err = mp_prime_miller_rabin(n, &b, &result)) != MP_OKAY)        goto LTM_ERR;
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

static mp_err mp_fermat(const mp_int *a, const mp_int *b, bool *result)
{
   mp_int  t, am1;
   mp_err  err;

   *result = false;
   if ((err = mp_init_multi(&t, &am1, NULL)) != MP_OKAY) {
      return err;
   }
   /* b^(a-1) = 1 mod a */
   if ((err = mp_sub_d(a,1u,&am1)) != MP_OKAY)                                           goto LTM_ERR;
   if ((err = mp_exptmod(b, &am1, a, &t)) != MP_OKAY)                                    goto LTM_ERR;
   if (mp_cmp_d(&t, 1) == MP_EQ) {
      *result = true;
   }

   err = MP_OKAY;
LTM_ERR:
   mp_clear_multi(&t, &am1, NULL);
   return err;
}

int main(int argc, char **argv)
{
   mp_int N, b;

   mp_digit base = 0ul;
   mp_err err = MP_OKAY;
   bool result = false;


   if (argc == 3) {
      base = strtoul(argv[1], NULL, 10);

   } else {
      fprintf(stderr,"Usage: %s base startpoint\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   if ((err = mp_init_multi(&N, &b, NULL)) != MP_OKAY) {
      fprintf(stderr,"mp_init_multi failed because of %s\n", mp_error_to_string(err));
      exit(EXIT_FAILURE);
   }

   if ((err = mp_read_radix(&N, argv[2], 10)) != MP_OKAY)                     goto LTM_ERR;

   fprintf(stderr,"base = %"PRIu64", startpoint = ", base);
   mp_print("", &N, 10, stderr);


   mp_set(&b, base);

   if (mp_iseven(&N)) {
      if ((err = mp_incr(&N)) != MP_OKAY)                                            goto LTM_ERR;
   }

   for (uint64_t i = 0ul; i < UINT32_MAX/2; i++) {
      if ((i != 0) && ((i % 1000000ul) == 0ul)) {
         mp_print("At: ", &N, 10, stderr);
      }
      /* Sieving, even that little, saves about 30% in time */
#ifdef USE_SIEVE
      if ((err = is_not_psp(&N, &result)) != MP_OKAY)        goto LTM_ERR;
      if (result) {
         if ((err = mp_add_d(&N, 2, &N)) != MP_OKAY)                                    goto LTM_ERR;
         continue;
      }
#endif
      if ((err = mp_fermat(&N, &b, &result)) != MP_OKAY)                 goto LTM_ERR;
      if (result) {
         if (!is_prime(&N)) {
            mp_print("", &N, 10, stdout);
            fflush(stdout);
         }
      }
      if ((err = mp_add_d(&N, 2, &N)) != MP_OKAY)                                    goto LTM_ERR;
   }

   mp_clear_multi(&N, &b, NULL);
   exit(EXIT_SUCCESS);
LTM_ERR:
   mp_clear_multi(&N, &b, NULL);
   exit(EXIT_FAILURE);
}

/* That's what worked here, your milage may vary */
/* clang -Weverything -O3 -g3 -DUSE_SIEVE -DMAX_SIEVE=20 fermat_bigint.c -o fermat_bigint ~/GITHUB/libtommath/libtommath.a */
/*

./fermat_bigint 2 18446744077123551623 >> psp_2_over_2_64

Last number checked was: 18446744077123551623


2-PSPs <2^64 found:
18446744073709551617

*/


