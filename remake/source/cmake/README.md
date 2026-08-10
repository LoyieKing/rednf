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
| 系统 | 宿主编译器 | community（独立构建，`cmake -S source/cmake/community -B build/community`） |

## 依赖
- GCC 4.x 工具链根：`$DNF_TC_ROOT`（缺省 /tmp），从零搭建见
  `docs/toolchain_bootstrap.md`（`bash source/toolchain/bootstrap_toolchain.sh`）。
- 共享模块：`source/toolchain/cmake/dnf_toolchain_44.cmake`、`dnf_helpers.cmake`、
  `dnf_c{5,6,6446r}_gxx.sh`（编译器包装）。

## 已验证
13 个服务的 CMake 产物与 build-*.sh 产物水位一致（MISSING=0）；statics/coserver/relay/
stun/channel/bridge/auction/point 逐字节一致，manager/dbmw/monitor/guild 非 tinyxml 对象
逐字节一致（tinyxml 用 4.4.7 统一编译，属豁免类），community 反汇编一致（仅 debug 路径
字符串差异）。
