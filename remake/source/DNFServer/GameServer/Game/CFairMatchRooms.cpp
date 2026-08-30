// ============================================================================
// df_game_r 还原 —— fair_pvp::CFairMatch 重逻辑组（唯一 owner 本 TU）
// 依据 docs/class_func_reports/fair_pvp__CFairMatch/*.md（汇编=objdump -d，
// C=Ghidra 反编译）。每个函数定义上方标注 ORIG 地址与报告路径；凡无独立
// 报告仅凭 objdump 转写的行均已标【推断】。
//
// 字段映射（CFairMatch.h 布局契约固定不变）：
//   +0x004 m_field04        匹配状态机（0空闲/1排队/2超时兜底/3匹配中/
//                           4对战中/5局间）
//   +0x008 m_roomInfo       PVP_ROOM_INFO（title/byte4/short6/byte8/passwd）
//   +0x018 m_matchNotifyId  TimerStartMatching 附带 id（ctor 置 nextAuthValue_）
//   +0x01c m_teams[0]       TeamInfoThird（m_field00@+0/m_field04@+4/
//                           m_players[4]@+8，stride 0x48）
//   +0x064 m_teams[1]
//   +0x0ac m_fieldAC        OnConnectP2P 完成连接计数
//   +0x0b0 m_fieldB0        已完赛局数（OnReward ++，<3 允许续赛）
//   +0x0b4 m_fieldB4        _SetEnterCharacNo 写入的进场角色号
//   +0x0b8 m_fieldB8        队0 队长座位（OnMatchRoom 存 GetManagerSeat()；
//                           OnAskRematch 存队伍重赛决定，双用【推断】）
//   +0x0bc m_fieldBC        队1 队长座位（OnMatchRoom 尾 get_user_seat(房主)；
//                           双用同上）
//   +0x0c0 m_fieldC0        IMatch* next match（GetNextMatch/SetNextMatch 槽）
//
// 等待表交互均经 pWaitingRoomList_（IRoomList）虚槽：AddRoom/DelRoom/Match。
// ============================================================================
#include "CFairMatch.h"

#include <algorithm>    // std::swap
#include <cstddef>

#include "PvP_Room.h"              // PvP_Room 全接口/CGameManager
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CMissionList_Charac.h"
#include "MissionTypes.h"
#include "CFairPvPScore.h"
#include "CSwitchLog.h"
#include "GameTypes.h"             // cMyTrace
#include "LogManager.h"
#include "CDataManager.h"
#include "ServerParameterScript.h" // m_schoolMatchSeal / m_schoolMatchDailyPlayLimit
#include "GameWorld.h"
#include "TimerQueue.h"
#include "TimerDispatcher.h"
#include "CRelayBattleMgr.h"

using fair_pvp::CFairMatch;
using fair_pvp::TeamInfoThird;
using fair_pvp::PlayerInfoThird;

// CEloRating / PvPGlobalEnvironmentVariable 权威声明在 OnlinePreliminary.h
// （OnlinePreliminary 批维护；calc 定义在其 OnlinePreliminaryMatch.cpp）
#include "OnlinePreliminary.h"
#include "CGameManager.h"

// SPS 训练印章配置块视图（m_serverParameter 位于 CDataManager+0x68；
// ORIG 读 dm+0x49d8..0x49e0 = SPS+0x4970..0x497a；宏内联——P49）
#define FM_SPS() (G_CDataManager()->m_serverParameter)
// [dedup] moved to CFairMatch.cpp
// [dedup] moved to CFairMatch.cpp
int CFairMatch::nextAuthValue_ = 0;                       // B 0x0943e150
// [dedup] moved to CFairMatch.cpp

// TeamInfoThird 首字节视图（ORIG 以 *(char*)(this+team*0x48+0x1c) 判在座；
// 宏内联——P49：-O0 下 static inline helper 产真实 call 符号）
#define FM_TeamByte0(t) (*(reinterpret_cast<unsigned char*>(&(t))))


// ===================================================================== //
// 小函数组（ORIG 逐指令转写）
// ===================================================================== //

// _CheckSameTeam ORIG T 0x085629e8 报告 _CheckSameTeam.md —— 恒返 1
int CFairMatch::_CheckSameTeam(PvP_Room* /*room*/, CUser* /*user*/)
{
    return 1;
}

// CheckUnusableEquipmentInUser ORIG T 0x08562cfe 报告同名 md —— 恒返 1（bool 返回：
// CheckJoinable +0x12/OnCreateRoom +0x11 均以 xor $1 取反，P22 实证）
bool CFairMatch::CheckUnusableEquipmentInUser(CUser* /*user*/)
{
    return true;
}

// OnGiveup ORIG T 0x08564de0 报告 OnGiveup.md —— 恒返 1
int CFairMatch::OnGiveup(CUser* /*user*/)
{
    return 1;
}

// OnWaitRematch ORIG T 0x0856649a 报告 OnWaitRematch.md —— 空 body
void CFairMatch::OnWaitRematch(CUser* /*user*/, PvP_Room* /*room*/)
{
}

// GetEndPvPResultTimeout ORIG W 0x0856892a —— mov $0xea60
int CFairMatch::GetEndPvPResultTimeout()
{
    return 0xea60;
}

// GetNextMatch / SetNextMatch / _SetEnterCharacNo ORIG W 0x08568934/42/54
IMatch* CFairMatch::GetNextMatch()
{
    return reinterpret_cast<IMatch*>(m_fieldC0);
}
void CFairMatch::SetNextMatch(IMatch* next)
{
    m_fieldC0 = reinterpret_cast<int>(next);
}
void CFairMatch::_SetEnterCharacNo(int no)
{
    m_fieldB4 = no;
}

// _GetRating ORIG W 0x08568966 —— m_teams[team].GetTeamRating() 取 float
float CFairMatch::_GetRating(ENUM_SCHOOL_MATCH_TEAM team)
{
    return static_cast<float>(m_teams[team].GetTeamRating());
}

// ===================================================================== //
// 检查组
// ===================================================================== //

// ORIG：while 形循环 seat<=7（cmpl $0x7/setle）统计 room->get_team(seat)==3 观战者；
// 末尾 size() 返回值以 ebx 缓存跨 call，cmp/setge 两分支：size-count >= playCount → 0，否则 1。
int CFairMatch::_CheckObserver(PvP_Room* room, CUser* /*user*/)
{
    int observerCnt = 0;
    for (int seat = 0; seat <= 7; ++seat) {
        if (room->get_team(seat) == 3)   // 队伍号 3 = 观战席
            ++observerCnt;
    }
    // ORIG 尾部：size() 返回值 ebx 缓存，call 后 movzbl %al 收窄 playCount，
    // cmp/setge 两分支：size-count >= playCount → 0，否则 → 1
    if (static_cast<int>(m_teams[0].size()) - observerCnt
        >= static_cast<int>(static_cast<unsigned char>(GetPlayCountPerOneTeam())))
        return 0;
    return 1;
}

