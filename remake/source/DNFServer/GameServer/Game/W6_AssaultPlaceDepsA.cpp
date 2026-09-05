// df_game_r Game/ pvp_assault::CAssaultPlace 前半 30 个 ORIG-only 方法（Wave6）。
// 组：W6_AssaultPlaceA（符号清单 /tmp/only_orig/pvp_assault.txt 第 24-53 行）。
// 全部语义取自 docs/class_func_reports/pvp_assault__CAssaultPlace/ 对应报告
// （汇编 + Ghidra C）；每函数前注释 ORIG 地址/尺寸。
#include "CAssaultMgr.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"           // s_event_manager / s_power_manager / s_systemTime_
#include "TimerDispatcher.h"      // TimerCloseAssault/TimerEndAssault/TimerConnectP2PAssaultTimeout
#include "CPowerManager.h"        // IsPowerWarEventOn / IsAbleToRevival / GetPowerWarLog(CPowerWarLog)
#include "CGuildServerProxy.h"    // CEventBase::IsEventing（vtable+0x34 槽）
#include "GameWorld.h"            // G_GameWorld / send_all / send_to_vill / get_disposition
#include "CDisconnectDetecter.h"  // disconnect_detecter::RegisterUser
#include "CDataManager.h"         // PowerParameterScript::GetPenalty/GetBreakBonus (G_CDataManager()+0xa628)
#include "Village.h"

namespace pvp_assault
{

// ---------------------------------------------------------------------------
// ORIG T 0x82e7162 (size 0x61)：_GetTeamCount(ENUM_ASSAULT_TEAM)
// ---------------------------------------------------------------------------
int CAssaultPlace::_GetTeamCount(ENUM_ASSAULT_TEAM team)
{
    int count = 0;
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()) {
            if ((int)m_assaulters[i].GetTeam() == (int)team) {
                count = count + 1;
            }
        }
    }
    return count;
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e7288 (size 0x71)：_GetTeamCount(int*, ENUM_ASSAULT_STATE)
// ---------------------------------------------------------------------------
void CAssaultPlace::_GetTeamCount(int* counts, ENUM_ASSAULT_STATE state)
{
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()) {
            if ((int)m_assaulters[i].GetState() == (int)state) {
                counts[(int)m_assaulters[i].GetTeam()] =
                    counts[(int)m_assaulters[i].GetTeam()] + 1;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e90a0 (size 0x6b)：_MakeChangeHp(PacketGuard*, int, int)
// 包 0x88：uid(short)+hp(int)
// ---------------------------------------------------------------------------
void CAssaultPlace::_MakeChangeHp(PacketGuard* p, int index, int hp)
{
    p->put_header(0, 0x88);
    unsigned short uid =
        m_assaulters[index].GetUserUniqueId();  // movzwl %ax zero-extend
    p->put_short((int)(uid & 0xffff));
    p->put_int(hp);
    p->finalize(true);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e910c (size 0x35)：_MakeGiveAlarm(PacketGuard*) —— 裸包 0x8b。
// 无判空防御（P140：ORIG 切片 0x082e910c 无 `cmpl $0,0xc(%ebp); je`，重建
// over-guard 已删；两调用点 152/364 均传局部 &packet，语义无损）。
// ---------------------------------------------------------------------------
void CAssaultPlace::_MakeGiveAlarm(PacketGuard* p)
{
    p->put_header(0, 0x8b);
    p->finalize(true);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e88a0 (size 0x35)：_MakeEndAssault(PacketGuard*) —— 裸包 0x84
// ---------------------------------------------------------------------------
void CAssaultPlace::_MakeEndAssault(PacketGuard* p)
{
    p->put_header(0, 0x84);
    p->finalize(true);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebd64 (size 0x41)：_IsProcsssChaos() —— 事件未进行时 true
// （GetRepeatEvent(0x20)→vtable+0x34 槽 IsEventing；!=1 时返回 true）
// ---------------------------------------------------------------------------
bool CAssaultPlace::_IsProcsssChaos()
{
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x20);
    return event->IsEventing((CUser*)0) == false;
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebaee (size 0x09)：_IsChangeDieState() —— ORIG 恒返 1
// ---------------------------------------------------------------------------
bool CAssaultPlace::_IsChangeDieState()
{
    // ORIG 0x082ebaee: empty body, return 1
    return true;
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e82a2 (size 0x5d)：_SetReadyState() —— 场状态置 2 + 全员 state 1
// ---------------------------------------------------------------------------
void CAssaultPlace::_SetReadyState()
{
    m_state = 2;
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty())
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)1);
    }
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e91fa (size 0x83)：DoConnectP2P() —— 场状态置 3 + timer 15s
// ---------------------------------------------------------------------------
void CAssaultPlace::DoConnectP2P()
{
    m_state = 3;
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty())
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)3);
    }
    TimerConnectP2PAssaultTimeout::registNextTimer(
        0xf, m_field104 /*+0x104*/, m_authenValue /*+0x108*/);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e804a (size 0x4d)：_BackupPartyInfo(CParty*, ENUM_ASSAULT_TEAM)
