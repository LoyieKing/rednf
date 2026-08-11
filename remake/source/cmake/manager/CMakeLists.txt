cmake_minimum_required(VERSION 3.10)
project(manager CXX)

# ============================================================
# df_manager_r CMake 构建（2026-08-10，替代 source/toolchain/build-manager.sh）
# 口径与原脚本完全一致：
#   - 源文件与顺序 = build-manager.sh 的 ls 链接顺序（_GLOBAL__I_ 顺序敏感）
#   - 按文件 flags：tinyxml* -O3 -std=gnu++98；CFileLogWriterBase -fno-exceptions；
#     yaSSL/TaoCrypt -O3 -fPIC -DNDEBUG -std=gnu++98 + yassl 专属 define
#   - include 链：ServerCommon/Manager/packet/shared/MySQL(mysqlclient include)
#   - 链接：mysqlclient .a + 多根静态 libstdc++/libgcc + -lpthread -ldl -lm
# 工具链：c6 变体（c6root gcc-4.4.7 包装脚本）。tinyxml 原脚本用 4.4.4 cc1plus
# 编译，CMake 单编译器统一为 4.4.7 cc1plus（偏差记录于报告）。
# 链接驱动：宿主 g++（c6root 4.4.7 驱动无法解析宿主 -m32 crt1.o，与原脚本一致）。
# ============================================================

