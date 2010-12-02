# Simple script for updating a series of repositories 
array=("$@")
len=${#array[*]}
if [ $len -lt 1 ]; then
   echo "No inputs!"
   exit 0
fi
let len=len-1
for i in $(seq 0 $len)
do
   targ="$HOME/${array[$i]}"
   echo "targ=$targ"
   if [ -d $targ ]
   then
      cd $targ
      if [ -d "$targ/.git" ]
      then
         git pull master master
      elif [ -d "$targ/.hg" ]
      then
         hg pull
         hg update
      elif [ -d "$targ/.svn" ]
      then
         svn update
      else
         echo "No version control found in $targ"
      fi
   else
      echo "$targ is not a directory!"
   fi
done
