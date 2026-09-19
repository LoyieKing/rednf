# ============================================================================
# DNF 重建工程 —— **唯一构建入口**（2026-09-17 起）
# ----------------------------------------------------------------------------
# 用法（等价于旧的 .buildall.sh，且不再需要任何 shell 构建脚本）：
#
#   cmake -S dnf_remake/source -B dnf_remake/build
#   cmake --build dnf_remake/build -j
#
#   只构建部分服务：
#   cmake -S source -B build -DDNF_SERVICES="game;manager"
#
#   构建路线（remake_verify，默认 OFF）：
#     OFF（默认）—— 宿主最新 LTS GCC，语义等价、可直接运行（见下方 §开关）
#     ON          —— GCC 4.x 工具链，逐字节 identical 验证口径
#   cmake -S source -B build -Dremake_verify=ON
#
#   单独构建某个服务/三方库：
#   cmake --build build --target game
#   cmake --build build --target third_party
#
# 产物：
#   build/<svc>/df_<svc>_r       —— 14 个服务二进制
#   build/geoip/libGeoIP.so.1    —— 真实 GeoIP Country edition 实现
#   build/manager/mysqlclient/libmysqlclient.a
#   build/manager/yassl/libyassl.a
#   toolchains/freetype32/lib/libfreetype.a
#
# 为什么是"树"而不是"一个 project()"：
#   各服务的 ORIG 编译变体不同（c5 / c6446r / c6444r / c6 / 系统编译器），
#   而一次 CMake 配置只能绑定一个编译器。因此本入口用 ExternalProject 驱动
#   每个服务各自的 source/cmake/<svc>/CMakeLists.txt 子配置，子配置仍按原有
#   CMAKE_TOOLCHAIN_FILE + DF_CC_VARIANT 选择编译器，产物与逐字节对齐水位保持不变。
#   三方静态库（原 build-*.sh）已在 source/cmake/third_party/ 中改为纯 CMake 目标。
# ============================================================================
cmake_minimum_required(VERSION 3.10)
project(DNFServerReconstruction NONE)

include(ExternalProject)
include(ProcessorCount)

