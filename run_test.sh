#! /bin/bash
a=0

while read line
do
 LINES[$a]=$line;
 a=$(expr $a + 1);
done < "test_cases.txt"

b=0

while [ $b -lt $a ]
do
 ${LINES[$b]};
 read -p "Press [Enter] to continue..."
 b=$(expr $b + 1);
done