// this+(team*4)+0x12c —— 即 m_parties[team]（PartyInfo::Set 写指针）
// ---------------------------------------------------------------------------
void CAssaultPlace::_BackupPartyInfo(CParty* party, ENUM_ASSAULT_TEAM team)
{
    // asm 对 party==0 与 !=0 都调 PartyInfo::Set（参数不同路径同型）。
    m_parties[team].Set(party);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e7ac4 (size 0x65)：_SendGiveAlarm() —— MakeGiveAlarm → SendPacket
// ---------------------------------------------------------------------------
void CAssaultPlace::_SendGiveAlarm()
{
    PacketGuard packet;
    _MakeGiveAlarm(&packet);
    SendPacket(packet);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e7918 (size 0x6d)：_SendUserInfo() —— UserInfo(USERINFO==1) 广播
// ---------------------------------------------------------------------------
void CAssaultPlace::_SendUserInfo()
{
    PacketGuard packet;
    _MakeUserInfo(&packet, (ENUM_USERINFO)1);
    SendPacket(packet);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e919a (size 0x5f)：_CheckAssaultable() —— state5 双队各>0
// ---------------------------------------------------------------------------
bool CAssaultPlace::_CheckAssaultable()
{
    int counts[2] = { 0, 0 };  // asm 仅清 -0x14/-0x10 两槽
    _GetTeamCount(counts, (ENUM_ASSAULT_STATE)5);
    for (int i = 0; i <= 1; ++i) {
        if (counts[i] == 0)
            return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e949c (size 0x65)：ClearBadP2PUser()
// 反复求 min/max 并按 min 清坏连接槽，直至区间收敛
// ---------------------------------------------------------------------------
void CAssaultPlace::ClearBadP2PUser()
{
    int minX = 0;
    int maxX = 0;
    for (int i = 0; i < 8; ++i) {
        _GetConnectP2PMaxMinPoint(minX, maxX);
        if (minX == maxX)
            return;          // asm cmp max,min je end
        _ClearConnectP2PRelative(minX);
    }
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebaf8 (size 0xa3)：_ChangeDieState(int)
// ---------------------------------------------------------------------------
void CAssaultPlace::_ChangeDieState(int index)
{
    m_assaulters[index].SetState((ENUM_ASSAULT_STATE)6);
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x20);
    if (event->IsEventing((CUser*)0))
        return;
    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        if (((CUserCharacInfo*)m_assaulters[index].GetUser())->getCurCharacVill()
            == 7)
        {
            return;  // bVar1=false：不离场
        }
    }
    // bVar1=true：
    m_assaulters[index].DieUser();
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebb9c (size 0xc7)：_ProcessChaos(int killer, int victim)
// ---------------------------------------------------------------------------
void CAssaultPlace::_ProcessChaos(int killer, int victim)
{
    if (victim < 0 || victim > 7)
        return;
    if ((int)m_assaulters[victim].GetTeam() != m_winTeam /*+0x124*/)
        return;

    CUserCharacInfo* killerUser =
        (CUserCharacInfo*)m_assaulters[killer].GetUser();
    CUserCharacInfo* victimUser = (CUserCharacInfo*)m_assaulters[victim].GetUser();
    if (victimUser == 0)
        return;

    unsigned char chaosFlag =
        m_chaosFlags[m_assaulters[killer].GetTeam()];  // this[0x128+killerTeam]
    if (chaosFlag != 1) {
        victimUser->AddCurCharacChaosPoint(10);
        if (victimUser->getCurCharacR() != 0) {
            PacketGuard packet;
            packet.put_header(0, 2);
            packet.put_byte(0);
            packet.put_short(1);
            ((CUser*)victimUser)->make_basic_info((char*)&packet, 0);
            packet.finalize(true);
            G_GameWorld()->send_all(packet, (CUser*)victimUser);
        }
    }
    if (killerUser != 0) {
        if (killerUser->GetCurCharacChaosPoint() < 1)
            victimUser->IncCurCharacChaosKillCount();
        else
            victimUser->IncCurCharacChaosModeCount();
        killerUser->IncCurCharacChaosDieCount();
    }
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e9502 (size 0x49f)：StartAssault()
// 返回 1=成功开战（ebx 结果），0=转结束流程。
// PowerWar 计数段依赖 CPowerWarLog（WAMgr 组权威头）。
// ---------------------------------------------------------------------------
bool CAssaultPlace::StartAssault()
{
    ClearBadP2PUser();

    if (m_field100 /*+0x100 参战人数*/ == 0) {
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\a');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\a');
        if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
            CPowerWarLog* log = GlobalData::s_power_manager->GetPowerWarLog();
            log->IncStartFailureCount();
            log = GlobalData::s_power_manager->GetPowerWarLog();
            log->IncP2PBadCountFailure();
        }
        m_state = 7;
        CloseAssault();
        return false;
    }

    if (!_CheckPlayable()) {   // xor $1 后 test：!playable 分支
        m_state = 6;
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x06');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x06');
        if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
            CPowerWarLog* log = GlobalData::s_power_manager->GetPowerWarLog();
            log->IncStartFailureCount();
            log = GlobalData::s_power_manager->GetPowerWarLog();
            log->IncP2PBadCountFailure();
        }
        EndAssault();
        return false;
    }

    m_state = 4;
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()) {
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)5);
            disconnect_detecter::GetInstanceDisconnectDetecter()->RegisterUser(
                m_assaulters[i].GetUser());
        }
    }
    if (m_parties[0].m_field00 != 0)
        ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x04');
    if (m_parties[1].m_field00 != 0)
        ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x04');

    PacketGuard packet;
    _MakeStartAssault(&packet);
    SendPacket(packet);
    packet.clear();  // InterfacePacketBuf::clear @0x080cb8e6
    _MakeViewAssaultIconPlayer(&packet, true);
    _SendPacketToArea(packet);
    _SendUserState();
    TimerEndAssault::registNextTimer(
        0xb4, m_field104 /*+0x104*/, m_authenValue /*+0x108*/);

    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        CPowerWarLog* log = GlobalData::s_power_manager->GetPowerWarLog();
        log->IncStartSuccessCount();
        cMyTrace trace1(__PRETTY_FUNCTION__,
                        0x897, 0);
        trace1("POWER WAR START SUCCESS");
        for (int i = 0; i < 8; ++i) {
            if (!m_assaulters[i].Empty()) {
                if ((int)m_assaulters[i].GetTeam() == 0) {
                    const char* name =
                        ((CUserCharacInfo*)m_assaulters[i].GetUser())
                            ->getCurCharacName();
                    cMyTrace traceB(
                        __PRETTY_FUNCTION__,
                        0x89f, 0);
                    traceB("BLUE TEAM : %s", name);
                } else {
                    const char* name =
                        ((CUserCharacInfo*)m_assaulters[i].GetUser())
                            ->getCurCharacName();
                    cMyTrace traceR(
                        __PRETTY_FUNCTION__,
                        0x8a1, 0);
                    traceR("RED TEAM : %s", name);
                }
            }
        }
    }

    m_abuseSec = /*+0x13c*/
        (int)GlobalData::s_systemTime_.getCurSec();
    _PowerWarAssaultCheckIn();
    return true;
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e99a2 (size 0x133)：EndAssault() —— 单播结算包 + 清场计时
// ---------------------------------------------------------------------------
void CAssaultPlace::EndAssault()
{
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()) {
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)7);
            PacketGuard packet;
            _MakeEndAssault(&packet);
            CUser* user = m_assaulters[i].GetUser();
            user->Send(packet);
        }
    }

    int aliveCounts[2] = { 0, 0 };
    _GetTeamCountAliveUser(aliveCounts);  // -0x24 起（两连 int）
    if (aliveCounts[0] == 0)
        m_teamFlag[1] = 1;                // this+0x121
    if (aliveCounts[1] == 0)
        m_teamFlag[0] = 1;                // this+0x120

    TimerCloseAssault::registNextTimer(
        5, m_field104 /*+0x104*/, m_authenValue /*+0x108*/);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ea004 (size 0x183)：TimeoutEndAssault()
