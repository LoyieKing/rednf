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
| dbmw | 1611 | **1351** | 5 | 68 | 187 | **73** | 23 | 127 |
| guild | 2111 | **1837** | 7 | 81 | 186 | **88** | 34 | 69 |
| monitor | 2289 | **2010** | 6 | 88 | 185 | **94** | 49 | 83 |
| statics | 1300 | **1101** | 1 | 13 | 185 | **14** | 40 | 10 |

**2026-09-15 本轮累计（截至最新核账）**：起始 94/126/131/25 → **73/88/94/14**，合计翻正 **107 件、0 回归**（每轮 `delta_report` 逐名比对 ADDED=0；其余 9 服务维持 0：auction/point/bridge/stun/channel/community/coserver/manager/relay）。

**本轮新增轴（均已闭合）**：
| 轴 | 内容 | 状态 |
|---|---|---|
| **RTTI/vtable 数据符号轴** | ORIG `_ZTI` 为 `__vmi_class_type_info`（24B）而我方 `__si_class_type_info`（12B）⇒ 基类可见性错（应为 `private`/`protected`）；四服务五处（`CKillUSRConfig`×4 + guild `CPowerWarConfig` + monitor `CMemberConfig`/`CMemberExpTbl`）已全部改为 `: private Base` 并 relink 验证 `_ZTI`=0x18 | **四服务 0 差异** ✓（`nm -S` 逐名比 `_ZTI*/_ZTV*/_ZTS*`；dbmw 76 个 only-OURS 的 yaSSL 符号按库豁免） |
| **per-TU 编译旗标轴** | `Guild/DNFUser.cpp` 的 `-fno-exceptions` 与 ORIG 不符 ⇒ 整片缺 EH landing pad；移除后 TU 214→**222**（`MemPool<CUser>::~MemPool` 等 8 件翻正） | 已修正 ✓ |
| **地址轴口径现象** | 链接口径的 NEAR 有两类：①真实操作数差异 ②**纯地址位移**（其它 TU 改动引起的段布局变化）；判② = 地址归一化后重跑 | 判据入册 ✓ |

### 5.1 2026-09-15 本轮进展（链接口径实测，`compare_svc_full.py`）

起始 → 当前：**dbmw 94→82、guild 126→107、monitor 131→111、statics 25→14**（合计翻正 **61 件，0 回归**）。
另：auction/point/channel/community/coserver/manager/relay 维持 **0**；bridge(1)/stun(3) 在 **AE 口径下经逐指令核对为 0**（其 NEAR 系 `gen_report_manifest` 粗口径不作数据地址伪代码化所致，非真残差）。

**本轮新增/确立的范式**（详见 `asm_translation_patterns.md` P372–P374 与广播记录）：

| 范式 | 内容 | 实证 |
|---|---|---|
| **P372** | C 风格**局部前置声明**决定 -O0 栈槽分配序（含 ORIG 初值 0 的具名量与赋值序） | statics 3/3、guild/dbmw/monitor 多处 |
| **P373** | 日志家族**单表达式化**（`CMyFileLog` 两语句 → `DNF_LOG_SCOPE_AT`）+ 删日志用具名中间量 | guild9 6/6、dbmw8 38 处、guild10 多处 |
| **P374** | 容器 value / 栈对象 **ctor 补 `throw()`** 消多余 landing-pad（六步判据 + 多调用点边界） | statics 一改翻正 5 件 |
| **P58** | `void*` 返回 → 具体类型（调用点装载序归位） | dbmw9/dbmw8/guild8 命中，含 `CApplication::Load` |
| **P301** | 派生指针成员 → 基类指针 upcast（`mov %ebx,%eax; mov %eax,%edx` 中转复现） | guild11/guild8/dbmw9 五处 |
| **P292/P211/P265** | `char/int → bool`、`int → unsigned` 类型轴（消费端 `xor $1,%eax; test; je` 判据） | monitor9 单轮多件、guild9、dbmw9 |
| 其它 | P319 整型加法+回 cast、P307 `register` 修槽、`do-while → while` 循环头 jmp、`for`-init 声明改槽序、函数尾裸 `return;` 复现 4.1 尾岛 | 各泳道回报 |

**已排除的轴（勿再试）**：①GCC 4.1.2（`c5-g++`）编出的 TU 更差（ndiff 29→202 等）；②444 后端不优于 446（同件 ndiff 10 vs 10，另两件更差）；③业务函数 prologue 两侧一致（`sub $0x10,%esp`），`lea -imm(%esp),%esp` 仅存在于**库符号**（已 stl 豁免）。

**剩余件性质（§5.1 口径）**：statics 剩余 14 件经逐件穷举试形后**全部判定为 4.4.x 小版本 codegen 轴**（EH landing-pad 块序 N104、lea↔add、scaled-index 因式分解、载入寄存器 vs mem 操作数、N121 尾岛），已附 ORIG 地址证据入册；guild/dbmw/monitor 同类登记合计 40+ 类。


### 5.2 方法学与关键结论（本轮补充）

**口径纪律（由实证确立）**：
1. **TU 口径（对 `.o`）对 STL 弱符号模板实例存在系统性假差异** —— 实证：dbmw 某批 20 件（`new_allocator<T>::allocate/construct`、`vector<T>::size/push_back`、`__copy_move*`、`Vector_base::~`、`std::pair/Rb_tree` 实例）TU 口径全数「翻正」，但**全量重编（96 个 `.o` 重建）+ relink 后链接口径 0 变化**；链接后这些符号本就一致（残差列表剩 1 条）。
2. ⇒ **报账只认「业务函数」的链接口径收益**；STL 弱实例类件不再投入。
3. **业务函数的 TU 口径改善 1:1 转化**（statics 22→14、guild 多轮、monitor 多轮均逐值吻合）。

**编译器/工具链轴的四向排除（勿再试）**：
| 轴 | 实证 |
|---|---|
| GCC 4.1.2（`c5-g++`，CentOS5） | statics FrameLagCollector 5 件 ndiff 29→**202**、44→66、46→71、324→382、193→266 ⇒ 更差 |
| 4.4.4 后端（`cc1plus444bin`） | 同件 ndiff 10 vs 446 的 10（平），另两件更差 ⇒ 无收益 |
| 4.4.7 后端 / `-march=i486 -mtune=generic` | 前会话矩阵已测：零翻正、唯一动作是劣化 |
| 库符号 `lea -imm(%esp),%esp` vs `sub $imm,%esp` | 仅存在于链接的 `libstdc++.a`（我方 4.4.7-23 / ORIG 4.4.7-1），已按 stl 豁免；业务函数两侧 prologue 一致（`sub`） |

**已验证并落档的新范式**：P372（局部前置声明→槽序）、P373（日志单表达式化）、**P374（ctor 补 `throw()` 消多余 LP，六步判据 + 多调用点边界）**；另有**位域具名视图**（`struct{uchar pad[K]; uchar lo:5,hi:3;}` 从结构首址访问，复现 4.1 的 `and $0x1f`/`shr $0x5,%al` lowering）、**P46 叶块序**（switch 叶块排放=case 书写序）、**路由错配轴**（`DNFPacketTracer.cpp` 曾误配 4.1.2/C5，ORIG 实为 4.4 编译；已修正）、**四条微形**（取反+空 then / 取反+块交换 / `goto L; … L:;` join jmp（仅非循环 if/else）/ `register` 复现 callee-saved 承载）。

**流程加固**：本会话发生一次 `git checkout --` 误操作（§5.3 明令禁止）导致 `DBMW/DBManager.cpp` 未提交改动丢失；处置 = 全队广播禁令（checkout/restore/stash/reset/clean 一律禁）+ 各泳道在 `/tmp/<lane>_backup/` 留恢复点（该机制随后在一次 `apply_patch` 误删中成功无损恢复）+ 事故文件经幂等脚本恢复至事故前四数（140/393/10/41/125）。

> **2026-09-14 口径补充（`compare_svc_full.py`）**：stl 豁免模式补 `_ZNKSb`
> （const `basic_string<wchar_t>` 弱实例，每服务 37 个），并新增 libgcc/libstdc++
> 运行时豁免（`_GLOBAL__I_*` 初始化桩、`__static_initialization_and_destruction_0`、
> `get_adjusted_ptr`、`check_exception_spec`、`_Znaj`）。
> 依据：这些符号在 ORIG/我方均为 weak 库符号，实现来自各自链接的 `libstdc++.a`
> （我方 c6root 4.4.7-23 / ORIG 4.4.7-1），逐函数比对只差库构建形态
> （`sub $imm,%esp` vs `lea -imm(%esp),%esp`），非本服务源码还原目标；
> 与 game 口径「STL 弱实例 + libgcc/libc 运行时不计入」一致。
> 本次调整只把上述库产物从 near/diff 移入 stl，未改动任何服务源码，
> 也未影响 game 的 `compare_game_full.py` 口径。
> 同日 Main 重链：guild（2 TU 待编）数字不变；monitor 旧产物落后源码
> （47 TU 待编），重链后 非identical 201 → **168**（identical 1901 → 1936），
> 上表 monitor 行已按新产物更新。

```
python3 source/toolchain/compare_svc_full.py all
```

### 5.3 2026-09-15 工具链轴深挖（4.4.7-1 溯源）

**ORIG 编译器构成（已从文件验证）**：四目标服务（dbmw/guild/monitor/statics）
的 `.comment` 均含**同一组四个版本**：

| 版本 | 来源（CentOS 对应） |
|---|---|
| `4.1.2 20080704 (Red Hat 4.1.2-52)` | CentOS 5.6/5.7 世代 |
| `4.4.4 20100726 (Red Hat 4.4.4-13)` | CentOS 6.0 |
| `4.4.6 20110731 (Red Hat 4.4.6-3)` | CentOS 6.2 |
| `4.4.7 20120313 (Red Hat 4.4.7-1)` | **RHEL/CentOS 6.2z–6.3z z-stream** |

**CentOS 镜像版本地图（实测，`mirrors.aliyun.com/centos-vault` 与清华镜像一致）**：

| 目录 | gcc-c++ |
|---|---|
| `6.0/os` | 4.4.4-13 |
| `6.1/os` | 4.4.2-3, 4.4.5-6 |
| `6.2/os` | 4.4.6-3, 4.4.6-4 |
| `6.3/os` | 4.4.6-4 |
| `6.3/cr` | **4.4.7-3** |
| `6.4/os` | 4.4.6-4, 4.4.7-3 |
| `6.4/cr` | 4.4.7-4 |
| `6.5/os` | 4.4.6-4, 4.4.7-4 |
| `6.5/cr` | 4.4.7-11 |

**结论：`4.4.7-1` 与 `-2` 在 CentOS 渠道不可得**。
- `vault.centos.org` 对本网络 403（含 UA/Referer 头仍 403）；tuna/ustc/nju/bfsu/huaweicloud
  的 `centos-vault` 快照中 6.3/os 均只有 4.4.6-4、6.3/cr 已是 4.4.7-3；
- `6.x/updates/` 在 vault 中已清空（无 gcc 包）；
- ⇒ 首版 4.4.7（`-1`/`-2`）已被后续 z-stream 覆盖，镜像快照只保留最终态。

**已做过的替代后端（`toolchains/`）**：`c5-g++`（4.1.2-55）、`c6-g++-444r`（4.4.4-13）、
`c6-g++-446r`（4.4.6-3）、`c6-g++-447r`（4.4.7-3，本次新增，`cc1plus447bin/`）。

**4.4.7-3 实测等同 4.4.6-3**（同一源码、全量重编、链接口径核账）：

| 服务 | 用 4.4.6-3 | 用 4.4.7-3 | 结论 |
|---|---|---|---|
| statics | 非identical **14** | **14** | 无变化 |
| monitor | 非identical **94** | **94** | 无变化 |

⇒ 补丁级 codegen 差异位于 **`-1` → `-3`** 之间（`-3` 至 `-23` 无 codegen 变化）。
**唯一剩余路径 = upstream GCC 4.4.7 源码构建**（`.comment` 的 `20120313` 即 upstream
4.4.7 发布日，`-1` 为 RHEL 首个 4.4.7 打包）。

**构建环境（已落地）**：`docker` + `centos:6` 容器 `gcc447b`，yum 源指向
`mirrors.aliyun.com/centos-vault/6.10`（容器内已验证可装 gcc 4.4.7-23）。
踩坑记录：
1. `docker run --rm` + 安装 ⇒ 包丢失 ⇒ 必须用**持久容器**；
2. `docker exec ... nohup make &` 在 exec 会话结束时被杀 ⇒ 必须用 **`docker exec -d`**；
3. CentOS 6 自带 **binutils 2.20 的 BFD bug**（`reloc.c line 443 in bfd_get_reloc_size`）
   在链接 `cc1-dummy`/`cc1plus-dummy`（含 `libbackend.a`）时必崩 ⇒ 已在容器内构建
   **binutils 2.22**（`/opt/binutils`）并以 `--with-ld/--with-as` 重建 GCC。

**单函数案例（`CGuild::ReplyGuildAllMembers`，本轮首次达到「残差全为栈偏移」）**：
该件 445 → **390** diff（equal 460 → 517），改动三处均为**纯源码形态**：
1. 删去源码中多余的 `unsigned short total = m_totalCnt;` 局部（ORIG 反汇编证实无此槽；
   `pkt.m_totalCnt=...` 之后紧接 `count=0`）；
