#!/bin/bash
# 按日期分表的“提前建表”：服务端按当前日期拼表名（见 docs/数据库.md「按日期分表」），
# 缺当月/当季/当年的表就会 SQL 报错。本脚本对每种模式找同库里已有的最新一张作为模板，
# CREATE TABLE IF NOT EXISTS <新表> LIKE <模板>，覆盖当前及未来若干周期。
#
# 用法：20-periodic-tables.sh [YYYY-MM-DD]      # 默认今天；传未来日期可模拟跨月
#   ENSURE_MONTHS_AHEAD（默认 3）：除当月外再建几个月；季表/年表同理各多建 1 个周期
#   连接：MySQL 镜像 initdb 阶段用本地 socket；运行容器里由 dnfctl.sh 设 DB_HOST/DB_PORT 调用
set -eo pipefail
D="${SERVER_GROUP_DB:-cain}"
BASE="${1:-$(date +%F)}"
AHEAD="${ENSURE_MONTHS_AHEAD:-3}"
if [ -n "${DB_HOST:-}" ]; then
  M=(mysql -h"$DB_HOST" -P"${DB_PORT:-3306}" -uroot -p"${DNF_DB_ROOT_PASSWORD:-$MYSQL_ROOT_PASSWORD}")
else
  M=(mysql --protocol=socket -uroot -p"$MYSQL_ROOT_PASSWORD")
fi
q() { "${M[@]}" -N -B -e "$1" 2>/dev/null; }

# ensure <库> <前缀> <后缀>：库/前缀下若无该表，则按同前缀、同后缀长度的最新已有表 LIKE 建
ensure() {
  local db=$1 pre=$2 suf=$3 tpl
  q "SELECT 1 FROM information_schema.SCHEMATA WHERE SCHEMA_NAME='$db'" | grep -q 1 || return 0
  if q "SELECT 1 FROM information_schema.TABLES WHERE TABLE_SCHEMA='$db' AND TABLE_NAME='$pre$suf'" | grep -q 1; then return 0; fi
  tpl=$(q "SELECT TABLE_NAME FROM information_schema.TABLES WHERE TABLE_SCHEMA='$db'
           AND TABLE_NAME REGEXP '^${pre}[0-9]{${#suf}}\$' ORDER BY TABLE_NAME DESC LIMIT 1")
  if [ -z "$tpl" ]; then echo "[periodic] 跳过 $db.$pre$suf：库里没有 ${pre}<${#suf} 位> 模板表"; return 0; fi
  "${M[@]}" -e "CREATE TABLE IF NOT EXISTS \`$db\`.\`$pre$suf\` LIKE \`$db\`.\`$tpl\`" 2>/dev/null \
    && echo "[periodic] 建表 $db.$pre$suf（LIKE $tpl）"
}

for i in $(seq 0 "$AHEAD"); do
  ym=$(date -d "$(date -d "$BASE" +%Y-%m-01) +$i month" +%Y%m)
  # df_auction_r / df_point_r：HandlerFor_DB_::GetYYYYMM()  snprintf("%d%02d")
  for adb in "taiwan_${D}_auction_gold" "taiwan_${D}_auction_cera"; do
    ensure "$adb" auction_history_ "$ym"
    ensure "$adb" auction_history_buyer_ "$ym"
  done
  # df_game_r：strftime("%Y%m")
  ensure "taiwan_${D}_log" log_response_time_ "$ym"           # APsystemOnlyOrig/dispatch_151.inc（DB_HANDLE_4）
  ensure taiwan_login member_mousepass_history_ "$ym"         # MouseRegisterEJ.cpp / dispatch_260.inc（DB_HANDLE_6）
  ensure taiwan_login churn_reward_history_ "$ym"             # dispatch_265.inc（DB_HANDLE_6，sprintf "%d%02d"）
done
# 季表：prod_sale_entry_<YY><Q>，Arad_OS/GetQuarterOfYear.inc sprintf("%02d%d", 年%2000, 月/3+1)（DB_HANDLE_7）
for i in 0 1; do
  d=$(date -d "$(date -d "$BASE" +%Y-%m-01) +$((i*3)) month" +%F)
  yq=$(printf '%02d%d' $(( $(date -d "$d" +%Y) % 2000 )) $(( ( $(date -d "$d" +%-m) - 1 ) / 3 + 1 )))
  ensure taiwan_prod prod_sale_entry_ "$yq"
done
# 年表：member_punish_info_history_<YYYY>，dispatch_209/54.inc strftime("%Y")（DB_HANDLE_1 = d_taiwan）
for i in 0 1; do
  ensure d_taiwan member_punish_info_history_ "$(( $(date -d "$BASE" +%Y) + i ))"
done
echo "[periodic] 检查完成（基准日期 $BASE，向后 $AHEAD 个月）"
