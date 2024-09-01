#!/bin/bash
echo "当前进程的id=$$"
#以后台的方式运行一个脚本，并获取它的进程号
sh position.sh &
echo "最后一个后台方式运行的进程id=$!"
echo "执行的结果是=$?"
