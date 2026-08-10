# ============================================================
# DNF 重建工程 CMake 入口说明（2026-08-10）
# ------------------------------------------------------------
# 13 个服务各自有自包含 CMakeLists（source/cmake/<svc>/CMakeLists.txt）。
# 因各服务编译器变体不同（c5 / c6 / c6446r / 系统编译器），**必须按服务单独
# 配置**（一个 CMake 配置只能有一个编译器）。一键构建见:
#   source/toolchain/cmake_build_all.sh
# 单服务示例（stun，64 位 + c5 变体）:
#   cmake -S source/cmake/stun -B build/stun \
#         -DCMAKE_TOOLCHAIN_FILE=$PWD/source/toolchain/cmake/dnf_toolchain_44.cmake \
#         -DDF_CC_VARIANT=c5
#   cmake --build build/stun -j
# community 用系统编译器独立构建:
#   cmake -S source/cmake/community -B build/community
#   cmake --build build/community -j
# ============================================================
cmake_minimum_required(VERSION 3.10)
project(DNFServerReconstruction)
message(STATUS "DNF 重建工程：请使用 source/toolchain/cmake_build_all.sh 或按服务单独配置（见 source/cmake/README.md）")
