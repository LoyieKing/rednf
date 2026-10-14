#!/bin/bash
# dnfctl.sh run|stop|restart-game <频道>|status
#   停服一律先 SIGTERM，等进程自己存档退出，超时才 SIGKILL（见 docs/运维.md）
[ -f /run/rednf.env ] && . /run/rednf.env
N=/home/neople; LOGD=/data/log; mkdir -p $LOGD
G=${SERVER_GROUP_NAME:-siroco}
svc() { # svc <name> <dir> <cmd...>
  local name=$1 dir=$2; shift 2
  ( cd "$dir" && rm -f pid/*.pid && exec "$@" ) >>"$LOGD/$name.out" 2>&1 &
  echo "[dnfctl] start $name pid=$!"
}
wait_port() { for _ in $(seq 1 60); do nc -z 127.0.0.1 "$1" 2>/dev/null && return 0; sleep 1; done; echo "[dnfctl] 等待端口 $1 超时"; }
term_wait() { # term_wait <pgrep pattern> [timeout]
  local pat="$1" t=${2:-60}; pkill -TERM -f "$pat" || return 0
  for _ in $(seq 1 "$t"); do pgrep -f "$pat" >/dev/null || return 0; sleep 1; done
  echo "[dnfctl] $pat 未在 ${t}s 内退出，SIGKILL"; pkill -KILL -f "$pat"
}
start_game() { # start_game <频道号>
  local ch=$1 seq=3; [ "$ch" -ge 52 ] && seq=5
  local name="$G$ch"
  sed -e "s/MAIN_BRIDGE_IP/${MAIN_BRIDGE_IP:-127.0.0.1}/g" -e "s/CHANNEL_NO/$ch/g" -e "s/PROCESS_SEQUENCE/$seq/g" \
      -e "s/PUBLIC_IP/$(cat /data/monitor_ip/MONITOR_PUBLIC_IP)/g" -e "s/DEC_GAME_PWD/$DEC_GAME_PWD/g" \
      -e "s/SERVER_GROUP/$SERVER_GROUP/g" $N/game/cfg/server.template > $N/game/cfg/$name.cfg
  ( cd $N/game && rm -f pid/$name.pid && LD_PRELOAD=$N/game/libtss_shmfix.so LD_LIBRARY_PATH=$N/game exec ./df_game_r $name start ) >>"$LOGD/game_$name.out" 2>&1 &
  echo "[dnfctl] start game $name"
}
case "${1:-run}" in
run)
  sed -e "s/MAIN_BRIDGE_IP/${MAIN_BRIDGE_IP:-127.0.0.1}/g" -e "s/PUBLIC_IP/$(cat /data/monitor_ip/MONITOR_PUBLIC_IP)/g" \
      -e "s/SERVER_GROUP/$SERVER_GROUP/g" /home/template/neople/channel/cfg/server.cfg > $N/channel/cfg/channel.cfg
  svc stun      $N/stun      ./df_stun_r start
  svc monitor   $N/monitor   ./df_monitor_r server start
  svc manager   $N/manager   ./df_manager_r server start
  svc relay     $N/relay     ./df_relay_r relay_200 start
  svc bridge    $N/bridge    ./df_bridge_r server start;  wait_port 7000
  svc channel   $N/channel   ./df_channel_r channel start
  svc dbmw_guild $N/dbmw_guild ./df_dbmw_r server_02 start
  svc dbmw_mnt  $N/dbmw_mnt  ./df_dbmw_r server_01 start
  svc dbmw_stat $N/dbmw_stat ./df_dbmw_r server_03 start
  HOME=$N/auction svc auction $N/auction ./df_auction_r ./cfg/server.cfg start ./df_auction_r
  HOME=$N/point   svc point   $N/point   ./df_point_r ./cfg/server.cfg start df_point_r
  svc guild     $N/guild     ./df_guild_r server start
  svc statics   $N/statics   ./df_statics_r server start
  svc coserver  $N/coserver  ./df_coserver_r server start
  svc community $N/community ./df_community_r server start
  # secsvr（TSS 反外挂）：ORIG ELF，来自 .origin_1031；与 df_game_r 同容器共享 IPC/shm
  ( cd $N/secsvr/gunnersvr && exec ./gunnersvr -t30 -i1 ) >>"$LOGD/gunnersvr.out" 2>&1 &
  ( cd $N/secsvr/zergsvr && exec ./zergsvr -t30 -i1 ) >>"$LOGD/zergsvr.out" 2>&1 &
  sleep 3
  ( cd $N/secsvr/zergsvr && exec ./secagent ) >>"$LOGD/secagent.out" 2>&1 &
  sleep 5
  for ch in ${OPEN_CHANNEL//,/ }; do start_game "$ch"; done
  trap 'echo "[dnfctl] 收到停止信号"; bash "$0" stop; exit 0' TERM INT
  while sleep 30; do :; done ;;
stop)
  term_wait 'df_game_r' 120          # 先停 game：TERM 后它会把在线角色存档
  for p in df_community_r df_coserver_r df_statics_r df_guild_r df_point_r df_auction_r df_dbmw_r \
           df_channel_r df_bridge_r df_relay_r df_manager_r df_monitor_r df_stun_r secagent zergsvr gunnersvr; do
    term_wait "$p" 30; done ;;
restart-game)
  ch=${2:?频道号，如 11}; term_wait "df_game_r $G$ch " 120; start_game "$ch" ;;
status)
  ps -eo pid,etime,args | grep -E 'df_|svr|secagent' | grep -v grep ;;
esac
