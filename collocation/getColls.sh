#!/bin/bash
time (
	time python3 count.py ../Articles/ver3/ &&
	time g++ -std=c++11 -O2 merge_coll.cpp -o merge_coll.out &&
	./merge_coll.out Count/Parts/*/w* &&
	rm merge_coll.out &&
	g++ -std=c++11 -O2 getCol.cpp -o getCol.out &&
	./getCol.out > stat/colres &&
	rm getCol.out
	)