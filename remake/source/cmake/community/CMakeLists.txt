cmake_minimum_required(VERSION 3.10)

# ============================================================
# df_community_r  CMake 构建（独立自包含，沿用 source/CMakeLists.txt 的 community
# 构建逻辑：系统编译器 GCC 13 + -m32 + 静态 libstdc++/libgcc + gnu++0x）
# 不引入 GCC 4.x 工具链文件（社区官方口径 113 IDENT 由该系统编译器构建达成）。
# ============================================================
project(community C CXX)
message(STATUS "Using system compiler")

# --- C++ standard matching original build ---
if(NOT DF_CXX_STANDARD)
    set(DF_CXX_STANDARD gnu++0x)
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${DF_CXX_STANDARD}")

# --- Global compiler/linker flags（同 source/CMakeLists.txt community 段）---
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "8")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32 -pthread -O0 -fno-pie -fno-PIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -pthread -O0 -fno-pie -fno-PIC -D_GLIBCXX_USE_CXX11_ABI=0 -static-libstdc++ -static-libgcc")
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -m32 -pthread -no-pie -static-libstdc++ -static-libgcc -lpthread -lrt -lm")
else()
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32 -pthread -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -pthread -O0")
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -m32 -pthread")
endif()
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32 -pthread")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -m32 -pthread")

set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-g -O0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O0")
set(CMAKE_C_FLAGS_MINSIZEREL "-O0")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-O0")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(SRC_ROOT ${CMAKE_CURRENT_LIST_DIR}/../..)
set(COMMUNITY ${SRC_ROOT}/Community)

set(DF_COMMUNITY_R_SOURCES
    ${COMMUNITY}/ArchiveLog.cpp
    ${COMMUNITY}/BuddyManager.cpp
    ${COMMUNITY}/CommunityServer.cpp
    ${COMMUNITY}/ConfigFileReader.cpp
    ${COMMUNITY}/CoreDump.cpp
    ${COMMUNITY}/GlobalFunction.cpp
    ${COMMUNITY}/GlobalInstance.cpp
    ${COMMUNITY}/NetworkSession.cpp
    ${COMMUNITY}/PacketDispatcher.cpp
    ${COMMUNITY}/PacketParser.cpp
    ${COMMUNITY}/PacketProc.cpp
    ${COMMUNITY}/ProcessManager.cpp
    ${COMMUNITY}/SessionManager.cpp
    ${COMMUNITY}/User.cpp
    ${COMMUNITY}/UserManager.cpp
    ${COMMUNITY}/rand_r.cpp
    ${SRC_ROOT}/DNFServer/ServerCommon/DNFFileLog.cpp
    ${SRC_ROOT}/DNFServer/ServerCommon/CFileLogWriterBase.cpp
    ${SRC_ROOT}/DNFServer/ServerCommon/DNFFunctionLib.cpp
    ${SRC_ROOT}/DNFServer/ServerCommon/Thread.cpp
    ${SRC_ROOT}/shared/packet/src/PacketHeader.cpp
    ${SRC_ROOT}/shared/packet/src/STGameUserInfo.cpp
    ${SRC_ROOT}/shared/packet/src/STPvPBuddyDBInfo.cpp
)
set_source_files_properties(
    ${COMMUNITY}/ArchiveLog.cpp
    PROPERTIES COMPILE_FLAGS "-fno-exceptions")
set_source_files_properties(
    ${SRC_ROOT}/DNFServer/ServerCommon/CFileLogWriterBase.cpp
    PROPERTIES COMPILE_FLAGS "-fno-exceptions")
add_executable(df_community_r ${DF_COMMUNITY_R_SOURCES})
target_include_directories(df_community_r PRIVATE
    ${COMMUNITY}
    ${SRC_ROOT}/DNFServer/ServerCommon
    ${SRC_ROOT}/Library3rd/Boost/Include
    ${SRC_ROOT}/shared
    ${SRC_ROOT}/shared/common/include
    ${SRC_ROOT}/shared/packet/include
)
target_compile_definitions(df_community_r PRIVATE BOOST_DISABLE_ASSERTS)
