#include <cstring>
#include <cstdio>
#include "CPowerManager.h"
#include "SigTypes.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "CGameManager.h"

#include "CGuildServerProxy.h"
#include "CServerProxyMgr.h"
#include "PacketGuard.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CPrivateStoreMgr.h"
#include "CSharedServerMessageManager.h"
// ---- CPowerManager（PowerWar 管理器，ORIG 0x0847e998 域，布局 0x208）----
// report: docs/class_func_reports/CPowerManager.md。对象为 GlobalData.cpp
// 的 m_pad[0x208]（offset 布局一致：+0x00 winnerSide / +0x04/+0x08 分数 /
// +0x0c/+0x10 频道分数 / +0x14/+0x18 阵营人数 / +0x28..+0x1b4 雕像排行 /
// +0x1b4 CPowerWarLog / +0x1f0 CPowerWarPacketLog）。
bool CPowerManager::IsPowerWarEventOn()
{
    // ORIG 0x0847f910：重复事件 0x1e（PowerWar）经 vtable+0x34 判活，无空指针检查。
    // ORIG 尾 `test %al,%al; je; mov$1; jmp; mov$0` 分支形态（P56），直 return
    // 表达式被 4.4 折叠。改 if/return true;false 保留 ORIG CFG。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x1e)->IsEventing(0) != 0)
        return true;
    return false;
}



float CPowerManager::GetPowerWarResponPenalty(CUser* user)
{
    // ORIG 0x08480ec8：penalty 初始 1.0；混沌时间 < 阈值时二次读取
    // GetCurCharacChaosStateTime / G_CDataManager 计算 2.0 - chaos/threshold。
    float penalty = 1.0f;
    if ((unsigned int)user->GetCurCharacChaosStateTime() <
        (unsigned int)G_CDataManager()->m_powerParameter.m_penaltyThreshold)
    {
        penalty = 2.0f -
                  (float)user->GetCurCharacChaosStateTime() /
                      (float)G_CDataManager()->m_powerParameter.m_penaltyThreshold;
    }
    return penalty;
}

int CPowerManager::GetPowerWarGhostTime(CUser* user, float f)
{
    // ORIG 0x08480e92：(int)(ghostTime * f)，fistpl 就近取整。
    (void)user;
    return (int)((float)G_CDataManager()->m_powerParameter.m_ghostTime * f);
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
    if (side == 1 || side == 3)
    {
        --m_side1UserCount;
        if (m_side1UserCount < 0)
            m_side1UserCount = 0;
    }
    else
    {
        --m_side2UserCount;
        if (m_side2UserCount < 0)
            m_side2UserCount = 0;
    }
}

