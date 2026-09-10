// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inter_DispatcherAradOnly/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— Inter_DispatcherAradOnly 实现（报告 docs/class_func_reports/
// Inter_DispatcherAradOnly/）。C1 0x084e7eb8：基类 ISigDispatcher() + 设 vptr +
// Arad_InternalDispatcher C1 + Init()；dispatch_sig 0x084e7f2c：读 data 前 4
// 字节 code → GetDispatcher → 空 cMyTrace(0x6ba2,5)+getCurCharacNo 返 0x6ba3
// → 否则虚调槽2 dispatch_sig(data+4, len-4)。
#include "Inter_DispatcherAradOnly.h"

#include "GameTypes.h"  // cMyTrace
#include "CUser.h"      // CUserCharacInfo::getCurCharacNo

#include "Inter_DispatcherAradOnly/Inter_DispatcherAradOnly.inc"
{
    m_disp.Init();   // ORIG @0x084e7eeb call Arad_InternalDispatcher::Init()
}

#include "Inter_DispatcherAradOnly/dtor_Inter_DispatcherAradOnly.inc"
{
}

// [b28d-solo-Inter_DispatcherAradOnly] dispatch_sig 0x084e7f2c：
// ① 末参须写 `(int)((unsigned int)len - 4u)`——裸 `len - 4` 被 4.4 折成
//    `lea -0x4(%edx)`（N1 族）；unsigned 形复现 ORIG `mov 0x14(%ebp),%eax;
//    sub $0x4,%eax` 且把 `data + 4` 归到 %ecx（探针 /tmp/probe_sub.cpp g5；
//    语义等价：补码下 (unsigned)len-4u 与 len-4 同值）。
// ② 末尾虚调用目标寄存器 `call *%edx` 属 N7 敏感族（P230 补）：4.4.7 恒 %eax，
//    4.4.6 后端逐条复现 ORIG；本 TU 当前未路由，路由需求已上报 Main。
#include "Inter_DispatcherAradOnly/dispatch_sig.inc"
{
    int code = *(int*)data;   // ORIG mov (%eax),%eax 直读 4 字节（local_14）
    ARAD::DISPATCHER::Arad_IInterDispatcher* disp =
        m_disp.GetDispatcher((ARAD::ENUM_INTERNALPACKET_JPN)code);   // ORIG call GetDispatcher
    if (disp == 0)
    {
        // ORIG：cMyTrace 临时链式（getCurCharacNo 先求值存 ebx 跨构造调用缓存）
        cMyTrace(__PRETTY_FUNCTION__, 0x6ba2, 5)
            ("[DISPATCHER_ARAD] Invalid Internal Packet Code. (code:%d, charac:%u)",
             code, user->getCurCharacNo());
        return 0x6ba3;   // ORIG mov $0x6ba3,%eax
    }
    return disp->dispatch_sig(user, data + 4, (int)((unsigned int)len - 4u));   // ORIG 虚槽2（vptr+8）直调
}
