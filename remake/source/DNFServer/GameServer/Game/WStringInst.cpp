// WStringInst.cpp —— std::basic_string<wchar_t> 显式实例化 TU（libstdc++ wstring DIFF 收敛）。
// 背景：ORIG 侧 wstring 成员实例来自 DF 重编 libstdc++ 4.1.2 (-O2, gnu++0x 模式)；
// 我们侧链接器从 c6root libstdc++.a (4.4.4/32, gnu++0x) 拉入 wstring-inst.o，
// 产生 68(l12)+32(lane10) 件 DIFF（/tmp/l12_final.tsv、/tmp/lane10_fresh.txt）。
// 本 TU 以 4.4.7 头文件 gnu++98 -O2 显式实例化提供同符号集定义，链接器优先采用
// 本 TU 定义、不再从归档拉入这些成员。
//
// 单 TU 验证（check_tu_game_orig.sh 本文件 -O2 -std=gnu++98，compare 口径）：
//   函数 122 个：identical=25 ae=97 diff=24 missing_in_game=10(仅 .o 内 local 别名)。
//   差异清单内 100 件：identical=2 ae=75 diff=16 归档兜底=7
//   （7 件 = initializer_list 族 5 + 迭代器 ctor C1IP{K,}w 2，gnu++98 头无法
//    产生，继续归档拉入维持原状；逐符号账 /tmp/wstri_verdict.tsv）。
//   16 件残余 DIFF：cmov/if-conversion 族（copy/_M_limit/compare 族/rfind/
//   find_last_of 族）+ l12 侧 3 件（_Rep9_S_create/eraseEjj/replaceEjjjw 等），
//   均为 4.4 头固有差（4.1.2 vs 4.4.4 O2 中端），与 DL10 探针结论一致；
//   -fno-if-conversion 实测更差（32 DIFF），不采用。
//
// NEW 符号说明（ORIG 无）：GCC 4.4 对显式实例化的 ctor/dtor 生成 10 个
// NOTYPE/LOCAL 的 C5/D5 同址别名（无代码体，非 ABI 函数符号）；另有 10 个
// LOCAL/SECTION 的 C2/D2 别名，链接后消失（ORIG ELF 无具名 .text.* 实证）。
// 链接产物需 objcopy -N 清 C5/D5 别名（Main 收尾步骤，已批准启用）：
// 命令清单 /tmp/wstri_objcopy_cmds.sh（用法 sh wstri_objcopy_cmds.sh <最终ELF>），
// 符号名细目 /tmp/wstri_new.tsv。对 .o 实证：清理后 compare verdict 不变
// （identical=25 ae=97 diff=24），FUNC/WEAK 全集无任何变化。
#include <string>

template class std::basic_string<wchar_t,
                                 std::char_traits<wchar_t>,
                                 std::allocator<wchar_t> >;
