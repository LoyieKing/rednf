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
├── tencrypt_new/              # 闭源加密库复刻（哈希族 11/11 + 对称加密族 47 个全部完成）
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

## 当前状态（2026-08-10 完成）

**三个二进制的全部工程源码已还原并验收**（符号 MISSING=0、行为对拍通过、32 位可链接）：

| 二进制 | CU 数 | 覆盖符号 | 结果 |
|---|---:|---:|---|
| gunnersvr | 88 | 1463（源码映射）| 0 MISSING |
| zergsvr | 98 | 1541 | 0 MISSING |
| secagent | 150 | 2875 | 0 MISSING |

- ✅ **tencrypt_new**：59 CU 全部实现（哈希族 11 + 对称加密族 47 + TencBase/TsLocal），
  uni_call 与二进制逐字节对拍。
- ✅ **zenlib**：三树 128 文件全部实现，符号 0 缺失。
- ✅ **protocol/common**：三二进制 38 文件（Tdr 运行库 + comm_conf_* + comm_proto_public_*）
  全部实现；TDR 大端线格式、日期/时间/IP 编码、XML 读写与二进制逐字节一致。
- ✅ **formmog 协议**（secagent）：antibot_client 980 + conf_secagent 48 + public_secsvr 162
  + public_tsssdk 90。
- ✅ **framework**：gunnersvr 13 / zergsvr 15 / secagent 15 文件全部实现
  （含 zergsvr mml 三件套、secagent transaction 两件套）。
- ✅ **服务代码**：gunnersvr 9 / zergsvr 15 / secagent 16 文件全部实现。
- ✅ **全量链接**：三个二进制已用 32 位 GCC 4.1.2 对象 + 宿主 32 位运行库成功链接为
  ELF32 可执行文件（/tmp/gunnersvr_rebuilt、/tmp/zergsvr_rebuilt、/tmp/secagent_rebuilt）；
  与原二进制逐符号对照：**全部函数符号命中（缺失 0）**，仅剩编译器版本产物
  （GCC 4.1.0 vs 4.1.2 的静态局部表命名 C.* / __PRETTY_FUNCTION__、libstdc++ 内联决策、
  闭源密码实现数据表命名）——此类数据表的值已逐字节对拍，仅符号名/作用域不同。
- 行为验收：construct/pack/unpack/visualize_ex/fromXml/toXml、TdrParse 168 向量、
  TdrBufUtil 42 项、TdrDate 14 组日期矩阵、zergsvrd.xml 真实解析等全部与二进制对拍一致。

还原方法、工具链与逐阶段记录见 `docs/`（decompile_order / build_recon /
dwarf_restoration / thirdparty_restoration）。

## 关联外部资源

- 原始 ELF：`../dnf_installer/build/dnf_data/home/template/neople/secsvr/...`
- 主工程（已不含 secsvr 队列）：`../dnf_decompile/`
