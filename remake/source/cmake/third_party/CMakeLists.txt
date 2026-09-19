# ============================================================================
# 三方静态库构建（原 source/toolchain/build-{mysqlclient,yassl,freetype248-static32}.sh）
#
# 本工程由 source/CMakeLists.txt 统一驱动；本目录通过 add_subdirectory() 进入，
# 三个库全部用 CMake 的自定义命令复现原脚本的编译参数、目标文件命名与归档顺序，
# 不再依赖任何 shell 构建脚本。
#
# 产物路径保持与服务 CMakeLists 中的既有引用一致（未做任何迁移）：
#   ${DNF_BUILD}/manager/mysqlclient/libmysqlclient.a   （manager/dbmw/auction/point/bridge/game 链接）
#   ${DNF_BUILD}/manager/yassl/libyassl.a               （game 链接）
#   ${DNF_TOOLCHAINS}/freetype32/lib/libfreetype.a      （game 链接）
#
# 编译器一律为 c6root（GCC 4.4.7），参数逐条取自原脚本：
#   mysqlclient : gcc -m32 -O2 -fPIC -DNDEBUG -DDBUG_OFF -DHAVE_YASSL -DYASSL_PURE_C -DHAVE_OPENSSL
#   yassl       : g++ -m32 -O3 -fPIC -DNDEBUG -std=gnu++98 -DHAVE_CONFIG_H -DYASSL_PREFIX -D_LIB ...
#   freetype    : gcc -m32 -O2 -fno-strict-aliasing -DFT2_BUILD_LIBRARY -w（7 模块、关闭 zlib）
# ============================================================================

cmake_minimum_required(VERSION 3.10)
project(dnf_third_party NONE)

if(NOT DNF_ROOT OR NOT DNF_BUILD OR NOT DNF_TOOLCHAINS)
    message(FATAL_ERROR "third_party 必须由 source/CMakeLists.txt 通过 add_subdirectory() 引入")
endif()

# 路径一律规范化（collapse ".."）——三方源码里有 __FILE__ 展开（yaSSL 的
# misc.hpp/runtime.hpp 等断言宏），编译器命令行中的路径写法会原样进入目标文件；
# 必须与原脚本 `$(cd …; pwd)` 得到的干净绝对路径逐字符一致，产物才能逐字节复现。
get_filename_component(DNF_ROOT       "${DNF_ROOT}"       ABSOLUTE)
get_filename_component(DNF_TOOLCHAINS "${DNF_TOOLCHAINS}" ABSOLUTE)
get_filename_component(DNF_BUILD      "${DNF_BUILD}"      ABSOLUTE)

set(DNF_C6        ${DNF_TOOLCHAINS}/c6root)
set(DNF_C6_CC     ${DNF_C6}/usr/bin/gcc)
set(DNF_C6_CXX    ${DNF_C6}/usr/bin/g++)
set(DNF_MYSQL     ${DNF_ROOT}/source/Library3rd/MySQL)
set(DNF_YASSL     ${DNF_MYSQL}/extlib/yassl)

find_program(DNF_HOST_AR NAMES ar)
if(NOT DNF_HOST_AR)
    message(FATAL_ERROR "未找到 host ar（原脚本使用 PATH 中的 ar）")
endif()

# c6root 工具链的运行期环境（原脚本在 shell 内导出；这里逐命令注入）
set(DNF_C6_ENV
    ${CMAKE_COMMAND} -E env
    "GCC_EXEC_PREFIX=${DNF_C6}/usr/libexec/gcc/"
    "LD_LIBRARY_PATH=${DNF_C6}/usr/lib64:${DNF_C6}/usr/lib")

# ---------------------------------------------------------------------------
# ① libmysqlclient.a —— MySQL Connector/C 6.0.2 客户端静态库
# ---------------------------------------------------------------------------
set(MC_OUT ${DNF_BUILD}/manager/mysqlclient)

