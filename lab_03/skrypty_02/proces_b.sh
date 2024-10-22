#!/bin/bash

bash ./proces_c.sh &

while :
do
  echo "Proces B"
  sleep 2
done
