// ============================================================================
// df_game_r 还原 —— StatisticsPvP（G2-4 批次，统计域）。
// 依据 docs/class_func_reports/StatisticsPvP.md + ORIG 反汇编。
// 布局：+0x00 Mutex | +0x18 map<PvP_Key,PvP_Room>。
// ============================================================================

#include <map>

#include "GameTypes.h"

class StatisticsPvP
{
public:
    struct PvP_Key
    {
        int m_a;  // +0x00
        int m_b;  // +0x04

        bool operator<(const PvP_Key& other) const
        {
            if (m_a != other.m_a)
            {
                return m_a < other.m_a;
            }
            return m_b < other.m_b;
        }
    };

    struct PvP_Room
    {
        int m_count;  // +0x00
        int m_total;  // +0x04
    };

    StatisticsPvP();
    ~StatisticsPvP();

    void reset();
    void incPvPRoomResult(int keyA, int keyB, int value);

    Mutex m_mutex;                         // +0x00
    std::map<PvP_Key, PvP_Room> m_map;     // +0x18
};

StatisticsPvP::StatisticsPvP()
    : m_mutex(), m_map()
{
}

StatisticsPvP::~StatisticsPvP() {}

void StatisticsPvP::reset()
{
    Guard<Mutex> guard(&m_mutex);
    m_map.clear();
}

void StatisticsPvP::incPvPRoomResult(int keyA, int keyB, int value)
{
    Guard<Mutex> guard(&m_mutex);

    PvP_Key key;
    key.m_a = keyA;
    key.m_b = keyB;

    std::map<PvP_Key, PvP_Room>::iterator it = m_map.find(key);
    if (it == m_map.end())
    {
        PvP_Room room;
        room.m_count = 1;
        room.m_total = value;
        m_map.insert(std::make_pair(key, room));
    }
    else
    {
        it->second.m_count += 1;
        it->second.m_total += value;
    }
}
