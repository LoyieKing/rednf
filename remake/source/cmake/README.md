# DNF 重建工程 CMake 构建说明（2026-08-10）

13 个服务全部改为 CMake 构建（取代 build-*.sh 的裸脚本）。每个服务一个自包含
`CMakeLists.txt`（`project(<svc> CXX)` + 工具链/帮助模块 + `dnf_service(...)`），
产物 `build/<svc>/df_<svc>_r`。

> 两条构建路线（现代路线默认 / 验证路线）与兼容层、GeoIP 的完整说明见
> **`docs/df_modern_route.md`**；本文件只写 CMake 入口与开关细节。

## 一键构建
```
cmake -S source -B build            # 统一入口（source/CMakeLists.txt）；默认现代路线（OFF）
cmake --build build -j              # 三方静态库 + 14 个服务
cmake --build build --target game   # 单个服务

# 逐字节 identical 验证路线（GCC 4.x 工具链）需显式打开：
cmake -S source -B build -Ddecompile_verify=ON
```

## decompile_verify：构建路线开关（默认 OFF）

| 取值 | 编译路线 | 产物 |
|---|---|---|
| `OFF`（默认） | 宿主最新 LTS GCC（本机 g++ 13.3），`-std=gnu++14 -fpermissive`，不加载 4.x 工具链 | 语义等价、可直接运行；**不保证**逐字节一致 |
| `ON` | GCC 4.x 工具链（c5/c6/c6446r/c6444r + 逐 TU 分发），`-nostdinc` + 显式历史头集 | 与 ORIG **逐字节 identical**（14/14 服务的验收口径） |

> 切换取值需用**干净构建目录**（或在同一目录显式改 `-Ddecompile_verify=`：顶层会按
> `flags.make` 实况检测编译器错配并自动重置对应服务的子构建目录）。

两种模式都会向源码暴露编译宏 `decompile_verify`（1/0），可写 `#if decompile_verify`。
`OFF` 时由 `dnf_helpers.cmake` 以 `-include` 注入兼容层
`source/toolchain/compat/decompile_verify_off.h`，修正新版 GCC 不再支持的语法/ABI：

- `register` 存储类（C++17 移除）→ 展开为空；
- 旧标准库的传递性包含（`<cstdio>`/`<map>`/`<pthread.h>` 等）→ 统一补齐；
- C++17 移除的 `unary_function`/`binary_function` 等旧函数对象基类 → 等价别名；
- 4.x 时代的静态 `libstdc++.a`/`libgcc*.a` → 现代路线自动剔除，改用宿主运行时。
- **`-D_GLIBCXX_USE_CXX11_ABI=0`**：恢复旧 libstdc++ 对象布局（32 位实测
  string=4 / list=8 / map=24 / vector=12，与 c6root 4.4.7 逐项相同）——
  否则新 ABI 的 `std::string=24` 会把所有按 ORIG 偏移定义的类布局顶掉；
- **`dnf-compat-layer` 语义内联**（`source/toolchain/compat/`）：上游
  https://github.com/llnut/dnf-compat-layer 是运行期 LD_PRELOAD 修老二进制，
  本工程在源码层等价实现，OFF 产物不依赖任何 preload：
  · `dnf_shm_compat.h`：shm 名/`/dev/shm` 多级路径扁平化（`/dev/shm/a/b → /dev/shm/a_b`）；
  · `decompile_verify_off.h` 宏覆盖 `shm_open`/`shm_unlink`/`mkdir`/`mkdirat`
    （其余路径家族以 `dnf_compat_*` 函数提供，避免误伤 `m_sock.open()` 这类成员调用）；
  · `dnf_compat_stat.cpp`：静态定义 `__xstat/__fxstat/__lxstat(+64/+at)` 族
    （老静态库引用、部分新发行版 libc 已移除的旧入口），按现代 `stat64` 转发；
- 逐服务适配点一律用 `#if decompile_verify` 收口（`ON` 分支文本不变 ⇒ 产物零变化）：
  4.1/4.4 头里 `std::strstr/strrchr` 的 out-of-line 重载、显式模板实参 `make_pair`
  的左值绑定、packed 成员绑非 const 引用、`<cstdatomic>`/`<bits/atomic_0.h>`、
  含 `std::map` 成员的 ORIG 布局 `static_assert` 等。

现状（2026-09-17）：`OFF` 模式 **14/14 服务** 全部用宿主 GCC 编译链接通过
（`build-modern/`），社区服/游戏服冒烟可启动到配置加载；`ON` 路线重新全量构建后
**14 个产物 md5 与改动前逐一相同**（零回归）。

## GeoIP：两条链路都用真实实现（2026-09-17）

