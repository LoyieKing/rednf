cmake_minimum_required(VERSION 3.16)
project(secsvr_rebuild LANGUAGES C CXX)

# secsvr 三件套：gunnersvr / zergsvr / secagent
# 源码树保留在 source/<binary>/，因为桩源码使用 "src/..." 形式的原始 include 路径，
# 且 tss_sdk 的 reuse 记录直接引用 source/secagent 与 source/gunnersvr/zergsvr。
# 本工程只负责构建/链接，不改变各二进制自己的 include 镜像。

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(SECSVR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
include("${SECSVR_ROOT}/cmake/SecsvrCommon.cmake")

secsvr_add_tencrypt()

secsvr_add_service(gunnersvr "${SECSVR_ROOT}/source/gunnersvr")
secsvr_add_service(zergsvr   "${SECSVR_ROOT}/source/zergsvr")
secsvr_add_service(secagent  "${SECSVR_ROOT}/source/secagent")

# 构建产物放到 build/bin，便于与原版 ELF 对照和替换。
set_target_properties(gunnersvr zergsvr secagent PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
