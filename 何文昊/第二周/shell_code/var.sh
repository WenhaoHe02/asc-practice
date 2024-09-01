#!/bin/bash
# 案例1：定义变量A
A=100
# 输出变量需要加上$
echo A=$A
echo "A=$A"
#案例2： 撤销变量A
unset A
echo "A=$A"
#案例3： 声明静态的变量B=2，不能unset
readonly B=2
echo "B=$B"
#unset B
#变量名一般习惯用大写 等号间不能有空格
#加反引号或者$()相当于把命令返回值赋给变量
C=`date`
D=$(date)
echo "C=$C"
echo "D=$D"
