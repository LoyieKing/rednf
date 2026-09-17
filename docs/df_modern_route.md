# decompile_verify=OFF 现代路线：构建、兼容层与 GeoIP（2026-09-17）

> 适用对象：想用**宿主最新 LTS GCC**（本机 g++ 13.3）构建全部 14 个服务、
> 要一份能直接跑的产物，或要排查 OFF 路线构建/运行问题的人。
> 逐字节 identical 的验证路线见 `docs/PROGRESS_STATUS.md` 与
> `source/toolchain/game_verify_guide.md`（那条路线用 `-Ddecompile_verify=ON`）。

---

## 1. 两条路线是什么、怎么选

| 路线 | 开关 | 编译器 | 产物 | 用途 |
|---|---|---|---|---|
| 现代路线 | `-Ddecompile_verify=OFF`（**默认**） | 宿主 g++ 13.3（`-m32`） | `build-modern/<svc>/df_<svc>_r` | 直接跑、部署、日常开发 |
| 验证路线 | `-Ddecompile_verify=ON` | GCC 4.x 工具链（c5/c6/c6446r/c6444r + 逐 TU 分发），`-nostdinc` + 显式历史头集 | `build/<svc>/df_<svc>_r` | 与 ORIG 逐字节 identical 的核账口径 |

两者共用同一份源码：所有差异都用 `#if decompile_verify` 收口，**ON 分支文本保持原样**，
因此改源码时两条路线可以同时维护（本次改造后 ON 侧 14 个产物 md5 零变化，见 §7）。

### 常用命令

```bash
# 现代路线（默认；可省略 -Ddecompile_verify=OFF）
cmake -S source -B build-modern
cmake --build build-modern -j$(nproc)

# 只构建某几个服务
cmake -S source -B build-modern -DDNF_SERVICES="game;guild;monitor"
cmake --build build-modern -j$(nproc)

# 单目标
cmake --build build-modern --target game
cmake --build build-modern --target third_party     # 三方库 + libGeoIP.so.1

# 验证路线（逐字节 identical 口径）
cmake -S source -B build -Ddecompile_verify=ON
cmake --build build -j$(nproc)
```

**切换取值用干净目录**。若要在同一目录里切，顶层会按 `flags.make` 实况检测编译器错配
并自动重置对应服务的子构建目录（ON→OFF、OFF→ON 两个方向都已实测），但干净目录更省事。

---

## 2. 现代路线做了什么（构建侧）

`dnf_helpers.cmake` 的 `dnf_service()` 在 `decompile_verify=OFF` 时走独立分支：

| 项 | 取值 / 行为 | 为什么 |
|---|---|---|
| 语言 | `-std=gnu++14 -O0 -fpermissive -w` | 4.x 源码里有大量现代编译器默认报错的老写法 |
| ABI | **`-D_GLIBCXX_USE_CXX11_ABI=0`** | 恢复旧 libstdc++ 对象布局：32 位实测 `string=4 / list=8 / map=24 / vector=12`，与 c6root 4.4.7 逐项相同；不加则新 ABI 的 `std::string=24` 会顶掉所有按 ORIG 偏移定义的类布局 |
| 兼容头 | `-include source/toolchain/compat/decompile_verify_off.h` | 见 §3 |
| 宏 | `-Ddecompile_verify=0` | 源码里 `#if decompile_verify` 的判据（ON 侧为 `=1`） |
| 逐 TU 旗标 | 丢弃 `-std=/-O/-march=/-mtune=/-B` 形式 | 这些是 4.x 形态复现用的；宿主上要么无意义，要么直接报错（实测 `-march` 与 glibc 强化头的 `always_inline` 冲突：`target specific option mismatch`） |
| 库 | 过滤掉 4.x 的 `libstdc++` / `libgcc`（`.a` 与 `.so` 都算） | 现代路线用宿主运行时；混链旧归档会 `No rule to make target …` |
| 链接 | `-m32 -Wl,--no-as-needed <服务 LINK_FLAGS>` | ① 链接期也要 `-m32`，否则驱动按 64 位挑隐式 libgcc/libc，32 位归档里的 `__udivdi3`/`operator new` 解析不到；② 历史服务把动态库写在 `LINK_FLAGS`（排在目标文件之前），宿主 g++ 默认 `--as-needed` 会把"还没被引用"的库丢掉（实测 game 的 GeoIP 未定义引用） |
| 三方库 | `build/<svc>/…` 仍链 `mysqlclient/yassl/freetype`（4.x 预编译归档） | ABI=0 与本侧兼容；彻底现代化留作后续（见 §8） |

