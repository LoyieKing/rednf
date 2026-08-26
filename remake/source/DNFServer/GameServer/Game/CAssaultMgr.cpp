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
#include "TimerDispatcher.h"   // TimerCompleteLoadAfterAssaultTimeout
#include "Village.h"
#include "Area.h"
#include "CVillageObjectMgr.h"    // on_move_area（_DelUser）
#include "CVillageMonsterMgr.h"   // OnMoveArea（_DelUser）
#include "CDisconnectDetecter.h"  // disconnect_detecter::UnregisterUser（_DelUser）
#include "ServerParameterScript.h"  // getAssaultGhostTime（GiveupPanalty）
#include "CPowerManager.h"
#include "CGuildServerProxy.h"    // CEventBase / CEventManager::GetRepeatEvent（_IsGiveupPanalty）
#include "CInventory.h"           // user_creature::CCreatureMgr::TurnStomach（DetachUser）
#include "GameWorld.h"            // G_GameWorld / send_all（SendUserState）

// 全局依赖前置（3 参版定义见本文件尾；6 参版 owner=CVillageMonsterMgr.cpp）
bool IsIncludeVirtualMoveAreas(std::vector<MapArea>* areas, int x, int y);

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
    return m_user;
}

bool CAssaulter::Empty()
{
    // ORIG W 0x82f055c（= IsAttachAble 0x82f054c）：user==0
    return m_user == 0;
}

ENUM_ASSAULT_TEAM CAssaulter::GetTeam()
{
    // ORIG W 0x82f0570
    return (ENUM_ASSAULT_TEAM)m_team;
}

unsigned short CAssaulter::GetUserUniqueId()
{
    // ORIG T 0x82e6662：user ? user->get_unique_id() : 0
    if (m_user == 0)
        return 0;
    return (unsigned short)(m_user)->get_unique_id();
}

void CAssaulter::SetState(ENUM_ASSAULT_STATE state)
{
    // ORIG W 0x82f057c
    m_state = (unsigned int)state;
}

ENUM_ASSAULT_STATE CAssaulter::GetState()
{
    // ORIG W 0x82f058a
    return (ENUM_ASSAULT_STATE)m_state;
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
    packet->put_short((int)m_posBase.m_x);
    packet->put_short((int)m_posBase.m_y);
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
    return m_hp;
}

// ORIG T 0x82e6812：卸下 creature（TurnStomach false）+ 清 user/team 槽位。
void CAssaulter::DetachUser()
{
    CUser* user = m_user;
    if (user != 0)
    {
        CInventory* inven = user->getCurCharacInvenW();
        if (inven != 0)
        {
            user_creature::CCreatureMgr* mgr = inven->GetCreatureMgrW();
            mgr->TurnStomach(false);
        }
    }
    m_user = 0;
    m_team = 0;
}

// ORIG T 0x82e6868：放弃惩罚 —— 备份耐力 → 记 GiveupPvP 日志 → 设 ghost 惩罚时间。
void CAssaulter::GiveupPanalty()
{
    CUser* user = m_user;
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
    assaulter.m_user = 0;   // 释放槽位（批3骨架；完整链见 _IsGiveupPanalty 依赖）
}

// ============================================================
// 批 A（OnlyOrig_Assault 恢复落盘；跳过 GetDistance(Point2d,Point2d) 与
// 6 参 IsIncludeVirtualMoveAreas——唯一 owner 归 CVillageMonsterMgr.cpp）
// ============================================================

const Point2d ASSAULT_RECT_PLAYER_POINT[8] = {
    Point2d(0x000, 0x0f), Point2d(0x038, 0x05), Point2d(0x054, 0x00),
    Point2d(0x01c, 0x0a), Point2d(0x104, 0x0f), Point2d(0x0cc, 0x05),
    Point2d(0x0b0, 0x00), Point2d(0x0e8, 0x0a)
};                                   // .rodata 0x8c1cce0 实测内容
const Point2d ASSAULT_RECT_ICON_POINT = { 0x82, 5 };   // .rodata 0x8c1cd20

int GetMaxCandidateMatirxRow()          // ORIG T _Z24GetMaxCandidateMatirxRowv 0x82e5c9e
{
    return 15;
}

Point2d::~Point2d() {}
int Point2d::operator-(const Point2d& o) const
{
    // ORIG 0x82f0486：|dx| + |dy|
    int dx = m_x - o.m_x;
    int dy = m_y - o.m_y;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    return dx + dy;
}

