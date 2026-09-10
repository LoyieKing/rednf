// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCommonStatisticsMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CCommonStatisticsMgr.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ---- CStatisticContainer ----

// ORIG 0x0860e7f8 T：先经 operator[] 取值入局部，再二次 operator[]
// 以 ++value 写回（保持 ORIG 的双调用与栈临时增量形态）。
#include "CCommonStatisticsMgr/IncreaseQuantity.inc"
{
    int value = m_quantityMap[quantity];
    m_quantityMap[quantity] = ++value;
}


// ---- CHelpedPartyCount::InsertDatabase（ORIG 0x0860e8aa T）----
// pool 流 0x512、msg 0x103、固定遍历键 0..4。
#include "CCommonStatisticsMgr/InsertDatabase.inc"
{
    if (m_quantityMap.size() == 0)
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x512),
                       true);
    **guard << 0x103;
    **guard << -1;
    for (int i = 0; i <= 4; ++i)
        **guard << m_quantityMap[i];
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_quantityMap.clear();
}

// ---- CPVPPlayerRate::InsertDatabase（ORIG 0x0860e9e0 T）----
// pool 流 0x525、msg 0x104、固定遍历键 0..5。
#include "CCommonStatisticsMgr/InsertDatabase_1.inc"
{
    if (m_quantityMap.size() == 0)
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x525),
                       true);
    **guard << 0x104;
    **guard << -1;
    for (int i = 0; i <= 5; ++i)
        **guard << m_quantityMap[i];
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_quantityMap.clear();
}

// ---- CPVPMapCount::InsertDatabase（ORIG 0x0860eb16 T）----
// pool 流 0x538、msg 0x105：-1 后写整表 size，再遍历输出全部 (key,value)。
#include "CCommonStatisticsMgr/InsertDatabase_2.inc"
{
    if (m_quantityMap.size() == 0)
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x538),
                       true);
    **guard << 0x105;
    **guard << -1;
    **guard << (int)m_quantityMap.size();
    for (std::map<int, int>::iterator it = m_quantityMap.begin(),
                                      endIt = m_quantityMap.end();
         it != endIt; ++it)
    {
        **guard << it->first;
        **guard << it->second;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_quantityMap.clear();
}

// ---- CCommonStatisticsMgr ----

// ORIG C1 0x86104e8 W：operator new(0x1c) × 3，各容器构造后依次存
// m_container[0..2]，每个 new 失败路径 delete 回滚。
#include "CCommonStatisticsMgr/CCommonStatisticsMgr.inc"
{
    m_container[0] = new CHelpedPartyCount;
    m_container[1] = new CPVPPlayerRate;
    m_container[2] = new CPVPMapCount;
}

// ORIG D1 0x8610432 W：i<=2 循环 delete 三容器
//（delete 空指针 no-op，编译器生成判空 + 静态 ~CStatisticContainer 0x8610412）。
#include "CCommonStatisticsMgr/dtor_CCommonStatisticsMgr.inc"
{
    for (int i = 0; i <= 2; ++i)
        delete m_container[i];
}

// ORIG 0x0860e838 T：边界检查 <0/>2 直接返回，无空指针分支（P1 数组组件寻址）。
#include "CCommonStatisticsMgr/IncreaseQuantity_1.inc"
                                            int quantity)
{
    if (container < 0 || container > 2)
        return;
    m_container[container]->IncreaseQuantity(quantity);
}

// ORIG 0x0860e86e T：i<=2 循环逐容器虚调 InsertDatabase，无空指针分支。
#include "CCommonStatisticsMgr/InsertDatabase_3.inc"
{
    for (int i = 0; i <= 2; ++i)
    {
        m_container[i]->InsertDatabase();
    }
}

// ORIG 0x860e765 T：函数内静态单例，__cxa_guard 构造-on-first-use +
// __cxa_atexit(~CCommonStatisticsMgr)；实例位于 ORIG BSS 0x94733ac。
// PvP_Room.cpp 经 GetInstanceCommonStatisticsMgr 直引。
CCommonStatisticsMgr* GetInstanceCommonStatisticsMgr()
{
    static CCommonStatisticsMgr instance;
    return &instance;
}


// ORIG 0x08610412 W：重置 vptr + map(+0x04) 析构（隐式生成即同形）。
#include "CCommonStatisticsMgr/dtor_CStatisticContainer.inc"
{
}
