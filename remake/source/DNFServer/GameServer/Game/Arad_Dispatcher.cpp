// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_Dispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Arad_Dispatcher.h"

#include <string.h>
#include <stdio.h>
#include <time.h>

#include "GlobalData.h"        // s_db_mgr / s_stream_pool / s_msgq_mgr / s_pCeraShop / s_pItemVendingMachine
#include "Stream.h"            // Stream operator>> / GetOutBuffer<T>
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "DBMgr.h"             // DBMgr 完整类型（GlobalData::s_db_mgr->GetDBHandle）
#include <stddef.h>
#include "MySQL.h"
#include "LogManager.h"
#include "SigTypes.h"          // MSG_BUY_CERASHOP_ITEM（0x3f84）
#include "MoreTypes.h"         // ARAD::INTERNAL_PACKET::PURCHASE_COUNT_CHECK
#include "CCeraShop.h"         // WongWork::CCeraShop::ProcessError
#include "ItemVendingMachine.h"
#include "CUser.h"

// 布局断言：ORIG Manager ctor 0x0818fdd4 两组各 0x354 指针数组，size = 0x1AA0。
typedef char AradDispatcherManagerSizeAssert[
    sizeof(ARAD::Arad_Dispatcher_Manager) == 0x354 * 4 * 2 ? 1 : -1];

// b28c-L26 [判词 N·跨版本 new-expression landing-pad 抑制，模板实例变体]
// 实测在 Manager ctor 定义**之前**放成员函数显式实例化
// （`template void GlobalInstance<ARAD::Arad_Dispatcher_Manager>::create();`）
// 仍产 24 条形（无 cleanup 岛）——4.4 对模板成员的代码生成延至 TU 末尾，
// 届时 ctor 体恒可见 ⇒ 判 nothrow 抑 landing pad。P210/BM11「定义前置」翻案
// 仅适用**非模板** caller（open()/NexonCash C1）；对 GlobalInstance<T>::create()
// 这类头模板成员无源码解（禁改头/禁删 ctor）。ORIG 37 条 = 双检 + new(0x1aa0)
// + ctor call + `mov %ebx,%eax; mov %eax,m_p` + 正常路径逐条等价，差额 13 条
// 全落在 ctor 抛异常的 delete+_Unwind_Resume 清理岛及其 push %edi/%esi/帧 0x1c。
// 已回退该无效实例化声明，保持纯自然形。
// [b28d-23 N·N100/模板] 复测维持：模板成员 create 的 landing pad 抑制无源码解
// （P210 定义前置仅适用非模板 caller）⇒ N。ORIG 0x0818e956
// [S5AradDisp 2026-09-11 复核维持 N] ORIG 0x0818e956 的 13 条独有段 = EH cleanup 岛
//   `0x0818e98b mov %edx,%esi; mov %eax,%edi; mov %ebx,(%esp); call _ZdlPv;
//    mov %edi,%eax; mov %esi,%edx; mov %eax,(%esp); call _Unwind_Resume` + 连带
//   push %edi/%esi 与帧 0x1c（我方 0x14）。GCC 只在 4.4 判定被调 ctor 可能抛时才发射该岛；
//   本 TU 内 Manager ctor（0x0818fdd4 同 TU，AE 26/26）定义可见且体不抛 ⇒ 4.4 判 nothrow
//   抑制；模板成员 create 的代码生成延至 TU 末（届时 ctor 定义恒可见），定义前置/显式
//   实例化声明均无效（已三轮实测）⇒ 维持 N。我方 gap +13（orig 37 / our 24）。]