2. `int idx = 0;` → `int idx;`（ORIG 只有 `for` 初始化那一次 `movl $0x0`）；
3. 内层循环的第二个 `CUser* m` 合并为函数级单一声明（ORIG 槽集少一个 4 字节槽）。
**残差性质判定**：`--all` 抽样显示剩余 390 条**全部是「同一指令 + 不同栈偏移常量」**
（`lea -0x2fcb(%ebp)` vs `lea -0x2fcf(%ebp)`、`-0x20` vs `-0x2c` 等），指令序列与操作数
形态完全一致 ⇒ 属**编译器栈槽分配轴**，非源码语义缺口。声明顺序穷举实验
（6 种组合）证实 `count → m → idx` 已是最优（其余组合 404/509/522/523）。

**upstream GCC 4.4.7 实测：显著劣化，路径封闭（2026-09-15）**

宿主机（`/tmp/gcc-4.4.7/build/gcc/cc1plus`）已成功产出 upstream 4.4.7 的 `cc1plus`
（`--version` = `GNU C++ (GCC) version 4.4.7 (x86_64-unknown-linux-gnu)`，
无 Red Hat 补丁标记；构建在 `all-target-libgcc` 阶段失败，但 `cc1plus` 已生成，
而本工程只需 `cc1plus` + `c6root` 的头与运行库）。已装入
`toolchains/cc1plus447ub/`，包装脚本 `source/toolchain/cmake/dnf_c6447ub_gxx.sh`
（**当前未接入任何服务路由**）。

**statics 全量重编 + 链接口径核账结果（同源码、只换 cc1plus）**：

| 后端 | 共同 | identical | near | diff | stl | 非identical |
|---|---|---|---|---|---|---|
| `4.4.6-3`（当前基线） | 1300 | **1101** | 1 | 13 | 185 | **14** |
| upstream `4.4.7` | 1215 | **658** | 70 | 302 | 185 | **372** |

⇒ upstream 4.4.7 **远劣于** RHEL 4.4.7-23/4.4.7-3/4.4.6-3。原因：RHEL 的 4.4.7 系列
带有大量**影响 codegen 的 backport 补丁**，未经补丁的 upstream 4.4.7 反而离 ORIG
（`Red Hat 4.4.7-1`，即 RHEL 首个 4.4.7 打包）最远。**该路径已封闭，勿再尝试。**

**工具链轴总结论（六向后端全部实测完毕）**：

| 后端 | 实测 |
|---|---|
| 4.1.2-55 | 劣化（statics 件 ndiff 29→202 等） |
| 4.4.4-13 | 持平或劣化 |
| **4.4.6-3** | **当前最优（基线）** |
| 4.4.7-3（6.4/os） | 与 4.4.6-3 完全一致 |
| 4.4.7-23（6.10） | 略劣（`lea -imm(%esp),%esp` 形态） |
| upstream 4.4.7 | 严重劣化（statics 14→372） |
| **RHEL 4.4.7-1** | **不可得**（CentOS 渠道 -1/-2 已被 z-stream 覆盖；vault 403） |

⇒ 剩余非 identical 件的残差**在现有可得工具链下不可消除**；
「每服务 ≤9」目标受此约束。

### 5.4 2026-09-15 第二轮：残差性质判别（结论修正为「源码级」）

**新增后端（本地已落地）**

| 后端 | 来源 | 用途 |
|---|---|---|
| `cc1plus412bin` + `dnf_c412_gxx.sh` | CentOS 5.8 `gcc-c++-4.1.2-52.el5`（**ORIG .comment 中的精确版本**） | 实测无收益，未接入路由 |
| `cc1plus447ub`（已删） | upstream GCC 4.4.7 源码构建 | 实测严重劣化，路径封闭 |
| 重建 4.4.7（已删） | 官方 4.4.7 源码 + RHEL 6.4 SRPM 补丁集（剔除 4.4.7-2/-3 changelog 所列新增补丁） | 用于下述判别实验 |

**判别实验（决定性）**：重建 4.4.7 后端与 4.4.6-3 差异极大
（statics identical 1101 → **658**、非identical 14 → **372**，证明它是不同的代码生成器），
但**同一批残差函数的 ndiff 完全不变**（`FrameLagDataStruct::init` 44、`AddSecretShopStatistic` 12、
`SendDBPartyCharacStatistic` 18、`SendDBMWHWSpec` 7、`PushOneFrameLagData` 10，与 4.4.6-3 逐值相同）。

⇒ **若残差源自编译器差异，换编译器必然改变这些函数**；实测不变 ⇒
**残差是源码级差异**（此前「编译器轴」的判定需要修正）。旁证：
4.1.2-52（ORIG .comment 版本）与原生 i686 cc1plus（经 `c6root/lib/ld-linux.so.2` + 32 位
mpfr/gmp 运行）在同一批函数上同样只劣不变。

**CMOV 轴（新发现，但不构成杠杆）**：ORIG 的 statics 业务代码 **cmov = 0**，我方 ELF 的 216 条
cmov 全部来自 i686 预编译库（TU 级 `.o` 为 0）。对 statics 全量加 `-march=i486/i586 [-mtune=generic]`
后：CMOV 归零、identical 集不变、**残差集（含函数名）完全不变** ⇒ 不是判别轴。

**ORIG 夹取形态 = GCC 的 MIN/MAX 展开**：`CIPCounter::setMinIPCount`（13 条）的 ORIG 形态为
「`movzbl` 载入 → 寄存器内 `cmp` → `jbe` 跳过 → 条件常量装载 → 单次存储」（**无 join jmp**），
即 GCC 对 `MIN_EXPR`/`MAX_EXPR` 的展开；我方默认（i686 + 混合类型三元）走通用三元展开（多一条 `jmp`）。
穷举 276 种源码形态 + 同类型操作数 MIN 形态 + `-march`/`-mtune` 组合后，最好为 **ndiff=3（13/14 条）**，
仍差 1 条指令，无法归零。

**本轮源码级收窄（不改计数，仅供后续）**

| 函数 | 前 → 后 | 手段 |
|---|---|---|
| `CGuild::ReplyGuildAllMembers` | diff 445 → **390** | 删多余 `unsigned short total`；`int idx = 0;`→`int idx;`；内层第二个 `CUser* m` 合并为函数级单一声明 |
| `CPacketTranslater::OnChangeCharName` | diff 86 → 57 → **40** | 三指针声明顺序穷举（6 排列）：`today→guild→pb` 最优；删 `deadBuf` 更差（77） |
| `CIPCounter::setMinIPCount` | diff 6 → **4** | 夹取改为三元式（贴近 ORIG 的 MIN 展开） |

**当前口径（本轮结束实测，零回归）**：statics **14** · monitor **94** · guild **88** · dbmw **73**。

### 5.5 2026-09-15 第三轮：六泳道并行收敛（新增翻正 8 件）

**复核后的口径（Main 实测，零回归）**：statics **14** · monitor **92** · guild **85** · dbmw **70**
（基线 269 → 261，**净翻正 8 件**）。中途曾出现 statics 14→16 的泳道在途回归
（`PushMonitoringSpecData` 两件变 NEAR），由属主泳道修复（`Pair m_i[41]` → `m_i[6][7]` 二维化 +
删无据 padding，三处索引改写为 `m_i[k][j]`），修复后这两件 ndiff=0，且
`FrameLagDataStruct::init` 101→104 条 / ndiff 44→**2**、`SaveFrameLagData` ndiff 193→**114**、
`accFrameLagStruct` ndiff 324→**312**（大幅收窄，未翻正）。

**新增翻正（可归因）**

| 服务 | 函数 | 手法 |
|---|---|---|
| guild | `CPacketTranslater::OnWriteGuildMemberMemo`（184/184/2 → **0**） | **真实结构体错误**：`Packet_DB_Write_Guild_Member_Memo` 成员序与 ORIG 相反（ORIG 为 `m_guildKey(+0xa)`/`m_charNo(+0xe)`），交换定义即归零 |
| monitor | `CPacketTranslater::OnUpdateMiniCraneSeed` 等 2 件 | 泳道 MonitorPktTrans |
| monitor | `COnTimeEventManager::OnRewardStart` 长度 96/107 → 96/96 | **P374 新清理判据**：ORIG 无 `operator delete`/`_Unwind_Resume` ⇒ 被构造类型 `COnTimeEventRewardEndTrigger` 的 ctor 加 `throw()`（唯一调用点，安全） |
| dbmw | `CDBManager` 3 件 | 泳道 DbmwDBManager |

**本轮新增可复用范式（已由泳道实测）**

1. **零填内联**：`char buf[N]; memset(buf,0,N);`/`__builtin_memset` ⇒ 改写为 `char buf[N] = {0};`
   （ORIG 有 `rep stos`、无 memset 调用时）。实测 `CMemoryCashManager::QueryCashMemoryBlackList` 36→5、
   `OnChangeGuildName` 225→39（帧 0x190→0x19c 与 ORIG 完全一致）。
2. **P374 新清理判据**：逐件比较 ORIG 与我方调用集合是否含 `operator delete` + `_Unwind_Resume`（`new T(...)` 的 EH 清理）。
   ORIG 缺 ⇒ 给**被构造类型的 ctor** 加 `throw()`（声明+定义，mangled 不变）。
   **陷阱**：同类型其它调用点会同时失去清理 ⇒ 若某处 ORIG 有清理则净收益为 0；例如
   `Arad_MomijiEvent.cpp` 中 `EndEffectTask::_DoExecute`（ORIG 无清理）与 `EventManager::StartEvent`（ORIG 有清理）
   自相矛盾，单一 ctor spec 无法同时满足，登记待解。
   全库扫描（`operator delete`/`_Unwind_Resume` 差集）：monitor 3 件、dbmw 3 件、statics/guild 0 件。
3. **声明顺序规则（GuildPktTrans 实证，本 TU 通用）**：
   - ebp 负偏移按「先声明 = 更深」分配；要让变量更浅必须声明得更晚。大对象（≥~0x50 的 Packet 实例）被放在帧底，不遵守。
   - `X* pb = (X*)pkt;` 应放在 `try {` 内、null-check **之后**（放函数顶部会多出 `mov 0x8(%ebp),%eax; mov %eax,-0xNN(%ebp)`）。
   - **帧比 ORIG 大 16 ⇒ 我方多 1 个 4 字节局部**（槽集整体平移）；最常见元凶是「为 log 缓存函数返回值的具名临时」。
   - 零填形态同上第 1 条。
4. **删中间临时局部**（DbmwDBManager 实证，dbmw 翻正 1 件）：`int idx = pkt->m_kind; if (idx >= f())`
   ⇒ 就地内联为 `if (pkt->m_kind >= f())`（ORIG 用寄存器保存左值而非栈槽）。
   同族：`unsigned int accId = ...; call(accId)` ⇒ `call(*(unsigned int*)&joinInfo.m_id)`
   （ORIG 形 `mov 0x1c(%ebp),%eax; add $0x8,%eax`）；指针算术 ⇒ 数组下标
   （`(char*)result + i*0xc + off` ⇒ `&result->m_items[i].m_field`，GCC 先算索引再 add base）。
   实测收益：`OnSavePacketOverflowWrite` ndiff 54→**0**、`OnGuildJoinByListApprove` 12→4、
   `loadLimitNpcBuyItemInfo` 14→6、`OnLoadGuildBoard` 6→3、`OnWriteGuildBoard` 30→5。

**已确认不可控类（勿重复投入）**

- 尾部 **jump-padding**（ORIG 在 `call __cxa_end_catch` 后多 `jmp .+2; nop` 使 epilogue 4 字节对齐）⇒ 非源码可控。
- 单一/少数**槽号互换**（坐标下降已达局部最优）、`lea` vs `add+mov` 的 RA 选择、`movzbl` 双重零扩展
  （4.1.2‑52/4.4.4‑13/4.4.6‑3/4.4.7‑3/4.4.7‑23/重建 4.4.7 六后端一致复现，ORIG 无）。

### 5.6 2026-09-15 目标范围与口径审计（Main）

**「所有二进制」的完整清单**（`neople/*/`，每个目录携带同一套 ELF；`game` 目录无 `df_*`，
`secsvr` 同）：auction · bridge · channel · community · coserver · **dbmw（×3 目录同一文件，md5 一致）** ·
guild · manager · monitor · point · relay · statics · stun。`df_game_r` 按目标排除。

**两套并存口径的实测值**：

| 二进制 | `compare_svc_full.py`（严格口径） | `function_reports/manifest_summary.tsv`（旧口径） |
|---|---|---|
| statics | 非identical **13** | 28 DIFF + 1 NEAR |
| monitor | **83** | 143 DIFF + 13 NEAR |
| guild | **83** | 153 DIFF + 17 NEAR |
| dbmw | **70** | 157 DIFF + 18 NEAR |
| **auction / point** | **21 / 21**（同集，均为 NEAR 为主） | **0 / 0** |
| bridge / stun | 目录表未收录 | 1 / 3 |
| channel / community / coserver / manager / relay | 目录表未收录 | 0 |

**结论**：`auction`/`point` 在旧口径为 0、在严格口径为 21 ⇒ 本目标按**严格口径**推进
（满足严格口径即同时满足旧口径）。两服务的 21 件**集合完全相同**（Main 已逐名 diff），且只分布在
`ServerLab/Auction/Strings.cpp` 与 `Search.cpp` 两个 TU ⇒ 修一套同时解决两个二进制（**42 个计数**），
已派泳道 `AuctionPoint`。

