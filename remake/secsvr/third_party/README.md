# third_party —— secsvr 三方库与参考材料

> 状态（2026-10-20）：本目录是 **构建期依赖**，不是运行时组件。secsvr 三件套的可运行性、
> 未完成项与 TODO 见 [`../README.md`](../README.md)。不要把“头文件存在”当作三件套已重建完成。

## 在 secsvr 中的角色

gunnersvr / zergsvr / secagent 是静态链接的 32 位 ELF；本目录提供编译所需的第三方头、
开源源码镜像与算法参考材料：

| 目录 | 内容 | 用途 |
|---|---|---|
| `rapidxml-1.13/` | RapidXml 1.13 原始发布包与说明 | XML 配置/协议解析（`comm_conf_*`、TDR XML） |
| `mysql-5.0.92/` | MySQL client 5.0.92 原始发布包与说明 | `zen_mysql_*` 封装与 DB 访问 |
| `import/include/` | 编译时头文件镜像 | 让桩源码的 `#include "import/include/..."` 可解析 |
| `references/` | Applied Cryptography / AC 源码等参考材料 | `tencrypt_new` 算法复刻与对拍（dnf_workspace 保留；rednf 发布树按需裁剪） |

TSS 专用头（`tencrypt`、`tsf4g`）不复制在本目录：

- `tencrypt_new/` 是独立加密库工程（`../tencrypt_new/`）；
- `tsf4g` TDR/TBus 头位于各服务的 `source/<binary>/import/include/tencore/tsf4g/`；
- 具体 include 顺序与构建方法见 [`../docs/df_secsvr_build_recon.md`](../docs/df_secsvr_build_recon.md)。

## 版本与证据

| 库 | 类别 | 版本 | 证据 |
|---|---|---|---|
| rapidxml | 开源 | 1.13 | include 闭包 3 头、`RAPIDXML_PARSE_ERROR` 宏族 |
| mysqlclient | 开源 | 5.0.92 | `MYSQL_VERSION_ID=50092`（`.debug_macinfo`） |
| tsf4g TDR/TBus | 闭源复刻 | TSS base SDK | `TDR_*` 宏族、`tsf4g_tdr` 命名空间符号 |
| tencrypt_new | 闭源复刻 | Tencent TSS SDK | 58 个对象；哈希/向量测试见 `../tencrypt_new/README.md` |

## 当前状态

- 开源库已 vendor，保留原始发布包/说明用于溯源；编译走 `import/include/` 镜像。
- 闭源 TSS 头按 DWARF 类型/符号复刻；实现主体在 `../tencrypt_new/` 与 `../source/`。
- 本目录没有独立构建/测试入口，也没有运行时逻辑。

## TODO

- [ ] 记录并校验 `rapidxml-1.13`、`mysql-5.0.92` 的 SHA-256、来源 URL 与许可证文件。
- [ ] 核对 `import/include/` 镜像与各服务 `source/<binary>/import/include/` 是否一致，
      避免两套头漂移。
- [ ] 在 secsvr 统一构建入口中固定 include 顺序、静态库/对象链接顺序与宏定义。
- [ ] 补充 `references/` 中算法参考材料的来源、版本与对拍向量索引；
      rednf 发布树若裁剪 `references/`，在构建 README 中明确说明。
