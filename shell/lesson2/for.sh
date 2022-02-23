#!/bin/bash
# for loop in 1 2 3 4 5
# do 
#    echo "the value is $loop"
# done
   
# shell输出文件夹下的所有文件
path=$1
files=$(ls "$path")
for filename in $files
do
    echo "filename is $filename"
    done