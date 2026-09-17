cmake_minimum_required(VERSION 3.13)
project(point CXX)

# c6446r 变体：c6root g++ 4.4.7 驱动 + cc1plus 4.4.6（等价 /tmp/c6-g++-446r 的 -B）。
if(decompile_verify)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -B ${DF_CC1P446}/")
endif()

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
# ORIG 依据（/tmp/orig_svc/df_point_r，与 df_auction_r 同源同形）：
#   nm | grep -cE ' T mysql_' = 110，' U mysql_' = 0 ⇒ ORIG 把 MySQL 客户端**静态链入**；
#   yaSSL/TaoCrypt 导出符号面（372 个 T）与 df_manager_r 逐名一致 ⇒ 同一份 yaSSL 源码。
# 故删除已不存在的 common_source/mysql_stubs.cpp（weak 空桩，提交 734cf82cf 已删），
# 改为列入 yaSSL 源码（SOURCES）+ libmysqlclient.a（LIBS），照 df_manager_r 写法。
set(MYSQL      "${DNF_SRC}/Library3rd/MySQL")
set(YASSL      "${MYSQL}/extlib/yassl")
set(MYSQLCLIENT_DIR "${DNF_SRC}/../build/manager/mysqlclient")
set(MYSQLCLIENT_A   "${MYSQLCLIENT_DIR}/libmysqlclient.a")
if(NOT EXISTS "${MYSQLCLIENT_A}")
    message(FATAL_ERROR
        "缺少 ${MYSQLCLIENT_A}\n"
        "三方库由 source/cmake/third_party 统一构建，请改用统一入口：\n"
        "  cmake -S source -B build && cmake --build build --target third_party")
endif()
# 注：不像 df_manager_r 那样显式列入 mysqlclient/trees.o —— ORIG df_point_r 的 zlib
# 是**动态** libz.so.1（readelf NEEDED 含 libz.so.1；nm 只有 `U compress`/
# `U uncompress`，无任何 zlib 的 T 符号），显式链 trees.o 会平白多出 5 个 ORIG
# 没有的静态符号。libmysqlclient.a 内部仍含 trees.o，将来启用压缩会自动拉入。

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

# 与 auction 同源，唯一差别：-DPOINT_SERVER（build-point.sh COMMON_FLAGS/TINYXML_FLAGS）。
dnf_service(point
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
    INCLUDES
        -isystem${DNF_SRC}/Library3rd/Boost/Include
        -isystem${DNF_SRC}/Library3rd/MySQL/include
        -I${BASIC_HDR} -I${COMMON_HDR} -I${BASIC} -I${COMMON} -I${AUCTIONHDR} -I${AUCTIONCOMMON} -I${SERVERCOMMON} -I${DNFSSHARED} -I${CORE}
        -I${AUCTIONGD} -I${AUCTIONAUCTION} -I${AUCTIONDICT} -I${AUCTIONSRC}
        -I${DNF_SRC}/shared -I${DNF_SRC}/shared/common/include -I${DNF_SRC}/shared/packet/include
    DEFINES
        -D_GNU_SOURCE -DPOINT_SERVER -std=gnu++0x -pthread -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS -DDNF_SVC_POINT
    SRC_OPTS
        "${SERVERCOMMON}/tinyxml.cpp|-O3 -std=gnu++98 -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS"
        "${SERVERCOMMON}/tinyxmlerror.cpp|-O3 -std=gnu++98 -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS"
        "${SERVERCOMMON}/tinyxmlparser.cpp|-O3 -std=gnu++98 -DTIXML_USE_STL -DBOOST_DISABLE_ASSERTS"
        "${SERVERCOMMON}/CFileLogWriterBase.cpp|-fno-exceptions"
        "${AUCTIONSRC}/HandlerFor_GA_.cpp|-DSTATISTICS_STDATA_57"
        "${AUCTIONSRC}/HandlerFor_GP_JPN.cpp|-DSTATISTICS_STDATA_57"
        # ServerLib 的 DBDispatcher 在 game 与 auction/point 里 ORIG 形态不同
        # （game 无 INTERNALMSG_SERVICE_UNAVAILABLE ctor，auction/point 有）
        "${COMMON}/DBDispatcher.cpp|-DDNF_SERVERLIB_AUCTION_SHAPE"
    # MySQL Connector/C 6.0.2 静态库（ORIG df_point_r：T mysql_ = 110、U mysql_ = 0）。
    # libstdc++ 保持动态（ORIG df_point_r 的 NEEDED 含 libstdc++.so.6）。
    LIBS
        ${MYSQLCLIENT_A}
        # [2026-09-15] ORIG df_point_r 的 NEEDED 含 libstdc++.so.6（动态）；原先误链静态
        #   libstdc++.a 会多带 eh_globals.o 的 __thread ⇒ .tbss 0x3c218 vs ORIG 0x3c210，
        #   全体 TLS local-exec 常量位移 −8 = 87 件假 NEAR。改回动态 .so（实测 88→1）。
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.so
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libgcc_eh.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc_eh.a
        ${DF_ZLIB32}/lib/libz.a
    LINK_FLAGS
        -static-libgcc -lpthread -lrt -ldl -lm
)

# __FILE__ 覆盖：复刻 ORIG 的 assert 路径字符串（见 point_filepath_override.h）。
# 仅影响 point 构建；与 auction 同源（ORIG 路径字符串一致）。
set(_POINT_FILEPATH_H "${CMAKE_CURRENT_LIST_DIR}/point_filepath_override.h")
set_source_files_properties(${CORE}/SecureStdio.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_POINT_FILEPATH_H};-DPOINT_FILEPATH_SECURESTDIO")
set_source_files_properties(${BASIC}/RecvBuffer.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_POINT_FILEPATH_H};-DPOINT_FILEPATH_RECVBUFFER")
set_source_files_properties(${COMMON}/CommonDataPool.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_POINT_FILEPATH_H};-DPOINT_FILEPATH_COMMONDATAPOOL")
set_source_files_properties(${BASIC}/Token.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_POINT_FILEPATH_H};-DPOINT_FILEPATH_TOKEN")
set_source_files_properties(${COMMON}/LogSendThread.cpp PROPERTIES COMPILE_OPTIONS
    "-include;${_POINT_FILEPATH_H};-DPOINT_FILEPATH_LOGSENDTHREAD")

# ---- yaSSL / TaoCrypt 逐文件 flags + 专属 include（照 df_manager_r 原样）----
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

# 链接用宿主 g++（与 build-point.sh 一致：-m32  -static-libgcc）。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
# [2026-09-14 修复] 原用**宿主 g++** 链接 ⇒ 产物要求 GLIBC_2.34/2.35，而目标运行环境是
#   CentOS 7（glibc 2.17）⇒ `version 'GLIBC_2.34' not found` 跑不起来；ORIG 只要 ≤2.4。
#   改用 game 同款 c6root 4.4.7 驱动 + --sysroot 链接（dnf_game_link.sh 本身通用）。
#   c6root 4.4.7 不认 -no-pie（晚于 4.4），4.4 默认即非 PIE。
if(decompile_verify)
    set(CMAKE_CXX_LINK_EXECUTABLE
        "${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_game_link.sh <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
endif()
