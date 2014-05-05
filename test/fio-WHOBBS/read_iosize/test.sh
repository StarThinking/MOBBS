#! /bin/bash

disk_size=16G
zipf=1.5

function test() {
  size=$1
  runtime=$2
  sleeptime=$3
  testname=$4
  echo "test $testname begin"  
  before 
  #ssh 192.168.122.53 "fio -filename=/dev/vdb -direct=1  -rw=randread -random_distribution=zipf:$zipf -bs=$size -size=$disk_size -numjobs=32 -runtime=$runtime -group_reporting --name=test; sleep $sleeptime;"
  ssh 192.168.122.53 "fio -filename=/dev/vdb -direct=1  -rw=read -bs=$size -size=$disk_size -numjobs=32 -runtime=$runtime -group_reporting --name=test; sleep $sleeptime;"
  after $testname
  echo "test $testname finish"  
} 

function before() {
  echo "before"
  rm /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log
  touch /usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log
  /root/WHOBBS/6h+6s/create_pool.sh
  sleep 10
  rbd -p hdd-pool create volume --size 16384
  sleep 60
  rbd -p hdd-pool map volume 
  sleep 60
  dd if=/dev/zero of=/dev/rbd1 bs=4M count=4096
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

#test 16k 8000 100 16k
test 32k 7000 100 32k
#test 64k 6000 100 64k
#test 128k 5000 100 128k
#test 256k 3000 100 256k
#test 512k 2000 100 512k




