# DNF 服务端逆向重建工程（dnf_decompile）

## 项目定位

本工程是 **DNF（地下城与勇士）服务端逆向分析与源码重建工作区**，与 `dnf_installer`（DNF 服务端一键安装部署仓库，本仓库的 git submodule）配套使用：

- `dnf_installer/` 提供部署资源与**原始服务器 ELF 运行时文件**，位于 `dnf_installer/build/dnf_data/home/template/neople/<服务名>/df_<服务名>_r`。
- `dnf_decompile/` 对这些 ELF 进行 Ghidra 逆向，并结合 DWARF 调试信息渐进式重建可维护的 C++ 源码。
- 重建产物统一放在 `source/` 下，通过 CMake 以「C++98 / 32 位 / O0」基线构建，尽量贴近原始二进制行为。

## 工作区布局

```
dnf_workspace/                      # git 仓库根
├── .github/copilot-instructions.md # 协作规则（逆向、构建、对比模式、沟通风格）
├── dnf_installer/                  # submodule：部署仓库，含原始 ELF 运行时
└── dnf_decompile/                  # 本工程：Ghidra 工程 + 重建源码
    ├── dnf_project.rep/            # Ghidra 工程数据（projectState / idata / user / versioned）
    ├── README.md                   # 本文档
    └── source/
        ├── CMakeLists.txt          # 重建构建定义（全部服务目标）
        ├── Community/              # df_community_r 真实重建实现（37 文件 / 约 2.5k 行）
        ├── shared/                 # 包结构与通用头（185 文件 / 约 3.1k 行，早期重建成果）
        ├── ChannelOld/             # df_channel_r 真实重建实现（25 TU，见 docs/df_channel_r_progress.md）
        │                           #   + df_bridge_r 的 DWARF 桩（DNFChannelBridge/）
        ├── DNFServer/              # ServerCommon / GameServer(ServerLab) / StunServer 的 DWARF 桩
        ├── DNFShared/ Library/     # 共享脚本与工具库的 DWARF 桩
        ├── placeholder/            # 无 DWARF 服务的占位 main（6 个）
        ├── compare_df_community_functions.py  # 符号/字符串/助记符对比脚本
        └── toolchain/              # 可复现构建/逐 TU 对比脚本（build-channel.sh 等）
```

> 说明：`build-verify-community/` 是 df_community_r 的验证构建目录，其下两个验证报告为生成物，已被 `.gitignore` 忽略，未纳入版本管理。
> `dnf_project.gpr` / `dnf_project.rep/` 为 Ghidra 工程文件（已忽略），非重建产物。

## 服务端组件架构

原始运行时目录 `neople/` 下共 17 个组件，角色整理自 `dnf_installer/doc/ArchitectureDiagram.md`：

| 组件 | 职责 | 主要端口 |
|---|---|---|
| `relay` | 客户端无法直连 P2P 时作为中继 | 7200/tcp+udp |
| `stun` | NAT 穿透（UDP hole punching），P2P 组队关键服务 | 2311-2313/udp |
| `channel` | 分流情报确认与下发 | 7001/tcp+udp → bridge:7000 |
| `bridge` | 分流档案形成/传送、外挂物品概率收集、死亡之塔排名、储值 D 币 | 7000/tcp+udp |
| `community` | 决斗场登录/注销、决斗场相关服务 | 31100/tcp+udp |
| `coserver` | 阻止重复连线（全部 GAME SERVER 组） | 30703/tcp+udp |
| `manager` | 管理监控、服务器组间消息传播 | 40403/tcp+udp |
| `auction` | 拍卖场（金币交易） | 30803/tcp+udp → monitor:30303 |
| `point` | 拍卖场（点券/现金交易） | 30603/tcp+udp → monitor:30303 |
| `monitor` | 阻止重复登录、好友/黑名单、踢人、活动（burning time）等 | 30303/udp |
| `guild` | 公会创立/加入/退出/解散 | 30403/tcp+udp → dbmw_guild:20403 |
| `statics` | 统计服务 | 30503/udp |
| `game` | 游戏核心服务（登录处理、地下城、现金购买、创角等） | 用户自定义频道端口 |
| `dbmw_guild/dbmw_mnt/dbmw_stat` | 数据库中间件（随 CFG 分 3 种） | 20403/20203/20303 |
| `secsvr` | 安全服务 | — |

