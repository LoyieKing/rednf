# DNF 服务构建帮助模块（配合 dnf_toolchain_44.cmake）
# 提供:
#   dnf_service(<name>
#       SOURCES <ordered .cpp list>
#       [INCLUDES <extra -I dirs>]
#       [DEFINES <defines>]
#       [SRC_OPTS <file|-opts>...]   # 按文件覆盖编译选项
#       [LIBS <static libs in link order>]
#       [LINK_FLAGS <extra link flags>]
#       [ARCH 32|64]                 # 默认 32
#       [OUTPUT <binary name>])
# 产物: $<BINARY_DIR>/<name>/df_<name>_r（若 OUTPUT 指定则用 OUTPUT）

function(dnf_service NAME)
    cmake_parse_arguments(ARG "" "OUTPUT;ARCH" "SOURCES;INCLUDES;DEFINES;SRC_OPTS;LIBS;LINK_FLAGS" ${ARGN})
    if(NOT ARG_OUTPUT)
        set(ARG_OUTPUT "df_${NAME}_r")
    endif()
    if(NOT ARG_ARCH)
        set(ARG_ARCH 32)
    endif()

    set(COMMON_FLAGS
        -O0 -fno-enforce-eh-specs -nostdinc
        # 注意：必须用 -isystem<path> 连接形式（单 token）——CMake 的
        # target_compile_options 会把重复的独立 "-isystem <path>" 参数吞掉，
        # 只保留第一个（GCC 4.4.7 实测已验证连接形式可用）。
        -isystem${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include
        -isystem${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include-fixed
        -isystem${DF_C6ROOT}/usr/include/c++/4.4.7
        -isystem${DF_C6ROOT}/usr/include/c++/4.4.7/x86_64-redhat-linux
        -isystem${DF_C6ROOT}/usr/include/c++/4.4.7/backward
        -isystem${DF_C6ROOT}/usr/include
        -isystem${DF_C5ROOT}/usr/include/c++/4.1.2
        -isystem${DF_C5ROOT}/usr/include/c++/4.1.2/x86_64-redhat-linux
        -isystem${DF_C5ROOT}/usr/include/c++/4.1.2/backward
        -isystem${DF_C5ROOT}/usr/include
        -isystem${DF_LSD44}/usr/include/c++/4.4.6
        -isystem${DF_LSD44}/usr/include/c++/4.4.6/x86_64-redhat-linux
        -isystem${DF_LSD44}/usr/include/c++/4.4.6/backward
        -isystem${CMAKE_CURRENT_SOURCE_DIR}
    )
    if(ARG_ARCH STREQUAL "32")
        list(APPEND COMMON_FLAGS -m32)
    endif()
    if(ARG_INCLUDES)
        list(APPEND COMMON_FLAGS ${ARG_INCLUDES})
    endif()
    if(ARG_DEFINES)
        list(APPEND COMMON_FLAGS ${ARG_DEFINES})
    endif()

    add_executable(${NAME} ${ARG_SOURCES})
    set_target_properties(${NAME} PROPERTIES
        OUTPUT_NAME "${ARG_OUTPUT}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    target_compile_options(${NAME} PRIVATE ${COMMON_FLAGS})

    # 按文件覆盖编译选项（SRC_OPTS 元素形如 "file.cpp|-O3 -std=gnu++98"）
    if(ARG_SRC_OPTS)
        foreach(pair IN LISTS ARG_SRC_OPTS)
            string(FIND "${pair}" "|" _sep)
            if(_sep GREATER -1)
                string(SUBSTRING "${pair}" 0 ${_sep} _f)
                math(EXPR _sp "${_sep}+1")
                string(SUBSTRING "${pair}" ${_sp} -1 _o)
                separate_arguments(_o)
                set_source_files_properties(${_f} PROPERTIES COMPILE_OPTIONS "${_o}")
            endif()
        endforeach()
    endif()

    if(ARG_LIBS)
        target_link_libraries(${NAME} PRIVATE ${ARG_LIBS})
    endif()
    if(ARG_LINK_FLAGS)
        target_link_options(${NAME} PRIVATE ${ARG_LINK_FLAGS})
    endif()
endfunction()
