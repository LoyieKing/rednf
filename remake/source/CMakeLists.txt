cmake_minimum_required(VERSION 3.10)

# ============================================================
# Original build environment:
#   Compiler: GNU C++ 4.1.2 20080704 (Red Hat 4.1.2-52)
#   Target:   x86_64-redhat-linux
#   Standard: C++98
#   Target mode: 32-bit
#   Optimization: O0
# ============================================================

project(DNFServer C CXX)
message(STATUS "Using system compiler")

# --- C++ standard matching original build ---
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)  # GNU extensions were used

# --- Global compiler/linker flags ---
# Rebuild policy: force 32-bit + O0 for all targets/configurations.
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32 -pthread -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -pthread -O0")
set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -m32 -pthread")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32 -pthread")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -m32 -pthread")

# Keep all CMake build types on O0 to avoid accidental optimization changes.
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O0")
set(CMAKE_C_FLAGS_MINSIZEREL "-O0")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-O0")

# Default to Debug to preserve symbols during reverse-aligned rebuilds.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
# === df_auction_r ===
set(DF_AUCTION_R_SOURCES
    DNFServer/GameServer/ServerLab/AppCommon/AuctionCommon/ServiceError.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/Character.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/DNFFunctionLibWrapper.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/GameDataPool.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/ServerLibrary2.0.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/StatisticsCollector.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/Zone.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/Auction.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/AuctionBot.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/AuctionDW.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/AuctionDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/AuctionString.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/AveragePriceDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/CharacterDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/ExpireTimeDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/ReliabilityDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/Search.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/version.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/HandlerFor_DB_.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/HandlerFor_GA_.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/HandlerFor_GP_JPN.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/HandlerFor_TE_.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/InterHandler.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/TeaInitialize.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/DataPool.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/DefGlobal.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Exception.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/LinuxService.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Method.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/RecvBuffer.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/SHA.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Script.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Socket.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/SocketSystem.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/System.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/TCPUser.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/TEA.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Thread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/ThreadLock.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Token.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/TraceLog.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/CommonDataPool.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/DBConnection.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/DBDispatcher.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/IArea.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/IHandler.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/InterDispatcher.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/LogSendThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/NSLDBThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/ServiceError.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/ServiceFactory.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TActiveConnect.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TCPDispatcher.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TCPSendThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TCPThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TimeManager.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TimerThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/WorkThread.cpp
    DNFServer/ServerCommon/DNFFileLog.cpp
    DNFServer/ServerCommon/DNFFunctionLib.cpp
    DNFServer/ServerCommon/ServerXml.cpp
    DNFShared/GameScript/RDARScriptAvatarColorInfo.cpp
    DNFShared/GameScript/RDARScriptItemInfo.cpp
    Library/Include/Core/SecureStdio.cpp
    Library/Include/Core/Strings.cpp
    Library/Include/Core/UnicodeConvert.cpp
)
add_executable(df_auction_r ${DF_AUCTION_R_SOURCES})

# === df_bridge_r ===
set(DF_BRIDGE_R_SOURCES
    ChannelOld/DNFChannelBridge/Authenticator.cpp
    ChannelOld/DNFChannelBridge/ChannelService.cpp
    ChannelOld/DNFChannelBridge/CheckThread.cpp
    ChannelOld/DNFChannelBridge/CommandLineParser.cpp
    ChannelOld/DNFChannelBridge/DBMgr.cpp
    ChannelOld/DNFChannelBridge/DNFChannelBridge.cpp
    ChannelOld/DNFChannelBridge/DNFServerCommon/DNFFunctionLib.cpp
    ChannelOld/DNFChannelBridge/Exception.cpp
    ChannelOld/DNFChannelBridge/Globals.cpp
    ChannelOld/DNFChannelBridge/LinuxService.cpp
    ChannelOld/DNFChannelBridge/PIDHelper.cpp
    ChannelOld/DNFChannelBridge/Script.cpp
    ChannelOld/DNFChannelBridge/ScriptData.cpp
    ChannelOld/DNFChannelBridge/ScriptRawData.cpp
    ChannelOld/DNFChannelBridge/ScriptThread.cpp
    ChannelOld/DNFChannelBridge/Service.cpp
    ChannelOld/DNFChannelBridge/SignalHandler.cpp
    ChannelOld/DNFChannelBridge/Socket.cpp
    ChannelOld/DNFChannelBridge/SocketSystem.cpp
    ChannelOld/DNFChannelBridge/System.cpp
    ChannelOld/DNFChannelBridge/TCPAcceptThread.cpp
    ChannelOld/DNFChannelBridge/TCPHandler.cpp
    ChannelOld/DNFChannelBridge/TCPThread.cpp
    ChannelOld/DNFChannelBridge/TCPUser.cpp
    ChannelOld/DNFChannelBridge/Thread.cpp
    ChannelOld/DNFChannelBridge/ThreadLock.cpp
    ChannelOld/DNFChannelBridge/Token.cpp
    ChannelOld/DNFChannelBridge/UDPHandler.cpp
    ChannelOld/DNFChannelBridge/UDPThread.cpp
    ChannelOld/DNFChannelBridge/UniqueIndex.cpp
    ChannelOld/DNFChannelBridge/stdafx.cpp
)
add_executable(df_bridge_r ${DF_BRIDGE_R_SOURCES})

