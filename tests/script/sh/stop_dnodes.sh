#!/bin/sh

set +e
#set -x

unset LD_PRELOAD
UNAME_BIN=`which uname`
OS_TYPE=`$UNAME_BIN`

PID=`ps -ef|grep /usr/bin/taosd | grep -v grep | awk '{print $2}'`
if [ -n "$PID" ]; then
  echo systemctl stop taosd
  systemctl stop taosd
fi

if [ "$OS_TYPE" != "Darwin" ]; then
  PID=`ps -C taosd | grep -w "[t]aosd" | awk '{print $1}' | head -n 1`
else
  PID=`ps a -c | grep -w "[t]aosd" | awk '{print $1}' | head -n 1`
fi
while [ -n "$PID" ]; do
  echo kill -9 $PID
  #pkill -9 taosd
  kill -9 $PID
  echo "Killing taosd processes"
  if [ "$OS_TYPE" != "Darwin" ]; then
    fuser -k -n tcp 6030
  else
    lsof -nti:6030 | xargs kill -9
  fi
  if [ "$OS_TYPE" != "Darwin" ]; then
    PID=`ps -C taosd | grep -w "[t]aosd" | awk '{print $1}' | head -n 1`
  else
    PID=`ps a -c | grep -w "[t]aosd" | awk '{print $1}' | head -n 1`
  fi
done

if [ "$OS_TYPE" != "Darwin" ]; then
  PID=`ps -C taos | grep -w "[t]aos" | awk '{print $1}' | head -n 1`
else
  PID=`ps a -c | grep -w "[t]aos" | awk '{print $1}' | head -n 1`
fi
while [ -n "$PID" ]; do
  echo kill -9 $PID
  #pkill -9 taos
  kill -9 $PID
  echo "Killing taos processes"
  if [ "$OS_TYPE" != "Darwin" ]; then
    fuser -k -n tcp 6030
  else
    lsof -nti:6030 | xargs kill -9
  fi
  if [ "$OS_TYPE" != "Darwin" ]; then
    PID=`ps -C taos | grep -w "[t]aos" | awk '{print $1}' | head -n 1`
  else
    PID=`ps a -c | grep -w "[t]aos" | awk '{print $1}' | head -n 1`
  fi
done

if [ "$OS_TYPE" != "Darwin" ]; then
  PID=`ps -C tmq_sim | grep -w "[t]mq_sim" | awk '{print $1}' | head -n 1`
else
  PID=`ps a -c | grep -w "[t]mq_sim" | awk '{print $1}' | head -n 1`
fi
while [ -n "$PID" ]; do
  echo kill -9 $PID
  #pkill -9 tmq_sim
  kill -9 $PID
  echo "Killing tmq_sim processes"
  if [ "$OS_TYPE" != "Darwin" ]; then
    fuser -k -n tcp 6030
  else
    lsof -nti:6030 | xargs kill -9
  fi
  if [ "$OS_TYPE" != "Darwin" ]; then
    PID=`ps -C tmq_sim | grep -w "[t]mq_sim" | awk '{print $1}' | head -n 1`
  else
    PID=`ps a -c | grep -w "[t]mq_sim" | awk '{print $1}' | head -n 1`
  fi
done
