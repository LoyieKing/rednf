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
    // [b28d-59] ORIG 0x0860d682：无 fieldIdx/level 具名局部（P0：形参直读 +
    // 索引处内联 get_charac_level()，0x0860d7a5 每次重调）；条件为
    // field<0(js) / field>0x1d(jg) / level<=0(jle) / level>0x46(jle) 四项短路，
    // 汇聚 bool 后 `test %al; je`。插入路径也是 `+= value`（读-加-写，非直接赋值）。
    if (field < 0 || field > 0x1d ||
        user->get_charac_level() <= 0 || user->get_charac_level() > 0x46)
    {
        // [b28d-59] 全表达式临时链（P227）：ORIG 0x0860d6d2 在 cMyTrace ctor 之前
        // 求 varargs（get_charac_level → %ebx、field → %esi 跨 ctor 保活），具名
        // trace 会把它挪到 ctor 之后。实参序 (field, level)。
        cMyTrace(__PRETTY_FUNCTION__, 0x306, false)(
            "VALUE_STATISTIC wrong index (%d,%d)",
            field, user->get_charac_level());
        return false;
    }

    ENUM_SERVER_GROUP group = user->GetServerGroup();
    std::map<ENUM_SERVER_GROUP, stValueStatistic>::iterator it = m_data.find(group);
    if (it == m_data.end())
    {
        stValueStatistic stat;
        stat.reset();
        stat.m_values[user->get_charac_level()][field] += value;
        m_data.insert(std::make_pair(group, stat));
    }
    else
    {
        it->second.m_values[user->get_charac_level()][field] += value;
    }
    return true;
}

int CValueStatistic::InsertValueStatistic()
{
    for (std::map<ENUM_SERVER_GROUP, stValueStatistic>::iterator it = m_data.begin();
         it != m_data.end(); ++it)
    {
        // [b28d-59] ORIG 0x0860d8d3/d8e6：group 槽 -0x1c、stat 引用槽 -0x18
        // （均具名局部，P0 槽往返实证）。
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
            // [b28d-59] ORIG 0x0860d933 `movzbl hasValue; xor $1; test; jne` ⇒
            // 原码取反形（P168a②直分支形）。
            if (hasValue == false)
            {
                continue;
            }
            {
                // ORIG 0x0860d8a4：ctor（PacketHeader(0x17ca,0x86)+m_level=0）构造，
                // 随后 m_level=level 与 memcpy m_values；不再整体 memset。
                Packet_Value_Statistic packet;
                packet.m_level = level;
                memcpy(packet.m_values, stat.m_values[level], 0x78);
                // [b28d-59] ORIG 0x0860d999..d9ad 无 proxy 判空（P171：原码不写即删；
                // GetServerProxy 返回值经 %eax 直传 SendPacket），旧判空多 1 槽 +3 条。
                GlobalData::s_statistic_proxy_mgr->GetServerProxy(group)->SendPacket(
                    (char*)&packet, 0x86);
            }
        }
        stat.reset();
    }
    return 1;
}
