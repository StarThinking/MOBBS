#! /bin/bash

pool=$1
echo "cal all objects in $pool"

objects=`rados -p $pool ls`
num=0
for object in $objects
do
	num=`expr $num + 1`
done
echo $num