`df_game_r` 的台湾地区限制（`RestrictGeolocation`）需要真实 GeoIP；原先是
`toolchains/geoip_stub/libGeoIP.so.1` 链接桩（3 个符号返回 0/NULL），地理限制
形同虚设。现改为 **本仓自建的真实 Country edition 实现**：

| 项 | 内容 |
|---|---|
| 源码 | `source/toolchain/geoip/geoip_country.c`（上游 dnf-compat-layer 的 `geoip-compat` Rust 版 C 等价移植，5 个导出符号逐一对齐） |
| 构建 | `source/cmake/third_party/CMakeLists.txt` 的 `geoip` 目标 → `build/geoip/libGeoIP.so.1`；`ON` 路线用 c6root 4.4.7（只需 GLIBC ≤2.12，CentOS 7 可跑），`OFF` 路线用宿主 gcc `-m32` |
| 链接 | game 侧改为给产物全路径；`SONAME=libGeoIP.so.1` ⇒ `DT_NEEDED` 记名与顺序、`ON` 产物逐字节 identical 均不变 |
| 数据 | 默认路径同上游：`/usr/local/share/GeoIP/GeoIP.dat`、`/usr/share/GeoIP/GeoIP.dat`、`/var/lib/GeoIP/GeoIP.dat`、`/data/GeoIP/GeoIP.dat`（容器内已存在 3.4MB/1.1MB 的 Country 库） |
| 兼容 | 尾部 1KB 内**从后往前**找 `FF FF FF <edition>`：edition=1 ⇒ Country；无 marker（老 Country 库）⇒ 按 MaxMind 1.4.8 默认分支当 Country；edition≠1（City 等）⇒ 明确拒绝 |

验证（2026-09-17，与真实实现逐 IP A/B，**零差异**）：

* 宿主 + Debian 版 `GeoIP.dat`：本实现（GCC 13） vs 容器里的 Rust `geoip-compat` — 5020 IP + 15 条边界输入全等；
* CentOS 7 容器 + 其自带 `GeoIP.dat`：本实现（c6root 4.4.7，CMake 产物） vs MaxMind `libGeoIP.so.1.4.8` — 5020 IP 全等（含 `GeoIP_new(0)` 默认路径）。 

## 单服务构建（编译器变体见下）
```
cmake -S source/cmake/<svc> -B build/<svc> \
      -DCMAKE_TOOLCHAIN_FILE=$PWD/source/toolchain/cmake/dnf_toolchain_44.cmake \
      -DDF_CC_VARIANT=<variant>
cmake --build build/<svc> -j
```

## 编译器变体
| 变体 | 编译器 | 服务 |
|---|---|---|
| c5 | c5root g++ 4.1.2-55（64 位） | stun |
| c6446r | c6root 4.4.7 驱动 + 4.4.6 cc1plus | channel / bridge / auction / point / relay |
| c6 | c6root g++ 4.4.7 | manager / dbmw / monitor / guild / statics / coserver |
| c6446r（自举） | c6root 4.4.7 驱动 + 4.4.6 cc1plus | community（独立库，CMakeLists 自己 include 工具链，裸命令即可：`cmake -S source/cmake/community -B build/community`） |

## 依赖
- GCC 4.x 工具链根：`$DNF_TC_ROOT`（community 的 CMakeLists 缺省自举为仓库内
  `dnf_decompile/toolchains/`；其余服务需显式传或经 `cmake_build_all.sh` 设置），
  从零搭建见 `docs/toolchain_bootstrap.md`。
- 共享模块：`source/toolchain/cmake/dnf_toolchain_44.cmake`、`dnf_helpers.cmake`、
  `dnf_c{5,6,6446r}_gxx.sh`（编译器包装）。

## 已验证
13 个服务的 CMake 产物与 build-*.sh 产物水位一致（MISSING=0）；statics/coserver/relay/
stun/channel/bridge/auction/point 逐字节一致，manager/dbmw/monitor/guild 非 tinyxml 对象
逐字节一致（tinyxml 用 4.4.7 统一编译，属豁免类），community 反汇编一致（仅 debug 路径
字符串差异）。

2026-09-14 复核：community 裸命令自举重建通过，产物 32 位（与 ORIG 同位宽），
`df_community_r` 的 335 个 community 自身类函数代码尺寸与 ORIG **逐个一致**；
符号总数 7751 vs ORIG 9430 的缺口集中在静态 libstdc++/libgcc_eh 版本差异、
宿主新链接器 CRT 脚手架（`__libc_csu_init` 等），以及 ORIG 里由约 18 个 TU 各自
实例化的头部级局部静态对象（boost placeholders、`nexon_packet::MaxBodySize` 等）。
