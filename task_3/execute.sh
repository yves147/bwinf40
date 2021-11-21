#!/bin/bash
for k in {0..6}
do
	for i in {0..2}
	do
	echo "$k - $i"
	time (cat examples/worte$k.txt | ./a.out $i)
	done
done
