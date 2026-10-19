# remake/secsvr —— secsvr 三件套还原工作区

状态（2026-10-20）：三件套的 DWARF 结构/符号镜像已基本齐全；**可运行、可部署的重建尚未完成**。
此前“2026-08-10 全部完成 / 0 MISSING / 行为对拍通过”的表述不再作为完成依据：
符号存在和链接成功不等于函数体完整，当前仍有空 `main()` 与桩 TU。构建骨架、子目录 README
只描述目录角色，状态以本文为准。

## 1. secsvr 是什么

secsvr 是 TSS 安全平台的三个独立进程，与 `df_game_r` 中的 `libtss_sdk.so.1.0.394` 配合工作：

| 组件 | 角色 |
|---|---|
| **gunnersvr** | TSS 平台对外安全接入与文件扫描服务；9 个专属 CU，启动参数 `-t30 -i1`（超时/间隔）。 |
| **zergsvr** | zerg 内部通信/控制服务；15 个专属 CU，负责 TCP/UDP 控制、自动连接、计费、IP 限制等。 |
| **secagent** | 反外挂 agent；16 个专属 CU，处理 AntiBot/DP/TSS 消息、用户与游戏服通道、封禁/限制、账单与监控。 |
| **tss_sdk** | `df_game_r` 进程内的安全 SDK（`libtss_sdk.so.1.0.394`），不是独立进程；负责 DP 协议、AntiBot 通道、SafeMode/Captcha/UIC 插件。 |
| **tencrypt_new** | 三件套静态链接的 TenCrypt/TenHash 加密库复刻；gunnersvr/zergsvr 使用哈希子集，secagent 使用全部工程 TU。 |
| **third_party** | 构建期第三方依赖：rapidxml 1.13、MySQL client 5.0.92，以及 TSS 头/参考材料。 |

主要数据路径：

```text
df_game_r (libtss_sdk)
   ├─ 玩家 TCP / 游戏包：CUser::m_network
   └─ /dev/shm/sec_tss_sdk_bus_N  <->  secagent  <->  gunnersvr / zergsvr
                                      └─ /tmp/dp_comm_config*  <->  libtss_sdk
```

`secagent` 与游戏服通过共享内存通道交换 TSS/AntiBot 数据；`libtss_sdk` 在游戏进程内完成
DP 加解密与包过滤。SDK 的源码与构建说明见 [`tss_sdk/README.md`](tss_sdk/README.md)。

## 2. 目录结构

