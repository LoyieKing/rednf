cmake_minimum_required(VERSION 3.13)
project(bridge CXX)

# c6446r 变体：c6root g++ 4.4.7 驱动 + cc1plus 4.4.6（等价 /tmp/c6-g++-446r 的 -B）。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B ${DF_CC1P446}/")

set(DNF_SRC "${CMAKE_CURRENT_LIST_DIR}/../..")
include("${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake")

set(_SD "${DNF_SRC}/ChannelOld/DNFChannelBridge")

# 源文件与顺序取自 build-bridge.sh 默认 SOURCES（含 DNFServerCommon/DNFFunctionLib）。
dnf_service(bridge
    SOURCES
        ${_SD}/stdafx.cpp
        ${_SD}/Exception.cpp
        ${_SD}/System.cpp
        ${_SD}/Thread.cpp
        ${_SD}/ThreadLock.cpp
        ${_SD}/Token.cpp
        ${_SD}/UDPHandler.cpp
        ${_SD}/Socket.cpp
        ${_SD}/SocketSystem.cpp
        ${_SD}/Service.cpp
        ${_SD}/LinuxService.cpp
        ${_SD}/Globals.cpp
        ${_SD}/ScriptData.cpp
        ${_SD}/ScriptRawData.cpp
        ${_SD}/Script.cpp
        ${_SD}/ScriptThread.cpp
        ${_SD}/TCPAcceptThread.cpp
        ${_SD}/TCPHandler.cpp
        ${_SD}/TCPThread.cpp
        ${_SD}/TCPUser.cpp
        ${_SD}/UDPThread.cpp
        ${_SD}/DBMgr.cpp
        ${_SD}/MysqlClient.cpp
        ${_SD}/DNFChannelBridge.cpp
        ${_SD}/ChannelService.cpp
        ${_SD}/CheckThread.cpp
        ${_SD}/DNFServerCommon/DNFFunctionLib.cpp
    INCLUDES
        -isystem${DNF_SRC}/Library3rd/MySQL/include
    DEFINES
        -D_GNU_SOURCE -std=gnu++98 -pthread
    LINK_FLAGS
        -no-pie -static-libgcc -lpthread -ldl ${DF_ZLIB32}/lib/libz.a -lm
)

# 链接用宿主 g++（与 build-bridge.sh 一致：-m32 -no-pie -static-libgcc）。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
set(CMAKE_CXX_LINK_EXECUTABLE "${DNF_HOST_GXX} -m32 -no-pie <OBJECTS> -o <TARGET> <LINK_FLAGS> <LINK_LIBRARIES>")
