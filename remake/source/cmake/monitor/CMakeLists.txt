cmake_minimum_required(VERSION 3.10)
project(monitor CXX)

# ============================================================
# df_monitor_r CMake 构建（2026-08-10，替代 source/toolchain/build-monitor.sh）
# 口径与原脚本完全一致（源顺序/按文件 flags/include/链接），参见 manager 说明。
# 本服务无 yaSSL/mysqlclient。
# ============================================================
set(CMAKE_CXX_LINK_EXECUTABLE "/usr/bin/g++ <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake)

set(SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(SVC_DIR ${SRC_ROOT}/DNFServer/GameServer/Monitor)
set(COMMON ${SRC_ROOT}/DNFServer/ServerCommon)
set(PACKET ${SRC_ROOT}/shared/packet)
set(SHARED ${SRC_ROOT}/shared)

dnf_service(monitor
    SOURCES
        ${SVC_DIR}/Arad_BirthdayEvent.cpp
        ${SVC_DIR}/Arad_MomijiEvent.cpp
        ${SVC_DIR}/BlackUser.cpp
        ${SVC_DIR}/BuddyRegisterManager.cpp
        ${COMMON}/CFileLogWriterBase.cpp
        ${SVC_DIR}/CashObject.cpp
        ${SVC_DIR}/DNFAppConfig.cpp
        ${SVC_DIR}/DNFAppStartInit.cpp
        ${SVC_DIR}/DNFAppStopInit.cpp
        ${SVC_DIR}/DNFApplication.cpp
        ${SVC_DIR}/DNFBuddy.cpp
        ${SVC_DIR}/DNFBuddyHandle.cpp
        ${SVC_DIR}/DNFChannelWaitingUser.cpp
        ${SVC_DIR}/DNFDBServer.cpp
        ${COMMON}/DNFFileLog.cpp
        ${SVC_DIR}/DNFFloatingPointExceptSig.cpp
        ${COMMON}/DNFFunctionLib.cpp
        ${SVC_DIR}/DNFGameServer.cpp
        ${SVC_DIR}/DNFInnerMsgHandler.cpp
        ${SVC_DIR}/DNFKillUserConfig.cpp
        ${SVC_DIR}/DNFManagerServer.cpp
        ${SVC_DIR}/DNFMember.cpp
        ${SVC_DIR}/DNFMemberConfig.cpp
        ${SVC_DIR}/DNFMemberManager.cpp
        ${SVC_DIR}/DNFMonitorServerMain.cpp
        ${SVC_DIR}/DNFNetworkThread.cpp
        ${SVC_DIR}/DNFPacketBuffer.cpp
        ${SVC_DIR}/DNFPacketDecoder.cpp
        ${SVC_DIR}/DNFPacketTracer.cpp
        ${SVC_DIR}/DNFPacketTranslater.cpp
        ${SVC_DIR}/DNFProhibitUser.cpp
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
        ${SVC_DIR}/DNFTowerRank.cpp
        ${SVC_DIR}/DNFUdpHandler.cpp
        ${SVC_DIR}/DNFUser.cpp
        ${SVC_DIR}/DNFUser1Sig.cpp
        ${SVC_DIR}/DNFUserManager.cpp
        ${SVC_DIR}/EventActionManager.cpp
        ${SVC_DIR}/GMAccounts.cpp
        ${SVC_DIR}/IPCounter.cpp
        ${SVC_DIR}/ItemLimitEditionManager.cpp
        ${SVC_DIR}/LimitNpcBuyItem.cpp
        ${SVC_DIR}/LoginLogoutStatistics.cpp
        ${SVC_DIR}/MemoryCashManager.cpp
        ${SVC_DIR}/OnTimeEventManager.cpp
        ${SVC_DIR}/PacketCounter.cpp
        ${PACKET}/src/PacketHeader.cpp
        ${SVC_DIR}/Peer.cpp
        ${SVC_DIR}/Scheduler.cpp
        ${SVC_DIR}/ServerLoadChecker.cpp
        ${SVC_DIR}/ServerXml.cpp
        ${SVC_DIR}/SystemTimeHandler.cpp
        ${SVC_DIR}/TaskImpl.cpp
        ${SVC_DIR}/TaskScheduler.cpp
        ${SVC_DIR}/TcpNetSystem.cpp
        ${COMMON}/Thread.cpp
        ${SVC_DIR}/TowerOfDespairReloadAPC.cpp
        ${SVC_DIR}/VillageAttackedManager.cpp
        ${COMMON}/tinyxml.cpp
        ${COMMON}/tinyxmlerror.cpp
        ${COMMON}/tinyxmlparser.cpp
        ${SVC_DIR}/version.cpp
    INCLUDES
        "-I${COMMON}" "-I${SVC_DIR}" "-I${PACKET}/include"
        "-I${SHARED}" "-I${SHARED}/common/include"
    DEFINES
        -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS
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

target_compile_options(monitor PRIVATE -std=gnu++0x)
