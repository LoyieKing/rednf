// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CodeHackChecksum/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__Z18ZeroMemoryChecksumP16CodeHackChecksum`（ORIG 0x808ac67）。
#include <iostream>


#include "CodeHackChecksum.h"

#include <cstring>

#include "SyncScriptDeps.h"   // toTString(const char*)

// W7_BuildHarmonize：struct/extern 由权威头 ShaSupport.h 取代（原 TU 局部
// 定义移出，保证唯一 owner 与全局命名空间 mangled 匹配）。
#include "ShaSupport.h"

static_assert(sizeof(sha256_context) == 0x68, "sha256_context layout drift");

#include "CodeHackChecksum/ZeroMemoryChecksum.inc"


#include "CodeHackChecksum/CalculateMemoryChecksum.inc"


#include "CodeHackChecksum/CalculateFileChecksum.inc"


// ===========================================================================
// class CodeHackChecksum —— POD；~CodeHackChecksum W @0x0808b7da 空体。
// ===========================================================================
// [b28d-Main 代落·S329 P299/P317] 用户 ctor/dtor 定义删除（声明端同步）：
// ORIG 无 C1/C2 符号，D1/D2 由使用点（new_allocator::destroy）发射 W。

#include "CodeHackChecksum/CodeHackChecksumData.inc"

#include "CodeHackChecksum/SetCodeHackCheckRange.inc"


#include "CodeHackChecksum/dtor_CodeHackChecksumData.inc"


#include "CodeHackChecksum/CodeHackChecksumData_1.inc"


#include "CodeHackChecksum/Clear.inc"


#include "CodeHackChecksum/Reserve.inc"


#include "CodeHackChecksum/AddCodeHackChecksum.inc"


#include "CodeHackChecksum/IsRightChecksum.inc"


#include "CodeHackChecksum/CodeHackChecksumEx.inc"


#include "CodeHackChecksum/dtor_CodeHackChecksumEx.inc"
                                                         // 随后隐式基类/vector 析构

// [b28d-lane16 N84②] 拷贝构造不定义：ORIG 0x08575f10 为 W 隐式合成（基类 C1ERKS
// + m_fileName rep movsl 0x41 + m_range 双 int 直拷）；显式体恒 call memcpy（24 vs 32）。
// 隐式发射由本 TU PopData 的 `CodeHackChecksumEx tmp(m_queue.front())` 保证（ODR 使用点）。

#include "CodeHackChecksum/SetFileName.inc"


#include "CodeHackChecksum/GetFileName.inc"



#include "CodeHackChecksum/GetCodeHackCheckRange.inc"


#include "CodeHackChecksum/setData.inc"


#include "CodeHackChecksum/IsRightChecksum_1.inc"


#include "CodeHackChecksum/CodeHackCheckRangeEx.inc"


#include "CodeHackChecksum/SetFileName_1.inc"


#include "CodeHackChecksum/GetFileName_1.inc"


#include "CodeHackChecksum/setData_1.inc"


#include "CodeHackChecksum/CodeHackCheckStorage.inc"


#include "CodeHackChecksum/dtor_CodeHackCheckStorage.inc"


#include "CodeHackChecksum/IsEmpty.inc"


#include "CodeHackChecksum/IsFull.inc"


#include "CodeHackChecksum/PushData.inc"


#include "CodeHackChecksum/PopData.inc"


#include "CodeHackChecksum/PopData_1.inc"


#include "CodeHackChecksum/IsRightChecksum_2.inc"


#include "CodeHackChecksum/reset.inc"

