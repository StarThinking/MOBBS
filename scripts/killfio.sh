ids=`ps aux | grep fio | awk '{print $2}'`
for id in $ids
do
        echo killing $id
	kill -9 $id
done
