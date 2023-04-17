#!/bin/sh

# Run several instances of Dana Jacobsens pseudoprime-generators to find Fermat primes to base 3 (three) in parallel.
# For a different base bs change is_pseudoprime(\$_,3) to is_pseudoprime(\$_,b)

# Find several those generators at the end of the page https://ntheory.org/pseudoprimes.html
# Caveat: these generators are restricted to 2^64 max for their argument, some even less, and so is this one

# We assume that "bc" is installed, the GNU coreutils (for "nproc"), and "parallel", of course but we check it, too.

# Excerpt from "parallel" manpage;
# {%} Job slot number. This replacement string will be replaced by the job's slot number between 1 and number of jobs to run in parallel. There
#     will never be 2 jobs running at the same time with the same job slot number.
#     The replacement string {%} can be changed with --slotreplace.


OPTIND=1
# Some cheap checks, just in case.

# Search for a method to count the number of cores, give hint at user to set value at command line
# if nothing has been found. We only search for "nproc" and try "getconf _NPROCESSORS_ONLN" next,
# nothing more complicates like parsing /proc/cpuinfo or the like.

CORE_COUNT=1;
nproc > 2&>1
if [ $? -ne 0 ] ; then
   echo "nproc not found, trying getconf"
   getconf _NPROCESSORS_ONLN;
   if [ $? -ne 0 ] ; then
     echo "nproc not found, getconf not found, giving up. Please set \"-j\" manually" 
     exit 127;
   else
      CORE_COUNT=$(getconf _NPROCESSORS_ONLN)
   fi
else
   CORE_COUNT=$(nproc)
fi


# check if parallel is installed
parallel --help > 2&>1
if [ $? -ne 0 ] ; then
   echo "parallel not found"
   exit 127;
fi

# check if bc is installed
bc -h > 2&>1
if [ $? -ne 0 ] ; then
   echo "bc not found"
   exit 127;
fi

# check if perl is installed
perl -e '' 2&>1
if [ $? -ne 0 ] ; then
   echo "Perl not found"
   exit 127;
fi
# check if perl module "ntheory" is installed
perl -e 'use ntheory;' 2&>1
if [ $? -ne 0 ] ; then
   echo "Perl module \"ntheory\" not found"
   exit 127;
fi

tstart=0;
njobs=$CORE_COUNT
# about an hour and some change per job on my AMD A8-6600K at full throttle (3900 MHz)
# will be slightly more the larger the start values get
blocksize="2^34"
verbose=0;
dryrun=""
# TODO: this makes the actual file, but we only need the name
tmpfilename=$(mktemp -q ./XXXXXXXpsp3.part)
cleanup=0;

while getopts "s:j:z:h?vdc" options; do
   case "$options" in
      s) tstart=${OPTARG};;
      j) njobs=${OPTARG};;
      z) blocksize=${OPTARG};;
      v) verbose=1;;
      c) cleanup=1;;
      d) dryrun="--dryrun";;
      h|\?)
         echo "$0 -sjzh?vd"
         echo "   -s start value"
         echo "   -j number of jobs"
         echo "   -z size of blocks"
         echo "   -d dryrun, do nothing, just print the commands"
         echo "   -v verbose, although not by much"
         echo "   -c clean-up. Deletes all temporary files. Be careful!"
         echo "   -h this help"
         exit 0;
         ;;
   esac
done

if [ $verbose -eq 1 ] ; then
   echo "Starting at $tstart with $njobs jobs and a block of size $blocksize."
   if [ ! -z $dryrun ] ; then
      echo "Don't worry, it is a dry-run, nothing will be executed, just printed"
      echo ""
   fi
fi

# bc code in a more readable form:
#
# tstart = $tstart;
# tend = 0;
# njobs = $njobs + 1;
# size = $blocksize;
# start = tstart;
# for(n = 1; n < njobs; n++){
#   tend = start + n * size;
#   print tstart, \",\", tend, \" \";
#   tstart = tend + 1;
# }

bcline="tstart = $tstart;tend = 0;njobs = $njobs + 1;size = $blocksize;start = tstart;for(n=1;n < njobs;n++){tend = start + n * size; print tstart, \",\", tend, \" \"; tstart = tend + 1;}"
# Will fill files psp3.part1, psp3.part2, psp3.part3, ..., psp3.part$njobs 
parallel $dryrun -j $njobs  "perl -Mntheory=:all -E 'foroddcomposites { say if is_pseudoprime(\$_,3) } {} '" ">" $tmpfilename{%} ::: $(echo "$bcline" |  BC_LINE_LENGTH=0 bc -q)

# If you want to check the output while it is worked at, you need to switch Perl's output buffer off.
# It is a bit slower but the 3-PSPs get quite sparse quite quickly, the difference is neglible.
# parallel $dryrun -j $njobs  "perl -Mntheory=:all -E 'STDOUT->autoflush(1);  foroddcomposites { say if is_pseudoprime(\$_,3) } {} '" ">" psp3.part{%} ::: $(echo "$bcline" |  BC_LINE_LENGTH=0 bc -q)

theend=$(echo "$bcline" |  BC_LINE_LENGTH=0 bc -q | rev | cut -d',' -f 1 | rev | tr -d ' ')
if [ -z $dryrun ] ; then
   sort  -nu $tmpfilename* >> $tmpfilename"s_concatenated"
   mv -v $tmpfilename"s_concatenated" "3-psp-"$tstart"-"$theend"_concatenated"
else
   # sort numerically and only print uniq lines
   echo "sort -nu $tmpfilename* >> $tmpfilename"s_concatenated""
   echo "mv -v $tmpfilename's_concatenated' '3-psp-'$tstart'-'$theend'_concatenated'"
fi

if [ $cleanup -eq 1 ] ; then
  echo "Cleaning up..."
  rm -v $tmpfilename*
fi

















