#!/bin/bash

[ "$1" ] || { echo "usage: `basename $0` /path/to/trac/project"; exit 1; }

FILE_LIST=`cat trac_wiki_list.txt`

for i in $FILE_LIST; do
   trac-admin $1 wiki remove $i
done
