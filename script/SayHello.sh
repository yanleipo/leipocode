#!/bin/bash


#
# 1>&2 rirect the message from stdout to stderr
#
SayHello()
{
   date
   echo "Hello $LOGNAME, Have nice computing." 1>&1
   return
}

SayHello