**剩余未达标范围（严格口径）**：statics 13 · monitor 83 · guild 83 · dbmw 70 · auction 21 · point 21。

### 5.7 2026-09-15 第四轮：范式补充（泳道实测，逐条可复用）

1. **R1-逆向**：ORIG 有时**保留**局部副本但**声明更晚**（在使用点邻近的分支内）。征兆：ORIG 首处
   直接用参数 `0x8(%ebp)` 而无局部装载，中段才出现 `mov %eax,-0xNN(%ebp)`。
   修法：把该局部**下移**到分支内（与常规 R1「删局部」方向相反）。实测
   `onItemLimitEditionBuyableRequest` 70→48。
2. **R2 反例**：同一函数内不同日志语句的 ORIG 形式可能不一致——`OnLogout` 的 log2 必须是宏临时
   （346→309），log3/log4 必须是命名对象（换宏 309→345 恶化）⇒ **逐条试，禁止批量套用**。
3. **R1 反例**：`OnLogout` 删死局部 `buddyReg` 反而 309→593 ⇒ 先确认 ORIG 是否保留。
4. **`lea`/`add` 族唯一已知例外**（打破全队「死胡同」结论）：memcpy/memmove 类调用**实参的 cast
   写法**会影响 gimplify 后的实参求值顺序。把 `(const void*)src` 改成 `(const char*)src`（dst 同步
   `(char*)`）即可让 src 先求值、寄存器分配连锁对齐。实测 `CHWSpecResearcher::SendDBMWHWSpec` 7→0
   （statics 14→13）。同族无效：去 cast、`(void*)dst`、`sizeof(...)`、结构体赋值（12 字节被内联成
   3 条 mov，更差）。
5. **槽差工具** `/tmp/slots.sh <elf> <mangled>`：出「栈槽集合 + 帧大小」差，用于把残差分为
   **真实局部差异**（可修）与**纯指令形态**（多为死胡同）。dbmw 21 件中 15 件 slotdiff=0/framediff=0
   ⇒ 直接登记死胡同，不必再试源码形态。

### 5.8 2026-09-15 第五/六轮：多维**声明**手法与收敛口径（21 泳道并行）

**当前口径**（严格口径，Main 实测）：statics **11** · monitor **72** · guild **76** · dbmw **70** ·
auction **21** · point **21**（本轮起点 311 ⇒ 累计翻正 **40 件**）。

**本轮决定性手法：多维索引必须在「成员声明（.h）」上真做**

| 结构 | 有效做法 | 实证 |
|---|---|---|
| `t[i+C]`（C=元素偏移） | 声明 `[N][1][1]` + 访问 `[i][0][C]` | `m_table[i+4]` ⇒ `CPacketDecoder::CPacketDecoder` 3→**0**、`MsgDecode` 6→**0** |
| `t[i*K+j+C]`（C≠0） | 声明 `[A][K][1]` + 访问 `[i][j][C]` | `m_frameLag[i*7+j+4]` ⇒ `OnFrameLagStatisticsAdd` 12→**0**、`is_valid_statistic_packet` 43→37 |
| `t[a*K+b]`（C=0） | 声明 `[A][K]` + 访问 `[a][b]` | `guild_att_exp[level+0x11*p]` ⇒ `GetAttendanceExp` 1→**0** |
| 元素**内部**数组下标（常量来自内层步长，如 `m_items[i].m_data[k]` 的 4k） | **无效**（两泳道各跑 7 个真声明变体，产物逐字节相同） | `AddSecretShopStatistic` 仍 12 |
| `.cpp` 内 cast 视图（2D/3D） | **无效**（C1Ev 3→4 更差） | 必须改 `.h` 声明 |

**其它本轮新判据**（各有多件实证）

- **被调函数返回类型 `bool` vs `char`/`int`**：`cmp $0x1,%al; setne` ↔ `xor $0x1; test; je`；返回类型不入
  mangled 但影响所有调用点，改后须全量重编 + 服务级核账。
- **成员同址 memcpy 源**：`memcpy(dst, this, n)` ⇒ 应写 `memcpy(dst, &m_info, n)`（3 件归零）。
- **链式赋值**：`maxIt = it2 = begin();` 复现 ORIG 的经引用取形（`SameRankWork` 2→0）。
- **`goto` + 相邻 label**：切分基本块以对齐 EH landing pad 插入点（`CNetworkThread::dispatch` 6→0；
  试过 20 种条件/块结构形态均恒 6）。
- **EH 族真因**：我方多 `operator delete`/`_Unwind_Resume` 常源于**多余具名临时对象**（`queryTowerFullRank`
  两侧都调外联 `push_back`，我方多一个 `stTowerRank_t` 临时 + 析构），**不是** ctor spec。
- **R1 命中**：删重建期冗余局部（`SaveFrameLagData` 删 `time_t now = t; (void)now;` 114→44）。

**已确证收敛的共性根因**（StaticsFrameLag）：残余主因是 ORIG 的 expand 顺序为「index 先、base 后」，
我方相反；**若找到让指针形参被 IRA 判为 spill 的源码杠杆**，可一次收益 −11 条以上（本 TU 3 件同源）。
已试无效：整数 cast 相加、成员数组视图、元素指针下标、41/42 元素×多种 cast、unsigned 索引。

**过程事故**：GuildCore 泳道两次把 `DNFGuild.cpp` 写截断（python 脚本写入），均在 1 分钟内用备份
恢复并自证；其运行最终以 `yield(null)` 失败退出，但 **edit 已安全落盘**，Main 复核树可编译、计数正确。
教训：批量替换脚本必须先写临时文件再原子替换，且每步 `diff` 自证。

### 5.9 2026-09-15 第七轮：auction/point 达标 + 死胡同分类学

**里程碑**：auction **8** · point **8**（均 ≤9 达标）· statics **11** · monitor **71** · guild **75** ·
dbmw **70**。本轮累计翻正 **68 件**（311 → 243）。

**新增结构族范式（GuildCore，guild 88→75）**：P-A 正向 `if` 包裹去多余 `jmp`+`nop` · P-B 空 then → return ·
P-C 多 early-return 合并为嵌套守卫（3 次归零）· P-D `bool` 物化 + 分支互换 · P-E 重复计数器**共槽复用**
（`ReplyGuildMembers` 265→41）· P-F 删重建期具名局部 · P-G 指针算术 `(a+i)`（**不可无差别套用**）·
P-H 具名值局部 → 具名**指针**局部（`SecedeProxyMember` 58→36）· P-I 复合 `while` 条件 → `if/break` ·
P-J R6 字段 signedness。

**U1/U2（UserLane，monitor −18）**：U1 末尾显式 `return;` 复现 if/else join 残留（**位置敏感**：须在
最内层 if/else 之外、外层块之内；放最外层 +2 变劣）· U2 `return c?A:B` → `if (c) return A; return B;`。

**判据边界（负面，均已实测回退）**

| 判据 | 适用边界 | 反例证据 |
|---|---|---|
| S2 比较操作数次序 | **仅**当 ORIG 真把 `end()` 放左侧时有效 | 反向套用一律劣化：statics 4 点（12→16 / 25→29 / 18→22 / 20→24）、FrameLag 2 点（10→14、44→48）、monitor AddSchoolNo 10→17 |
| S1 作用域深度 | 仅当**变量槽集合本身有差**时有效 | 槽集合已逐一相同的件上，改作用域生成 .o **逐字节不变**（ndiff 恒 175） |
| 多维声明 | 仅「指向元素的整数偏移」族 | 「元素内部下标」族 7 变体逐字节相同 |

**死胡同分类学（本轮定案，共 8 族，勿再投入）**

1. **STL 类类型局部 + BIND/EH 展开顺序族**：指令序列逐条相同，仅 iterator / `std::map` 临时等**类类型
   局部**的槽号分配顺序相反（`DeleteUsersOnGameServerDown` 34、`AddSchoolNo` 10、`CTcpNetSystem::SendPacket` 6、
   `ReplyGuildAllMembers` 209 的同模式 ~20 处）。标量版 S1/S2 碰不到该机制。
2. `lea`/`add` 地址计算族。
3. 中转 `mov` 残留族（R6 型）。
4. 槽使用次数分布差异族。
5. `jne`/`je` 分支布局 + 寄存器选择族。
6. `push`/寄存器分配族。
7. 大对象栈复用族。
8. 尾部 jump-padding 族。

**口径纪律（本轮最大教训）**：所有收益必须以**重链接后的 ELF** 核账——auction/point 的 21→16 差额全部来自
「`.o` 已改、ELF 未重链」的假读数；`ninja -n` 干跑可安全确认是否需要链接。

### 5.10 2026-09-15 第八轮：三个二进制达标 + C1/C2/O1/O2 判据集

**达标进度（严格口径，每轮重链接后用 `compare_svc_full.py` 实测）**

| 服务 | identical | 非identical | 状态 |
|---|---|---|---|
| **auction** | 1209 | **1** | ✓ ≤9 |
| **point** | 1209 | **1** | ✓ ≤9 |
| **statics** | 1106 | **9** | ✓ ≤9 |
| monitor | 2035 | 69 | 进行中 |
| guild | 1851 | 74 | 进行中 |
| dbmw | 1360 | 64 | 进行中 |

累计翻正 **93 件**（311 → 218）。

**C1 cast 层决定 `ARRAY_REF` 的 base/index 展开次序**（受控实验，4 变体逐条复现）

- `pkt->m_arr[i].f`（参数类型正确、**零 cast**）⇒ index 先、base 后 ← ORIG 形态
- `((View*)pkt)->m_arr[i].f`（**有 cast**）/ `char*` 参数 + cast / 两段式 cast ⇒ base 先、index 后
- 修法优先级：①把视图**下沉进权威类型声明**（零 cast）→②正确类型的**局部指针**（+4 栈槽）→③登记死胡同
- 实证：`CGuildWar::GetGuildWarInfo` 12→**0**、`ReplyGuildAllMembers` 209→**168**、
  `is_valid_statistic_packet` 43→**17**、`AddLoadingTimeReportStatistics` 31→**0**、
  `DNFApplication::getCollectItems()` 去 `void*` 后 monitor **+2 件**
- **修法②的失效条件**：若 ORIG 侧本身就是「每次重新调用 + 就地 cast」，改用局部指针会劣化（monitor 5→84）

**O1 先看权威类型是否本就有字段**（决定改造成本的必做前置）

- `Statistics.h` 只有前置声明 + 本地 wire 视图，而权威头 `Packet_Loading_Time_Report_Statistics.h` /
  `Packet_Stat_Lag_Statistics.h` **本就有字段** ⇒ 只需在 .cpp 补 `#include`，**零头改动**（31→9→0）
- 反例：`shared/packet/include/Packet_Frame_Lag_Statistic_Add.h` 是**无字段的极简版**（有字段那份在 `Game/`），
  且 `DBMW/DNFPacket.h` 里 packet 类**全是空类** ⇒ **C1 在该处不可行**，登记死胡同，不要改头

**C2 元素内部数组「标量化」**（补上多维声明管不到的那一族）

- 症状：ORIG 统一 `sub $CONST` + **各字段各自位移**（0x11/0x15/0x19）；我方 `sub` 每字段变 + 位移固定（同址折叠）
- 修法：`int m_data[13]` → `m_data0..m_data12`（**布局/sizeof 不变**，纯标量成员形）
- 仓内先例：`GMAccounts.h` 的 `STPartyJobWireItem` 注释已逐字记载该机理

**O2 `++x[i]` vs `x[i] += 1` 是方向性的**（-O0 下寻址路径不同）

- 正例（ORIG 单路取址 + 自增）：`AddLoadingTimeReportStatistics` 9→**0**、`accFrameLagStruct` 260→**196**
- 反例（ORIG 双路重算）：`AddSchoolNo` 10→18、`PushOneFrameLagData` 10→15 ⇒ **每次先看 ORIG 形态**

**取证式否决（流程样板）**：`RandomOption` 布局争议——反汇编 ORIG 的 `RandomOption::reset()` 逐字段比对
（+0/+3/+6/+9/+0xa/+0xd）证明**我方布局正确**，`+0/+4/+8/+0xc` 的 dword 读法在 ORIG 源码里就是一次 cast 视图
⇒ **不动跨服务共享头**，按死胡同登记。凡涉共享头布局的猜测，先做这类独立取证。

**深度否证**：`FrameLagDataStruct` 的 sizeof 0x248 由 ORIG `mov $0x92,%eax; rep movsl`（146×4=584B）硬锚定；
`m_i` 起点 0x100 + 类尾 0x248 ⇒ 41 元素，但 **41 为质数、无法分解成任何二维形状**，而 ORIG 明确是二维展开
（内层 j 走 `%ebx`）⇒ 「41 元素 + 二维展开」只能靠 cast，已穷举 8 种 cast（最优 12）⇒ 维持 ndiff=2 登记死结。

### 5.11 2026-09-15 判据表定稿（全队交叉验证，含边界与反例）

**口径**：statics **9 ✓** · monitor **69** · guild **73** · dbmw **64** · auction **1 ✓** · point **1 ✓**
（311 → 217，累计翻正 **94 件**；game 333 逐项不变）。

