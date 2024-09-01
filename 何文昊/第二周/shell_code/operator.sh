#!/bin/bash
# 基本语法:$(()) $[] 或expr m + n, expr运算符间要有空格如果希望将expr结果赋给某个变量，使用`
Res=$(((2 + 3) * 4))
echo "res = $Res "
Res2=`expr \( 2 + 3 \) \* 4`
echo "res2 = $Res2"
