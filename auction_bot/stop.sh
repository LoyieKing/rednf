#!/bin/bash
# 停止拍卖行机器人（断开拍卖服连接，拍卖服会自动注销该频道；已上架的系统挂单保留在拍卖行，到期自动下架）
cd "$(dirname "$0")"
if [ ! -f data/bot.pid ] || ! kill -0 "$(cat data/bot.pid)" 2>/dev/null; then
  echo "机器人未在运行"; rm -f data/bot.pid; exit 0
fi
PID=$(cat data/bot.pid)
kill -TERM "$PID"
for i in $(seq 1 20); do kill -0 "$PID" 2>/dev/null || break; sleep 0.5; done
if kill -0 "$PID" 2>/dev/null; then echo "未能在 10 秒内退出，强制结束"; kill -KILL "$PID"; fi
rm -f data/bot.pid
echo "已停止"
