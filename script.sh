#!/bin/bash
max=100
ip=localhost
filename=index.html
port=3000
tempo=2


for (( i=1; i<=max; ++i )) do
    echo "Request $i of $max"
    curl -s "$ip:$port/$filename" > /dev/null

done