int CPowerManager::CheckPowerWarEnterLimit(CUser* user)
{
    int minLevel = G_CDataManager()->m_powerParameter.m_minEnterLevel;
    int minAge = G_CDataManager()->m_powerParameter.m_minEnterAge;
    if (minLevel == 0)
        minLevel = 0x1e;
    if (minAge == 0)
        minAge = 0xf;
    // ORIG 直调 CUser::CheckLimitMiniumAge（T 0x0864fc3e），禁内联展开（§4.1）。
    int outUnique;    // ORIG 不预清 -0x14（P198①：出参槽仅 lea，无 movl $0）
    // ORIG `xor $1;test;je`：`if (!bool)` 翻转直测；`==1` 反成 `test;je`（P196 极性）。
    if (!user->CheckLimitMiniumAge(minAge, outUnique))
    {
        return 0x5f;
    }
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
        // ORIG 直调 PowerParameterScript::checkPowerWarServer(a=+0x378 serverGroup,
        // b=+0x1b0 channelNo)，G_CEnvironment() 每次重物化（P183）；禁指针缓存与
        // 发明 helper（§4.1）。m_serverGroup 于 Server_Envir+0x1e0 ⇒ CEnvironment+0x378，
        // m_channelNo 于 Server_Envir+0x18 ⇒ CEnvironment+0x1b0。
        if (G_CDataManager()->m_powerParameter.checkPowerWarServer(
                G_CEnvironment()->m_serverEnvir.m_serverGroup,
                G_CEnvironment()->m_serverEnvir.m_channelNo) != 1)
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
    // ORIG 两次 getPowerSide()（cmp $3 / cmp $4 独立调用，P183 反向勿缓存）。
    if (user->getPowerSide() == 3 || user->getPowerSide() == 4)
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
      m_side1UserCount(0), m_side2UserCount(0), m_field1c(0)
{
    // ORIG 0x0847e998：初始化列表按声明序清 +0..+1c（不写 +0x20）；编译器按
    // 声明序前置成员构造——m_rankings[3] 循环调 STPowerWarStatueRankerInfo::C1、
    // CPowerWarLog::C1、CPowerWarPacketLog::C1（与 ORIG 顺序一致）；
    // 函数体再 memset (+0x24,4)、memset (+0x28,0x18c)，最后 Reset 两日志子对象。
    memset(m_powerWarStartTime, 0, sizeof(m_powerWarStartTime));
    memset(m_rankings, 0, sizeof(m_rankings));
    m_powerWarLog.Reset();
    m_powerWarPacketLog.Reset();
}

bool CPowerManager::Init()
{
    // ORIG 0x0847eabc：清零 +4,+8,+0x14,+0x18,+0xc,+0x10,+0x1c,+0x20，
    // memset (+0x24,4)、memset (+0x28,0x18c)、Reset 两日志，return 1。
    m_side1Point = 0;
    m_side2Point = 0;
    m_side1UserCount = 0;     // +0x14（ORIG 先清 +0x14/+0x18 再清 +0xc/+0x10）
    m_side2UserCount = 0;     // +0x18
    m_side1ChannelPoint = 0;  // +0x0c
    m_side2ChannelPoint = 0;  // +0x10
    m_field1c = 0;
    m_field20 = 0;
    memset(m_powerWarStartTime, 0, sizeof(m_powerWarStartTime));
    memset(m_rankings, 0, sizeof(m_rankings));
    m_powerWarLog.Reset();
    m_powerWarPacketLog.Reset();
    return 1;
}

void CPowerManager::LoadPowerWarInfo()
{
    // ORIG 0x08480d98：DB 消息 SIG_LOAD_POWER_WAR 0x19b / -1 / server_group。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x3c4), true);
    **guard << 0x19b;
    **guard << -1;
    **guard << G_CEnvironment()->m_serverEnvir.m_serverGroup;   // ORIG env+0x378 直读
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

void CPowerManager::LoadRankerInfo()
{
    // ORIG 0x0847f4fe：DB 消息 0x165 / -1 / server_group。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x12e), true);
    **guard << 0x165;
    **guard << -1;
    **guard << G_CEnvironment()->m_serverEnvir.m_serverGroup;   // ORIG env+0x378 直读
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
bool CPowerManager::CheckCompleteDBLoadStatueInfo()
{
    // ORIG 0x847f1bf 尾部 addl;mov slot;cmp;setbe 含 mov slot,%eax —— 探针
    // /tmp/l6/probe5.cpp 三形（do-while/++i/i++）4.4 恒折 cmpl $2,mem（N18b 族
    // mem 操作数比较折叠），源形不可复现，维持 for（残差 1 insn）。
    for (unsigned int i = 0; i <= 2; ++i)
        if (m_rankings[i].m_flag != 1)
            return 0;
    return 1;
}
void CPowerManager::SetPowerInfo(char side, int point, int channelPoint)
{
    // ORIG 0x0847eb76：*this=side; +4=point; +8=channelPoint。
    m_winnerSide = side;
    m_side1Point = point;
    m_side2Point = channelPoint;
}

void CPowerManager::SetPowerWarStartTime(unsigned char y, unsigned char m,
                                         unsigned char d, unsigned char h)
{
    m_powerWarStartTime[0] = y;
    m_powerWarStartTime[1] = m;
    m_powerWarStartTime[2] = d;
    m_powerWarStartTime[3] = h;
}

void CPowerManager::SetPowerWarUserRankingInfo(unsigned int* charNos)
{
    // ORIG 0x0847ebec：characNo==0 -> 置 flag，否则写 +0x2c 并发 DB 消息 0x166。
    for (int i = 0; i < 3; ++i)
    {
        if (charNos[i] != 0)
        {
            m_rankings[i].m_characNo = charNos[i];
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x7d), true);
            **guard << 0x166;
            **guard << -1;
            **guard << m_rankings[i].m_characNo;   // unsigned int → operator<<(uint)（ORIG P167）
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
        else
        {
            m_rankings[i].m_flag = 1;
        }
    }
}



void CPowerManager::SetPowerWarUserStatueInfo(SIG_LOAD_POWER_WAR_STATUE_INFO* info)
{
    // ORIG 0x0847ed4e：按 characNo（info+0x00）匹配条目，拷贝雕像字段并置 flag，
    // 完成后若 DB 加载齐则 SendPowerWarUserStatueInfo(0)。
    // SIG_LOAD_POWER_WAR_STATUE_INFO 仅建模为 m_pad[0x100]，字段按 ORIG 偏移访问：
    // +0x00 characNo / +0x04 name(0x1e) / +0x22 job / +0x23 level / +0x24 grow /
    // +0x25 guildName(0x28) / +0x50 guildId / +0x54 equipment(0x2c)。
    const unsigned char* src = (const unsigned char*)info->m_pad;
    for (int i = 0; i < 3; ++i)
    {
        if (m_rankings[i].m_characNo != *(const unsigned int*)(src + 0x00))
            continue;
        m_rankings[i].m_job = (char)*(const unsigned char*)(src + 0x22);
        m_rankings[i].m_level = (char)*(const unsigned char*)(src + 0x23);
        m_rankings[i].m_grow = (char)*(const unsigned char*)(src + 0x24);
        m_rankings[i].m_guildId = *(const int*)(src + 0x50);
        strncpy(m_rankings[i].m_name, (const char*)(src + 0x04), 0x1e);
        strncpy(m_rankings[i].m_guildName, (const char*)(src + 0x25), 0x28);
        memcpy(m_rankings[i].m_equipment, src + 0x54, 0x2c);
        // ORIG 0x0847f0e5：cMyTrace(3 参) + "Set Statue Ranker ..." 变参日志。
        // 注意 ORIG 压栈 job/lev 槽位互换（lev 值在 job 槽、job 值在 lev 槽），
        // 且 EQUP 用 m_equipment[0..8] 与 [10]（跳过 [9]）——忠实保留。
        cMyTrace trace(
            "void CPowerManager::SetPowerWarUserStatueInfo(SIG_LOAD_POWER_WAR_STATUE_INFO*)",
            0xa4, 0);
        trace("Set Statue Ranker %s(%d), job:%d, lev:%d, grow:%d, guild_name:%s, guild_id:%d, EQUP [head:%d][hair:%d][face:%d][jacket:%d][pants:%d][shoes:%d][breast:%d][waist:%d][skin:%d][weapon:%d]",
              m_rankings[i].m_name, m_rankings[i].m_characNo,
              static_cast<int>(m_rankings[i].m_level),
              static_cast<int>(m_rankings[i].m_job),
              static_cast<int>(m_rankings[i].m_grow),
              m_rankings[i].m_guildName, m_rankings[i].m_guildId,
              m_rankings[i].m_equipment[0], m_rankings[i].m_equipment[1],
              m_rankings[i].m_equipment[2], m_rankings[i].m_equipment[3],
              m_rankings[i].m_equipment[4], m_rankings[i].m_equipment[5],
              m_rankings[i].m_equipment[6], m_rankings[i].m_equipment[7],
              m_rankings[i].m_equipment[8], m_rankings[i].m_equipment[10]);
        break;
    }
    if (CheckCompleteDBLoadStatueInfo())
        SendPowerWarUserStatueInfo(0);
}

void CPowerManager::ClearMVPInfo()
{
    cMyTrace trace("void CPowerManager::ClearMVPInfo()", 0x143, 0);
    trace("CLEAR MVP INFO");
    memset(m_rankings, 0, sizeof(m_rankings));
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x149), true);
    **guard << 0x1a5;
    **guard << -1;
    // ORIG 直读 G_CEnvironment()+0x378（m_serverEnvir.m_serverGroup），两次
    // 各自重物化（P183）；禁 getter 调用（EXTRA symbol）与跨调用缓存。
    **guard << G_CEnvironment()->m_serverEnvir.m_serverGroup;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

    cMyTrace tr("void CPowerManager::ClearMVPInfo()", 0x152, 0);
    tr("PUT DB MSG - E_DELETE_POWER_WAR_STATUE_MESSAGE / server_group : %d",
       G_CEnvironment()->m_serverEnvir.m_serverGroup);

    CSharedServerMessageManager* mgr =
        G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        mgr->DeleteSpecificMessage(1);
        mgr->DeleteSpecificMessage(2);
        mgr->DeleteSpecificMessage(3);
    }

    PacketGuard pkt;
    pkt.put_header(0, 0xc0);
    pkt.put_byte(2);
    pkt.put_byte(3);
    char* msg = (char*)"null string";   // ORIG char* → put_str(char*,int) 重载
    int len = static_cast<int>(strlen(msg));
    for (int i = 0; i < 3; ++i)
    {
        pkt.put_byte(i + 1);
        pkt.put_int(len);
        pkt.put_str(msg, len);
        pkt.put_int(len);
        pkt.put_str(msg, len);
    }

    pkt.finalize(true);
    G_GameWorld()->send_all_with_state(pkt, 3);
}

