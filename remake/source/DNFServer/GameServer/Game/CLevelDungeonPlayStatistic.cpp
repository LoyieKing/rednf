// ============================================================================
// [b28d-38 判词] CLevelDungeonPlayStatistic::InsertDatabase 0x0860edf4 DIFF：
//  p167 CLEAR 28/28（语义完成）；残差 N18a 单段直落（ORIG mov 0x8;mov 0x4;mov 双段）。
// df_game_r 还原 —— CLevelDungeonPlayStatistic::InsertDatabase。
// ORIG 0x0860edf4 T（_ZN26CLevelDungeonPlayStatistic14InsertDatabaseEv）。
// 报告：docs/class_func_reports/CLevelDungeonPlayStatistic/InsertDatabase.md
// 语义：统计表空则直接返回；否则从 s_stream_pool 取流（Statistics.cpp:0x569），
//   写消息头 0x107、-1、表 size，再遍历每节点输出
//   (level, m_userCount, m_playCount) 三字段，
//   最后 put(LOGDB_Q) 入库并 clear 统计表。
// IncreaseLevelDungeonPlay 实现在 GlobalData.cpp（ORIG 0x0860ecc6）。
// ============================================================================
#include "CLevelDungeonPlayStatistic.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// [b28c-32] verdict=DIFF：N18a（ORIG it->second 成员读发 8 字节两段载入，4.4 恒折单条）。
void CLevelDungeonPlayStatistic::InsertDatabase()
{
    if (m_levelPlayMap.size() == 0)
        return;
    // ORIG 栈槽（低→高）：end@-0x18、it@-0x14、guard@-0x10；
    // gcc4.4 按声明序从高地址往下分配，故声明序 guard → it → endIt。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x569),
                       true);
    **guard << 0x107;
    **guard << -1;
    **guard << (int)m_levelPlayMap.size();
    std::map<int, stDungeonPlayInfo>::iterator it = m_levelPlayMap.begin();
    std::map<int, stDungeonPlayInfo>::iterator endIt = m_levelPlayMap.end();
    while (it != endIt)
    {
        // pair 起始即 key；value（stDungeonPlayInfo）@+4，故 value+4=m_userCount、
        // value+8=m_playCount（ORIG 经 iterator::operator-> 读 +0/+4/+8）。
        **guard << it->first;
        **guard << it->second.m_userCount;
        **guard << it->second.m_playCount;
        ++it;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    m_levelPlayMap.clear();
}