#### A. 最高价值判据（各有多件归零实证）

| 代号 | 内容 | 适用条件 | 反例/边界 |
|---|---|---|---|
| **C1** | cast 层决定 `ARRAY_REF` 的 base/index 展开次序（零 cast ⇒ index 先） | 权威类型**本就有字段**（O1 先核） | 权威头无字段（shared 极简版 / `DBMW/DNFPacket.h` 空类）⇒ 不可行 |
| **C2** | 元素内部数组**标量化**（`int m_data[13]` → `m_data0..12`，布局不变） | 症状 = ORIG 统一 `sub` + 各字段各自位移 | 已折成单条 `mov disp(%eax,%ecx,1)` 者无效 |
| **重建遗漏语句（三连）** | ①我方明显短 ②ORIG 有某槽而我方完全没有 ③**加占位后帧变大** ⇒ **回 ORIG 补语句** | 三条同时成立 | 帧不变且槽组对齐才是真 S5；单条差值 1 条多为对齐 nop（`ROI_SetSearchResult`） |
| **短路物化族** | 先比该处**是否已逐条一致** → 一致则不动；不一致且 ORIG **有**五连 `mov $1/%jmp/mov $0/test/je` ⇒ 保留短路；**无** ⇒ 改**正条件嵌套 if**（不写 `!`、不 return） | `objdump -d \| grep -A4 'mov $0x1,%eax' \| grep 'mov $0x0,%eax'` 筛 | `OnRegisterEventItem` 拆后 1→23（已一致还拆）；同函数内多处必须逐处判 |
| **S5 占位局部** | ①只加 N/4 个**独立 4 字节变量**（差 8 ⇒ 2 个 `int`，**绝不用单个 8 字节**）②两步分离：先用 `int` 调**序位**，未归零再换 `char[4]` 调**类型**③三种同尺寸写法都试取最小 | 前提 = **某个可寻址槽组整体平移 N 且我方浅** | 槽集合相同仅帧差 ⇒ EH/对齐区，占位必无效；两个占位必劣化；不预设 `char[4]`（有件 `int`=11 反优于 17） |
| **X2 跨服务照搬** | 先比 ORIG 的**帧/槽集合/槽顺序**三者同形，**再比**我方长度是否已等于本服务 ORIG ⇒ 才**逐处移植**（非整函数替换） | 同一 ORIG 函数被多服务重编 | 同名不同形（`OnLogin` 帧 0x8c vs 0xac）⇒ 照搬会引入错误形态；普查 29 个候选中仅 2 个真同形 |

#### B. S3 与「我方多局部」族的分辨（症状零重叠）

| 族 | 槽集合 | 帧 | 函数开头 | 修法 |
|---|---|---|---|---|
| **S3 NULL 归属** | **逐项相同**（如各 6 槽） | **相同**（0xac） | **恰有一条** `movl $0x0,-0xNN` | 外层指针声明提前并置 NULL、值指针去掉 `= NULL`（`OperateByLv` 等 4 件） |
| **我方多局部** | 我方**多槽** | 我方**更大** | — | 先核 ORIG 有无该槽；**不可盲目 R1**（`OnDBLoadRequestGuildBoardWrite` 删后 9→34、`OnLogin` 内联 `oldGs` 后 409/365） |

#### C. 方向性判据（**用前必看 ORIG 形态**，否则劣化）

| 代号 | 内容 | 正例 | 反例 |
|---|---|---|---|
| **S1** 作用域深度 | 槽号同时取决于声明所处作用域层级 | `join`/`tokenize` 4 件归零、`accFrameLagStruct` 324→260 | 槽集合已逐一相同者，改作用域生成 .o **逐字节不变** |
| **S2** 比较操作数次序 | ORIG 把 `end()` 放左 ⇒ `end() == pos` | `sendGuildBoardData` 64→63、`OperateByLv` 19→11 | statics 4 点、FrameLag 2 点、monitor 1 点、guild 1 点全劣化 |
| **O2** `++x[i]` vs `x[i]+=1` | -O0 下寻址路径不同 | `AddLoadingTimeReportStatistics` 9→0、`accFrameLagStruct` 260→196 | `PushOne` 10→15、`AddSchoolNo` 10→18（ORIG 是双路重算） |
| **P3/S4** `register` | 由 ORIG 的**栈槽有无**决定 | VillageLane `register int t` 必删、`register int i` 必删 | guild `register int ret/flag` 必留、`register bool over` 必留、`register bool out __asm__("al")` 必留 |
|---|---|---|---|

#### D. 结构族手法（块/赋值/初始化）

**P-A** 正向 `if` 包裹去尾部 `jmp`+`nop`（条件：early-return 在函数首且其后无语句）· **P-B** 空 then → return ·
**P-C** 多 early-return 合并为嵌套守卫 · **P-D** `bool` 物化 + 分支互换 · **P-E** 重复计数器**共槽复用** ·
**P-F** 删重建期具名局部（须先看 ORIG 有无该槽）· **P-G** 指针算术 `(a+i)`（不可无差别套用）·
**P-H** 具名值局部 → 具名**指针**局部 · **P-I** 复合 `while` 条件 → `if/break` · **P-J** R6 字段 signedness ·
**U1** 末尾显式 `return;` 复现 if/else join 残留（**位置敏感**）· **U2** `return c?A:B` → 两臂 `return` ·
**U3** `map.erase(it++)` vs 具名 `cur`（指令同、槽分配不同）· **T1** 包字段作计数器/结果槽 ·
**T2** 槽集合 30 秒分诊（`objdump -d --disassemble=<mangled> \| grep -oE -- '-0x[0-9a-f]+\(%ebp' \| sort -u`）·
**P1** 聚合赋值触发 LHS-first（视图须**非 packed**）· **P2** 赋值目标 signedness 决定载入序 ·
**P4** 零填的**声明点位置**进二进制 · **P5** 先载入者 = 源码**左操作数** · **P6** pair 成员赋值次序可观测 ·
**X1** 成员数组 `sizeof(元素)` == ORIG 步长（**否则产生假信号**）· **X3** `(*it).second` vs `it->second`。

#### E. 硬约束与陷阱

- **packed 结构 + 非 const 引用形参**：具名字段访问无法绑定（编译失败）⇒ 字节偏移 `*(int*)(&x + N)` 是该调用点**唯一合法写法**。
- **多维索引必须在 .h 真声明**，`.cpp` cast 视图无效；**C1/C2 同理**（cast 不改展开次序）。
- **共享极简头**（`shared/packet/include/` 无字段版）与 `Game/` 版同名不同内容 ⇒ 改前先确认**被 include 的是哪一份**。
- **`game`/`dbmw` 用 Makefile**（`make -C build/{game,dbmw} -j4`），其余为 ninja；**dbmw 的 make 时间戳不可靠 ⇒ 改源码后必须先 `rm` 对应 `.o`**。
- **`game` 的 ELF 在本环境无法重链接**：`make -C build/game` 编译阶段通过，但**链接**阶段缺
  `toolchains/freetype32/lib/libfreetype.a`（该文件全盘不存在）⇒ `compare_svc_full game` 的 36113/333
  **逐轮不变、已不再构成回归信号**。跨服务头改动请改用**目标文件级 md5 闸门**（改动前后 `md5sum <受影响>.o`，
  逐字节一致即证明 game 机器码未变——这比计数更强）；受影响 `.o` 清单按实际 include 面枚举，
  实例：`Packet_Monitor_Guild_Chat_Hyper_Link.h` 改名后 `GameWorld.cpp.o` md5 前后完全相同。
- **dbmw 源码目录是全大写 `DBMW/`**。
- **交付前必须跑一次全服务重编**（孪生函数笔误曾阻塞整个泳道群）。
- **一 `.cpp` 一写者**；共享头改动须先广播、改后核 game（36113/333）等全服务。

#### E-2. 权威口径（**源码标记不是权威**）

源码里的 `[DNF-NONIDENTICAL] DNF-*-DIFF-00NN` 标记**会过期**：实测 `DBMW/DBManager.cpp` 有 **76** 个标记
而实际残差仅 **37** 件（约 39 个已 identical 却仍标 DIFF）；`Guild/DNFPacketTranslater.cpp` 75→46（29 个过期）；
全仓共 **463** 个标记（`Monitor/DNFPacketTranslater.cpp` 63 · `Guild/DNFGuild.cpp` 43 ·
`DBMW/DNFPacketTranslater.cpp` 26 · `Statics/Statistics.cpp` 13 …）；`Auction/` 目录整体零标记。
**⇒ 做件前的唯一权威判据**：
```bash
python3 source/toolchain/compare_svc_full.py <svc> --non-id     # 权威残差 mangled 清单
nm <该 .o> | grep ' T '                                        # 定义位置核对（勿按符号前缀 grep）
```
**清理规则**：以 `--non-id` 清单为唯一判据 —— 有标记但不在清单 ⇒ 改为
`// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-…-00NN 已消除）| <svc> | <类::函数>`；
在清单但无标记 ⇒ 补标记；**只动注释行**，改完重编核账且**服务计数必须逐项不变**。

#### E-3. 跨服务 `.o` 实验纪律（两次事故后固化）

`game` 的 ELF **无法重链**（缺 `libfreetype.a`，全盘不存在），其 `.o` 一旦损坏只能重建，而任何未验证的
game 侧改动**不可逆地无法判定**。两次 `rm <game .o>` + 显式目标名不匹配造成 40 秒–2 分钟缺 `.o` 窗口
（均已完整恢复 + md5 校验）。
**规则**：跨服务 `.o` 的受控实验一律在 **`/tmp` 独立 probe 台**；确需在共享 `build/<svc>/` 实验时，
**先 `make -n <target>` 干跑确认目标存在**，且**实验后立即恢复并 md5 校验**。
**第 1 级黄金标准**：用**两份头（改动版 vs 改动前快照）分别强制重编同一批 `.o`**，逐对 md5 比对
（比「只比前后」更强：可排除环境噪声）；**「改动前」的 md5 必须在改动前当场记录**，否则会把已污染的
产物当成基线（本轮发生过一次，已由真 A/B 纠正）。
**第 0 级判据**：枚举 `.o` 时须**核对路径里的源目录**（同名 `.o` 可能来自各服务目录的独立源文件；
只有源路径匹配才受影响）；**第 1 级**枚举 include 面时须**匹配 `#include` 指令**
（文件名出现在注释里不算依赖）。

#### F. 已确证死胡同（8 族，附负例锚点）

1. **STL 类类型局部 + BIND/EH 展开顺序族**（最大族）2. `lea`/`add` 地址计算族 3. 中转 `mov` 残留族
4. 槽使用次数分布族 5. `jne`/`je` 分支布局族 6. `push`/寄存器分配族 7. 大对象栈复用族 8. 尾部 jump-padding 族。
**负例锚点**：槽集合逐项相同仅两槽互换 ⇒ 占位四位置全劣化（6→44/11/11/44）；槽集合相同仅次数分布不同 ⇒ 加值局部 119→202；
槽集合相同仅帧差 4/8 ⇒ EH/对齐不可见区（dbmw 五种占位写法全劣化）。

#### G. 槽序问题的二分判据（S5 与 B3 合并，附 30 秒判断顺序）

**第 ⓪ 步（先比帧大小，最快）**：`grep 'sub \$0x..,%esp'` 两侧对比 ——
· **帧变大**（加占位后）⇒ ORIG 那个槽是**真实变量** ⇒ 回源码**补语句**（重建遗漏族，见 A 表）；
· **帧不变** ⇒ 进入下表分支。

| 症状（槽集合） | 帧 | 机理 | 修法 | 实证 |
|---|---|---|---|---|
| 逐项**差 N**（我方浅） | 相同 | 缺一个槽的分配起点 | **加占位**：单个 4 字节 → 先扫**序位** → 再比**类型**；差 ≥8 时**全试同量级写法**（无默认解） | `SearchByLv` 12→6→**0** · `Load` 77→**27** |
| 逐项**相等但两槽互换** | 相同 | **声明序位/R4** 决定谁先占槽 | **改声明位置**（局部上移/换序）；**占位无效** | `SendGuildInfoToMembers` 9→**0** · `pp_container` 前置四件 |
| 两侧**各有独占槽** | 任意 | 反向平移/混合态 | **非遗漏、非简单占位**；若互斥 ⇒ 登记「两作用域占位」未解机理 | `UpdateDisjointAvatarStatistic` 161 · `SendCharacRank` 14 · `Load` 余 27 |

**混合态的零扰动现象（ItemTaskLane 实测）**：在「两侧各有独占槽」件上，**S5 占位 / B3 局部上移 / S1 作用域层级 三轴可能全部"逐字节无变化"**
（编译器分配起点不动）⇒ 此时应把精力放回「**缺/多语句 + 对象内部偏移**」轴
（该件的真实收益来自 **T1**：删 `expired[30]+count`、改用 `pkt.m_ipgNo[]` 存结果 + `pkt.m_sellEndNum` 计数 ⇒ 帧 0x264→0x1e4、101→88）。

**混合态的复合族判例**：`SaveDBPowerWarRank` 的 **W×A×S 三维 8 格**（base 230 恒最优）+ 该件最终由「内联 `winnerSide`+`app`」改善到 **190**，
而 ORIG 确证的**缺语句**（同 `i` 两次 `vector::at`，写 `statuePkt.m_rankers[i]` 与 `noticePkt.m_data[i]`）补回后反而 399/236 ⇒ **单轴不可解**，
矩阵与逐格 ndiff 已登记在函数上方注释。

