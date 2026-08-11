cmake_minimum_required(VERSION 3.10)

# ============================================================
# df_relay_r  CMake 构建（取代 source/toolchain/build-relay.sh）
# 混编结构（必须原样保持）：
#   C6 组（23 TU）：c6root g++ 驱动 + cc1plus 4.4.6-3（c6446r 变体）
#        -m32 -O0 -std=gnu++0x -DRELAY_USERPOOL_C6
#   C5 组（5 TU）：c5root g++ 4.1.2 预编译 .o 后加入目标
#        -m32 -O0 -D_GNU_SOURCE -fno-builtin-memset/-strlen/-strcmp
# 链接：宿主机 g++ -m32 -no-pie + 4.4.4/4.1.1 静态 libstdc++/libgcc
# 源顺序 = build 脚本链接顺序（UserPool、DNFRelayServer 在前，其余按基名排序）
# ============================================================
set(DF_CC_VARIANT "c6446r")
include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_toolchain_44.cmake)

# 同 statics/coserver：包装脚本（C6 组用 c6root 驱动，4.4.6 cc1plus 通过 -B 选择；
# -B 由下方 target_compile_options 提供）。
set(DF_CXX_WRAPPER ${CMAKE_BINARY_DIR}/df-cxx-wrap.sh)
file(WRITE ${DF_CXX_WRAPPER}
    "#!/bin/sh\nexport LD_LIBRARY_PATH=\"${DF_C6ROOT}/usr/lib64:${DF_C6ROOT}/usr/lib\"\nexec \"${DF_C6ROOT}/usr/bin/g++\" \"$@\"\n")
execute_process(COMMAND chmod +x ${DF_CXX_WRAPPER})
set(ENV{LD_LIBRARY_PATH} "")
set(CMAKE_CXX_COMPILER ${DF_CXX_WRAPPER})

project(relay CXX)

# 链接驱动用宿主机 g++（同 build 脚本）
set(CMAKE_CXX_LINK_EXECUTABLE
    "g++ <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake)

set(SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(RELAY    ${SRC_ROOT}/DNFServer/GameServer/Relay)

# 2026-08-11 第二轮回归：A/B 试编译证实原版这 5 个 TU 的函数形态为
# 4.4.6-3 + gnu++0x（get_ms_tick/make_dir/setInfo/stopPending 等 C6 下 strict 全等，
# C5 4.1.2-52 为 DIFF），并入 C6 组编译。原 C5 预编译段移除。

dnf_service(relay
    SOURCES
        ${RELAY}/UserPool.cpp
        ${RELAY}/DNFRelayServer.cpp
        ${RELAY}/Exception.cpp
        ${RELAY}/Globals.cpp
        ${RELAY}/Helper.cpp
        ${RELAY}/LinuxService.cpp
        ${RELAY}/PIDHelper.cpp
        ${RELAY}/Reactor.cpp
        ${RELAY}/RelayService.cpp
        ${RELAY}/Script.cpp
        ${RELAY}/ScriptData.cpp
        ${RELAY}/ScriptRawData.cpp
        ${RELAY}/Service.cpp
        ${RELAY}/SignalHandler.cpp
        ${RELAY}/Socket.cpp
        ${RELAY}/SocketSystem.cpp
        ${RELAY}/System.cpp
        ${RELAY}/TCPAcceptThread.cpp
        ${RELAY}/TCPHandler.cpp
        ${RELAY}/TCPThread.cpp
        ${RELAY}/TCPUser.cpp
        ${RELAY}/Thread.cpp
        ${RELAY}/ThreadLock.cpp
        ${RELAY}/Token.cpp
        ${RELAY}/UDPHandler.cpp
        ${RELAY}/UDPThread.cpp
        ${RELAY}/UDPUser.cpp
        ${RELAY}/User.cpp
    INCLUDES
        -I${RELAY}
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

# C6 组：cc1plus 4.4.6-3（-B）+ gnu++0x + RELAY_USERPOOL_C6（仅作用于 C6 源文件）
target_compile_options(relay PRIVATE -B ${DF_CC1P446}/ -std=gnu++0x -DRELAY_USERPOOL_C6)

# ------------------------------------------------------------
# 修正链接顺序：CMake 会把外部 .o（c5/*.o）追加到行尾，而 build-relay.sh 的
# 链接顺序是 UserPool、DNFRelayServer 在前，其余按 .o 基名排序（C5 对象原位插入）。
# 用 POST_BUILD 以脚本完全相同的顺序重链接一次（对象本身已逐字节一致）。
# ------------------------------------------------------------
set(RELAY_OBJ_ROOT ${CMAKE_BINARY_DIR}/CMakeFiles/relay.dir)
file(REAL_PATH ${RELAY} _relay_abs)
string(SUBSTRING ${_relay_abs} 1 -1 _relay_rel)
set(RELAY_OBJ_PREFIX ${RELAY_OBJ_ROOT}/${_relay_rel})

add_custom_command(TARGET relay POST_BUILD
    COMMAND g++ -m32 -no-pie -o ${CMAKE_BINARY_DIR}/df_relay_r
        ${RELAY_OBJ_PREFIX}/UserPool.cpp.o
        ${RELAY_OBJ_PREFIX}/DNFRelayServer.cpp.o
        ${RELAY_OBJ_PREFIX}/Exception.cpp.o
        ${RELAY_OBJ_PREFIX}/Globals.cpp.o
        ${RELAY_OBJ_PREFIX}/Helper.cpp.o
        ${RELAY_OBJ_PREFIX}/LinuxService.cpp.o
        ${RELAY_OBJ_PREFIX}/PIDHelper.cpp.o
        ${RELAY_OBJ_PREFIX}/Reactor.cpp.o
        ${RELAY_OBJ_PREFIX}/RelayService.cpp.o
        ${RELAY_OBJ_PREFIX}/Script.cpp.o
        ${RELAY_OBJ_PREFIX}/ScriptData.cpp.o
        ${RELAY_OBJ_PREFIX}/ScriptRawData.cpp.o
        ${RELAY_OBJ_PREFIX}/Service.cpp.o
        ${RELAY_OBJ_PREFIX}/SignalHandler.cpp.o
        ${RELAY_OBJ_PREFIX}/Socket.cpp.o
        ${RELAY_OBJ_PREFIX}/SocketSystem.cpp.o
        ${RELAY_OBJ_PREFIX}/System.cpp.o
        ${RELAY_OBJ_PREFIX}/TCPAcceptThread.cpp.o
        ${RELAY_OBJ_PREFIX}/TCPHandler.cpp.o
        ${RELAY_OBJ_PREFIX}/TCPThread.cpp.o
        ${RELAY_OBJ_PREFIX}/TCPUser.cpp.o
        ${RELAY_OBJ_PREFIX}/Thread.cpp.o
        ${RELAY_OBJ_PREFIX}/ThreadLock.cpp.o
        ${RELAY_OBJ_PREFIX}/Token.cpp.o
        ${RELAY_OBJ_PREFIX}/UDPHandler.cpp.o
        ${RELAY_OBJ_PREFIX}/UDPThread.cpp.o
        ${RELAY_OBJ_PREFIX}/UDPUser.cpp.o
        ${RELAY_OBJ_PREFIX}/User.cpp.o
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libgcc_eh.a
        ${DF_C5R52I}/usr/lib/gcc/i386-redhat-linux/4.1.1/libstdc++.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc_eh.a
        -lpthread -ldl -lm
    COMMENT "RELINK df_relay_r（build 脚本链接顺序）"
)
