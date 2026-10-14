#!/bin/bash
# MySQL 首次初始化（/docker-entrypoint-initdb.d）：
#   1) 解开上游 llnut/dnf 的 init_sql.tgz（.origin_1031 只读挂载到 /dnf-origin）
#   2) 按上游 init_main_db.sh / init_server_group_db.sh 的库清单建库导入
#   3) 再执行 90-remake-extra.sql 补齐上游缺失的表（如 taiwan_cain.cube_premium）
set -euo pipefail
D="${SERVER_GROUP_DB:-cain}"
SRC=/dnf-origin/build/dnf_data/home/template/init/init_sql.tgz
[ -f "$SRC" ] || { echo "缺少 $SRC：请先 git submodule update --init .origin_1031"; exit 1; }
T=$(mktemp -d); tar -xzf "$SRC" -C "$T"
M=(mysql --protocol=socket -uroot -p"$MYSQL_ROOT_PASSWORD")
load() { # load <db> <sql 文件名>
  echo "[dnf-init] $1 <= $2"
  "${M[@]}" -e "CREATE DATABASE IF NOT EXISTS \`$1\` DEFAULT CHARACTER SET utf8"
  "${M[@]}" "$1" < "$T/$2"
}
for db in d_taiwan d_taiwan_secu d_technical_report tw; do load $db $db.sql; done
load "d_channel_$D"            d_channel.sql
load d_guild                   d_guild.sql
load "taiwan_$D"               taiwan_cain.sql
load "taiwan_${D}_2nd"         taiwan_cain_2nd.sql
load "taiwan_${D}_log"         taiwan_cain_log.sql
load "taiwan_${D}_web"         taiwan_cain_web.sql
load "taiwan_${D}_auction_gold" taiwan_cain_auction_gold.sql
load "taiwan_${D}_auction_cera" taiwan_cain_auction_cera.sql
for db in taiwan_login taiwan_prod taiwan_game_event taiwan_se_event taiwan_login_play taiwan_billing; do load $db $db.sql; done
sed "s/taiwan_cain\./taiwan_${D}./g" /docker-entrypoint-initdb.d/remake-extra.sql.in | "${M[@]}"
rm -rf "$T"
echo "[dnf-init] 完成"
