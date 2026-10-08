#!/bin/bash
# 查看机器人状态
cd "$(dirname "$0")"
if [ -f data/bot.pid ] && kill -0 "$(cat data/bot.pid)" 2>/dev/null; then
  echo "运行中（pid $(cat data/bot.pid)）"
else
  echo "未运行"
fi
curl -s --max-time 5 http://127.0.0.1:8088/api/status | python3 -c '
import json,sys
try:
    s=json.load(sys.stdin)
except Exception:
    print("网页接口无响应"); sys.exit()
print("拍卖服连接:", "已连接" if s["connected"] else "断开", "/ 就绪" if s["ready"] else "", "/ 暂停" if s["paused"] else "")
print("自动上架:", s["seller_enabled"], " 自动收购:", s["buyer_enabled"], " 孤儿清理:", s["orphan_enabled"], "(演练)" if s["orphan_dry_run"] else "")
for it in s["seller_items"]:
    print("  %s(%d) 基础价 %d 在售 %d/%d 条 共 %s 个 %s" % (it["name"], it["item_id"], it["base_price"], it["active"], it["target_listings"], it["active_qty"] or 0, it["disabled"] or ""))
'
tail -5 logs/bot.log 2>/dev/null
