// df_game_r Game/ CDungeonEntranceLog 实现（批3：从 GameStubs.cpp 迁移）。
// 语义依据 ORIG IncrementDungeonEntrance 0x860e1ee / DecrementDungeonEntrance 0x860e324 /
// InsertDungeonEntrance 0x860e59c / InsertDungeonEntrance_Hour 0x860e38e。
#include "CDungeonEntranceLog.h"

#include "GlobalData.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"

void CDungeonEntranceLog::IncrementDungeonEntrance(int dungeonIdx, bool isParty)
{
    // ORIG 0x860e1ee：存在路径 isParty 仅 ++m_partyEnter（不加 total/curInside）；
    // 插入路径 isParty 时 ++m_partyEnter 且 ++m_total/++m_curInside（不对称，忠实复刻）。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.find(dungeonIdx);
    if (it != m_inoutMap.end())
    {
        if (isParty)
        {
            it->second.m_partyEnter += 1;
        }
        else
        {
            it->second.m_enter += 1;
            it->second.m_total += 1;
            it->second.m_curInside += 1;
        }
    }
    else
    {
        STDungeonInoutCnt cnt;
        if (isParty)
        {
            cnt.m_partyEnter += 1;
        }
        else
        {
            cnt.m_enter += 1;
        }
        cnt.m_total += 1;
        cnt.m_curInside += 1;
        m_inoutMap.insert(std::make_pair(dungeonIdx, cnt));
    }
}

void CDungeonEntranceLog::DecrementDungeonEntrance(int dungeonIdx, bool /*isParty*/)
{
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.find(dungeonIdx);
    if (it != m_inoutMap.end())
        it->second.m_curInside -= 1;   // ORIG 节点 +0x10 -1（未命中不动）
}

void CDungeonEntranceLog::InsertDungeonEntrance()
{
    // ORIG 0x860e59c：map 非空时把 (key, m_enter, m_partyEnter) 写入 Statistics DB
    // 流（msg 0xe0, LOGDB_Q=4），随后清空 map。
    if (m_inoutMap.empty())
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x4da), true);
    **guard << 0xe0;
    **guard << -1;
    **guard << (int)m_inoutMap.size();
    for (std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
         it != m_inoutMap.end(); ++it)
    {
        **guard << it->first;
        **guard << it->second.m_enter;        // 节点 +0x4
        **guard << it->second.m_partyEnter;   // 节点 +0x8
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_inoutMap.clear();
}

void CDungeonEntranceLog::InsertDungeonEntrance_Hour()
{
    // ORIG 0x860e38e：map 非空时把 (key, m_total, m_curInside) 写入 Statistics DB
    // 流（msg 0xe1, LOGDB_Q=4），随后归零 m_total/m_curInside。
    if (m_inoutMap.empty())
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x4b5), true);
    **guard << 0xe1;
    **guard << -1;
    **guard << (int)m_inoutMap.size();
    for (std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
         it != m_inoutMap.end(); ++it)
    {
        **guard << it->first;
        **guard << it->second.m_total;       // 节点 +0xc
        **guard << it->second.m_curInside;   // 节点 +0x10
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    for (std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.begin();
         it != m_inoutMap.end(); ++it)
        it->second.m_curInside = 0;
}

CDungeonEntranceLog* GetInstanceDungeonEntranceLog()
{
    // ORIG _Z29GetInstanceDungeonEntranceLogv：guard 单例。
    static CDungeonEntranceLog instance;
    return &instance;
}
