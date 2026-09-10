// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCirculationStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CCirculationStatistic.h"
#include "CUser.h"
#include <string.h>
#include "Packet_Circulation_Statistic.h"   // ORIG 包布局（LinkClosureD 补 include）
#include "CStatisticServerProxy.h"          // SendPacket
#include "CServerProxyMgr.h"                // GetServerProxy 模板
#include "GlobalData.h"                     // s_statistic_proxy_mgr


#include "CCirculationStatistic/reset.inc"
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
#include "CCirculationStatistic/AddCirculationStatistic.inc"
                                                       unsigned int value)
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0860daaa `test %al,%al` 直测 vs 我们
    // bool 槽 `mov %al,-0x19; cmpb`（P0 表达式直用/P168a③b 族）；语义等价。
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
#include "CCirculationStatistic/InsertCirculationStatistic.inc"
{
    // [b28d-34] 复核维持 DIFF：ORIG 0x0860dccc 槽组整体平移 4
    // （-0xf4/-0x1c vs -0xf8/-0x20）+ 一段顺序差（N82/N98 族）；语义等价。
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
            packet.fields.m_channel =
                static_cast<unsigned short>(G_CEnvironment()->get_channel_no());
            packet.fields.m_level = level;
            memcpy(packet.m_data, stat.m_values[level],
                   0x30 * sizeof(int));
            CStatisticServerProxy* proxy =
                GlobalData::s_statistic_proxy_mgr->GetServerProxy(group);
            proxy->SendPacket(reinterpret_cast<char*>(&packet), 0xd0);
        }
        stat.reset();
    }
}
