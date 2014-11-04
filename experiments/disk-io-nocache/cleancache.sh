#! /bin/bash

sleep_time=$1

ips[1]=20
ips[2]=21

function clean() {
	for((i=1; i<=2; i++))
 	do 
		echo "clean cache in 10.0.0.${ips[$i]}"
		ssh 10.0.0."${ips[$i]}" "sync; echo 3 > /proc/sys/vm/drop_caches"&
		tids[$i]=$!
	done

	for((k=1; k<=2; k++))
	do
		wait ${tids[$i]}
		echo "finish"
	done
}

while true
do
        clean
        echo "sleep $sleep_time seconds"
	sleep "$sleep_time"s
done
