#!/bin/bash

echo "This script extract part of a string."
echo "Method one extracts from starting matching pattern to ending matching pattern."
echo "Method two extracts from starting matching pattern and end with if next character"
echo "does not belong to [a-zA-Z0-9_-]."
echo " "

LINE="var fullscreenUrl = \"watch?v=fjdsasdfsajkl&t=fdsajkldsa&hl=fdjsaklfdsafd_fdhsakfds&lpid=dfsa\""
echo "The string is $LINE"
echo "We want to extract the content between \"&t=\" and \"&hl=\""

# extract str bettween "&t=" and "&hl="
t=`echo $LINE | sed -n 's/.*fullscreenUrl.*&t=\(.*\)&hl=.*/\1/p'`
echo "Result using method 1: $t"

# extract from "&t=" and end before "&"
t=`echo $LINE | sed -n 's/.*fullscreenUrl.*&t=\([a-zA-Z0-9_-]*\).*/\1/p'`
echo "Result using method 2: $t"