# 链接驱动：复刻 build-manager.sh 的最终链接（宿主 g++ + 静态库 + -m32 -no-pie）
set(CMAKE_CXX_LINK_EXECUTABLE "/usr/bin/g++ <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake)

set(SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(SVC_DIR ${SRC_ROOT}/DNFServer/GameServer/Manager)
set(COMMON ${SRC_ROOT}/DNFServer/ServerCommon)
set(PACKET ${SRC_ROOT}/shared/packet)
set(SHARED ${SRC_ROOT}/shared)
set(MYSQL ${SRC_ROOT}/Library3rd/MySQL)
set(YASSL ${MYSQL}/extlib/yassl)

# ---- 源文件与顺序：与 build-manager.sh 的 ls 链接顺序一致 ----
dnf_service(manager
    SOURCES
        ${COMMON}/CFileLogWriterBase.cpp
        ${SVC_DIR}/DBManager.cpp
        ${SVC_DIR}/DNFAppConfig.cpp
        ${SVC_DIR}/DNFAppStartInit.cpp
        ${SVC_DIR}/DNFAppStopInit.cpp
        ${SVC_DIR}/DNFApplication.cpp
        ${COMMON}/DNFFileLog.cpp
        ${SVC_DIR}/DNFFloatingPointExceptSig.cpp
        ${COMMON}/DNFFunctionLib.cpp
        ${SVC_DIR}/DNFKillUserConfig.cpp
        ${SVC_DIR}/DNFManagerServerMain.cpp
        ${SVC_DIR}/DNFMonitorServer.cpp
        ${SVC_DIR}/DNFMySql.cpp
        ${SVC_DIR}/DNFNetworkThread.cpp
        ${SVC_DIR}/DNFPacketBuffer.cpp
        ${SVC_DIR}/DNFPacketDecoder.cpp
        ${SVC_DIR}/DNFPacketTracer.cpp
        ${SVC_DIR}/DNFPacketTranslater.cpp
        ${SVC_DIR}/DNFSegmentationFaultSig.cpp
        ${SVC_DIR}/DNFServerConfig.cpp
        ${SVC_DIR}/DNFServerHandler.cpp
        ${SVC_DIR}/DNFSignal.cpp
        ${SVC_DIR}/DNFSignalTranslator.cpp
        ${SVC_DIR}/DNFSystemFailSig.cpp
        ${SVC_DIR}/DNFTableBase.cpp
        ${SVC_DIR}/DNFTcpAcceptThread.cpp
        ${SVC_DIR}/DNFTcpHandler.cpp
        ${SVC_DIR}/DNFTcpNetworkThread.cpp
        ${SVC_DIR}/DNFTcpServerInterface.cpp
        ${SVC_DIR}/DNFTcpSocket.cpp
        ${SVC_DIR}/DNFTerminateSig.cpp
        ${SVC_DIR}/DNFThreadInterface.cpp
        ${SVC_DIR}/DNFTickHandler.cpp
        ${SVC_DIR}/DNFUdpHandler.cpp
        ${SVC_DIR}/DNFUser1Sig.cpp
        ${PACKET}/src/PacketHeader.cpp
        ${SVC_DIR}/Peer.cpp
        ${SVC_DIR}/ProhibitUser.cpp
        ${SVC_DIR}/QueryCounter.cpp
        ${SVC_DIR}/SystemTimeHandler.cpp
        ${SVC_DIR}/TcpNetSystem.cpp
        ${COMMON}/Thread.cpp
        ${SVC_DIR}/UserManager.cpp
        ${COMMON}/tinyxml.cpp
        ${COMMON}/tinyxmlerror.cpp
        ${COMMON}/tinyxmlparser.cpp
        # ---- yaSSL / TaoCrypt（与脚本 TAOCRYPT_SRC + YASSL_SRC 一致）----
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
        ${SRC_ROOT}/../build/manager/mysqlclient/trees.o
    INCLUDES
        "-I${COMMON}" "-I${SVC_DIR}" "-I${PACKET}/include"
        "-I${SHARED}" "-I${SHARED}/common/include"
        "-I${MYSQL}/include" "-I${MYSQL}/mysys" "-I${MYSQL}/strings"
    DEFINES
        -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS -DDNF_SVC_MANAGER
    SRC_OPTS
        "${COMMON}/CFileLogWriterBase.cpp|-fno-exceptions"
        "${COMMON}/tinyxml.cpp|-O3 -std=gnu++98"
        "${COMMON}/tinyxmlerror.cpp|-O3 -std=gnu++98"
        "${COMMON}/tinyxmlparser.cpp|-O3 -std=gnu++98"
        "${SVC_DIR}/DNFTableBase.cpp|-fenforce-eh-specs"
        # 逐 TU 工具链对齐（ORIG .comment 混合 4.4.6/4.4.7/4.4.4/4.1.2）：
        # DNFApplication.cpp / DNFSignalTranslator.cpp / DNFTcpHandler.cpp 在
        # 4.4.6-3 cc1plus 下与 ORIG 逐条一致（A/B 验证；成员链虚调用 fn 指针
        # 进 ecx/edx，4.4.7 进 eax，与 ORIG 不符）。
        "${SVC_DIR}/DNFApplication.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFSignalTranslator.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFTcpHandler.cpp|-B${DF_CC1P446}/"
        # 2026-08-11 第 4 轮：以下 TU 的 ORIG 虚调用同样为 fn 指针进
        # ecx/edx（4.4.7-23 进 eax），A/B 试编译 4.4.6-3 后 call 寄存器
        # 与 ORIG 一致（其余差异继续逐函数源码修复）。
        "${SVC_DIR}/DBManager.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFMySql.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFNetworkThread.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFPacketDecoder.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFPacketTranslater.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFTcpAcceptThread.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFTcpNetworkThread.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFTableBase.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFThreadInterface.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/DNFPacketBuffer.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/QueryCounter.cpp|-B${DF_CC1P446}/"
        "${SVC_DIR}/TcpNetSystem.cpp|-B${DF_CC1P446}/"
    LIBS
        ${SRC_ROOT}/../build/manager/mysqlclient/libmysqlclient.a
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libgcc_eh.a
        ${DF_C5R52I}/usr/lib/gcc/i386-redhat-linux/4.1.1/libstdc++.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc_eh.a
    LINK_FLAGS
        -m32 -no-pie -lpthread -ldl -lm
)

# 脚本全局 -std=gnu++0x（tinyxml/yaSSL 由逐文件 -std=gnu++98 覆盖）
target_compile_options(manager PRIVATE -std=gnu++0x)

# ---- yaSSL/TaoCrypt 逐文件 flags + 专属 include ----
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

# ---- MySQL Connector/C 6.0.2 客户端静态库（build/manager/mysqlclient）----
add_custom_target(mysqlclient_manager ALL
    COMMAND sh ${SRC_ROOT}/toolchain/build-mysqlclient.sh
    WORKING_DIRECTORY ${SRC_ROOT}/toolchain
    COMMENT "build-mysqlclient.sh -> build/manager/mysqlclient/libmysqlclient.a")
add_dependencies(manager mysqlclient_manager)
