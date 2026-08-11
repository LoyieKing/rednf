cmake_minimum_required(VERSION 3.10)
project(guild CXX)

# ============================================================
# df_guild_r CMake 构建（2026-08-10，替代 source/toolchain/build-guild.sh）
# 口径与原脚本完全一致（源顺序/按文件 flags/include/链接），参见 manager 说明。
# 本服务无 yaSSL/mysqlclient。
# ============================================================
set(CMAKE_CXX_LINK_EXECUTABLE "/usr/bin/g++ <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake)

set(SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(SVC_DIR ${SRC_ROOT}/DNFServer/GameServer/Guild)
set(COMMON ${SRC_ROOT}/DNFServer/ServerCommon)
set(PACKET ${SRC_ROOT}/shared/packet)
set(SHARED ${SRC_ROOT}/shared)

dnf_service(guild
    SOURCES
        ${SVC_DIR}/BlackUser.cpp
        ${COMMON}/CFileLogWriterBase.cpp
        ${SVC_DIR}/CashObject.cpp
        ${SVC_DIR}/DNFAppConfig.cpp
        ${SVC_DIR}/DNFAppStartInit.cpp
        ${SVC_DIR}/DNFAppStopInit.cpp
        ${SVC_DIR}/DNFApplication.cpp
        ${SVC_DIR}/DNFDBServer.cpp
        ${COMMON}/DNFFileLog.cpp
        ${SVC_DIR}/DNFFloatingPointExceptSig.cpp
        ${COMMON}/DNFFunctionLib.cpp
        ${SVC_DIR}/DNFGameServer.cpp
        ${SVC_DIR}/DNFGuild.cpp
        ${SVC_DIR}/DNFGuildManager.cpp
        ${SVC_DIR}/DNFGuildServerMain.cpp
        ${SVC_DIR}/DNFGuildWar.cpp
        ${SVC_DIR}/DNFInnerMsgHandler.cpp
        ${SVC_DIR}/DNFKillUserConfig.cpp
        ${SVC_DIR}/DNFManagerServer.cpp
        ${SVC_DIR}/DNFMonitorServer.cpp
        ${SVC_DIR}/DNFNetworkThread.cpp
        ${SVC_DIR}/DNFPacketBuffer.cpp
        ${SVC_DIR}/DNFPacketDecoder.cpp
        ${SVC_DIR}/DNFPacketTracer.cpp
        ${SVC_DIR}/DNFPacketTranslater.cpp
        ${SVC_DIR}/DNFPowerWarConfig.cpp
        ${SVC_DIR}/DNFSegmentationFaultSig.cpp
        ${SVC_DIR}/DNFServerConfig.cpp
        ${SVC_DIR}/DNFServerHandler.cpp
        ${SVC_DIR}/DNFServerInterface.cpp
        ${SVC_DIR}/DNFSignal.cpp
        ${SVC_DIR}/DNFSignalTranslator.cpp
        ${SVC_DIR}/DNFSystemFailSig.cpp
        ${SVC_DIR}/DNFTableBase.cpp
        ${SVC_DIR}/DNFTcpAcceptThread.cpp
        ${SVC_DIR}/DNFTcpHandler.cpp
        ${SVC_DIR}/DNFTcpNetworkThread.cpp
        ${SVC_DIR}/DNFTcpSocket.cpp
        ${SVC_DIR}/DNFTerminateSig.cpp
        ${SVC_DIR}/DNFThreadInterface.cpp
        ${SVC_DIR}/DNFTickHandler.cpp
        ${SVC_DIR}/DNFUdpHandler.cpp
        ${SVC_DIR}/DNFUser.cpp
        ${SVC_DIR}/DNFUser1Sig.cpp
        ${SVC_DIR}/DNFUserManager.cpp
        ${SVC_DIR}/DNFVersion.cpp
        ${SVC_DIR}/GMAccounts.cpp
        ${SVC_DIR}/GuildBoard.cpp
        ${SVC_DIR}/GuildCargo.cpp
        ${SVC_DIR}/MemPool.cpp
        ${SVC_DIR}/MemoryCashManager.cpp
        ${SVC_DIR}/PacketCounter.cpp
        ${PACKET}/src/PacketHeader.cpp
        ${SVC_DIR}/Peer.cpp
        ${SVC_DIR}/Power.cpp
        ${SVC_DIR}/PowerManager.cpp
        ${SVC_DIR}/PowerWar.cpp
        ${SVC_DIR}/PowerWarCharacInfo.cpp
        ${SVC_DIR}/PowerWarGuildInfo.cpp
        ${SVC_DIR}/Scheduler.cpp
        ${SVC_DIR}/ServerLoadChecker.cpp
        ${SVC_DIR}/ServerXml.cpp
        ${SVC_DIR}/SystemTimeHandler.cpp
        ${SVC_DIR}/TcpNetSystem.cpp
        ${COMMON}/Thread.cpp
        ${SVC_DIR}/WebEvent.cpp
        ${COMMON}/tinyxml.cpp
        ${COMMON}/tinyxmlerror.cpp
        ${COMMON}/tinyxmlparser.cpp
    INCLUDES
        "-I${COMMON}" "-I${SVC_DIR}" "-I${PACKET}/include"
        "-I${SHARED}" "-I${SHARED}/common/include"
    DEFINES
        -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS -DDNF_SVC_GUILD
    SRC_OPTS
        "${COMMON}/CFileLogWriterBase.cpp|-fno-exceptions"
        "${COMMON}/tinyxml.cpp|-O3 -std=gnu++98"
        "${COMMON}/tinyxmlerror.cpp|-O3 -std=gnu++98"
        "${COMMON}/tinyxmlparser.cpp|-O3 -std=gnu++98"
    LIBS
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libgcc_eh.a
        ${DF_C5R52I}/usr/lib/gcc/i386-redhat-linux/4.1.1/libstdc++.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc_eh.a
    LINK_FLAGS
        -m32 -no-pie -lpthread -ldl -lm
)

target_compile_options(guild PRIVATE -std=gnu++0x)
