#! /bin/bash
#set -x

echo "filebench test begin"

pool=whobbs

for workload in fileserver 
#for workload in fileserver varmail webserver
do 
        #xfs
	if true; then
	echo "test $pool xfs $workload"
	cd /usr/local/libvirt/var/log/libvirt/qemu
	rm Ubuntu.log
	touch Ubuntu.log
	cd -

	virsh start Ubuntu
	sleep 30
	ssh 192.168.122.53 "mkfs.xfs -f /dev/vdb"
	ssh 192.168.122.53 "mount /dev/vdb /mnt"
	ssh 192.168.122.53 "filebench -f /root/filebench_test/$pool/$workload.f" > ./filebench-"$pool-$workload"-xfs.log
	sleep 2000
        virsh destroy Ubuntu
	sleep 30
	cp /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log ./qemu-"$pool-$workload"-xfs.log
	fi
	
	# ext4
	if false;then 
	echo "test $pool ext4 $workload"
	cd /usr/local/libvirt/var/log/libvirt/qemu
	rm Ubuntu.log
	touch Ubuntu.log
	cd -

	virsh start Ubuntu
	sleep 30
	ssh 192.168.122.53 "mkfs.ext4 /dev/vdb"
	ssh 192.168.122.53 "mount /dev/vdb /mnt"
	ssh 192.168.122.53 "filebench -f /root/filebench_test/$pool/$workload.f" > ./filebench-"$pool-$workload"-ext4.log
	sleep 2000
	virsh destroy Ubuntu
	sleep 30
	cp /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log ./qemu-"$pool-$workload"-ext4.log
	fi
done
