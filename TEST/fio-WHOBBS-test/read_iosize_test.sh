#! /bin/bash

for size in 16k 
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=32 -runtime=3000 -group_reporting --name=test; sleep 1440;
done

for size in 32k 
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=32 -runtime=2400 -group_reporting --name=test; sleep 1200;
done

for size in 64k  
do
# runtime + 240 
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=32 -runtime=1800 -group_reporting --name=test; sleep 1080;
done

for size in 128k 
do
# runtime + 240 
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=32 -runtime=1440 -group_reporting --name=test; sleep 960;
done

for size in 256k 
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=32 -runtime=1200 -group_reporting --name=test; sleep 720;
done

for size in 512k
do
	fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:1.5 -bs=$size -size=128G -numjobs=32 -runtime=960 -group_reporting --name=test; sleep 600;
done