// CheckJoinable ORIG T 0x08562a6e 报告 CheckJoinable.md
int CFairMatch::CheckJoinable(PvP_Room* room, CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 1;
    if (!(room->get_player_count() <
          static_cast<int>((GetPlayCountPerOneTeam() & 0xff) * 2)))
        return 4;
    const int total = static_cast<int>(
        static_cast<unsigned>(m_teams[0].size()) +
        static_cast<unsigned>(m_teams[1].size()));
    if (total > static_cast<int>((GetPlayCountPerOneTeam() & 0xff) * 2))
        return 4;

    switch (m_field04) {
    case 0:
        if (_CheckSameTeam(room, user) != 1)
            return 4;
        if (_CheckObserver(room, user) != 1)
            return 4;
        return 0;
    case 3:
        return 0;
    default:   // 1/2/4/5 及越界跳表 default 全部拒绝
        return 4;
    }
}

// OnReady ORIG T 0x08565018 报告 OnReady.md
bool CFairMatch::OnReady(CUser* /*user*/, bool ready)
{
    if (!ready && m_field04 == 4)
        return false;
    return true;
}

// _CheckMatchable(const) ORIG T 0x08563a54 报告 _CheckMatchable.md
int CFairMatch::_CheckMatchable(PvP_Room* room) const
{
    int observerCnt = 0;
    for (int seat = 0; seat <= 7; ++seat) {
        if (room->get_team(seat) == 3)
            ++observerCnt;
        CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(seat));
        if (u != 0) {
            if (G_CDataManager()->GetMaxGradePvPChannel() <
                u->get_pvp_grade())
                return 0xd;
        }
    }
    const int cnt =
        static_cast<int>(static_cast<unsigned>(m_teams[0].size()) - observerCnt);
    return cnt > static_cast<int>(const_cast<CFairMatch*>(this)->GetPlayCountPerOneTeam() & 0xff)
        ? 0x16 : 0;
}

// _CheckRematchable(const) ORIG T 0x08563b16 报告 _CheckRematchable.md
bool CFairMatch::_CheckRematchable(PvP_Room* room) const
{
    int counts[3] = { 0, 0, 0 };   // [1]/[2] 计真实两队人数
    for (int seat = 0; seat <= 7; ++seat) {
        const int team = room->get_team(seat);
        if (team == 1 || team == 2)
            ++counts[team];
    }
    if (counts[1] != static_cast<int>(const_cast<CFairMatch*>(this)->GetPlayCountPerOneTeam() & 0xff))
        return false;
    if (counts[2] != static_cast<int>(const_cast<CFairMatch*>(this)->GetPlayCountPerOneTeam() & 0xff))
        return false;
    return true;
}

// CheckMatchable ORIG T 0x08565c54 报告 CheckMatchable.md
bool CFairMatch::CheckMatchable(IMatch* other)
{
    CUser* otherUser = 0;
    for (int i = 0; i < 4; ++i) {
        otherUser = other->GetUser(static_cast<unsigned>(i));
        if (otherUser != 0) break;
    }
    if (otherUser == 0) return false;

    CUser* myUser = 0;
    for (int i = 0; i < 4; ++i) {
        myUser = GetUser(static_cast<unsigned>(i));
        if (myUser != 0) break;
    }
    return myUser != 0;
}

// GetGap ORIG T 0x0856491c 报告 GetGap.md —— fabs(_GetRating(0) 差值)
float CFairMatch::GetGap(IMatch* other)
{
    const float mine  = _GetRating(static_cast<ENUM_SCHOOL_MATCH_TEAM>(0));
    const float theirs = static_cast<CFairMatch*>(other)->_GetRating(
        static_cast<ENUM_SCHOOL_MATCH_TEAM>(0));
    const float diff = mine - theirs;
    return diff < 0.0f ? -diff : diff;
}

// idx∈[0,4)：ORIG 反汇编 0x856515c/0x856517d 实证槽基址 = this + 0x24 + idx*0x10
// （movzbl 0x18+(idx+1)*16 读 m_flag、+(idx+1)*16+0x14 读 m_user），即
// m_teams[0].m_players[idx]（m_teams[0] @ +0x1c，m_players @ +0x08）。
CUser* CFairMatch::GetUser(unsigned int idx)
{
    if (idx > 3u) return 0;
    PlayerInfoThird& slot = m_teams[0].m_players[idx];
    if (slot.m_flag == 0) return 0;
    return slot.m_user;
}

// GetPlayerCount ORIG T 0x8566d3e 报告 GetPlayerCount.md
int CFairMatch::GetPlayerCount()
{
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        if (m_teams[1].m_players[i].m_flag != 0) ++count;
        if (m_teams[0].m_players[i].m_flag != 0) ++count;
    }
    return count;
}

// _FindTeam ORIG T 0x856663a8 报告 _FindTeam.md
int CFairMatch::_FindTeam(CUser* user)
{
    for (int t = 0; t < 2; ++t) {
        for (int i = 0; i < 4; ++i) {
            PlayerInfoThird& s = m_teams[t].m_players[i];
            if (s.m_flag != 0 && s.m_user != 0 && s.m_user == user)
                return t;
        }
    }
    return 2;
}

// ===================================================================== //
// 房间生命周期组
// ===================================================================== //

// OnCreateRoom ORIG T 0x8562d08 报告 OnCreateRoom.md
// 装备检查通过 → 自增授权号、复位双队与计数；失败返回错误码 0x13
// （ORIG +0x18 mov $0x13 / +0xb7 mov $0x0；IMatch 基类返回 int）
int CFairMatch::OnCreateRoom(CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 0x13;
    ++nextAuthValue_;
    m_matchNotifyId = nextAuthValue_;
    m_field04 = 0;
    m_teams[1].reset();          // ORIG 先 reset +0x64 队再 reset +0x1c 队
    m_teams[0].reset();
    m_teams[0].add(user);
    m_fieldAC = 0;
    m_fieldB0 = 0;
    m_fieldB4 = 0;
    m_fieldB8 = 0;
    m_fieldBC = 0;
    m_fieldC0 = 0;
    return 0;
}

// OnDestroyRoom ORIG T 0x8562dc6 报告 OnDestroyRoom.md
void CFairMatch::OnDestroyRoom(CUser* /*user*/)
{
    m_field04 = 0;
    pWaitingRoomList_->DelRoom(this);        // 等待表 vtable[+0x04]=DelRoom(this)
}

// OnJoinRoom ORIG T 0x8562df6 报告 OnJoinRoom.md
void CFairMatch::OnJoinRoom(CUser* user)
{
    if (m_field04 != 3)
        m_teams[0].add(user);
}

// _SendMatchStatePacket ORIG T 0x85651a6 报告 _SendMatchStatePacket.md
// msg=0xaf：state 广播给前 teamSpan 支队伍全员
void CFairMatch::_SendMatchStatePacket(int state, int teamSpan)
{
    PacketGuard pkt;
    pkt.put_header(0, 0xaf);
    pkt.put_byte(state);
    pkt.finalize(true);
    for (int t = 0; t < teamSpan; ++t) {
        for (int i = 0; i < 4; ++i) {
            PlayerInfoThird& s = m_teams[t].m_players[i];
            if (s.m_flag != 0 && s.m_user != 0)
                s.m_user->Send(pkt);
        }
    }
}

// _BackupRoomInfo ORIG T 0x8563674 报告 _BackupRoomInfo.md
void CFairMatch::_BackupRoomInfo(PvP_Room* room)
{
    room->get_room_info(&m_roomInfo);
}

