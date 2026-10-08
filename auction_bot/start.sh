#!/bin/bash
# 启动拍卖行机器人（后台运行），日志：logs/bot.log，网页：http://localhost:8088/
cd "$(dirname "$0")"
mkdir -p data logs
if [ -f data/bot.pid ] && kill -0 "$(cat data/bot.pid)" 2>/dev/null; then
  echo "机器人已在运行（pid $(cat data/bot.pid)）"; exit 0
fi
setsid nohup python3 -u -m auction_bot.main -c "${1:-config.yaml}" >> logs/bot.log 2>&1 < /dev/null &
echo $! > data/bot.pid
sleep 2
if kill -0 "$(cat data/bot.pid)" 2>/dev/null; then
  echo "已启动（pid $(cat data/bot.pid)），网页控制台 http://localhost:$(python3 -c 'import yaml;print(yaml.safe_load(open("'"${1:-config.yaml}"'",encoding="utf-8")).get("web",{}).get("port",8088))')/"
else
  echo "启动失败，请查看 logs/bot.log"; tail -20 logs/bot.log; exit 1
fi
