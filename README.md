# sprps
## Collection of strong pseudoprimes to small prime bases

This packet contains the strong pseudoprimes to, at the time of writing, the
prime bases 2 to 2293 that are smaller than `2^32` (4,294,967,296).

The raw data is in the folder `raw` with one strong pseudoprime per line with a
Unix line-end (ASCII `\n` `0x0a`).

The factored data is in the folder `factored` with one strong pseudoprime per
line with a Unix line-end (ASCII `\n` `0x0a`), followed by a colon (ASCII `:` `0x3a`)
followed by one space (ASCII ` ` `0x20`) followed by the space separated factors.

There are some tables with strong pseudoprimes to the composite bases 4 to 100
in the folder `raw_composite` and their factors in `factored_composite`.

There are Fermat pseudoprimes to base 3 (three) in the folder `misc` up to
`2^40` (1,099,511,627,776) together with their factoring. Because it is quite easy
to get the strong pseudoprimes to base 3 (three) from that data the strong pseudoprimes
to base 3 (three) together with their factoring is also in the folder `misc`.

There is a script `compute_3psp_parallel.sh` to run the 3-psp search in parallel in the
folder `scripts`. Currentyl base 3 is hardcoded but can be changed manually in the script.

## What is not in the repository?
Large files:
 - A file with the modular order of three for all primes below `2^32` is in ax
   [Google drive](https://drive.google.com/file/d/1Uadc7Eolh6bI-3Tgt86W5CY1p0dkAD9g/view?usp=sharing)

## How to get the whole repository as a compressed packet

`wget https://github.com/czurnieden/sprps/archive/master.tar.gz`

For the branch `master`. For the other branches yet to come replace `master.tar.gz`
with `other_branch.tar.gz`.

### Why?
It took over 150 CPU hours to generate that data and I hate to waste that.

## License
The license here is "unlicense" but the raw data itself, being just data,
is Public Domain, of course. Yes, there is a difference.

## Is there more to come?
Probably.

### There are currently beavering away:
 - PRP of base 3 up to `2^64` (which means SPRP to base 3, too)
 - SPRP to more prime bases (planned is up to base 10007)
 - SPRP to more composite bases (planned is up to base 10008)

### Planned:
 - extend 2-SPRP `>2^64` (first one `>2^64` is, believe it or not: `1 + 2^64`)
 - extend 7-SPRP and 61-SPRP `>2^32` but only a bit to check distribution

## Help needed?
If you have some CPU cycles to spare or already computed some tables/data not in this
collection: you are heartily welcome!

## Where are the sources?
All tables have been generated with Dana Jacobsen's
[ntheory](https://metacpan.org/pod/ntheory). The relevant snippets are listed at
["Pseudoprime Statistics, Tables, and Data"](https://ntheory.org/pseudoprimes.html)
at the end of the page.

For values above `2^64` the relevant functions from the big integer library 
[LibTomMath](https://github.com/libtom/libtommath) have been used. 
Check directory `src` for some code.

The factors were generated with `factor(1)` by Paul Rubin, Torbjorn Granlund, and Niels Moller
which can be found in the [GNU coreutils](https://www.gnu.org/software/coreutils/).

## How long do you think MS will accept such large amount of non-code?
Get it while it's hot!