// _RegisterWaitingRoom ORIG T 0x8563a32 报告 _RegisterWaitingRoom.md
// (**pWaitingRoomList_)->vtable[0] = AddRoom —— 经 IRoomList::AddRoom(this)
void CFairMatch::_RegisterWaitingRoom(PvP_Room* /*room*/)
{
    // ORIG 0x8563a38 实证无 null 检查直接解引用（pWaitingRoomList_ 恒非 0）
    pWaitingRoomList_->AddRoom(this);
}

// _ChangeTeamLeader ORIG T 0x8563272 报告 _ChangeTeamLeader.md
// 离队者恰为队长座位 → 该队首名在座玩家接任并 0x13d 包通知全队
void CFairMatch::_ChangeTeamLeader(PvP_Room* room,
                                   ENUM_SCHOOL_MATCH_TEAM teamIdx,
                                   int oldLeaderSeat)
{
    CUser* newLeader = 0;
    int& leaderSeatRef = (teamIdx == 0) ? m_fieldB8 : m_fieldBC;
    if (leaderSeatRef == oldLeaderSeat) {
        for (int i = 0; i < 4; ++i) {
            PlayerInfoThird& s = m_teams[teamIdx].m_players[i];
            if (s.m_flag != 0) { newLeader = s.m_user; break; }
        }
    }
    if (newLeader != 0) {
        leaderSeatRef = room->get_user_seat(newLeader);
        PacketGuard pkt;
        pkt.put_header(0, 0x13d);
        pkt.put_short(leaderSeatRef);
        pkt.finalize(true);
        m_teams[teamIdx].SendToTeam(pkt);
    }
}

// OnLeaveRoom ORIG T 0x8562fbc 报告 OnLeaveRoom.md
void CFairMatch::OnLeaveRoom(CUser* user, PvP_Room* room,
                             int teamNo, int flag)
{
    if (m_field04 == 4) {
        _SendMatchStatePacket(4, 2);
        m_field04 = 0;
        _PullOutRoomTeam(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(1));
    } else if (m_field04 == 5) {
        if (room->GetTeamCount(teamNo) == 1)
            _UpdateRating(teamNo, static_cast<ENUM_ELO_RATING_SYSTEM>(0));
        const int myTeam = _FindTeam(user);
        if (myTeam == 2) {
            LogManager::logFormat(
                1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__,
                0xb0b, "error _FindTeam");
            return;
        }
        const ENUM_SCHOOL_MATCH_TEAM other =
            _GetOtherTeam(static_cast<ENUM_SCHOOL_MATCH_TEAM>(myTeam));
        if (other == static_cast<ENUM_SCHOOL_MATCH_TEAM>(2)) {
            LogManager::logFormat(
                1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__,
                0xb12, "error _GetOtherTeam");
            return;
        }
        if (FM_TeamByte0(m_teams[other]) != 0) {
            PacketGuard pkt;
            pkt.put_header(0, 0x13a);
            pkt.finalize(true);
            m_teams[other].SendToTeam(pkt);
            m_teams[other].m_field04 = 1;
        }
    } else {
        m_field04 = 0;
    }

    if (teamNo == 1) {
        m_teams[0].del(user);
        _ChangeTeamLeader(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(0), flag);
    } else {
        m_teams[1].del(user);
        _ChangeTeamLeader(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(1), flag);
    }
    if (pWaitingRoomList_) pWaitingRoomList_->DelRoom(this);
    _SendMatchStatePacket(5, 1);
}

// OnUnregisterMatch ORIG T 0x85663cc6?（README 行：0x08563cc6）报告同名 md
// ORIG 返回 int：条件不满足 → 0；执行 → 1。
int CFairMatch::OnUnregisterMatch(CUser* /*user*/, PvP_Room* /*room*/)
{
    if (m_field04 == 1 || m_field04 == 2) {
        m_field04 = 0;
        if (pWaitingRoomList_) pWaitingRoomList_->DelRoom(this);
        _SendMatchStatePacket(5, 1);
        return 1;
    }
    return 0;
}

// OnRegisterMatch ORIG T 0x85623bba?（README 0x08563bba）报告同名 md
// ORIG 返回 int：m_field04==1 → 1；失败路径 → 0；成功 → 1。
int CFairMatch::OnRegisterMatch(CUser* user, PvP_Room* room)
{
    if (m_field04 == 1)
        return 1;
    if (!bMatchable_) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199), 0x13);
        return 0;
    }
    const int err = _CheckMatchable(room);
    if (err != 0) {
        user->SendCmdErrorPacket(
            static_cast<ENUM_CMDPACKET>(199),
            static_cast<unsigned char>(err & 0xff));
        return 0;
    }
    // ORIG 0x8563c54 实证：m_field04==2（超时兜底中）跳过等待表注册
    if (m_field04 != 2) {
        _RegisterWaitingRoom(room);
    }
    m_field04 = 1;
    TimerStartMatching::registNextTimer(10, room->get_index(),
                                        m_matchNotifyId);
    _SendMatchStatePacket(0, 1);
    return 1;
}

// ===================================================================== //
// 对局进行 / 换房组
// ===================================================================== //

// _CheckMovableRoom ORIG T 0x85663ed8?（README 0x08563ed8）报告同名 md
int CFairMatch::_CheckMovableRoom(IMatch* other, PvP_Room* /*room*/,
                                  PvP_Room** out)
{
    // ORIG 0x8563ede 实证 other==0 直接 return 0（无 trace；0xc5d trace 仅在
    // 循环后 member==0 分支，见下）
    if (other == 0) {
        return 0;
    }
    CUser* member = 0;
    for (int i = 0; i < 4; ++i) {
        member = other->GetUser(static_cast<unsigned>(i));
        if (member != 0) break;
    }
    if (member == 0) {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xc5d, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0xc5d);
        return 0;
    }
    *out = reinterpret_cast<PvP_Room*>(member->GetPVPRoom());
    if (*out == 0) {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xc64, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0xc64);
        return 0;
    }
    return 1;
}

