// df_game_r Game/ pvp_assault::CAssaultMgr 实现（批3：从 GameStubs.cpp 迁移）。
// 语义依据 ORIG：OnMoveArea 0x82ef610 / SendPacket 0x82efb54 /
// OnLeaveAssaultPlace 0x82ef176 / _GetAssaultPlace 0x82ed9fe。
#include "CAssaultMgr.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CGameManager.h"     // CAssaultPlace 完整类型与方法声明
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"           // s_power_manager / s_event_manager / s_systemTime_
#include "ServerParameterScript.h"  // getAssaultGhostTime（GiveupPanalty）
#include "CPowerManager.h"
#include "CGuildServerProxy.h"    // CEventBase / CEventManager::GetRepeatEvent（_IsGiveupPanalty）
#include "CInventory.h"           // user_creature::CCreatureMgr::TurnStomach（DetachUser）
#include "GameWorld.h"            // G_GameWorld / send_all（SendUserState）

namespace pvp_assault
{

CAssaultMgr* GetInstanceAssaultMgr()
{
    // ORIG _ZN11pvp_assault21GetInstanceAssaultMgrEv：guard 单例。
    static CAssaultMgr s;
    return &s;
}

CAssaultPlace* CAssaultMgr::_GetAssaultPlace(unsigned int id)
{
    // ORIG 0x82ed9fe：未命中抛 ENUM_ERROR(0x15)。
    std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.find(id);
    if (it == m_places.end())
        throw (ENUM_ERROR)0x15;   // ORIG 抛 ENUM_ERROR(0x15)
    return it->second;
}

void CAssaultMgr::OnMoveArea(CUser* user)
{
    // ORIG 0x82ef610：向进入袭击区域的玩家汇总可见图标（包 0x86）。
    PacketGuard packet;
    packet.put_header(0, 0x86);
    int countIdx = packet.get_index();
    int count = 0;
    packet.put_short(count);
    for (std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.begin();
         it != m_places.end(); ++it)
    {
        CAssaultPlace* place = it->second;
        if (place == 0)
            continue;
        int area = user->get_area(false);
        char vill = (char)user->getCurCharacVill();
        if (!place->IsArea((int)vill, area))
            continue;
        bool view;
        if (place->ExistUser(user))
            view = false;
        else if (!place->CheckPlayable())
            view = false;
        else
            view = (place->GetState() == 4);
        if (view)
        {
            place->MakeViewAssaultIconOnePlayer(&packet, true);
            count += 1;
        }
    }
    if (count != 0)
    {
        packet.put_short(countIdx, count);
        packet.finalize(true);
        user->Send(packet);
    }
}

void CAssaultMgr::SendPacket(CUser* user, bool flag, PacketGuard* packet)
{
    // ORIG 0x82efb54：按用户所在袭击区域转发；flag=true 时限定队伍。
    CAssaultPlace* place = _GetAssaultPlace(
        (unsigned int)user->GetAssaultPlace());
    if (flag)
    {
        ENUM_ASSAULT_TEAM team;
        if (place->GetTeam(user, team))
            place->SendPacket(*packet, team);
    }
    else
    {
        place->SendPacket(*packet);
    }
}

void CAssaultMgr::OnLeaveAssaultPlace(CUser* user, bool flag)
{
    // ORIG 0x82ef176：取用户所在区域并执行离开流程。
    CAssaultPlace* place = _GetAssaultPlace(
        (unsigned int)user->GetAssaultPlace());
    place->LeaveAssaultPlace(user, flag);
}

void CAssaultMgr::SetAssaultChannel(bool flag)
{
    // ORIG W _ZN11pvp_assault11CAssaultMgr17SetAssaultChannelEb @0x08376048：
    // movzbl flag → *(unsigned char*)this = flag（写入 +0x00 通道开关）。
    m_bAssaultChannel = flag;
}

// ---- CAssaulter 访问器（ORIG 布局：+0x00 user / +0x04 team / +0x08 state）----

CUser* CAssaulter::GetUser()
{
    // ORIG W 0x82f0542：mov (%eax),%eax
    return (CUser*)m_field00;
}

bool CAssaulter::Empty()
{
    // ORIG W 0x82f055c（= IsAttachAble 0x82f054c）：user==0
    return m_field00 == 0;
}

ENUM_ASSAULT_TEAM CAssaulter::GetTeam()
{
    // ORIG W 0x82f0570
    return (ENUM_ASSAULT_TEAM)m_field04;
}

unsigned short CAssaulter::GetUserUniqueId()
{
    // ORIG T 0x82e6662：user ? user->get_unique_id() : 0
    if (m_field00 == 0)
        return 0;
    return (unsigned short)((CUser*)m_field00)->get_unique_id();
}

void CAssaulter::SetState(ENUM_ASSAULT_STATE state)
{
    // ORIG W 0x82f057c
    m_state08 = (unsigned int)state;
}

ENUM_ASSAULT_STATE CAssaulter::GetState()
{
    // ORIG W 0x82f058a
    return (ENUM_ASSAULT_STATE)m_state08;
}

int GetDirectionByTeam(ENUM_ASSAULT_TEAM team)
{
    // ORIG T 0x82e564c：team 0→6、1→4、其他→0。
    switch (team) {
    case 0: return 6;
    case 1: return 4;
    default: return 0;
    }
}

// ---- CAssaultPlace 内部前置函数 ----

void CAssaultPlace::_GetTeamCount(int* counts)
{
    // ORIG T 0x82e7232：按队伍统计非空槽位。
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty()) {
            int team = (int)m_assaulters[i].GetTeam();
            counts[team] = counts[team] + 1;
        }
    }
}

