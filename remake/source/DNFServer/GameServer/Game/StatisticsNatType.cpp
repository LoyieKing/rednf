// ============================================================================
// df_game_r 还原 —— StatisticsNatType（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsNatType.md + ORIG 反汇编。
// 布局：+0x00 map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>。
// NAT_TYPE_PEER_TO_PEER(0x8)/RESULT_COUNT(0x8)/SIG_NAT_TYPE(0x14) 见下。
// 类定义统一到权威头 GameWorld.h（StatisticsNatType/嵌套 key/value），
// 本 TU 仅保留方法定义。
// ============================================================================

#include <map>
#include <string.h>
#include <time.h>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameWorld.h"
#include "SigTypes_E.h"

// SIG_NAT_TYPE：权威占位在 SigTypes_E.h（char m_pad[0x14]；ORIG GetInBuffer 符号
// _ZN11CStreamGuard11GetInBufferI12SIG_NAT_TYPEEPT_v @0x08610f6e 使用权威类型）。
// 字段写入经同布局具名视图（P82 尾部视图惯例，不伪造对象生命周期）。
struct SIG_NAT_TYPE_VIEW
{
    int m_natA;     // +0x00
    int m_natB;     // +0x04
    int m_third;    // +0x08
    int m_fourth;   // +0x0c
    time_t m_time;  // +0x10
};

// ORIG 08610262：参数对按升序存储（m_a <= m_b）
StatisticsNatType::NAT_TYPE_PEER_TO_PEER::NAT_TYPE_PEER_TO_PEER(int a, int b)
{
    if (a > b)
    {
        m_a = b;
        m_b = a;
    }
    else
    {
        m_a = a;
        m_b = b;
    }
}

// ORIG 08610294：两个计数清零
StatisticsNatType::RESULT_COUNT::RESULT_COUNT()
{
    m_countA = 0;
    m_countB = 0;
}

StatisticsNatType::StatisticsNatType()
    : m_counts()
{
}

StatisticsNatType::~StatisticsNatType()
{
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_counts.begin();
    while (it != m_counts.end())
    {
        RESULT_COUNT* rc = it->second;
        if (rc != 0)
        {
            delete rc;
        }
        ++it;
    }
}

void StatisticsNatType::Reset()
{
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_counts.begin();
    while (it != m_counts.end())
    {
        RESULT_COUNT* rc = it->second;
        if (rc != 0)
        {
            rc->m_countB = 0;
            rc->m_countA = 0;
        }
        ++it;
    }
}

void StatisticsNatType::IncreaseNatType(int natA, int natB, unsigned char count)
{
    NAT_TYPE_PEER_TO_PEER key(natA, natB);
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_counts.find(key);

    RESULT_COUNT* rc = 0;
    if (it != m_counts.end())
    {
        rc = it->second;
    }
    else
    {
        rc = new RESULT_COUNT();
        m_counts.insert(std::make_pair(key, rc));
    }

    if (count != 0)
    {
        rc->m_countA += 1;
    }
    else
    {
        rc->m_countB += 1;
    }
}

void StatisticsNatType::UpdateNatType()
{
    time_t now = time(0);
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_counts.begin();
    while (true)
    {
        if (it == m_counts.end())
        {
            Reset();
            return;
        }

        RESULT_COUNT* rc = it->second;
        if (rc != 0 && (rc->m_countB > 0 || rc->m_countA > 0))
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x1c7), true);

            **guard << 0x83;
            **guard << -1;
            SIG_NAT_TYPE* sig = guard->GetInBuffer<SIG_NAT_TYPE>();
            memset(sig, 0, sizeof(SIG_NAT_TYPE));
            SIG_NAT_TYPE_VIEW* view = reinterpret_cast<SIG_NAT_TYPE_VIEW*>(sig);
            view->m_natA = it->first.m_a;
            view->m_natB = it->first.m_b;
            view->m_third = rc->m_countB;
            view->m_fourth = rc->m_countA;
            view->m_time = now;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
        }

        ++it;
    }
}
