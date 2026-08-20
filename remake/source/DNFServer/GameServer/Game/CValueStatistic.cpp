// df_game_r Game/ CValueStatistic 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/CValueStatistic.md：
//   AddValueStatistic 0x0860d682 / InsertValueStatistic 0x0860d8a4
#include "CInventory.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include <string.h>

// ---- CStatisticServerProxy（定义见 GameStubs_remaining.cpp） ----
#include "CStatisticServerProxy.h"

// ---- Packet_Value_Statistic（统计服务器发包，0x86 字节） ----
// 布局依据 InsertValueStatistic 反汇编：
//   +0x00 包头(6 字节)  +0x06 int level  +0x0a int values[30]（0x78 字节）
struct Packet_Value_Statistic
{
    char m_header[0x06];  // +0x00
    int m_level;          // +0x06
    int m_values[30];     // +0x0a（0x78 字节）
};

CValueStatistic* GetInstanceValueStatistic()
{
    static CValueStatistic s;
    return &s;
}

void CValueStatistic::stValueStatistic::reset()
{
    memset(m_values, 0, sizeof(m_values));
}

void CValueStatistic::AddValueStatistic(VALUE_STATISTIC_FIELD field, CUser* user,
                                        unsigned int value)
{
    int fieldIdx = (int)field;
    // 校验 field 范围 [0, 0x1d] 且 level > 0 且 level < 0x47
    if (fieldIdx < 0 || fieldIdx >= 0x1e)
    {
        return;
    }
    int level = user->get_charac_level();
    if (level <= 0 || level >= 0x47)
    {
        return;
    }

    ENUM_SERVER_GROUP group = user->GetServerGroup();
    std::map<ENUM_SERVER_GROUP, stValueStatistic>::iterator it = m_data.find(group);
    if (it == m_data.end())
    {
        stValueStatistic stat;
        stat.reset();
        stat.m_values[level][fieldIdx] = value;
        m_data.insert(std::make_pair(group, stat));
    }
    else
    {
        it->second.m_values[level][fieldIdx] += value;
    }
}

int CValueStatistic::InsertValueStatistic()
{
    for (std::map<ENUM_SERVER_GROUP, stValueStatistic>::iterator it = m_data.begin();
         it != m_data.end(); ++it)
    {
        ENUM_SERVER_GROUP group = it->first;
        stValueStatistic& stat = it->second;
        for (int level = 1; level < 0x47; ++level)
        {
            bool hasValue = false;
            for (int f = 0; f < 0x1e; ++f)
            {
                if (stat.m_values[level][f] != 0)
                {
                    hasValue = true;
                    break;
                }
            }
            if (hasValue)
            {
                Packet_Value_Statistic packet;
                memset(&packet, 0, sizeof(packet));
                packet.m_level = level;
                memcpy(packet.m_values, stat.m_values[level], 0x78);
                CStatisticServerProxy* proxy =
                    GlobalData::s_statistic_proxy_mgr->GetServerProxy(group);
                if (proxy)
                {
                    proxy->SendPacket((char*)&packet, 0x86);
                }
            }
        }
        stat.reset();
    }
    return 1;
}
