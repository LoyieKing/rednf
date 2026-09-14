# 全部 df_* 二进制 identical 情况表

> 生成时间：**2026-09-14**
> 对照：`dnf_installer/build/dnf_data/home/template/` 下 ORIG ELF
> 重建：`dnf_decompile/build/<svc>/df_<svc>_r`

## 0. 口径

| 范围 | 脚本 | identical 定义 |
|---|---|---|
| 13 个服务 | `source/toolchain/gen_report_manifest.py` | `IDENTICAL` = `norm_identical`（仅 `j*`/`call`/`loop*` 目标 → `<T>`）；`IDENTICAL_AE` = 数据地址伪代码化后相等。两者都算 identical。 |
| `df_game_r` | `source/toolchain/compare_game_full.py` | `strict` + `ae` + `sem`（z3/angr 白名单）合计为 identical；`stl` 弱实例另行豁免，不计入非 identical。 |

共同约定：

- 符号范围：symtab 已定义命名函数 `T/t/W/w`，跳过 `.L*`、初始化守卫、libgcc/libc 运行时。
- 第三方豁免（tinyxml / 加密哈希 / yaSSL / Boost / STL / 工具链运行时 / MySQL·zlib 等）**不计入** `total_named`。
- `non_identical` = `NEAR + DIFF + EMPTY`（**不含** `MISSING`：后者多为第三方静态库符号，无源码修复意义）。
- 覆盖率 = `(identical + identical_ae) / total_named`（game 为 `identical合计 / 共同符号`）。

机器可读副本：`function_reports/manifest_summary.tsv`（13 服务）+ 本文件。

## 1. 总览（14 个二进制）

| 服务 | ORIG 字节 | OURS 字节 | 产物时间 | 命名函数 | identical | AE | 合计 | NEAR | DIFF | MISSING | 非 identical | 覆盖率 |
|---|---:|---:|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| **game** | 36360954 | 29168220 | 2026-09-13 04:36 | 36481 | 26858 | 8152 | **36113** | 45 | 291 | 2 | 336 | **99.0%** |
| auction | 12202759 | 4078528 | 2026-09-14 21:56 | 1677 | 923 | 666 | 1589 | 87 | 1 | 0 | 88 | 94.8% |
| point | 12202755 | 4078528 | 2026-09-14 21:56 | 1677 | 923 | 666 | 1589 | 87 | 1 | 0 | 88 | 94.8% |
| bridge | 3379127 | 2938624 | 2026-09-14 21:56 | 569 | 330 | 213 | 543 | 1 | 0 | 25 | 1 | 95.4% |
| channel | 1305013 | 505704 | 2026-09-14 21:56 | 640 | 348 | 292 | 640 | 0 | 0 | 0 | **0** | **100%** |
| community | 967844 | 952924 | 2026-09-14 21:56 | 278 | 189 | 89 | 278 | 0 | 0 | 0 | **0** | **100%** |
| coserver | 1073816 | 870352 | 2026-09-14 21:56 | 343 | 181 | 162 | 343 | 0 | 0 | 0 | **0** | **100%** |
| dbmw | 4041084 | 3747512 | 2026-09-14 21:56 | 1107 | 557 | 375 | 932 | 18 | 157 | 0 | 175 | 84.2% |
| guild | 2254651 | 1866088 | 2026-09-14 21:56 | 1549 | 1000 | 379 | 1379 | 17 | 153 | 0 | 170 | 89.0% |
| manager | 3199927 | 3130180 | 2026-09-14 21:56 | 600 | 344 | 256 | 600 | 0 | 0 | 0 | **0** | **100%** |
| monitor | 2461318 | 1928212 | 2026-09-14 21:57 | 1678 | 1088 | 434 | 1522 | 13 | 143 | 0 | 156 | 90.7% |
| relay | 418098 | 421112 | 2026-09-14 21:56 | 475 | 366 | 109 | 475 | 0 | 0 | 0 | **0** | **100%** |
| statics | 1748067 | 1491804 | 2026-09-14 21:56 | 842 | 579 | 234 | 813 | 1 | 28 | 0 | 29 | 96.6% |
| stun | 131126 | 743808 | 2026-09-14 21:56 | 20 | 3 | 14 | 17 | 3 | 0 | 0 | 3 | 85.0% |
| **13 服务小计** | | | | **11455** | **6831** | **3889** | **10720** | **227** | **483** | **25** | **710** | **93.6%** |

