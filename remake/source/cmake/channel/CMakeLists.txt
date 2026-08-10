cmake_minimum_required(VERSION 3.13)
project(channel CXX)

# c6446r 变体：c6root g++ 4.4.7 驱动 + cc1plus 4.4.6（等价 /tmp/c6-g++-446r 的 -B）。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B ${DF_CC1P446}/")

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
    LINK_FLAGS
        -no-pie -static-libgcc -lpthread -ldl ${DF_ZLIB32}/lib/libz.a
)

# 链接用宿主 g++（与 build-channel.sh 一致：-m32 -no-pie -static-libgcc 动态链宿主
# libstdc++/libc）。c6root 4.4.7 驱动不识别 -no-pie，且其默认库搜索路径在现代宿主失效。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
set(CMAKE_CXX_LINK_EXECUTABLE "${DNF_HOST_GXX} -m32 -no-pie <OBJECTS> -o <TARGET> <LINK_FLAGS> <LINK_LIBRARIES>")