### 5.12 2026-09-15 第九轮：落点 D 四件套（该族翻案）+ 权威链与工具守卫

**最终口径**：statics **1106/9 ✓** · monitor **2035/69** · guild **1855/70** · dbmw **1362/62** ·
**auction 1210/0 ✓** · **point 1210/0 ✓**（311 → 210，累计翻正 **101 件**；3.5 个二进制达标）。

#### A. 落点 D 四件套 —— 「循环体尾 join jmp」由死胡同翻案为可调族

```cpp
for (i = pContainer->begin(); i != pContainer->end(); ++i)   // ① for 的 increment 承载 ++i
{
    result = GetAuctionItemInfo(...);
    if (result != 0) { continue; }   // ② early-continue（原写法 if (result == 0) { body }）
    {                                // ③ 裸复合块包住余下 body
        ...
    }
}
return 0;                            // ④ 函数末尾 return 保留（覆盖落点 A）
```
**必要三条件**（缺一即结构与语义冲突，**不可批量套用**）：
1. **长度差 = 我方「少」1~2 组 `jmp+nop`**（长度相等 ⇒ 加裸块**必无变化**；我方多 ⇒ 方向相反，应消 join）；
2. **跳过路径为「继续前进」型**（`continue` 落到 `for` 的 increment）——`break`/`goto out`（提前退出）型**结构上不适用**；
3. 循环条件**无副作用赋值**（如 `do { x = f(); if (…) break; n++; } while (n<3)` ⇒ 改 `continue` 会死循环或语义反向）。
**正例 4**：`ROI_SetSearchResult` **1→0**（四件套齐备）· `GetAttendanceInfo` 7→**0** · `UpdateHuntingPoint` 8→**3** · `QueryCashMemoryBlackList` 10→**5**。
**反例 8+**：长度已相等者 · `break` 型（`HWSpecSend`/`ItemTaskLane`/`GuildC`/`GuildB`）· 条件含副作用者 · 同 TU 反向实例 · `AddSecretShopStatistic`（恒 12）。
**机理**：两处生成**同目标**的 jmp，GCC 对**两级嵌套 join 各发一条** ⇒ 单改一处会被另一处抵消（**A 与 D 必须配对**）；余数常为「GCC 两条同目标 jmp vs ORIG 一条」（`UpdateHuntingPoint` 余 3 条即此）。

#### B. 权威链（三级分工）与工具守卫

| 工具 | 口径 | 用途 |
|---|---|---|
| `compare_svc_full.py` | 服务级 strict/ae/near/diff（含目标重索引） | **「是否 identical」唯一权威** |
| `source/toolchain/mdiff_ta.py` | 单函数 STRICT/AE/NEAR/DIFF（ELF + 目标感知） | **级别判定** |
| `mdiff`（.o，剥目标） | 单函数 ndiff | 快速迭代判趋势，**禁止单独作终判** |

**加强版结论**：**`ndiff=0` 不仅不能证明 strict，甚至不能证明「已归入 identical」** ——
`NoticeMemberLogin_Out`(224/224)、`NoticeChatMsgToMemberMembers`(137/137) 在剥目标口径下 `ndiff=0 且 equal 全等`，
而服务级仍判 **NEAR**（残差恰好全是跳转目标差异）。
⇒ **第 4 类族（块尾标签次序）快速识别指纹**：**剥目标 ndiff=0 + 服务级/含目标口径仍 NEAR/DIFF**。
**工具守卫（`mdiff_ta.py` 内置两道拒绝）**：① 第 2 参数若为路径/`.o`/`.cpp` ⇒ 拒绝（防「三参数误用」：
`mdiff_ta.py <svc> <某.o> <mangled>` 会把 `.o` 当符号名，查无此符号后**假报 `orig=0 ours=0 STRICT`**，
本项目已有两条泳道因此误声明 STRICT）；② `--orig/--ours` 指向 `.o` ⇒ 拒绝（未链接 `.o` 与本 ELF 口径不同）。
**工具验证纪律**：任何新工具先用「**已知 identical 且自己从未改动**」的函数校准；不过关即弃用
（GuildC 的自建脚本对未改动的 `TcpProcess`/`UdpProcess` 误报 74 而弃用）。
**双口径读数**：`strict 残差 ≈ ae 口径的 1.06–2.5 倍`（因件而异）⇒ 交付须**同时给两口径 + 级别**。

#### C. 读数纪律（并发窗口）

取数前确认**源与 `.o` 时间戳一致**且 `flock` 串行已完成；读数与自己登记不符时**先核时间戳**，
不要立即回退或重做实验（实例：`sendGuildBoardData` 8→70 虚警、`MakePacketDBPowerWarPoint` 4→5、
`mdiff` 输出全空、`ours=0`）。同源教训：`make <显式目标>` 在 `.o` 已删时报「没有规则」⇒ 退回整体 `make`；
`compare_svc_full` 读**链接产物** ⇒ 只编 `.o` 不重链时读数不变。

#### D. 过期标记治理

全仓 `[DNF-NONIDENTICAL]` 标记 **463 → 约 356**（清理约 107 个）；多文件已做到
**标记数 = 权威残差数**（`Guild/DNFGuild.cpp` 42→12 · `Guild/DNFPacketTranslater.cpp` 75→46 ·
`Monitor/DNFUserManager+DNFMemberManager` 18→3 · dbmw `DBManager.cpp` 76→37+4）。
**判据**：`--non-id` 清单为唯一权威；只改注释行；改后**服务计数逐项不变**（`DbmwDBManager` 的最强证据：
改后 `.o` **md5 逐字节不变**）；逐行 `assert` 原行含标记以防误伤；跳过他人在写的文件。

### 5.13 2026-09-15 第十轮收官：B3 五步 · R6 第三形态 · 工具治理

**最终口径**（每轮重链接后 `compare_svc_full.py` 实测）

| 服务 | identical | 非identical | 目标 ≤9 |
|---|---|---|---|
| **statics** | 1106（strict 882 + ae 224） | **9** | ✓ |
| **auction** | 1210（strict 934 + ae 276） | **0** | ✓ |
| **point** | 1210（strict 934 + ae 276） | **0** | ✓ |
| monitor | 2035（strict 1585 + ae 450） | 69 | — |
| guild | 1857 | 68 | — |
| dbmw | 1364 | 60 | — |

**累计翻正 105 件**（311 → 206）；**过期标记治理 463 → 280**（清理 183 个，多文件做到「标记数 = 权威残差数」）。

#### A. B3 判据升至五步（该族执行口径定稿）

| 步 | 内容 | 排除对象 |
|---|---|---|
| 1 | 槽集合差集：空 / 恒差 / 不恒定 | 平移族（⇒S5）· 混合态 |
| 2 | **差异是否落在槽操作数** | 不在槽轴者（epilogue / 地址编码 / 寄存器 / 常量 / 参数栈载入序） |
| 3 | 空差集 + 落在槽操作数 ⇒ 名义互换族 | — |
| 4 | 落盘 1–2 变体实测 | — |
| **5** | **该槽差异是否由「声明位置」唯一决定？若由求值序决定 ⇒ 无靶点** | 求值序族 |

**第 5 步的实证**：`CGuild::AddGuildFund` 四步全过（`lea -0x28` vs `-0x30`），但交换声明次序 **4→15 劣化**；
回退后定位真差异为 **`add $0xc0` vs `add $0x3b`（地址求值先后）**，且改声明次序后 GCC 仍先算 `fundPtr` ⇒ 与声明位置无关 ✓
**全队五步复核（21 泳道）**：第 2 步排除 17 条泳道 · 第 5 步排除 4 条（`AddGuildFund`/`SendPacket`×2/`OnLogin`/`CBuddyHandle::add`）· 第 1 步分流其余 ⇒ **B3 零靶点**。

#### B. R6 第三形态（读取侧 signedness）与两例产出

| 形态 | 可观测指令 | 触发处 |
|---|---|---|
| P2 赋值序 | RHS-first vs LHS-first | 赋值语句 |
| 比较指令 | `jg/ja` vs `jle/jbe` | 比较语句 |
| **扩展指令** | **`movzbl`（零扩展 ⇒ unsigned）vs `movsbl`（符号扩展 ⇒ signed）** | 读取单字节/位域字段之后 |

**指纹**：`mov[sz]bl`（建议宽口径 `mov[sz][bw]l`）序列比对 —— 对 -O0 内联/槽平移/寄存器选择**完全免疫**；
**两级用法**：粗筛（两侧计数比对，一条 `grep`）→ 细查（对齐 hunk 内同位置）。
**产出**：statics `m_detailCount` → `unsigned char`（`.o` 112→110、ELF 101→100、game 三 TU md5 逐字节中性）·
dbmw `DNFPacket.h` 的 `m_batchFlag`/`m_count`/`m_rest` → `unsigned char`（A/B：ndiff 109→**108**、失配 4→**3**、服务 1364/60 零回归 ⇒ **保留**）✓
statics `PushOneFrameLagData` 的 `char match` → `unsigned char`（**第 3 例**：ORIG `movzbl -0xd(%ebp),%eax` + `movsbl %dl,%ebx`
vs 我方仅 `movsbl` ⇒ 字段为 unsigned；验收 = `mnemonic_fp` 命中 **4→0**；`.o` ndiff 10→10 不变 ⇒ **纯语义级收益**）✓
**第一级快筛最终形态（与对齐无关，1 秒）**：**`movs*`（符号扩展）两侧计数不等 ⇒ 真候选**；
`movz*` 不等而 `movs*` 相等 ⇒ 内联/块布局差异（**非** signedness）✓
**严格配对判据（定稿）**：真候选 = 「**两侧都含 `mov[sz][bw]l` + 源操作数相同 + 扩展方向相反**」；
「一侧有扩展、另一侧没有」= 100% 形态差异（求值序/取值族/载入调度），**不计入候选** ✓
**工具陷阱（实测，全队 8 条反馈）**：`mnemonic_fp` 的「命中=N」**含 hunk 内位置错位配对与上下文行**，
不等于指纹计数差 ⇒ 宽/窄口径的「命中」**必须第二级逐条读内容**才能定案；
`GuildB` 的计数快筛（两侧 `movzbl`/`movsbl` 计数不等即候选）是正确方向，`z`/`s` 分侧 + 严格配对是其精确化 ✓
**全队覆盖**：21 泳道扫描完毕（含 `AuctionPoint` 的 21 件**归零件阴性组** 21/21 零命中）⇒ 真实命中 **3/197**，
其余上报「命中」经逐条读内容全部归因于：大段 `replace` 内错位配对 · 地址编码/计算族 · 分支布局族 ·
地址求值序 · 载入调度 · 取址/建值形态 · EH 段 · 槽/寄存器族 · 上下文行误计 ✓

#### C. 工具三件套治理（一文件一写者 + 冻结令）

| 文件 | 唯一维护者 | 能力 |
|---|---|---|
| `source/toolchain/mdiff_ta.py` | **Main** | 目标感知（含 `<#NN>`）+ 两道参数守卫（拒 `.o` / 拒路径当符号名） |
| `source/toolchain/mnemonic_fp.py` | **DbmwDBManager** | 对齐 hunk 内助记符指纹（R6 第三形态）；只取 `replace` 成对行 |
| `source/toolchain/tgt_aligned.py` | **GuildA** | LCS/Myers 对齐 + 全函数目标块多重集 + `--calibrate` 三组 PASS/FAIL（rc=3）+ 批量 |

**本轮教训（三条纪律）**：① **工具一文件一写者 + 跨泳道只读**（同一文件曾被 6 方轮写 ⇒ 各方读到不同副本 ⇒ 多起「工具坏了」误判）·
② **调用新工具前先读 docstring/`--help` 确认参数契约**（`--calibrate <svc> <mangled>`，开关在前）·
③ **报缺陷必须附「命令行 + 文件 md5/字节数 + 报错原文」三项证据** ✓
**校准件白名单**（均「已知 identical 且报告者未改动」）：monitor `_ZN14CPacketDecoder10TcpProcessEv`(242/242) ·
`_ZN7CMember21GetConnLowerMemberCntEv`(41/41) · guild `_ZN9CGuildWar12SameRankWorkEv`(164/164) ·
statics `_ZN14CNetworkThread8dispatchEPv`(543/543) · dbmw `OnAddBuddy`(109/109) 等；
**排除项**：空函数（`OnHeartBeat`/`OnEventEnd`/`OnEventStart`，本 ELF 中 `orig=0` ⇒ 会被误报 DIFF）✓

#### D. 缺口性质（诚实评估）

monitor **69** / guild **68** / dbmw **60** —— 剩余件经 **10 类系统化扫描全部零靶点**
（(a2) 签名法 · 成员函数选择 · packed 对齐 · B3 五步 · R6 三形态 · 目标列 · 第 4 类指纹 · 落点 A–D · S5 · `end()` 缓存），
主体为**寄存器分配 / 求值序 / 块布局 / EH / 编译器栈槽复用**（已穷尽的不可调族）；
本轮新增机理命中面为 **1/197** 量级 ⇒ **需要新机理**才能继续成批推进。

