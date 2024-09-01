#!/bin/bash
#while循环 while[条件表达式]
#do
#程序
#done
#案例1： 从1加到100
SUM=0
i=0
while [ $i -le $1 ]
do 
SUM=$(( SUM + $i ))
i=$(( $i + 1 ))
done
echo "$SUM"