void PartyInfo::Set(CParty* party)                 // ORIG 0x82f05fc
{
    m_field00 = (unsigned int)party;
}

CandidatePoint::CandidatePoint() : m_point() {}    // +0 故意不初始化（ORIG 同）
CandidatePoint::~CandidatePoint() {}

CExtraMatrix::CExtraMatrix() {}
CExtraMatrix::~CExtraMatrix() {}

void CExtraMatrix::AddPoint(Point2d p)             // ORIG 0x82e5962
{
    m_points.push_back(p);
}

void CExtraMatrix::MovePoint(const Point2d& delta) // ORIG 0x82e597c
{
    for (std::vector<Point2d>::iterator it = m_points.begin();
         it != m_points.end(); ++it)
    {
        *it += delta;
    }
}

bool CExtraMatrix::CheckPoint(std::vector<MapArea>* areas,
                              Point2d& out)        // ORIG 0x82e5b4e
{
    for (std::vector<Point2d>::iterator it = m_points.begin();
         it != m_points.end(); ++it)
    {
        if (IsIncludeVirtualMoveAreas(areas, it->m_x, it->m_y,
                                      out.m_x, out.m_y, 0))
        {
            return true;
        }
    }
    return false;
}

bool CExtraMatrix::MoveUser(CForcedMoveCharacter* fm)   // ORIG 0x82e5c0a
{
    int idx = fm->m_slotIdx;
    if (idx < 0 || (unsigned int)idx >= m_points.size())
        return false;
    fm->m_x = m_points.at(idx).m_x;
    fm->m_y = m_points.at(idx).m_y;
    fm->ForcedMove();
    return true;
}

CCandidateMatrix::CCandidateMatrix() {}
CCandidateMatrix::~CCandidateMatrix() {}

unsigned int CCandidateMatrix::At(unsigned int idx, int& x, int& y)  // ORIG 0x82e590c（恒 false）
{
    if (m_points.size() > idx)
    {
        x = m_points[idx].m_point.m_x;
        y = m_points[idx].m_point.m_y;
    }
    return 0;
}

void CCandidateMatrix::_AddCandidatePoint(const Point2d& from1,
                                          const Point2d& from2,
                                          const Point2d& relTo,
                                          int weight)              // ORIG 0x82e5894
{
    CandidatePoint cp;
    cp.m_point = from1;                        // point ← from1
    cp.m_priority = weight;
    cp.m_priority += from1 - relTo;            // 曼哈顿 op-
    cp.m_priority -= from2 - from1;
    m_points.push_back(cp);
}

void CCandidateMatrix::MakeCandidatePoint(const Point2d& a, const Point2d& b) // ORIG 0x82e5ca8
{
    _AddCandidatePoint(a, a, b, 10);
    for (int i = 1; i <= 7; ++i)
    {
        _AddCandidatePoint(Point2d(a.m_x, a.m_y + i * 10), a, b, 10);
        _AddCandidatePoint(Point2d(a.m_x, a.m_y - i * 10), a, b, 10);
    }
    for (int j = 0; GetMaxCandidateMatirxRow() > j; ++j)
    {
        for (int i = 1; i <= 7; ++i)
        {
            _AddCandidatePoint(Point2d(m_points[j].m_point.m_x + i * 10,
                                       m_points[j].m_point.m_y), a, b, 0);
            _AddCandidatePoint(Point2d(m_points[j].m_point.m_x - i * 10,
                                       m_points[j].m_point.m_y), a, b, 0);
        }
    }
}

void CCandidateMatrix::RemoveCandidatePoint(std::vector<MapArea>* areas) // ORIG 0x82e5f36
{
    std::vector<CandidatePoint>::iterator it = m_points.begin();
    while (it != m_points.end())
    {
        if (!::IsIncludeVirtualMoveAreas(areas, it->m_point.m_x,
                                       it->m_point.m_y))
        {
            it = m_points.erase(it);
            continue;
        }
        ++it;
    }
}

void CCandidateMatrix::ArrangeCandidatePoint()          // ORIG 0x82e5fee
{
    std::sort(m_points.begin(), m_points.end());
}

// ============================================================
// 全局依赖（本 TU 唯一 owner 部分）
// ============================================================

} // namespace pvp_assault

