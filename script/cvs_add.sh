FILE_LIST=echo `ls -R | grep ".*:" | sed -n '/CVS/!p' | sed 's/:/ /g'`
CURDIR=`pwd`

for ifile in $FILE_LIST; do
    cd $ifile
    echo "add $ifile"
    cvs add *
    cvs commit -F $CURDIR/cvs_log.txt *
    cd $CURDIR
done
