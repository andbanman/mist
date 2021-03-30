#!/bin/bash

cachestats=$1
Ms=(100 1000 10000)
Ps=(10 30 50 70 90 100)

b=3
n=100
first=true

for m in ${Ms[*]}; do
	for p in ${Ps[*]}; do
		if $first; then
			$cachestats "$n" "$m" "$b" none > memcache.csv
			first=false
		else
			$cachestats "$n" "$m" "$b" none | tail -n2 >> memcache.csv
		fi
		$cachestats "$n" "$m" "$b" mru  "$p" | tail -n2 >> memcache.csv
		$cachestats "$n" "$m" "$b" flat "$p" | tail -n2 >> memcache.csv
	done
done