# === df_channel_r ===
set(DF_CHANNEL_R_SOURCES
    ChannelOld/DNFChannelServer/ChannelService.cpp
    ChannelOld/DNFChannelServer/CheckThread.cpp
    ChannelOld/DNFChannelServer/CommandLineParser.cpp
    ChannelOld/DNFChannelServer/DNFChannelServer.cpp
    ChannelOld/DNFChannelServer/Exception.cpp
    ChannelOld/DNFChannelServer/Globals.cpp
    ChannelOld/DNFChannelServer/LinuxService.cpp
    ChannelOld/DNFChannelServer/Method.cpp
    ChannelOld/DNFChannelServer/PIDHelper.cpp
    ChannelOld/DNFChannelServer/Reactor.cpp
    ChannelOld/DNFChannelServer/Rijndael.cpp
    ChannelOld/DNFChannelServer/SHA.cpp
    ChannelOld/DNFChannelServer/Script.cpp
    ChannelOld/DNFChannelServer/ScriptData.cpp
    ChannelOld/DNFChannelServer/ScriptRawData.cpp
    ChannelOld/DNFChannelServer/Service.cpp
    ChannelOld/DNFChannelServer/SignalHandler.cpp
    ChannelOld/DNFChannelServer/Socket.cpp
    ChannelOld/DNFChannelServer/SocketSystem.cpp
    ChannelOld/DNFChannelServer/System.cpp
    ChannelOld/DNFChannelServer/TCPAcceptThread.cpp
    ChannelOld/DNFChannelServer/TCPHandler.cpp
    ChannelOld/DNFChannelServer/TCPThread.cpp
    ChannelOld/DNFChannelServer/TCPUser.cpp
    ChannelOld/DNFChannelServer/Thread.cpp
    ChannelOld/DNFChannelServer/ThreadLock.cpp
    ChannelOld/DNFChannelServer/Token.cpp
    ChannelOld/DNFChannelServer/UDPHandler.cpp
    ChannelOld/DNFChannelServer/UDPThread.cpp
    ChannelOld/DNFChannelServer/UDPUser.cpp
    ChannelOld/DNFChannelServer/UniqueIndex.cpp
    ChannelOld/DNFChannelServer/stdafx.cpp
)
add_executable(df_channel_r ${DF_CHANNEL_R_SOURCES})

# === df_community_r ===
# No DWARF info - reverse engineered from Ghidra decompilation (df_playground)
set(DF_COMMUNITY_R_SOURCES
    Community/src/AbstractSocket.cpp
    Community/src/ArchiveLog.cpp
    Community/src/BuddyManager.cpp
    Community/src/ConfigFileReader.cpp
    Community/src/CoreDump.cpp
    Community/src/DateTime.cpp
    Community/src/Epoll.cpp
    Community/src/FindBuddy.cpp
    Community/src/FindUser.cpp
    Community/src/ISessionManager.cpp
    Community/src/NetworkSession.cpp
    Community/src/PacketDispatcher.cpp
    Community/src/ProcessManager.cpp
    Community/src/SessionManager.cpp
    Community/src/SessionProxy.cpp
    Community/src/User.cpp
    Community/src/UserManager.cpp
    Community/src/global.cpp
    Community/src/main.cpp
    shared/packet/src/PacketHeader.cpp
    shared/packet/src/STGameUserInfo.cpp
    shared/packet/src/STPvPBuddyDBInfo.cpp
)
add_executable(df_community_r ${DF_COMMUNITY_R_SOURCES})
target_include_directories(df_community_r PRIVATE
    Community/src
    shared/common/include
    shared/packet/include
)

