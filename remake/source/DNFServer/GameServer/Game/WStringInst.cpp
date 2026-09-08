// WStringInst.cpp —— std::basic_string<wchar_t> 显式实例化 TU（libstdc++ wstring DIFF 收敛）。
// 背景：ORIG 侧 wstring 成员实例来自 Red Hat 32 位默认旗标（-march=i486 -mtune=generic）
// 构建的 libstdc++ wstring-inst.o；我们侧链接器从 c6root libstdc++.a (4.4.4/32) 拉入的
// 归档版本在 16 件上差 if-conversion/cmov 族（4.4 默认 -march=i686 才发 cmov）。
// 本 TU 显式实例化同符号集，链接器优先采用本 TU 定义。
//
// 编译路由（CMake SRC_OPTS）：-O2 -std=gnu++0x -march=i486 -mtune=generic
//   -march=i486：复现 ORIG 无 cmov 的条件跳转形（16 件残差根因，b28c Main 实证）；
//   -std=gnu++0x：4.4 头补出 initializer_list 族 7 符号（gnu++98 头无这些成员）。
// 单 TU 验证（check_tu_game_orig.sh 本文件同旗标，compare 口径）：
//   函数 153 个：identical=25 ae=128 diff=0（全量零差，含原 16 件 + 原 7 件归档兜底）。
//
// NEW 符号说明（ORIG 无）：GCC 4.4 对显式实例化的 ctor/dtor 生成 C5/D5 NOTYPE/LOCAL 同址
// 别名（无代码体，非 ABI 函数符号）；链接产物需 objcopy -N 清理（Main 收尾步骤）：
// 命令清单 source/toolchain/wstri_objcopy_cmds.sh（用法 sh wstri_objcopy_cmds.sh <最终ELF>）。
// 对 .o 实证：清理后 compare verdict 不变（identical=25 ae=128 diff=0），FUNC/WEAK 全集无变化。
#include <string>

template class std::basic_string<wchar_t,
                                 std::char_traits<wchar_t>,
                                 std::allocator<wchar_t> >;