# 源文件与顺序完全照搬原脚本（-- 分段与脚本内的 for 循环一一对应）
set(MC_LIBMYSQL client.c errmsg.c get_password.c libmysql.c my_time.c net_serv.c pack.c password.c)
set(MC_MYSYS
    array.c charset-def.c charset.c checksum.c default.c default_modify.c errors.c hash.c list.c md5.c
    mf_brkhant.c mf_cache.c mf_dirname.c mf_fn_ext.c mf_format.c mf_getdate.c mf_iocache.c mf_iocache2.c
    mf_keycache.c my_safehash.c mf_keycaches.c mf_loadpath.c mf_pack.c mf_path.c mf_qsort.c mf_qsort2.c
    mf_radix.c mf_same.c mf_sort.c mf_soundex.c mf_strip.c mf_arr_appstr.c mf_tempdir.c mf_tempfile.c
    mf_unixpath.c mf_wcomp.c mf_wfile.c mulalloc.c my_access.c my_aes.c my_alarm.c my_alloc.c my_append.c
    my_bit.c my_bitmap.c my_chmod.c my_chsize.c my_clock.c my_compress.c my_conio.c my_copy.c my_create.c
    my_delete.c my_div.c my_error.c my_file.c my_fopen.c my_fstream.c my_gethostbyname.c my_gethwaddr.c
    my_getopt.c my_getsystime.c my_getwd.c my_init.c my_lib.c my_lock.c my_lockmem.c my_malloc.c my_messnc.c
    my_dup.c my_mkdir.c my_mmap.c my_net.c my_once.c my_open.c my_pread.c my_pthread.c my_quick.c my_read.c
    my_realloc.c my_redel.c my_rename.c my_seek.c my_sleep.c my_static.c my_symlink.c my_symlink2.c my_sync.c
    my_thr_init.c my_wincond.c my_winerr.c my_winfile.c my_windac.c my_winthread.c my_write.c ptr_cmp.c
    queues.c stacktrace.c rijndael.c safemalloc.c sha1.c string.c thr_alarm.c thr_lock.c thr_mutex.c
    thr_rwlock.c tree.c typelib.c my_vle.c base64.c my_memmem.c my_getpagesize.c lf_alloc-pin.c lf_dynarray.c
    lf_hash.c my_atomic.c my_getncpus.c my_rnd.c my_uuid.c wqueue.c waiting_threads.c my_port.c)
set(MC_STRINGS
    bchange.c bcmp.c bfill.c bmove512.c bmove_upp.c ctype-big5.c ctype-bin.c ctype-cp932.c ctype-czech.c
    ctype-euc_kr.c ctype-eucjpms.c ctype-extra.c ctype-gb2312.c ctype-gbk.c ctype-latin1.c ctype-mb.c
    ctype-simple.c ctype-sjis.c ctype-tis620.c ctype-uca.c ctype-ucs2.c ctype-ujis.c ctype-utf8.c
    ctype-win1250ch.c ctype.c decimal.c int2str.c is_prefix.c llstr.c longlong2str.c my_strtoll10.c
    my_vsnprintf.c r_strinstr.c str2int.c str_alloc.c strcend.c strend.c strfill.c strmake.c strmov.c
    strnmov.c strtol.c strtoll.c strtoul.c strtoull.c strxmov.c strxnmov.c xml.c dtoa.c my_strchr.c
    strcont.c strinstr.c strnlen.c strappend.c)
set(MC_VIO   vio.c viosocket.c viossl.c viosslfactories.c)
set(MC_ZLIB  adler32.c compress.c crc32.c deflate.c gzio.c infback.c inffast.c inflate.c inftrees.c trees.c uncompr.c zutil.c)
set(MC_DBUG  dbug.c)

set(MC_SRCS "")
foreach(_f IN LISTS MC_LIBMYSQL)
    list(APPEND MC_SRCS ${DNF_MYSQL}/libmysql/${_f})
endforeach()
foreach(_f IN LISTS MC_MYSYS)
    list(APPEND MC_SRCS ${DNF_MYSQL}/mysys/${_f})
endforeach()
foreach(_f IN LISTS MC_STRINGS)
    list(APPEND MC_SRCS ${DNF_MYSQL}/strings/${_f})
endforeach()
foreach(_f IN LISTS MC_VIO)
    list(APPEND MC_SRCS ${DNF_MYSQL}/vio/${_f})
endforeach()
foreach(_f IN LISTS MC_ZLIB)
    list(APPEND MC_SRCS ${DNF_MYSQL}/extlib/zlib/${_f})
endforeach()
foreach(_f IN LISTS MC_DBUG)
    list(APPEND MC_SRCS ${DNF_MYSQL}/extlib/dbug/${_f})
endforeach()

