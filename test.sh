#!/bin/bash

rnums=()
for i in {1..10000}; do
	rnums+=($RANDOM)
done
./lcm -lcm ${rnums[@]}
