cmake_minimum_required(VERSION 3.13)
# channel 专用编译规则：经 channel_cc.sh 把 "-c <绝对路径>" 改写为
# "cd 到源目录 + basename"，复现 ORIG 构建的 __FILE__（见 channel_cc.sh）。
if(decompile_verify)
    set(CMAKE_CXX_COMPILE_OBJECT
        "${CMAKE_CURRENT_LIST_DIR}/channel_cc.sh <DEFINES> <INCLUDES> <FLAGS> -MD -MT <TARGET> -MF <DEP_FILE> -o <OBJECT> -c <SOURCE>")
endif()
project(channel CXX)

# c6446r 变体：c6root g++ 4.4.7 驱动 + cc1plus 4.4.6（等价 /tmp/c6-g++-446r 的 -B）。
if(decompile_verify)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B ${DF_CC1P446}/")
endif()

set(DNF_SRC "${CMAKE_CURRENT_LIST_DIR}/../..")
include("${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake")

set(_SD "${DNF_SRC}/ChannelOld/DNFChannelServer")

# 源文件与顺序取自 build-channel.sh 默认 SOURCES（31+1 个 TU，链接顺序敏感）。
dnf_service(channel
    SOURCES
        ${_SD}/stdafx.cpp
        ${_SD}/Exception.cpp
        ${_SD}/System.cpp
        ${_SD}/Thread.cpp
        ${_SD}/ThreadLock.cpp
        ${_SD}/Token.cpp
        ${_SD}/SocketSystem.cpp
        ${_SD}/Method.cpp
        ${_SD}/SHA.cpp
        ${_SD}/Rijndael.cpp
        ${_SD}/Service.cpp
        ${_SD}/LinuxService.cpp
        ${_SD}/Globals.cpp
        ${_SD}/ScriptData.cpp
        ${_SD}/ScriptRawData.cpp
        ${_SD}/Script.cpp
        ${_SD}/Socket.cpp
        ${_SD}/TCPUser.cpp
        ${_SD}/TCPHandler.cpp
        ${_SD}/UDPUser.cpp
        ${_SD}/UDPHandler.cpp
        ${_SD}/TCPAcceptThread.cpp
        ${_SD}/TCPThread.cpp
        ${_SD}/UDPThread.cpp
        ${_SD}/CheckThread.cpp
        ${_SD}/ChannelService.cpp
        ${_SD}/DNFChannelServer.cpp
        ${_SD}/PIDHelper.cpp
        ${_SD}/SignalHandler.cpp
        ${_SD}/UniqueIndex.cpp
        ${_SD}/CommandLineParser.cpp
        ${_SD}/Reactor.cpp
    DEFINES
        -D_GNU_SOURCE -std=gnu++98 -pthread
    # [2026-09-14] 改用 c6root 驱动（--sysroot -nostdlib）后，C++ 运行时不再由宿主
    #   驱动隐式带上 ⇒ 必须像 manager/game 那样显式列出静态 libstdc++/libgcc；
    #   并补 -lrt（老 glibc 的 clock_gettime 在 librt 里，宿主 glibc 2.39 已并入 libc）。
    LIBS
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libgcc_eh.a
        ${DF_C5R52I}/usr/lib/gcc/i386-redhat-linux/4.1.1/libstdc++.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc_eh.a
        # zlib 必须放在**对象之后**（CMake 会把 LINK_FLAGS 排在对象之前 ⇒ 归档不被重扫 ⇒
        # `undefined reference to compress2`，实测踩过）
        ${DF_ZLIB32}/lib/libz.a
    LINK_FLAGS
        -static-libgcc -lpthread -lrt -ldl
)

# 链接用宿主 g++（与 build-channel.sh 一致：-m32  -static-libgcc 动态链宿主
# libstdc++/libc）。c6root 4.4.7 驱动不识别 -no-pie，且其默认库搜索路径在现代宿主失效。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
# [2026-09-14 修复] 原用**宿主 g++** 链接 ⇒ 产物要求 GLIBC_2.34/2.35，而目标运行环境
#   是 CentOS 7（glibc 2.17）⇒ `version 'GLIBC_2.34' not found` 直接跑不起来；
#   ORIG 只要 GLIBC_2.4。改用 game 同款的 c6root 4.4.7 驱动 + --sysroot 链接脚本
#   （dnf_game_link.sh 本身是通用 32 位 c6root 链接驱动，非 game 专属）。
#   注意：c6root 4.4.7 的 g++ 不认 `-no-pie`（晚于 4.4），4.4 默认即非 PIE ⇒ 已从 LINK_FLAGS 去掉。
if(decompile_verify)
    set(CMAKE_CXX_LINK_EXECUTABLE
        "${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_game_link.sh <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
endif()
