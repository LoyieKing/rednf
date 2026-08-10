cmake_minimum_required(VERSION 3.10)

# ============================================================
# df_statics_r  CMake 构建（取代 source/toolchain/build-statics.sh）
# 自包含：工具链文件 + 帮助模块 + dnf_service(...)
# 编译：c6root g++ 4.4.7（-m32 -O0 -std=gnu++0x，CFileLogWriterBase -fno-exceptions）
# 链接：宿主机 g++ -m32 -no-pie + 4.4.4/4.1.1 静态 libstdc++/libgcc（同 build 脚本）
# 源顺序 = build 脚本链接顺序（按 .o 基名排序，对 _GLOBAL__I_ 敏感，必须原样保持）
# ============================================================
set(DF_CC_VARIANT "c6")
include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_toolchain_44.cmake)

# 工具链文件会全局设置 ENV{LD_LIBRARY_PATH}，会污染 CMake 自身的子进程（jsoncpp/icu
# 需要新版 libstdc++）；改为生成编译器包装脚本，只把 4.4.7 运行库注入编译器子进程。
set(DF_CXX_WRAPPER ${CMAKE_BINARY_DIR}/df-cxx-wrap.sh)
file(WRITE ${DF_CXX_WRAPPER}
    "#!/bin/sh\nexport LD_LIBRARY_PATH=\"${DF_C6ROOT}/usr/lib64:${DF_C6ROOT}/usr/lib\"\nexec \"${DF_C6ROOT}/usr/bin/g++\" \"$@\"\n")
execute_process(COMMAND chmod +x ${DF_CXX_WRAPPER})
set(ENV{LD_LIBRARY_PATH} "")
set(CMAKE_CXX_COMPILER ${DF_CXX_WRAPPER})

project(statics CXX)

# 链接驱动用宿主机 g++（c6root 4.4.7 无法链接 32 位 crt/libc；build 脚本亦如此）
set(CMAKE_CXX_LINK_EXECUTABLE
    "g++ <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

include(${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake)

set(SRC_ROOT  ${CMAKE_CURRENT_LIST_DIR}/../..)
set(COMMON   ${SRC_ROOT}/DNFServer/ServerCommon)
set(STATICS  ${SRC_ROOT}/DNFServer/GameServer/Statics)
set(PACKET   ${SRC_ROOT}/shared/packet)

dnf_service(statics
    SOURCES
        ${COMMON}/CFileLogWriterBase.cpp
        ${STATICS}/CubeStatistics.cpp
        ${STATICS}/DNFAppConfig.cpp
        ${STATICS}/DNFAppStartInit.cpp
        ${STATICS}/DNFAppStopInit.cpp
        ${STATICS}/DNFApplication.cpp
        ${STATICS}/DNFDBServer.cpp
        ${COMMON}/DNFFileLog.cpp
        ${STATICS}/DNFFloatingPointExceptSig.cpp
        ${COMMON}/DNFFunctionLib.cpp
        ${STATICS}/DNFGameServer.cpp
        ${STATICS}/DNFInnerMsgHandler.cpp
        ${STATICS}/DNFKillUserConfig.cpp
        ${STATICS}/DNFManagerServer.cpp
        ${STATICS}/DNFNetworkThread.cpp
        ${STATICS}/DNFPacketDecoder.cpp
        ${STATICS}/DNFPacketTracer.cpp
        ${STATICS}/DNFPacketTranslater.cpp
        ${STATICS}/DNFSegmentationFaultSig.cpp
        ${STATICS}/DNFServerConfig.cpp
        ${STATICS}/DNFServerHandler.cpp
        ${STATICS}/DNFSignal.cpp
        ${STATICS}/DNFSignalTranslator.cpp
        ${STATICS}/DNFStatisticServerMain.cpp
        ${STATICS}/DNFSystemFailSig.cpp
        ${STATICS}/DNFTableBase.cpp
        ${STATICS}/DNFTerminateSig.cpp
        ${STATICS}/DNFThreadInterface.cpp
        ${STATICS}/DNFTickHandler.cpp
        ${STATICS}/DNFUdpHandler.cpp
        ${STATICS}/DNFUdpRecvBuffer.cpp
        ${STATICS}/DNFUser1Sig.cpp
        ${STATICS}/FrameLagCollector.cpp
        ${STATICS}/GMAccounts.cpp
        ${STATICS}/HWSpecResearcher.cpp
        ${PACKET}/src/PacketHeader.cpp
        ${STATICS}/Scheduler.cpp
        ${STATICS}/StatisticProxy.cpp
        ${STATICS}/Statistics.cpp
        ${STATICS}/TaskImpl.cpp
        ${COMMON}/Thread.cpp
        ${STATICS}/UdpCharacteristic.cpp
        ${STATICS}/Version.cpp
    INCLUDES
        -I${COMMON} -I${STATICS} -I${PACKET}/include
        -I${SRC_ROOT}/shared -I${SRC_ROOT}/shared/common/include
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

target_compile_options(statics PRIVATE -std=gnu++0x)