// _PullOutRoom ORIG T 0x856633ce?（README 0x085633ce）报告同名 md
void CFairMatch::_PullOutRoom(CUser* user, PvP_Room* room, int seat)
{
    {
        CSwitchLog log(
            "void fair_pvp::CFairMatch::_PullOutRoom(CUser*, PvP_Room*, int)",
            0xb5a, 0, 0);
        log("pvp@log %s,%s,room(%d),%d,%d",
            user->get_acc_name(), user->getCurCharacName(),
            room->get_index(), room->get_pvp_battle_mode(), seat);
    }
    const int ok = room->OnSetPVPSeatState(
        user, seat,
        static_cast<ENUM_SEAT_STATE>(0xfe),
        static_cast<ENUM_WALKOUT_TYPE>(2));
    if (!ok) {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
}

// _PutInRoom ORIG T 0x856334bc?（README 0x085634bc）报告同名 md
int CFairMatch::_PutInRoom(CUser* user, PvP_Room* room, bool observer)
{
    PacketGuard pkt;
    int seat = 0;
    const int result = room->join_room(user, seat, observer);
    {
        CSwitchLog log("int fair_pvp::CFairMatch::_PutInRoom(CUser*, PvP_Room*, bool)",
                       0xb6a, 0, 0);
        log("pvp@log %s,%s,room(%d),%d,%d",
            user->get_acc_name(), user->getCurCharacName(),
            room->get_index(), room->get_pvp_battle_mode(), result);
    }
    if (result >= 1) {
        pkt.put_header(1, 0x36);
        pkt.put_byte(0);
        pkt.put_byte(static_cast<unsigned char>(result));
        pkt.finalize(true);
        user->Send(pkt);
    } else {
        // ORIG: make_seat_info(room,(char*)&pkt,seat) —— PacketGuard 对象
        // 地址被传入，函数内部经其 InterfacePacketBuf 基类写包。
        char* guardAsBuf = reinterpret_cast<char*>(&pkt);
        room->make_seat_info(guardAsBuf, seat);
        G_GameWorld()->send_all(pkt);
        G_GameWorld()->goto_pvp(user);
    }
    pkt.finalize(true);   // ORIG 尾部 finalize（两条路径均执行）
    return seat;
}

// _MakeRoom ORIG T 0x8563692 报告 _MakeRoom.md
PvP_Room* CFairMatch::_MakeRoom(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(1, 0x35);

    CGameManager* mgr = G_CGameManager();
    PvP_Room* room = mgr->GetPvp();
    if (room == 0) {
        pkt.put_byte(0);
        pkt.put_byte(4);
        pkt.finalize(true);
        user->Send(pkt);
        return 0;
    }

    // ORIG 实参序列：byte=m_roomInfo.byte4、name=title.c_str()、
    // short=short6、bool=byte8、password=passwd.c_str()/length()、mode=2
    const int result = room->create_room(
        user,
        static_cast<char>(m_roomInfo.byte4),
        const_cast<char*>(m_roomInfo.title.c_str()),
        m_roomInfo.short6,
        m_roomInfo.byte8 != 0,
        m_roomInfo.passwd.c_str(),
        static_cast<int>(m_roomInfo.passwd.length()),
        static_cast<PVP_BATTLE_MODE>(2));
    {
        CSwitchLog log("PvP_Room* fair_pvp::CFairMatch::_MakeRoom(CUser*)",
                       0xba0, 0, 0);
        log("pvp@log %s,%s,room(%d),%d,%d",
            user->get_acc_name(), user->getCurCharacName(),
            room->get_index(), room->get_pvp_battle_mode(), result);
    }

    if (result < 1) {
        pkt.clear();
        pkt.put_header(0, 2);
        pkt.put_byte(1);
        pkt.put_short(1);
        // ORIG 0x85661b4 实证 make_basic_info 直接写 pkt（无独立缓冲/put_binary；
        // make_basic_info 内部将 buf reinterpret_cast 为 InterfacePacketBuf）
        user->make_basic_info(reinterpret_cast<char*>(&pkt), 1);
        pkt.finalize(true);
        user->Send(pkt);

        pkt.clear();
        pkt.put_header(0, 0x29);
        pkt.put_short(1);
        room->make_room_info(reinterpret_cast<char*>(&pkt));
        pkt.finalize(true);
        G_GameWorld()->send_all(pkt);
        G_GameWorld()->goto_pvp(user);
        return room;
    }

    pkt.put_byte(0);
    pkt.put_byte(static_cast<unsigned char>(result));
    pkt.finalize(true);
    user->Send(pkt);
    LogManager::logFormat(
        1, "MatchingSystem.cpp",
        __PRETTY_FUNCTION__, 0xba9,
        "create_room fail: PVP슬롯반환 ID[%d]\n", room->get_index());
    G_CGameManager()->PutPvp(room);
    return 0;
}

// _MoveRoom ORIG T 0x8563d32 报告 _MoveRoom.md
// 遍历 from 房 8 座逐个换到 to；观战队（team==3）换入后重设观战态
void CFairMatch::_MoveRoom(PvP_Room* to, PvP_Room* from, bool observerFlag)
{
    for (int seat = 0; seat <= 7; ++seat) {
        CUser* mover =
            reinterpret_cast<CUser*>(from->get_user_seat(seat));
        if (mover == 0) continue;
        const int teamOf = from->get_team(seat);

        PacketGuard pkt;
        pkt.put_header(0, 0x29);
        pkt.put_short(1);
        from->make_room_info(reinterpret_cast<char*>(&pkt));  // 同 _PutInRoom 注释
        pkt.finalize(true);
        from->send_to_pvp(pkt);

        _PullOutRoom(mover, from, seat);
        _SetEnterCharacNo(mover->getCurCharacNo());
        const int newSeat = _PutInRoom(mover, to, observerFlag);
        if (teamOf == 3) {
            to->OnSetPVPSeatState(mover, newSeat,
                                  static_cast<ENUM_SEAT_STATE>(3),
                                  static_cast<ENUM_WALKOUT_TYPE>(2));
        }
    }
}

// OnMatchRoom ORIG T 0x8564150 报告 OnMatchRoom.md。ORIG 失败路径 → 0；成功 → 1。
int CFairMatch::OnMatchRoom(PvP_Room* room, int notifyId)
{
    if (m_matchNotifyId != notifyId) {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xc8b, 0);
        trace("OnMatchRoom timeout");
        return 0;
    }
    if (m_field04 != 1)
        return 0;

    room->setReamtchable(GetPlayCountPerOneTeam() == 1);
    m_fieldB8 = room->GetManagerSeat();

    IMatch* other = 0;
    bool matched = pWaitingRoomList_ != 0 && pWaitingRoomList_->Match(this, &other);
    if (!matched) {   // 等待表 vtable[+0x08]=Match
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        {
            CSwitchLog l0("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcb0, 0, 0);
            char nameBuf[256]; nameBuf[0] = '\0';
            GetUserName(this, nameBuf);
            l0("pvp@log 1 call match(%x), %s ",
               reinterpret_cast<void*>(this), nameBuf);
            char nameBuf2[256];
            GetUserName(other, nameBuf2);
            CSwitchLog l1("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcb1, 0, 0);
            l1("pvp@log 1 opposite match(%x), %s ", other, nameBuf2);
        }
        return 0;
    }

    PvP_Room* movable = 0;
    if (_CheckMovableRoom(other, room, &movable) == 0) {
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        {
            CSwitchLog l0("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcb0, 0, 0);
            char nameBuf[256]; GetUserName(this, nameBuf);
            l0("pvp@log 1 call match(%x), %s ",
               reinterpret_cast<void*>(this), nameBuf);
            char nameBuf2[256]; GetUserName(other, nameBuf2);
            CSwitchLog l1("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcb1, 0, 0);
            l1("pvp@log 1 opposite match(%x), %s ", other, nameBuf2);
        }
        return 0;
    }

    if (room->get_index() == movable->get_index()) {
        // 撞进同一间：只作战斗开始兜底
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        {
            CSwitchLog l0("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcbb, 0, 0);
            char nameBuf[256]; GetUserName(this, nameBuf);
            l0("pvp@log 2 call match(%x), %s ",
               reinterpret_cast<void*>(this), nameBuf);
            char nameBuf2[256]; GetUserName(other, nameBuf2);
            CSwitchLog l1("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcbc, 0, 0);
            l1("pvp@log 2 opposite match(%x), %s ", other, nameBuf2);
        }
        return 0;
    }

    // 正式配成：沿其它匹配单元的 next-match 链把对方队伍搬进来
    m_teams[1].reset();                       // TeamInfoThird::reset(+0x64)
    m_field04 = 3;
    int leaderTeamSide = 0;                   // local_2c 满编一侧标记
    CUser* managerUser = 0;                   // local_28
    for (IMatch* it = other; it != 0;
         it = it->GetNextMatch()) {
        PvP_Room* moveTarget = 0;
        if (_CheckMovableRoom(it, room, &moveTarget) == 0) {
            m_field04 = 2;
            _SendMatchStatePacket(1, 1);
            return 0;
        }
        if (static_cast<CFairMatch*>(it)->GetPlayerCount()
            == static_cast<int>(GetPlayCountPerOneTeam())) {
            leaderTeamSide = 1;
            // 【推断】ORIG +0x28 为 manager 用户指针；PvP_Room.h 现声明 int
            managerUser = reinterpret_cast<CUser*>(moveTarget->GetManager());
            _BackupRoomInfo(moveTarget);
        }
        for (int p = 0; p < 4; ++p) {
            PlayerInfoThird& s =
                static_cast<CFairMatch*>(it)->m_teams[1].m_players[p];
            if (s.m_flag != 0 && s.m_user != 0)
                m_teams[leaderTeamSide].add(s.m_user);
        }
        _SendMatchStatePacket(3, 2);
        _MoveRoom(moveTarget, room, leaderTeamSide == 0);
    }
    m_fieldBC = room->get_user_seat(managerUser);
    if (pWaitingRoomList_) pWaitingRoomList_->DelRoom(this);
    m_field04 = 4;
    _SendMatchStatePacket(2, 2);
    m_fieldAC = 0;
    const unsigned timerKey = room->gen_timer_key(static_cast<TIMER_MESSAGE>(0x3b));
    G_TimerQueue()->InsertTimer(
        static_cast<TimerEntry::OBJ_TYPE>(5), room->get_index(),
        static_cast<TIMER_MESSAGE>(0x3b), 0xb4,
        static_cast<int>(timerKey), 0);
    PacketGuard pkt;
    {
        char* gb = reinterpret_cast<char*>(&pkt);
        room->make_seat_info(gb, 0xffff);     // ORIG make_seat_info(room,&pkt,-1)
        G_GameWorld()->send_all(pkt);
    }
    for (int t = 0; t < 2; ++t) {
        pkt.clear();
        pkt.put_header(0, 0x13d);
        pkt.put_short((t == 0) ? m_fieldB8 : m_fieldBC);
        pkt.finalize(true);
        m_teams[t].SendToTeam(pkt);
    }
    return 1;
}

// ===================================================================== //
// 结算 / 重赛 / 奖励组
// ===================================================================== //

// _ReturnTeam ORIG T 0x85664dea?（README 0x08564dea）报告同名 md
// 把 room 内指定队成员退回各自新开房；excludedUser 不参与搬运
PvP_Room* CFairMatch::_ReturnTeam(PvP_Room* room, int teamNo, CUser* excluded)
{
    {
        CSwitchLog log(
            "PvP_Room* fair_pvp::CFairMatch::_ReturnTeam(PvP_Room*, int, CUser*)",
            0xd95, 0, 0);
        log("pvp@log room(%d), %d, %d",
            room->get_index(), room->get_pvp_battle_mode(),
            room->get_waiter_count());
    }
    CUser* member = 0;
    PvP_Room* newRoom = 0;
    for (int seat = 0; seat <= 7; ++seat) {
        member = reinterpret_cast<CUser*>(room->get_user_seat(seat));
        if (member == 0) continue;
        const int teamOf = room->get_team(seat);
        if (teamNo == teamOf) {
            if (teamNo == 2) {   // 观战队：直接按 walkoutType=2 清座
                room->OnSetPVPSeatState(
                    member, seat,
                    static_cast<ENUM_SEAT_STATE>(1),
                    static_cast<ENUM_WALKOUT_TYPE>(2));
            }
            continue;   // asm: 命中本队后不搬
        }
        if (excluded != 0 && member == excluded)
            continue;

        _PullOutRoom(member, room, seat);
        if (newRoom == 0) {
            newRoom = _MakeRoom(member);
        } else {
            _PutInRoom(member, newRoom, true);   // asm 第3参恒 1
        }
    }
    m_field04 = 0;
    return newRoom;
}

// _PullOutRoomTeam ORIG T 0x85665d86?（README 0x08565d86）报告同名 md
int CFairMatch::_PullOutRoomTeam(PvP_Room* room, ENUM_SCHOOL_MATCH_TEAM teamIdx)
{
    {
        CSwitchLog log(
            "bool fair_pvp::CFairMatch::_PullOutRoomTeam(PvP_Room*, ENUM_SCHOOL_MATCH_TEAM)",
            0xfa7, 0, 0);
        log("pvp@log room(%d),%d,%d",
            room->get_index(), room->get_pvp_battle_mode(),
            static_cast<int>(teamIdx));
    }
    int lastTeam = 0;
    for (int i = 0; i < 4; ++i) {
        PlayerInfoThird& s = m_teams[teamIdx].m_players[i];
        if (s.m_flag != 0 && s.m_user != 0) {
            const int seat = room->get_user_seat(s.m_user);
            if (seat != -1) {
                lastTeam = room->get_team(seat);
                _PullOutRoom(s.m_user, room, seat);
            }
        }
    }
    if (lastTeam == 1) {
        for (int seat = 0; seat <= 7; ++seat) {
            CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(seat));
            if (u != 0 && room->get_team(seat) == 2) {
                room->OnSetPVPSeatState(u, seat,
                                        static_cast<ENUM_SEAT_STATE>(1),
                                        static_cast<ENUM_WALKOUT_TYPE>(2));
            }
        }
    }
    return 1;
}

// OnWalkOut ORIG T 0x8564fb2 报告 OnWalkOut.md。ORIG 返回 1。
// 转发到本类虚槽 +0x2c（OnLeaveRoom(user,room,team,seat)）
int CFairMatch::OnWalkOut(PvP_Room* room, int seat)
{
    const int team = room->get_team(seat);
    const int userSeat = room->get_user_seat(seat);
    OnLeaveRoom(reinterpret_cast<CUser*>(userSeat), room, team, seat);
    return 1;
}

// CheckRematch ORIG T 0x85665d04 报告 CheckRematch.md
int CFairMatch::CheckRematch(PvP_Room* room)
{
    if (m_fieldB0 >= fair_pvp::maxRematchCount_) return 0;
    if (!_CheckRematchable(room)) return 0;
    // this[t*0x48+0x1c] 首字节须为 1（两队“可重赛”标记，TeamInfoThird 头）
    for (int t = 0; t < 2; ++t) {
        if (FM_TeamByte0(m_teams[t]) != 1) return 0;
    }
    return 1;
}

// OnEndFight ORIG T 0x8565048 报告 OnEndFight.md
void CFairMatch::OnEndFight(PvP_Room* room)
{
    const int mgrTeam = room->get_manager_team();
    m_field04 = 0;
    if (GetPlayCountPerOneTeam() == 1) {
        // 单人模式：0x3e 定时器 0ms 兜底唤醒战房清算链
        const unsigned key = room->gen_timer_key(static_cast<TIMER_MESSAGE>(0x3e));
        G_TimerQueue()->InsertTimerInMilisecond(
            static_cast<TimerEntry::OBJ_TYPE>(1), room->get_index(),
            static_cast<TIMER_MESSAGE>(0x3e),
            static_cast<long long>(0), static_cast<int>(key), 0);
        return;
    }
    _ReturnTeam(room, mgrTeam, 0);
    if (mgrTeam != 1) {
        // m_teams[1] 整块 memcpy 到 m_teams[0]（0x12 个 int）
        unsigned* dst = reinterpret_cast<unsigned*>(&m_teams[0]);
        const unsigned* src = reinterpret_cast<const unsigned*>(&m_teams[1]);
        for (int n = 0x12; n != 0; --n) *dst++ = *src++;
    }
    m_teams[0].refresh();
    m_teams[1].reset();
}

// OnReward ORIG T 0x85665966?（README 0x08565966）报告 OnReward.md
void CFairMatch::OnReward(PvP_Room* room, int winTeam, bool draw)
{
    if (draw) {
        _UpdateRating(1, static_cast<ENUM_ELO_RATING_SYSTEM>(1));
        _UpdateRating(2, static_cast<ENUM_ELO_RATING_SYSTEM>(1));
        UpdateScore(room, m_teams[1].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(0),
                    0, 0, 1);
        UpdateScore(room, m_teams[0].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(1),
                    0, 0, 1);
    } else if (winTeam == 1) {
        _UpdateRating(1, static_cast<ENUM_ELO_RATING_SYSTEM>(2));
        _UpdateRating(2, static_cast<ENUM_ELO_RATING_SYSTEM>(0));
        UpdateScore(room, m_teams[1].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(0),
                    1, 0, 0);
        UpdateScore(room, m_teams[0].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(1),
                    0, 1, 0);
    } else {
        _UpdateRating(2, static_cast<ENUM_ELO_RATING_SYSTEM>(2));
        _UpdateRating(1, static_cast<ENUM_ELO_RATING_SYSTEM>(0));
        UpdateScore(room, m_teams[1].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(0),
                    0, 1, 0);
        UpdateScore(room, m_teams[0].m_players[0], static_cast<ENUM_SCHOOL_MATCH_TEAM>(1),
                    1, 0, 0);
    }
    ++m_fieldB0;
    PacketGuard pkt;
    pkt.put_header(0, 0x13c);
    pkt.put_int(3 - m_fieldB0);
    pkt.finalize(true);
    room->send_to_pvp(pkt);
}

// UpdateScore ORIG T 0x8565700 报告 UpdateScore.md
// 遍历 teamIdx 队 4 槽在座玩家逐个结算；win/lose/result 即报告 param5/6/7。
// info 仅取 m_job(+5)/m_growType 低 nibble(+6)（ORIG asm 读 info+5/+6）。
void CFairMatch::UpdateScore(PvP_Room* room, PlayerInfoThird& info,
                             ENUM_SCHOOL_MATCH_TEAM teamIdx,
                             int win, int lose, int result)
{
    (void)teamIdx;
    (void)info;
    for (int i = 0; i < 4; ++i) {
        PlayerInfoThird& s = m_teams[teamIdx].m_players[i];
        if (s.m_flag == 0 || s.m_user == 0)
            continue;
        CUser* user = s.m_user;
        const int seat = room->get_user_seat(user);
        if (seat < 0 || seat > 7)
            return;
        {
            CSwitchLog log(
                "void fair_pvp::CFairMatch::UpdateScore(PvP_Room*, fair_pvp::PlayerInfoThird&, ENUM_SCHOOL_MATCH_TEAM, int, int, int)",
                0xee5, 0, 0);
            log("pvp@log %s,%s,room(%d),%d,%d,%d",
                user->get_acc_name(), user->getCurCharacName(),
                room->get_index(), room->get_pvp_battle_mode(),
                win, lose);
        }
        CRelayBattleMgr& relay = room->m_relayMgr;   // PvP_Room +0x620
        const int killCnt = relay.GetKillCount(seat);
        const unsigned playCount = GetPlayCountPerOneTeam() & 0xff;
        // ORIG asm：job=info+5 字节、subKey=info+6 低 nibble（info 为队长视图）
        PlayerInfoThird& cap = m_teams[teamIdx].m_players[0];
        CFairPvPScore* score = reinterpret_cast<CFairPvPScore*>(
            user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)));
        score->UpdateScore(static_cast<int>(cap.m_job),
                           cap.m_growType & 0xf,
                           static_cast<int>(playCount),
                           win, lose, result, killCnt);

        CMissionList_Charac* missions = reinterpret_cast<CMissionList_Charac*>(
            user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(8)));
        MissionClearCondition_Parameter cond(
            static_cast<PVP_BATTLE_MODE>(
                GetPlayCountPerOneTeam() == 1 ? 2 : 3));
        missions->Update_GameOver_event(*user, cond);
        GiveTrainingSeal(user, win != 0);
    }
}

// GiveTrainingSeal ORIG T 0x85652c8 报告 GiveTrainingSeal.md
void CFairMatch::GiveTrainingSeal(CUser* user, bool win)
{
    if (user->getCurCharacR() == 0)
        return;

    bool bonusRemain = true;
    ServerParameterScript& sps = FM_SPS();
    const unsigned dailyLimit = sps.m_schoolMatchDailyPlayLimit;
    CFairPvPScore* score = reinterpret_cast<CFairPvPScore*>(
        user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)));
    const int remain =
        static_cast<int>(dailyLimit - score->GetDailyPlayCount());
    if (remain <= 0) {
        score->SetRemainBonusCount(0);
        bonusRemain = false;
    } else {
        score->SetRemainBonusCount(remain);
    }

    int sealTotal = 0;
    if (GetPlayCountPerOneTeam() == 1) {
        sealTotal += static_cast<int>(sps.m_schoolMatchSeal[win ? 0 : 1]);
        if (!bonusRemain) {
            score->SetLastTrainingSealBonusCount(0);
        } else {
            sealTotal += static_cast<int>(sps.m_schoolMatchSeal[win ? 2 : 3]);
            score->SetLastTrainingSealBonusCount(
                static_cast<int>(sps.m_schoolMatchSeal[win ? 2 : 3]));
        }
    } else {
        sealTotal += static_cast<int>(sps.m_schoolMatchSeal[win ? 4 : 5]);
        if (!bonusRemain) {
            score->SetLastTrainingSealBonusCount(0);
        } else {
            sealTotal += static_cast<int>(sps.m_schoolMatchSeal[win ? 6 : 7]);
            score->SetLastTrainingSealBonusCount(
                static_cast<int>(sps.m_schoolMatchSeal[win ? 6 : 7]));
        }
    }
    score->SetLastTrainingSealCount(sealTotal);
    if (sealTotal <= 0)
        return;

    std::vector<std::pair<int,int> > giveList;
    std::vector<int> updatedSlots;
    std::pair<int,int> entry(static_cast<int>(0xcf0), sealTotal);
    giveList.push_back(entry);
    CInventory* inven = user->getCurCharacInvenW();
    const int insertResult = inven->insert_event_items(
        giveList, updatedSlots,
        static_cast<eMoneyAddReason>(3), static_cast<eItemAddReason>(8),
        true, "game_server_msg_171", "game_server_msg_172");
    bool ok = false;
    if (insertResult >= 1 && updatedSlots.size() >= 3u)
        ok = true;
    if (ok) {
        user->SendUpdateItem(CUser::eSendTarget_1,
                             static_cast<ENUM_ITEMSPACE>(0),
                             updatedSlots[0]);
    }
}

