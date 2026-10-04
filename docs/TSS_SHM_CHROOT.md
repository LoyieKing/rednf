# Remake TSS shm + 进城镇 DIAG 注意

## 选角空槽
`encReq=0`、缺 ORIG `561×3`、选角无字时先查 antibot：

1. `dnf-game-keeper` 必须 `--ipc=host`
2. chroot 前：`mount --bind /dev/shm /opt/dnf1031/dev/shm`
3. `LD_PRELOAD=/home/neople/game/libtss_shmfix.so`

成功：`AntibotInterface::Initialize, rc=0`

启动：`/opt/dnf1031/home/neople/game/start_siroco11_tss.sh`

## 进城镇 Loading / 不能动
选角通了仍可能卡 Loading：检查 `insert_game_world.inc` 是否残留崩溃二分 DIAG
（跳过 `send_clear_quest_list` / `get_quest_info` / post-339 extras）。必须走 ORIG 发包路径。
