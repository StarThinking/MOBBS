parse_network()
{
	cat network.log | grep '^ *[0-9]' | awk '{t+=$1;c+=1} END {a=t/c;print "net in",a,"KB/S"}'
}

parse_cpu_mem()
{
	cat cpu_mem.log | awk '{cpu+=$3;mem+=$4;c++} END {rc=cpu/c;rm=mem/c;print "cpu:",rc,"mem:",rm}'
}

parse_network > info.log
parse_cpu_mem >> info.log