// OnRematchTimeout ORIG T 0x85665f92 报告 OnRematchTimeout.md
void CFairMatch::OnRematchTimeout(PvP_Room* room)
{
    const int rematchable = CheckRematch(room);
    {
        CSwitchLog log(
            "virtual void fair_pvp::CFairMatch::OnRematchTimeout(PvP_Room*)",
            0xfd7, 0, 0);
        log("pvp@log room(%d),%d,%d",
            room->get_index(), room->get_pvp_battle_mode(), rematchable);
    }

    if (CheckRematch(room) == 1) {
        m_teams[0].refresh();
        m_teams[1].refresh();
        PacketGuard pkt;
        room->battle_reset();
        for (int seat = 0; seat <= 7; ++seat) {
            if (room->get_user_seat(seat) != 0) {
                CRelayBattleMgr& relay =
                    room->m_relayMgr;
                relay.OnReady(seat, true);
            }
        }
        room->start_pvp();
        room->make_state_info(reinterpret_cast<char*>(&pkt));
        G_GameWorld()->send_all(pkt);
        return;
    }

    // 不能整场续战：两支队伍按各自去留意图分别拉出
    // ORIG: pull 队1 条件 = size(team1)!=0 且 *(int*)(this+0x68)==0
    //      （this+0x68 = m_teams[1].m_field04，重赛决定位）
    if (m_teams[1].size() != 0 && m_teams[1].m_field04 == 0) {
        _PullOutRoomTeam(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(1));
    }
    // ORIG 0x856607a 实证读 *(int*)(this+0x20) = m_teams[0].m_field04（4 字节）
    // （队决定位，OnAskRematch 写入；FM_TeamByte0 队头字节仅 CheckRematch 用）
    const int team0Decision = m_teams[0].m_field04;
    if (team0Decision == 0) {
        _PullOutRoomTeam(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(0));
        if (m_teams[1].size() != 0) {
            PacketGuard pkt;
            pkt.put_header(0, 0x13b);
            pkt.finalize(true);
            m_teams[1].SendToTeam(pkt);
            unsigned* dst = reinterpret_cast<unsigned*>(&m_teams[0]);
            const unsigned* src = reinterpret_cast<const unsigned*>(&m_teams[1]);
            for (int n = 0x12; n != 0; --n) *dst++ = *src++;
            m_teams[0].refresh();
            m_teams[1].reset();
            m_fieldB0 = 0;
        }

    } else if (-1 < team0Decision && team0Decision < 3) {
        const int mgrTeam = room->get_manager_team();
        PvP_Room* kept =
            _ReturnTeam(room, mgrTeam, 0);
        PacketGuard pkt;
        pkt.put_header(0, 0x13b);
        pkt.finalize(true);
        if (kept != 0)
            kept->send_to_pvp(pkt);
        m_teams[0].SendToTeam(pkt);
        m_teams[1].SendToTeam(pkt);
        if (m_teams[0].size() == 0) {
            unsigned* dst = reinterpret_cast<unsigned*>(&m_teams[0]);
            const unsigned* src = reinterpret_cast<const unsigned*>(&m_teams[1]);
            for (int n = 0x12; n != 0; --n) *dst++ = *src++;
        }
        m_teams[0].refresh();
        m_teams[1].reset();
        m_fieldB0 = 0;
    }
}