### 5.14 2026-09-15 第十一轮：signedness 三家族穷尽 · cast 级语义修复 · 工具纪律五次实战

#### A. signedness 三家族（全队结案，206 件全量核账）

| 家族 | 层面 | 判据 | 落盘 | 锚点 |
|---|---|---|---|---|
| **载入级** | `movzbl`↔`movsbl`（单步 `movsbl 0x??(%reg),%eax` = 对象基址） | 字段改 `unsigned char` | **3 处** | statics `m_detailCount` · statics `PushOneFrameLagData::match` · dbmw `DNFPacket.h` 三字段（`m_batchFlag`/`m_count`/`m_rest`） |
| **表达式级** | 两连 `movzbl` 载入 + `movsbl %al,%reg` | **改该读点值表达式**：`(int)(unsigned char)x`（字段不动、语义等价、1 处、零跨服务面） | **2 处** | dbmw `OnRequestIPCounterList`（命中 1→0 · `.o` 78→76 · ELF 109→107）· dbmw `QueryPartyJobStatisticCreate`（命中 2→0） |
| **cast 级** | `idiv`↔`div`（ORIG 有 `sar $0x1f`/`cdq` + `idiv`，我方 `div`） | **删/改正操作数上的 `(unsigned …)` cast** | **1 处（语义级）** | statics `SendDBLagStatistics` |

**排除类**（与三类同构的负判据）：① 两侧同形 ⇒ 非靶点 ② 单侧出现且无符号扩展特征 ⇒ 长度/内联族 ✗
③ 同计数位置互换 ⇒ 须做序列级比对（计数相等不足以排除）④ `movsbl -0xNN(%ebp)`（栈帧基址）⇒ 局部变量扩展，与字段无关

**全量核账（Main 亲跑，206 件）**：
```python
# 对 statics 9 / monitor 69 / guild 68 / dbmw 60 逐件提取两侧函数体，比对 idiv/div/cdq/cltd 计数
扫描残差件: {'statics': (9, 9), 'monitor': (69, 69), 'guild': (68, 68), 'dbmw': (60, 60)}
idiv/div/cdq 计数不等件数: 0
```
⇒ **该轴自此关闭**，各泳道已移出复查清单 ✓

#### B. cast 级唯一真实命中（`SendDBLagStatistics`）与「语义优先」裁决

```cpp
- (unsigned int)m_modules[i].m_data0 / (unsigned int)m_modules[i].m_data3,
+ m_modules[i].m_data0 / m_modules[i].m_data3,          // 三处均删 cast
```
ORIG 三处全为 `mov %edx; sar $0x1f,%edx; idivl -0x45c(%ebp)`（**有符号除法**）；原 cast 让 GCC 生成 `div`（无符号）
⇒ **真实语义差异**（负数时结果不同，且写进 SQL 统计值）⇒ 修 ✓

| 指标 | 改前 | 改后 | ORIG |
|---|---|---|---|
| `sar` / `idivl` 计数 | 1 / 1 | **3 / 3** | **3 / 3** ✓ |
| 指令长度 | 300 | **302** | **302 ✓ 首次完全一致** |
| `.o` / ELF ndiff | 8 / 10 | 18 / 18 ↑ | — |
| statics 服务 | — | **1106/9** | **零回归** ✓ |

**⇒ 判据（已入 §5.13 体系）**：**当「语义与 ORIG 一致」与「ndiff 不上升」冲突时，前者优先**，
前提三条：**(a)** 差异是真实语义差（正负/溢出/截断可区分）· **(b)** 服务计数不劣化 · **(c)** 长度或形态向 ORIG 靠拢 ✓
（ndiff 上升属**正确代码的连带效应**：有符号除法需 `cdq`/符号扩展 + 额外寄存器约束）

#### C. 三条方法学沉淀（均由实战失误抽象而来）

1. **「源码写了 unsigned 除法」≠ 指令层生成 `div`** ⇒ **判据以指令层为准**，源码 grep（`\(unsigned[^)]*\)…[/%]`）**只用于定位书写位置** ✓
   （与 R6 的「字段声明 ≠ 实际扩展指令」同源：**声明层 ≠ 指令层**）
2. **`objdump` 取助记符的两种实现路径**：**按列提取**（`ln.split("\t")[2]`）**必须加 `--no-show-raw-insn`**，
   否则取到字节码列 ⇒ **空结果假阴性**；**全行正则匹配**（`grep -oE '\b(sar|shr|idiv|div|cdq)\b'`）**天然免疫** ——
   字节码列由十六进制字符组成，不含 `i`/`v`/`r`/`s`/`q` 等字母 ⇒ 不可能误匹配 ✓
3. **报长度一律用 `mdiff_ta` 首行的 `orig=/ours=`**（函数体口径）—— 避免与「全表表行口径」产生同一件两个长度差的假分歧 ✓

#### D. 工具纪律体系（本轮 5 次失误 → 5 次被挡下/自我更正）

| # | 失误 | 性质 | 处置 |
|---|---|---|---|
| 1 | `mnemonic_fp.py --fp` **只改输出头、不参与判定**（`rx` 定义但判定用硬编码 `ext`） | 全队 20 条泳道的新轴扫描是**数学必然的假阴性** | **`VillageLane` 按纪律③（命令行 + 文件 md5 + 源码证据）挡下** ✓ 维护者已修（`--fp` 驱动判定）+ 6 泳道用 objdump 独立法重跑 |
| 2 | `objdump` 按列取助记符未加 `--no-show-raw-insn` | 空结果假阴性 | `ItemTaskLane`/`StaticsStatMgr` 自查主动更正 ✓ 判据条 2 已精确化 |
| 3 | `NetThreadDispatch` 原型同 #1 缺陷 | 同上 | 自曝并修正 ✓ |
| 4 | `PktTransFrameLag` 把**修复前**的 LCS 读数与**修复后**的工具读数并列 | 跨实验态合并 ⇒ 结论作废 | 自我更正 ✓ 并抽象为**可执行门禁** |
| 5 | 同一件两个「长度差」（296/251 vs 320/277） | 口径分歧 | 两泳道约定统一为 `mdiff_ta` 首行口径 ✓ |

**⇒ 五道门禁（全队强制，结论中必须齐备）**：
**① 产物版本**（源/`.o`/ELF 的 mtime 或 md5，工具另附 md5）· **② 取数命令**（原样附上）·
**③ `两读数同侧：是/否`**（两读数须取自同一产物版本 / 同一修复侧）· **④ 工具版本**（扫前现取 md5）·
**⑤ 参数有效性**（新工具先用「必然非零的样本」验证参数真的参与判定）

#### E. 案例闭环：`OnDBLoadRequestGuildBoardOpen`（指令层差 ⇒ 反查源码形式）

```
ORIG  : 两处 `mov $0x66666667,%edx; imul; sar $0x2,%edx; mov; sar $0x1f,%eax; sub`（两处 /10 idiom）
OURS  : 一处（`sar $0x1f` 计数 1）
源码  : int pageCount = count / 10;
        int rem = count - pageCount * 10;    // ← 复用 pageCount ⇒ 无第二次除法
```
⇒ ORIG 源码几乎肯定是 `int rem = count % 10;`（`%10` 由 GCC 转成同一 idiom）⇒ **语义恒等**
（`count % 10 == count - (count/10)*10` 对全部 int 含负数成立：C++98 除法向零截断 ⇒ `-1/10=0`、`-1%10=-1`、`-1-0*10=-1`）✓
⇒ 处置：形改 `count % 10`（零语义风险）**已落盘并三级验收通过** ✓（`StaticsFrameLag` 执行）：

| 指标 | 改前 | 改后 | ORIG |
|---|---|---|---|
| `sar $0x1f` 计数 | 1 | **2** | **2 ✓ 完全对齐** |
| `mdiff` ndiff | 364 | **358** ↓ | — |
| equal | 133 | **144** ↑ | — |
| dbmw 服务 | 1364/60 | **1364/60** | **零回归 ✓** |

**⇒ 由此确立新子族：「手工展开取模 `x - (x/n)*n` vs `% n`」**
（**`x - (x/n)*n` ≡ `x % n`** 语义恒等，但指令形态不同：前者 = **1 除法 + 1 `imul` 回算**；后者 = **2 除法 idiom**）。
**判据**：**当 ORIG 在邻近位置出现「同一除数的两套 idiom」时，源侧几乎必是 `/` 与 `%` 并用** ⇒ 改 `%` 可零语义风险对齐 ✓
**指令层快筛**：`grep -cE 'sar +\$0x1f'`（或 `cdq`）**ORIG > OURS ⇒ 候选** ✓
**⚠ 源码层 grep 误报率高（边界，必须遵守）**：正则 `- … * <num>` 会把**线性插值/比例缩放公式**
`C₁ - (C₂ - x) * C₃`（如 `0xbc4 - (0x96 - count) * 0x14`）一并捞入 —— 它们**无除法、无取模** ✓
⇒ **必须先确认式中含除法**（形如 `x - (x / n) * n`，或 `x - k * n` 且 `k = x / n`），
**再以指令层「ORIG 同除数 idiom 次数 > 我方」定案** ✓（与「声明层 ≠ 指令层」同一条教训的又一实例：**写作形态 ≠ 生成指令**）
**价值**：首例「**指令层计数差 → 反查源码形式 → 判定语义等价 → 形态对齐实验**」闭环
（此前「cast 级 signedness」假设被 `NetThreadDispatch` 正确证伪 ✓）

#### F. 本轮缺口（诚实评估）

monitor **69** / guild **68** / dbmw **60** —— 剩余 197 件经 **11 类扫描轴**（(a2) 三/四/五法 · `call` 序列 ·
签名多重集 · T2/B3 五步 · R6 三形态 · **signedness 三家族** · 落点 A–D · S5 平移 · packed v3 ·
第 4 类指纹 · 成员函数选择 · `end()` 缓存 · 作用域调用序 · 遗漏族 v1–v3）**全部零靶点**；
主体为**寄存器分配 / 求值序 / 块布局 / EH / 编译器栈槽复用**（已穷尽的不可调族）⇒ **仍需新机理**。

### 5.15 2026-09-15 第十二轮：跨服务同源 · EH 五形态 · 读数门禁 6 条 · 七条新轴（全负）

#### A. 水位（本轮净增 2 件）

| 服务 | identical | 非identical | 目标 ≤9 |
|---|---|---|---|
| statics | 1106 | **9** | ✓ 达标 |
| auction | 1210 | **0** | ✓ 达标 |
| point | 1210 | **0** | ✓ 达标 |
| monitor | 2035 | **69** | ✗ |
| guild | 1857 | **68** | ✗ |
| dbmw | 1365 | **59** | ✗ |

起点 311 → **205**（累计翻正 **106 件**，**−34%**）；过期标记 463 → 280。

#### B. 新机理一：跨服务同源符号比对（兑现 1 件）

**原理**：同一 mangled 符号在多个服务各有一份平行实现（如 `CPacketTranslater::*` 在 monitor/guild/dbmw 各一份 `.cpp`）⇒
**若它在 A 服务已 identical、在 B 服务仍为残差，则两侧源码可 diff，差异即修复点** ✓

**三条预筛（效力排序已实证）**：

| # | 预筛 | 判据 | 效力 |
|---|---|---|---|
| **P1** | **ORIG 同形性** | 逐服务取 ORIG 该符号的**指令数 + 归一化文本**；不同 ⇒ **本就不是同一函数** | **决定性**（30 秒）· **未通过即结案** |
| **P2** | 函数体行数量级 | 两侧源码行数**差 > 2 倍** ⇒ 桩 vs 实现；±20% 内才进入逐行 diff | **非决定性**（实测 1.13 通过仍不可用） |
| **P3** | 业务指纹 | **日志行号 / 包 opcode / DB 调用名**任一不同 ⇒ 服务职责差异 | 补充排除 |

**重要边界**：**P1 的「同形」即使指令数相同仍须逐条比文本** —— 正/负对照：
`convertFromUtf8`（161=161=161 · ndiff=0 ⇒ **通过**）vs `ShowLogo`（46 vs 47 相近，但 **`putchar` vs ostream `operator<<`** ⇒ **业务异构**）

**最终账**：23 候选（放宽 29，**已含 game 组合**）→ P1 通过 **2** → 目标内可用 **1**：

```
_ZN10CSwapQueueISt5queueIP14CTcpRecvBufferSt5dequeIS2_SaIS2_EEELi2EEC1Ev
  ORIG：monitor/guild/dbmw 均 70 条、逐条同形（OURS：monitor/guild 相同，dbmw 92）
  修法：DBMW/DNFPacketBuffer.h:90  ->  void Init() throw() { ... }   （1 行）
  => mdiff_ta 68/68 STRICT ；dbmw 60 -> 59
```

**判据补充**：
1. **成员级存在性核查**（`nm -C <elf> | grep "<Class>::<Method>"` + `grep -rn "<Method>" <对侧源目录>` 双确认）
   —— 仅统计**类名级**符号数会误判：`CPowerManager` 在 game 有 **36** 个符号，但**无目标成员** ✓
2. **跨服务同名函数形参名不同**（`header` vs `pkt`）⇒ 用完整签名定位函数体会**误判「一方缺该函数」** ⇒ 用**前缀匹配**
3. **`convertFromUtf8` 的残差侧是 game ⇒ 净收益 0**（game `ours = None` 无法重链 ⇒ **只能作参照**）

