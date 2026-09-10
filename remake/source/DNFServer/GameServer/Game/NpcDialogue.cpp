// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 NpcDialogue/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r NpcDialogue 实现（OrigDataClears 批，2026-08-28）。
// ORIG：ctor T 0x89fb688 / clear T 0x89fb71c（objdump 逐指令转写）。
#include "NpcDialogue.h"

#include <string>
#include <vector>

// ORIG ctor 0x89fb688：+0x0c/+0x10 string 与 +0x14 vector 由编译器默认构造，
// 函数体仅调 clear()（089fb6ba mov this,(%esp); call clear）。
#include "NpcDialogue/NpcDialogue.inc"
{
    clear();
}

// ORIG clear 0x89fb71c：
//   movl $0,(this)；for (i=0; i<=1; ++i) movl $0,0x4(this+i*4)（cmpl $0x1,setle）；
//   string@+0xc/0x10 operator=("")；vector<int>@+0x14 clear()。
#include "NpcDialogue/clear.inc"
{
    m_field0 = 0;                 // movl $0,(%eax)
    for (int i = 0; i <= 1; ++i)  // ORIG 循环头：jmp 到尾部 cmpl $0x1,setle
        m_field4[i] = 0;          // movl $0,0x4(%eax,%edx,4)
    m_str0c = "";                 // string::operator=(char const*)
    m_str10 = "";                 // string::operator=(char const*)
    m_vec14.clear();              // vector<int>::clear()
}
