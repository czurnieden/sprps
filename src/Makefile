# You may or may not need to adapt it to your needs
# It assumes a shared version of libtommmath which is not
# made by default in libtommath.
# Otherwise exchange "-ltommath" with a link to "libtommath.a"

VERSION=0.0.1

PLATFORM := $(shell uname | sed -e 's/_.*//')

ifndef MAKE
ifneq (,$(findstring $(PLATFORM),FreeBSD OpenBSD DragonFly NetBSD))
  MAKE=gmake
else
  MAKE=make
endif
endif


ifeq ($(CC), clang)
CFLAGS += -Weverything
else
CFLAGS += -Wall -Wextra -Wsign-compare -Wshadow 
endif

CFLAGS += -g3 -O3 -funroll-loops -fomit-frame-pointer

# Needs a compiler with some kind of sanitizer linked
# If that is unknown: try to set SANITIZER=1 and if it
# works it...uhm...works.
ifneq (,$(SANITIZER))
CFLAGS +=  -fsanitize=undefined -fno-sanitize-recover=all
endif

all: factoring miller_rabin mr_bigint fermat

clean:
	rm -f *.o *.obj *.exe factoring miller_rabin mr_bigint fermat_bigint

factoring:
	$(CC) $(CFLAGS) factoring.c -o factoring

miller_rabin:
	$(CC) $(CFLAGS) miller_rabin.c -o miller_rabin -ltommath

mr_bigint:
	$(CC) $(CFLAGS) mr_bigint.c -o mr_bigint -ltommath

fermat:
# MAX_SIEVE=20 worked well here, YMMV, of course (max.: MAX_SIEVE=512)
	$(CC) $(CFLAGS) -DUSE_SIEVE -DMAX_SIEVE=20  fermat_bigint.c -o fermat_bigint -ltommath


