#! /bin/bash

counter=5
bs=1
power=2

for jobs in 1 2 4 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40
do
	iopss=`cat "$jobs".log* | grep iops | awk 'BEGIN{FS=",|:|="}{print $7}'`
	sum=0
	for iops in $iopss
	do
		sum=`expr $sum + $iops`
	done
	avg=`expr $sum / $counter`
	bw=`expr $avg \* $bs` 
	bs=`expr $bs \* $power`

	lats=`cat "$jobs".log* | grep 99.00 | awk 'BEGIN{FS=",|=|\[|]"}{print $3}'`
	sum2=0
	for lat in $lats
	do
		sum2=`expr $sum2 + $lat`
	done
	avg2=`expr $sum2 / $counter`
	echo "$jobs avg iops = $avg, avg lat = $avg2, bw = $bw KB, "
done