bool CAssaultPlace::_CheckPlayable()
{
    // ORIG T 0x82e9142：两队人数均非 0 才可开局。
    int counts[2] = { 0, 0 };
    _GetTeamCount(counts);
    for (int i = 0; i < 2; ++i) {
        if (counts[i] == 0)
            return false;
    }
    return true;
}

bool CAssaultPlace::CheckPlayable()
{
    // ORIG W 0x82f06ea：tail-call _CheckPlayable。
    return _CheckPlayable();
}

int CAssaultPlace::GetState()
{
    // ORIG W 0x82f06fe：读 +0x10c。
    return m_state;
}

bool CAssaultPlace::IsArea(int vill, int area)
{
    // ORIG W 0x82f06ae：比较 +0x118/+0x11c（Point2d）。
    return m_pos.m_x == vill && m_pos.m_y == area;
}

int CAssaultPlace::_GetUserIndex(CUser* user, bool strict)
{
    // ORIG T 0x82e849a：找不到时 strict=true 抛 ENUM_ERROR(0x15)，否则返回 -1。
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty() && m_assaulters[i].GetUser() == user)
            return i;
    }
    if (strict)
        throw (ENUM_ERROR)0x15;
    return -1;
}

bool CAssaultPlace::ExistUser(CUser* user)
{
    // ORIG T 0x82e8366
    for (int i = 0; i < 8; ++i) {
        if (!m_assaulters[i].Empty() && m_assaulters[i].GetUser() == user)
            return true;
    }
    return false;
}

bool CAssaultPlace::GetTeam(CUser* user, ENUM_ASSAULT_TEAM& team)
{
    // ORIG T 0x82e6cdc
    for (int i = 0; i < 8; ++i) {
        if (m_assaulters[i].GetUser() == user) {
            team = m_assaulters[i].GetTeam();
            return true;
        }
    }
    return false;
}

bool CAssaultPlace::SendPacket(PacketGuard& packet)
{
    // ORIG T 0x82e6bf6：全员转发；任一 Send 失败则返回 false。
    bool ok = true;
    for (int i = 0; i < 8; ++i) {
        CUser* user = m_assaulters[i].GetUser();
        if (user != 0 && !user->Send(packet))
            ok = false;
    }
    return ok;
}

bool CAssaultPlace::SendPacket(PacketGuard& packet, ENUM_ASSAULT_TEAM team)
{
    // ORIG T 0x82e6c56：仅指定队伍转发。
    bool ok = true;
    for (int i = 0; i < 8; ++i) {
        CUser* user = m_assaulters[i].GetUser();
        if (user != 0 && m_assaulters[i].GetTeam() == team
            && !user->Send(packet))
            ok = false;
    }
    return ok;
}

