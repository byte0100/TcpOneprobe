#! /bin/sh

ps -aux|grep agent|grep -v 'grep'|awk '{print $2}'|while read pid

do
  echo "agent is running,to kill agent pid=$pid"
  kill -9 $pid
  echo "kill result: $?"
done
