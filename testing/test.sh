#!/bin/bash

for a in 1 2 3 4 5 6 7 8 9 10
do           # Reset inner loop counter.

  # ===============================================
  # Beginning of inner loop.
  for B in 2 4 6 8 10
  do
    for h in 2 4 6 8 10
    do
    for tableSize in 2 4 6 8 10
    do
    	java SplashTable $B 3 $tableSize $h inputFile.txt probeFile.txt resultFile.txt
  	done
   done
  done
  # End of inner loop.
  # ===============================================
             # Space between output blocks in pass of outer loop.
done