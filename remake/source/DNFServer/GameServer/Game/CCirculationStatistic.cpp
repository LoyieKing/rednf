#include "CCirculationStatistic.h"
#include "CUser.h"
#include <string.h>
#include "Packet_Circulation_Statistic.h"   // ORIG 包布局（LinkClosureD 补 include）
#include "CStatisticServerProxy.h"          // SendPacket
#include "CServerProxyMgr.h"                // GetServerProxy 模板
#include "GlobalData.h"                     // s_statistic_proxy_mgr


void CCirculationStatistic::stCirculationStatistic::reset()
{
    memset(m_values, 0, sizeof(m_values));
}

CCirculationStatistic* GetInstanceCirculationStatistic()
{
    static CCirculationStatistic instance;
    return &instance;
}

bool CCirculationStatistic::AddCirculationStatistic(CIRCULATION_STATISTIC_FIELD field, CUser* user,
                                                       unsigned int value)
{
    if (!user || field < 0 || field >= 0x30)
        return false;
    const int level = user->get_charac_level();
    if (level <= 0 || level >= 0x47)
        return false;
    stCirculationStatistic& stat = m_data[user->GetServerGroup()];
    stat.m_values[level][field] += value;
    return true;
}

// ORIG T 0x0860dccc sz 0x184（InsertCirculationStatistic.md 全文已读）：
// 遍历 m_data（map<ENUM_SERVER_GROUP,stCirculationStatistic>）；每条目 level 1..0x46
// 有任一 field 值非 0 → 构造 Packet_Circulation_Statistic（0xd0 字节）：
//   ctor(0x086100f8)→m_channel=get_channel_no()→m_level=level→memcpy(数据, level*0xc0)
//   → GetServerProxy(group).SendPacket(buf,0xd0)；全部 level 发送完 → reset。
// 注意：局部包对象在栈上，ctor 置 m_level/m_channel 为 0（Packet_Circulation_Statistic.h
// 初始化列表已对位）。
void CCirculationStatistic::InsertCirculationStatistic()
{
    for (std::map<ENUM_SERVER_GROUP, stCirculationStatistic>::iterator it =
             m_data.begin(), endIt = m_data.end();
         it != endIt; ++it)
    {
        const ENUM_SERVER_GROUP group = it->first;
        stCirculationStatistic& stat = it->second;
        for (int level = 1; level <= 0x46; ++level)
        {
            bool hasValue = false;
            for (int field = 0; field <= 0x2f; ++field)
            {
                if (stat.m_values[level][field] != 0)
                {
                    hasValue = true;
                    break;
                }
            }
            if (!hasValue)
                continue;

            Packet_Circulation_Statistic packet;
            packet.m_channel =
                static_cast<unsigned short>(G_CEnvironment()->get_channel_no());
            packet.m_level = level;
            memcpy(packet.m_data, stat.m_values[level],
                   0x30 * sizeof(int));
            CStatisticServerProxy* proxy =
                GlobalData::s_statistic_proxy_mgr->GetServerProxy(group);
            proxy->SendPacket(reinterpret_cast<char*>(&packet), 0xd0);
        }
        stat.reset();
    }
}