// state==4 才推进到 5 并 EndAssault。
// ---------------------------------------------------------------------------
void CAssaultPlace::TimeoutEndAssault()
{
    cMyTrace trace(__PRETTY_FUNCTION__,
                   0x9ce, 0);
    trace("TimeoutEndAssault() state(%d) %s,%s,%s,%s,%s,%s,%s,%s",
          m_state,
          m_assaulters[0].GetCharacName(), m_assaulters[1].GetCharacName(),
          m_assaulters[2].GetCharacName(), m_assaulters[3].GetCharacName(),
          m_assaulters[4].GetCharacName(), m_assaulters[5].GetCharacName(),
          m_assaulters[6].GetCharacName(), m_assaulters[7].GetCharacName());

    if (m_state != 4)
        return;
    m_state = 5;
    if (m_parties[0].m_field00 != 0)
        ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x05');
    if (m_parties[1].m_field00 != 0)
        ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x05');
    EndAssault();
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e9502 系列使用的 enemy 局部逻辑已并入上面 StartAssault；
// 本段以下为 ConnectP2P/CompleteLoad/Revival/Evade/Timeout 等。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ORIG T 0x82eb956 (size 0x197)：ConnectP2P(CUser*, char*, int)
// ---------------------------------------------------------------------------
void CAssaultPlace::ConnectP2P(CUser* user, char* ip, int len)
{
    if (m_state != 3)
        throw (ENUM_ERROR)0x13;

    char allConnected = 1;
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()) {
            if (m_assaulters[i].GetUser() == user) {
                if ((int)m_assaulters[i].GetState() != 3)
                    throw (ENUM_ERROR)0x12;
                m_assaulters[i].SetState((ENUM_ASSAULT_STATE)4);
                m_assaulters[i].SetUserConnectP2P(ip, len);
            }
            if ((int)m_assaulters[i].GetState() != 4)
                allConnected = 0;
        }
    }

    if (!allConnected)
        return;
    StartAssault();
    if (m_parties[0].m_field00 != 0)
        ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x04');
    if (m_parties[1].m_field00 != 0)
        ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x04');
}