# 路径规范化：服务与三方库的编译命令行里不得出现 ".."（三方源码/工具链头文件的
# __FILE__ 会原样写入目标文件；原 .buildall.sh 用的是 $(pwd) 干净绝对路径）
get_filename_component(DNF_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
get_filename_component(DNF_ROOT       ${CMAKE_CURRENT_LIST_DIR}/.. ABSOLUTE)
get_filename_component(DNF_BUILD      ${CMAKE_BINARY_DIR} ABSOLUTE)
set(DNF_CMAKE_DIR      ${DNF_SOURCE_DIR}/cmake)
set(DNF_TOOLCHAIN_DIR  ${DNF_SOURCE_DIR}/toolchain/cmake)
set(DNF_TOOLCHAIN_FILE ${DNF_TOOLCHAIN_DIR}/dnf_toolchain_44.cmake)

set(DNF_TOOLCHAINS ${DNF_ROOT}/toolchains CACHE PATH "GCC 4.x 工具链根（c5root/c6root/cc1plus* 等）")
get_filename_component(DNF_TOOLCHAINS ${DNF_TOOLCHAINS} ABSOLUTE)
if(NOT EXISTS ${DNF_TOOLCHAINS}/c6root)
    message(FATAL_ERROR "未找到工具链根 ${DNF_TOOLCHAINS}（-DDNF_TOOLCHAINS=<path> 指定）")
endif()

set(DNF_JOBS "" CACHE STRING "并行度（空 = 自动探测 CPU 数）")
if(NOT DNF_JOBS)
    ProcessorCount(DNF_JOBS)
endif()
if(DNF_JOBS LESS 1)
    set(DNF_JOBS 1)
endif()

# 三方静态库：纯 CMake 目标（mysqlclient / yassl / freetype）
add_subdirectory(${DNF_CMAKE_DIR}/third_party ${DNF_BUILD}/third_party)

# ---------------------------------------------------------------------------
# 服务表：<服务名>|<配置模式>|<变体>
#   std   : 通用工具链 dnf_toolchain_44.cmake + DF_CC_VARIANT=<变体>
#   game  : 专用工具链 dnf_toolchain_game.cmake（逐 TU 编译器分发）
#   guild : 专用工具链 dnf_toolchain_guild.cmake（默认 c6 + 4 个 TU 走 c6444r）
#   self  : 该服务 CMakeLists 自行 include 工具链（community/relay/coserver/statics）
# 变体口径以「与 ORIG 的 identical 水位」实测选定（compare_{svc}_full.py）：
#   statics 用 c6446r（identical 1148）显著优于 c6（1091）；
#   dbmw 用 c6444r（1388）可启用其 DNFMySql.cpp 的 C5 逐 TU 分流；
#   game/guild 必须用专用工具链（game 36090 vs 通用 35395；guild 1897 vs 1780）。
# ---------------------------------------------------------------------------
set(DNF_ALL_SERVICES
    "stun|std|c5"
    "relay|self|c6446r"
    "channel|std|c6446r"
    "bridge|std|c6446r"
    "auction|std|c6446r"
    "point|std|c6446r"
    "coserver|self|c6446r"
    "dbmw|std|c6444r"
    "game|game|"
    "guild|guild|"
    "manager|std|c6"
    "monitor|std|c6"
    "statics|std|c6446r"
    "community|self|c6446r")

set(DNF_SERVICES "" CACHE STRING "为空 = 构建全部；否则分号分隔的服务名列表")

# ---------------------------------------------------------------------------
# remake_verify：全局编译开关（默认 OFF）
#   ON  —— 逐字节 identical 路线：GCC 4.x 工具链 + 逐 TU 编译器分发（现状行为）
#   OFF —— 现代路线：宿主最新 LTS GCC 编译，由兼容层
#          source/toolchain/compat/remake_verify_off.h 修正新版 GCC 不再支持
#          的语法/ABI（register、旧标准库可见性、旧函数对象基类等），语义与 ON 等价。
# 两种模式的差异只体现在编译/链接方式；源码里可用 `#if remake_verify` 分支。
# ---------------------------------------------------------------------------
option(remake_verify "ON=逐字节 identical（GCC 4.x）；OFF=宿主最新 LTS GCC（语义等价）" OFF)

function(dnf_expected_cxx_wrapper _svc _mode _variant _out)
    if(_svc STREQUAL "game")
        set(_c "dnf_game_cxx.sh")
    elseif(_svc STREQUAL "guild")
        set(_c "dnf_guild_cxx.sh")
    elseif(_svc STREQUAL "monitor")
        set(_c "dnf_monitor_cxx.sh")
    elseif(_svc STREQUAL "stun")
        set(_c "")                 # stun 自带 CMAKE_CXX_COMPILE_OBJECT，flags.make 无该行
    elseif(_svc STREQUAL "statics" OR _svc STREQUAL "coserver" OR _svc STREQUAL "relay")
        set(_c "df-cxx-wrap.sh")   # 服务自建包装（内容 = exec dnf_<variant>_gxx.sh）
    elseif(_svc STREQUAL "community")
        set(_c "dnf_c6446r_gxx.sh")
    else()
        set(_c "dnf_${_variant}_gxx.sh")
    endif()
    set(${_out} "${_c}" PARENT_SCOPE)
endfunction()

function(dnf_add_service _svc _mode _variant)
    dnf_expected_cxx_wrapper("${_svc}" "${_mode}" "${_variant}" _expected_cc)
    set(_cc_file ${DNF_BUILD}/${_svc}/CMakeFiles/${_svc}.dir/flags.make)
    set(_cfg_args "")
    set(_cfg_pre "")
    # 每个服务都要显式收到开关值（CMake 里未定义变量在 if() 中恒为假，
    # 不显式传入会让 `if(remake_verify)` 走错分支）
    if(NOT remake_verify)
        # 现代路线：不传 4.x 工具链/包装，直接用宿主 g++（最新 LTS）
        set(_cfg_args -Dremake_verify=OFF)
        if(DNF_ASAN)
            list(APPEND _cfg_args -DDNF_ASAN=ON)
        endif()
    elseif(_mode STREQUAL "std")
        set(_cfg_args
            -Dremake_verify=ON
            -DCMAKE_TOOLCHAIN_FILE=${DNF_TOOLCHAIN_FILE}
            -DDF_CC_VARIANT=${_variant})
    elseif(_mode STREQUAL "game")
        set(_cfg_args
            -Dremake_verify=ON
            -DCMAKE_TOOLCHAIN_FILE=${DNF_TOOLCHAIN_DIR}/dnf_toolchain_game.cmake
            -DCMAKE_CXX_COMPILER=${DNF_TOOLCHAIN_DIR}/dnf_game_cxx.sh
            -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY)
    elseif(_mode STREQUAL "guild")
        set(_cfg_args
            -Dremake_verify=ON
            -DCMAKE_TOOLCHAIN_FILE=${DNF_TOOLCHAIN_DIR}/dnf_toolchain_guild.cmake
            -DCMAKE_CXX_COMPILER=${DNF_TOOLCHAIN_DIR}/dnf_guild_cxx.sh
            -DCMAKE_TRY_COMPILE_TARGET_TYPE=STATIC_LIBRARY)
    elseif(_mode STREQUAL "self")
        set(_cfg_args -Dremake_verify=ON)
    elseif(NOT _mode STREQUAL "self")
        message(FATAL_ERROR "未知配置模式 '${_mode}'（服务 ${_svc}）")
    endif()

    # 工具链根必须进**子配置缓存**：ExternalProject 只在首次 configure 时带
    # `cmake -E env DNF_TC_ROOT=…`；之后若 CMakeLists/工具链文件变动触发 make 的
    # 自动重配，env 不再存在 ⇒ dnf_toolchain_44.cmake 会回退 /tmp，flags 里的
    # -isystem 指向不存在的头（实测 "cstdio: No such file"）。缓存后两种路径都稳。
    if(remake_verify)
        list(APPEND _cfg_args -DDF_TC_ROOT=${DNF_TOOLCHAINS})
    endif()

    # 换编译器必须重置既有缓存：CMake 不会在已有构建目录里更换缓存的编译器，
    # 服务自己 set(CMAKE_CXX_COMPILER) 的写法也会被旧缓存压住（statics/coserver
    # 的 df-cxx-wrap.sh 就是这种情形），结果是「配置说 A、实际用 B」的静默错配。
    # 这里以 flags.make 记录的实际编译器为准，与预期包装不一致就删缓存重建。
    if(NOT remake_verify)
        # OFF 路线：同一构建目录若曾按 ON 配置过，子缓存里的编译器仍是 4.x 包装
        # （CMake 不会在既有缓存里换编译器）⇒ 按 flags.make 实况检测并重置，
        # 否则出现"配置说 OFF、实际仍用 c6root"的静默错配。
        set(_need_reset FALSE)
        if(EXISTS ${_cc_file})
            file(STRINGS ${_cc_file} _cc_line REGEX "^# compile CXX with")
            if(_cc_line)
                string(FIND "${_cc_line}" "/toolchain/cmake/dnf_" _w1)
                string(FIND "${_cc_line}" "df-cxx-wrap.sh" _w2)
                if(NOT _w1 EQUAL -1 OR NOT _w2 EQUAL -1)
                    set(_need_reset TRUE)
                    set(_why "OFF 路线但子构建仍指向 4.x 包装（${_cc_line}）")
                endif()
            endif()
        elseif(EXISTS ${DNF_BUILD}/${_svc}/CMakeCache.txt)
            set(_need_reset TRUE)   # 有缓存但没有规则文件：半配置状态
            set(_why "缺少 CMakeFiles 规则")
        endif()
    elseif(_expected_cc AND remake_verify)
        set(_need_reset FALSE)
        if(EXISTS ${_cc_file})
            file(STRINGS ${_cc_file} _cc_line REGEX "^# compile CXX with")
            if(_cc_line)
                string(FIND "${_cc_line}" "${_expected_cc}" _cc_pos)
                # 部分服务自建 df-cxx-wrap.sh（内容即 exec dnf_<variant>_gxx.sh），两者等价；
                # 反之亦然：预期是自建包装时，实际命中 dnf_<variant>_gxx.sh 同样接受。
                if(_cc_pos EQUAL -1)
                    string(FIND "${_cc_line}" "df-cxx-wrap.sh" _cc_pos)
                endif()
                if(_cc_pos EQUAL -1 AND _expected_cc STREQUAL "df-cxx-wrap.sh")
                    string(FIND "${_cc_line}" "dnf_${_variant}_gxx.sh" _cc_pos)
                endif()
                if(_cc_pos EQUAL -1)
                    set(_need_reset TRUE)
                    set(_why "${_cc_line} 与预期 ${_expected_cc} 不一致")
                endif()
            endif()
            # 工具链根校验：早期用错 DNF_TC_ROOT 配置过的目录会把 -isystem/tmp/... 之类
            # 的失效路径写进规则（编译期报莫名缺头文件/缺成员），必须重置。
            if(NOT _need_reset)
                file(READ ${_cc_file} _flags_all)
                string(FIND "${_flags_all}" "-isystem" _isys_pos)
                string(FIND "${_flags_all}" "/tmp/" _any_tmp_pos)
                if(NOT _isys_pos EQUAL -1 OR NOT _any_tmp_pos EQUAL -1)
                    string(FIND "${_flags_all}" "${DNF_TOOLCHAINS}" _tc_pos)
                    string(FIND "${_flags_all}" "/tmp/" _tmp_pos)
                    if(_tc_pos EQUAL -1 OR NOT _tmp_pos EQUAL -1)
                        set(_need_reset TRUE)
                        set(_why "规则里的工具链路径不是 ${DNF_TOOLCHAINS}（或残留 /tmp 路径）")
                    endif()
                endif()
            endif()
        elseif(EXISTS ${DNF_BUILD}/${_svc}/CMakeCache.txt)
            set(_need_reset TRUE)   # 有缓存但没有规则文件：半配置状态
            set(_why "缺少 CMakeFiles 规则")
        endif()
        if(_need_reset)
            message(STATUS "服务 ${_svc}：${_why}，重置 build/${_svc}")
            set(_cfg_pre COMMAND ${CMAKE_COMMAND} -E remove ${DNF_BUILD}/${_svc}/CMakeCache.txt
                         COMMAND ${CMAKE_COMMAND} -E remove_directory ${DNF_BUILD}/${_svc}/CMakeFiles
                         COMMAND)
        endif()
    endif()

    # 专用工具链服务额外校验缓存里的工具链根：早期用错 DNF_TC_ROOT 配置过的目录
    # （flags.make 会退化成 -isystem/usr/include）必须重置，否则编译直接失败。
    if(NOT _cfg_pre)
        set(_cache ${DNF_BUILD}/${_svc}/CMakeCache.txt)
        if(EXISTS ${_cache})
            file(STRINGS ${_cache} _cached_ts REGEX "^DF_TC_ROOT:")
            file(STRINGS ${_cache} _cached_cc REGEX "^CMAKE_CXX_COMPILER:")
            set(_stale "")
            if(_cached_ts AND NOT "${_cached_ts}" STREQUAL "DF_TC_ROOT:PATH=${DNF_TOOLCHAINS}")
                set(_stale "工具链根缓存为 '${_cached_ts}'")
            elseif(_cached_cc)
                string(REGEX REPLACE "^CMAKE_CXX_COMPILER:[^=]*=" "" _ccpath "${_cached_cc}")
                if(NOT EXISTS "${_ccpath}")
                    set(_stale "编译器缓存 ${_ccpath} 已不存在")
                endif()
            endif()
            if(_stale)
                message(STATUS "服务 ${_svc}：${_stale}，重置 build/${_svc}")
                set(_cfg_pre COMMAND ${CMAKE_COMMAND} -E remove ${_cache}
                             COMMAND ${CMAKE_COMMAND} -E remove_directory ${DNF_BUILD}/${_svc}/CMakeFiles
                             COMMAND)
            endif()
        endif()
    endif()

    ExternalProject_Add(${_svc}
        SOURCE_DIR      ${DNF_CMAKE_DIR}/${_svc}
        BINARY_DIR      ${DNF_BUILD}/${_svc}
        DOWNLOAD_COMMAND ""
        UPDATE_COMMAND   ""
        PATCH_COMMAND    ""
        CONFIGURE_COMMAND ${_cfg_pre} ${CMAKE_COMMAND} -E env DNF_TC_ROOT=${DNF_TOOLCHAINS}
                          ${CMAKE_COMMAND} -S <SOURCE_DIR> -B <BINARY_DIR> ${_cfg_args}
        BUILD_COMMAND     ${CMAKE_COMMAND} --build <BINARY_DIR> --parallel ${DNF_JOBS}
        INSTALL_COMMAND   ""
        BUILD_ALWAYS 1
        DEPENDS third_party
        USES_TERMINAL_CONFIGURE 1
        USES_TERMINAL_BUILD 1
        LOG_CONFIGURE 1
        LOG_BUILD 1
        COMMENT "构建服务 ${_svc}（模式 ${_mode}${_variant}）")
endfunction()

set(DNF_SERVICE_TARGETS "")
foreach(_entry IN LISTS DNF_ALL_SERVICES)
    string(REPLACE "|" ";" _fields "${_entry}")
    list(GET _fields 0 _svc)
    list(GET _fields 1 _mode)
    list(GET _fields 2 _variant)
    if(DNF_SERVICES AND NOT _svc IN_LIST DNF_SERVICES)
        continue()
    endif()
    dnf_add_service("${_svc}" "${_mode}" "${_variant}")
    list(APPEND DNF_SERVICE_TARGETS ${_svc})
endforeach()

if(NOT DNF_SERVICE_TARGETS)
    message(FATAL_ERROR "DNF_SERVICES=${DNF_SERVICES} 未匹配任何服务；可选：stun relay channel bridge auction point coserver dbmw game guild manager monitor statics community")
endif()

add_custom_target(df_binaries DEPENDS ${DNF_SERVICE_TARGETS})

message(STATUS "DNF 统一构建入口：targets = third_party + ${DNF_SERVICE_TARGETS}")
message(STATUS "  并行度        : ${DNF_JOBS}")
message(STATUS "  工具链根      : ${DNF_TOOLCHAINS}")
message(STATUS "  子构建目录    : ${DNF_BUILD}/<svc>")
