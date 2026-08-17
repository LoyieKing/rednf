// ============================================================================
// df_game_r 还原 —— StatisticsNatType（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsNatType.md + ORIG 反汇编。
// 布局：+0x00 map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>。
// NAT_TYPE_PEER_TO_PEER(0x8)/RESULT_COUNT(0x8)/SIG_NAT_TYPE(0x14) 见下。
// ============================================================================

#include <map>
#include <string.h>
#include <time.h>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"

// SIG_NAT_TYPE：+0x00 两个 NAT 值 + 两个计数 + +0x10 time_t（总 0x14）
class SIG_NAT_TYPE
{
public:
    int m_natA;     // +0x00
    int m_natB;     // +0x04
    int m_third;    // +0x08
    int m_fourth;   // +0x0c
    time_t m_time;  // +0x10
};

class StatisticsNatType
{
public:
    struct NAT_TYPE_PEER_TO_PEER
    {
        NAT_TYPE_PEER_TO_PEER(int a, int b);

        int m_a;  // +0x00
        int m_b;  // +0x04

        bool operator<(const NAT_TYPE_PEER_TO_PEER& other) const
        {
            if (m_a != other.m_a)
            {
                return m_a < other.m_a;
            }
            return m_b < other.m_b;
        }
    };

    struct RESULT_COUNT
    {
        RESULT_COUNT();

        int m_first;   // +0x00
        int m_second;  // +0x04
    };

    StatisticsNatType();
    ~StatisticsNatType();

    void Reset();
    void IncreaseNatType(int natA, int natB, unsigned char count);
    void UpdateNatType();

    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*> m_map;  // +0x00
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
    m_first = 0;
    m_second = 0;
}

StatisticsNatType::StatisticsNatType()
    : m_map()
{
}

StatisticsNatType::~StatisticsNatType()
{
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_map.begin();
    while (it != m_map.end())
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
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_map.begin();
    while (it != m_map.end())
    {
        RESULT_COUNT* rc = it->second;
        if (rc != 0)
        {
            rc->m_second = 0;
            rc->m_first = 0;
        }
        ++it;
    }
}

void StatisticsNatType::IncreaseNatType(int natA, int natB, unsigned char count)
{
    NAT_TYPE_PEER_TO_PEER key(natA, natB);
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_map.find(key);

    RESULT_COUNT* rc = 0;
    if (it != m_map.end())
    {
        rc = it->second;
    }
    else
    {
        rc = new RESULT_COUNT();
        m_map.insert(std::make_pair(key, rc));
    }

    if (count != 0)
    {
        rc->m_first += 1;
    }
    else
    {
        rc->m_second += 1;
    }
}

void StatisticsNatType::UpdateNatType()
{
    time_t now = time(0);
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_map.begin();
    while (true)
    {
        if (it == m_map.end())
        {
            Reset();
            return;
        }

        RESULT_COUNT* rc = it->second;
        if (rc != 0 && (rc->m_second > 0 || rc->m_first > 0))
        {
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x1c7), true);

            **guard << 0x83;
            **guard << -1;
            SIG_NAT_TYPE* sig = guard->GetInBuffer<SIG_NAT_TYPE>();
            memset(sig, 0, sizeof(SIG_NAT_TYPE));
            sig->m_natA = it->first.m_a;
            sig->m_natB = it->first.m_b;
            sig->m_third = rc->m_second;
            sig->m_fourth = rc->m_first;
            sig->m_time = now;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
        }

        ++it;
    }
}
