// ============================================================================
// df_game_r 还原 —— CCommonStatisticsMgr（S5 语义对齐批次）。
// ORIG：mgr ctor 0x86104e8（new 三个子容器）；三个 InsertDatabase
// 0x0860e8aa / 0x0860e9e0 / 0x0860eb16；GetInstanceCommonStatisticsMgr
// 0x860e765。布局：+0x00 CStatisticContainer* m_container[3]。
// ============================================================================
#include "CCommonStatisticsMgr.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ---- CStatisticContainer::IncreaseQuantity（ORIG 0x0860e7f8）----
void CStatisticContainer::IncreaseQuantity(int quantity)
{
    // ORIG：m_quantityMap(quantity) 取引用后自增（键即 quantity 参数）。
    ++m_quantityMap[quantity];
}
void CStatisticContainer::insertDatabaseCommon(int packetId, bool fixedFive)
{
    if (m_quantityMap.empty())
        return;

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CommonStatisticsMgr.cpp",
                                           packetId == 0x103
                                               ? 0x512
                                               : packetId == 0x104 ? 0x525
                                                                   : 0x538),
        true);
    **guard << packetId;
    **guard << (int)-1;
    if (fixedFive)
    {
        // CHelpedPartyCount/CPVPPlayerRate：固定遍历键 0..4。
        for (int i = 0; i <= 4; ++i)
            **guard << m_quantityMap[i];
    }
    else
    {
        // CPVPMapCount：遍历整张 map。
        for (std::map<int, int>::iterator it = m_quantityMap.begin();
             it != m_quantityMap.end(); ++it)
        {
            **guard << it->first;
            **guard << it->second;
        }
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_quantityMap.clear();
}

void CHelpedPartyCount::InsertDatabase() { insertDatabaseCommon(0x103, true); }
void CPVPPlayerRate::InsertDatabase() { insertDatabaseCommon(0x104, true); }
void CPVPMapCount::InsertDatabase() { insertDatabaseCommon(0x105, false); }

// ---- CCommonStatisticsMgr（ORIG ctor 0x86104e8）----
CCommonStatisticsMgr::CCommonStatisticsMgr()
{
    m_container[0] = new CHelpedPartyCount;   // sizeof 0x1c
    m_container[1] = new CPVPPlayerRate;
    m_container[2] = new CPVPMapCount;
}

void CCommonStatisticsMgr::IncreaseQuantity(ENUM_STATISTIC_CONTAINER container,
                                            int quantity)
{
    if (container < 0) return;
    if (container > 2) return;
    if (m_container[container] == NULL) return;
    m_container[container]->IncreaseQuantity(quantity);
}

void CCommonStatisticsMgr::InsertDatabase()
{
    for (int i = 0; i <= 2; ++i)
    {
        if (m_container[i] == NULL) continue;
        m_container[i]->InsertDatabase();
    }
}

// ORIG 0x860e765 T（_Z30GetInstanceCommonStatisticsMgrv）：函数内静态单例，
// __cxa_guard 构造-on-first-use + __cxa_atexit 析构；实例位于 ORIG BSS
// 0x94733ac。PvP_Room.cpp 经 sub_GetInstanceCommonStatisticsMgr 直引。
CCommonStatisticsMgr* GetInstanceCommonStatisticsMgr()
{
    static CCommonStatisticsMgr instance;
    return &instance;
}