# === df_coserver_r ===
# No DWARF info available - placeholder
# add_executable(df_coserver_r placeholder/df_coserver_r.cpp)

# === df_dbmw_r ===
set(DF_DBMW_R_SOURCES
    Library/Include/Core/StackBuffer.cpp
    Library/Include/Core/StringFormat.cpp
)
add_executable(df_dbmw_r ${DF_DBMW_R_SOURCES})

# === df_guild_r ===
# No DWARF info available - placeholder
# add_executable(df_guild_r placeholder/df_guild_r.cpp)

# === df_manager_r ===
# No DWARF info available - placeholder
# add_executable(df_manager_r placeholder/df_manager_r.cpp)

# === df_monitor_r ===
# No DWARF info available - placeholder
# add_executable(df_monitor_r placeholder/df_monitor_r.cpp)

# === df_point_r ===
set(DF_POINT_R_SOURCES
    DNFServer/GameServer/ServerLab/AppCommon/AuctionCommon/ServiceError.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/Character.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/DNFFunctionLibWrapper.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/GameDataPool.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/ServerLibrary2.0.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/StatisticsCollector.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/Zone.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/Auction.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/AuctionBot.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/AuctionDW.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/AuctionDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/AuctionString.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/AveragePriceDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/CharacterDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/ExpireTimeDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/ReliabilityDictionary.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/auction/dictionary/Search.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/GameData/version.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/HandlerFor_DB_.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/HandlerFor_GA_.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/HandlerFor_GP_JPN.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/HandlerFor_TE_.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/InterHandler.cpp
    DNFServer/GameServer/ServerLab/Auction/AuctionServer/auction_source/TeaInitialize.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/DataPool.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/DefGlobal.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Exception.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/LinuxService.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Method.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/RecvBuffer.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/SHA.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Script.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Socket.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/SocketSystem.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/System.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/TCPUser.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/TEA.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Thread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/ThreadLock.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/Token.cpp
    DNFServer/GameServer/ServerLab/ServerLib/basic_source/TraceLog.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/CommonDataPool.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/DBConnection.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/DBDispatcher.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/IArea.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/IHandler.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/InterDispatcher.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/LogSendThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/NSLDBThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/ServiceError.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/ServiceFactory.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TActiveConnect.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TCPDispatcher.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TCPSendThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TCPThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TimeManager.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/TimerThread.cpp
    DNFServer/GameServer/ServerLab/ServerLib/common_source/WorkThread.cpp
    DNFServer/ServerCommon/DNFFileLog.cpp
    DNFServer/ServerCommon/DNFFunctionLib.cpp
    DNFServer/ServerCommon/ServerXml.cpp
    DNFShared/GameScript/RDARScriptAvatarColorInfo.cpp
    DNFShared/GameScript/RDARScriptItemInfo.cpp
    Library/Include/Core/SecureStdio.cpp
    Library/Include/Core/Strings.cpp
    Library/Include/Core/UnicodeConvert.cpp
)
add_executable(df_point_r ${DF_POINT_R_SOURCES})

# === df_relay_r ===
# No DWARF info available - placeholder
# add_executable(df_relay_r placeholder/df_relay_r.cpp)

# === df_statics_r ===
# No DWARF info available - placeholder
# add_executable(df_statics_r placeholder/df_statics_r.cpp)

# === df_stun_r ===
set(DF_STUN_R_SOURCES
    DNFServer/StunServer/stun_server/global_func.cpp
    DNFServer/StunServer/stun_server/server.cpp
    DNFServer/StunServer/stun_server/stun.cpp
    DNFServer/StunServer/stun_server/udp.cpp
)
add_executable(df_stun_r ${DF_STUN_R_SOURCES})
