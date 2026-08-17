cmake_minimum_required(VERSION 3.10)
project(game CXX)

# ============================================================
# df_game_r CMake 构建（2026-08-16 起，NSL 基础库 + ServerCommon 起步）
# 口径沿用 guild（c6root 4.4.x 驱动 + c5 库），逐类 identical 调优见
# docs/df_game_r_class_topology.md 与 docs/class_func_reports/。
# 本服务含 yaSSL/TaoCrypt/MySQL（Library3rd/MySQL 自带源码）。
# ============================================================
set(CMAKE_CXX_LINK_EXECUTABLE "/usr/bin/g++ <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake)

set(SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(SVC_DIR ${SRC_ROOT}/DNFServer/GameServer/Game)
set(SERVERLIB ${SRC_ROOT}/DNFServer/GameServer/ServerLab/ServerLib)
set(AUCTION ${SRC_ROOT}/DNFServer/GameServer/ServerLab/Auction/AuctionServer)
set(COMMON ${SRC_ROOT}/DNFServer/ServerCommon)
set(PACKET ${SRC_ROOT}/shared/packet)
set(SHARED ${SRC_ROOT}/shared)
set(THIRD ${SRC_ROOT}/Library3rd)

dnf_service(game
    SOURCES
        # ---- NSL 基础库（ServerLib basic_source）----
        ${SERVERLIB}/basic_source/DataPool.cpp
        ${SERVERLIB}/basic_source/DefGlobal.cpp
        ${SERVERLIB}/basic_source/Exception.cpp
        ${SERVERLIB}/basic_source/LinuxService.cpp
        ${SERVERLIB}/basic_source/Method.cpp
        ${SERVERLIB}/basic_source/RecvBuffer.cpp
        ${SERVERLIB}/basic_source/SHA.cpp
        ${SERVERLIB}/basic_source/Script.cpp
        ${SERVERLIB}/basic_source/Socket.cpp
        ${SERVERLIB}/basic_source/SocketSystem.cpp
        ${SERVERLIB}/basic_source/System.cpp
        ${SERVERLIB}/basic_source/TCPUser.cpp
        ${SERVERLIB}/basic_source/TEA.cpp
        ${SERVERLIB}/basic_source/Thread.cpp
        ${SERVERLIB}/basic_source/ThreadLock.cpp
        ${SERVERLIB}/basic_source/Token.cpp
        ${SERVERLIB}/basic_source/TraceLog.cpp
        ${SERVERLIB}/basic_source/WorkThread.cpp
        # ---- NSL 基础库（ServerLib common_source）----
        ${SERVERLIB}/common_source/ActiveNetClose.cpp
        ${SERVERLIB}/common_source/CommonDataPool.cpp
        ${SERVERLIB}/common_source/DBConnection.cpp
        ${SERVERLIB}/common_source/DBDispatcher.cpp
        ${SERVERLIB}/common_source/IArea.cpp
        ${SERVERLIB}/common_source/IHandler.cpp
        ${SERVERLIB}/common_source/InterDispatcher.cpp
        ${SERVERLIB}/common_source/LogSendThread.cpp
        ${SERVERLIB}/common_source/NSLDBThread.cpp
        ${SERVERLIB}/common_source/ServiceError.cpp
        ${SERVERLIB}/common_source/ServiceFactory.cpp
        ${SERVERLIB}/common_source/Socket.cpp
        ${SERVERLIB}/common_source/TActiveConnect.cpp
        ${SERVERLIB}/common_source/TCPDispatcher.cpp
        ${SERVERLIB}/common_source/TCPSendThread.cpp
        ${SERVERLIB}/common_source/TCPThread.cpp
        ${SERVERLIB}/common_source/TimeManager.cpp
        ${SERVERLIB}/common_source/TimerThread.cpp
        ${SERVERLIB}/common_source/mysql_stubs.cpp
        ${SERVERLIB}/common_source/tmp_stubs.cpp
        # ---- ServerCommon ----
        ${COMMON}/CFileLogWriterBase.cpp
        ${COMMON}/DNFFileLog.cpp
        ${COMMON}/DNFFunctionLib.cpp
        ${COMMON}/ServerXml.cpp
        ${COMMON}/Thread.cpp
        ${COMMON}/tinyxml.cpp
        ${COMMON}/tinyxmlerror.cpp
        ${COMMON}/tinyxmlparser.cpp
        # ---- Game 入口（最小桩，翻译 DNFApplication 时替换）----
        ${SVC_DIR}/GameMain.cpp
        # ---- Game 底层类（第二层，2026-08-16）----
        ${SVC_DIR}/Stream.cpp
        ${SVC_DIR}/StreamPool.cpp
        ${SVC_DIR}/CStreamGuard.cpp
        ${SVC_DIR}/PacketBuf.cpp
        ${SVC_DIR}/InterfacePacketBuf.cpp
        ${SVC_DIR}/CEnvironment.cpp
        ${SVC_DIR}/CRijndael.cpp
        ${SVC_DIR}/Cipher.cpp
        ${SVC_DIR}/CNCrypto.cpp
        ${SVC_DIR}/CNCryptoAlgo.cpp
        ${SVC_DIR}/Encryption.cpp
        ${SVC_DIR}/MsgQueueMgr.cpp
        ${SVC_DIR}/TimerQueue.cpp
        ${SVC_DIR}/EpollHandler.cpp
        ${SVC_DIR}/CTEA.cpp
        ${SVC_DIR}/LogManager.cpp
        ${SVC_DIR}/GlobalData.cpp
        ${SVC_DIR}/CSystemTime.cpp
        ${SVC_DIR}/CNetwork.cpp
        ${SVC_DIR}/CDataManager.cpp
        ${SVC_DIR}/MySQL.cpp
        ${SVC_DIR}/DBMgr.cpp
        ${SVC_DIR}/CQueryCounter.cpp
        ${SVC_DIR}/BaseServerProxy.cpp
        ${SVC_DIR}/CHadesServerProxy.cpp
        ${SVC_DIR}/CGuildServerProxy.cpp
        ${SVC_DIR}/CMonitorServerProxy.cpp
        ${SVC_DIR}/Cipher.cpp
        ${SVC_DIR}/CRijndael.cpp
        ${SVC_DIR}/CNCrypto.cpp
        ${SVC_DIR}/CNCryptoAlgo.cpp
        ${SVC_DIR}/Encryption.cpp
        ${SVC_DIR}/CUdpHandler.cpp
        ${SVC_DIR}/STItemScript.cpp
        ${SVC_DIR}/Inven_Item.cpp
        ${SVC_DIR}/CItemAmplifier.cpp
        ${SVC_DIR}/CItem.cpp
        ${SVC_DIR}/CEquipItem.cpp
        ${SVC_DIR}/CStackableItem.cpp
        ${SVC_DIR}/CItemList.cpp
        ${SVC_DIR}/CInventory.cpp
        ${SVC_DIR}/CSkill.cpp
        ${SVC_DIR}/SkillSlot.cpp
        ${SVC_DIR}/CUserCharacInfo.cpp
        ${SVC_DIR}/ComboSkill.cpp
        ${SVC_DIR}/CExpandEquipslot.cpp
        ${SVC_DIR}/CUser.cpp
        ${SVC_DIR}/CParty.cpp
        ${SVC_DIR}/CDungeon.cpp
        ${SVC_DIR}/CDungeonClearTracer.cpp
        ${SVC_DIR}/CGameManager.cpp
        ${SVC_DIR}/PvP_Room.cpp
        ${SVC_DIR}/WarRoom.cpp
        ${SVC_DIR}/BlueMarble.cpp
        ${SVC_DIR}/CBattle_Field.cpp
        ${SVC_DIR}/GameWorld.cpp
        ${SVC_DIR}/CRelayBattleMgr.cpp
        ${SVC_DIR}/CTradeSpace.cpp
        ${SVC_DIR}/GameResultSet.cpp
        ${SVC_DIR}/GameResultType.cpp
        ${SVC_DIR}/WarField.cpp
        ${SVC_DIR}/BlueMarbleInfoScript.cpp
        ${SVC_DIR}/CAuctionAveragePrice.cpp
        ${SVC_DIR}/CDisconnectDetecter.cpp
        ${SVC_DIR}/CDoubleConnCheckServerProxy.cpp
        ${SVC_DIR}/CEventCharacterHandler.cpp
        ${SVC_DIR}/CGuildAgitManager.cpp
        ${SVC_DIR}/CPremiumLetheManager.cpp
        ${SVC_DIR}/CSchoolMgr.cpp
        ${SVC_DIR}/Area.cpp
        ${SVC_DIR}/BattleData.cpp
        ${SVC_DIR}/BlueMarbleMapScript.cpp
        ${SVC_DIR}/BlueMarbleUserInfo.cpp
        ${SVC_DIR}/CCommonStatisticsMgr.cpp
        ${SVC_DIR}/CDeathMatchBattleMgr.cpp
        ${SVC_DIR}/CMTRand.cpp
        ${SVC_DIR}/CMission.cpp
        ${SVC_DIR}/CMissionList_Charac.cpp
        ${SVC_DIR}/CTitleBook.cpp
        ${SVC_DIR}/Quest.cpp
        ${SVC_DIR}/CPartyResultRecvFlag.cpp
        ${SVC_DIR}/CPartyTelePort.cpp
        ${SVC_DIR}/CSharedServerMessageManager.cpp
        ${SVC_DIR}/CSwitchLog.cpp
        ${SVC_DIR}/CTraceMobDieHack.cpp
        ${SVC_DIR}/CUserGlobalInfoHandle.cpp
        ${SVC_DIR}/CUserProc.cpp
        ${SVC_DIR}/HistoryLog.cpp
        ${SVC_DIR}/MapInfo.cpp
        ${SVC_DIR}/MazeScript.cpp
        ${SVC_DIR}/PvP_GuildWar_Log.cpp
        ${SVC_DIR}/PvpResultRecvFlag.cpp
        ${SVC_DIR}/PvpResultSet.cpp
        ${SVC_DIR}/PvpResultType.cpp
        ${SVC_DIR}/PvpUserTable.cpp
        ${SVC_DIR}/SECRET_SHOP_DATA.cpp
        ${SVC_DIR}/Secu_HackLogCheckByParty.cpp
        ${SVC_DIR}/Statistics.cpp
        ${SVC_DIR}/StatisticsNatType.cpp
        ${SVC_DIR}/StatisticsPvP.cpp
        ${SVC_DIR}/StatisticsShusiaService.cpp
        ${SVC_DIR}/StatisticsToObserveCrash.cpp
        ${SVC_DIR}/TournamentMonster.cpp
        ${SVC_DIR}/UserQuest.cpp
        ${SVC_DIR}/Village.cpp
        ${SVC_DIR}/CAccountCargo.cpp
        ${SVC_DIR}/CAchievement.cpp
        ${SVC_DIR}/CAuction.cpp
        ${SVC_DIR}/CCargo.cpp
        ${SVC_DIR}/CCeraShopAddRestrict.cpp
        ${SVC_DIR}/CCeraShopGoods.cpp
        ${SVC_DIR}/CGuildWarEvent.cpp
        ${SVC_DIR}/CItemShop.cpp
        ${SVC_DIR}/CSecretShop.cpp
        ${SVC_DIR}/EquipmentParameterInfo.cpp
        ${SVC_DIR}/RandomOptionScript.cpp
        ${SVC_DIR}/STDungeonScript.cpp
        ${SVC_DIR}/STEquipmentScript.cpp
        ${SVC_DIR}/STMapScript.cpp
        ${SVC_DIR}/STMonsterScript.cpp
        ${SVC_DIR}/STQuestScript.cpp
        ${SVC_DIR}/STSkillScript.cpp
        ${SVC_DIR}/STStackableScript.cpp
        ${SVC_DIR}/ServerParameterScript.cpp
        ${SVC_DIR}/CGameOption.cpp
        ${SVC_DIR}/CBoosterGage.cpp
        ${SVC_DIR}/CListTables.cpp
        ${SVC_DIR}/AvatarCoin.cpp
        ${SVC_DIR}/Arad_AvatarRoulette.cpp
        ${SVC_DIR}/CDungeonGainedGold.cpp
        ${SVC_DIR}/GameStubs.cpp
    INCLUDES
        "-I${SERVERLIB}/basic_header" "-I${SERVERLIB}/common_header"
        "-I${AUCTION}/GameData" "-I${AUCTION}/GameData/auction"
        "-I${AUCTION}/auction_header"
        "-I${COMMON}" "-I${PACKET}/include"
        "-I${SHARED}" "-I${SHARED}/common/include"
        "-I${THIRD}/MySQL/include"
        "-I${THIRD}/MySQL/extlib/yassl/include"
        "-I${THIRD}/MySQL/extlib/yassl/taocrypt/include"
        "-I${THIRD}/MySQL/extlib/yassl/taocrypt/mySTL"
        "-I${THIRD}/Boost/Include"
    DEFINES
        -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS
    SRC_OPTS
        "${COMMON}/CFileLogWriterBase.cpp|-fno-exceptions"
        "${SVC_DIR}/PacketBuf.cpp|-fno-exceptions"
        "${COMMON}/tinyxml.cpp|-O3 -std=gnu++98"
        "${COMMON}/tinyxmlerror.cpp|-O3 -std=gnu++98"
        "${COMMON}/tinyxmlparser.cpp|-O3 -std=gnu++98"
    LIBS
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libgcc_eh.a
        ${DF_C5R52I}/usr/lib/gcc/i386-redhat-linux/4.1.1/libstdc++.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc_eh.a
        /home/loyieking/dnf_workspace/dnf_decompile/toolchains/zlib32/lib/libz.a
        /home/loyieking/dnf_workspace/dnf_decompile/build/manager/mysqlclient/libmysqlclient.a
        /home/loyieking/dnf_workspace/dnf_decompile/build/manager/yassl/libyassl.a
    LINK_FLAGS
        -m32 -no-pie -lpthread -ldl -lm
)

target_compile_options(game PRIVATE -std=gnu++0x)
