#!/bin/bash
#案例1 计算两个参数的和
function getSum()
{
     SUM=$(($n1 + $n2))
     echo "和是$SUM"
}
read -t 10 -p"请输入一个数n1" n1
read -t 10 -p"请输入一个数n2" n2
getSum $n1 $n2