#### C. 新机理二：EH landing 轴（兑现 1 件 + 五形态全部有实证锚点）

**原理**：GCC 4.4.7 对「**同一 TU 可见的弱成员 / 头内 inline / 模板实例**」的调用会**布置 EH landing（cleanup 块）** ⇒
与 ORIG 的 landing 分布不一致时即产生指令差。

**五形态（全部实证）**：

| # | 形态 | 处置 | 锚点 |
|---|---|---|---|
| 1 | 头内 inline/弱成员 + 被调**可判定 `nothrow`** | **加 `throw()` 可修** | **`CSwapQueue<...>::C1`**（已落盘） |
| 2 | 作用域含**虚调用 / 不可判定调用**（如纯虚接口 `CDBHandle`） | 不可修（`throw()` 无从施加；**R1-逆向亦必生临时对象** ⇒ 实测 ndiff 96→204） | dbmw `QueryIPCounter` / `queryTowerFullRank` / `QueryLoadARSInfo` |
| 3 | 同一被调**多调用点且 ORIG landing 不一致** | 不可修（单一 spec 不可满足） | monitor `EndEffectTask::_DoExecute` |
| 4 | 包类 ctor：**仅成员初始化、无成员函数调用** | 结构性无 landing（两侧天然相等） | `PktTransFrameLag` 的 **222** 个弱符号 |
| 5 | **landing pad 合并/汇流策略差异**（ORIG 用 `jmp <#N>` 汇流，我方各自 `_Unwind_Resume`） | 不可修（由 **try/catch 嵌套层次**决定） | monitor `CUdpNetworkThread::dispatch` |

**EH-2（镜像：我方少 landing）的前提被证伪** —— 原假设（日志宏**临时对象 vs 具名对象**）来自 **`.o` 口径的假阴性**。

#### D. 读数门禁 6 条（本轮最大方法学收益）

| # | 门禁 | 内容 |
|---|---|---|
| 1 | 产物三元组 | 源 / `.o` / ELF 的 mtime 或 md5（工具另附 md5） |
| 2 | 取数命令 | 原样附上 |
| 3 | **两读数同侧** | 两读数须取自**同一产物版本 / 同一修复侧**（**禁用修复后读数反推「曾经的状态」**） |
| 4 | 工具版本 | 扫前**现取**指纹 |
| 5 | 参数有效性 | 新工具先用「**必然非零的样本**」验证参数真参与判定 |
| 6a | **弱符号多实例** | `nm` 显示 `W` ⇒ **以链接后 ELF 为准**；**禁用单个 `.o`** 下结论（该 `.o` 可能是**未被链接采用**的副本） |
| 6b | **`.o` 符号级失真** | **`.o` 中所有跨 TU 外部调用都是未解析重定位** ⇒ `objdump` **不显示符号名** ⇒ **符号级 grep 必然漏计**（EH/`__cxa_*`/libc/日志）⇒ **符号级扫描一律用 ELF；`.o` 仅限「助记符 + 立即数 + 操作数」类** |

**6b 的实证：6 例同型自纠**（`CashLane` 1/0 假差异 · `GuildC` 全 0 假阴性 · `StaticsStatMgr` 1/0 · `StaticsRest2` 11 件全 0 · `GuildCore` 1/0 · **`UserLane` 6/0 ⇒ 阻止了一次无效实验**）
⇒ **结论正确不能免除方法合规**。

#### E. 本轮七条新扫描轴（全部为负结果或结构性发现）

| # | 轴 | 口径 | 结果 |
|---|---|---|---|
| 1 | 分配类调用指纹 | `_Znwm/_Znam/_ZdlPv/_ZdaPv` 计数（局部缓冲**堆 vs 栈**） | 1 件（已知项） |
| 2 | 编译器辅助符号指纹 | `__cxa_guard_*` · `__cxa_atexit` · `__stack_chk_fail` · `_Unwind_Resume` · `_ZSt*throw*` | 6 件，全部归入 EH |
| 3 | 跳转表/间接跳转 | `jmp *0x…(%reg,4)`（`switch` 产物） | **0 命中** |
| 4 | 残差 TU 分布 + Δ 聚类 | 各 TU 残差数 + `Δ = OURS − ORIG` 分布 | TU 集中于 `DNFPacketTranslater.cpp`（monitor 41/69 · guild 46/68 · dbmw 17/59）但 **Δ 分布宽泛**（Δ=0 者 monitor 20 / guild 11 / dbmw 11 ⇒ **纯寄存器/求值序差异**）⇒ 无共享根因 |
| 5 | `.rodata`/字符串集 | 两侧字符串集差 | 揭示**全局/静态对象集差异**，无函数级靶点 |
| 6 | 静态初始化器集 | `_GLOBAL__I_*` / `_GLOBAL__D_*` 差 | **ORIG 独有 40(statics)/49(monitor)/34(guild)/23(dbmw)** 但**与残差数无相关**（statics 缺 40 却仅 9 件残差）⇒ **非因果** |
| 7 | **`仅ORIG` 符号集（重建遗漏检测）** | `nm` 差集的 `T`/`t` 项 | **正结果：无缺失函数实现** —— 仅含 `_GLOBAL__I_*`（monitor 50/guild 37/statics 41）与 `.L*` 标签（dbmw 227）⇒ **重建覆盖 ORIG 全部函数** |

#### F. 缺口性质（定稿，诚实评估）

monitor **69** / guild **68** / dbmw **59** —— 剩余 **197 件**经 **28 类扫描轴 + 9 归因族 + 3 预筛 + EH 五形态** 系统性排查后**全部零靶点**。

**主体构成（有实证）**：
- **Δ = OURS − ORIG 为 0 或小值**（monitor 20 件 Δ=0 · guild 11 · dbmw 11）⇒ **纯寄存器分配 / 求值序 / 栈槽复用**（`-O0` 编译器内部行为）
- 大 Δ 者已逐件归因（内联/调用 · 平移 · 折叠 · 重复计算合并 · 地址编码 · 取址重解释 · 帧调整）

**在现有认识与工具链（CentOS 5.8 GCC 4.1.2-52 · C++98 · 32 位 · `-O0`）下，无源码级可修面。**

**语义条款证据**：`仅ORIG` 无缺失函数实现 + **6 类真实语义修复已落盘**（(a2) 5 例 · R6 载荷 5 处 · cast 级 1 · 取模形态 1 · packed 1 · 重建遗漏 1）。

### 5.16 2026-09-15 语义等价性验证（针对 205 件非 identical 件）

**方法**：对六服务的全部 205 件非 identical 件，逐件提取两侧函数体，比对三类**非地址特征**：
① **分支数** ② **`call` 目标集 + 计数** ③ **业务立即数集**（剔除 ELF 地址 `≥0x1000000` 与**帧调整** `sub/add $imm,%esp`）

**结果**：

| 类别 | 件数 | 说明 |
|---|---|---|
| **三特征全等** | **80** | **语义等价强证据**（控制流 + 调用集 + 业务常量全同，差异仅在寄存器/槽） |
| 有差异 | 125 | 逐类定性后**均为形状级**（见下） |

**差异逐类定性（抽查 + 全量归因）**：

| 差异型 | 例 | 判定 |
|---|---|---|
| **分支极性反转 + 显式 `jmp`** | `setMinIPCount`：ORIG `cmp $0xc8,%al; jbe <fwd>`（1 分支）vs OURS `cmpb …; ja <fwd>; …; jmp <fwd>`（2 分支） | **控制流相同、行为等价** ✓ 属「分支反转/落点 A」形状族 |
| **栈槽偏移重锚** | `0x176a` vs `0x176e/0x1772`、`0x171/0x173`、`0x18/0x1c` | 槽分配差异 ✓ 非语义 |
| **帧大小差** | `0x119c` vs `0x11ac`（差 0x10） | callee-saved/帧调整族 ✓ 非语义 |
| **模板辅助调用** | `_ZNKSt17_Rb_tree_iterator…` | 实例化/内联差异 ✓ 非语义 |
| **业务常量归属** | `AddLagStatistics` 的 `0xb` 单侧 | 常量折叠/锚点族 ✓ 非语义 |

**⇒ 结论**：205 件的差异经分类后**全部落在已登记的形状族**（分支极性 · 栈槽 · 帧调整 · 模板实例 · 常量折叠），**未发现行为级（控制流分支去向 / 调用集 / 业务常量值）差异** ✓
**⇒ 支持目标的语义条款**：`仅ORIG` 无缺失函数实现（§5.15-E-7）+ 本节的形状级差异分类 ⇒ **无证据表明存在语义差异** ✓

**⚠ 边界（诚实）**：本方法为**必要非充分**条件 —— 特征全等不排除隐藏语义差异，特征不等亦可能只是形状差异 ✓；**真正的语义验证需逐函数人工对照**（已完成的部分见 §5.11–5.15 的 6 类语义修复 ✓）

### 5.17 2026-09-15 补充：栈帧/槽偏移的结构性检查（语义安全正向证据）

**口径**：对 205 件残差逐件提取两侧的 `-0xNN(%ebp)` 槽偏移集合与 `sub $N,%esp` 帧大小，做两种异常组合检查。

**结果**：

| 检查 | 结果 | 含义 |
|---|---|---|
| **帧大小两侧相同而槽偏移集合不同** | **68 件** | 槽预算相同、**分配不同** ⇒ 归「槽分配/复用族」（编译器内部 ✓） |
| **帧大小不同而槽偏移集合相同** | **0 件** | —— |
| **两侧帧大小均相等者** | 多数（68 件中出现者全部帧等 ✓） | **⇒ 无「我方某局部类型尺寸不同」的证据** ✓ |

**⇒ 语义安全结论（正向）**：若我方某结构/局部类型尺寸与 ORIG 不同，`sub $N,%esp` 的帧大小**必然普遍不同**；
实测**帧大小一致** ⇒ **无类型尺寸不匹配的迹象** ✓ —— 与以下证据共同支持目标语义条款：
1. `仅ORIG` **无缺失函数实现**（§5.15-E-7）✓
2. 80 件**三特征全等** + 125 件形状级差异分类（§5.16）✓
3. **6 类真实语义修复已落盘** ✓
4. identical 的 8053 件**逐字节相同** ⇒ 语义必然相同 ✓
5. **帧大小一致**（本节）✓

**⚠ 边界**：以上均为**必要非充分**证据（不构成形式化证明 ✓）；真正的语义保证来自逐函数人工对照（§5.11–5.15 的 6 类修复即其产物 ✓）。

### 5.18 2026-09-15 新线索：单区域差异候选（14 件）—— 供后续机理开发

**口径**：对 205 件残差做**归一化序列 diff**（`difflib.SequenceMatcher`，归一化 = 剥符号名/地址/空白 ✓），统计**差异连续区域数**（hunk 数）。

**发现**：**14 件仅有 1 个连续差异区域**（其余 191 件为多区域成片差异）⇒ 单区域者**最接近「单点可修」**：

| 类型 | 件 | 形态 |
|---|---|---|
| **`jmp`+`nop` 尾部缺失（4 件）** ⚠ | guild `OnDeleteToBlackList` · `OnDBLoadReplyGuildBoardWrite` · `OnInnerPacketLogin` · `CGuild::LoadGuild` | ORIG 尾部多 `jmp <epilogue>` + `nop`（我方 fall-through）⇒ **「落点 D / 块布局 jmp」族**（该族在 auction `ROI_SetSearchResult` 曾成功归零 ✓） |
| `insert`（我方多） | monitor `EndEffectTask::_DoExecute`(+9) · `UpdateHuntingPoint`(+2) · `OnRegisterEventItem`(+2) | 多为 EH landing（已归因 ✓） |
| `replace`（等长替换） | guild `InsertHistory`(21-29) · `GuildMemLogout`(155-157) · monitor `onSocialEventRewardItemResponse` | 局部 8/2/2 条替换 |
| **归一化后完全相等（差异仅在地址/操作数）** | monitor `NoticeMemberLogin_Out` · `NoticeChatMsgToMemberMembers` | 即 NEAR(2) ✓（与已登记一致 ✓） |

**后续建议**：4 件 `jmp`+`nop` 型是**最值得试**的目标（族已在 auction 成功过一次 ✓）；**方法**：定位 ORIG 中该 `jmp` 的来源块结构（疑为 `if(X){…return;} else {…}` vs `if(X){…return;} return;` 之类的**块嵌套差异** ✓）⇒ 按 §5.14 规程做 1 变体实验 ✓
**⚠ 记录**：本轮 Main 尝试自行定位 `LoadGuild` 的对应源码块**未成功**（函数同名多处/行号定位精度不足 ✓，`assert` 阻止了所有误改 ⇒ **盘面零改动** ✓）⇒ 该实验**留给后续持有该 TU 的泳道** ✓

#### 5.18-补：`jmp`+`nop` 型的 3 变体实验（Main 亲做，**全部无效 ⇒ 已回退** ✓）

**目标件**：guild `CPacketTranslater::OnDeleteToBlackList`（尾部 `delete` hunk ✓）
**ORIG 尾**：`… call __cxa_end_catch ; jmp <epilogue> ; nop ; <epilogue>`（180 条）
**OURS 尾（原始）**：`… call __cxa_end_catch ; <epilogue>`（178 条）