// ORIG T _ZN7MapArea7includeEii 0x82f0298：矩形包含判定。
int MapArea::include(int x, int y)
{
    if (m_x > x) return 0;
    if (m_x + m_w < x) return 0;
    if (m_y > y) return 0;
    if (m_y + m_h < y) return 0;
    return 1;
}

// ORIG T _Z25IsIncludeVirtualMoveAreasPSt6vectorI7MapAreaSaIS0_EEii
// 0x82e5ed1：全部区域均包含 (x,y) 才返回 true。
bool IsIncludeVirtualMoveAreas(std::vector<MapArea>* areas, int x, int y)
{
    for (unsigned int i = 0; i < areas->size(); ++i)
    {
        if (!areas->at(i).include(x, y))
            return false;
    }
    return true;
}

namespace pvp_assault
{

// ============================================================
// 批 B：CAssaulter（ORIG 逐指令转写）
void CAssaulter::AttachUser(CUser* user, ENUM_ASSAULT_TEAM team)   // ORIG T 0x82e67e6
{
    m_user = user;
    m_team = (unsigned int)team;
    _PrepareAssault(user);
}

bool CAssaulter::IsAttachAble()        // ORIG W 0x82f054c
{
    return m_user == 0;
}

void CAssaulter::_PrepareAssault(CUser* user)     // ORIG T 0x82e6780
{
    if (user->getCurCharacR() == 0)
        return;
    m_hp = (unsigned char)user->getCurCharacStamina();
    user->SetCurCharacLeaveTimeout(false);
    CInventory* inven = user->getCurCharacInvenW();
    if (inven != 0)
    {
        user_creature::CCreatureMgr* mgr = inven->GetCreatureMgrW();
        if (mgr != 0)
            mgr->TurnStomach(true);
    }
}

bool CAssaulter::IsPowerWarWinner()     // ORIG W 0x82f05ae
{
    return m_powerWarWinner != 0;
}

void CAssaulter::SetPowerWarWinner(bool flag)   // ORIG W 0x82f0596
{
    m_powerWarWinner = (char)(flag ? 1 : 0);
}

void CAssaulter::SetUserConnectP2P(char* ip, int len)   // ORIG T 0x82e6a82
{
    int n = len;
    if (n > 8)
        n = 8;                       // ORIG std::min<int>(len, 8)
    memcpy(m_connectIp, ip, n);
}

void CAssaulter::Reset()               // ORIG W 0x82f05ba
{
    m_user = 0;
    m_team = 0;
    m_state = 0;
    m_lastHpSec = 0;
    m_powerWarWinner = 0;
}

void CAssaulter::DieUser()             // ORIG T 0x82e6a40
{
    if (m_user == 0)
        return;
    m_user->DieCurCharac(0, m_user);
    ((CCharacterView*)((char*)m_user + 0x796f4))->enableSaveCharacView();
}

bool CAssaulter::ChangeHp(int hp)      // ORIG T 0x82e6694
{
    if (hp < 0 || hp > 100)
    {
        cMyTrace trace("bool pvp_assault::CAssaulter::ChangeHp(int)", 0x18a, 0);
        trace("ASSAULT_CHANGE_HP error (%d)", hp);
        return false;
    }
    bool isPowerWar =
        GlobalData::s_power_manager->IsPowerWarEventOn() &&
        m_user->getCurCharacVill() == 7;
    if (isPowerWar)
        m_user->setPowerWarHP((unsigned short)hp);
    else
        m_hp = hp;

    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    int diff = (int)(curSec - m_lastHpSec);
    m_lastHpSec = curSec;
    return diff > 2;
}

const char* CAssaulter::GetCharacName() const    // ORIG T 0x82e663c
{
    if (m_user == 0)
        return "";
    return m_user->getCurCharacName();
}


// ============================================================
// 批 A 续：Disposition / Factory / CAssaultPlace 访问器（恢复落盘）
// ============================================================

void CStaticDisposition::_MoveMatrix(const Point2d& delta)   // ORIG 0x82e6498
{
    for (int j = 0; j <= 1; ++j)
        m_mats[j].MovePoint(delta);
    m_iconPos += delta;
}

void CStaticDisposition::GetFlagPoint(int& x, int& y)        // ORIG 0x82f0526
{
    x = m_iconPos.m_x;
    y = m_iconPos.m_y;
}

bool CStaticDisposition::_CheckMatrix(std::vector<MapArea>* areas,
                                      Point2d& out)          // ORIG 0x82e64f2
{
    for (int j = 0; j <= 1; ++j)
    {
        if (!m_mats[j].CheckPoint(areas, out))
            return false;
    }
    return IsIncludeVirtualMoveAreas(areas, m_iconPos.m_x, m_iconPos.m_y,
                                     out.m_x, out.m_y, 0);
}

void CStaticDisposition::SetVirtualMap(std::vector<MapArea>* areas) // ORIG 0x82e6588
{
    Point2d probe;
    for (int i = 0; i <= 4; ++i)
    {
        if (_CheckMatrix(areas, probe))
            return;
        _MoveMatrix(probe);
    }
}

bool CStaticDisposition::MoveUser(CForcedMoveCharacter* fm)  // ORIG 0x82e65ee
{
    return m_mats[fm->m_field00].MoveUser(fm);
}

CStaticDisposition::CStaticDisposition(CUser** users, int* counts) // ORIG 0x82e62aa
{
    m_iconPos = ASSAULT_RECT_ICON_POINT;
    for (int j = 0; j <= 1; ++j)
    {
        int cnt = counts[j * 4];      // ORIG counts[j<<2] 槽位布局
        for (int k = 0; k <= 3 && cnt > k; ++k)
            m_mats[j].AddPoint(ASSAULT_RECT_PLAYER_POINT[j * 4 + k]);
    }
    Point2d origin(users[0]->get_posX(), users[0]->get_posY());
    _MoveMatrix(origin);
}

CStaticDisposition::~CStaticDisposition() {}                 // ORIG 0x82f10b0

void CDynamicDisposition::GetFlagPoint(int& x, int& y)       // ORIG 0x82f050a
{
    x = m_anchor.m_x;
    y = m_anchor.m_y;
}

void CDynamicDisposition::SetVirtualMap(std::vector<MapArea>* areas) // ORIG 0x82e624c
{
    for (int i = 0; i < 2; ++i)
        m_matrices[i].RemoveCandidatePoint(areas);
    for (int i = 0; i < 2; ++i)
        m_matrices[i].ArrangeCandidatePoint();
}

CDynamicDisposition::CDynamicDisposition(CUser** users, int* counts) // ORIG 0x82e6032
{
    m_users[0] = users[0];
    m_users[1] = users[1];
    m_anchor.m_x = (unsigned short)m_users[0]->get_posX()
                 + ((unsigned short)m_users[1]->get_posX() >> 1);
    m_anchor.m_y = (unsigned short)m_users[0]->get_posY()
                 + ((unsigned short)m_users[1]->get_posY() >> 1);
    Point2d p0(m_users[0]->get_posX(), m_users[0]->get_posY());
    Point2d p1(m_users[1]->get_posX(), m_users[1]->get_posY());
    m_matrices[0].MakeCandidatePoint(p0, p1);
    m_matrices[1].MakeCandidatePoint(p0, p1);
}

CAssaultPlaceFactory::CAssaultPlaceFactory() {}   // ORIG C1 0x82f0f96
CAssaultPlaceFactory::~CAssaultPlaceFactory() {}  // ORIG D1 0x82f0f82

CAssaultPlace* CAssaultPlaceFactory::AcquireAssaultPlace()   // ORIG 0x82f0f54
{
    return m_pool.Acquire();
}

void CAssaultPlaceFactory::FreeAssaultPlace(CAssaultPlace* place)  // ORIG 0x82f0f68
{
    m_pool.Free(place);
}

CAssaultPlaceFactory* GetInstanceAssaultPlaceFactory()       // ORIG 0x82f0fa9
{
    static CAssaultPlaceFactory instance;                    // guard 0x943bcc0/0x943bce0
    return &instance;
}

int CAssaultPlace::GetAuthenValue()             // ORIG T 0x82f06a0
{
    return m_authenValue;
}

int CAssaultPlace::GetAssaultPlaceId()          // ORIG T 0x82f0680
{
    return m_field104;
}

void CAssaultPlace::SetAssaultPlaceId(unsigned int id)   // ORIG T 0x82f068e
{
    m_field104 = (int)id;
}

int CAssaultPlace::GetAssaulterCount()          // ORIG W 0x82f06dc
{
    return m_field100;
}

const char* CAssaultPlace::GetAnotherTeamCharacName(int idx)   // ORIG T 0x82eb3ba
{
    if (m_assaulters[idx].Empty())
        return "";
    ENUM_ASSAULT_TEAM myTeam = m_assaulters[idx].GetTeam();
    for (int j = 0; j <= 7; ++j)
    {
        if (m_assaulters[j].GetTeam() == myTeam)
            continue;
        CUser* u = m_assaulters[j].GetUser();
        if (u != 0)
            return u->getCurCharacName();
    }
    return "";
}

void CAssaultPlace::AbuseUser(CUser* user)      // ORIG T 0x82eb5e2（size 0x192）
{
    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();
    int diff = (int)(curSec - (unsigned int)m_abuseSec);
    if (diff <= 20)
    {
        int characNo = user->getCurCharacNo();
        cMyTrace trace("void pvp_assault::CAssaultPlace::AbuseUser(CUser*)",
                       0xc23, 0);
        trace("CAssaultPlace::AbuseUser() charac_no(%d), timeout sec(%d)",
              characNo, diff);
    }
    if (m_state == 4)
    {
        for (int j = 0; j <= 7; ++j)
        {
            if (m_assaulters[j].Empty())
                continue;
            CUser* slotUser = m_assaulters[j].GetUser();
            WongWork::CHackAnalyzer* ha = slotUser->getHackAnalyzer();
            ha->addServerHackCnt(slotUser,
                                 (WongWork::ENUM_HACKTYPE)0x32e, 1, 0, 0);
        }
        m_state = 6;
    }
    if (m_parties[0].m_field00 != 0)
        ((CParty*)m_parties[0].m_field00)->SetAssaultState(6);
    if (m_parties[1].m_field00 != 0)
        ((CParty*)m_parties[1].m_field00)->SetAssaultState(6);
    TimeoutCloseAssault();
}

// ============================================================================
// CAssaultPlace 构造 / 析构（ORIG W C1/C2 0x82f0614 size 0x6c、D1/D2 0x82f13b6
// size 0x31）。ORIG ctor：CAssaulter[8] 循环默认构造 → Point2d::C1 于 this+0x110
// （m_posBase）→ PartyInfo[2] 循环构造；其余字段留待 Reset()/Acquire 初始化。
// ORIG dtor：仅反向析构 assaulters[8]（Point2d/PartyInfo 析构平凡）。
// ============================================================================
CAssaultPlace::CAssaultPlace()
{
}

CAssaultPlace::~CAssaultPlace()
{
}

// ORIG T 0x82e6d3a：m_pos.x/y 非零才向所在区域广播。
void CAssaultPlace::_SendPacketToArea(PacketGuard& packet)
{
    if (m_pos.m_x == 0 && m_pos.m_y == 0)
        return;
    G_GameWorld()->send_to_area(m_pos.m_x, m_pos.m_y, packet);
}

// ORIG T 0x82e8afc：包 0x86 图标列表 = put_header(0,0x86) + put_short(1) +
// MakeViewAssaultIconOnePlayer + finalize(true)。
void CAssaultPlace::_MakeViewAssaultIconPlayer(PacketGuard* packet, bool flag)
{
    packet->put_header(0, 0x86);
    packet->put_short(1);
    MakeViewAssaultIconOnePlayer(packet, flag);
    packet->finalize(true);
}

// ORIG T 0x82e79f6：_MakeUserState 后 G_GameWorld 全服广播。
void CAssaultPlace::_SendUserState()
{
    PacketGuard packet;
    _MakeUserState(&packet);
    G_GameWorld()->send_all(packet);
}

// ORIG T 0x82e8c5a：包 0x03 全场用户状态；先记录写位置，尾部回填人数。
void CAssaultPlace::_MakeUserState(PacketGuard* packet)
{
    packet->put_header(0, 3);
    int index = packet->get_index();
    int count = 0;
    packet->put_byte(count);
    for (int i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        CUser* slotUser = m_assaulters[i].GetUser();
        packet->put_short((int)(slotUser->get_unique_id() & 0xffff));
        packet->put_byte(slotUser->GetUserState());
        ++count;
    }
    packet->put_byte(index, count);
    packet->finalize(true);
}

// ORIG T 0x82e6e2c：移除指定槽位用户并联动村庄/断线检测子系统。
void CAssaultPlace::_DelUser(int index, bool flag)
{
    if (flag && m_assaulters[index].GetState() > (ENUM_ASSAULT_STATE)4)
    {
        CUser* delUser = m_assaulters[index].GetUser();
        disconnect_detecter::GetInstanceDisconnectDetecter()->UnregisterUser(delUser);
    }
    int team = m_assaulters[index].GetTeam();
    CUser* delUser = m_assaulters[index].GetUser();
    m_assaulters[index].DetachUser();
    m_field100 -= 1;
    if (delUser == 0)
        return;
    if (delUser->getCurCharacR() == 0)
        return;

    bool powerWarVill =
        GlobalData::s_power_manager->IsPowerWarEventOn()
        && delUser->getCurCharacVill() == 7;
    if (!powerWarVill)
    {
        int now = GlobalData::s_systemTime_.getCurSec();
        delUser->m_historyLog.EndPvP(
            (ENUM_PVP_HISTORY_REASON)5,
            m_teamFlag[team],
            now - m_abuseSec);
    }

    GameWorld* world = G_GameWorld();
    Village* village = world->getUserVillage(delUser);
    if (village != 0)
    {
        int areaIdx = delUser->get_area(false);
        Area* area = village->getArea(areaIdx);
        if (area != 0)
        {
            area->send_area_users(delUser);
            GetInstanceAssaultMgr()->OnMoveArea(delUser);
        }
    }
    GlobalData::s_villageObjectMgr->on_move_area(delUser);
    GlobalData::s_villageMonsterMgr->OnMoveArea(delUser);
    delUser->SetAssaultPlace(0);
}

// ORIG T 0x82ea188 size 0x191：超时关场。trace 打印全场角色名；
// state 5/6 时两队下发 SetAssaultState(7)、置 state=7 并 CloseAssault()。
void CAssaultPlace::TimeoutCloseAssault()
{
    cMyTrace trace("void pvp_assault::CAssaultPlace::TimeoutCloseAssault()",
                   0x9ea, 0);
    trace("TimeoutCloseAssault() state(%d) %s,%s,%s,%s,%s,%s,%s,%s",
          m_state,
          m_assaulters[0].GetCharacName(), m_assaulters[1].GetCharacName(),
          m_assaulters[2].GetCharacName(), m_assaulters[3].GetCharacName(),
          m_assaulters[4].GetCharacName(), m_assaulters[5].GetCharacName(),
          m_assaulters[6].GetCharacName(), m_assaulters[7].GetCharacName());

    if (m_state != 5 && m_state != 6)
        return;
    if (m_parties[0].m_field00 != 0)
        ((CParty*)m_parties[0].m_field00)->SetAssaultState(7);
    if (m_parties[1].m_field00 != 0)
        ((CParty*)m_parties[1].m_field00)->SetAssaultState(7);
    m_state = 7;
    CloseAssault();
}

// ORIG T 0x82e6ab8（报告 Reset.md）：全场复位；归还池前调用。
void CAssaultPlace::Reset()
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].GetUser() != 0)
            m_assaulters[i].GetUser()->SetAssaultPlace(0);
        m_assaulters[i].Reset();
    }
    m_field100 = 0;
    m_field104 = 0;
    m_authenValue = 0;
    m_state = 0;
    m_posBase.m_x = 0;
    m_posBase.m_y = 0;
    m_pos.m_x = 0;
    m_pos.m_y = 0;
    m_teamFlag[0] = 0;
    m_teamFlag[1] = 0;
    m_winTeam = 0;
    m_pad134[0] = 0;
    m_abuseSec = 0;
}

