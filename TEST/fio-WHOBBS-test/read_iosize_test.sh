#! /bin/bash

disk_size=64G

for size in 16k 
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=$disk_size -numjobs=32 -runtime=3600 -group_reporting --name=test; sleep 1800;
done

for size in 32k 
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=$disk_size -numjobs=32 -runtime=3000 -group_reporting --name=test; sleep 1600;
done

for size in 64k  
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=$disk_size -numjobs=32 -runtime=2400 -group_reporting --name=test; sleep 1400;
done

for size in 128k 
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=$disk_size -numjobs=32 -runtime=1800 -group_reporting --name=test; sleep 1200;
done

for size in 256k 
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=$disk_size -numjobs=32 -runtime=1200 -group_reporting --name=test; sleep 800;
done

for size in 512k
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=$disk_size -numjobs=32 -runtime=1000 -group_reporting --name=test; sleep 600;
done


