#!/bin/bash
#read(选项)(参数)
#选项:
#-p 指定读取值时的提示符
#-t 指定读取值时等待的时间，如果没有在指定时间内输入，就不再等待
#参数
#变量:指定读取值的变量名
#案例:test_read.sh
read -p"请输入一个数，NUM1 = " NUM1
echo "输入的变量NUM1 = $NUM1"
read -t 10 -p"请输入一个数，NUM2 = " NUM2
echo "输入的变量NUM2 = $NUM2"
