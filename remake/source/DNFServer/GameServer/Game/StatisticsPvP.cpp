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
        // 2026-08-31：ORIG sizeof 16（map 节点 40 = 16+pair(8+16)；incPvPRoomResult
        // 0x860c653 构造 4 dword：{keyA, keyB, 1, value}）——原仅 2 int，补全。
        int m_field0;  // +0x00
        int m_field4;  // +0x04
        int m_field8;  // +0x08
        int m_fieldC;  // +0x0c
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
        // ORIG 0x860c653：PvP_Room 四字段 {keyA, keyB, 1, value}。
        PvP_Room room;
        room.m_field0 = keyA;
        room.m_field4 = keyB;
        room.m_field8 = 1;
        room.m_fieldC = value;
        m_map.insert(std::make_pair(key, room));
    }
    else
    {
        it->second.m_field8 += 1;
        it->second.m_fieldC += value;
    }
}
