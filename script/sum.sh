#!/bin/sh

INFILE=$1

sum=0
while read inputline
do
    if [ "$inputline" != ""  ] ; then
    	let "sum= sum + $inputline"
    fi 
done < $INFILE

echo "Sum is $sum"
