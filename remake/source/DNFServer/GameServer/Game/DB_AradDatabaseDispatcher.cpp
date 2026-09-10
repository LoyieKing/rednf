// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_AradDatabaseDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— DB_AradDatabaseDispatcher 实现（报告 docs/class_func_reports/
// DB_AradDatabaseDispatcher/）。C1 0x0844bc38：基类 DBDispatcher() + 设 vptr +
// Arad_DatabaseDispatcher C1 + Init()；dispatch 0x0844bcac：Stream>>code →
// GetDispatcher → 空 cMyTrace(0xbc8b,5) 返 false → 否则虚调槽2 dispatch。
#include "DB_AradDatabaseDispatcher.h"

#include "GameTypes.h"  // cMyTrace

#include "DB_AradDatabaseDispatcher/DB_AradDatabaseDispatcher.inc"
{
    m_disp.Init();   // ORIG @0x0844bc6b call Arad_DatabaseDispatcher::Init()
}

#include "DB_AradDatabaseDispatcher/dtor_DB_AradDatabaseDispatcher.inc"
{
}

// [b28c-32] verdict=NEAR：N7 虚调用 callee 寄存器互换（50=50）。
#include "DB_AradDatabaseDispatcher/dispatch.inc"
{
    int code = 0;   // ORIG movl $0,-0x20(%ebp)
    *stream >> code;   // ORIG call Stream::operator>>(int&)

    ARAD::DISPATCHER::Arad_IDatabaseDispatcher* disp =
        m_disp.GetDispatcher((ARAD::ENUM_INTERNALPACKET_JPN)code);   // ORIG call GetDispatcher
    if (disp == 0)
    {
        // ORIG：cMyTrace 临时链式（code 提前入 ebx 跨构造调用缓存）
        cMyTrace(__PRETTY_FUNCTION__, 0xbc8b, 5)
            ("[DISPATCHER_ARAD] Invalid Internal Packet Code. (code:%d)",
             code);
        return false;   // ORIG mov $0,%eax
    }
    return disp->dispatch(userIdx, slot, stream);   // ORIG 虚槽2（vptr+8）直调
}
