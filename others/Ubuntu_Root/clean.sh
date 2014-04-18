#! /bin/bash

while true
do
	echo "clean cache"
	sync; echo 3 > /proc/sys/vm/drop_caches
	sleep 2
done