// ORIG T 0x82edb6a：map 摘除 + place->Reset() + 归还 StaticPool。
void CAssaultMgr::FreeAssaultPlace(int id, CAssaultPlace* place)
{
    if (id == 0)
        return;
    std::map<unsigned int, CAssaultPlace*>::iterator it = m_places.find(id);
    if (it != m_places.end())
        m_places.erase(it);
    if (place == 0)
        return;
    place->Reset();
    GetInstanceAssaultPlaceFactory()->FreeAssaultPlace(place);
}

// ORIG T 0x82e9ad6 size 0x52d：关场主流程 —— 全员按 PowerWar/普通分支
// 发关闭包并回补体力，广播状态与图标， ghost 记录，定时两队重载，
// 清场后归还池。
void CAssaultPlace::CloseAssault()
{
    int lastVill = 0;
    if (m_field100 <= 0)
        return;

    for (int i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        CUser* slotUser = m_assaulters[i].GetUser();
        if (slotUser == 0)
            continue;

        bool powerWarVill =
            GlobalData::s_power_manager->IsPowerWarEventOn()
            && slotUser->getCurCharacVill() == 7;
        if (powerWarVill)
        {
            if (!m_assaulters[i].IsPowerWarWinner())
                ForcedMoveAtPowerWarPlace(slotUser);
            _SendCloseAssault(slotUser,
                              (int)(slotUser->getPowerWarHP() & 0xffff));
        }
        else
        {
            if (slotUser->getCurCharacR() != 0)
            {
                if (slotUser->IsCurCharacGhost() == 0)
                {
                    unsigned char stamina =
                        (unsigned char)_SendCloseAssault(
                            slotUser, m_assaulters[i].GetHp());
                    slotUser->setCurCharacStamina(stamina);
                }
                else
                {
                    unsigned char stamina =
                        (unsigned char)_SendCloseAssault(slotUser, 100);
                    slotUser->setCurCharacStamina(stamina);
                }
            }
        }
        lastVill = (int)slotUser->getCurCharacVill();

        slotUser->SetAssaultPlace(0);
        if (m_assaulters[i].IsPowerWarWinner())
        {
            if (slotUser->getPowerWarHP() < 0x1e)
                slotUser->setPowerWarHP(0x1e);
        }

        bool gainChaosKill =
            m_assaulters[i].GetTeam() == m_winTeam
            && !GlobalData::s_power_manager->IsPowerWarEventOn()
            && slotUser->getCurCharacVill() != 7;
        if (gainChaosKill)
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            slotUser->SetCurCharacChaosKillTime(now);
        }

        int ghostTime = 0;
        bool powerWar = GlobalData::s_power_manager->IsPowerWarEventOn()
                        || slotUser->getCurCharacVill() == 7;
        if (powerWar)
        {
            float penalty =
                GlobalData::s_power_manager->GetPowerWarResponPenalty(slotUser);
            ghostTime =
                GlobalData::s_power_manager->GetPowerWarGhostTime(slotUser,
                                                                 penalty);
        }
        else
        {
            int chaos = slotUser->GetCurCharacChaosPoint();
            int level = slotUser->get_charac_level();
            ServerParameterScript* sps =
                (ServerParameterScript*)((char*)G_CDataManager() + 0x68);
            ghostTime = sps->getAssaultGhostTime(level, chaos);
        }

        if (slotUser->IsCurCharacGhost() != 0 && ghostTime != 0)
        {
            int now = GlobalData::s_systemTime_.getCurSec();
            slotUser->SetCurCharacChaosDieTime(now);
            slotUser->SetCurCharacChaosResponTime(now + ghostTime);
            slotUser->m_characterView.enableSaveCharacView();
            CAssaultMgr::NotifyGhostTime(slotUser, ghostTime, ghostTime);
        }
    }
    _SendUserState();

    PacketGuard packet;
    _MakeViewAssaultIconPlayer(&packet, false);
    _SendPacketToArea(packet);

    bool reloadParty =
        GlobalData::s_power_manager->IsPowerWarEventOn() && lastVill == 7;
    if (reloadParty)
    {
        if (m_parties[0].Get() != 0)
        {
            CParty* party = (CParty*)m_parties[0].Get();
            TimerCompleteLoadAfterAssaultTimeout::registNextTimer(
                0x3c, party->GetPartyIndex(), party->gen_timer_key((TIMER_MESSAGE)0x2a));
        }
        if (m_parties[1].Get() != 0)
        {
            CParty* party = (CParty*)m_parties[1].Get();
            TimerCompleteLoadAfterAssaultTimeout::registNextTimer(
                0x3c, party->GetPartyIndex(), party->gen_timer_key((TIMER_MESSAGE)0x2a));
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        _DelUser(i, true);
    }
    GetInstanceAssaultMgr()->FreeAssaultPlace(m_field104, this);
}

} // namespace pvp_assault

// ORIG T 0x0863771a size 0x42：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE 1, partyIdx, TIMER_MESSAGE 0x2a,
//                               sec, key, 0)
// —— CloseAssault 后按队安排 CompleteLoadAfterAssault 超时重载。
void TimerCompleteLoadAfterAssaultTimeout::registNextTimer(long sec,
                                                           int p1, int p2)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, p1,
                                (TIMER_MESSAGE)0x2a, (int)sec, (unsigned int)p2,
                                0);
}
