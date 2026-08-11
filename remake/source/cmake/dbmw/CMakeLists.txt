cmake_minimum_required(VERSION 3.10)
project(dbmw CXX)

# ============================================================
# df_dbmw_r CMake 构建（2026-08-10，替代 source/toolchain/build-dbmw.sh）
# 口径与原脚本完全一致（源顺序/按文件 flags/include/链接），参见 manager 说明。
# 注意：mysqlclient 与 manager 共用 build/manager/mysqlclient（原脚本如此）。
# ============================================================
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CURRENT_LIST_DIR}/../../toolchain/link_dbmw.sh <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake)

set(SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(SVC_DIR ${SRC_ROOT}/DNFServer/GameServer/DBMW)
set(COMMON ${SRC_ROOT}/DNFServer/ServerCommon)
set(PACKET ${SRC_ROOT}/shared/packet)
set(SHARED ${SRC_ROOT}/shared)
set(MYSQL ${SRC_ROOT}/Library3rd/MySQL)
set(YASSL ${MYSQL}/extlib/yassl)

dnf_service(dbmw
    SOURCES
        ${COMMON}/CFileLogWriterBase.cpp
        ${SVC_DIR}/CMisc.cpp
        ${SVC_DIR}/DBManager.cpp
        ${SVC_DIR}/DNFAppConfig.cpp
        ${SVC_DIR}/DNFAppStartInit.cpp
        ${SVC_DIR}/DNFAppStopInit.cpp
        ${SVC_DIR}/DNFApplication.cpp
        ${SVC_DIR}/DNFDBServerMain.cpp
        ${COMMON}/DNFFileLog.cpp
        ${SVC_DIR}/DNFFloatingPointExceptSig.cpp
        ${COMMON}/DNFFunctionLib.cpp
        ${SVC_DIR}/DNFGameServer.cpp
        ${SVC_DIR}/DNFGuildServer.cpp
        ${SVC_DIR}/DNFKillUserConfig.cpp
        ${SVC_DIR}/DNFMonitorServer.cpp
        ${SVC_DIR}/DNFMySql.cpp
        ${SVC_DIR}/DNFNetworkThread.cpp
        ${SVC_DIR}/DNFPacketBuffer.cpp
        ${SVC_DIR}/DNFPacketDecoder.cpp
        ${SVC_DIR}/DNFPacketTracer.cpp
        ${SVC_DIR}/DNFPacketTranslater.cpp
        ${SVC_DIR}/DNFProhibitUser.cpp
        ${SVC_DIR}/DNFSegmentationFaultSig.cpp
        ${SVC_DIR}/DNFServerConfig.cpp
        ${SVC_DIR}/DNFServerHandler.cpp
        ${SVC_DIR}/DNFSignal.cpp
        ${SVC_DIR}/DNFSignalTranslator.cpp
        ${SVC_DIR}/DNFStatisticsServer.cpp
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
        ${SVC_DIR}/DNFUserManager.cpp
        ${SVC_DIR}/EpollHandler.cpp
        ${SVC_DIR}/GMAccounts.cpp
        ${SVC_DIR}/GuildManager.cpp
        ${SVC_DIR}/Method.cpp
        ${SVC_DIR}/PacketCounter.cpp
        ${PACKET}/src/PacketHeader.cpp
        ${SVC_DIR}/Peer.cpp
        ${SVC_DIR}/QueryCounter.cpp
        ${SVC_DIR}/QueryCounterFileLog.cpp
        ${SVC_DIR}/SHA.cpp
        ${SVC_DIR}/ServerLoadChecker.cpp
        ${SVC_DIR}/ServerXml.cpp
        ${SVC_DIR}/StackBuffer.cpp
        ${SVC_DIR}/StringFormat.cpp
        ${SVC_DIR}/SystemTimeHandler.cpp
        ${SVC_DIR}/TEA.cpp
        ${SVC_DIR}/TcpNetSystem.cpp
        ${COMMON}/Thread.cpp
        ${COMMON}/tinyxml.cpp
        ${COMMON}/tinyxmlerror.cpp
        ${COMMON}/tinyxmlparser.cpp
        ${SVC_DIR}/version.cpp
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
        ${SRC_ROOT}/../build/manager/mysqlclient/trees.o
    INCLUDES
        "-I${COMMON}" "-I${SVC_DIR}" "-I${PACKET}/include"
        "-I${SHARED}" "-I${SHARED}/common/include"
        "-I${MYSQL}/include" "-I${MYSQL}/mysys" "-I${MYSQL}/strings"
    DEFINES
        -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS -DDNF_SVC_DBMW
    SRC_OPTS
        "${COMMON}/CFileLogWriterBase.cpp|-fno-exceptions"
        "${COMMON}/tinyxml.cpp|-O3 -std=gnu++98"
        "${COMMON}/tinyxmlerror.cpp|-O3 -std=gnu++98"
        "${COMMON}/tinyxmlparser.cpp|-O3 -std=gnu++98"
        # 虚调用 fn 指针寄存器形态对齐（ORIG 进 ecx/edx，4.4.7-23 进 eax）：
        # 与 manager 同源 TU 同批 A/B 验证（DNFApplication/DNFSignalTranslator），
        # DNFPacketTracer/DNFMySql 同形态逐函数复检。
        "${SVC_DIR}/DNFApplication.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DNFSignalTranslator.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DNFPacketTracer.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DNFMySql.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DNFSignal.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DNFTcpHandler.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DNFThreadInterface.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DNFServerHandler.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DNFPacketTranslater.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DNFPacketBuffer.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/StackBuffer.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/DBManager.cpp|-B${DF_CC1P446}"
        "${SVC_DIR}/QueryCounter.cpp|-B${DF_CC1P446} -march=i486 -mtune=pentiumpro"
        "${SVC_DIR}/QueryCounterFileLog.cpp|-B${DF_CC1P446}"
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

target_compile_options(dbmw PRIVATE -std=gnu++0x)

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

add_custom_target(mysqlclient_dbmw ALL
    COMMAND sh ${SRC_ROOT}/toolchain/build-mysqlclient.sh
    WORKING_DIRECTORY ${SRC_ROOT}/toolchain
    COMMENT "build-mysqlclient.sh -> build/manager/mysqlclient/libmysqlclient.a")
add_dependencies(dbmw mysqlclient_dbmw)
