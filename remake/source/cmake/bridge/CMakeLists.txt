cmake_minimum_required(VERSION 3.13)
project(bridge CXX)

# c6446r 变体：c6root g++ 4.4.7 驱动 + cc1plus 4.4.6（等价 /tmp/c6-g++-446r 的 -B）。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B ${DF_CC1P446}/")
# ORIG 项目 TU 为 i586 目标形态（无 CMOV；常量除法用 sar/shr/lea 展开），
# 第三方静态库（mysql/zlib/yaSSL）为 i686（含 cmov）。mtune=i686 保留 imul。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i586 -mtune=i686")

set(DNF_SRC "${CMAKE_CURRENT_LIST_DIR}/../..")
include("${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake")

set(_SD "${DNF_SRC}/ChannelOld/DNFChannelBridge")

# ---- MySQL 客户端 + yaSSL/TaoCrypt：与 manager 完全同源同写法 ----
# ORIG df_bridge_r 的判定依据：
#   nm | grep -cE ' T mysql_'  = 110（与本工程 manager 的符号集**逐名相同**）
#   nm | grep -ciE 'yassl|taocrypt' = 1638（yaSSL/TaoCrypt 亦静态链入）
#   nm | grep -E ' T ...' 显示 deflate/inflate/crc32 等 zlib 亦被客户端引用拉入
#   DWARF：31 个 CU 全部为工程 TU，**无 MysqlClient.cpp** —— 因此本工程早期
#   为占位而写的 MysqlClient.cpp（8 个 mysql_* 桩）从 SOURCES 中移除，
#   DBMgr 的 mysql_* 调用改为直接解析到静态链接的真实客户端。
set(MYSQL ${DNF_SRC}/Library3rd/MySQL)
set(YASSL ${MYSQL}/extlib/yassl)

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
        ${_SD}/DNFChannelBridge.cpp
        ${_SD}/ChannelService.cpp
        ${_SD}/CheckThread.cpp
        ${_SD}/DNFServerCommon/DNFFunctionLib.cpp
        # ---- yaSSL / TaoCrypt（与 manager 的 TAOCRYPT_SRC + YASSL_SRC 同表同序）----
        ${YASSL}/taocrypt/src/aes.cpp
        ${YASSL}/taocrypt/src/aestables.cpp
        ${YASSL}/taocrypt/src/algebra.cpp
        ${YASSL}/taocrypt/src/arc4.cpp
        ${YASSL}/taocrypt/src/asn.cpp
        ${YASSL}/taocrypt/src/coding.cpp
        ${YASSL}/taocrypt/src/des.cpp
        ${YASSL}/taocrypt/src/dh.cpp
        ${YASSL}/taocrypt/src/dsa.cpp
        ${YASSL}/taocrypt/src/file.cpp
        ${YASSL}/taocrypt/src/hash.cpp
        ${YASSL}/taocrypt/src/integer.cpp
        ${YASSL}/taocrypt/src/md2.cpp
        ${YASSL}/taocrypt/src/md4.cpp
        ${YASSL}/taocrypt/src/md5.cpp
        ${YASSL}/taocrypt/src/misc.cpp
        ${YASSL}/taocrypt/src/random.cpp
        ${YASSL}/taocrypt/src/ripemd.cpp
        ${YASSL}/taocrypt/src/rsa.cpp
        ${YASSL}/taocrypt/src/sha.cpp
        ${YASSL}/taocrypt/src/block_instnt.cpp
        ${YASSL}/src/buffer.cpp
        ${YASSL}/src/cert_wrapper.cpp
        ${YASSL}/src/crypto_wrapper.cpp
        ${YASSL}/src/handshake.cpp
        ${YASSL}/src/lock.cpp
        ${YASSL}/src/log.cpp
        ${YASSL}/src/socket_wrapper.cpp
        ${YASSL}/src/ssl.cpp
        ${YASSL}/src/timer.cpp
        ${YASSL}/src/yassl_error.cpp
        ${YASSL}/src/yassl_imp.cpp
        ${YASSL}/src/yassl_int.cpp
        # ---- mysqlclient 的 zlib trees.o（位于 libmysqlclient.a 之前）----
        ${DNF_SRC}/../build/manager/mysqlclient/trees.o
    INCLUDES
        -isystem${MYSQL}/include
    DEFINES
        -D_GNU_SOURCE -std=gnu++98 -pthread
    LIBS
        ${DNF_SRC}/../build/manager/mysqlclient/libmysqlclient.a
        # [2026-09-14] 改用 c6root 驱动（--sysroot -nostdlib）后 C++ 运行时不再由宿主
        #   驱动隐式带上 ⇒ 显式列出静态 libstdc++/libgcc；zlib 也必须放在对象之后。
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libgcc_eh.a
        ${DF_C5R52I}/usr/lib/gcc/i386-redhat-linux/4.1.1/libstdc++.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc_eh.a
        ${DF_ZLIB32}/lib/libz.a
    LINK_FLAGS
        -static-libgcc -lpthread -lrt -ldl -lm
)