// OnAskRematch ORIG T 0x856664a0 报告 OnAskRematch.md
// param：0=拒绝 1=接受 2=全队同意提交；队决定写入 m_field04(team)
void CFairMatch::OnAskRematch(CUser* user, int decision)
{
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x1058, "error GetPVPRoom");
        return;
    }
    {
        CSwitchLog log(
            "virtual void fair_pvp::CFairMatch::OnAskRematch(CUser*, int)",
            0x105c, 0, 0);
        log("pvp@log %s,%s,room(%d),%d,%d",
            user->get_acc_name(), user->getCurCharacName(),
            room->get_index(), room->get_pvp_battle_mode(), decision);
    }

    const int myTeam = _FindTeam(user);
    if (myTeam == 2) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x1062, "error _FindTeam");
        return;
    }
    const ENUM_SCHOOL_MATCH_TEAM other =
        _GetOtherTeam(static_cast<ENUM_SCHOOL_MATCH_TEAM>(myTeam));
    if (other == static_cast<ENUM_SCHOOL_MATCH_TEAM>(2)) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x1069, "error _GetOtherTeam");
        return;
    }

    PacketGuard pkt;
    if (-1 < decision && decision < 2) {          // 拒绝 / 单人接受
        pkt.clear();
        pkt.put_header(0, 0x13a);
        pkt.finalize(true);
        m_teams[myTeam].SendToTeam(pkt);
        m_teams[myTeam].m_field04 = decision;     // asm 写 4 字节到 team+4
        if (FM_TeamByte0(m_teams[other]) != 0) {
            m_teams[other].SendToTeam(pkt);
            room->setReamtchable(false);
        }
        room->revengeMission(false);
    } else if (decision == 2) {                   // 全队满编 → 提交重赛
        *reinterpret_cast<unsigned char*>(&m_teams[myTeam]) = 1;   // 队头字节置可重赛标记
        bool reject = true;
        if (m_teams[other].m_field04 == 2) {
            if (m_teams[other].size() != 0 &&
                _CheckRematchable(room) == 1) {
                reject = false;
            }
        }
        if (reject) {
            pkt.clear();
            pkt.put_header(0, 0x13a);
            pkt.finalize(true);
            m_teams[myTeam].SendToTeam(pkt);
            m_teams[myTeam].m_field04 = 1;
        } else if (FM_TeamByte0(m_teams[other]) != 0) {
            pkt.clear();
            pkt.put_header(0, 0x13a);
            pkt.finalize(true);
            room->send_to_pvp(pkt);
            room->revengeMission(true);
        }
    }

    // 全房广播本玩家投票（msg 0x137）
    pkt.clear();
    pkt.put_header(0, 0x137);
    pkt.put_byte(decision);
    pkt.put_short(room->get_user_seat(user));
    pkt.finalize(true);
    room->send_to_pvp(pkt);
}

