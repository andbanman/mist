#!/bin/bash

cachestats=$1
Ms=(5000 10000 20000)

b=3
n=100
first=true

for m in ${Ms[*]}; do
	if $first; then
		$cachestats "$n" "$m" "$b" none > iocache.csv
		first=false
	else
		$cachestats "$n" "$m" "$b" none | tail -n2 >> iocache.csv
	fi
	rm -rf cache_small_files #clear the IO cache
	$cachestats "$n" "$m" "$b" io | tail -n2 >> iocache.csv
done
