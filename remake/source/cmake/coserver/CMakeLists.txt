cmake_minimum_required(VERSION 3.10)

# ============================================================
# df_coserver_r  CMake 构建（取代 source/toolchain/build-coserver.sh）
# 自包含：工具链文件 + 帮助模块 + dnf_service(...)
# 编译：c6root g++ 4.4.7（-m32 -O0 -std=gnu++0x -DDF_NO_CODEPAGE，
#       CFileLogWriterBase -fno-exceptions）
# 链接：宿主机 g++ -m32 -no-pie + 4.4.4/4.1.1 静态 libstdc++/libgcc（同 build 脚本）
# 源顺序 = build 脚本链接顺序（按 .o 基名排序，必须原样保持）
# ============================================================
set(DF_CC_VARIANT "c6")
include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_toolchain_44.cmake)

# 同 statics：包装脚本避免工具链文件的全局 LD_LIBRARY_PATH 污染 CMake 自身进程。
set(DF_CXX_WRAPPER ${CMAKE_BINARY_DIR}/df-cxx-wrap.sh)
file(WRITE ${DF_CXX_WRAPPER}
    "#!/bin/sh\nexport LD_LIBRARY_PATH=\"${DF_C6ROOT}/usr/lib64:${DF_C6ROOT}/usr/lib\"\nexec \"${DF_C6ROOT}/usr/bin/g++\" \"$@\"\n")
execute_process(COMMAND chmod +x ${DF_CXX_WRAPPER})
set(ENV{LD_LIBRARY_PATH} "")
set(CMAKE_CXX_COMPILER ${DF_CXX_WRAPPER})

project(coserver CXX)

# 链接驱动用宿主机 g++（同 build 脚本）
set(CMAKE_CXX_LINK_EXECUTABLE
    "g++ <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake)

set(SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(COMMON  ${SRC_ROOT}/DNFServer/ServerCommon)
set(COS     ${SRC_ROOT}/DNFServer/GameServer/COServer)
set(PACKET  ${SRC_ROOT}/shared/packet)

dnf_service(coserver
    SOURCES
        ${COMMON}/CFileLogWriterBase.cpp
        ${COS}/DNFAppConfig.cpp
        ${COS}/DNFAppStartInit.cpp
        ${COS}/DNFAppStopInit.cpp
        ${COS}/DNFAppThread.cpp
        ${COS}/DNFApplication.cpp
        ${COMMON}/DNFFileLog.cpp
        ${COMMON}/DNFFunctionLib.cpp
        ${COS}/DNFGameServer.cpp
        ${COS}/DNFKillUserConfig.cpp
        ${COS}/DNFNetworkThread.cpp
        ${COS}/DNFPacket.cpp
        ${COS}/DNFPacketDecoder.cpp
        ${COS}/DNFPacketTracer.cpp
        ${COS}/DNFPacketTranslater.cpp
        ${COS}/DNFServerConfig.cpp
        ${COS}/DNFServerHandler.cpp
        ${COS}/DNFSignal.cpp
        ${COS}/DNFSignalTranslator.cpp
        ${COS}/DNFTableBase.cpp
        ${COS}/DNFThreadInterface.cpp
        ${COS}/DNFTickHandler.cpp
        ${COS}/DNFUdpHandler.cpp
        ${COS}/DNFUdpRecvBuffer.cpp
        ${COS}/DNFUser.cpp
        ${COS}/DNFUser1Sig.cpp
        ${COS}/DNFUserInOutCounter.cpp
        ${COS}/DNFUserManager.cpp
        ${COS}/DNFVersion.cpp
        ${PACKET}/src/PacketHeader.cpp
        ${COS}/SystemTimeHandler.cpp
        ${COMMON}/Thread.cpp
    INCLUDES
        -I${COMMON} -I${COS} -I${PACKET}/include
        -I${SRC_ROOT}/shared -I${SRC_ROOT}/shared/common/include
    DEFINES
        -DDF_NO_CODEPAGE
    SRC_OPTS
        "${COMMON}/CFileLogWriterBase.cpp|-fno-exceptions"
    LIBS
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libstdc++.a
        ${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32/libgcc_eh.a
        ${DF_C5R52I}/usr/lib/gcc/i386-redhat-linux/4.1.1/libstdc++.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc.a
        ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.1/32/libgcc_eh.a
        -lpthread -ldl -lm
    LINK_FLAGS
        -m32 -no-pie
)

target_compile_options(coserver PRIVATE -std=gnu++0x)