> `game`、`secsvr`、`dbmw_*` 目前不在 `source/CMakeLists.txt` 的重建目标范围内。

## 重建状态矩阵

以下信息综合自 `source/CMakeLists.txt`、原始 ELF 的 `.debug_info` 检测（`readelf`）与 2026-08-06 的实际全量构建验证。

| 服务 | CMake 目标 | 原始 ELF | 有 DWARF？ | 当前源码形态 | 可链接？ |
|---|---:|---|---|---|---|
| community | `df_community_r` | 968 KB | 否（Ghidra 反编译） | 真实实现（37 文件） | 是 |
| channel | `df_channel_r` | 1.3 MB | 是 | 真实实现（25 TU，含 DWARF 布局对齐） | 是 |
| auction | `df_auction_r` | 12.2 MB | 是 | 真实实现（66 TU，MISSING=0） | 是（`source/build-auction/`） |
| point | `df_point_r` | 12.2 MB | 是 | 真实实现（同源，`-DPOINT_SERVER` 切 3 处常量） | 是（`source/build-point/`） |
| bridge | `df_bridge_r` | 3.4 MB | 是 | 真实实现（31 TU，复用 channel 框架 + Ghidra 重建） | 是（`source/build-bridge/`，91.4% 助记符级精确） |
| stun | `df_stun_r` | 131 KB | 是 | 真实实现（8 文件，见 `docs/df_stun_r_restoration_report.md`） | 是（`source/build-stun/`，64 位） |
| dbmw | `df_dbmw_r` | — | — | 仅工具函数（Library/Core） | 否（缺 `main`） |
| relay / coserver / statics / guild / monitor | 独立脚本构建（build-*.sh） | 0.4–2.5 MB | 否 | 真实实现（monitor：`source/monitor/`，MISSING=0，见 `docs/df_monitor_r_progress.md`） | 是 |
| manager | 独立脚本构建（build-manager.sh） | 3.2 MB | 否 | 主体完成（`source/manager/`；MISSING=0，助记符重叠 88.96%，见 `docs/df_manager_r_validation.md`） | 是 |

**构建验证结论（2026-08-07 实测）**：`df_community_r`、`df_channel_r`（`source/toolchain/build-channel.sh`，GCC 4.4.6）、`df_stun_r` 与 `df_bridge_r`（`source/toolchain/build-bridge.sh`，同 4.4.6 工具链）均已完整链接并生成可运行 ELF；其余目标失败原因统一为 `undefined reference to main` —— 桩代码尚未包含入口函数，属预期状态而非编译配置问题。

**stun 位宽说明**：`df_stun_r` 原始 ELF 是 **64 位**，与 CMake 的 `-m32` 基线不同；
已用独立构建脚本 `source/toolchain/build-stun.sh`（64 位 + GCC 4.1.2）完成重建，
26 个唯一项目函数全部语义等价（助记符级 25/26），详见 `docs/df_stun_r_restoration_report.md`。

## df_community_r 重建与验证

`df_community_r` 是当前唯一完成主体重建的服务，无 DWARF 信息，基于 Ghidra 反编译结果人工重建（CMake 注释中标注来源为 df_playground 反编译）。

### 验证方法

`source/compare_df_community_functions.py` 以原始 ELF 与重建产物为输入：

1. `nm -S` 导出原始 ELF 的符号表（含大小），与重建对象文件符号对比，计算命中率；
2. 对比重建二进制中出现的字符串是否存在于原始 ELF；
3. 对可匹配符号提取指令助记符序列，计算逐函数重叠率。

### 关键结果（生成于 `source/build-verify-community/` 报告）

| 指标 | 值 |
|---|---:|
| 架构对齐 | ELF32 / Intel 80386，**均为 EXEC**；NEEDED 与原始完全一致 |
| 项目函数精确（可靠审计，2026-08-07 最终） | **403 / 490（82.2%）**（符号名级 **490/490，100%**） |
| 精确符号匹配 | **3399 / 3741（90.9%）**（初始 303；missing/extra 均为 libstdc++ 内部） |
| 字符串交集 | **4018**（初始 539） |
| 加权符号命中率 | 67.4%（初始 39.51%） |
| 平均助记符重叠率 | **77.72%**（初始 46.84%；精确一致 351 个，初始 0） |
| 项目级缺失函数 | **0**（已全部补齐） |