set(MC_FLAGS
    -m32 -O2 -fPIC -DNDEBUG -DDBUG_OFF -DHAVE_YASSL -DYASSL_PURE_C -DHAVE_OPENSSL
    -nostdinc
    -isystem ${DNF_C6}/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include
    -isystem ${DNF_C6}/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include-fixed
    -isystem ${DNF_C6}/usr/include
    -I${DNF_MYSQL}/include -I${DNF_MYSQL}/mysys -I${DNF_MYSQL}/strings
    -I${DNF_YASSL}/include -I${DNF_YASSL}/taocrypt/include
    -I${DNF_YASSL}/taocrypt/mySTL -I${DNF_MYSQL}/extlib/zlib)

set(MC_OBJS "")
foreach(_src IN LISTS MC_SRCS)
    get_filename_component(_base ${_src} NAME_WE)
    set(_obj ${MC_OUT}/${_base}.o)
    add_custom_command(
        OUTPUT ${_obj}
        # 全新构建目录下输出目录并不存在（原 build-*.sh 脚本先 mkdir -p）；
        # 并行 make 时若无人建目录会直接 "can't create …: No such file or directory"。
        COMMAND ${CMAKE_COMMAND} -E make_directory ${MC_OUT}
        COMMAND ${DNF_C6_ENV} ${DNF_C6_CC} ${MC_FLAGS} -c ${_src} -o ${_obj}
        DEPENDS ${_src}
        COMMENT "CC  ${_base}.c"
        VERBATIM)
    list(APPEND MC_OBJS ${_obj})
endforeach()

# 原脚本用 `ar rcs lib.a *.o`（shell glob 按名排序），此处显式排序复现同一归档顺序
set(MC_OBJS_ORDERED ${MC_OBJS})
list(SORT MC_OBJS_ORDERED)
add_custom_command(
    OUTPUT ${MC_OUT}/libmysqlclient.a
    COMMAND ${CMAKE_COMMAND} -E rm -f ${MC_OUT}/libmysqlclient.a
    COMMAND ${DNF_HOST_AR} rcs ${MC_OUT}/libmysqlclient.a ${MC_OBJS_ORDERED}
    DEPENDS ${MC_OBJS}
    COMMENT "AR  libmysqlclient.a"
    VERBATIM)
add_custom_target(mysqlclient ALL DEPENDS ${MC_OUT}/libmysqlclient.a)

# ---------------------------------------------------------------------------
# ② libyassl.a —— yaSSL / TaoCrypt 静态库
# ---------------------------------------------------------------------------
set(YS_OUT ${DNF_BUILD}/manager/yassl)
set(YS_TAOCRYPT aes aestables algebra arc4 asn coding des dh dsa file hash integer md2 md4 md5 misc
               random ripemd rsa sha block_instnt)
set(YS_CORE     buffer cert_wrapper crypto_wrapper handshake lock log socket_wrapper ssl timer
               yassl_error yassl_imp yassl_int)

set(YS_FLAGS
    -m32 -O3 -fPIC -DNDEBUG -std=gnu++98 -DHAVE_CONFIG_H -DYASSL_PREFIX -D_LIB
    -DHAVE_YASSL -DYASSL_PURE_C -DHAVE_OPENSSL
    -nostdinc
    -isystem ${DNF_C6}/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include
    -isystem ${DNF_C6}/usr/lib/gcc/x86_64-redhat-linux/4.4.7/include-fixed
    -isystem ${DNF_C6}/usr/include
    -I${DNF_YASSL}/include -I${DNF_YASSL}/taocrypt/include -I${DNF_YASSL}/taocrypt/mySTL)

# 脚本按 TAOCRYPT 段 + CORE 段依次编译并保持该顺序入档，此处同序
set(YS_OBJS "")
foreach(_f IN LISTS YS_TAOCRYPT)
    set(_src ${DNF_YASSL}/taocrypt/src/${_f}.cpp)
    set(_obj ${YS_OUT}/obj/taocrypt_${_f}.o)
    add_custom_command(
        OUTPUT ${_obj}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${YS_OUT}/obj
        COMMAND ${DNF_C6_ENV} ${DNF_C6_CXX} ${YS_FLAGS} -c ${_src} -o ${_obj}
        DEPENDS ${_src}
        COMMENT "CXX taocrypt_${_f}.o"
        VERBATIM)
    list(APPEND YS_OBJS ${_obj})
