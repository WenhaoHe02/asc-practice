#!/bin/bash
#案例1: "ok"是否等于"ok
#中括号之间需要空格
if [ "ok" = "ok" ]
then 
echo "equal" 
fi
#案例2 23是否大于等于32
if [ 23 -ge 32 ]
then
echo "大于"
fi
#案例3 /root/shcode/aaa.txt是否存在
#判断语句，使用 -f
if [ -f /home/umr/shell_code/var.sh ] 
then 
echo "存在"
fi
