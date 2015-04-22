parse_network()
{
	cat network.log | grep '^ *[0-9]' | awk '{i+=$1;o+=$2;c+=1} END {ri=i/c;ro=o/c;print "net in",ri,"KB/S","net out",ro,"KB/S"}'
}

parse_cpu_mem()
{
	cat cpu_mem.log | awk '{cpu+=$3;mem+=$4;c++} END {rc=cpu/c;rm=mem/c;print "cpu:",rc,"mem:",rm}'
}

parse_network > info.log
parse_cpu_mem >> info.log