服务 CMakeLists 里所有 4.x 专用设置都按 `if(decompile_verify)` 收口，本次改到的有：
auction/bridge/channel/point 的 `-B cc1plus446bin`、channel 的 `channel_cc.sh` 编译规则
（含 `__FILE__` 复现）、bridge 的 `-march=i586 -mtune=i686`、relay 的 `-B` 与
POST_BUILD 4.x 重链、stun 的 64 位分支。

**工具链根**：`dnf_toolchain_44.cmake` 在 OFF 时不生效（提前 `return`），但服务
CMakeLists 仍会引用 `DF_ZLIB32` 之类的路径 ⇒ `dnf_helpers.cmake` 在 OFF 时按同一
约定补齐这些变量，避免出现 `/lib/libz.a` 这种空变量拼出来的路径。
ON 路线的工具链根现在也写进子配置缓存（`-DDF_TC_ROOT=…`），否则 make 触发的"裸重配"
会丢环境变量、把 `-isystem` 指到 `/tmp`（实测 `cstdio: No such file`）。

---

## 3. 兼容层之一：语法 / ABI（`decompile_verify_off.h`）

`source/toolchain/compat/decompile_verify_off.h` 由 `-include` 注入每个 TU：

1. `register` 存储类（C++17 移除）→ 展开为空；
2. 旧标准库的传递性包含（`<cstdio>`/`<map>`/`<pthread.h>` 等）→ 统一补齐；
3. C++17 移除的 `unary_function` / `binary_function` → 等价别名；
4. `_LARGEFILE64_SOURCE`：让 `<sys/stat.h>` 暴露 `stat64` 族（§4 的 `__xstat` 实现要用）；
5. §4 的 shm / 路径 / mkdir 宏与函数。

### 源码侧逐 TU 适配（全部 `#if decompile_verify` 收口）

| 症状（宿主 GCC） | 处置 | 涉及文件 |
|---|---|---|
| `'char* std::strstr(char*, const char*)' conflicts with a previous declaration` / 调用二义 | 4.1/4.4 头的 out-of-line 重载只在 ON 定义；OFF 用现代重载 | `ServerLab/ServerLib/basic_source/LinuxService.cpp`、`Game/Divide_C_ClassIP_EndIP.cpp`、`Game/Dispatchers.cpp`（`_ZSt6strstrPcPKc` asm 别名 → OFF 内联转发） |
| `cannot bind packed field … to 'int&'` | OFF 走"临时量中转"宏/局部量（ON 展开为原始单语句） | `DBMW/DBManager.cpp`（14 处）、`Statics/Statistics.cpp`、`Monitor/DNFPacketTranslater.cpp` |
| `cannot bind rvalue reference … to lvalue`（显式模板实参 `make_pair` 左值绑定） | OFF 直接构造 `std::pair` | `Monitor/DNFApplication.cpp`×2、`Game/CDisconnectDetecter/RegisterUser.inc`、`Game/STStackableScript/ImportStackableScript.inc`×2、`Game/CreatureScript/importCreatureScript.inc`×2、`Game/InitScriptProducers_6.cpp`×2 |
| `<cstdatomic>` / `<bits/atomic_0.h>` 不存在（`std::__atomic0::atomic_flag`） | OFF 用 `<atomic>` + `std::atomic_flag` | `Game/OnlyOrigBatch7_Force.cpp`（+`forceCtorEmissions.inc`） |
| ORIG 布局 `static_assert` 失败（现代 `std::map`/`unordered_map` 内部布局与 4.4 不同） | 断言块只在 ON 生效 | `Game/CUser.h`（`_Charac_info` 21 条）、`Game/CItemGenerator.h`（`CItemUpgradeTable` 1 条） |

---

## 4. 兼容层之二：dnf-compat-layer 语义内联（shm / 路径 / mkdir / stat）

上游 https://github.com/llnut/dnf-compat-layer 用 LD_PRELOAD 在**运行期**修老二进制；
本工程把它做进**源码层**，OFF 产物不依赖任何 preload。

