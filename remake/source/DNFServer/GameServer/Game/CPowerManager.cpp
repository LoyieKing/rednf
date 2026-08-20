#include <cstring>
#include "CPowerManager.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"
#include "CServerProxyMgr.h"
#include "PacketGuard.h"
#include "CPrivateStoreMgr.h"

// ---- CPowerManager（PowerWar 管理器，ORIG 0x0847e998 域，布局 0x208）----
// report: docs/class_func_reports/CPowerManager.md。对象为 GlobalData.cpp
// 的 m_pad[0x208]（offset 布局一致：+0x00 winnerSide / +0x04/+0x08 分数 /
// +0x0c/+0x10 频道分数 / +0x14/+0x18 阵营人数 / +0x28..+0x1b4 雕像排行 /
// +0x1b4 CPowerWarLog / +0x1f0 CPowerWarPacketLog）。
static bool PowerManager_IsOverBirthDay(CUser* user)
{
    return user != 0 && user->IsOverBirthDay();
}

static bool PowerManager_CheckPowerWarServer(int serverGroup, int channelNo)
{
    const std::vector<std::pair<int, int> >& servers =
        G_CDataManager()->m_powerParameter.m_powerWarServerList;
    for (std::vector<std::pair<int, int> >::const_iterator it = servers.begin();
         it != servers.end(); ++it)
    {
        if (it->first == serverGroup && it->second == channelNo)
            return true;
    }
    return false;
}

bool CPowerManager::IsPowerWarEventOn()
{
    // ORIG 0x0847f910：重复事件 0x1e（PowerWar）经 vtable+0x34 判活。
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x1e);
    return event && event->IsEventing(0) != 0;
}

float CPowerManager::GetPowerWarResponPenalty(CUser* user)
{
    float penalty = 1.0f;
    unsigned int chaos = (unsigned int)user->GetCurCharacChaosStateTime();
    int threshold = G_CDataManager()->m_powerParameter.m_penaltyThreshold;
    if (chaos < (unsigned int)threshold)
        penalty = 2.0f - (float)chaos / (float)threshold;
    return penalty;
}
int CPowerManager::GetPowerWarGhostTime(CUser* user, float f)
{
    (void)user;
    int base = G_CDataManager()->m_powerParameter.m_ghostTime;
    return (int)((float)base * f + 0.5f);
}

void CPowerManager::IncreasePowerUserCount(char side)
{
    if (side == 1 || side == 3)
        ++m_side1UserCount;
    else
        ++m_side2UserCount;
}

void CPowerManager::DecreasePowerUserCount(char side)
{
    int* count = side == 1 || side == 3
        ? &m_side1UserCount : &m_side2UserCount;
    --*count;
    if (*count < 0)
        *count = 0;
}

int CPowerManager::CheckPowerWarEnterLimit(CUser* user)
{
    int minLevel = G_CDataManager()->m_powerParameter.m_minEnterLevel;
    int minAge = G_CDataManager()->m_powerParameter.m_minEnterAge;
    if (minLevel == 0)
        minLevel = 0x1e;
    if (minAge == 0)
        minAge = 0xf;
    // CUser::CheckLimitMiniumAge(user, minAge, &out) 内联。

    int out = 0;
    int age = user->GetAge();
    int pass = 1;
    if ((unsigned int)(age - 1) < (unsigned int)minAge)
    {
        out = user->get_unique_id() & 0xffff;
        pass = 0;
    }
    else if (age - 1 == minAge && !PowerManager_IsOverBirthDay(user))
    {
        out = user->get_unique_id() & 0xffff;
        pass = 0;
    }
    if (pass != 1)
        return 0x5f;
    if (user->get_charac_level() < minLevel)
        return 0xe;
    return 0;
}

char CPowerManager::GetWinnerSide()
{
    return m_winnerSide;
}

bool CPowerManager::IsWinerSide(char side)
{
    if (m_winnerSide == 0 || side == 0)
        return false;
    return m_winnerSide == side;
}

