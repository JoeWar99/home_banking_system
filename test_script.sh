#!/bin/sh

for i in $(seq 1 100) 
do 
    ./user 0 "adminpass" $(shuf -i 10-100 -n 1) 0 "$(shuf -i 1-200 -n 1) $(shuf -i 1000-100000 -n 1) coisoaberto" &
done

for i in $(seq 1 100)
do
    ./user $(shuf -i 1-200 -n 1) "coisoaberto" $(shuf -i 10-100 -n 1) 1 "" &
done

for i in $(seq 1 100)
do
    ./user $(shuf -i 1-200 -n 1) "coisoaberto" $(shuf -i 10-100 -n 1) 2 "$(shuf -i 1-200 -n 1) $(shuf -i 100-10000 -n 1)" &
done

./user 0 "adminpass" 10 3 "" &