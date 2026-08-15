# DNF 重建 GCC 4.x 工具链文件（2026-08-10）
# 用法: cmake -S source -B build/<svc> -DDF_SERVICE=<svc>
#       -DCMAKE_TOOLCHAIN_FILE=source/toolchain/cmake/dnf_toolchain_44.cmake
# 工具链根: $DNF_TC_ROOT（bootstrap_toolchain.sh 生成；缺省回退 /tmp）
#
# 编译器变体（DF_CC_VARIANT）:
#   c6      -> c6root g++ 4.4.7（默认，多数服务）
#   c5      -> c5root g++ 4.1.2-55（relay C5 组 / stun）
#   c6446r  -> c6root 驱动 + 4.4.6 cc1plus（channel/bridge/auction/point）
#   c6444r  -> c6root 驱动 + Red Hat 4.4.4-13 cc1plus（monitor ORIG .comment 主体）
if(NOT DEFINED DF_TC_ROOT)
    if(DEFINED ENV{DNF_TC_ROOT} AND NOT "$ENV{DNF_TC_ROOT}" STREQUAL "")
        set(DF_TC_ROOT "$ENV{DNF_TC_ROOT}")
    else()
        set(DF_TC_ROOT "/tmp")
    endif()
endif()
if(NOT DEFINED DF_CC_VARIANT)
    set(DF_CC_VARIANT "c6")
endif()

set(DF_C6ROOT  "${DF_TC_ROOT}/c6root")
set(DF_C5ROOT  "${DF_TC_ROOT}/c5root")
set(DF_C5R52T  "${DF_TC_ROOT}/c5r52tool")
set(DF_C5R52I  "${DF_TC_ROOT}/c5r52i386")
set(DF_LSD44   "${DF_TC_ROOT}/lsd44/v4.4.6")
set(DF_LSD444  "${DF_TC_ROOT}/lsd44/v4.4.4")
set(DF_ZLIB32  "${DF_TC_ROOT}/zlib32")
set(DF_CC1P446 "${DF_TC_ROOT}/cc1plus446bin")

# 编译器一律走包装脚本（dnf_c<variant>_gxx.sh）：脚本内部导出 LD_LIBRARY_PATH 供
# cc1plus 使用（libmpfr.so.1 等只在 c6root 内），同时不污染宿主 cmake。
set(_DNF_TC_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}")
if(DF_CC_VARIANT STREQUAL "c5")
    set(CMAKE_C_COMPILER   "${_DNF_TC_CMAKE_DIR}/dnf_c5_gxx.sh")
    set(CMAKE_CXX_COMPILER "${_DNF_TC_CMAKE_DIR}/dnf_c5_gxx.sh")
elseif(DF_CC_VARIANT STREQUAL "c6446r")
    set(CMAKE_C_COMPILER   "${_DNF_TC_CMAKE_DIR}/dnf_c6446r_gxx.sh")
    set(CMAKE_CXX_COMPILER "${_DNF_TC_CMAKE_DIR}/dnf_c6446r_gxx.sh")
elseif(DF_CC_VARIANT STREQUAL "c6444r")
    set(CMAKE_C_COMPILER   "${_DNF_TC_CMAKE_DIR}/dnf_c6444r_gxx.sh")
    set(CMAKE_CXX_COMPILER "${_DNF_TC_CMAKE_DIR}/dnf_c6444r_gxx.sh")
else()
    set(CMAKE_C_COMPILER   "${_DNF_TC_CMAKE_DIR}/dnf_c6_gxx.sh")
    set(CMAKE_CXX_COMPILER "${_DNF_TC_CMAKE_DIR}/dnf_c6_gxx.sh")
endif()
unset(_DNF_TC_CMAKE_DIR)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 注意：不要在这里 set(ENV{LD_LIBRARY_PATH} ...) —— 现代宿主上 /usr/bin/cmake 自身
# 依赖新版 libstdc++（GLIBCXX_3.4.29+），指向 c6root 4.4.7 的旧 libstdc++ 会打崩
# cmake。LD_LIBRARY_PATH 由上面的编译器包装脚本在子进程内导出（cc1plus 需要
# libmpfr.so.1 等 c6root 库）。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -O0 -fno-enforce-eh-specs")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32 -O0")
