#!/usr/bin/env perl

use File::Slurp;
use Math::Combinatorics;

my @lines = read_file("allbases", chomp => 1);
my $combinat = Math::Combinatorics->new(count => 2,
                                        data => [@lines],
                                       );

while(my @permu = $combinat->next_combination){
  print join(' ', @permu)."\n";
}

# print join "\n", map { join " ", @$_ } combine 3, @lines;