int CPowerManager::ProcessJoinPowerWar(CUser* user, char side)
{
    // ORIG 0x0847fb02：加入/退出 PowerWar 的完整校验与处理。
    // 职业 9/10（未转职）与佣兵不可参加。
    if (user->get_charac_job() == 9)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x15);
        return 1;
    }
    if (user->get_charac_job() == 10)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x15);
        return 1;
    }
    if (user->isCompetitionMercenary())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x15);
        return 1;
    }
    if (user->get_charac_guildkey() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 100);
        return 1;
    }
    if (user->getPowerSide() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x82);
        return 1;
    }
    if (side != 0 && side != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 1);
        return 1;
    }
    int enterLimit = 0;
    if (side == 1)  // 加入
    {
        CEnvironment* env = G_CEnvironment();
        int serverGroup = env->get_server_group();
        int channelNo = env->get_channel_no();
        if (!PowerManager_CheckPowerWarServer(serverGroup, channelNo))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x80);
            return 1;
        }
        if (!GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x7f);
            return 1;
        }
        enterLimit = CheckPowerWarEnterLimit(user);
        if (enterLimit != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, (unsigned char)(enterLimit & 0xff));
            return 1;
        }
        int cooldown = G_CDataManager()->m_powerParameter.m_powerWarCooldownMinutes * 0x3c;
        long lastPlay = user->getCurCharacLastPlayTickPowerWar();
        if (GlobalData::s_systemTime_.getCurSec() - lastPlay <= cooldown)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x84);
            return 1;
        }
        if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 1);
            return 0;
        }
        if (user->IsBusyDisjointStore())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 1);
            return 0;
        }
    }
    user->setCurrCharacJoinPower(side);
    char isOriginal = 0;
    unsigned char powerSide = user->getPowerSide();
    if (powerSide == 3 || powerSide == 4)
        isOriginal = 1;
    if (side == 1)
    {
        if (isOriginal)
        {
            unsigned char side2 = user->getPowerSide();
            unsigned int guildKey = (unsigned int)user->get_charac_guildkey();
            unsigned int charNo = (unsigned int)user->getCurCharacNo();
            CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr
                                           ->GetServerProxy((ENUM_SERVER_GROUP)0);
            proxy->SendApplyOriginalPowerSide(charNo, guildKey, side2);
        }
        long t = GlobalData::s_systemTime_.getCurSec();
        user->SetPowerWarLastPlayTime(t);
        unsigned short stamina = (unsigned short)user->getCurCharacStamina();
        user->setPowerWarHP(stamina & 0xff);
        user->resetStraightVictories();
        GlobalData::s_power_manager->IncreasePowerUserCount((char)user->getPowerSide());
    }
    else
    {
        GlobalData::s_power_manager->DecreasePowerUserCount((char)user->getPowerSide());
        long t = GlobalData::s_systemTime_.getCurSec();
        user->setCurCharacLastPlayTickPowerWar(t);
        user->SetCurCharacChaosDieTime(0);
        user->m_characterView.enableSaveCharacView();
    }
    if (!isOriginal)
    {
        PacketGuard pkt;
        pkt.put_header(1, 0xc3);
        pkt.put_byte(1);
        pkt.put_byte((int)side);
        pkt.finalize(true);
        user->Send(pkt);

        PacketGuard pkt2;
        pkt2.put_header(0, 2);
        pkt2.put_byte(0);
        pkt2.put_short(1);
        user->make_basic_info((char*)&pkt2, 0);
        pkt2.finalize(true);
        G_GameWorld()->send_all(pkt2, user);
    }
    return 1;
}



CPowerManager::CPowerManager()
    : m_winnerSide(0), m_side1Point(0), m_side2Point(0),
      m_side1ChannelPoint(0), m_side2ChannelPoint(0),
      m_side1UserCount(0), m_side2UserCount(0)
{
    memset(m_pad14, 0, sizeof(m_pad14));
    memset(m_pad20, 0, sizeof(m_pad20));
    memset(m_pad1b4, 0, sizeof(m_pad1b4));
}

bool CPowerManager::Init()
{
    return true;
}

void CPowerManager::LoadPowerWarInfo() {}
void CPowerManager::LoadRankerInfo() {}

bool CPowerManager::CheckCompleteDBLoadStatueInfo() {
    for (int i = 0; i < 3; ++i) if (m_rankings[i].m_characNo == 0) return false;
    return true;
}
void CPowerManager::SetPowerInfo(char side, int point, int channelPoint) {
    if (side == 1 || side == 3) { m_side1Point = point; m_side1ChannelPoint = channelPoint; }
    else { m_side2Point = point; m_side2ChannelPoint = channelPoint; }
}
void CPowerManager::SetPowerWarStartTime(unsigned char y, unsigned char m, unsigned char d, unsigned char h) {
    m_pad20[4] = y; m_pad20[5] = m; m_pad20[6] = d; m_pad20[7] = h;
}
void CPowerManager::SetPowerWarUserRankingInfo(unsigned int* n) {
    if (!n) return; for (int i = 0; i < 3; ++i) m_rankings[i].m_characNo = n[i];
}
void CPowerManager::SetPowerWarUserStatueInfo(SIG_LOAD_POWER_WAR_STATUE_INFO*) {}
void CPowerManager::ClearMVPInfo() { memset(m_rankings, 0, sizeof(m_rankings)); }
bool CPowerManager::IsAbleToRevival(CUser*, unsigned int n, int& r) { r = GetRankingByCharacNo(n); return r != 0; }
void CPowerManager::IncreaePowerWarPointInChannel(char side, int p) { if (side == 1 || side == 3) m_side1ChannelPoint += p; else m_side2ChannelPoint += p; }
void CPowerManager::SendPowerWarUserStatueInfo(CUser*) {}

int CPowerManager::GetPowerUserCount(char side)
{
    return side == 1 || side == 3 ? m_side1UserCount : m_side2UserCount;
}

int CPowerManager::GetPowerWarPoint(ENUM_POWER_SIDE_TYPE side)
{
    return side == ENUM_POWER_SIDE_TYPE_DUMMY ? 0 : m_side2Point;
}

int CPowerManager::GetPowerWarPointInChannel(char side)
{
    return side == 1 || side == 3 ? m_side1ChannelPoint : m_side2ChannelPoint;
}

int CPowerManager::GetRankingByCharacNo(unsigned int charNo)
{
    for (int i = 0; i < 3; ++i)
        if (m_rankings[i].m_characNo == charNo)
            return i + 1;
    return 0;
}

