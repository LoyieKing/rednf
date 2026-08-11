cmake_minimum_required(VERSION 3.13)
project(auction CXX)

# c6446r 变体：c6root g++ 4.4.7 驱动 + cc1plus 4.4.6（等价 /tmp/c6-g++-446r 的 -B）。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B ${DF_CC1P446}/")

set(DNF_SRC "${CMAKE_CURRENT_LIST_DIR}/../..")
include("${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake")

set(BASIC       "${DNF_SRC}/DNFServer/GameServer/ServerLab/ServerLib/basic_source")
set(COMMON      "${DNF_SRC}/DNFServer/GameServer/ServerLab/ServerLib/common_source")
set(AUCTIONCOMMON "${DNF_SRC}/DNFServer/GameServer/ServerLab/AppCommon/AuctionCommon")
set(SERVERCOMMON "${DNF_SRC}/DNFServer/ServerCommon")
set(DNFSSHARED  "${DNF_SRC}/DNFShared/GameScript")
set(CORE        "${DNF_SRC}/Library/Include/Core")
set(AUCTION     "${DNF_SRC}/DNFServer/GameServer/ServerLab/Auction/AuctionServer")
set(AUCTIONGD   "${AUCTION}/GameData")
set(AUCTIONAUCTION "${AUCTION}/GameData/auction")
set(AUCTIONDICT "${AUCTION}/GameData/auction/dictionary")
set(AUCTIONSRC  "${AUCTION}/auction_source")

# 源文件与顺序：build-auction.sh 的链接顺序 = `ls *.o`（按 .o 基名字母序，
# ServerCommon/Thread.cpp 以 ServerCommon_Thread.o 参与排序）。SRC_OPTS 复刻
# 逐文件 flags：tinyxml*（-O3 gnu++98）、CFileLogWriterBase（-fno-exceptions）、
# HandlerFor_GA_/HandlerFor_GP_JPN（-DSTATISTICS_STDATA_57）。
dnf_service(auction
    SOURCES
        ${COMMON}/ActiveNetClose.cpp
        ${AUCTIONAUCTION}/Auction.cpp
        ${AUCTIONAUCTION}/AuctionBot.cpp
        ${AUCTIONAUCTION}/AuctionDW.cpp
        ${AUCTIONDICT}/AuctionDictionary.cpp
        ${AUCTIONGD}/AuctionString.cpp
        ${AUCTIONDICT}/AveragePriceDictionary.cpp
        ${SERVERCOMMON}/CFileLogWriterBase.cpp
        ${AUCTIONGD}/Character.cpp
        ${AUCTIONDICT}/CharacterDictionary.cpp
        ${COMMON}/CommonDataPool.cpp
        ${COMMON}/DBConnection.cpp
        ${COMMON}/DBDispatcher.cpp
        ${SERVERCOMMON}/DNFFileLog.cpp
        ${SERVERCOMMON}/DNFFunctionLib.cpp
        ${AUCTIONGD}/DNFFunctionLibWrapper.cpp
        ${BASIC}/DataPool.cpp
        ${BASIC}/DefGlobal.cpp
        ${BASIC}/Exception.cpp
        ${AUCTIONDICT}/ExpireTimeDictionary.cpp
        ${AUCTIONGD}/GameDataPool.cpp
        ${AUCTIONSRC}/HandlerFor_DB_.cpp
        ${AUCTIONSRC}/HandlerFor_GA_.cpp
        ${AUCTIONSRC}/HandlerFor_GP_JPN.cpp
        ${AUCTIONSRC}/HandlerFor_TE_.cpp
        ${COMMON}/IArea.cpp
        ${COMMON}/IHandler.cpp
        ${COMMON}/InterDispatcher.cpp
        ${AUCTIONSRC}/InterHandler.cpp
        ${BASIC}/LinuxService.cpp
        ${COMMON}/LogSendThread.cpp
        ${BASIC}/Method.cpp
        ${COMMON}/NSLDBThread.cpp
        ${DNFSSHARED}/RDARScriptAvatarColorInfo.cpp
        ${DNFSSHARED}/RDARScriptItemInfo.cpp
        ${BASIC}/RecvBuffer.cpp
        ${AUCTIONDICT}/ReliabilityDictionary.cpp
        ${BASIC}/SHA.cpp
        ${BASIC}/Script.cpp
        ${AUCTIONDICT}/Search.cpp
        ${CORE}/SecureStdio.cpp
        ${SERVERCOMMON}/Thread.cpp
        ${AUCTIONGD}/ServerLibrary2.0.cpp
        ${SERVERCOMMON}/ServerXml.cpp
        ${COMMON}/ServiceError.cpp
        ${COMMON}/ServiceFactory.cpp
        ${BASIC}/Socket.cpp
        ${BASIC}/SocketSystem.cpp
        ${CORE}/StackBuffer.cpp
        ${AUCTIONGD}/StatisticsCollector.cpp
        ${CORE}/StringFormat.cpp
        ${CORE}/Strings.cpp
        ${BASIC}/System.cpp
        ${COMMON}/TActiveConnect.cpp
        ${COMMON}/TCPDispatcher.cpp
        ${COMMON}/TCPSendThread.cpp
        ${COMMON}/TCPThread.cpp
        ${BASIC}/TCPUser.cpp
        ${BASIC}/TEA.cpp
        ${AUCTIONSRC}/TeaInitialize.cpp
        ${BASIC}/Thread.cpp
        ${BASIC}/ThreadLock.cpp
        ${COMMON}/TimeManager.cpp
        ${COMMON}/TimerThread.cpp
        ${BASIC}/Token.cpp
        ${BASIC}/TraceLog.cpp
        ${CORE}/UnicodeConvert.cpp
        ${BASIC}/WorkThread.cpp
        ${AUCTIONGD}/Zone.cpp
        ${COMMON}/mysql_stubs.cpp
        ${SERVERCOMMON}/tinyxml.cpp
        ${SERVERCOMMON}/tinyxmlerror.cpp
        ${SERVERCOMMON}/tinyxmlparser.cpp
        ${COMMON}/tmp_stubs.cpp
        ${AUCTIONGD}/version.cpp
    INCLUDES
        -isystem${DNF_SRC}/Library3rd/Boost/Include
        -isystem${DNF_SRC}/Library3rd/MySQL/include
        -I${BASIC} -I${COMMON} -I${AUCTIONCOMMON} -I${SERVERCOMMON} -I${DNFSSHARED} -I${CORE}
        -I${AUCTIONGD} -I${AUCTIONAUCTION} -I${AUCTIONDICT} -I${AUCTIONSRC}
        -I${DNF_SRC}/shared -I${DNF_SRC}/shared/common/include -I${DNF_SRC}/shared/packet/include
    DEFINES
        -D_GNU_SOURCE -std=gnu++0x -pthread -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS -DDNF_SVC_AUCTION
    SRC_OPTS
        "${SERVERCOMMON}/tinyxml.cpp|-O3 -std=gnu++98 -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS"
        "${SERVERCOMMON}/tinyxmlerror.cpp|-O3 -std=gnu++98 -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS"
        "${SERVERCOMMON}/tinyxmlparser.cpp|-O3 -std=gnu++98 -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS"
        "${SERVERCOMMON}/CFileLogWriterBase.cpp|-fno-exceptions"
        "${AUCTIONSRC}/HandlerFor_GA_.cpp|-DSTATISTICS_STDATA_57"
        "${AUCTIONSRC}/HandlerFor_GP_JPN.cpp|-DSTATISTICS_STDATA_57"
    LINK_FLAGS
        -no-pie -static-libgcc -lpthread -ldl ${DF_ZLIB32}/lib/libz.a -lm
)

