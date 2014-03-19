#! /bin/bash

fio -filename=/dev/vdb -direct=1  -rw=randwrite -percentage_random=1 -random_distribution=zipf:1.5 -bs=4k -size=128G -numjobs=32 -runtime=3000 -group_reporting --name=test; sleep 1200;

fio -filename=/dev/vdb -direct=1  -rw=randwrite -percentage_random=1 -random_distribution=zipf:2.0 -bs=4k -size=128G -numjobs=32 -runtime=2400 -group_reporting --name=test; sleep 960;

fio -filename=/dev/vdb -direct=1  -rw=randwrite -percentage_random=1 -random_distribution=zipf:2.5 -bs=4k -size=128G -numjobs=32 -runtime=1800 -group_reporting --name=test; sleep 720;

fio -filename=/dev/vdb -direct=1  -rw=randwrite -percentage_random=1 -random_distribution=zipf:3.0 -bs=4k -size=128G -numjobs=32 -runtime=1200 -group_reporting --name=test; sleep 720;

fio -filename=/dev/vdb -direct=1  -rw=randwrite -percentage_random=1 -random_distribution=zipf:3.5 -bs=4k -size=128G -numjobs=32 -runtime=1200 -group_reporting --name=test; sleep 600;

fio -filename=/dev/vdb -direct=1  -rw=randwrite -percentage_random=1 -random_distribution=zipf:4.0 -bs=4k -size=128G -numjobs=32 -runtime=1200 -group_reporting --name=test; sleep 600;