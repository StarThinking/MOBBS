#! /bin/bash

counter=3 

for skew in 1.5 2.0 2.5 3.0 3.5 4.0
do
	iopss=`cat "$skew"* | grep iops | awk 'BEGIN{FS=",|:|="}{print $7}'`
	sum=0
	for iops in $iopss
	do
		sum=`expr $sum + $iops`
	done
	avg=`expr $sum / $counter`
	echo "$skew avg iops = $avg"
done
