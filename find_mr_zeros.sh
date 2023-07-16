#!/bin/bash

while read line; do
   echo -n "$line ";
   echo -n  "$line " >> all_combinations_sorted_uniqued_counted;
   read -r -a ARR <<< "$line"
   T1=`sort -n "${ARR[0]}" "${ARR[1]}" | uniq -d | sort -n /dev/stdin "${ARR[2]}"  | uniq -d | wc -l`;
   echo $T1;
   echo $T1 >> all_combinations_sorted_uniqued_counted;
done < allcombinationsofthreewithoutrepetition_next