// ---------------------------------------------------------------------------
// 本 TU 实例化：GlobalInstance<ARAD::Arad_Dispatcher_Manager>::create()  @0x0818e956
// （报告 docs/class_func_reports/StrikerScript/create.md 属撞名——其 `## 反编译 C` 段为
// StrikerScript::create@0x8a9dc70，与本符号无关；verdict 以 nm 精确切片的 ORIG 汇编为权威，
// §6.3）。
// [b23判词·跨版本 N（new-expression landing-pad 抑制）+ AE 假阳性] 双检（两次
// `mov m_p,%eax; test; jne`）、`movl $0x1aa0` → new → Manager ctor → `mov %ebx,%eax;
// mov %eax,m_p` → `jmp` + `nop` 岛与 ORIG 正常路径逐条相等；37−24=13 条差额全部落在
// ctor 抛异常的清理岛（operator delete + _Unwind_Resume）及其连带的 push %edi/%esi 与
// 帧 0x1c/0x14（根因同 open()，见其判词：基类 ctor/dtor 本 TU 可见空定义 ⇒ 4.4 推断
// nothrow 抑制 landing pad，/tmp 探针 A/B/C/D 实证）。另有 3 处 m_p 数据地址与
// 2 处 call 目标显示为 .o 重定位占位（§6.3 假阳性），relink 后转 IDENTICAL_AE。
// ---------------------------------------------------------------------------

namespace ARAD
{

#include "Arad_Dispatcher/db_dispatch.inc"


#include "Arad_Dispatcher/open.inc"


#include "Arad_Dispatcher/Arad_Dispatcher_BuyItemCheck_Impl.inc"


#include "Arad_Dispatcher/Arad_Dispatcher.inc"


#include "Arad_Dispatcher/dtor_Arad_Dispatcher.inc"


#include "Arad_Dispatcher/inter_dispatch.inc"


#include "Arad_Dispatcher/register_dispatcher.inc"


#include "Arad_Dispatcher/unregister_dispatcher.inc"


#include "Arad_Dispatcher/Arad_Dispatcher_Manager.inc"


#include "Arad_Dispatcher/dtor_Arad_Dispatcher_Manager.inc"


#include "Arad_Dispatcher/register_dispatcher_1.inc"


#include "Arad_Dispatcher/unregister_dispatcher_1.inc"


#include "Arad_Dispatcher/inter_dispatch_1.inc"


#include "Arad_Dispatcher/db_dispatch_1.inc"


#include "Arad_Dispatcher/dtor_Arad_Dispatcher_BuyItemCheck_Impl.inc"
{
}

#include "Arad_Dispatcher/inter_dispatch_2.inc"
                                                      char* body, int /*len*/)
{
    // P82/P99：本地 pack(1) 尾部视图使 4.4 折叠 `mov 0x3a(%eax),%eax`
    // （裸 (b+0x3a) cast 恒 add 分解，探针 /tmp/l18p/p.cpp）。
    #pragma pack(push, 1)
    struct ItemIdxView { char m_pad[0x3a]; unsigned int m_idx; };
    #pragma pack(pop)
    // ORIG 0x0818ea3e：
    //   body[0]==1 → ItemVendingMachine::BuyItem(user, *(MSG*)(body+1))；
    //               成功返回 0，失败 logFormat(…0x40…) 返回 -1。
    //   body[0]!=1 → CCeraShop::ProcessError(s_pCeraShop,user,0x74,0,0,1,1,0)，返回 0。
    // ORIG 0x0818ea44：body 形参 spill 至 -0xc 槽（P0 具名局部）；0x0818ea4a
    // `movzbl (%eax),%eax; xor $0x1,%eax; test; je`＝ `!*(bool*)b` 直判形
    // （xor 精确判 b==1，P171 原码 bool 视角照抄）。
    char* b = body;
    if (!*(bool*)b)
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x74, 0, 0, 1, 1, '\0');
        return 0;
    }
    if (!GlobalData::s_pItemVendingMachine->BuyItem(
            user, *(const MSG_BUY_CERASHOP_ITEM*)(b + 1)))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
            __PRETTY_FUNCTION__, 0x40,
            "[ERROR] BuyItem() Result Error ItemIdx(%d)",
            ((const ItemIdxView*)b)->m_idx);
        return -1;
    }
    return 0;
}

#include "Arad_Dispatcher/db_dispatch_2.inc"


#include "Arad_Dispatcher/GetQuarterOfYear.inc"
{
    time_t t;
    time(&t);
    struct tm tmp = *localtime(&t);
    sprintf(buf, "%02d%d",
            (tmp.tm_year + 1900) - ((tmp.tm_year + 1900) / 2000) * 2000,
            tmp.tm_mon / 3 + 1);
}

#include "Arad_Dispatcher/arad_sp_buy_item_check.inc"



#include "Arad_Dispatcher/dispathc_code.inc"


}  // namespace ARAD