高命中文件（结构较接近）：`global.cpp`（96.8%）、`PacketDispatcher.cpp`（97.6%）、`ConfigFileReader.cpp`（92.9%）、`DNFFunctionLib.cpp`（91.6%）、`AbstractSocket.cpp`（85.2%）、`NetworkSession.cpp`（80.8%）。

低命中（多数为编译器版本差异，语义一致）：`PacketHeader.cpp`（42.9%）、`STGameUserInfo.cpp`/`STPvPBuddyDBInfo.cpp`（50.0%）、`Thread.cpp`（59.2%）、`main.cpp`（63.3%）、`SessionManager.cpp`（68.8%）。

> 2026-08-07 最终（决定性）：从原始 ELF 的 C++0x 特征符号（`decay_and_strip` make_pair、变参 construct、右值 push_back）确认**原始用 gnu++0x 编译**，CMakeLists 默认标准改为 gnu++0x。项目函数符号名精确匹配 **490/490（100%）**、MISSING/EXTRA 项目函数均为 0、精确符号匹配 3399/3741（90.9%）、字符串交集 4018、文件大小 877,264（原始 967,844）。剩余差异全部为原始「多编译器版本混合链接」（.comment: 4.1.2/4.4.4/4.4.6/4.4.7）与 libstdc++ 静态库内部实现差异，非项目源码可修。详见 `docs/df_community_r_function_divergences.md` 与 `docs/function_review/report_review_full_20260807.md`。

> 2026-08-09 三方库真实版本替换（见 `docs/tinyxml_boost_version_verify.md`）：经原始 ELF 常量/符号/DWARF 行号三重证据，确认 **TinyXML = 2.6.2、Boost = 1.48.0**。原始 tinyxml 是独立对象，由 GCC 4.4.6-3（guild 为 4.4.4-13）以 **-O3 + gnu++98 + 4.4.6 libstdc++ 头 + TIXML_USE_STL** 编译。已用纯净上游 2.6.2 四文件替换手写重建（`tinyxml.h/.cpp` + `tinyxmlerror.cpp` + `tinyxmlparser.cpp`），Boost 1.48.0 官方头 vendor 至 `source/Library3rd/Boost/Include`，删除手写 `BoostPool.h`。校验：auction/point tinyxml **149/150**、boost **120/121**（归一化 identical，仅剩 DNF 手工展开的 GetEntity 与编译器内联微差）；community（4.1.2 混合链）boost **46/46**、符号集 50/50。

## df_channel_r 重建与验证（2026-08-07）

`df_channel_r`（分流 channel 服务，32 位，约 1488 个命名函数，32 个 TU 均为 GCC 4.4.6）已完成全部类解码与逐 TU 对齐：

| 指标 | 值 |
|---|---:|
| 全 25 TU 编译 + 链接 | 通过（`source/toolchain/build-channel.sh`，`/tmp/final/df_channel_r`，`-static-libgcc`） |
| ChannelService（590 函数） | **578 逐字节一致**，其余 10 个为寄存器分配/分支布局差异（语义等价） |
| TCPThread（320） / CheckThread（111） | 312 / 102 逐字节一致，其余语义等价 |
| 小型 TU（Exception/System/ThreadLock/SocketSystem/Token/Globals/Thread 等） | 全部逐字节一致 |
| SHA-256 / AES（Rijndael） | 测试向量验证通过（按用户指示以算法正确性为准） |
| 真实缺失函数 | **0**（剩余 "EMPTY" 均为比较工具别名误报，符号存在且尺寸一致） |

关键对齐开关（详见 `docs/df_channel_r_progress.md`）：`ChannelServiceApp` 为 namespace 而非 class、
`-fno-enforce-eh-specs`、报文结构 `#pragma pack(1)`、TScopedLock 引用参数、TMsgCell `throw()`、
线程类基类顺序、CMsgCell/Channel/ChannelScript/TCPThread 成员布局、`-static-libgcc` 链接等。
原始工具链为 Red Hat GCC 4.4.6-3（`source/toolchain/c6-g++-446r` 驱动 + cc1plus_446）。

## 文档与报告

`docs/` 目录保存全部调研报告与验证数据：