| 文件 | 内容 |
|---|---|
| `source/toolchain/compat/dnf_shm_compat.h` | 上游 `glibc-compat/src/lib.rs` 的内联版：shm 名扁平化（除首字符外 `/`→`_`）、`/dev/shm/a/b → /dev/shm/a_b`、`/dev/shm/…` 判定 |
| `source/toolchain/compat/decompile_verify_off.h`（§5 段） | 宏覆盖 `shm_open` / `shm_unlink` / `mkdir` / `mkdirat`（`/dev/shm` 下假成功、`EEXIST` 视为成功）；其余路径家族以 `dnf_compat_*` 函数提供 —— **宏不覆盖 `open`/`stat` 是有意的**：本仓有 `m_sock.open()` 这类同名成员调用，函数式宏会误伤 |
| `source/toolchain/compat/dnf_compat_stat.cpp` | 静态定义 `__xstat`/`__xstat64`/`__fxstat`/`__fxstat64`/`__lxstat`/`__lxstat64`/`__fxstatat`/`__fxstatat64`：老静态库（MySQL Connector/C、yaSSL 等按 4.x 头编译）引用这些旧入口，部分新发行版 libc 已移除 ⇒ 按现代 `stat64` 转发 + `/dev/shm` 扁平化 |

该 TU 由 `dnf_helpers.cmake` 在 OFF 时自动加进每个服务目标（`stun` 是自带分支，
单独加了同一文件），因此 14 个 OFF 产物里这些符号都是**已定义（T）**，运行期不查 libc。

---

## 5. GeoIP：两条链路都是真实实现

`df_game_r` 的台湾地区限制（`Game/RestrictGeolocation*`）走 `GeoIP_new` /
`GeoIP_country_code_by_addr` / `GeoIP_country_code_by_name` / `GeoIP_open` /
`GeoIP_delete`。原先链接的是 `toolchains/geoip_stub/libGeoIP.so.1`（三个符号恒返回
0/NULL 的桩）⇒ 地理限制形同虚设。现在两条路线都链**本仓自建的真实实现**：

| 项 | 内容 |
|---|---|
| 源码 | `source/toolchain/geoip/geoip_country.c`（上游 `geoip-compat` Rust 版的 C 等价移植，导出符号逐一对齐） |
| 算法 | MaxMind Legacy **Country edition**：3 字节记录 trie 走位（32 位 IPv4 高位→低位），值 ≥ `GEOIP_COUNTRY_BEGIN(0xFFFF00)` 即国家编号 → 256 项国家码表；`"--"`/越界 → NULL |
| 版本判定 | 尾部 1KB **从后往前**找 `FF FF FF <edition>`：`edition=1` → Country；**无 marker**（老 Country 库，如 CentOS7 容器里那份 1.1MB 的 `GeoIP.dat`）→ 按 MaxMind 1.4.8 的默认分支当 Country；`edition≠1`（City 等）→ 明确拒绝 |
| 输入 | 只收 IPv4 字面量；拒前导 0、拒 4 段以外、拒主机名（与上游 Rust 解析器一致） |
| 数据路径 | `/usr/local/share/GeoIP/GeoIP.dat`、`/usr/share/GeoIP/GeoIP.dat`、`/var/lib/GeoIP/GeoIP.dat`、`/data/GeoIP/GeoIP.dat`（同上游；容器内已有真实 Country 库） |
| 构建 | `source/cmake/third_party/CMakeLists.txt` 的 `geoip` 目标 → **ON 用 c6root 4.4.7**（产物只需 GLIBC ≤2.12，CentOS 7 可跑）→ `build/geoip/libGeoIP.so.1`；**OFF 用宿主 gcc -m32** → `build-modern/geoip/libGeoIP.so.1` |
| 链接 | game 侧改为给 `.so` 全路径；`SONAME=libGeoIP.so.1` ⇒ `DT_NEEDED` 记名与顺序不变、无 RUNPATH、ON 产物逐字节 identical 不受影响 |

部署提示：容器内本来就有 MaxMind 1.4.8 + 数据库，行为不变；若目标机没有 libGeoIP，
把对应 `build*/geoip/libGeoIP.so.1` 放进 `LD_LIBRARY_PATH` 覆盖即可（同 SONAME 可直接替换）。
当前只实现 Country edition；City/ISP 等其它 edition 会被明确拒绝。

---

## 6. 验证记录（2026-09-17）

### 6.1 构建

* OFF 路线：**14/14 服务**编译链接通过（`build-modern/`，含 `libGeoIP.so.1`）；
  干净目录 + 默认开关的端到端构建也实测通过（community + geoip）。
* ON 路线：全量重编后 **14 个产物 md5 与本次改造前逐一相同**（零回归），
  `df_game_r` 的 `DT_NEEDED` 顺序仍为 `libpthread → librt → libnxencryption →
  libGeoIP.so.1 → libdl → libm → libc`。

### 6.2 GeoIP 逐 IP 对拍（零差异）

