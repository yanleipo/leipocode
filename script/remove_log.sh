#!/bin/bash

help_menu()
{
   echo "USAGE: remove_log.sh START END"
   echo "START and END are 3-digit numbers"
}

if [ $# -lt 1 ]; then
   help_menu
   exit
fi

START=$1
END=$2

while [ $START -le $END ]
do
   fname=`printf "log-%03d\n" $START`
   rm $fname
   ((START += 1))
##   let "START += 1"
done 

