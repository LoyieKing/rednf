# third_party —— secsvr 三方库目录

本目录存放 secsvr 三件套（gunnersvr / zergsvr / secagent）还原所需的全部第三方库，
分两类：

- **开源库**：找到确切版本，vendor 上游原始发布包（保留溯源），并把编译所需头文件
  镜像到 `import/include/` 下，使桩源码里的 `#include "import/include/..."` 可直接编译。
- **闭源库**：基于二进制 DWARF 类型信息 + 符号表 + 公开算法参考实现**自行复刻**，
  每个库附 README 说明复刻依据与验收方式。

## 目录布局

```
third_party/
├── README.md                         # 本文档
├── rapidxml-1.13/                    # 开源：rapidxml 1.13（SourceForge 官方 zip + 说明）
├── mysql-5.0.92/                     # 开源：MySQL 5.0.92（downloads.mysql.com 官方 tar.gz + 说明）
├── tsf4g_tdr/                        # 闭源复刻：tsf4g TDR 运行库头（来自 DWARF 类型信息）
├── tencrypt_new/                     # 闭源复刻：TenCrypt/TenHash 加密库（tencrypt_new 独立工程）
└── import/include/                   # 编译镜像：满足桩源码 include 路径的最终头文件集合
    ├── opensource/rapidxml/rapidxml/ #   ← 来自 rapidxml-1.13
    ├── opensource/mysqlclient/       #   ← 来自 mysql-5.0.92
    ├── tencore/tencrypt/             #   ← 复刻（TenCrypt.h / TenHash.h）
    └── tencore/tsf4g/tsf4g_base/     #   ← 复刻（tdr/ + tbus/）
```

## 版本与来源（证据见各子目录 README）

| 库 | 类别 | 版本 | 来源 | 二进制内证据 |
|---|---|---|---|---|
| rapidxml | 开源 | 1.13 | SourceForge rapidxml-1.13.zip | include 闭包 3 头、RAPIDXML_PARSE_ERROR 宏族 |
| mysqlclient | 开源 | 5.0.92 | downloads.mysql.com mysql-5.0.92.tar.gz | MYSQL_VERSION_ID=50092（.debug_macinfo） |
| tsf4g TDR | 闭源 | tsf4g base SDK（无公开源码） | DWARF 复刻 | TDR_* 宏族、tsf4g_tdr 命名空间 87 个函数 |
| tencrypt_new | 闭源 | Tencent TSS SDK | DWARF 复刻 | TenMd5/TenSha1/CTwofish 等 59 个 TU 符号 |

## 使用方式

编译任一服务时添加 include 路径：

```
-I <workspace>                        # 桩源码根（src/...、import/include/... 相对路径）
-I <workspace>/third_party/import/include   # 三方库最终头（含复刻）
```

桩源码的 include 形如 `#include "import/include/opensource/..."`，由上述两条路径共同解析。
开源库保留的原始发布包仅作溯源；编译一律走 `import/include/` 镜像。