void CAssaultPlace::MakeViewAssaultIconOnePlayer(PacketGuard* packet, bool flag)
{
    // ORIG T 0x82e89c0：put_int(+0x104)、put_byte(flag)；flag 时写位置与成员图标列表。
    packet->put_int(m_field104);
    packet->put_byte(flag ? 1 : 0);
    if (!flag)
        return;
    packet->put_short(m_view110);
    packet->put_short(m_view114);
    int countIdx = packet->get_index();
    int count = 0;
    packet->put_short(count);
    for (int i = 0; i < 8; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        packet->put_short((int)m_assaulters[i].GetUserUniqueId());
        packet->put_byte(GetDirectionByTeam(m_assaulters[i].GetTeam()));
        count += 1;
    }
    packet->put_short(countIdx, count);
}

void CAssaultPlace::LeaveAssaultPlace(CUser* user, bool flag)
{
    // ORIG T 0x82eb450：idx=_GetUserIndex(user,true) → LeaveAssaultPlace(idx,flag,false)。
    int index = _GetUserIndex(user, true);
    LeaveAssaultPlace(index, flag, false);
}

// ============================================================================
// CAssaulter 方法（ORIG 权威；LeaveAssaultPlace 链依赖）
// ============================================================================

// ORIG T 0x82e6688：return *(int*)(this+0x10)。
int CAssaulter::GetHp()
{
    return m_hp10;
}

// ORIG T 0x82e6812：卸下 creature（TurnStomach false）+ 清 user/team 槽位。
void CAssaulter::DetachUser()
{
    CUser* user = (CUser*)m_field00;
    if (user != 0)
    {
        CInventory* inven = user->getCurCharacInvenW();
        if (inven != 0)
        {
            user_creature::CCreatureMgr* mgr = inven->GetCreatureMgrW();
            mgr->TurnStomach(false);
        }
    }
    m_field00 = 0;
    m_field04 = 0;
}

// ORIG T 0x82e6868：放弃惩罚 —— 备份耐力 → 记 GiveupPvP 日志 → 设 ghost 惩罚时间。
void CAssaulter::GiveupPanalty()
{
    CUser* user = (CUser*)m_field00;
    if (user == 0)
        return;
    if (user->getCurCharacR() != 0)
    {
        unsigned char stamina = user->getCurCharacStamina();
        user->setCurCharacBackupStamina(stamina);
        bool powerWar = GlobalData::s_power_manager->IsPowerWarEventOn();
        bool isPowerWarVill = powerWar || (user->getCurCharacVill() == 7);
        user->m_historyLog.GiveupPvP(isPowerWarVill ? (ENUM_PVP_HISTORY_REASON)7
                                                    : (ENUM_PVP_HISTORY_REASON)3);
        int ghostTime;
        if (powerWar && user->getCurCharacVill() == 7)
        {
            float penalty = GlobalData::s_power_manager->GetPowerWarResponPenalty(user);
            ghostTime = GlobalData::s_power_manager->GetPowerWarGhostTime(user, penalty);
        }
        else
        {
            int chaos = user->GetCurCharacChaosPoint();
            int level = user->get_charac_level();
            ServerParameterScript* sps = (ServerParameterScript*)((char*)G_CDataManager() + 0x68);
            ghostTime = sps->getAssaultGhostTime(level, chaos);
        }
        if (ghostTime != 0)
        {
            user->SetCurCharacGhost(true);
            int now = GlobalData::s_systemTime_.getCurSec();
            user->SetCurCharacChaosDieTime(now);
            user->SetCurCharacChaosResponTime(now + ghostTime);
            user->m_characterView.enableSaveCharacView();
        }
    }
}

// ============================================================================
// CAssaultPlace LeaveAssaultPlace 链私有 helper（ORIG 权威；自包含部分已还原，
// 其余依赖未还原子系统处标注，不造桩）。
// ============================================================================