game 行的「命名函数」是两侧**共同符号**（`compare_game_full`）；另有仅 ORIG 2、仅 NEW 127、sem 1103（已计入 identical 合计 36113）、stl 32（不计入非 identical）。非 identical 336 = 45 near + 291 diff。

## 2. 按覆盖率排序

| 档 | 服务 | 覆盖率 | 非 identical |
|---|---|---:|---:|
| 已清空 | channel · community · coserver · manager · relay | 100% | 0 |
| ≥95% | game 99.0% · statics 96.6% · bridge 95.4% | | 336 / 29 / 1 |
| 90–95% | auction = point 94.8% · monitor 90.7% | | 88 / 88 / 156 |
| <90% | guild 89.0% · stun 85.0% · dbmw 84.2% | | 170 / 3 / 175 |

## 3. game 细目（`compare_game_full.py`）

```
共同 36481: strict=26858 ae=8152 sem=1103 (合计 identical=36113)
near=45 diff=291 stl=32 非identical=336 仅ORIG=2 仅NEW=127
```

| 桶 | 数量 | 说明 |
|---|---:|---|
| strict | 26858 | `norm_line` 逐条相等，且分支落点索引相等 |
| ae | 8152 | AE 归一（大地址→`<A>`、去 `@plt`、剥对齐 nop、落点重索引）后相等 |
| sem | 1103 | `equiv/whitelist_semantic_equiv.txt` 语义豁免，计入 identical |
| **identical 合计** | **36113** | strict + ae + sem |
| near | 45 | 助记符序列等长且助记符相同，操作数不同 |
| diff | 291 | 结构不同 |
| stl | 32 | `_ZNKSs` / 流类弱实例，不计入非 identical |
| 仅 ORIG | 2 | |
| 仅 NEW | 127 | |
| 产物 md5 | `7863d8e14e180e55c6a38e1607a616f9` | `build/game/df_game_r` |

## 4. 脚注

- **bridge MISSING 25**：全部是 AES/DES/3DES/SHA256/384/512 及 TEA 变换（`AES_Encrypt`/`DES_Decrypt`/`SHA256_Transform`/`encrypt`/`decrypt` 等）。按第三方加密豁免规则本可不计；`gen_report_manifest` 未把这组 mangled 名收进 `is_exempt_symbol`，故出现在 MISSING。不计入 `non_identical`。
- **auction = point**：同源 Auction 代码，计数逐项相同。各 87 NEAR + 1 DIFF。
- **stun NEAR 3**：`getMessage` / `sendMessage` / `openPort`。产物 743808 vs ORIG 131126，是 64 位静态 libstdc++/libgcc 链接体积（GLIBC ≤2.4），不是函数丢失。
- **monitor**：本次重建前 CMakeCache 指向已删除的 `dnf_toolchain_monitor.cmake`，已改用 `dnf_toolchain_44.cmake` + `c6446r` 重配后编过。
- ORIG 体积普遍更大：主因 DWARF 调试段 + per-TU `static const` 副本，不是函数覆盖缺口。

## 5. dbmw / guild / monitor 的 game 口径（`compare_*_full.py`）

上表 13 服务用的是 `gen_report_manifest.py`。dbmw / guild / monitor 现已有与
`compare_game_full.py` **同一套**链接口径（落点重索引 + AE + stl 豁免），
报账以这个为准（见 `source/toolchain/svc_verify_guide.md` 与 `PROGRESS_STATUS.md` §11）：

| 服务 | 共同 | identical | near | diff | stl | 非 identical | 仅ORIG | 仅NEW |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| dbmw | 1611 | **1231** | 19 | 219 | 142 | 238 | 23 | 170 |
| guild | 2111 | **1732** | 18 | 219 | 142 | 237 | 34 | 69 |
| monitor | 2287 | **1901** | 14 | 230 | 142 | 244 | 51 | 87 |

```
python3 source/toolchain/compare_svc_full.py all
```

## 6. 复现

```bash
# 13 服务（不含 game）
python3 source/toolchain/gen_report_manifest.py \
  --services auction bridge channel community coserver dbmw \
             guild manager monitor point relay statics stun

# game（权威口径）
python3 source/toolchain/compare_game_full.py
```
