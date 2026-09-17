cmake_minimum_required(VERSION 3.13)
project(stun CXX)

# stun 是 64 位目标（ORIG 为 ELF 64-bit x86-64，.comment 9 条全部为
# GCC 4.1.2-52）。全部 4 个 TU 用 c5r52tool 4.1.2-52（ORIG 精确版本）
# 编译，消除 -55 补丁级代码生成差异；WORKING_DIRECTORY 切到源目录传
# basename，使 __FILE__ 与 ORIG 一致（"udp.cpp" 而非全路径）。

set(DNF_SRC "${CMAKE_CURRENT_LIST_DIR}/../..")
include("${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_helpers.cmake")

set(_SD "${DNF_SRC}/DNFServer/StunServer/stun_server")

if(NOT decompile_verify)
    # decompile_verify=OFF：宿主最新 LTS GCC 直接编译 4 个 TU（语义等价；不追求逐字节）
    add_executable(stun
        ${_SD}/stun.cpp ${_SD}/udp.cpp ${_SD}/global_func.cpp ${_SD}/server.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../toolchain/compat/dnf_compat_stat.cpp)
    set_target_properties(stun PROPERTIES
        OUTPUT_NAME "df_stun_r"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    target_compile_options(stun PRIVATE -O0 -std=gnu++14 -fpermissive -w -Ddecompile_verify=0
        -include ${CMAKE_CURRENT_LIST_DIR}/../../toolchain/compat/decompile_verify_off.h)
    target_link_libraries(stun PRIVATE -lpthread)
    return()
endif()

set(C5R52_GXX ${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_c5r52_gxx.sh)
set(C5R52_FLAGS
    -O0 -D_GNU_SOURCE -std=gnu++98 -nostdinc
    -isystem${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include
    -isystem${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.2/include-fixed
    -isystem${DF_C5ROOT}/usr/include/c++/4.1.2
    -isystem${DF_C5ROOT}/usr/include/c++/4.1.2/x86_64-redhat-linux
    -isystem${DF_C5ROOT}/usr/include/c++/4.1.2/backward
    -isystem${DF_C5ROOT}/usr/include)

# global_func TU 带 -fno-builtin-memset：ORIG 中 write_log 的 memset 是普通
# 调用形态（lea 直入 rdi），save_pid 是 builtin 形态（lea-rax+mov，源码用
# __builtin_memset）；A/B 试编译确认该组合为唯一同时复现两侧形态的配置。
set(C5R52_GF_FLAGS ${C5R52_FLAGS} -fno-builtin-memset)

# 链接顺序敏感（与 build-stun.sh 一致：stun udp global_func server）。
foreach(f stun udp global_func server)
    if(f STREQUAL "global_func")
        set(_TU_FLAGS ${C5R52_GF_FLAGS})
    else()
        set(_TU_FLAGS ${C5R52_FLAGS})
    endif()
    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/${f}.o
        COMMAND ${C5R52_GXX} ${_TU_FLAGS} -c ${f}.cpp
                -o ${CMAKE_BINARY_DIR}/${f}.o
        WORKING_DIRECTORY ${_SD}
        DEPENDS ${_SD}/${f}.cpp
                ${_SD}/stun.h ${_SD}/udp.h
                ${_SD}/global_func.h ${_SD}/server.h
        COMMENT "${f}.cpp -> GCC 4.1.2-52 (c5r52)"
        VERBATIM)
endforeach()

add_executable(stun
    ${CMAKE_BINARY_DIR}/stun.o
    ${CMAKE_BINARY_DIR}/udp.o
    ${CMAKE_BINARY_DIR}/global_func.o
    ${CMAKE_BINARY_DIR}/server.o)
set_target_properties(stun PROPERTIES
    OUTPUT_NAME "df_stun_r"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LINKER_LANGUAGE CXX
)
# [2026-09-14] c6root 驱动 + -nostdlib 后 C++ 运行时需显式给出（ORIG stun 的编译器
#   是 c5r52tool 4.1.2-52，故用同根的 64 位静态 libstdc++/libgcc）。
target_link_libraries(stun PRIVATE
    ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.2/libstdc++.a
    ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.2/libgcc_eh.a
    ${DF_C5R52T}/usr/lib/gcc/x86_64-redhat-linux/4.1.2/libgcc.a

    -lpthread)

# 链接用宿主 g++（与 build-stun.sh 一致：-no-pie 动态链接宿主 libstdc++/libc）。
# c5root 4.1.2 驱动不识别 -no-pie，且其默认库搜索路径（/usr/lib64）在现代宿主上失效。
find_program(DNF_HOST_GXX NAMES g++ PATHS /usr/bin /usr/local/bin NO_DEFAULT_PATH)
# [2026-09-14 修复] 原用宿主 g++ ⇒ 产物要求 GLIBC_2.34，CentOS 7(glibc 2.17) 跑不起来
#   （ORIG df_stun_r 只要 GLIBC_2.2.5）。stun 是 64 位目标，改用 c6root EL6 64 位根链接
#   （dnf_game_link.sh 现按 -m64 自动切 64 位 crt/libc，产物 glibc ≤2.12）。
if(decompile_verify)
    set(CMAKE_CXX_LINK_EXECUTABLE
        "${CMAKE_CURRENT_LIST_DIR}/../../toolchain/cmake/dnf_game_link.sh -m64 <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
endif()