```text
remake/secsvr/
├── README.md                  # 本文（角色、真实状态、TODO、验收口径）
├── CMakeLists.txt             # 三件套 CMake 构建入口（gunnersvr/zergsvr/secagent）
├── cmake/SecsvrCommon.cmake   # 编译/链接规则
├── compat/                    # 现代 GCC/glibc 兼容头（ext/hash_fun.h、hashtable.h、libio.h）
├── docs/                      # 还原报告、构建方法、build_recon/ 结构骨架
├── source/                    # DWARF 镜像源码：gunnersvr / zergsvr / secagent
├── tencrypt_new/              # TenCrypt/TenHash 复刻（独立 Makefile，CMake 复用）
├── third_party/               # rapidxml 1.13 / MySQL client 5.0.92 / references
├── tools/                     # DWARF 提取、符号/行为对拍、uni_call 等工具
└── tss_sdk/                   # 游戏服安全 SDK 重建（独立构建，228 个 TU）

原始 ELF 位于 `.origin_1031/build/dnf_data/home/template/neople/secsvr/...`；
Docker 运行时相关脚本在仓库根 `docker/`，不在本目录。

## 3. 当前完成度（按“可运行/可替换原版”标准）

| 组件 | 源码规模 | 当前状态 | 结论 |
|---|---|---|---|
| **gunnersvr** | 88 个 C++ CU，362 个文件 | 入口 `main` 已实现；CMake 构建通过；7/88 个 `.cpp` 仍带 DWARF stub 标记 | 可构建，行为未验收 |
| **zergsvr** | 98 个 C++ CU，418 个文件 | 入口 `main` 已实现；CMake 构建通过；9/98 个 `.cpp` 仍带 stub 标记 | 可构建，行为未验收 |
| **secagent** | 150 个 C++ CU，538 个文件 | `secagent_main.cpp` 的 `main()` 为空；CMake 构建通过；16/150 个 `.cpp` 仍带 stub 标记；`TenHash.cpp` 有 HAVAL/RIPEMD/GOST TODO | 可构建但**不可运行** |
| **tss_sdk** | 228 个编译单元 | `build_server.py` 构建通过（228/228），链接无未解析符号 | 可构建；独立于 secsvr 三件套 |
| **tencrypt_new** | 58 个对象（Makefile） | 编译通过；`make test` 的哈希/向量测试 **ALL PASS** | 算法库主体可用；待接 secsvr 全量符号/行为验收 |
| **third_party** | 头文件/参考材料 | rapidxml、MySQL client 已 vendor；TSS 头由 DWARF 复刻 | 构建期依赖，当前无独立运行时 |

### 3.1 必须明确的事实

1. `source/secagent/src/formmog/secagent/secagent_main.cpp:178-183`：
   `init_cfg_info()` 与 `main()` 均为空函数。按当前源码链接出的 secagent 不会执行启动逻辑。
2. `source/secagent/src/TenHash.cpp` 仍有 `TODO(haval/ripemd/gost)`；
   `tencrypt_new` 的实现不能自动视为 `source/secagent` 对应 TU 已完成。
3. CMake 入口已存在并可构建三目标（`CMakeLists.txt` / `cmake/SecsvrCommon.cmake`）；
   `docs/build_recon/` 仍是结构骨架。构建通过只是格式/链接门槛，行为与 live 验收仍未完成。
4. `0 MISSING`、`nm` 符号命中、仓库中存在文件，都不能证明函数体、启动流程或协议行为完成。
5. **rednf 当前部署仍使用原版 secsvr**：`docker/Dockerfile` 从 `.origin_1031` 复制
   `home/template`；`docker/runtime/dnfctl.sh` 启动原版 `gunnersvr` / `zergsvr` / `secagent`，
   不是本目录源码构建的 ELF。
6. 原版 secagent/gunnersvr/zergsvr 使用 `shm_open("sec/tss_sdk_bus_N")`；现代 glibc 对含 `/`
   的名字返回 `EINVAL`。因此 `docker/runtime/dnfctl.sh` 仍通过
   `LD_PRELOAD=libtss_shmfix.so` 归一化名字；在三个对端全部换成使用合法 POSIX 名的重建 ELF
   之前，`remake/runtime/libtss_shmfix.c` 不能删除。

## 4. 未完成项与 TODO

### P0：让 secagent 能启动

- [ ] 实现 `secagent_main.cpp` 的 `init_cfg_info()` 与 `main()`：参数解析、配置加载、
      `SecAgentApp` 初始化/运行/退出、信号与日志。
- [ ] 逐项核对 `SecAgentApp::init/proc/process_recv_appframe` 与 `SecAgentGamesvrChannel`
      的启动顺序，确认共享内存通道在 game 之前/之后建立的时序。
- [ ] 清掉 secagent 剩余 16 个 stub 标记 TU 中真正未实现的函数，至少覆盖启动、消息处理、
      用户管理、限制/封禁、账单、定时器与文件加载路径。

### P0：建立可复现构建

- [ ] 为 gunnersvr / zergsvr / secagent 增加检入的构建入口（Makefile 或 CMake），固定：
      GCC 4.1.x/4.1.2 工具链、`-m32 -O2 -DNDEBUG -pthread`、include 顺序、
      `tencrypt_new` 与 `third_party` 的链接方式。
- [ ] 产出到固定输出目录，并记录命令、版本、SHA-256 与符号清单。
- [ ] 禁止再用 `/tmp` 临时链接结果作为完成证据；重建 ELF 必须可重复生成。

### P1：补齐三件套行为

- [ ] `gunnersvr`：补齐剩余 7 个 stub TU，验证文件扫描、上报、统计与超时路径。
- [ ] `zergsvr`：补齐剩余 9 个 stub TU，验证 TCP/UDP 控制、自动连接、计费与 IP 限制。
- [ ] `secagent`：完成 AntiBot/DP 消息、加解密、用户/频道、限制与封禁、账单与监控。
- [ ] `tencrypt_new`：对 secagent 全量符号与调用点做最终对拍；把
      `source/secagent/TenHash.cpp` 的 HAVAL/RIPEMD/GOST TODO 与已实现算法对齐。
- [ ] 清理 `tencrypt_new/libtencrypt_new.a` 的 root 所有权；普通用户执行 `make` 会因无法覆盖
      该归档报 `Permission denied`（可临时用 `make LIB=/tmp/libtencrypt_new.a test`）。

### P1：真实运行验收

- [ ] 在同一 IPC 命名空间启动 game + secsvr 三件套，确认
      `AntibotInterface::Initialize, rc=0`、共享内存通道建立、选角/进城镇/换线正常。
- [ ] 对 `gunnersvr`/`zergsvr`/`secagent` 分别记录启动、收包、超时、退出与重启行为。
- [ ] 用真实 `df_game_r` 会话验证 AntiBot 上报、DP 包、SafeMode/Captcha/UIC 路径；
      不以“进程能起来”或“日志有输出”代替协议/状态验收。

### P2：切换 rednf 部署

- [ ] 用重建 ELF 替换 `docker/Dockerfile` 中来自 `.origin_1031` 的 secsvr 运行时，
      同步更新 `docker/runtime/dnfctl.sh` 与相关文档。
- [ ] 只有在所有对端都使用合法 POSIX 共享内存名（如 `/sec_tss_sdk_bus_N`）后，
      才删除 `remake/runtime/libtss_shmfix.c` 及 `dnfctl.sh` 中的 `LD_PRELOAD`；
      否则保留 shim，并把它标注为“原版 secsvr 兼容层”。
- [ ] 切换前保留原版 ELF 与回滚路径，记录逐进程 SHA-256 与 live 对照结果。

## 5. 构建与运行现状

- **secsvr 三件套**：`cmake -S remake/secsvr -B build/secsvr -DCMAKE_BUILD_TYPE=Release` +
  `cmake --build build/secsvr -j`。`docs/build_recon/` 仅用于还原 CI 目录结构，不是可执行构建；
  三目标仍待行为/live 验收。
- **tss_sdk**：`cd remake/secsvr/tss_sdk && python3 build_server.py --jobs 8 --output-dir /tmp/tss_sdk_build`
  （首次需运行 `toolchains/bootstrap.sh` 生成 `toolchains/c5root`）。
- **tencrypt_new**：`make` / `make test`；当前工作区归档文件被 root 占用时使用
  `make LIB=/tmp/libtencrypt_new.a test`。
- **原始 ELF**：`.origin_1031/build/dnf_data/home/template/neople/secsvr/...`。
- **共享内存兼容层**：`remake/runtime/libtss_shmfix.c`，当前仍是原版 secsvr 运行的必要条件。

## 6. 完成标准

secsvr 三件套只有同时满足以下条件才算完成：

1. gunnersvr / zergsvr / secagent 都能从检入源码可复现地构建；
2. 三者的 `main` 与全部启动路径有真实实现，不再有未实现的核心 TU；
3. 符号、协议、行为与原始 ELF 对拍通过，并有独立 oracle/真实会话证据；
4. 与真实 `df_game_r` 完成登录→选角→进城镇/换线等 live 链路；
5. rednf Docker 实际运行重建 ELF，原版 ELF 仅作为回滚/对照；
6. 共享内存名在源码层统一为合法 POSIX 名，`libtss_shmfix.c` 可删除，或明确记录为
   仅用于未重建原版进程的兼容层。

## 7. 证据与相关文档

- `docs/df_secsvr_build_recon.md`（dnf_workspace）：原始构建方法、工具链、三方库与模块依赖。
- `source/secagent/src/formmog/secagent/secagent_main.cpp:178-183`：空 `main()`。
- `source/secagent/src/TenHash.cpp`：HAVAL/RIPEMD/GOST TODO。
- `tss_sdk/README.md`：SDK 构建、协议与 TerSafe 交互边界。
- `docker/Dockerfile`：从 `.origin_1031` 复制原版 secsvr 运行时。
- `docker/runtime/dnfctl.sh`：原版三件套 + `libtss_shmfix.so` 预加载。
- `remake/runtime/libtss_shmfix.c`：共享内存名兼容层。
