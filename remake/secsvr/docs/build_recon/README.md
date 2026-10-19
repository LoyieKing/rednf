# build_recon —— secsvr 原始构建树骨架

> **状态（2026-10-20）**：本目录只是 CI 构建树的结构镜像（目录 + 说明），**不包含源码副本，
> 不是可运行实现**。三件套的真实完成度、未完成项与 TODO 统一见
> [`../../README.md`](../../README.md)。不要以本目录存在、文件数或“链接成功”判断 secsvr 已完成。

## 作用

记录原始 CI workspace（`/data/secci/ci/jenkins/workspace/g3_release_suse32*`）的构建布局，
供补桩、建立可复现构建入口和核对 include/输出路径使用：

```text
src/                 主工程源码树（commlib / commsvr / formmog / protocol）
output/              各库 release/include 安装头（lib/ 为推断）
import/include/      三方/内部 SDK 头（rapidxml、mysqlclient、tencrypt、tsf4g）
include/ inc/         tencrypt_new 独立工程的对外/内部头
```

实际桩源码在 `../../source/{gunnersvr,zergsvr,secagent}/`；原始构建方法见
`../df_secsvr_build_recon.md`。

## 子目录 README 的口径

`src/`、`src/commlib/`、`src/protocol/`、`src/commsvr/` 等子目录 README 只说明目录角色与
CU 归属，不代表该模块已实现。各服务节点的当前状态：

| 节点 | 状态 |
|---|---|
| `src/commsvr/gunnersvr/` | 入口 `main` 已实现；仍有 stub TU；无可复现构建/ELF |
| `src/commsvr/zergsvr/` | 入口 `main` 已实现；仍有 stub TU；无可复现构建/ELF |
| `src/formmog/secagent/` | `main()` 为空；仍有 16 个 stub 标记 TU；**不可运行** |

## TODO

- [ ] 基于本骨架建立三件套的可复现 Makefile/CMake，固定工具链、宏、include 顺序与链接库。
- [ ] 每补完一个 CU 后更新对应服务节点 README 的“已实现/待补”列表。
- [ ] 产出 ELF 后记录 SHA-256、`nm` 符号清单与 live 验收结果；不要把结构骨架当完成证据。