// OnEndPvPResult ORIG T 0x8566694c 报告 OnEndPvPResult.md
void CFairMatch::OnEndPvPResult(PvP_Room* room, CUser* user)
{
    const int seat = room->get_user_seat(user);
    if (seat == -1) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x10b3, "error get_user_seat");
        return;
    }
    {
        CSwitchLog log(
            "virtual void fair_pvp::CFairMatch::OnEndPvPResult(PvP_Room*, CUser*)",
            0x10b8, 0, 0);
        log("pvp@log %s,%s,room(%d),%d,%d",
            user->get_acc_name(), user->getCurCharacName(),
            room->get_index(), room->get_pvp_battle_mode(),
            room->get_waiter_count());
    }
    const int teamOf = room->get_team(seat);
    if (teamOf == 0xfe) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x10be, "error get_team");
        return;
    }
    const int myTeam = _FindTeam(user);
    if (myTeam == 2) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x10c5, "error _FindTeam");
        return;
    }
    if (m_teams[myTeam].m_field04 == 2)
        return;                                   // 双方均已表决

    const ENUM_SCHOOL_MATCH_TEAM other =
        _GetOtherTeam(static_cast<ENUM_SCHOOL_MATCH_TEAM>(myTeam));
    if (other == static_cast<ENUM_SCHOOL_MATCH_TEAM>(2)) {
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x10d2, "error _GetOtherTeam");
        return;
    }

    room->set_state(1);   // ORIG PvP_Room::set_state(room,1)
    for (int i = 0; i < 4; ++i) {
        PlayerInfoThird& s = m_teams[myTeam].m_players[i];
        if (s.m_flag != 0 && s.m_user != 0)
            s.m_user->set_state(ch_state6);       // ORIG set_state(user,6)
    }
    if (m_teams[myTeam].m_field04 == 1) {
        // 已请求返回：把对方队伍整体搬回自建房并广播离场包
        const int oppo = _GetOppositeTeam(teamOf);
        PvP_Room* kept = _ReturnTeam(room, oppo, 0);
        if (kept != 0) {
            PacketGuard pkt;
            pkt.put_header(0, 0x13b);
            pkt.finalize(true);
            kept->send_to_pvp(pkt);
        }
    } else if (m_teams[myTeam].m_field04 == 0) {
        _PullOutRoomTeam(room,
                         static_cast<ENUM_SCHOOL_MATCH_TEAM>(myTeam));
    }
    room->set_state(2);
    if (teamOf == 1) {
        std::swap(m_teams[0], m_teams[1]);        // ORIG std::swap<TeamInfoThird>
    }
}

