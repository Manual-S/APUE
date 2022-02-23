#!/bin/sh

a=10
b=20
if [ $a = $b ] 
then
    echo "a 等于 b"
elif [ $a -gt $b ] 
then
   echo 'a 大于 b'
else
   echo 'a 小于 b'
fi
