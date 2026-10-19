# secsvr CMake helper
#
# 目标：
#   - tencrypt_new: 32 位静态库，保留 -fabi-version=2 以对齐 GCC 4.1 的 C1/C2 符号。
#   - gunnersvr / zergsvr / secagent: 从各自的 source/<binary>/ 树编译，使用该树的
#     import/include 与 output/**/release/include 镜像。
#
# 该模块只定义构建/链接，不修改源码；仍为空实现的 TU 会正常编译，但运行时行为
# 仍需按 README 的 TODO 补齐和验收。

function(secsvr_compile_options target)
  target_compile_options(${target} PRIVATE
    -m32
    -O2
    -DNDEBUG
    -pthread
    -fno-strict-aliasing
    -fpermissive
    -fabi-version=2
    -D_GLIBCXX_USE_CXX11_ABI=0
    -Wno-deprecated-declarations
  )
  target_link_options(${target} PRIVATE -m32 -pthread)
endfunction()

function(secsvr_add_tencrypt)
  file(GLOB_RECURSE tencrypt_sources CONFIGURE_DEPENDS
    "${SECSVR_ROOT}/tencrypt_new/src/*.cpp")

  add_library(tencrypt_new STATIC ${tencrypt_sources})
  target_include_directories(tencrypt_new PRIVATE
    "${SECSVR_ROOT}/tencrypt_new"
    "${SECSVR_ROOT}/tencrypt_new/include"
    "${SECSVR_ROOT}/tencrypt_new/inc"
  )
  target_compile_options(tencrypt_new PRIVATE
    -m32
    -O2
    -DNDEBUG
    -pthread
    -fno-strict-aliasing
    -fpermissive
    -fabi-version=2
    -D_GLIBCXX_USE_CXX11_ABI=0
    -Wno-deprecated-declarations
  )
  set_target_properties(tencrypt_new PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endfunction()

function(secsvr_add_service name root)
  file(GLOB_RECURSE service_sources CONFIGURE_DEPENDS
    "${root}/src/*.cpp"
    "${root}/src/*.c")

  add_executable(${name} ${service_sources})

  target_include_directories(${name} PRIVATE
    "${SECSVR_ROOT}/compat"
    "${SECSVR_ROOT}/tencrypt_new"
    "${SECSVR_ROOT}/tencrypt_new/include"
    "${SECSVR_ROOT}/tencrypt_new/inc"
    "${SECSVR_ROOT}/tencrypt_new/src"
    "${root}"
    "${root}/import/include"
  )

  # 原构建树把头安装到 output/<lib>/release/include；这里自动收集所有镜像目录。
  file(GLOB service_output_includes
    "${root}/output/*/release/include"
    "${root}/output/*/*/release/include"
    "${root}/output/*/*/*/release/include")
  if(service_output_includes)
    target_include_directories(${name} PRIVATE ${service_output_includes})
  endif()

  secsvr_compile_options(${name})
  target_link_libraries(${name} PRIVATE tencrypt_new rt dl m)
endfunction()
