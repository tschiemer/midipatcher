#!/bin/bash

while read line
do
  cmd=$(echo $line | cut -f1 -d' ')

  if [ "$cmd" = "cc" ]; then
    ch=$(echo $line | cut -f2 -d' ')
    cc=$(echo $line | cut -f3 -d' ')
    val=$(echo $line | cut -f4 -d' ')

    ch=$((ch+1))

    echo "cc $ch $cc $val" 
  else
    echo $line
  fi
done < "${1:-/dev/stdin}"
