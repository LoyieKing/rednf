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

# remake_verify=OFF 兼容层头（在函数外用定义期路径，避免 CMAKE_CURRENT_LIST_DIR
# 在 function 体内解析成调用方目录）
get_filename_component(_DNF_HELPERS_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
set(DNF_COMPAT_HEADER "${_DNF_HELPERS_DIR}/../compat/remake_verify_off.h")
set(DNF_COMPAT_STAT_SRC "${_DNF_HELPERS_DIR}/../compat/dnf_compat_stat.cpp")

# remake_verify=OFF：4.x 工具链文件提前 return，不会定义 DF_* 路径变量，
# 但各服务 CMakeLists 的 LIBS 仍会引用它们（如 ${DF_ZLIB32}/lib/libz.a）⇒
# 这里按同一约定补齐，让引用落到真实路径；libstdc++/libgcc 由 OFF 分支的
# LIBS 过滤器剔除（现代路线用宿主运行时）。
if(NOT remake_verify AND NOT DEFINED DF_C6ROOT)
    if(DEFINED ENV{DNF_TC_ROOT} AND NOT "$ENV{DNF_TC_ROOT}" STREQUAL "")
        set(DF_TC_ROOT "$ENV{DNF_TC_ROOT}")
    elseif(DEFINED DNF_TOOLCHAINS)
        set(DF_TC_ROOT "${DNF_TOOLCHAINS}")
    else()
        set(DF_TC_ROOT "${_DNF_HELPERS_DIR}/../../../toolchains")
    endif()
    get_filename_component(DF_TC_ROOT "${DF_TC_ROOT}" ABSOLUTE)
    set(DF_C6ROOT  "${DF_TC_ROOT}/c6root")
    set(DF_C5ROOT  "${DF_TC_ROOT}/c5root")
    set(DF_C5R52T  "${DF_TC_ROOT}/c5r52tool")
    set(DF_C5R52I  "${DF_TC_ROOT}/c5r52i386")
    set(DF_LSD44   "${DF_TC_ROOT}/lsd44/v4.4.6")
    set(DF_LSD444  "${DF_TC_ROOT}/lsd44/v4.4.4")
    set(DF_ZLIB32  "${DF_TC_ROOT}/zlib32")
    set(DF_CC1P446 "${DF_TC_ROOT}/cc1plus446bin")
endif()

function(dnf_service NAME)
    cmake_parse_arguments(ARG "" "OUTPUT;ARCH" "SOURCES;INCLUDES;DEFINES;SRC_OPTS;LIBS;LINK_FLAGS" ${ARGN})
    if(NOT DEFINED remake_verify)
        set(remake_verify OFF)
    endif()
    if(NOT ARG_OUTPUT)
        set(ARG_OUTPUT "df_${NAME}_r")
    endif()
    if(NOT ARG_ARCH)
        set(ARG_ARCH 32)
    endif()

    if(NOT remake_verify)
        # ---- remake_verify=OFF：宿主最新 LTS GCC + 兼容层（语义等价）----
        # 不使用 4.x 工具链头、不加 -nostdinc、不做逐 TU 后端分发；
        # 兼容层由 -include 注入（register/旧标准库可见性/函数对象基类等）。
        # _GLIBCXX_USE_CXX11_ABI=0：恢复旧 libstdc++ 的对象布局（32 位下
        #   string=4 / list=8 / map=24 / vector=12，与 c6root 4.4.7 实测逐项相同），
        #   否则新 ABI 的 std::string=24 会把所有按 ORIG 偏移定义的类布局顶掉。
        set(COMMON_FLAGS
            -O0 -std=gnu++14 -fpermissive -w
            -Dremake_verify=0
            -D_GLIBCXX_USE_CXX11_ABI=0
            -include ${DNF_COMPAT_HEADER}
        )
        if(ARG_ARCH STREQUAL "32")
            list(APPEND COMMON_FLAGS -m32)
        endif()
        if(ARG_INCLUDES)
            list(APPEND COMMON_FLAGS ${ARG_INCLUDES})
        endif()
        if(ARG_DEFINES)
            # 服务 DEFINES 里的 -std=gnu++98 是 4.x 路线留下的，现代路线统一
            # gnu++14（否则会把上面的 -std 覆盖回去）。
            foreach(_d IN LISTS ARG_DEFINES)
                if(NOT _d MATCHES "^-std=")
                    list(APPEND COMMON_FLAGS ${_d})
                endif()
            endforeach()
        endif()
        # __xstat/__fxstat/__lxstat 族静态实现（老静态库引用、部分新 libc 已移除）
        list(APPEND ARG_SOURCES ${DNF_COMPAT_STAT_SRC})
        # 可选：AddressSanitizer（仅 OFF 路线排查内存越界用；-DDNF_ASAN=ON 打开）
        if(DNF_ASAN)
            list(APPEND COMMON_FLAGS -fsanitize=address -static-libasan -fno-omit-frame-pointer -g)
        endif()
        add_executable(${NAME} ${ARG_SOURCES})
        set_target_properties(${NAME} PROPERTIES
            OUTPUT_NAME "${ARG_OUTPUT}"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
        target_compile_options(${NAME} PRIVATE ${COMMON_FLAGS})
        # 三方/历史 SRC_OPTS 只保留与新编译器兼容的部分（丢 -std=gnu++98 / -B 后端选择）
        if(ARG_SRC_OPTS)
            foreach(pair IN LISTS ARG_SRC_OPTS)
                string(FIND "${pair}" "|" _sep)
                if(_sep GREATER -1)
                    string(SUBSTRING "${pair}" 0 ${_sep} _f)
                    math(EXPR _sp "${_sep}+1")
                    string(SUBSTRING "${pair}" ${_sp} -1 _o)
                    separate_arguments(_o)
                    set(_keep "")
                    foreach(_opt IN LISTS _o)
                        # 4.x 路线的逐 TU 形态旗标（-std / -O / -march / -mtune / -B）
                        # 在宿主 GCC 上要么无意义要么直接报错（实测：-march 与 glibc
                        # 强化头 would-be always_inline 冲突 → target specific option
                        # mismatch），现代路线统一用 COMMON_FLAGS 的基线。
                        if(NOT _opt MATCHES "^-std=" AND NOT _opt MATCHES "^-O"
                           AND NOT _opt MATCHES "^-march=" AND NOT _opt MATCHES "^-mtune="
                           AND NOT _opt MATCHES "^-B")
                            list(APPEND _keep ${_opt})
                        endif()
                    endforeach()
                    if(_keep)
                        set_property(SOURCE ${_f} APPEND PROPERTY COMPILE_OPTIONS "${_keep}")
                    endif()
                endif()
            endforeach()
        endif()
        if(ARG_LIBS)
            # 现代路线：丢掉 4.x 时代的静态 libstdc++/libgcc 归档（宿主编译器自带运行时，
            # 混链旧归档会 ABI 不一致）；其余三方库（mysqlclient/yassl/freetype 等）保留。
            set(_modern_libs "")
            foreach(_lib IN LISTS ARG_LIBS)
                # 4.x 的 libstdc++/libgcc（静态 .a 或动态 .so，含带版本号路径）一律剔除：
                # 现代路线用宿主运行时，混链旧版会在链接期报 "No rule to make target …".
                if(_lib MATCHES "libstdc\\+\\+" OR _lib MATCHES "libgcc(_eh)?\\.")
                    continue()
                endif()
                list(APPEND _modern_libs ${_lib})
            endforeach()
            if(_modern_libs)
                target_link_libraries(${NAME} PRIVATE ${_modern_libs})
            endif()
        endif()
        # 链接期同样要 -m32（否则驱动按 64 位挑隐式 libgcc/libstdc++/libc，
        # 32 位归档里的 __udivdi3 / operator new 会解析不到）。
        set(_dnf_link_opts "")
        if(ARG_ARCH STREQUAL "32")
            list(APPEND _dnf_link_opts -m32)
        endif()
        if(DNF_ASAN)
            list(APPEND _dnf_link_opts -fsanitize=address -static-libasan)
        endif()
        # 历史服务把动态库（-lGeoIP 等）写在 LINK_FLAGS 里 ⇒ CMake 会把它排在
        # 目标文件之前；宿主 g++ 默认 --as-needed 会在此时把"还没被引用"的库丢掉
        # （实测 game 的 GeoIP 未定义引用）。旧 4.4 驱动无此默认，故这里关掉。
        list(APPEND _dnf_link_opts -Wl,--no-as-needed ${ARG_LINK_FLAGS})
        target_link_options(${NAME} PRIVATE ${_dnf_link_opts})
        return()
    endif()

    set(COMMON_FLAGS
        -O0 -fno-enforce-eh-specs -nostdinc -std=gnu++0x -Dremake_verify=1
        # 注意：必须用 -isystem<path> 连接形式（单 token）——CMake 的
        # target_compile_options 会把重复的独立 "-isystem <path>" 参数吞掉，
        # 只保留第一个（GCC 4.4.7 实测已验证连接形式可用）。
        -isystem${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include
        -isystem${DF_C6ROOT}/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include-fixed
    )
    # C++ 标准库头：按编译变体匹配（4.4.x TU 不得混用 4.1.2 头，反之亦然），
    # 紧跟 gcc 内建头，避免被后置的 c5root 4.1.2 头遮蔽。
    if(DF_CC_VARIANT STREQUAL "c6444r")
        list(APPEND COMMON_FLAGS
            -isystem${DF_LSD444}/usr/include/c++/4.4.4
            -isystem${DF_LSD444}/usr/include/c++/4.4.4/i686-redhat-linux
            -isystem${DF_LSD444}/usr/include/c++/4.4.4/backward
        )
    elseif(DF_CC_VARIANT STREQUAL "c6446r")
        list(APPEND COMMON_FLAGS
            -isystem${DF_LSD44}/usr/include/c++/4.4.6
            -isystem${DF_LSD44}/usr/include/c++/4.4.6/x86_64-redhat-linux
            -isystem${DF_LSD44}/usr/include/c++/4.4.6/backward
        )
    elseif(DF_CC_VARIANT STREQUAL "c5")
        list(APPEND COMMON_FLAGS
            -isystem${DF_C5ROOT}/usr/include/c++/4.1.2
            -isystem${DF_C5ROOT}/usr/include/c++/4.1.2/x86_64-redhat-linux
            -isystem${DF_C5ROOT}/usr/include/c++/4.1.2/backward
        )
    else()
        list(APPEND COMMON_FLAGS
            -isystem${DF_C6ROOT}/usr/include/c++/4.4.7
            -isystem${DF_C6ROOT}/usr/include/c++/4.4.7/x86_64-redhat-linux
            -isystem${DF_C6ROOT}/usr/include/c++/4.4.7/backward
        )
    endif()
    # 系统头与其余尾序与既有构建一致（c5root 4.1.2 / lsd44 4.4.6 靠后，不遮蔽变体头）
    list(APPEND COMMON_FLAGS
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
                # 用 set_property(APPEND) 而不是 set_source_files_properties：
                # 后者把多选项字符串展开成多个位置参数，只有第一个选项生效
                # （如 "-O3 -std=gnu++98 -DTIXML_USE_STL ..." 只保留 -O3）。
                set_property(SOURCE ${_f} APPEND PROPERTY COMPILE_OPTIONS "${_o}")
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
