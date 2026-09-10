// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 VendingMachine/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>

#include "VendingMachine.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "TimerQueue.h"

// ----------------------------------------------------------------------------
// 构造 / 析构（ORIG 0x0861062c C1/C2、0x08610694 D1/D2）
// ----------------------------------------------------------------------------
#include "VendingMachine/CVendingMachineStatistic.inc"
{
    // ORIG ctor：构造 vector@+0x00 与 map@+0x10（自动），随后调 _init()。
    _init();
}

#include "VendingMachine/dtor_CVendingMachineStatistic.inc"
{
    // ORIG dtor：析构 map@+0x10（+0x00 vector 自动析构）。
}

// ----------------------------------------------------------------------------
// _init（ORIG 0x086105ac）
// ----------------------------------------------------------------------------
#include "VendingMachine/_init.inc"
{
    // ORIG：m_useCount=0；m_usedItems 追加 0x1d1b/0x1d1c/0x1d1d 三项；
    // m_output.clear()。
    m_useCount = 0;
    m_usedItems.push_back(0x1d1b);   // 追踪的售货机物品类型索引（ORIG 3 项）
    m_usedItems.push_back(0x1d1c);
    m_usedItems.push_back(0x1d1d);
    m_output.clear();
}

// ----------------------------------------------------------------------------
// incUseCount（ORIG 0x0822d714）
// ----------------------------------------------------------------------------
#include "VendingMachine/incUseCount.inc"
{
    m_useCount += 1;
}

// ----------------------------------------------------------------------------
// addOutputItem（ORIG 0x0860f5a4）
// ----------------------------------------------------------------------------
// [b28c-31] AE：`if (it == end) insert else +=` 正向形复现 ORIG 0x0860f6xx
// `call eq; test; je <+= 块>` 布局（95=95）。
#include "VendingMachine/addOutputItem.inc"
                                             unsigned int count)
{
    // ORIG：若 itemIdx 属于 m_usedItems（追踪类型之一），则在 m_output 中
    // 累加 count；否则 ignore（直接返回）。
    std::vector<unsigned long>::iterator found =
        std::find(m_usedItems.begin(), m_usedItems.end(), itemIdx);
    if (found == m_usedItems.end())
        return;

    std::map<unsigned long, unsigned int>::iterator it = m_output.find(itemIdx);
    // [b28c-31] ORIG 0x0860f6xx：`call eq; test %al,%al; je <+= 块>`（eq 真即
    // it==end 时 fall-through 到 insert 块）⇒ 源码须写 `== end` 正向形。
    if (it == m_output.end())
        m_output.insert(std::make_pair(itemIdx, count));
    else
        it->second += count;
}

// ----------------------------------------------------------------------------
// updateDataIntoDB（ORIG 0x0860f6de）
// ----------------------------------------------------------------------------
// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 见 class_func_reports/CVendingMachineStatistic/updateDataIntoDB.md。
#include "VendingMachine/updateDataIntoDB.inc"
{
    // ORIG：Acquire Stream("Statistics.cpp",0x624) → CStreamGuard(stream,true) →
    // 写 SIG=0x163, -1, m_useCount, m_output.size(), 每键值对 →
    // MsgQueueMgr::put(LOGDB_Q) → _init()。
    Stream* stream = GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x624);
    CStreamGuard guard(stream, true);

    **guard << 0x163;                    // SIG（DB 侧据此路由）
    **guard << (int)-1;                  // 固定 -1
    **guard << m_useCount;               // 使用次数
    **guard << (unsigned int)m_output.size();

    for (std::map<unsigned long, unsigned int>::const_iterator it =
             m_output.begin();
         it != m_output.end(); ++it)
    {
        **guard << it->first;            // 物品索引（unsigned long）
        **guard << it->second;           // 数量（unsigned int）
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    _init();
}

// ----------------------------------------------------------------------------
// GetInstanceVendingMachineStatistic（ORIG 0x0860f8ad）—— 函数内静态单例
// ----------------------------------------------------------------------------
CVendingMachineStatistic* GetInstanceVendingMachineStatistic()
{
    static CVendingMachineStatistic instance;
    return &instance;
}

// ----------------------------------------------------------------------------
// TimerVendingMachineItemNotify::registTimer（ORIG 0x08638074）—— static
// ----------------------------------------------------------------------------
#include "VendingMachine/registTimer.inc"
                                                unsigned int count,
                                                unsigned int type)
{
    // ORIG：G_TimerQueue()->InsertTimer(OBJ_TYPE_0, param_1, TIMER_MESSAGE(0xa),
    //   sec=param_4, param2=param_2, param3=param_3)。
    // P24：实参内联——itemIdx/count 物化 callee-saved 跨 G_TimerQueue call。
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid, (TIMER_MESSAGE)0xa,
                                (int)type, (int)itemIdx, count);
}
