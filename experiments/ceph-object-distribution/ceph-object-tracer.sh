#!/bin/bash

POOL=$1
export HDD_C=0
export SSD_C=0

rados ls -p $POOL | 
while read line;
do
	osd=`ceph osd map $POOL $line | awk '{print substr($14, 2, 1)}'`;
	#echo $osd
	if [ $osd = 0 ] || [ $osd = 1 ]; then
		HDD_C=`expr $HDD_C + 1`
	else
		SSD_C=`expr $SSD_C + 1`
	fi
	echo HDD:$HDD_C,ssd:$SSD_C
done

#echo HDD:$HDD_C, ssd:$SSD_C