# __FILE__ 覆盖：复刻 ORIG 的 assert 路径字符串（见 auction_filepath_override.h）。
# 仅影响 auction 构建；point/其它服务不受影响。
set(_AUCTION_FILEPATH_H "${CMAKE_CURRENT_LIST_DIR}/auction_filepath_override.h")
set_source_files_properties(${CORE}/SecureStdio.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_AUCTION_FILEPATH_H};-DAUCTION_FILEPATH_SECURESTDIO")
set_source_files_properties(${BASIC}/RecvBuffer.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_AUCTION_FILEPATH_H};-DAUCTION_FILEPATH_RECVBUFFER")
set_source_files_properties(${COMMON}/CommonDataPool.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_AUCTION_FILEPATH_H};-DAUCTION_FILEPATH_COMMONDATAPOOL")
set_source_files_properties(${BASIC}/Token.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_AUCTION_FILEPATH_H};-DAUCTION_FILEPATH_TOKEN")
set_source_files_properties(${COMMON}/LogSendThread.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_AUCTION_FILEPATH_H};-DAUCTION_FILEPATH_LOGSENDTHREAD")

# 链接用宿主 g++（与 build-auction.sh 一致：-m32 -no-pie -static-libgcc）。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
set(CMAKE_CXX_LINK_EXECUTABLE "${DNF_HOST_GXX} -m32 -no-pie <OBJECTS> -o <TARGET> <LINK_FLAGS> <LINK_LIBRARIES>")
