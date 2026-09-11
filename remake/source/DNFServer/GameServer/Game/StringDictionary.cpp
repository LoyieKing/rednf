// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StringDictionary/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "StringDictionary.h"

#include <algorithm>
#include <cctype>
#include <cstring>

// ORIG getStringHashValue 为文件局部 static（0x8ab21f8，nm t）。
//   文件级 `static`（非匿名 namespace）：mangled 恰为 _ZL18getStringHashValuePKc
//   与 ORIG 一致（匿名 namespace 会错发 _ZN12_GLOBAL__N_1…，Main 修正）。
//   hash = 0; for each non-null byte c: hash = hash*65599 + (signed char)c;
//   return hash.
static unsigned int getStringHashValue(const char* s)
{
    // [b28d-48] ORIG 0x08ab21f8：具名 char c 槽(-0x1) + `c=*s++` 先读后增
    // （@0x05 movzbl → @0x06 mov %al,-0x1 → @0x07 addl $1,0x8(%ebp) →
    //  @0x08 cmpb $0,-0x1 → je），for 形被 4.4 折成 ++s 后置。
    unsigned int hash = 0;
    char c;
    while (true)
    {
        c = *s++;
        if (c == '\0')
            break;
        // hash*0x40 + hash*0x10000 - hash + c = hash*0x40ff + c = hash*65599 + c
        hash = (hash << 6) + (hash << 16) - hash + (unsigned int)(signed char)c;
    }
    return hash;
}

#include "StringDictionary/StringDictionary.inc"


#include "StringDictionary/dtor_StringDictionary.inc"


#include "StringDictionary/addString.inc"


#include "StringDictionary/add.inc"


#include "StringDictionary/freeze.inc"


#include "StringDictionary/get.inc"


#include "StringDictionary/getCount.inc"


#include "StringDictionary/getKey.inc"


#include "StringDictionary/getValue.inc"


#include "StringDictionary/setValue.inc"

