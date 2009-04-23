#!/bin/sh

/sbin/insmod fd2fp_module.ko

# Create /dev/test directory if not exist
# ----------------------------------------
if [ ! -e /dev/test ]; then mkdir -p /dev/test ;fi
chmod 755 /dev/test

# Create a node in /dev/test (Arg 1=node_name, Arg 2=nbdev)
# ---------------------------------------------------------
function create_node ()
{
 node_name="$1"
 nbdev="$2"
 major=`cat /proc/devices | grep "\b$node_name\b" | cut -b1-3`
 echo "node_name $node_name nbdev $nbdev major $major"
 if [ ! -z "$major" ]; then
     j=0
     while [ $j -lt $nbdev ]; do
	 dev="/dev/test/$node_name$j"
	 cur_major=$((0x`stat -c %t $dev 2>/dev/null`))
	 if [ ! -c ${dev} ] || [ $major -ne $cur_major ]; then
	     rm -f ${dev} 2> /dev/null
	     mknod $dev c $major $j || return 1
	     chmod 0660 $dev || return 1
	 fi
	 j=$((j+1))
     done
 fi
}

function read_list ()
{
    while read node_name nbdev; do
        create_node $node_name $nbdev
    done
}

read_list <<EOS
fd2fp 4
EOS
