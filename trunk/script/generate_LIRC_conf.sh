#!/bin/bash

let "START = 0x0000"
let "END = 0x1FFF"

while [ $START -le $END ]
do
   fname=`printf " \t%0.4x \t0x%0.4x\n" $START $START`
   echo $fname
   ((START += 1))
done