- `docs/decompile_order.md` — 全部服务端二进制的反编译还原顺序（从易到难）
- `docs/df_community_r_reverification.md` — 2026-08-06 深度复核报告（构建参数/ABI、符号覆盖、数据段、包构造函数缺陷、修复清单）
- `docs/df_community_r_missing_functions.md` — 原始有、重建无的项目函数完整清单（分类）
- `docs/df_community_r_validation.md` / `df_community_r_function_validation.md` — 符号/字符串命中率与逐函数助记符重叠验证报告（生成物副本）
- `docs/df_community_r_function_divergences.md` — 全量逐函数比对报告（463 个函数，修正 15+10 处不一致）
- `docs/df_community_r_semantic_equivalence_proof.md` — 语义等价证明（490 个项目函数全部语义等价）
- `docs/df_channel_r_progress.md` — channel 服务还原进度（逐字节一致水位与对齐开关）
- `docs/df_stun_r_restoration_report.md` — stun 服务还原报告（26 个唯一函数语义等价）
- `docs/function_review/` — 第二轮 + 第 3~11 轮全量逐函数复核报告
- `docs/data/` — 全部对比明细数据（逐函数/逐文件 TSV、misalign 明细），生成脚本见 `docs/README.md`

完整索引见 `docs/README.md`。

## 构建方法

```bash
# 环境要求：gcc/g++ 支持 -m32（multilib），cmake >= 3.10
cmake -S source -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target df_community_r -j$(nproc)
```

构建基线（固定于 `source/CMakeLists.txt`）：

- C++98 + GNU 扩展（匹配原始 GCC 4.1.2 / RedHat）
- `-m32 -pthread -O0`，默认 Debug，全部构建类型强制 O0
- 32 位可执行文件，与原始 ELF 位宽对齐（stun 除外）

## 逆向与重建工作流（规则摘要）

完整规则见 `.github/copilot-instructions.md`，要点：

1. **事实与推断分离**：结论须标注「已从文件/日志验证」或「来自反编译/DWARF 的推断」，不得伪造确定性。
2. **源码对比模式**：Ghidra Headless 导出伪 C → 可读化整理 → LLM 逐函数比对 → 更新源码 → 编译验证。禁止脚本自动判定「逻辑一致」，禁止以编译通过代替逻辑判断。
3. **多文件批量分工**：主 Agent 负责 TODO 列表、审核结论、修改源码与编译验证；每个文件的 Ghidra 提取与逐函数比对须由 `decompiler` SubAgent 完成；SubAgent 不得直接改源码。
4. **命名稳定**：已引入的命名保持稳定，除非有强证据支持重命名。
5. **最小化变更**：优先保持 CMake 基线一致，避免大范围重构；路径引用必须精确（依赖严格运行时目录布局）。

## 当前缺口与建议下一步

1. 按验证报告复核 `df_community_r` 低命中文件（`PacketDispatcher`、`ConfigFileReader`、`SessionManager`、`main`、`SessionProxy`）。
2. 按下方「反编译还原顺序」推进各服务重建。
3. 将验证脚本与报告纳入可复现流程（当前报告位于 gitignored 目录，未纳入版本管理）。

## 反编译还原顺序（从易到难）

排序依据（2026-08-06 实测，08-08 补充 game/secsvr/dbmw）：文件规模、DWARF 完整度（编译单元数/覆盖源文件）、命名函数数量（不含 `.L` 局部标签）、字符串量、符号可读性，以及现有重建进度。`df_community_r`、`stun`、`channel`、`bridge`、`auction/point` 已完成主体重建，作为对照基准，不再列入队列。

**关键事实**

- 全 DWARF：`stun`、`channel`、`bridge`、`auction`、`point`（GCC 4.4.6，stun 为 GCC 4.1.2）、secsvr 三件套（91/101/153 CU，GCC 4.1.0 SUSE，共享 commlib 框架）。
- 部分 DWARF：`game`（21 CU 仅覆盖 `../../Include/` 共享库与静态 FreeType，GameServer 核心无调试信息）。
- 无 DWARF：`relay`、`coserver`、`statics`、`guild`、`monitor`、`manager`、`dbmw`（`dbmw` 仅有 2 个编译单元，仅覆盖 StackBuffer/StringFormat 两个工具文件，对服务主体基本无效）。
- `auction` 与 `point` 符号集合完全一致（11089 个符号 100% 重叠），同源不同配置，还原一个即得两个。
- `dbmw_guild / dbmw_mnt / dbmw_stat` 三个二进制 md5 相同，只需还原一次。
- `stun` 是唯一 64 位 ELF，重建时需单独评估位宽基线；其余均为 32 位。
- `df_game_r`、`gunnersvr`、`zergsvr`、`secagent` 已加入 Ghidra 工程 `dnf_project`（原 13 + 4 = 17 个程序）；基本信息与还原计划见 `docs/df_game_secsvr_dbmw_basic_info.md`。
- **`game` 优先级固定为最低**（队列末尾）：体量最大、GameServer 核心无 DWARF，任何时候不提前。