# ---- yaSSL/TaoCrypt 逐文件 flags + 专属 include（与 manager 逐字相同）----
set(YASSL_OPTS -O3 -fPIC -DNDEBUG -std=gnu++98 -DHAVE_CONFIG_H -DYASSL_PREFIX -D_LIB -DHAVE_YASSL -DYASSL_PURE_C -DHAVE_OPENSSL)
set(YASSL_TAOCRYPT aes aestables algebra arc4 asn coding des dh dsa file hash integer md2 md4 md5 misc random ripemd rsa sha block_instnt)
set(YASSL_CORE buffer cert_wrapper crypto_wrapper handshake lock log socket_wrapper ssl timer yassl_error yassl_imp yassl_int)
foreach(f IN LISTS YASSL_TAOCRYPT)
    set_source_files_properties(${YASSL}/taocrypt/src/${f}.cpp PROPERTIES
        COMPILE_OPTIONS "${YASSL_OPTS}"
        INCLUDE_DIRECTORIES "${YASSL}/include;${YASSL}/taocrypt/include;${YASSL}/taocrypt/mySTL")
endforeach()
foreach(f IN LISTS YASSL_CORE)
    set_source_files_properties(${YASSL}/src/${f}.cpp PROPERTIES
        COMPILE_OPTIONS "${YASSL_OPTS}"
        INCLUDE_DIRECTORIES "${YASSL}/include;${YASSL}/taocrypt/include;${YASSL}/taocrypt/mySTL")
endforeach()

# ---- MySQL Connector/C 6.0.2 客户端静态库（与 manager 共用 build-mysqlclient.sh）----
# 产物目录与 manager 相同（build/manager/mysqlclient）——这是脚本 OUT 的硬编码口径。
# 若已存在则不再挂 custom target：避免与并行构建的 manager 同时 ar 同一个归档。
if(NOT EXISTS ${DNF_SRC}/../build/manager/mysqlclient/libmysqlclient.a)
    add_custom_target(mysqlclient_bridge ALL
        COMMAND sh ${DNF_SRC}/toolchain/build-mysqlclient.sh
        WORKING_DIRECTORY ${DNF_SRC}/toolchain
        COMMENT "build-mysqlclient.sh -> build/manager/mysqlclient/libmysqlclient.a")
    add_dependencies(bridge mysqlclient_bridge)
endif()

# 链接用宿主 g++（与 build-bridge.sh 一致：-m32  -static-libgcc）。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
# [2026-09-14 修复] 原用**宿主 g++** 链接 ⇒ 产物要求 GLIBC_2.34/2.35，而目标运行环境是
#   CentOS 7（glibc 2.17）⇒ `version 'GLIBC_2.34' not found` 跑不起来；ORIG 只要 ≤2.4。
#   改用 game 同款 c6root 4.4.7 驱动 + --sysroot 链接（dnf_game_link.sh 本身通用）。
#   c6root 4.4.7 不认 -no-pie（晚于 4.4），4.4 默认即非 PIE。
set(CMAKE_CXX_LINK_EXECUTABLE
    "${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_game_link.sh <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
