// df_game_r Game/ CValueStatistic 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/CValueStatistic.md：
//   AddValueStatistic 0x0860d682 / InsertValueStatistic 0x0860d8a4
#include "CInventory.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include <string.h>

// ---- CStatisticServerProxy（定义见 GameStubs_remaining.cpp） ----
#include "CStatisticServerProxy.h"

#include "Packet_Value_Statistic.h"

CValueStatistic* GetInstanceValueStatistic()
{
    static CValueStatistic instance;
    return &instance;
}

void CValueStatistic::stValueStatistic::reset()
{
    memset(m_values, 0, sizeof(m_values));
}

bool CValueStatistic::AddValueStatistic(VALUE_STATISTIC_FIELD field, CUser* user,
                                        unsigned int value)
{
    int fieldIdx = (int)field;
    // ORIG 0x0860d682：校验 field [0,0x1d] 且 level∈(0,0x47)；非法记日志并返 false。
    if (fieldIdx < 0 || fieldIdx >= 0x1e ||
        user->get_charac_level() <= 0 || user->get_charac_level() >= 0x47)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x306, false);
        // ORIG 0x0860d6ea：trace 实参序为 (field, level)（0x8(%esp)=field、
        // 0xc(%esp)=level），输出 "(%d,%d)" 为 field,level。
        trace("VALUE_STATISTIC wrong index (%d,%d)",
              fieldIdx, user->get_charac_level());
        return false;
    }
    int level = user->get_charac_level();

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
    return true;
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
                // ORIG 0x0860d8a4：ctor（PacketHeader(0x17ca,0x86)+m_level=0）构造，
                // 随后 m_level=level 与 memcpy m_values；不再整体 memset。
                Packet_Value_Statistic packet;
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
