#!/bin/bash

#
# mount nfs to /mnt/nfs
#

SATASRC=`ls /mnt/nfs`
SERVERIP="10.80.114.188"

if [ -z "$SATASRC" ] ; then
   mount -t ext2 $SERVERIP:/export/rootfs/target /mnt/nfs

   if [ $? -ne 0 ] ; then
      mount -t nfs $SERVERIP:/export/rootfs/target /mnt/nfs
   fi
   if [ $? -ne 0 ]; then
      mount -t vfat $SERVERIP:/export/rootfs/target /mnt/nfs
   fi
fi


