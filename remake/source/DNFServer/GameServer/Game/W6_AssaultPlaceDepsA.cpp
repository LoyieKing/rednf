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
        if (m_assaulters[i].Empty())
            continue;
        if ((int)m_assaulters[i].GetTeam() == (int)team) {
            count = count + 1;
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
        // b28 BL6: f3 岛形（P164 族）——Empty continue→nop 岛，正向 == 包 ++，
        // 体尾 4.4 保留前向 jmp 统一尾（ORIG 0x82e7288 jne/je/jmp+nop 全等）。
        if (m_assaulters[i].Empty())
            continue;
        if ((int)m_assaulters[i].GetState() == (int)state) {
            ++counts[(int)m_assaulters[i].GetTeam()];
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
    p->put_short((int)(m_assaulters[index].GetUserUniqueId() & 0xffff));
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
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing((CUser*)0))
        return false;
    return true;
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
        if (m_assaulters[i].Empty())
            continue;
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
        if (m_assaulters[i].Empty())
            continue;
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
    // ORIG 双路各发一次 PartyInfo::Set（0x82e806f/0x82e8090）：显式 if/else
    // 物化两 call（4.4 -O0 不合并两路 ⇒ 逐条同）。
    if (party != NULL)
        m_parties[team].Set(party);
    else
        m_parties[team].Set(NULL);
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
        _ClearConnectP2PRelative(maxX);   // ORIG -0x14=maxX 槽（第三实参引用）
    }
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebaf8 (size 0xa3)：_ChangeDieState(int)
// ---------------------------------------------------------------------------
void CAssaultPlace::_ChangeDieState(int index)
{
    m_assaulters[index].SetState((ENUM_ASSAULT_STATE)6);
    // b28 BM4: P223 goto 尾空标签——ORIG +0x4a jne→+0xa0 nop 岛 + body 尾
    // +0x9e jmp +0xa1（eventing-true 落点在 body 之后 leave 之前）。嵌套
    // if{}else{} 形 4.4 jump-thread 合并落点缺此 jmp；goto 形保留 nop 中继。
    // 短路 `||` 段沿用 BL6 v6（!boolcallee 的 xor$1 直传形对齐 +0x59）。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x20)->IsEventing((CUser*)0))
        goto EVENT_DONE;
    if (!GlobalData::s_power_manager->IsPowerWarEventOn()
        || ((CUserCharacInfo*)m_assaulters[index].GetUser())
               ->getCurCharacVill() != 7)
        m_assaulters[index].DieUser();
EVENT_DONE:
    ;
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ebb9c (size 0xc7)：_ProcessChaos(int killer, int victim)
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF：P226 语句表达式复现 sete 守卫 + P248
//   档② bool 字段（m_chaosFlags）裸 if + 槽归位后，残差 = ORIG `cmp %edx,%eax`
//   角色互换（N7/N18a；交换操作数引 push %ebx 劣化已回退）+ make_basic_info/
//   send_all 实参装载序（4.1 receiver 先 vs 4.4 实参先，P75 族）。p167 CLEAR。ORIG 0x082ebb9c
void CAssaultPlace::_ProcessChaos(int killer, int victim)
{
    if (victim < 0 || victim > 7)
        return;
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ebbd2 `cmp %edx,%eax; sete %al;
    //   test %al,%al; je <函数尾>`（零槽物化）⇒ P226 裸语句表达式条件向。
    //   ORIG 0x082ebc25 `movzbl 0x128(%edx,%eax,1),%eax; xor $1; test %al,%al; je`
    //   ⇒ P248 档② 字段 bool 化 + 裸 if(!field)，无 chaosFlag 槽。
    // 操作数序：交换成 `field == call` 使 4.4 把字段提前载入 %ebx（push %ebx
    //   劣化，实测回退）；残差 = ORIG `cmp %edx,%eax` vs 我方 `cmp %eax,%edx`
    //   角色互换（N7/N18a 族）。
    if (({ (int)m_assaulters[victim].GetTeam() == m_winTeam /*+0x124*/; })) {
        CUserCharacInfo* killerUser =
            (CUserCharacInfo*)m_assaulters[killer].GetUser();
        CUserCharacInfo* victimUser = (CUserCharacInfo*)m_assaulters[victim].GetUser();
        if (victimUser != 0) {
            if (!m_chaosFlags[m_assaulters[killer].GetTeam()]) {
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
    }
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e9502 (size 0x49f)：StartAssault()
// 返回 1=成功开战（ebx 结果），0=转结束流程。
// PowerWar 计数段依赖 CPowerWarLog（WAMgr 组权威头）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF：P223 循环岛 + P221 trace 临时链后，残差
//   N95——4.4 局部槽基恒 ebp-8（帧顶留 8B gap），ORIG 大局部顶到 ebp ⇒ 全帧槽号
//   差 8（PacketGuard -0x54 vs ORIG -0x4c、trace1 -0x48 vs -0x40、i -0x18 vs -0x10，
//   帧同为 0x60；double framePad 反致下移）；p167 CLEAR 53=53。ORIG 0x082e9502
bool CAssaultPlace::StartAssault()
{
    ClearBadP2PUser();

    if (m_field100 /*+0x100 参战人数*/ == 0) {
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\a');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\a');
        if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
            GlobalData::s_power_manager->GetPowerWarLog()->IncStartFailureCount();
            GlobalData::s_power_manager->GetPowerWarLog()->IncP2PBadCountFailure();
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
            GlobalData::s_power_manager->GetPowerWarLog()->IncStartFailureCount();
            GlobalData::s_power_manager->GetPowerWarLog()->IncP2PBadCountFailure();
        }
        EndAssault();
        return false;
    }

    m_state = 4;
    for (int i = 0; i < 8; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e96b7 `jne +0x1f6`（nop 岛）
        //   + 体尾 `jmp +0x1f7` ⇒ P223 双标签 goto 形。
        if (m_assaulters[i].Empty())
            goto LOOP_CONT;
        {
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)5);
            disconnect_detecter::GetInstanceDisconnectDetecter()->RegisterUser(
                m_assaulters[i].GetUser());
        }
        goto LOOP_END;
LOOP_CONT:
        ;
LOOP_END:
        ;
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
        GlobalData::s_power_manager->GetPowerWarLog()->IncStartSuccessCount();
        // [b28d-solo-W6_AssaultPlaceDepsA] P221：ORIG 槽序 trace1 -0x40 / traceB
        //   -0x30 / traceR -0x20（先声明深）⇒ 全表达式临时链；具名 trace 使 4.4
        //   反向（-0x34/-0x24/-0x14）并把 PacketGuard 挤到 -0x24。
        cMyTrace(__PRETTY_FUNCTION__, 0x897, 0)("POWER WAR START SUCCESS");
        for (int i = 0; i < 8; ++i) {
            // ORIG 0x082e986a `test %al,%al; jne +0x436`（nop 岛）+ 体尾 jmp。
            if (m_assaulters[i].Empty())
                goto TRACE_CONT;
            {
                if ((int)m_assaulters[i].GetTeam() == 0) {
                    const char* name =
                        ((CUserCharacInfo*)m_assaulters[i].GetUser())
                            ->getCurCharacName();
                    cMyTrace(__PRETTY_FUNCTION__, 0x89f, 0)("BLUE TEAM : %s", name);
                } else {
                    const char* name =
                        ((CUserCharacInfo*)m_assaulters[i].GetUser())
                            ->getCurCharacName();
                    cMyTrace(__PRETTY_FUNCTION__, 0x8a1, 0)("RED TEAM : %s", name);
                }
            }
            goto TRACE_END;
TRACE_CONT:
            ;
TRACE_END:
            ;
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
// [b28d-solo-W6_AssaultPlaceDepsA] NEAR：P223 双标签 goto 消循环 nop 岛 +
//   P309 头部声明使 i=-0x10/allDead=-0x9 归位后 88=88；残差 N82附——4.4 聚合
//   对象优先占低槽：PacketGuard -0x24 vs ORIG -0x1c、aliveCounts -0x18/-0x14 vs
//   ORIG -0x24/-0x20。p167 CLEAR。ORIG 0x082e99a2
void CAssaultPlace::EndAssault()
{
    // [b28d-solo-W6_AssaultPlaceDepsA] P309 试验：ORIG 槽 i=-0x10 / allDead=-0x9
    //   （4.1 先声明深）⇒ 头部声明 i、allDead（赋值留原位）试归位。
    int i;
    bool allDead;
    for (i = 0; i < 8; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e99c9 `test %al,%al; jne +0xad`
        //   （nop 岛）+ 体尾 `jmp +0xae` ⇒ P223 双标签 goto 形（if{}else{} 被 4.4
        //   jump-thread，少 jmp+nop 两条）。
        if (m_assaulters[i].Empty())
            goto LOOP_CONT;
        {
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)7);
            PacketGuard packet;
            _MakeEndAssault(&packet);
            ((CUser*)m_assaulters[i].GetUser())->Send(packet);
        }
        goto LOOP_END;
LOOP_CONT:
        ;
LOOP_END:
        ;
    }

    int aliveCounts[2] = { 0, 0 };
    // [b28d-62] P232：ORIG 0x082e9a71 有 dead bool 预清 `movb $0x0,-0x9`（后续
    //   无读取），缺失导致 i 槽 -0xc vs ORIG -0x10 全帧浅 4 ⇒ 按原样补声明。
    allDead = false;
    (void)allDead;
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
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF：P227 全表达式临时链复现 ctor 前右起
//   varargs 求值 + P285 正向包裹消 epilogue 尾岛后，仅剩 N18a——ORIG 0x082ea099
//   `mov 0x10c(%eax),%eax; mov %eax,%ebx` 两段 regmove 被 4.4 折单段（P248 档②
//   enum 实参 cast 实测无效，m_state 字段化跨 TU 不可行）。p167 CLEAR。ORIG 0x082ea004
void CAssaultPlace::TimeoutEndAssault()
{
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ea00d 段：8 个 GetCharacName 在
    //   cMyTrace ctor call **之前**右起求值（[7]→[0]，前 6 个落槽 -0x40..-0x2c、
    //   后 2 个落 %edi/%esi、state 落 %ebx），ctor 后才压 varargs —— P227 全表达式
    //   临时链形（具名局部 trace 使 4.4 把 ctor 提前，帧 0x5c vs ORIG 0x6c）。
    cMyTrace(__PRETTY_FUNCTION__, 0x9ce, 0)(
        "TimeoutEndAssault() state(%d) %s,%s,%s,%s,%s,%s,%s,%s",
        // P265：ORIG 0x082ea099 `mov 0x10c(%eax),%eax; mov %eax,%ebx` 两段中转
        //   ⇒ 实参处显式 enum cast（int 字段 + (ENUM_X) 实参）。
        (ENUM_ASSAULT_STATE)m_state,
        m_assaulters[0].GetCharacName(), m_assaulters[1].GetCharacName(),
        m_assaulters[2].GetCharacName(), m_assaulters[3].GetCharacName(),
        m_assaulters[4].GetCharacName(), m_assaulters[5].GetCharacName(),
        m_assaulters[6].GetCharacName(), m_assaulters[7].GetCharacName());

    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ea119 `jne +0x17b`（落 epilogue）
    //   + 体尾直落 ⇒ 正向包裹（早返形使 4.4 多 `jmp <epilogue>; nop` 尾岛，N121）。
    if (m_state == 4) {
        m_state = 5;
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x05');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x05');
        EndAssault();
    }
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e9502 系列使用的 enemy 局部逻辑已并入上面 StartAssault；
// 本段以下为 ConnectP2P/CompleteLoad/Revival/Evade/Timeout 等。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// ORIG T 0x82eb956 (size 0x197)：ConnectP2P(CUser*, char*, int)
// ---------------------------------------------------------------------------
// [b28d-62] AE：正向 if 包裹（P257/P285）消除体尾 `jmp+nop` 中继；p167 CLEAR；
//   108=108。ORIG 0x082eb956
void CAssaultPlace::ConnectP2P(CUser* user, char* ip, int len)
{
    if (m_state != 3)
        throw (ENUM_ERROR)0x13;

    char allConnected = 1;
    for (int i = 0; i < 8; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        if (m_assaulters[i].GetUser() == user) {
            if ((int)m_assaulters[i].GetState() != 3)
                throw (ENUM_ERROR)0x12;
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)4);
            m_assaulters[i].SetUserConnectP2P(ip, len);
        }
        if ((int)m_assaulters[i].GetState() != 4)
            allConnected = 0;
    }

    // [b28d-62] 正向包裹复现 ORIG 尾部 fall-through（早返 `if(!allConnected)return;`
    // 使 4.4 发体尾 `jmp <epilogue>; nop` 中继，ORIG 4.1 直落 leave；P257/P285）。
    if (allConnected) {
        StartAssault();
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x04');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x04');
    }
}