| 场景 | 对照组 | 结果 |
|---|---|---|
| 宿主 + Debian 版 `GeoIP.dat`（3.4MB，尾部带 marker） | 容器里部署的 Rust `geoip-compat` | 5020 个 IP（5000 随机 + 20 已知）+ 15 条边界输入**全等** |
| CentOS7 容器 + 其自带 `GeoIP.dat`（1.1MB，尾部无 marker）+ `GeoIP_new(0)` 默认路径 | MaxMind `libGeoIP.so.1.4.8` | 5020 个 IP**全等**（含 `8.8.8.8→US`、`168.95.1.1/61.220.1.1→TW`、`114.114.114.114/223.5.5.5→CN`） |

### 6.3 冒烟

* OFF：`community` 打印 banner 正常退出；`df_game_r` 在宿主上跑到 `publickey.pem` 缺失
  （环境缺配置，非构建问题）；`ldd` 确认 `libGeoIP.so.1` 解析到本仓产物。

---

## 7. identical 水位（2026-09-17 复测，验证路线口径）

ON 路线重编后按既有口径复测（明细与历史见 `docs/df_binaries_identical.md`）：

| 服务 | 口径 | 非 identical |
|---|---|---|
| game | `compare_game_full.py` | **348**（共同 36478：identical 36090 = strict 26853 + ae 8134 + sem 1103，near 45 + diff 303，stl 40；仅ORIG 5、仅NEW 129） |
| dbmw / guild / monitor / statics | `compare_svc_full.py` | 36 / 33 / 50 / 3 |
| auction / point | `compare_svc_full.py` | 0 / 0（六服务权威合计 122） |
| 13 服务清单口径 | `gen_report_manifest.py` | dbmw 41、guild 36、monitor 57、statics 3、bridge 1、stun 3，其余 0（合计 141） |

---

## 8. 已知限制 / 未做

1. **三方静态库仍是 4.x 产物**（`libmysqlclient.a` / `libyassl.a` / `libfreetype.a`
   由 c6root gcc 4.4.7 编译）。ABI=0 下与本侧兼容、当前可用；若要彻底现代化需另起一轮
   （注意 yaSSL 的 `std::string` 也必须用 ABI=0 重编）。
2. **OFF 产物的 glibc 版本跟随宿主**（Ubuntu 24.04 ⇒ GLIBC 2.39+ 依赖）⇒
   **不能**直接放进 CentOS 7 容器（`GLIBC_2.34 not found`）；要进老环境用 ON 产物。
3. GeoIP 只支持 Country edition；没有实现 `GEOIP_CHECK_CACHE` / `INDEX_CACHE` /
   `MMAP_CACHE` 等 flags（与上游 Rust 版一致，参数仅为 API 兼容）。
4. `__xstat` 族是**强定义**进可执行文件（优先于 libc）。若将来接入同样定义这些符号的
   库，需要改成 weak 或加开关。
5. 兼容层宏不覆盖 `open`/`openat`/`stat` 等名字（同名成员调用风险），本仓当前也没有
   `/dev/shm` 使用点；如将来出现，用 `dnf_compat_open` 之类的函数入口显式调用。

---

## 9. 本次新增/修改文件清单

* 新增：`source/toolchain/compat/dnf_shm_compat.h`、`source/toolchain/compat/dnf_compat_stat.cpp`、
  `source/toolchain/geoip/geoip_country.c`、本文件。
* 构建：`source/CMakeLists.txt`（默认 OFF、OFF 侧子构建重置、`DF_TC_ROOT` 缓存）、
  `source/toolchain/cmake/dnf_helpers.cmake`、`dnf_toolchain_44.cmake`、
  `source/cmake/{game,relay,channel,bridge,auction,point,stun,third_party}/CMakeLists.txt`
  （`make_directory` 修复：mysqlclient/yassl 对象目录在全新构建目录下不再竞态失败）。
* 源码适配（`#if decompile_verify`）：`LinuxService.cpp`、`Divide_C_ClassIP_EndIP.cpp`、
  `Dispatchers.cpp`、`DBManager.cpp`、`Statistics.cpp`、`Monitor/DNFApplication.cpp`、
  `Monitor/DNFPacketTranslater.cpp`、`Game/CUser.h`、`Game/CItemGenerator.h`、
  `OnlyOrigBatch7_Force.cpp`、`OnlyOrigBatch7_Force/forceCtorEmissions.inc`、
  `CDisconnectDetecter/RegisterUser.inc`、`STStackableScript/ImportStackableScript.inc`、
  `CreatureScript/importCreatureScript.inc`、`InitScriptProducers_6.cpp`。
* 文档：`source/cmake/README.md`、`dnf_decompile/README.md`、`docs/PROGRESS_STATUS.md`、
  `docs/df_binaries_identical.md`、`source/toolchain/class_dump.py`（注释）。