endforeach()
foreach(_f IN LISTS YS_CORE)
    set(_src ${DNF_YASSL}/src/${_f}.cpp)
    set(_obj ${YS_OUT}/obj/yassl_${_f}.o)
    add_custom_command(
        OUTPUT ${_obj}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${YS_OUT}/obj
        COMMAND ${DNF_C6_ENV} ${DNF_C6_CXX} ${YS_FLAGS} -c ${_src} -o ${_obj}
        DEPENDS ${_src}
        COMMENT "CXX yassl_${_f}.o"
        VERBATIM)
    list(APPEND YS_OBJS ${_obj})
endforeach()

add_custom_command(
    OUTPUT ${YS_OUT}/libyassl.a
    COMMAND ${CMAKE_COMMAND} -E rm -f ${YS_OUT}/libyassl.a
    COMMAND ${DNF_HOST_AR} rcs ${YS_OUT}/libyassl.a ${YS_OBJS}
    DEPENDS ${YS_OBJS}
    COMMENT "AR  libyassl.a"
    VERBATIM)
add_custom_target(yassl ALL DEPENDS ${YS_OUT}/libyassl.a)

# ---------------------------------------------------------------------------
# ③ libfreetype.a —— FreeType 2.4.8 静态库（7 模块、关闭 zlib）
# ---------------------------------------------------------------------------
set(FT_DIR      ${DNF_TOOLCHAINS}/freetype32)
set(FT_TARBALL  ${FT_DIR}/src/freetype-VER-2-4-8.tar.gz)
set(FT_WORK     ${DNF_BUILD}/third_party/freetype)
set(FT_SRC      ${FT_WORK}/freetype-VER-2-4-8)
set(FT_A        ${FT_DIR}/lib/libfreetype.a)

if(NOT EXISTS ${FT_TARBALL})
    message(STATUS "FreeType 源码包缺失，尝试下载 VER-2-4-8 …")
    file(DOWNLOAD
        https://github.com/freetype/freetype/archive/refs/tags/VER-2-4-8.tar.gz
        ${FT_TARBALL} STATUS _ft_dl_status)
    list(GET _ft_dl_status 0 _ft_dl_rc)
    if(NOT _ft_dl_rc EQUAL 0)
        message(FATAL_ERROR "FreeType 源码包下载失败：${FT_TARBALL}（${_ft_dl_status}）")
    endif()
endif()

# 解包 + 模块裁剪 + 关闭 zlib（对应原脚本的 tar / ftmodule.h / ftoption.h 三步）
if(NOT EXISTS ${FT_SRC}/include/freetype/ft.h)
    file(MAKE_DIRECTORY ${FT_WORK})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${FT_TARBALL}
        WORKING_DIRECTORY ${FT_WORK}
        RESULT_VARIABLE _ft_rc)
    if(NOT _ft_rc EQUAL 0)
        message(FATAL_ERROR "FreeType 解包失败 rc=${_ft_rc}")
    endif()
endif()