// ---------------------------------------------------------------------------
// ORIG T 0x82eb774 (size 0x1e1)：CompleteLoad(CUser*)
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] AE：P223 双标签 goto 消 nop 岛 + P0 删 slotUser
//   槽 + P59 正向包裹（allLoaded）⇒ 131=131 逐条对齐；p167 CLEAR。ORIG 0x082eb774
void CAssaultPlace::CompleteLoad(CUser* user)
{
    if (m_state != 2)
        throw (ENUM_ERROR)0x13;

    char allLoaded = 1;
    for (int i = 0; i < 8; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082eb7d9 `test %al,%al; jne +0x169`
        //   （nop 岛）+ body 尾 `jmp +0x16a` ⇒ P223 双标签 goto 形（if{}else{} 被
        //   4.4 jump-thread，少 jmp+nop 两条）。slotUser 无具名槽（P0 内联，
        //   ORIG 0x082eb7f0 `cmp 0xc(%ebp),%eax; sete`）。
        if (m_assaulters[i].Empty())
            goto LOOP_CONT;
        {
            if (m_assaulters[i].GetUser() == user) {
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
        goto LOOP_END;
LOOP_CONT:
        ;
LOOP_END:
        ;
    }

    if (allLoaded) {
        DoConnectP2P();
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x03');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x03');
    }
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ea716 (size 0x183)：TimeoutConnectP2P()
// 超时尚未 P2P 连接的参战者逐一登记/通报，随后重试开战；若期间某空场转失败
// （state 变 6）则提前返回 0。
// ---------------------------------------------------------------------------
// [b28c-L7][SIGFIX:pvp_assault::CAssaultPlace::TimeoutConnectP2P] char→bool
//（ORIG __PRETTY_FUNCTION__ 串 "bool ...TimeoutConnectP2P()"，tsv 第 16 行；
// mangled 不变；`started` 局部保持 char——它是 ORIG 的字节槽，非返回通道）。
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF：P221 trace 临时链 + SECEDE 直传 GetUser
//   （P171 照抄 ORIG，p167 EXTRA 归零 39=39）后，残差 N108（ORIG 双守卫分落
//   +0x304/+0x305 差 1，4.4 合并共享出口）+ N95 槽基 gap + N7。ORIG 0x082ea716
bool CAssaultPlace::TimeoutConnectP2P()
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
                // [b28d-solo-W6_AssaultPlaceDepsA] P221：ORIG 槽序 -0x5c/-0x4c/
                //   -0x3c/-0x2c（先声明深）⇒ 全表达式临时链。
                cMyTrace(__PRETTY_FUNCTION__, 0xa80, 0)("POWER WAR P2P SECEDE");
                for (int j = 0; j < 8; ++j) {
                    if (!m_assaulters[j].Empty()) {
                        if ((int)m_assaulters[j].GetTeam() == 0) {
                            const char* name =
                                ((CUserCharacInfo*)m_assaulters[j].GetUser())
                                    ->getCurCharacName();
                            cMyTrace(__PRETTY_FUNCTION__, 0xa88, 0)(
                                "BLUE TEAM : %s", name);
                        } else {
                            const char* name =
                                ((CUserCharacInfo*)m_assaulters[j].GetUser())
                                    ->getCurCharacName();
                            cMyTrace(__PRETTY_FUNCTION__, 0xa8a, 0)(
                                "RED TEAM : %s", name);
                        }
                    }
                }
                // ORIG 0x082ea909 `call GetUser; mov %eax,%ebx` 直传 GetUser 返回值
                //（无 getCurCharacName 调用，P171 照抄 ORIG；p167 EXTRA 归零）。
                cMyTrace(__PRETTY_FUNCTION__, 0xa8d, 0)(
                    "SECEDE : %s", m_assaulters[i].GetUser());
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
                return false;
        }
    }

    // ORIG 0x082eaab4 `call StartAssault; mov %al,-0x19`（无三目归一化）。
    char started = StartAssault();
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
// [b28d-solo-W6_AssaultPlaceDepsA] DIFF：`||` 直接物化 + uid 内联（P0）后逐条
//   对齐，仅剩 N104 EH 尾块相对序——ORIG 正常路径 PacketGuard D1+epilogue 在函数
//   最末、unwind 块在 ret 之后；4.4 把 unwind 块居中、正常路径尾多 `jmp` 跳过
//   （块内容逐条全等，仅两尾块互换）。p167 CLEAR。ORIG 0x082ed43e
void CAssaultPlace::RevivalUser(CUser* user, int& result)
{
    if (!((CUserCharacInfo*)user)->IsCurCharacGhost())
        throw (ENUM_ERROR)0x12;

    int nowSec = (int)GlobalData::s_systemTime_.getCurSec();

    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ed493 段：`||` 直接物化
    //   `mov $1/%eax ... mov $0/%eax; test; je`（P0 无具名 bool 槽；具名 char
    //   会多 movb 物化并把 nowSec 槽推到 -0x14、PacketGuard 推到 -0x20）。
    if (GlobalData::s_power_manager->IsPowerWarEventOn()
        || ((CUserCharacInfo*)user)->getCurCharacVill() == 7)
    {
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
    // ORIG 0x082ed58d `movzwl %ax,%eax` 直传（P0：无 uid 具名槽）。
    _MakeRevivalAssaultPlayer(&packet, (int)((CUser*)user)->get_unique_id());
    G_GameWorld()->send_all(packet);
    ((CUserCharacInfo*)user)->SetCurCharacGhost(false);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82eb49c (size 0x145)：EvadeAssaultPlace(CUser*)
// ---------------------------------------------------------------------------
// [b28d-62] N18a/N7：GetTeam 结果与 m_winTeam 的 cmp 操作数角色互换（4.4 this
//   入 %eax 触发原值 spill）；交换操作数序实测更劣。p167 CLEAR。ORIG 0x082eb49c
void CAssaultPlace::EvadeAssaultPlace(CUser* user)
{
    if ((unsigned int)(m_state - 2) > 1u) {
        if ((int)GlobalData::s_systemTime_.getCurSec() - m_abuseSec > 5)
            throw (ENUM_ERROR)0x13;
    }
    if (m_checkUser /*+0x134*/ == user)
        throw (ENUM_ERROR)0x13;

    int idx = _GetUserIndex(user, true);
    // b28 BL6 实测：交换操作数序→4.4 走 %ebx callee-saved+push 更劣（13）；
    // 保留原序，残差 4 条 = 4.4 把 this 装载选入 %eax 触发原值 spill %edx
    // （N18a 反向 regmove 假象，同 TU 探针 f0/f3/f4 均出 ORIG 形、真实函数
    // 上下文不可造）判跨版本寄存器分配。
    // [b28d-solo-W6_AssaultPlaceDepsA] 残差 N108+N7：ORIG 0x082eb4b1 `jbe +0x64`
    //   与 0x082eb4d2 `je +0x65` 双守卫分落两独立 landing（差 1）——4.4 恒合并共享
    //   出口（P223 双标签/负条件 goto 两形实测均被 jump-thread）；且 0x082eb564
    //   `cmp %edx,%eax` 角色互换（交换操作数序反引 push %ebx 劣化，已回退）。
    if ((int)m_assaulters[idx].GetTeam() != m_winTeam /*+0x124*/)
        throw (ENUM_ERROR)0x13;

    LeaveAssaultPlace(_GetUserIndex(user, true), true, true);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82ecaee (size 0x2cd)：_SendHoldVictory(CUser*, CUser*, int)
// 包 0x109：双方连破名单广播（无队用单名，有队 4 成员名）。
// ---------------------------------------------------------------------------
// [b28d-62] N98：p167 EXTRA 4×strlen（4.1 内联 word-at-a-time vs 4.4 恒 call）。
//   ORIG 0x082ecaee
void CAssaultPlace::_SendHoldVictory(CUser* userA, CUser* userB, int winTeam)
{
    PacketGuard packet;
    packet.put_header(0, 0x109);

    packet.put_byte((int)((CUser*)userA)->getPowerSide());
    packet.put_int(winTeam + 1);

    CParty* partyA = (CParty*)((CUser*)userA)->GetParty();
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ecb60 `cmpl $0x0,-0x40; je +0x10e`
    //   ⇒ 非零（有队）走 fall-through 有队分支、零落 +0x10e 无队块（P59 正极性）。
    if (partyA != 0) {
        packet.put_byte(partyA->get_member_count());
        // [b28d-solo-W6_AssaultPlaceDepsA] N82：ORIG 循环变量槽 s1=-0x3c/s2=-0x20
        //   （4.1 生命周期序）；4.4 按首次使用序（-0x38 共用 / 头部声明 -0x40/-0x3c），
        //   实测两形均不闭合 ⇒ 槽序封顶（member/name/len 随之下移 4）。
        for (int s = 0; s < 4; ++s) {
            CUser* member = (CUser*)partyA->get_user(s);
            if (member != 0) {
                const char* name =
                    ((CUserCharacInfo*)member)->getCurCharacName();
                // P0：ORIG strlen 结果落具名槽 -0x30，put_int/put_str 复用同一槽。
                size_t len = strlen(name);
                packet.put_int((int)len);
                packet.put_str(name, (int)len);
            }
        }
    } else {
        packet.put_byte(1);
        const char* name =
            ((CUserCharacInfo*)userA)->getCurCharacName();
        size_t len = strlen(name);
        packet.put_int((int)len);
        packet.put_str(name, (int)len);
    }

    CParty* partyB = (CParty*)((CUser*)userB)->GetParty();
    if (partyB != 0) {
        packet.put_byte(partyB->get_member_count());
        for (int s = 0; s < 4; ++s) {
            CUser* member = (CUser*)partyB->get_user(s);
            if (member != 0) {
                const char* name =
                    ((CUserCharacInfo*)member)->getCurCharacName();
                size_t len = strlen(name);
                packet.put_int((int)len);
                packet.put_str(name, (int)len);
            }
        }
    } else {
        packet.put_byte(1);
        const char* name =
            ((CUserCharacInfo*)userB)->getCurCharacName();
        size_t len = strlen(name);
        packet.put_int((int)len);
        packet.put_str(name, (int)len);
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
    // [b23S3C] P0 槽映射：ORIG 预清 6 槽（低→高 -0x20..-0xc）=
    // myVictory/penaltyValue/breakBonus/winCount/enemy/循环 i；删 dead local
    // enemySideChecked 与 powerParam/slotUser/penaltyOut 缓存槽（P48 每用点重取
    // GetUser()，ORIG 循环体 3 次 call GetUser、侧写结果经 callee-saved %ebx 跨调）。
    int myVictory = 0;
    int penaltyValue = 0;
    int breakBonus = 0;
    int winCount = 0;
    CUser* enemy = 0;

    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].GetUser() == 0)
            continue;
        if (((CUser*)user)->getPowerSide()
            != ((CUser*)m_assaulters[i].GetUser())->getPowerSide())
        {
            enemy = (CUser*)m_assaulters[i].GetUser();
            break;
        }
    }

    if (((CUser*)user)->getPowerSide() == side) {
        myVictory = ((CUser*)user)->GetPowerWarPartyVictory();
        penaltyValue = G_CDataManager()->m_powerParameter
                           .GetPenalty(myVictory).m_fieldC;
        if (enemy != 0) {
            winCount = ((CUser*)enemy)->GetPowerWarPartyVictory();
            if (winCount > 4) {
                breakBonus = G_CDataManager()->m_powerParameter.GetBreakBonus(
                    winCount);
                _SendHoldVictory(user, enemy, winCount);
            }
        }
        _SendVictoryBonus(user, myVictory, penaltyValue, winCount, breakBonus);
    } else if (enemy != 0) {
        myVictory = ((CUser*)enemy)->GetPowerWarPartyVictory();
        penaltyValue = G_CDataManager()->m_powerParameter
                           .GetPenalty(myVictory).m_fieldC;
        winCount = ((CUser*)user)->GetPowerWarPartyVictory();
        if (winCount > 4) {
            breakBonus = G_CDataManager()->m_powerParameter.GetBreakBonus(
                winCount);
            _SendHoldVictory(enemy, user, winCount);
        }
        _SendVictoryBonus(enemy, myVictory, penaltyValue, winCount, breakBonus);
    }
    // b28 BL6: 4.4 右操作数先入 %eax（ORIG 尾块 eax=-0x18=breakBonus/edx=-0x1c=pen），
    // 交换加法两侧复现 ORIG lea 序（数学等价）。
    return penaltyValue + breakBonus;
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e8098 (size 0x209)：_SeparateTeam(CUser*, CUser*)
// 双队长各自队伍列出并布阵。依赖 _AddUser/_MoveUser/_MoveAssaultPlace/
// _SendUserUdpIpPort（B 组交付）。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] AE：users[2] 头部零初始化（ORIG 0x082e80a1 双
//   movl $0 证据）+ party 槽复用 + at 内联实参 + 去三目 ⇒ 167=167 对齐；
//   p167 CLEAR。ORIG 0x082e8098
void CAssaultPlace::_SeparateTeam(CUser* leader1, CUser* leader2)
{
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e80a1/0x082e80a8 函数头两条
    //   `movl $0x0,-0x2c/-0x28` ⇒ users[2] 在函数头声明并零初始化（P0 槽证据）；
    //   末尾 +0x16b/+0x171 才写 leader1/leader2。
    CUser* users[2] = { 0, 0 };
    std::vector<CUser*> lists[2];
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e8123/0x082e8155 两次写同一槽
    //   -0x24（GetUserListByParty 结果复用）⇒ 单个具名局部两次赋值（P0 槽复用）。
    CParty* party = leader1->GetUserListByParty(lists[0]);
    _BackupPartyInfo(party, (ENUM_ASSAULT_TEAM)0);
    party = leader2->GetUserListByParty(lists[1]);
    _BackupPartyInfo(party, (ENUM_ASSAULT_TEAM)1);

    for (unsigned int u = 0; u < 8; ++u) {
        for (int t = 0; t < 2; ++t) {
            if (lists[t].size() > u) {
                // ORIG 0x082e81cc at 结果 (%eax) 直传 _AddUser，无 member 具名槽。
                _AddUser(lists[t].at(u), (ENUM_ASSAULT_TEAM)t);
            }
        }
    }

    users[0] = leader1;
    users[1] = leader2;

    // ORIG 0x082e821a `test %eax,%eax; setg %dl; mov %dl,0x128(%eax)` ⇒ 直接
    // 布尔存字节（三目 ?1:0 多归一化段）。
    m_chaosFlags[0] =
        ((CUserCharacInfo*)leader1)->GetCurCharacChaosPoint() > 0;
    m_chaosFlags[1] =
        ((CUserCharacInfo*)leader2)->GetCurCharacChaosPoint() > 0;

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

    // b28 BL6: P0——ORIG 无 flagX/flagY 槽往返，出参直绑 m_posBase(+0x110/+0x114)。
    disposition.GetFlagPoint(m_posBase.m_x, m_posBase.m_y);   // this+0x110 / +0x114 输出目标
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
// [b28d-62] AE：删 P171 多余空指针守卫 + Empty() 正极性 continue + P134 user
//   免槽内联；71=71，p167 CLEAR。ORIG 0x082e8b66
void CAssaultPlace::_MakeUserInfo(PacketGuard* p, ENUM_USERINFO type)
{
    // [b28d-62] P171：ORIG 0x082e8b6e 无空指针守卫，删多余 `if (p==0) return;`。
    p->put_header(0, 2);
    p->put_byte((int)type);
    int indexSlot = 0;
    int count = 0;
    indexSlot = p->get_index();
    p->put_short(count);
    for (int i = 0; i < 8; ++i) {
        // [b28d-62] ORIG 0x082e8be3 `test %al,%al; jne <循环尾>` ⇒ `if (Empty()) continue;`
        //   （正极性直测）；user 免槽直用（P134，ORIG 无 -0xc 指针槽）。
        if (m_assaulters[i].Empty())
            continue;
        m_assaulters[i].GetUser()->make_basic_info((char*)p, (char)type);
        count = count + 1;
    }
    p->put_short(indexSlot, count);
    p->finalize(true);
}

// ---------------------------------------------------------------------------
// ORIG T 0x82e75ca (size 0xd1)：_AvailableAssault(CUser*, int&)
// 年龄门槛 0xf（PowerWar 中 vill==7 时 0xc）；<0x1e 级未入场检查 → 0xe/0xd；
// 组队分支：party is_limit_minimum_age→0x5f、is_limit_minimum_level→0xff/0xfe。
// ---------------------------------------------------------------------------
// [b28d-solo-W6_AssaultPlaceDepsA] AE：P0 删 bool/uid 具名槽 + `<= 0x1d`（setle）+
//   P59 正极性 throw 顺序 ⇒ 149=149 对齐；p167 CLEAR。ORIG 0x082e75ca
void CAssaultPlace::_AvailableAssault(CUser* user, int& result)
{
    CParty* party = 0;
    if (user->CheckInParty()) {
        party = (CParty*)user->GetParty();
    }

    int needAge = 0xf;
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e75fb 段：IsPowerWarEventOn 与
    //   getCurCharacVill==7 的 `&&` 直接物化 `mov $1/%eax ... mov $0/%eax; test; je`
    //   （P0 无具名 bool 槽；4.4 对具名 bool 会多 movb 物化 + 帧 +0x10）。
    if (GlobalData::s_power_manager->IsPowerWarEventOn()
        && ((CUserCharacInfo*)user)->getCurCharacVill() == 7)
    {
        needAge = 0xc;
    }

    if (!user->CheckLimitMiniumAge(needAge, result)) {
        throw (ENUM_ERROR)0x5f;   // ORIG __cxa_throw ENUM_ERROR(0x5f)
    }

    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082e7689 `cmp $0x1d,%eax; setle`
    //   ⇒ 原码 `<= 0x1d`（写 `< 0x1e` 产 setl 不同形）；uid 无具名槽（P0 内联，
    //   ORIG movzwl %ax,%edx 直存 *result）；throw 分支 ORIG `jne <0xe>` fall-through
    //   0xd ⇒ 正极性 `== user` 写法（P59 反向布局）。
    if (((CUserCharacInfo*)user)->get_charac_level() <= 0x1d) {
        result = (int)user->get_unique_id();
        if (m_checkUser /* +0x134 */ == user) {
            throw (ENUM_ERROR)0xd;
        }
        throw (ENUM_ERROR)0xe;
    }

    if (party != 0) {
        int partyNeedAge = 0xf;
        if (GlobalData::s_power_manager->IsPowerWarEventOn()
            && ((CUserCharacInfo*)user)->getCurCharacVill() == 7)
        {
            partyNeedAge = 0xc;
        }

        if (!party->is_limit_minimum_age(partyNeedAge, result)) {
            throw (ENUM_ERROR)0x5f;
        }
        if (party->is_limit_minimum_level(0x1e, result)) {
            return;
        }
        // ORIG 0x082e77c1 `cmp 0xc(%ebp),%eax; jne <0xff>` ⇒ 相等落 fall-through 0xfe。
        if (m_checkUser == user) {
            throw (ENUM_ERROR)0xfe;
        }
        throw (ENUM_ERROR)0xff;
    }
}
}  // namespace pvp_assault
