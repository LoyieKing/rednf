# DNF 重建工程 CMake 构建说明（2026-08-10）

13 个服务全部改为 CMake 构建（取代 build-*.sh 的裸脚本）。每个服务一个自包含
`CMakeLists.txt`（`project(<svc> CXX)` + 工具链/帮助模块 + `dnf_service(...)`），
产物 `build/<svc>/df_<svc>_r`。

## 一键构建
```
bash source/toolchain/cmake_build_all.sh            # 全部 13 个
bash source/toolchain/cmake_build_all.sh stun guild # 指定服务
```

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
