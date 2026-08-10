cmake_minimum_required(VERSION 3.13)
project(stun CXX)

# stun 是 64 位目标（ORIG 为 ELF 64-bit x86-64，GCC 4.1.2-52）。
# toolchain 默认带 -m32，这里清掉，并把 c5root 4.1.2 的完整 isystem 链前置
# （CMAKE_CXX_FLAGS 先于 helper 的 target 选项，先搜先赢；4.1.2 编译器不能用
# 4.4.7 的 libstdc++ 头——allocator.h 的 __is_empty 会编译失败）。
set(CMAKE_CXX_FLAGS "-O0 -nostdinc -isystem${DF_C5ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include -isystem${DF_C5ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed -isystem${DF_C5ROOT}/usr/include/c++/4.1.2 -isystem${DF_C5ROOT}/usr/include/c++/4.1.2/x86_64-redhat-linux -isystem${DF_C5ROOT}/usr/include/c++/4.1.2/backward -isystem${DF_C5ROOT}/usr/include")

set(DNF_SRC "${CMAKE_CURRENT_LIST_DIR}/../..")
include("${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake")

set(_SD "${DNF_SRC}/DNFServer/StunServer/stun_server")

# 源文件与顺序取自 build-stun.sh：stun udp global_func server（链接顺序敏感）。
dnf_service(stun
    ARCH 64
    SOURCES
        ${_SD}/stun.cpp
        ${_SD}/udp.cpp
        ${_SD}/global_func.cpp
        ${_SD}/server.cpp
    DEFINES
        -D_GNU_SOURCE -std=gnu++98
    LIBS
        -lpthread -lcrypt
)

# 链接用宿主 g++（与 build-stun.sh 一致：-no-pie 动态链接宿主 libstdc++/libc）。
# c5root 4.1.2 驱动不识别 -no-pie，且其默认库搜索路径（/usr/lib64）在现代宿主上失效。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
set(CMAKE_CXX_LINK_EXECUTABLE "${DNF_HOST_GXX} -no-pie <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
