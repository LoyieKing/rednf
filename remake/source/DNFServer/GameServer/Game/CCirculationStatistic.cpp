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

// ORIG 0x0860daaa：无 user 判空、无 level 局部缓存（get_charac_level 在条件里被
// 调两次、错误分支再调一次、命中分支各调一次），且槽位不是 map::operator[] 而是
// find/end 比较 + 未命中时栈上零初始化记录 insert(make_pair(...))。
bool CCirculationStatistic::AddCirculationStatistic(CIRCULATION_STATISTIC_FIELD field, CUser* user,
                                                       unsigned int value)
{
    bool wrongIndex = !(field >= 0 && field < 0x30 &&
                        user->get_charac_level() > 0 &&
                        user->get_charac_level() < 0x47);
    if (wrongIndex)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x39b, 0)(
            "CIRCULATION_STATISTIC wrong index (%d,%d)", field, user->get_charac_level());
        return false;
    }

    std::map<ENUM_SERVER_GROUP, stCirculationStatistic>::iterator it =
        m_data.find(user->GetServerGroup());
    if (it == m_data.end())
    {
        stCirculationStatistic stat;
        memset(&stat, 0, 0x3540);
        stat.m_values[user->get_charac_level()][field] = value;
        m_data.insert(std::make_pair(user->GetServerGroup(), stat));
    }
    else
    {
        it->second.m_values[user->get_charac_level()][field] += value;
    }
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
