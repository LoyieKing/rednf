cmake_minimum_required(VERSION 3.10)

# ============================================================
# df_community_r  CMake 构建（2026-08-11 切换为 GCC 4.4.6 精确对齐工具链）
#
# 原系统编译器(GCC 13)构建与 ORIG(GCC 4.4.6-3)代码生成差异过大，265 个函数全部
# DIFF。与 channel/bridge/auction/point 相同方案：c6root 4.4.7 驱动 + cc1plus
# 4.4.6（DF_CC_VARIANT=c6446r），dnf_toolchain_44.cmake 提供 -nostdinc + c6root
# 头文件；链接用宿主 g++ -m32 （同 build-channel.sh）。
#
# 用法（工具链自举，无需外部 -DCMAKE_TOOLCHAIN_FILE）:
#   cmake -S source/cmake/community -B build/community
#   cmake --build build/community -j8
# ============================================================
# 工具链自举（2026-09-14 修）：dnf_helpers.cmake 里 COMMON_FLAGS 无条件带 -nostdinc，
# 完全依赖 ${DF_C6ROOT}/${DF_LSD44} 等变量的 -isystem 路径提供标准头。若 configure 时
# 没有加载 dnf_toolchain_44.cmake（README 的裸命令就是这样），这些变量为空 →
# -isystem/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include 等全部指向不存在的目录，
# 同时宿主 <c++/13> 路径又被 -nostdinc 剔除 ⇒ cstdio/vector/iostream 全部
# "No such file or directory"。这里在 project() 之前把工具链文件 include 进来，
# 使 README 的裸命令即可构建（等价于命令行传 -DCMAKE_TOOLCHAIN_FILE）。
# DF_TC_ROOT 缺省用仓库内 toolchains/（旧文档写的 /tmp 已不存在）。
if(NOT decompile_verify)
    message(STATUS "decompile_verify=OFF：community 使用宿主最新 LTS GCC（跳过 4.x 工具链）")
else()
if(NOT DEFINED DF_TC_ROOT)
    get_filename_component(_DNF_TC_DEFAULT "${CMAKE_CURRENT_LIST_DIR}/../../../toolchains" REALPATH)
    set(DF_TC_ROOT "${_DNF_TC_DEFAULT}" CACHE PATH "GCC 4.x 工具链根")
    unset(_DNF_TC_DEFAULT)
endif()
if(NOT DEFINED DF_CC_VARIANT)
    set(DF_CC_VARIANT "c6446r" CACHE STRING "编译器变体 (c6/c5/c6446r/c6444r)")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_toolchain_44.cmake")

# 工具链根缺失时立刻报清楚，而不是让 -nostdinc 之后吐出满屏 "cstdio: No such file"。
if(DF_CC_VARIANT STREQUAL "c5")
    set(_DNF_TC_NEED "${DF_C5ROOT}")
else()
    set(_DNF_TC_NEED "${DF_C6ROOT}")
endif()
if(NOT IS_DIRECTORY "${_DNF_TC_NEED}")
    message(FATAL_ERROR
        "GCC 4.x 工具链根不存在: ${_DNF_TC_NEED}\n"
        "  请用 -DDF_TC_ROOT=<含 c6root/c5root 的目录> 指定（仓库内为 "
        "${CMAKE_CURRENT_LIST_DIR}/../../../toolchains），"
        "或见 docs/toolchain_bootstrap.md 重建工具链。")
endif()
unset(_DNF_TC_NEED)
endif()

project(community CXX)
if(decompile_verify)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B ${DF_CC1P446}/")
endif()

set(DNF_SRC "${CMAKE_CURRENT_LIST_DIR}/../..")
include("${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake")

set(COMMUNITY     "${DNF_SRC}/Community")
set(SERVERCOMMON  "${DNF_SRC}/DNFServer/ServerCommon")
set(SHARED        "${DNF_SRC}/shared")

dnf_service(community
    SOURCES
        ${COMMUNITY}/ArchiveLog.cpp
        ${COMMUNITY}/BuddyManager.cpp
        ${COMMUNITY}/CommunityServer.cpp
        ${COMMUNITY}/ConfigFileReader.cpp
        ${COMMUNITY}/CoreDump.cpp
        ${COMMUNITY}/GlobalFunction.cpp
        ${COMMUNITY}/GlobalInstance.cpp
        ${COMMUNITY}/NetworkSession.cpp
        ${COMMUNITY}/PacketDispatcher.cpp
        ${COMMUNITY}/PacketParser.cpp
        ${COMMUNITY}/PacketProc.cpp
        ${COMMUNITY}/ProcessManager.cpp
        ${COMMUNITY}/SessionManager.cpp
        ${COMMUNITY}/User.cpp
        ${COMMUNITY}/UserManager.cpp
        ${COMMUNITY}/rand_r.cpp
        ${SERVERCOMMON}/DNFFileLog.cpp
        ${SERVERCOMMON}/CFileLogWriterBase.cpp
        ${SERVERCOMMON}/DNFFunctionLib.cpp
        ${SERVERCOMMON}/Thread.cpp
        ${SHARED}/packet/src/PacketHeader.cpp
        ${SHARED}/packet/src/STGameUserInfo.cpp
        ${SHARED}/packet/src/STPvPBuddyDBInfo.cpp
    INCLUDES
        -I${COMMUNITY} -I${SERVERCOMMON}
        -isystem${DNF_SRC}/Library3rd/Boost/Include
        -I${SHARED} -I${SHARED}/common/include -I${SHARED}/packet/include
    DEFINES
        -D_GNU_SOURCE -std=gnu++0x -pthread -DBOOST_DISABLE_ASSERTS -DDNF_SVC_COMMUNITY
    SRC_OPTS
        "${COMMUNITY}/ArchiveLog.cpp|-fno-exceptions"
        "${SERVERCOMMON}/CFileLogWriterBase.cpp|-fno-exceptions"
    LIBS
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libgcc_eh.a
        ${DF_C5R52I}/usr/lib/gcc/i386-redhat-linux/4.1.1/libstdc++.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc_eh.a
        -lpthread -ldl -lm
    LINK_FLAGS
        -m32 
)

# 链接用宿主 g++（同 build-channel.sh / build-auction.sh：-m32  -static-libgcc）。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
# [2026-09-14 修复] 原用**宿主 g++** 链接 ⇒ 产物要求 GLIBC_2.34/2.35，而目标运行环境是
#   CentOS 7（glibc 2.17）⇒ `version 'GLIBC_2.34' not found` 跑不起来；ORIG 只要 ≤2.4。
#   改用 game 同款 c6root 4.4.7 驱动 + --sysroot 链接（dnf_game_link.sh 本身通用）。
#   c6root 4.4.7 不认 -no-pie（晚于 4.4），4.4 默认即非 PIE。
if(decompile_verify)
    set(CMAKE_CXX_LINK_EXECUTABLE
        "${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_game_link.sh <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
endif()
