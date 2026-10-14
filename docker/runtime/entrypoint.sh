#!/bin/bash
# rednf 运行容器入口：渲染配置 → 等数据库 → 初始化 db_connect → 按顺序拉起全部服务
set -euo pipefail
: "${PUBLIC_IP:?必须设置 PUBLIC_IP（客户端能访问到的本机 IP）}"
export SERVER_GROUP="${SERVER_GROUP:-3}"              # 3 = 希洛克（端口 3xxxx）
export SERVER_GROUP_NAME="${SERVER_GROUP_NAME:-siroco}"
export SERVER_GROUP_DB="${SERVER_GROUP_DB:-cain}"     # 库名 taiwan_${SERVER_GROUP_DB}
export MAIN_BRIDGE_IP="${MAIN_BRIDGE_IP:-127.0.0.1}"
export DB_HOST="${DB_HOST:-mysql}" DB_PORT="${DB_PORT:-3306}"
export DNF_DB_ROOT_PASSWORD="${DNF_DB_ROOT_PASSWORD:?}" DNF_DB_GAME_PASSWORD="${DNF_DB_GAME_PASSWORD:?}"
export DNF_DB_GAME_PASSWORD="${DNF_DB_GAME_PASSWORD:0:8}"   # 服务端只认 8 位
export OPEN_CHANNEL="${OPEN_CHANNEL:-11,52}"
export DEC_GAME_PWD="$(/usr/local/bin/TeaEncrypt "$DNF_DB_GAME_PASSWORD")"
R=/opt/remake/runtime
log() { echo "[rednf $(date +%T)] $*"; }

# 1) 渲染 /home/template/neople → /home/neople（与上游 llnut/dnf 相同的占位符）
if [ ! -f /home/neople/.rendered ]; then
  log "渲染配置"
  rm -rf /home/neople && cp -a /home/template/neople /home/neople
  find /home/neople -type f \( -name '*.cfg' -o -name '*.tbl' \) -print0 | xargs -0 sed -i \
    -e "s/GAME_PASSWORD/$DNF_DB_GAME_PASSWORD/g" -e "s/DEC_GAME_PWD/$DEC_GAME_PWD/g" \
    -e "s/SERVER_GROUP_NAME/$SERVER_GROUP_NAME/g" -e "s/SERVER_GROUP_DB/$SERVER_GROUP_DB/g" \
    -e "s/SERVER_GROUP/$SERVER_GROUP/g"
  touch /home/neople/.rendered
fi
mkdir -p /data /data/monitor_ip
echo "$PUBLIC_IP" > /data/monitor_ip/MONITOR_PUBLIC_IP

# 2) PVF / RSA 公钥（首次从上游 Script.tgz 解出；用户可替换 /data/Script.pvf）
if [ ! -f /data/Script.pvf ]; then
  log "解出上游 Script.pvf"; tar -xzf /home/template/init/Script.tgz -C /data Script.pvf
fi
ln -sf /data/Script.pvf /home/neople/game/Script.pvf
if [ ! -f /data/publickey.pem ]; then
  openssl genrsa -out /data/privatekey.pem 2048 2>/dev/null
  openssl rsa -in /data/privatekey.pem -pubout -out /data/publickey.pem 2>/dev/null
fi
cp /data/publickey.pem /home/neople/game/publickey.pem

# 3) 服务端写死连 127.0.0.1:3306/3307 → 转发到 MySQL 容器
for p in 3306 3307; do socat TCP-LISTEN:$p,bind=127.0.0.1,fork,reuseaddr TCP:$DB_HOST:$DB_PORT & done

# 4) 等数据库并写 db_connect（等价上游 init_main_db.sh）
log "等待 MySQL $DB_HOST:$DB_PORT"
until mysql -h"$DB_HOST" -P"$DB_PORT" -uroot -p"$DNF_DB_ROOT_PASSWORD" -e 'select 1' >/dev/null 2>&1; do sleep 2; done
bash "$R/db_connect.sh"

# 5) 拍卖/点券服 latin1：独立 HOME 下的 .my.cnf（见 docs/拍卖行.md）
for s in auction point; do printf '[UseSQL]\ndefault-character-set=latin1\n' > /home/neople/$s/.my.cnf; done

# 6) 拉起服务
exec bash "$R/dnfctl.sh" run
