#! /bin/bash

disk_size=64G
size=4k

function test() {
  zipf=$1
  runtime=$2
  sleeptime=$3
  testname=$4
  echo "test $testname begin"  
  before 
  if [ $zipf = 0 ]
  then
    ssh 192.168.122.53 "fio -filename=/dev/vdb -direct=1  -rw=randwrite -percentage_random=1 -bs=$size -size=$disk_size -numjobs=32 -runtime=$runtime -group_reporting --name=test; sleep $sleeptime;"
  else
    ssh 192.168.122.53 "fio -filename=/dev/vdb -direct=1  -rw=randwrite -percentage_random=1 -random_distribution=zipf:$zipf -bs=$size -size=$disk_size -numjobs=32 -runtime=$runtime -group_reporting --name=test; sleep $sleeptime;"
  fi
  after $testname
  echo "test $testname finish"  
} 

function before() {
  echo "before"
  rm /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log
  touch /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log
  /root/Qemu_RBD/6h+6s/create_pool.sh
  sleep 10
  rbd -p hdd-pool create volume --size 65536
  sleep 60
  rbd -p hdd-pool map volume 
  sleep 60
  dd if=/dev/zero of=/dev/rbd1 bs=4M count=16384
  sleep 60
  virsh start Ubuntu
  sleep 60
}

function after() {
  testname=$1
  echo "after"
  virsh destroy Ubuntu
  sleep 60
  cp /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log ./$testname.log
  rbd unmap /dev/rbd1
  sleep 60
}

test 2.5 3000 100 zipf-2.5
test 2.25 4000 100 zipf-2.25
test 2.0 6000 100 zipf-2.0
test 1.75 7000 100 zipf-1.75
test 1.5 8000 100 zipf-1.5
test 1.25 8000 100 zipf-1.25
test 0 80000 100 zipf-0