| 变体 | 改动（源码尾部 `catch` 块） | 结果 |
|---|---|---|
| **V-last** | 在**末个** `catch (...)` 末尾加 `return;` | **182 条** ✗（产生了 `nop; jmp; nop` ⇒ **多一个 nop**，未对齐） |
| **V-both** | 两个 catch 末尾各加 `return;` | **178 条**（无变化 ✗ 编译器消除冗余 return） |
| **V-first** | 仅首个 `catch` 末尾加 `return;` | **178 条**（无变化 ✗） |

**⇒ 结论**：该 `jmp`+`nop` 由 **GCC 的 EH 区域布局**决定（疑 ORIG 在 catch handlers 之后仍有 **landing pad/cleanup 区域** ✓），**非源级 return 结构可调** ✗
**状态**：**已回退**（`diff -q` 与备份一致 ✓）· 六服务读数逐项不变（9/69/68/59/0/0 ✓）· 该件登记为「**EH 区域布局族**（与 `jmp`+`nop` 表观同形）」 ✓
**⇒ 4 件 `jmp`+`nop` 候选的处置建议更新**：优先核查**两侧 EH 区域/landing pad 数**（`__cxa_end_catch` 之后的区域 ✓）而非源码 return 结构 ✓

#### 5.18-补2：`jmp`+`nop` 型的追加 2 变体（**5 变体全部无效 ⇒ 定案** ✓）

| 变体 | 改动 | 结果 |
|---|---|---|
| V-last（前轮） | 末个 `catch` 末尾加 `return;` | 182 ✗ |
| V-both / V-first（前轮） | 两个 / 首个 catch 加 `return;` | 178（无变化）✗ |
| **V-after** | **函数作用域**（catch 之后）加 `return;` | **182** ✗ |
| **V-brace** | 末尾多包一个空块 `{ }` | **178**（无变化）✗ |

**⇒ 定案**：该 `jmp`+`nop` **非任何源级 return/块结构可调** ✓（5 变体穷尽 ✓）⇒ 确认为 **GCC EH 区域布局的产物** ✗ 勿再投入 ✓
**处置**：全部回退（`diff -q` 逐字节一致 ✓）· 六服务读数不变 ✓

### 5.19 2026-09-15 第十三轮（多智能体攻坚波）：五条新机理 · 累计翻正 112 件

#### A. 水位

| 服务 | identical | 非identical | 目标 ≤9 |
|---|---|---|---|
| statics | 1106 | **9** | ✓ |
| auction | 1210 | **0** | ✓ |
| point | 1210 | **0** | ✓ |
| monitor | 2036 | **68** | ✗ |
| guild | 1858 | **67** | ✗ |
| dbmw | 1369 | **55** | ✗ |

起点 311 → **199**（累计翻正 **112 件**，−36%）。本轮（第六至十三轮）中，**本波单波翻正 6 件**（monitor +1 · guild +1 · dbmw +4）。

**关键转折**：此前 §5.15-F 判定「剩余件全部归因于编译器内部行为、零可攻靶点」——**该结论被本波推翻** ✓。以「逐件微观 diff → 反推源码形态 → 试 ≤3 变体」的方法（NEAR 件优先），6 件被翻正，并产出**五条此前未被识别的源码级机理**。

#### B. 五条新机理（全部多次实证 + 边界明确）

| # | 机理 | 症状（指令级） | 修法 | 实证 |
|---|---|---|---|---|
| **A** | **占位 ↔ 具名临时 代偿对** | **两个同尺寸栈块「对调」**（非整体平移） | **删 `reservedSlot`/占位 + 内联具名临时**（如 `std::pair r = f(); if (!r.second)` → `if (!f().second)`）—— **缺一不可** | **2 例**：monitor `InsertCashMemorySetCharacterObject` **STRICT** ✓ · `CApplication::Load` ndiff **27→6** ✓ |
| **B** | **显式寄存器绑定** | **单一寄存器选择差异**（指令结构完全相同） | `register <类型> <变量> __asm__("<reg>") = <初值>;` | **2 例**：guild `GuildMemLogout` **AE** ✓ · monitor `QueryCashMemoryBuddyInfo` ndiff **7→4** ✓ |
| **C** | **字段 signedness 决定载入序** | **紧邻两条字段赋值的 LHS/RHS 载入序差** | **改字段类型**（有转换 ⇒ 先载 RHS；无转换 ⇒ 先载 LHS） | **3 例**：dbmw `DBMW/DNFPacket.h` 2 字段 ⇒ `OnGuildJoinByListApprove` **AE** ✓ + `SaveUnchangableGuildInfo` **AE** ✓（连带）+ `OnGuildJoin` **212→211** ✓ |
| **D** | **误加 `else`**（**语义级 ⚠**） | **唯一一条 `je` 目标不同**，且 ORIG 的 `je` 与相邻 `jne` **汇入同一入口** | **删我方多余的 `else`**（保留块作用域） | **1 例**：dbmw `SaveUnchangableGuildInfo` **AE** ✓ |
| **E** | **空类 → 真实字段** | **ARRAY_REF base/index 载入序**（我方 base 先 / ORIG index 先），两侧指令数相同、**无 `add` 生化物** | 给**空 packet 类**补与 .cpp 视图**逐项同布局**的真实字段 + .cpp 删 cast 直访 + 删废弃视图（clean cutover） | **2 例**：dbmw `QueryCubeStatisticCreate` **AE** ✓ · `QueryDeathTowerPlaydataPartyStatisticCreate` **AE** ✓ |

**各机理的排除边界（同样重要 ✓）**：

| 机理 | 排除条件（反例锚点） |
|---|---|
| **A** | 两个同尺寸块对调，**但函数内既无占位数组、也无具名临时对象** ⇒ 由 GCC 临时槽分配器决定 ⇒ **源码层无靶点** ✗（锚点：guild `AddGuildFund` · dbmw `insertHolePunchingResult` · monitor `CTcpNetSystem::SendPacket`） |
| **B** | 绑到**会被复用的寄存器**（`esi`/`eax`）⇒ **反向劣化** ✗（monitor `QueryCashMemoryBuddyInfo` V1: 7→13；V3: 7→61）；仅当该寄存器在 ORIG 中**确为该变量独占**时有效 ✓ |
| **C** | `.cpp` 内写 `*(unsigned int*)&field = ...` ⇒ 转换消除但丢掉 disp 融合寻址 ⇒ 多出 `add $0x4` ⇒ **劣化** ✗（408→410，两个独立泳道逐字一致）；⇒ **只能改头** ✓。**跨服务头**（`Game/` `source/shared/**`）⇒ game 不可重链 ⇒ **只登记「待环境」** ⚠（锚点：guild `GuildCargo::DeleteItem`，沙箱实证可 NEAR→AE ✓） |
| **D** | 若 `je` 目标差异**不在「与 `jne` 同入口」形态**上 ⇒ 属其它族 ✗ |
| **E** | 需先 grep 确认**无 `sizeof`/指针步进**依赖 ✓（两例均 0 命中 ✓）；若视图与类**无法用同一套字段表达**（packed 与对齐冲突）⇒ 不可行 ✗ |

#### C. 两项附加发现

**C-1｜`误加 else` 是语义级缺陷 ⚠**：`SaveUnchangableGuildInfo` 的 ORIG 源码**无 `else`** ⇒ then 块成功出口与条件为假出口**汇入同一点** ⇒ 被调语句块**无条件执行**（会长改名场景**两条 SQL 都发**）；我方重建时误加 `else` ⇒ then 成功后**直接 return 1** ⇒ **执行路径不同** ✗ ⇒ 已修正 ✓
**⇒ 该形态应全库复扫**（症状：唯一一条 `je` 目标不同且 ORIG 的 `je`/`jne` 同入口 ✓）

**C-2｜空类迫使 cast ⇒ 决定 C1 展开序**：`Packet_DBMW_*` 系列在头里若为**空类**，.cpp 只能用视图 cast ⇒ **必然 base 先** ✗ ⇒ 补真实字段后可直访 ⇒ **index 先** ✓
**⇒ 已证 14 种 .cpp-only 写法全部无法等效**（cast 表达式 / 双 cast / 解引用 / 引用 cast / 具名指针 / const 指针 / 成员指针 / 指针算术 / 3 种派生视图 ✓）⇒ **必须改头** ✓

#### D. 已确认为「不可调」的本轮取证（附实测）

| 件 | 结论 | 证据 |
|---|---|---|
| monitor `NoticeMemberLogin_Out` · `NoticeChatMsgToMemberMembers` | **块尾标签次序族** | 剥目标 + 地址归一化后 **ndiff=0** ✓ |
| statics `FrameLagDataStruct::init` | **编译器常量拆分轴** | 4 后端探针（4.4.6-3 基线 / 4.4.4-13 / 4.4.7-23 / 4.1.2-52）**全部不匹配** ORIG 的 `(imm, disp)` 拆分 ✓ + **结构性约束**：ORIG `PushMonitoringSpecData` 的 `rep movsl $0x92`（146 ints = **0x248** = 现 `sizeof`）**当前 identical** ⇒ 改为 42 元素（0x250）会破坏它 ⇒ 不可兼得 ✓ |
| monitor `CUserManager::AddSchoolNo` | **BIND_EXPR 声明序族** | 两侧帧同为 0xfc、长度 259/259、归一化后 residual diff=0 ✓ |
| dbmw `insertHolePunchingResult` | **栈布局/帧分配族** | 87 条**逐条相同**，仅帧 0x9c vs 0x6c + 5 槽偏移 ✓；探针穷举 3 后端 × 8 声明位 + 未使用变量/`(void)`/`sizeof` 使用/派生视图 ⇒ **无法复现浅池 0xc** ✓ |
| monitor `CApplication::Load` 残余 6 条 | **栈块顺序族** | ORIG 侧 `pkt2(10B)→pkt3(4B)→now(4B)→pt(44B)` 地址**完全连续**；我方 `now/pt` 属 try 块直接变量 ⇒ 按 GCC BIND_EXPR 批处理规则先分配 ⇒ 下推 4 字节 ✓（L5 方向对但引发别处回退 ⇒ 保留 L4） |

#### E. 新测得的知识资产

**GCC 4.4 `-O0` 栈槽分配规则（3 个探针实测 ✓）**：
> 各 **`BIND_EXPR` 的 vars 按声明序「一次性」加入**，**外层块先于嵌套块**；地址**自高向低**（先加入者得较高地址 ⇒ 偏移绝对值较小）✓
> 探针：① `Big o1 / {Sm o2} / Big o3 / Sm o4` ⇒ `o1=-0x1c, o3=-0x30, o4=-0x34, o2=-0x38` · ② `f1(x=-12, p1=-16 匿名块, q1=-20 if块)` vs `f2(x=-12, q1=-16 函数体, p1=-20)` · ③ `SendPacket` 结构精确复现（`G g=-0x28, It it=-0x2c`）
> 工具链：`source/toolchain/cmake/dnf_monitor_cxx.sh`（cc1plus 4.4.6，与 monitor 构建同源）

**跨服务同源第三例（可一次解 3 件 ✓）**：`CTcpNetSystem::SendPacket` 在 **monitor / guild / dbmw 三服务均为 NEAR ndiff=6 且逐条同形**（ORIG 槽位亦一致 ✓）⇒ 若攻破可一次翻正 3 件 ✓（注：源码注释中「guild 已 identical」的旧说法**不成立** ✓ 已更正）

#### F. 方法论要点（本波验证有效 ✓）

1. **NEAR 优先**：与 ORIG 仅差 1-2 条的件最易反推源码形态 ✓
2. **逐条全对照**（而非统计口径）：把两侧函数体**逐条列出**并标注唯一差异位置 ⇒ 直接指向源码语句 ✓
3. **先验证前提再试形**：`NEAR 攻坚` 中多次出现「3 个候选形态前提不成立（类型无继承/无转换 ✓）⇒ 一次编译都不浪费」✓
4. **`/tmp` 全隔离预编译**：改头前在 `/tmp` 建全套副本预编译并做**全 TU 函数级指纹**（617 函数中仅目标变化 ✓）⇒ 零风险验证 ✓
5. **变体上限 + 劣化即回退**：每件 ≤3 形态（结构性机理可放宽到 4-5 并说明 ✓），全部劣化形态**逐字节回退** ✓
6. **并发写者串行化**：同一文件多泳道时按时间点交棒（`16:23:41` 起 A 停写、B 接手 ✓），并在报告中标明 ✓

#### G. 缺口与后续

monitor **68** / guild **67** / dbmw **55** —— 经本波证明**源码级可调面仍未穷尽** ✓
**后续方向（按优先级）**：
1. **机理 D/E 全库复扫**（两者均为**语义/结构级** ✓ 且症状可指令级识别 ✓）
2. **机理 A/B/C 全库复扫**（含非 NEAR 长件 ✓ 本波扩散波正在执行 ✓）
3. **跨服务同源第三例攻坚**（`SendPacket` 三服务同形 ⇒ 一次 3 件 ✓）
4. 对已登记「不可调」的件，在**新机理出现时重估**（本波即有此例：多条旧结论被新机理推翻 ✓）

## 6. 复现

```bash
# 13 服务（不含 game）
python3 source/toolchain/gen_report_manifest.py \
  --services auction bridge channel community coserver dbmw \
             guild manager monitor point relay statics stun

# game（权威口径）
python3 source/toolchain/compare_game_full.py
```
