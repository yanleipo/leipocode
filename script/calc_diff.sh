#!/bin/sh

INFILE=$1
OUTFILE=`printf "%s_2" $INFILE`

rm $OUTFILE

prevline=0
linediff=0
while read inputline
do
    if [  $prevline -ne 0  ] ; then
	let "linediff= $inputline - $prevline"
	echo $linediff >> $OUTFILE
    fi
    let "prevline=$inputline"
done < $INFILE
