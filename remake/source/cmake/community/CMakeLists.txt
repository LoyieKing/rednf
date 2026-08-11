cmake_minimum_required(VERSION 3.10)
project(community CXX)

# ============================================================
# df_community_r  CMake 构建（2026-08-11 切换为 GCC 4.4.6 精确对齐工具链）
#
# 原系统编译器(GCC 13)构建与 ORIG(GCC 4.4.6-3)代码生成差异过大，265 个函数全部
# DIFF。与 channel/bridge/auction/point 相同方案：c6root 4.4.7 驱动 + cc1plus
# 4.4.6（DF_CC_VARIANT=c6446r），dnf_toolchain_44.cmake 提供 -nostdinc + c6root
# 头文件；链接用宿主 g++ -m32 -no-pie（同 build-channel.sh）。
#
# 用法:
#   cmake -S source/cmake/community -B build/community \
#       -DCMAKE_TOOLCHAIN_FILE=source/toolchain/cmake/dnf_toolchain_44.cmake \
#       -DDF_CC_VARIANT=c6446r -DDF_TC_ROOT=/tmp
#   cmake --build build/community -j4
# ============================================================
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B ${DF_CC1P446}/")

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
        -m32 -no-pie
)

# 链接用宿主 g++（同 build-channel.sh / build-auction.sh：-m32 -no-pie -static-libgcc）。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
set(CMAKE_CXX_LINK_EXECUTABLE "${DNF_HOST_GXX} -m32 -no-pie <OBJECTS> -o <TARGET> <LINK_FLAGS> <LINK_LIBRARIES>")
