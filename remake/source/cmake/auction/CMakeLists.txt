cmake_minimum_required(VERSION 3.13)
project(auction CXX)

# c6446r 变体：c6root g++ 4.4.7 驱动 + cc1plus 4.4.6（等价 /tmp/c6-g++-446r 的 -B）。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B ${DF_CC1P446}/")

set(DNF_SRC "${CMAKE_CURRENT_LIST_DIR}/../..")
include("${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake")

set(BASIC       "${DNF_SRC}/DNFServer/GameServer/ServerLab/ServerLib/basic_source")
set(BASIC_HDR   "${DNF_SRC}/DNFServer/GameServer/ServerLab/ServerLib/basic_header")
set(COMMON      "${DNF_SRC}/DNFServer/GameServer/ServerLab/ServerLib/common_source")
set(COMMON_HDR  "${DNF_SRC}/DNFServer/GameServer/ServerLab/ServerLib/common_header")
set(AUCTIONCOMMON "${DNF_SRC}/DNFServer/GameServer/ServerLab/AppCommon/AuctionCommon")
set(SERVERCOMMON "${DNF_SRC}/DNFServer/ServerCommon")
set(DNFSSHARED  "${DNF_SRC}/DNFShared/GameScript")
set(CORE        "${DNF_SRC}/Library/Include/Core")
set(AUCTION     "${DNF_SRC}/DNFServer/GameServer/ServerLab/Auction/AuctionServer")
set(AUCTIONGD   "${AUCTION}/GameData")
set(AUCTIONAUCTION "${AUCTION}/GameData/auction")
set(AUCTIONDICT "${AUCTION}/GameData/auction/dictionary")
set(AUCTIONSRC  "${AUCTION}/auction_source")
set(AUCTIONHDR  "${AUCTION}/auction_header")

# ---- MySQL Connector/C 6.0.2 客户端（与 df_manager_r 同一套源码/产物）----
# ORIG 依据（/tmp/orig_svc/df_auction_r）：
#   nm | grep -cE ' T mysql_' = 110，' U mysql_' = 0 ⇒ ORIG 不是靠 weak 空桩，
#   而是把 MySQL 客户端**静态链入**；yaSSL/TaoCrypt 的导出符号面（372 个 T）
#   与 df_manager_r 逐名一致 ⇒ 同一份 yaSSL 源码编入。
# 故删除已不存在的 common_source/mysql_stubs.cpp（weak 空桩，提交 734cf82cf 已删），
# 改为在 SOURCES 里直接列入 ${MYSQLCLIENT_A} 与 yaSSL 源码（照 df_manager_r 写法）。
set(MYSQL      "${DNF_SRC}/Library3rd/MySQL")
set(YASSL      "${MYSQL}/extlib/yassl")
set(MYSQLCLIENT_DIR "${DNF_SRC}/../build/manager/mysqlclient")
set(MYSQLCLIENT_A   "${MYSQLCLIENT_DIR}/libmysqlclient.a")
set(MYSQLCLIENT_TREES "${MYSQLCLIENT_DIR}/trees.o")
if(NOT EXISTS "${MYSQLCLIENT_A}" OR NOT EXISTS "${MYSQLCLIENT_TREES}")
    message(STATUS "libmysqlclient.a 缺失，调用 source/toolchain/build-mysqlclient.sh 生成…")
    execute_process(COMMAND sh ${DNF_SRC}/toolchain/build-mysqlclient.sh
        WORKING_DIRECTORY ${DNF_SRC}/toolchain RESULT_VARIABLE _mysqlclient_rc)
    if(NOT _mysqlclient_rc EQUAL 0)
        message(FATAL_ERROR "build-mysqlclient.sh 失败 rc=${_mysqlclient_rc}")
    endif()
endif()

# yaSSL / TaoCrypt 源码清单（名序与 df_manager_r 的 TAOCRYPT_SRC / YASSL_SRC 一致）
set(YASSL_TAOCRYPT aes aestables algebra arc4 asn coding des dh dsa file hash integer
    md2 md4 md5 misc random ripemd rsa sha block_instnt)
set(YASSL_CORE buffer cert_wrapper crypto_wrapper handshake lock log socket_wrapper
    ssl timer yassl_error yassl_imp yassl_int)
set(YASSL_SOURCES "")
foreach(f IN LISTS YASSL_TAOCRYPT)
    list(APPEND YASSL_SOURCES ${YASSL}/taocrypt/src/${f}.cpp)
endforeach()
foreach(f IN LISTS YASSL_CORE)
    list(APPEND YASSL_SOURCES ${YASSL}/src/${f}.cpp)
endforeach()

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
        # mysql_stubs.cpp（weak 空桩）已随 734cf82cf 删除 → 换成真正的 MySQL 客户端
        # 静态库（见下方 LIBS 的 ${MYSQLCLIENT_A}）+ yaSSL/TaoCrypt 源码。
        ${YASSL_SOURCES}
        ${SERVERCOMMON}/tinyxml.cpp
        ${SERVERCOMMON}/tinyxmlerror.cpp
        ${SERVERCOMMON}/tinyxmlparser.cpp
        ${AUCTIONGD}/version.cpp
        # mysqlclient 的 zlib trees.o（位于 libmysqlclient.a 之前，同 df_manager_r）
        ${MYSQLCLIENT_TREES}
    INCLUDES
        -isystem${DNF_SRC}/Library3rd/Boost/Include
        -isystem${DNF_SRC}/Library3rd/MySQL/include
        -I${BASIC_HDR} -I${COMMON_HDR} -I${BASIC} -I${COMMON} -I${AUCTIONHDR} -I${AUCTIONCOMMON} -I${SERVERCOMMON} -I${DNFSSHARED} -I${CORE}
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
    # MySQL Connector/C 6.0.2 静态库（ORIG df_auction_r：T mysql_ = 110、U mysql_ = 0）。
    # libstdc++ 保持动态（ORIG df_auction_r 的 NEEDED 含 libstdc++.so.6），
    # 与 df_manager_r 的写法不同：manager 静态链 libstdc++。
    LIBS
        ${MYSQLCLIENT_A}
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

# ---- yaSSL / TaoCrypt 逐文件 flags + 专属 include（照 df_manager_r 原样）----
# ORIG 的 yaSSL 即用这套口径编出（-O3 -fPIC -DNDEBUG -std=gnu++98 + yassl define）。
set(YASSL_OPTS -O3 -fPIC -DNDEBUG -std=gnu++98 -DHAVE_CONFIG_H -DYASSL_PREFIX -D_LIB -DHAVE_YASSL -DYASSL_PURE_C -DHAVE_OPENSSL)
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

# 链接用宿主 g++（与 build-auction.sh 一致：-m32 -no-pie -static-libgcc）。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
set(CMAKE_CXX_LINK_EXECUTABLE "${DNF_HOST_GXX} -m32 -no-pie <OBJECTS> -o <TARGET> <LINK_FLAGS> <LINK_LIBRARIES>")
