#! /bin/sh

if [ $# -eq 0 ]
then
  echo "Usage:sh startagent.sh <port> <-log>"
  echo "Start under default conditions ./agent 9090 -log"
  ./agent 9090 -log
  exit 0
fi

if [ $# -eq 1 ]
then
  ./agent $1
  exit 1
fi

./agent $1 $2
exit 2


