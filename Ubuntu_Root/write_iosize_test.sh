#! /bin/bash

for size in 4k 16k 
do
	fio -filename=/dev/vdb -direct=1  -rw=randwrite -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=30 -runtime=3000 -group_reporting --name=test; sleep 1800;
done

for size in 64k 256k 
do
	fio -filename=/dev/vdb -direct=1  -rw=randwrite -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=30 -runtime=2400 -group_reporting --name=test; sleep 1200;
done

for size in 1M 4M
do
	fio -filename=/dev/vdb -direct=1  -rw=randwrite -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=30 -runtime=960 -group_reporting --name=test; sleep 720;
done