bool CPowerManager::IsAbleToRevival(CUser* user, unsigned int charNo, int& result)
{
    // ORIG 0x0847f952：复活冷却判定。penalty 同 GetPowerWarResponPenalty，
    // ghost=ROUND(ghostTime*penalty)，与 (charNo - 混沌死亡时刻) 比较。
    float penalty = 1.0f;
    if ((unsigned int)user->GetCurCharacChaosStateTime() <
        (unsigned int)G_CDataManager()->m_powerParameter.m_penaltyThreshold)
    {
        penalty = 2.0f -
                  (float)user->GetCurCharacChaosStateTime() /
                      (float)G_CDataManager()->m_powerParameter.m_penaltyThreshold;
    }
    int ghost = (int)((float)G_CDataManager()->m_powerParameter.m_ghostTime * penalty);
    int delta = (int)charNo - user->GetCurCharacChaosDieTime();
    if (ghost <= delta)
        result = 0;
    else
        result = ghost - delta;
    return ghost <= delta;
}

void CPowerManager::IncreaePowerWarPointInChannel(char side, int point)
{
    if (side == 1)
        m_side1ChannelPoint += point;
    else
        m_side2ChannelPoint += point;
}

void CPowerManager::SendPowerWarUserStatueInfo(CUser* user)
{
    // ORIG 0x0847f200：组 SEND 雕像信息包（0xb6），含各条目姓名/职业/等级/
    // 成长/公会名/公会id/装备，user==0 全服广播，否则单发。
    PacketGuard pkt;
    pkt.put_header(0, 0xb6);
    int count = 0;
    for (int i = 0; i < 3; ++i)
        if (m_rankings[i].m_characNo != 0)
            ++count;
    pkt.put_byte((int)(char)count);
    for (int i = 0; i < count; ++i)
    {
        pkt.put_int((int)strlen(m_rankings[i].m_name));
        pkt.put_str(m_rankings[i].m_name, strlen(m_rankings[i].m_name));
        pkt.put_byte((int)(char)m_rankings[i].m_job);
        pkt.put_byte((int)(char)m_rankings[i].m_level);
        pkt.put_byte((int)(char)m_rankings[i].m_grow);
        pkt.put_int((int)strlen(m_rankings[i].m_guildName));
        pkt.put_str(m_rankings[i].m_guildName, strlen(m_rankings[i].m_guildName));
        pkt.put_int(m_rankings[i].m_guildId);
        for (int j = 0; j < 0xb; ++j)
        {
            if (m_rankings[i].m_equipment[j] == 0 || j == 9)
                pkt.put_int(-1);
            else
                pkt.put_int(m_rankings[i].m_equipment[j]);
        }
    }
    pkt.finalize(true);
    if (user == 0)
        G_GameWorld()->send_all(pkt);
    else
        user->Send(pkt);
}

int CPowerManager::GetPowerUserCount(char side)
{
    return side == 1 ? m_side1UserCount : m_side2UserCount;
}

int CPowerManager::GetPowerWarPoint(ENUM_POWER_SIDE_TYPE side)
{
    return (int)side == 1 ? m_side1Point : m_side2Point;
}

int CPowerManager::GetPowerWarPointInChannel(char side)
{
    return side == 1 ? m_side1ChannelPoint : m_side2ChannelPoint;
}

int CPowerManager::GetRankingByCharacNo(unsigned int charNo)
{
    // N18b 族同上（mov slot,%eax 不可复现），维持 for。
    for (unsigned int i = 0; i <= 2; ++i)
        if (m_rankings[i].m_characNo == charNo)
            return (int)i + 1;
    return 0;
}

// ORIG 0x084ebe2e（_ZN14CPowerManager16SetPowerWarPointEii，W）：
//   *(int*)(this+0x04) = arg1; *(int*)(this+0x08) = arg2; ret。
void CPowerManager::SetPowerWarPoint(int sideIdx, int point)
{
    m_side1Point = sideIdx;
    m_side2Point = point;
}
