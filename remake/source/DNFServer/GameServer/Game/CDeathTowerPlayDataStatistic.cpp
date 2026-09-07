// ============================================================================
// df_game_r 还原 —— CDeathTowerPlayDataStatistic::SendDeathTowerPlayInfo。
// ORIG 0x0860f45c T
// （_ZN28CDeathTowerPlayDataStatistic22SendDeathTowerPlayInfoEicPK30stDeathTowerRecordMemberInfo_tjj）。
// 报告：docs/class_func_reports/CDeathTowerPlayDataStatistic/SendDeathTowerPlayInfo.md
// 语义：按 idx 取成员记录（stride 0x17）的 +0x14 level 低字节，构造并发送
//   Job 统计包（0xc3d, 0x16）；level==0 时仅 log 错误不发送。随后无条件
//   构造并发送 Party 统计包（0xc3f, 0x10）。恒返回 1（__PRETTY_FUNCTION__
//   证据显示 ORIG 原返回类型为 bool，ABI 等价 int）。
// 两个 Packet 类的弱内联 ctor（ORIG 0x0860ffe6 / 0x0861000a）在各自头文件
// 内联定义（C1/C2 弱符号与 ORIG 一致），本 TU 仅实现 SendDeathTowerPlayInfo。
// ============================================================================
#include "CDeathTowerPlayDataStatistic.h"

#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "Packet_DeathTower_Statistic_Playdata_Job.h"
#include "Packet_DeathTower_Statistic_Playdata_Party.h"

// P86/P30：ORIG 三处 `imul $0x17,%eax,%eax; add 0x14(%ebp),%eax` 前向类型
// 真实下标访问形（probe pr4.cpp k1 实证：形参指针 + 完整类型下标出 add mem
// 形；TU 局部 MemberView 视图则恒 lea 双步）。本 TU 补全定义（仅类型，
// 不实例化不发射符号；stride 0x17 与 ORIG 访存偏移一致）。
class stDeathTowerRecordMemberInfo_t
{
public:
    char m_pad[0x14];
    unsigned char m_level;   // +0x14
    unsigned char m_job;     // +0x15
    unsigned char m_grow;    // +0x16
};

int CDeathTowerPlayDataStatistic::SendDeathTowerPlayInfo(
    int idx, char deathTowerType,
    const stDeathTowerRecordMemberInfo_t* members,
    unsigned int clearStage, unsigned int partyCount)
{
    Packet_DeathTower_Statistic_Playdata_Job jobPkt;
    jobPkt.m_deathTowerType = deathTowerType;
    jobPkt.m_level = (unsigned short)members[idx].m_level;
    jobPkt.m_characJob = (unsigned int)members[idx].m_job;
    jobPkt.m_characGrow = (char)members[idx].m_grow;
    jobPkt.m_field = (int)clearStage;
    if (jobPkt.m_level == 0)
    {
        LogManager::logFormat(1, "Statistics.cpp",
            __PRETTY_FUNCTION__,
            0x5fe,
            "CDeathTowerPlayDataStatistic ERROR!! deathTower_type (%d) level (%d) job_ (%d) grow_type_ (%d) clearStage (%d)",
            (int)(char)jobPkt.m_deathTowerType, (int)(short)jobPkt.m_level,
            (int)jobPkt.m_characJob, (int)(char)jobPkt.m_characGrow,
            (int)jobPkt.m_field);
    }
    else
    {
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
            ->SendPacket(reinterpret_cast<char*>(&jobPkt), 0x16);
    }

    Packet_DeathTower_Statistic_Playdata_Party partyPkt;
    partyPkt.m_deathTowerType = deathTowerType;
    partyPkt.m_partyCount = (char)partyCount;
    partyPkt.m_field = (int)clearStage;
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&partyPkt), 0x10);
    return 1;
}
