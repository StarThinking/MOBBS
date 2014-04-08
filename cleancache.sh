#! /bin/bash

sleep_time=$1

ips[1]=6
ips[2]=15
ips[3]=19
ips[4]=20
ips[5]=101
ips[6]=102

function clean() {
	for((i=1; i<=6; i++))
 	do 
		echo "clean cache in 10.0.0.${ips[$i]}"
		ssh 10.0.0."${ips[$i]}" "sync; echo 3 > /proc/sys/vm/drop_caches"&
		tids[$i]=$!
	done

	for((k=1; k<=6; k++))
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
