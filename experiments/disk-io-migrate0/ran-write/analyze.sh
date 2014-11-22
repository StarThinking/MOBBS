#! /bin/bash

counter=20
bs=1
power=2
size=4k

for ((c=1; c<=$counter; c++))
do
	iopss=`cat "${size}.log$c" | grep iops | awk 'BEGIN{FS=",|:|="}{print $7}'`
	sum=0
	for iops in $iopss
	do
		sum=`expr $sum + $iops`
	done
	bw=`expr $sum \* 4` 
	echo "$size $c iops = $sum, bw = $bw KB"
done
