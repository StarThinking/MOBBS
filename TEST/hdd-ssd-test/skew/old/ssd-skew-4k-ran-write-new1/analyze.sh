#! /bin/bash

counter=3 
bs=1
power=2

for skew in 1.25 1.5 1.75 2.0 2.25 2.5 2.75 3.0 3.25 3.5 3.75 4.0
do
	iopss=`cat "$skew"* | grep iops | awk 'BEGIN{FS=",|:|="}{print $7}'`
	sum=0
	for iops in $iopss
	do
		sum=`expr $sum + $iops`
	done
	avg=`expr $sum / $counter`
	bw=`expr $avg \* $bs`
	bs=`expr $bs \* $power`
	echo "$skew avg iops = $avg, bw = $bw KB"
done
