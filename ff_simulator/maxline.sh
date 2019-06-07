#!/bin/sh

cat $1 | while read line
do

count=$(echo $line | wc -c)
echo $line $count

done
