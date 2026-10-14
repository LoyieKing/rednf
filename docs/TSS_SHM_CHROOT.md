# 非 Docker（chroot）部署时的 TSS 共享内存

Docker 镜像里 game 与 secsvr 同容器，无需额外处理。若像早期那样把运行时放在 chroot
（例如 `<ROOT>=/opt/dnf1031`）里、由一个容器或宿主进程拉起，需要：

1. 拉起 chroot 的容器使用 `--ipc=host`（secsvr 与 game 必须在同一 IPC 命名空间）；
2. 进入 chroot 前：`mount --bind /dev/shm <ROOT>/dev/shm`，并确认存在 `<ROOT>/dev/shm/sec_tss_sdk_bus_*`；
3. 启动 game 时 `LD_PRELOAD=/home/neople/game/libtss_shmfix.so`（源码 `remake/runtime/libtss_shmfix.c`）。

示例：

```bash
mountpoint -q <ROOT>/dev/shm || mount --bind /dev/shm <ROOT>/dev/shm
chroot <ROOT> /bin/bash -lc 'cd /home/neople/game && LD_PRELOAD=/home/neople/game/libtss_shmfix.so exec ./df_game_r siroco11 start'
```

成功标志：`AntibotInterface::Initialize, rc=0`。另外不要在 `insert_game_world` 留下任何调试跳过
（CLEAR_QUEST / QUEST_INFO / post-339），否则进城镇一直 Loading。
