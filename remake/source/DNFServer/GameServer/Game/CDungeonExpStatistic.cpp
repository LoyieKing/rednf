// ============================================================================
// df_game_r 还原 —— CDungeonExpStatistic::InsertDatabase。
// ORIG 0x0860f110 T（_ZN20CDungeonExpStatistic14InsertDatabaseEv）。
// 报告：docs/class_func_reports/CDungeonExpStatistic/InsertDatabase.md
// 语义：统计表空则直接返回；否则从 s_stream_pool 取流（Statistics.cpp:0x59c），
//   写消息头 0x116、-1、表 size，再遍历每节点输出
//   (level, m_exp, m_count, m_clearTimes) 四字段，
//   最后 put(LOGDB_Q) 入库并 clear 统计表。
// IncreaseDungeonExp 实现在 PartyClosureC2.cpp（ORIG 0x0860efca）。
// ============================================================================
#include "CDungeonExpStatistic.h"

#include "AdditionalDungeonInfo.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ============================================================================
// AdditionalDungeonInfo::~AdditionalDungeonInfo —— ORIG D1/D2 0x08a6887c（W，空体）。
// 非多态 POD（8 字节两 int，无 vtable/typeinfo）；属主 TU 提供真实空析构，
// 供 vector<AdditionalDungeonInfo>（QuestScript::m_field16c）析构实例化引用。
// ============================================================================
AdditionalDungeonInfo::~AdditionalDungeonInfo()
{
}

void CDungeonExpStatistic::InsertDatabase()
{
    if (m_map.size() == 0)
        return;
    // e1/e2/e3 按 ORIG 栈槽分配序（gcc4.1 由低到高 e3/e2/e1）倒序声明，
    // gcc4.4 按声明序从高地址往下分配，倒排后槽位与 ORIG 对齐。
    stDungeonExp e3;
    stDungeonExp e2;
    stDungeonExp e1;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x59c),
                       true);
    **guard << 0x116;
    **guard << -1;
    **guard << (int)m_map.size();
    std::map<int, stDungeonExp>::iterator endIt = m_map.end();
    for (std::map<int, stDungeonExp>::iterator it = m_map.begin();
         it != endIt; ++it)
    {
        **guard << it->first;
        e1 = it->second;
        **guard << e1.m_exp;
        e2 = it->second;
        **guard << e2.m_count;
        e3 = it->second;
        **guard << e3.m_clearTimes;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_map.clear();
}
