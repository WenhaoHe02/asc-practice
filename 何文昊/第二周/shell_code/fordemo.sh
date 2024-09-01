#!/bin/bash
#for 变量 in 值1 值2 值3
#do
#程序代码
#done
#或
#for (( 初始值;循环控制条件;变量变化))
#do
#程序代码
#done
#案例1: 打印命令行输入的参数
for i in "$*"
do 
echo "num is $i"
done

for i in "$@"
do 
echo "num is $i"
done
# 案例2： 1-100累加
SUM=0
for (( i=1; i<=100; i++))
do
SUM=$(( $SUM + i ))
done
echo "SUM = $SUM"