// ===================================================================== //
// 载入 / P2P 组
// ===================================================================== //

// OnCompleteLoad ORIG T 0x8564962 报告 OnCompleteLoad.md（msg 0x116）
void CFairMatch::OnCompleteLoad(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(0, 0x116);
    pkt.put_short(static_cast<unsigned short>(user->get_unique_id() & 0xffff));
    pkt.finalize(true);
    for (int t = 0; t < 2; ++t) {
        for (int i = 0; i < 4; ++i) {
            PlayerInfoThird& s = m_teams[t].m_players[i];
            if (s.m_flag != 0 && s.m_user != 0)
                s.m_user->Send(pkt);
        }
    }
}

// OnConnectP2PTimeout ORIG T 0x8564d3a 报告 OnConnectP2PTimeout.md
void CFairMatch::OnConnectP2PTimeout(PvP_Room* room)
{
    if (m_field04 == 4) {
        _SendMatchStatePacket(4, 2);
        const int mgrTeam = room->get_manager_team();
        _ReturnTeam(room, mgrTeam, 0);
        if (mgrTeam != 1) {
            unsigned* dst = reinterpret_cast<unsigned*>(&m_teams[0]);
            const unsigned* src =
                reinterpret_cast<const unsigned*>(&m_teams[1]);
            for (int n = 0x12; n != 0; --n) *dst++ = *src++;
        }
        m_teams[0].refresh();
        m_teams[1].reset();
    }
}

// OnConnectP2P ORIG T 0x8564a8e 报告 OnConnectP2P.md
// addr 为座位连接标志数组（按序逐座非空即已连接）
void CFairMatch::OnConnectP2P(CUser* user, PvP_Room* room,
                              char* addr, int port)
{
    (void)user;
    if (m_field04 != 4) {
        _SendMatchStatePacket(4, 2);
        m_field04 = 0;
        const int mgrTeam = room->get_manager_team();
        _ReturnTeam(room, mgrTeam, 0);
        return;
    }

    int i = 0;
    while (true) {
        const bool cont = (i < 8) && (i < port);
        if (!cont) break;
        if (addr[i] == '\0') break;
        ++i;
    }
    if ((i < 8) && (i < port)) {
        // 未全部连上：发失败态并直接走 +0x40 兜底虚槽（OnEndFight）
        _SendMatchStatePacket(4, 2);
        OnEndFight(room);
        return;
    }

    ++m_fieldAC;
    if (m_fieldAC != static_cast<int>((GetPlayCountPerOneTeam() & 0xff) * 2))
        return;

    PacketGuard pkt;
    pkt.put_header(0, 0x117);
    pkt.finalize(true);
    for (int t = 0; t < 2; ++t) {
        for (int p = 0; p < 4; ++p) {
            PlayerInfoThird& s = m_teams[t].m_players[p];
            if (s.m_flag != 0 && s.m_user != 0)
                s.m_user->Send(pkt);
        }
    }
    pkt.clear();
    room->battle_reset();
    for (int seat = 0; seat <= 7; ++seat) {
        if (room->get_user_seat(seat) != 0) {
            CRelayBattleMgr& relay = room->m_relayMgr;   // PvP_Room +0x620
            relay.OnReady(seat, true);
        }
    }
    room->start_pvp();
    room->make_state_info(reinterpret_cast<char*>(&pkt));
    G_GameWorld()->send_all(pkt);
}


// ORIG 反汇编实证：teamNo==1 分支 GetTeamRating 用 this+0x64（m_teams[1]），
// 队员循环读 this+0x24+i*0x10（m_teams[0]）；else 分支相反（0x1c / 0x6c）。
// BLOCKED 局部依赖：CEloRating::calc 本体与 PvPGlobalEnvironmentVariable::
// school_match_elo_k_ 定义尚未由任何 TU 提供（仅 ORIG 符号）。
void CFairMatch::_UpdateRating(int teamNo, ENUM_ELO_RATING_SYSTEM system)
{
    float oppositeRating = 0.0f;
    float memberRating = 0.0f;
    CUser* member = 0;
    if (teamNo == 1) {
        oppositeRating = static_cast<float>(m_teams[1].GetTeamRating());
        for (int i = 0; i < 4; ++i) {
            PlayerInfoThird& s = m_teams[0].m_players[i];
            if (s.m_flag != 0 && s.m_user != 0) {
                member = s.m_user;
                memberRating = s.m_rating;
                CEloRating::calc(memberRating, oppositeRating, system,
                                 PvPGlobalEnvironmentVariable::school_match_elo_k_);
                member->UpdateRating(memberRating);
            }
        }
    } else {
        oppositeRating = static_cast<float>(m_teams[0].GetTeamRating());
        for (int i = 0; i < 4; ++i) {
            PlayerInfoThird& s = m_teams[1].m_players[i];
            if (s.m_flag != 0 && s.m_user != 0) {
                member = s.m_user;
                memberRating = s.m_rating;
                CEloRating::calc(memberRating, oppositeRating, system,
                                 PvPGlobalEnvironmentVariable::school_match_elo_k_);
                member->UpdateRating(memberRating);
            }
        }
    }
}
