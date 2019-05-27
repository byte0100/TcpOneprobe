#! /bin/sh

if [ $# -lt 1 ]
then
  echo "Usage:sh startagent.sh <port>"
  exit 1
fi

./agent $1

