// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CData_charac_expand/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ charac_expand::CData 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 声明以 CBoosterGage.h 为准。
#include "CBoosterGage.h"

namespace charac_expand
{

#include "CData_charac_expand/CData.inc"
{
    // ORIG W 0x80c8cba：仅置基类 vptr（编译器自动）；不触碰 m_bIsSave。
}

#include "CData_charac_expand/dtor_CData.inc"
{
}

#include "CData_charac_expand/alter.inc"
{
    m_bIsSave = 1;
}

// ORIG _ZN13charac_expand5CData6isSaveEv W 0x832ae80：`movzbl 0x4(%eax); ret` 直返
// m_bIsSave 字节（无 test;setne 布尔归一）。bool 字段 + bool 返回时 4.4 同样直返
// （探针 /tmp/l11p/issave.cpp：bool 字段 isSave 体 = movzbl;ret，调用点 = test %al;je；
// char 返回则调用点物化 setne——P73）。签名 mangled 不含返回类型。
#include "CData_charac_expand/isSave.inc"
{
    return m_bIsSave;
}

#include "CData_charac_expand/saveData.inc"
{
    // [b28c-11] NEAR 封顶（ORIG 0x832ae8c）：残余 = 虚调用 callee 寄存器
    // %edx↔%eax 互换（`mov (%eax),%edx; …call *%edx` vs `mov (%eax),%eax; …call *%eax`），
    // N7 族（4.4 间接 call 目标恒 %eax），助记符序列全等。
    bool result = _saveData(pUser);
    m_bIsSave = 0;
    return result;
}

#include "CData_charac_expand/reset.inc"
{
    // [b28c-11] NEAR 封顶（ORIG 0x832aebc）：同 saveData（N7 虚调用寄存器互换）。
    m_bIsSave = 0;
    _reset();
}

}  // namespace charac_expand
