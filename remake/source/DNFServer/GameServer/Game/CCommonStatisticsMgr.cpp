// ============================================================================
// df_game_r 还原 —— CCommonStatisticsMgr。
// 类生命周期：CStatisticContainer C1/D1 0x8610474/0x8610412（W），
// CHelpedPartyCount C1 0x8610494、CPVPPlayerRate C1 0x86104b0、
// CPVPMapCount C1 0x86104cc、CCommonStatisticsMgr C1/D1 0x86104e8/0x8610432；
// GetInstanceCommonStatisticsMgr 0x860e765 T。
// 三个 InsertDatabase：0x0860e8aa / 0x0860e9e0 / 0x0860eb16。
// 报告：docs/class_func_reports/{CCommonStatisticsMgr,CStatisticContainer,
// CHelpedPartyCount,CPVPPlayerRate,CPVPMapCount,_lt_global_gt_}/。
// ============================================================================
#include "CCommonStatisticsMgr.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ---- CStatisticContainer ----

// ORIG 0x0860e7f8 T：先经 operator[] 取值入局部，再二次 operator[]
// 以 ++value 写回（保持 ORIG 的双调用与栈临时增量形态）。
void CStatisticContainer::IncreaseQuantity(int quantity)
{
    int value = m_quantityMap[quantity];
    m_quantityMap[quantity] = ++value;
}


// ---- CHelpedPartyCount::InsertDatabase（ORIG 0x0860e8aa T）----
// pool 流 0x512、msg 0x103、固定遍历键 0..4。
void CHelpedPartyCount::InsertDatabase()
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
void CPVPPlayerRate::InsertDatabase()
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
void CPVPMapCount::InsertDatabase()
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
CCommonStatisticsMgr::CCommonStatisticsMgr()
{
    m_container[0] = new CHelpedPartyCount;
    m_container[1] = new CPVPPlayerRate;
    m_container[2] = new CPVPMapCount;
}

// ORIG D1 0x8610432 W：i<=2 循环 delete 三容器
//（delete 空指针 no-op，编译器生成判空 + 静态 ~CStatisticContainer 0x8610412）。
CCommonStatisticsMgr::~CCommonStatisticsMgr()
{
    for (int i = 0; i <= 2; ++i)
        delete m_container[i];
}

// ORIG 0x0860e838 T：边界检查 <0/>2 直接返回，无空指针分支（P1 数组组件寻址）。
void CCommonStatisticsMgr::IncreaseQuantity(ENUM_STATISTIC_CONTAINER container,
                                            int quantity)
{
    if (container < 0 || container > 2)
        return;
    m_container[container]->IncreaseQuantity(quantity);
}

// ORIG 0x0860e86e T：i<=2 循环逐容器虚调 InsertDatabase，无空指针分支。
void CCommonStatisticsMgr::InsertDatabase()
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
CStatisticContainer::~CStatisticContainer()
{
}