### 排序表

| 顺序 | 二进制 | 规模 | 命名函数 | 字符串 | DWARF | 现有进度 | 说明 |
|---|---:|---:|---:|---|---|---|
| ~~1~~ | ~~`stun`~~ | 131 KB | 27 | 3.2k | 完整（4 CU） | ✅ 已完成 | 26 个唯一函数全部语义等价（详见 `docs/df_stun_r_restoration_report.md`） |
| ~~2~~ | ~~`channel`~~ | 1.3 MB | 1,451 | 11.2k | 完整（32 CU） | ✅ 已完成 | 25 TU 编译链接通过，主体逐字节一致 |
| ~~3~~ | ~~`bridge`~~ | 3.4 MB | 3,812 | 30.2k | 完整（31 CU） | ✅ 已完成 | 918 个项目函数全部翻译、语义等价 |
| 4 | `auction`/`point`（一组） | 12.2 MB | 8,205 ×2 | 84.7k | 完整（66 CU） | ✅ 已完成 | 4736 项目函数全部还原；还原一个得两个 |
| 5 | `gunnersvr`（secsvr） | 13.5 MB | 1,883 | 307.1k | 完整（91 CU） | 已导入 | secsvr 三件套中最小；共享 commlib 框架 |
| 6 | `zergsvr`（secsvr） | 15.5 MB | 2,059 | 358.3k | 完整（101 CU） | 已导入 | 与 gunnersvr 共享 commlib |
| 7 | `secagent`（secsvr） | 18.7 MB | 4,035 | 388.2k | 完整（153 CU） | 已导入 | 反外挂 agent + 加密库 |
| 8 | `relay` | 418 KB | 1,921 | 3.1k | 无 | 主体实现（应用级 MISSING=0） | 无 DWARF 中最小的入门目标 |
| 9 | `coserver` | 1.1 MB | 2,987 | 6.1k | 无 | 应用层主体完成 | 功能单一（阻止重复连线） |
| 10 | `statics` | 1.7 MB | 6,059 | 9.7k | 无 | 主体实现（符号命中 99.6%） | 统计服务 |
| 11 | `guild` | 2.3 MB | 7,024 | 12.3k | 无 | 主体实现中（缺失 69） | 公会操作 |
| 12 | `monitor` | 2.5 MB | 7,899 | 13.3k | 无 | 占位 | 无 DWARF 中函数最多 |
| 13 | `manager` | 3.2 MB | 5,948 | 25.8k | 无 | 占位 | 字符串量最大，消息/管理面广 |
| 14 | `dbmw` | 4.0 MB | 7,816 | 32.1k | 几乎无效（2 CU） | 占位 | 无 DWARF 中体量最大；三副本 md5 相同只需还原一次 |
| 15 | `game` | 36.4 MB | 116,059（含静态库） | 212.0k | 部分（21 CU，核心无） | 已导入 | `../../Include/` 走 DWARF 补桩、GameServer 核心走 Ghidra、FreeType 黑盒；**优先级最低（固定队尾）** |

### 还原策略

- **有完整 DWARF 组**：沿用现有自动生成桩，按文件逐函数补全（签名、类布局、源文件路径均来自 DWARF）；secsvr 三件套共享 commlib 框架可交叉复用；注意 secsvr 为 GCC 4.1.0 SUSE 工具链。
- **`game`（部分 DWARF）**：`../../Include/` 按 DWARF 补桩，GameServer 核心按 Ghidra 路线，FreeType 等静态库黑盒处理。
- **无有效 DWARF 组**：走 Ghidra Headless 导出伪 C → 逐函数人工比对的社区流程；先做符号可读性最好的小目标（`relay`）积累经验，再按体量递增。
- 每完成一个服务，用 `compare_df_community_functions.py` 同款方法（符号/字符串/助记符重叠）产出一份验证报告，再进入下一个。

> 完整排序依据、指标与策略见 `docs/decompile_order.md`。
