# dnf_secsvr —— secsvr 三件套独立还原工作区

本目录是 **secsvr（安全服务）三件套**的专属还原工作区，与主工程 `dnf_decompile/`
（DNFServer 系服务还原）相互独立。三件套的源码树
（`/data/secci/ci/jenkins/workspace/g3_release_suse32*`，Neople 安全组 CI）与
`dnf_decompile` 中其他二进制（`/home/neople/source/DNFServer/...`、`ChannelOld/`、
`ServerLab/` 等）没有项目源码关联，故从主 order 队列移出，单独在此还原。

## 目录结构

```
dnf_secsvr/
├── README.md                  # 本文档（工作区总览与状态）
├── docs/
│   ├── decompile_order.md     # secsvr 专属还原顺序与策略
│   ├── df_secsvr_dwarf_restoration.md  # DWARF 桩还原报告（2026-08-09）
│   ├── df_secsvr_build_recon.md        # 构建方法与库结构重建报告（2026-08-09）
│   ├── df_secsvr_thirdparty_restoration.md  # 三方库还原阶段报告（2026-08-09）
│   └── df_game_secsvr_dbmw_basic_info.md  # game/secsvr/dbmw 基本信息（副本，含三件套指标）
├── build_recon/               # 重建的原始构建树骨架（目录 + 节点说明，无源码副本）
├── third_party/               # 三方库：rapidxml 1.13 / mysql 5.0.92（开源）+ tsf4g/tencrypt 复刻
├── tencrypt_new/              # 闭源加密库复刻（哈希族 11/11 + 对称族 24 个已逐字节验证）
├── source/                    # DWARF 桩源码（文件镜像 + 类型信息 + 参数/变量名，函数体为空）
│   ├── gunnersvr/
│   ├── zergsvr/
│   └── secagent/
└── toolchain/
    └── gen_dwarf_stubs.py     # DWARF 桩生成器（自包含）
```

## 目标二进制（原始 ELF 位于 dnf_installer）

| 二进制 | 原始路径 | 大小 | C++ CU | 源码树 tag |
|---|---|---:|---:|---|
| gunnersvr | `neople/secsvr/gunnersvr/gunnersvr` | 13.5 MB | 88 | `g3_release_suse32` |
| zergsvr | `neople/secsvr/zergsvr/zergsvr` | 15.5 MB | 98 | `g3_release_suse32_bugfix_tag296` |
| secagent | `neople/secsvr/zergsvr/secagent` | 18.7 MB | 150 | `g3_release_suse32_bugfix_tag435` |

## 当前状态（2026-08-09）

- ✅ 三个二进制已完成 **DWARF 桩还原**：全部工程编译单元与头文件镜像、类型信息、
  参数/局部变量名；**函数体为空**（具体内容暂未动）。
- ✅ **三方库阶段进行中**：rapidxml 1.13 / mysql 5.0.92 已 vendor；
  tsf4g TDR 判定闭源（项目 Tdr* 头已重建）；tencrypt_new 哈希族 **11/11** 已实现并
  通过测试向量 + 符号级对照（104/104，见 `docs/df_secsvr_thirdparty_restoration.md`）。
- ⏳ 下一步：对称加密族（TenCrypt/TencBase + 块密码 47 个）→
  zenlib/protocol/framework 共享库 → gunnersvr/zergsvr/secagent 服务代码。
- 构建基线：32 位 / C++98 / O0；GCC 4.1.0 SUSE 为工程首次出现的工具链，需单独评估。

## 关联外部资源

- 原始 ELF：`../dnf_installer/build/dnf_data/home/template/neople/secsvr/...`
- 主工程（已不含 secsvr 队列）：`../dnf_decompile/`