// ORIG T 0x82eacf4：GetRepeatEvent(0x20) 后经 vtable+0x34 虚调用判活——该槽
// 即 IsEventing(CUser*)，与 IsPowerWarEventOn 0x0847f910 的 GetRepeatEvent→
// vtable+0x34→call(0) 形态逐指令一致；活动进行中直接返回 false（不惩罚）。
// 否则 m_state（+0x10c）∈[2,4] 且 !flag 时返回 true。ORIG 对事件指针无空检，
// 保持原样。
bool CAssaultPlace::_IsGiveupPanalty(bool flag)
{
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x20);
    if (event->IsEventing((CUser*)0))
        return false;
    return m_state >= 2 && m_state <= 4 && !flag;
}

// ORIG T 0x82eab1a：_IsBroadcastRunaway —— PowerWar 且村庄==7 时 true。
bool CAssaultPlace::_IsBroadcastRunaway(CUser* user)
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn())
    {
        if (user->getCurCharacVill() == 7)
            return true;
    }
    return false;
}

// ORIG T 0x82eab7a：_BroadcastRunaway(CUser*, int) —— 向玩家广播逃跑消息（包 0x85）。
void CAssaultPlace::_BroadcastRunaway(CUser* user, int index)
{
    // ORIG：构造 packet（header 0x85）+ unique_id + 逃跑索引，发给 user。
    PacketGuard packet;
    packet.put_header(0, 0x85);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_byte(index);
    packet.finalize(true);
    user->Send(packet);
}

// ORIG T 0x82e9046：_MakeLeaveUser —— 包 0x87 + 玩家 unique_id。
void CAssaultPlace::_MakeLeaveUser(PacketGuard* p, int index)
{
    p->put_header(0, 0x87);
    p->put_short((int)m_assaulters[index].GetUserUniqueId());
    p->finalize(true);
}

// ORIG T 0x82e88d6：_MakeCloseAssault —— 包 0x89 + tick。
void CAssaultPlace::_MakeCloseAssault(PacketGuard* p, int tick)
{
    p->put_header(0, 0x89);
    p->put_byte(tick);
    p->finalize(true);
}

// ORIG T 0x82e8d60：_MakeUserState(PacketGuard*, CUser*) —— 包 0x03 + state。
void CAssaultPlace::_MakeUserState(PacketGuard* p, CUser* user)
{
    p->put_header(0, 3);
    p->put_byte(1);
    p->put_short(user->get_unique_id() & 0xffff);
    p->put_byte(user->GetUserState());
    p->finalize(true);
}

// ORIG T 0x82e83c4：SendUserState(CUser*) —— 组 0x03 状态包并全服广播
// （报告 pvp_assault__CAssaultPlace/SendUserState.md；ORIG 调用点仅压 CUser*，
//   头文件声明为静态形态）。
void CAssaultPlace::SendUserState(CUser* user)
{
    PacketGuard packet;
    packet.put_header(0, 3);
    packet.put_byte(1);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_byte(user->GetUserState());
    packet.finalize(true);
    GameWorld* world = G_GameWorld();
    world->send_all(packet, user);
}

// ORIG T 0x82e7b2a：_SendCloseAssault(CUser*, int) —— tick 下限 0x1e，发关闭包。
int CAssaultPlace::_SendCloseAssault(CUser* user, int tick)
{
    if (user->getCurCharacR() == 0)
        return 0;
    if (tick < 0x1e)
        tick = 0x1e;
    PacketGuard packet;
    _MakeCloseAssault(&packet, tick);
    user->Send(packet);
    return 1;
}

// ============================================================================
// LeaveAssaultPlace(int,bool,bool) —— ORIG 0x82ead6a。
// 保留批3已验证的可链接骨架（校验索引并释放槽位）；已还原的自包含 helper
// （_MakeLeaveUser/_SendCloseAssault/_IsBroadcastRunaway/_BroadcastRunaway）供
// 后续在 _IsGiveupPanalty 事件判定、_DelUser/CloseAssault/EndAssault、
// CPowerWarLog 等未还原子系统落地后接入完整链。不造桩。
// ============================================================================
void CAssaultPlace::LeaveAssaultPlace(int index, bool delFlag, bool penaltyFlag)
{
    if (index < 0 || index >= 8)
        return;
    CAssaulter& assaulter = m_assaulters[index];
    if (assaulter.GetUser() == 0)
        return;
    assaulter.m_field00 = 0;   // 释放槽位（批3骨架；完整链见 _IsGiveupPanalty 依赖）
}

}  // namespace pvp_assault