// ---------------------------------------------------------------------------
// ORIG T 0x82eb774 (size 0x1e1)：CompleteLoad(CUser*)
// ---------------------------------------------------------------------------
void CAssaultPlace::CompleteLoad(CUser* user)
{
    if (m_state != 2)
        throw (ENUM_ERROR)0x13;

    char allLoaded = 1;
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()) {
            CUser* slotUser = m_assaulters[i].GetUser();
            if (slotUser == user) {
                if ((int)m_assaulters[i].GetState() != 1)
                    throw (ENUM_ERROR)0x12;
                m_assaulters[i].SetState((ENUM_ASSAULT_STATE)2);
                PacketGuard packet;
                _MakeCompleteLoad(&packet, i);
                SendPacket(packet);
            }
            if ((int)m_assaulters[i].GetState() != 2)
                allLoaded = 0;
        }
    }

    if (!allLoaded)
        return;
    DoConnectP2P();
    if (m_parties[0].m_field00 != 0)
        ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x03');
    if (m_parties[1].m_field00 != 0)
        ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x03');
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ea716 (size 0x183)：TimeoutConnectP2P()
// 超时尚未 P2P 连接的参战者逐一登记/通报，随后重试开战；若期间某空场转失败
// （state 变 6）则提前返回 0。
// ---------------------------------------------------------------------------
char CAssaultPlace::TimeoutConnectP2P()
{
    if (m_state != 3)
        throw (ENUM_ERROR)0x13;

    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()
            && (int)m_assaulters[i].GetState() != 4)
        {
            if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
                CPowerWarLog* log = GlobalData::s_power_manager->GetPowerWarLog();
                log->IncP2PTimeOutCountSecede();
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0xa80, 0);
                trace("POWER WAR P2P SECEDE");
                for (int j = 0; j < 8; ++j) {
                    if (!m_assaulters[j].Empty()) {
                        if ((int)m_assaulters[j].GetTeam() == 0) {
                            const char* name =
                                ((CUserCharacInfo*)m_assaulters[j].GetUser())
                                    ->getCurCharacName();
                            cMyTrace tB(
                                __PRETTY_FUNCTION__,
                                0xa88, 0);
                            tB("BLUE TEAM : %s", name);
                        } else {
                            const char* name =
                                ((CUserCharacInfo*)m_assaulters[j].GetUser())
                                    ->getCurCharacName();
                            cMyTrace tR(
                                __PRETTY_FUNCTION__,
                                0xa8a, 0);
                            tR("RED TEAM : %s", name);
                        }
                    }
                }
                CUserCharacInfo* secedeUser =
                    (CUserCharacInfo*)m_assaulters[i].GetUser();
                cMyTrace tS(
                    __PRETTY_FUNCTION__,
                    0xa8d, 0);
                tS("SECEDE : %s", secedeUser ? secedeUser->getCurCharacName()
                                             : "");
            }

            CUser* leaveUser = m_assaulters[i].GetUser();
            if (GlobalData::s_power_manager->IsPowerWarEventOn()
                && ((CUserCharacInfo*)leaveUser)->getCurCharacVill() == 7)
            {
                PacketGuard packet;
                packet.put_header(0, 0xe6);
                packet.put_byte(2);
                packet.finalize(true);
                leaveUser->Send(packet);
            }
        }
    }

    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()
            && (int)m_assaulters[i].GetState() != 4)
        {
            LeaveAssaultPlace(i, true, false);
            if (m_state == 6)
                return '\0';
        }
    }

    char started = StartAssault() ? 1 : 0;
    if (started) {
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x04');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x04');
    }
    return started;
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ed43e (size 0x1b5)：RevivalUser(CUser*, int&)
// ---------------------------------------------------------------------------
void CAssaultPlace::RevivalUser(CUser* user, int& result)
{
    if (!((CUserCharacInfo*)user)->IsCurCharacGhost())
        throw (ENUM_ERROR)0x12;

    int nowSec = (int)GlobalData::s_systemTime_.getCurSec();

    char powerWarGate = 0;
    if (GlobalData::s_power_manager->IsPowerWarEventOn()
        || ((CUserCharacInfo*)user)->getCurCharacVill() == 7)
    {
        powerWarGate = 1;
    }

    if (powerWarGate) {
        if (!GlobalData::s_power_manager->IsAbleToRevival(user,
                                                          (unsigned int)nowSec,
                                                          result))
            throw (ENUM_ERROR)0x16;

    } else {
        int dieDiff =
            nowSec - ((CUserCharacInfo*)user)->GetCurCharacChaosDieTime();
        if (!(dieDiff > 0x257)) {  // <=599
            result = 0x258 - dieDiff;
            throw (ENUM_ERROR)0x16;
        }
    }

    PacketGuard packet;
    unsigned short uid = ((CUser*)user)->get_unique_id();
    _MakeRevivalAssaultPlayer(&packet, uid & 0xffff);
    G_GameWorld()->send_all(packet);
    ((CUserCharacInfo*)user)->SetCurCharacGhost(false);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82eb49c (size 0x145)：EvadeAssaultPlace(CUser*)
// ---------------------------------------------------------------------------
void CAssaultPlace::EvadeAssaultPlace(CUser* user)
{
    if ((unsigned int)(m_state - 2) > 1u) {
        int nowSec = (int)GlobalData::s_systemTime_.getCurSec();
        if (nowSec - m_abuseSec > 5)
            throw (ENUM_ERROR)0x13;
    }
    if (m_checkUser /*+0x134*/ == user)
        throw (ENUM_ERROR)0x13;

    int idx = _GetUserIndex(user, true);
    ENUM_ASSAULT_TEAM team =
        m_assaulters[idx].GetTeam();
    if ((int)team != m_winTeam /*+0x124*/)
        throw (ENUM_ERROR)0x13;

    idx = _GetUserIndex(user, true);
    LeaveAssaultPlace(idx, true, true);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ecaee (size 0x2cd)：_SendHoldVictory(CUser*, CUser*, int)
// 包 0x109：双方连破名单广播（无队用单名，有队 4 成员名）。
// ---------------------------------------------------------------------------
void CAssaultPlace::_SendHoldVictory(CUser* userA, CUser* userB, int winTeam)
{
    PacketGuard packet;
    packet.put_header(0, 0x109);

    packet.put_byte((int)((CUser*)userA)->getPowerSide());
    packet.put_int(winTeam + 1);

    CParty* partyA = (CParty*)((CUser*)userA)->GetParty();
    if (partyA == 0) {
        packet.put_byte(1);
        const char* name =
            ((CUserCharacInfo*)userA)->getCurCharacName();
        packet.put_int((int)strlen(name));
        packet.put_str(name, (int)strlen(name));
    } else {
        packet.put_byte(partyA->get_member_count());
        for (int s = 0; s < 4; ++s) {
            CUser* member = (CUser*)partyA->get_user(s);
            if (member != 0) {
                const char* name =
                    ((CUserCharacInfo*)member)->getCurCharacName();
                packet.put_int((int)strlen(name));
                packet.put_str(name, (int)strlen(name));
            }
        }
    }

    CParty* partyB = (CParty*)((CUser*)userB)->GetParty();
    if (partyB == 0) {
        packet.put_byte(1);
        const char* name =
            ((CUserCharacInfo*)userB)->getCurCharacName();
        packet.put_int((int)strlen(name));
        packet.put_str(name, (int)strlen(name));
    } else {
        packet.put_byte(partyB->get_member_count());
        for (int s = 0; s < 4; ++s) {
            CUser* member = (CUser*)partyB->get_user(s);
            if (member != 0) {
                const char* name =
                    ((CUserCharacInfo*)member)->getCurCharacName();
                packet.put_int((int)strlen(name));
                packet.put_str(name, (int)strlen(name));
            }
        }
    }

    packet.finalize(true);
    G_GameWorld()->send_to_vill(7, packet);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ec89c (size 0x251)：_VictoryBonus(CUser*, char)
// 返回 penalty + breakBonus。阵营判定同 getPowerSide(char side)。
// ---------------------------------------------------------------------------
int CAssaultPlace::_VictoryBonus(CUser* user, char side)
{
    CUser* enemy = 0;
    int penalty = 0;
    int victoryPenaltyValue = 0;
    int breakBonus = 0;
    int winCount = 0;
    int myVictory = 0;
    int enemySideChecked = 0;

    for (int i = 0; i < 8; ++i) {
        CUser* slotUser = m_assaulters[i].GetUser();
        if (slotUser != 0) {
            if (((CUser*)user)->getPowerSide()
                != ((CUser*)slotUser)->getPowerSide())
            {
                enemy = slotUser;
                break;
            }
        }
    }

    if (((CUser*)user)->getPowerSide() == (unsigned char)side) {
        myVictory = ((CUser*)user)->GetPowerWarPartyVictory();
        PowerParameterScript* powerParam =
            &G_CDataManager()->m_powerParameter;  // G_CDataManager()+0xa628
        stVictoryPenalty penaltyOut = powerParam->GetPenalty(myVictory);
        victoryPenaltyValue = penaltyOut.m_fieldC;

        if (enemy != 0) {
            winCount = ((CUser*)enemy)->GetPowerWarPartyVictory();
            if (winCount > 4) {
                breakBonus = G_CDataManager()->m_powerParameter.GetBreakBonus(
                    winCount);
                _SendHoldVictory(user, enemy, winCount);
            }
        }
        _SendVictoryBonus(user, myVictory, victoryPenaltyValue, winCount,
                          breakBonus);
    } else if (enemy != 0) {
        myVictory = ((CUser*)enemy)->GetPowerWarPartyVictory();
        stVictoryPenalty penaltyOut =
            G_CDataManager()->m_powerParameter.GetPenalty(myVictory);
        victoryPenaltyValue = penaltyOut.m_fieldC;

        winCount = ((CUser*)user)->GetPowerWarPartyVictory();
        if (winCount > 4) {
            breakBonus = G_CDataManager()->m_powerParameter.GetBreakBonus(winCount);
            _SendHoldVictory(enemy, user, winCount);
        }
        _SendVictoryBonus(enemy, myVictory, victoryPenaltyValue, winCount,
                          breakBonus);
    }
    return victoryPenaltyValue + breakBonus;
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e8098 (size 0x209)：_SeparateTeam(CUser*, CUser*)
// 双队长各自队伍列出并布阵。依赖 _AddUser/_MoveUser/_MoveAssaultPlace/
// _SendUserUdpIpPort（B 组交付）。
// ---------------------------------------------------------------------------
void CAssaultPlace::_SeparateTeam(CUser* leader1, CUser* leader2)
{
    std::vector<CUser*> lists[2];
    CParty* party1 = leader1->GetUserListByParty(lists[0]);
    _BackupPartyInfo(party1, (ENUM_ASSAULT_TEAM)0);
    CParty* party2 = leader2->GetUserListByParty(lists[1]);
    _BackupPartyInfo(party2, (ENUM_ASSAULT_TEAM)1);

    for (unsigned int u = 0; u < 8; ++u) {
        for (int t = 0; t < 2; ++t) {
            if (lists[t].size() > u) {
                CUser* member = lists[t].at(u);
                _AddUser(member, (ENUM_ASSAULT_TEAM)t);
            }
        }
    }

    m_chaosFlags[0] =
        (((CUserCharacInfo*)leader1)->GetCurCharacChaosPoint() > 0) ? 1 : 0;
    m_chaosFlags[1] =
        (((CUserCharacInfo*)leader2)->GetCurCharacChaosPoint() > 0) ? 1 : 0;

    CUser* users[2];
    users[0] = leader1;
    users[1] = leader2;
    _DispositionUser(users);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e7bbc (size 0x14d)：_DispositionUser(CUser**)
// ---------------------------------------------------------------------------
void CAssaultPlace::_DispositionUser(CUser** users)
{
    int counts[2] = { 0, 0 };
    _GetTeamCount(counts);
    CStaticDisposition disposition(users, counts);

    if (!G_GameWorld()->get_disposition(m_pos.m_x /*+0x118*/,
                                        m_pos.m_y /*+0x11c*/, &disposition))
        throw (ENUM_ERROR)0x03;

    int flagX = 0;
    int flagY = 0;
    disposition.GetFlagPoint(flagX, flagY);   // this+0x110 / +0x114 输出目标
    _SendGiveAlarm();
    _SendUserInfo();
    _SendUserUdpIpPort();
    _MoveUser(&disposition, m_pos.m_x, m_pos.m_y);
    _MoveAssaultPlace();
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e8b66 (size 0xf3)：_MakeUserInfo(PacketGuard*, ENUM_USERINFO)
// 包 0x02：byte(type)+short(count)+逐人 basic_info。
// ---------------------------------------------------------------------------
void CAssaultPlace::_MakeUserInfo(PacketGuard* p, ENUM_USERINFO type)
{
    if (p == 0)
        return;
    p->put_header(0, 2);
    p->put_byte((int)type);
    int indexSlot = 0;
    int count = 0;
    indexSlot = p->get_index();
    p->put_short(count);
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()) {
            CUser* user = m_assaulters[i].GetUser();
            user->make_basic_info((char*)p, (char)type);
            count = count + 1;
        }
    }
    p->put_short(indexSlot, count);
    p->finalize(true);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e75ca (size 0xd1)：_AvailableAssault(CUser*, int&)
// 年龄门槛 0xf（PowerWar 中 vill==7 时 0xc）；<0x1e 级未入场检查 → 0xe/0xd；
// 组队分支：party is_limit_minimum_age→0x5f、is_limit_minimum_level→0xff/0xfe。
// ---------------------------------------------------------------------------
void CAssaultPlace::_AvailableAssault(CUser* user, int& result)
{
    CParty* party = 0;
    if (user->CheckInParty()) {
        party = (CParty*)user->GetParty();
    }

    int needAge = 0xf;
    bool powerWarVill;
    if (!GlobalData::s_power_manager->IsPowerWarEventOn()
        || ((CUserCharacInfo*)user)->getCurCharacVill() != 7)
    {
        powerWarVill = false;
    } else {
        powerWarVill = true;
    }
    if (powerWarVill) {
        needAge = 0xc;
    }

    if (!user->CheckLimitMiniumAge(needAge, result)) {
        throw (ENUM_ERROR)0x5f;   // ORIG __cxa_throw ENUM_ERROR(0x5f)
    }

    if (((CUserCharacInfo*)user)->get_charac_level() < 0x1e) {
        unsigned int uid = (unsigned int)user->get_unique_id();
        result = (int)(uid & 0xffff);
        if (m_checkUser /* +0x134 */ != user) {
            throw (ENUM_ERROR)0xe;
        }
        throw (ENUM_ERROR)0xd;
    }

    if (party != 0) {
        int partyNeedAge = 0xf;
        bool partyPowerWarVill;
        if (!GlobalData::s_power_manager->IsPowerWarEventOn()
            || ((CUserCharacInfo*)user)->getCurCharacVill() != 7)
        {
            partyPowerWarVill = false;
        } else {
            partyPowerWarVill = true;
        }
        if (partyPowerWarVill) {
            partyNeedAge = 0xc;
        }

        if (!party->is_limit_minimum_age(partyNeedAge, result)) {
            throw (ENUM_ERROR)0x5f;
        }
        if (party->is_limit_minimum_level(0x1e, result)) {
            return;
        }
        if (m_checkUser != user) {
            throw (ENUM_ERROR)0xff;
        }
        throw (ENUM_ERROR)0xfe;
    }
}
}  // namespace pvp_assault