file(WRITE ${FT_SRC}/include/freetype/config/ftmodule.h
"FT_USE_MODULE( FT_Driver_ClassRec, tt_driver_class )
FT_USE_MODULE( FT_Module_Class, sfnt_module_class )
FT_USE_MODULE( FT_Renderer_Class, ft_raster1_renderer_class )
FT_USE_MODULE( FT_Renderer_Class, ft_raster5_renderer_class )
FT_USE_MODULE( FT_Renderer_Class, ft_smooth_renderer_class )
FT_USE_MODULE( FT_Renderer_Class, ft_smooth_lcd_renderer_class )
FT_USE_MODULE( FT_Renderer_Class, ft_smooth_lcdv_renderer_class )
")
file(READ ${FT_SRC}/include/freetype/config/ftoption.h _ft_option)
string(REPLACE "#define FT_CONFIG_OPTION_USE_ZLIB" "/* #define FT_CONFIG_OPTION_USE_ZLIB */" _ft_option "${_ft_option}")
file(WRITE ${FT_SRC}/include/freetype/config/ftoption.h "${_ft_option}")

set(FT_SRCS src/base/ftsystem.c src/base/ftinit.c src/base/ftdebug.c src/base/ftbase.c
            src/truetype/truetype.c src/sfnt/sfnt.c src/smooth/smooth.c src/raster/raster.c)
set(FT_FLAGS -m32 -O2 -fno-strict-aliasing -DFT2_BUILD_LIBRARY -I${FT_SRC}/include -w)

set(FT_OBJS "")
foreach(_f IN LISTS FT_SRCS)
    set(_src ${FT_SRC}/${_f})
    get_filename_component(_base ${_f} NAME_WE)
    set(_obj ${FT_WORK}/objs/${_base}.o)
    add_custom_command(
        OUTPUT ${_obj}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${FT_WORK}/objs
        COMMAND ${DNF_C6_ENV} ${DNF_C6_CC} ${FT_FLAGS} -c ${_src} -o ${_obj}
        DEPENDS ${_src}
                ${FT_SRC}/include/freetype/config/ftmodule.h
                ${FT_SRC}/include/freetype/config/ftoption.h
        COMMENT "CC  freetype/${_base}.c"
        VERBATIM)
    list(APPEND FT_OBJS ${_obj})
endforeach()
set(FT_OBJS_ORDERED ${FT_OBJS})
list(SORT FT_OBJS_ORDERED)
add_custom_command(
    OUTPUT ${FT_A}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${FT_DIR}/lib
    COMMAND ${CMAKE_COMMAND} -E rm -f ${FT_A}
    COMMAND ${DNF_HOST_AR} rcs ${FT_A} ${FT_OBJS_ORDERED}
    DEPENDS ${FT_OBJS}
    COMMENT "AR  libfreetype.a"
    VERBATIM)
add_custom_target(freetype ALL DEPENDS ${FT_A})

# ---------------------------------------------------------------------------
# ④ libGeoIP.so.1 —— 真实 Country edition 实现（替换原 geoip_stub 链接桩）
#    · 源码 source/toolchain/geoip/geoip_country.c（上游 dnf-compat-layer
#      geoip-compat 的 C 等价移植：5 个导出符号逐一对齐）
#    · ON  路线用 c6root gcc 4.4.7（产物只需 GLIBC ≤2.12，CentOS 7 也能跑）
#    · OFF 路线用宿主 gcc -m32（glibc 2.39，容器 Ubuntu 24.04）
#    · 产物 SONAME=libGeoIP.so.1 ⇒ 游戏侧 DT_NEEDED 记名与 ORIG/桩件一致，
#      ON 路线逐字节 identical 不受影响。
# ---------------------------------------------------------------------------
set(GEOIP_SRC ${DNF_ROOT}/source/toolchain/geoip/geoip_country.c)
set(GEOIP_OUT ${DNF_BUILD}/geoip)
set(GEOIP_SO  ${GEOIP_OUT}/libGeoIP.so.1)

if(remake_verify)
    set(G32 ${DNF_C6}/usr/lib/gcc/x86_64-redhat-linux/4.4.4/32)
    set(GEOIP_CMD ${DNF_C6_ENV} ${DNF_C6_CC}
        -m32 -shared -fPIC -std=gnu99 -O2 -w
        --sysroot=${DNF_C6} -B${G32}
        ${DNF_C6}/usr/lib/crti.o ${G32}/crtbeginS.o
        ${GEOIP_SRC}
        -o ${GEOIP_SO}
        ${G32}/crtendS.o ${DNF_C6}/usr/lib/crtn.o
        -L${DNF_C6}/usr/lib -L${DNF_C6}/lib -nostdlib
        -lgcc -lc -lgcc -lgcc_eh
        -Wl,-soname,libGeoIP.so.1)
else()
    find_program(DNF_HOST_CC NAMES gcc cc)
    if(NOT DNF_HOST_CC)
        message(FATAL_ERROR "未找到宿主 C 编译器（构建 libGeoIP.so.1 需要）")
    endif()
    set(GEOIP_CMD ${DNF_HOST_CC} -m32 -shared -fPIC -std=gnu99 -O2 -w
        -Wl,-soname,libGeoIP.so.1 ${GEOIP_SRC} -o ${GEOIP_SO})
endif()

add_custom_command(
    OUTPUT ${GEOIP_SO}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GEOIP_OUT}
    COMMAND ${GEOIP_CMD}
    DEPENDS ${GEOIP_SRC}
    COMMENT "CC/LD libGeoIP.so.1（真实 Country edition 实现）"
    VERBATIM)
add_custom_target(geoip ALL DEPENDS ${GEOIP_SO})

# 统一依赖点：服务子配置以 third_party 为目标前置
add_custom_target(third_party ALL)
add_dependencies(third_party mysqlclient yassl freetype geoip)
