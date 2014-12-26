#!/bin/bash

parse_cpu()
{
	cat cpu.info | awk '{t+=$1;c+=1} END {a=t/c;print "cpu",a,"%"}'
}

parse_memory()
{
	cat memory.info | awk '{t+=$1;c+=1} END {a=t/c;print "memory",a,"%"}'
}

parse_network_in()
{
	cat network.info | grep '^ *[0-9]' | awk '{if($1>=100) {t+=$1;c+=1}} END {a=t/c;print "net in",a,"KB/S"}'
}

parse_network_out()
{
	cat network.info | grep '^ *[0-9]' | awk '{if($2>=100) {t+=$2;c+=1}} END {a=t/c;print "net out",a,"KB/S"}'
}

parse_cpu
parse_memory
parse_network_in
parse_network_out
