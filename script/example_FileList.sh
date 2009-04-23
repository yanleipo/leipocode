#!/bash

example_FileList

#
# used to rm unwanted recorded files in Alticaptor Demo
#
example_FileList()
{
   CURRENTDIR=`pwd`
   if [ -d /root/pdr/media_vol ]; then
      cd /root/pdr/media_vol;
      FILES=`echo *rec*`
      FILES2=`echo *rec_0* *rec_1* *rec_2* *rec_3*`

      for dfile in $FILES; do
         inlist=0;
         for d1file in $FILES2; do
            if [ $dfile == $d1file ]; then 
	      inlist=1;
            fi
         done
         if [ $inlist == 0 ]; then
	    rm $dfile -rf
         fi
      done
      cd $CURRENTDIR;
   fi

   if [ -d /root/pdr/media_vol/MW/gem_storage ]; then
      cp /root/pdr/media_vol/MW/gem_storage /root/bin -rf
   fi
}
