// ============================================================================
// df_game_r 还原 —— CLeagueMatch 实现（学院联赛匹配）
// 依据 docs/class_func_reports/CLeagueMatch/ 全部报告逐函数转写；TeamInfo /
// PlayerInfo 依据 docs/class_func_reports/TeamInfo/ 报告与 ORIG objdump。
// Packet_PvPResult 的 ORIG ctor（W 0x08568dc6，PacketHeader(this,0xfa5,0x80)）
// 由 SigTypes 域提供（当前仅有 m_pad[0x80] 形态）——此处按 objdump 手动展开
// 头部字段并注明 UNLINKED_PRODUCER。
// ============================================================================

#include "CLeagueMatch.h"
#include <algorithm>
#include <string.h>

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PvP_Room.h"
#include "CGameManager.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GameTypes.h"                  // cMyTrace
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CSchoolServerProxy.h"
#include "SchoolMatchEnums.h"           // ENUM_ELO_RATING_SYSTEM
#include "OnlinePreliminary.h"          // CEloRating / PvPGlobalEnvironmentVariable
#include "RDARScriptStringManager.h"
#include "TimerDispatcher.h"            // TimerStartMatching
#include "CInventory.h"                 // OS_API::snprintf

namespace {

// GetGap 排位增量表：ORIG 各 TU 本地 _ZL13g_RankingIncr（16 字节，
// df_game_r nm 显示 15 份内容一致）＝ {1.0f, 1.0f, 1.0f, 1.0f}。
const float g_RankingIncr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

inline void MakePacketHeader10(char* obj, unsigned short id, unsigned short len)
{
    // ORIG _ZN12PacketHeaderC1Ett @0x08110b70：+0 u16 / +2 u16 / +4 u16=0 / +6 int=0
    *reinterpret_cast<unsigned short*>(obj + 0x00) = id;
    *reinterpret_cast<unsigned short*>(obj + 0x02) = len;
    *reinterpret_cast<unsigned short*>(obj + 0x04) = 0;
    *reinterpret_cast<int*>(obj + 0x06) = 0;
}

}  // namespace

CLeagueMatchRoomList* CLeagueMatch::pWaitingRoomList_ = 0;   // B 0x0943e064
int CLeagueMatch::nWaitingRoomListRefCount_ = 0;             // B 0x0943e068
int CLeagueMatch::nextAuthValue_ = 0;                        // B 0x0943e06c
bool CLeagueMatch::bMatchable_ = false;                      // B 0x0943e060
unsigned char CLeagueMatch::nBattleMode_ = 3;                // D 0x093abb75 初值 3
unsigned char CLeagueMatch::nPlayerCountPerOneTeam_ = 2;     // D 0x093abb74 初值 2

// ---------------------------------------------------------------------------
// PlayerInfo / TeamInfo（报告 TeamInfo/*.md + ORIG objdump 转写）
// ---------------------------------------------------------------------------

void PlayerInfo::reset()    // ORIG W 0x085687d4
{
    m_accId = 0;
    m_rating = 0.0f;
    m_user = 0;
    m_flag = 0;
    m_schoolNo = 0;
    for (int i = 0; i < 3; ++i)
        m_badges[i] = 0;
}

bool CompareRating(PlayerInfo const& a, PlayerInfo const& b)  // ORIG T 0x0855c952
{
    if (a.m_flag > b.m_flag) return true;
    if (a.m_flag != b.m_flag) return false;
    return a.m_rating > b.m_rating;
}

void TeamInfo::reset()      // ORIG T 0x0855cb16
{
    for (int i = 0; i <= 3; ++i)
        m_players[i].reset();
}

void TeamInfo::refresh()    // ORIG T 0x0855c9ae
{
    for (int i = 0; i <= 3; ++i) {
        PlayerInfo& pi = m_players[i];
        if (pi.m_flag != 0) {
            pi.m_accId = pi.m_user->get_acc_id();
            pi.m_rating = pi.m_user->GetRating();
            pi.m_schoolNo = pi.m_user->GetSchoolNo();
            pi.m_user->GetDailyBadge(pi.m_badges);
        }
    }
}

int TeamInfo::add(CUser* user)   // ORIG T 0x0855cb54
{
    for (int i = 0; i <= 3; ++i) {
        PlayerInfo& pi = m_players[i];
        if (pi.m_flag != 0)
            continue;
        pi.m_flag = 1;
        pi.m_user = user;
        pi.m_accId = user->get_acc_id();
        pi.m_rating = user->GetRating();
        pi.m_schoolNo = user->GetSchoolNo();
        user->GetDailyBadge(pi.m_badges);
        return 1;
    }
    return 0;
}

int TeamInfo::del(CUser* user)   // ORIG T 0x0855cc9c
{
    for (int i = 0; i <= 3; ++i) {
        if (m_players[i].m_flag != 0 && m_players[i].m_user == user) {
            m_players[i].m_flag = 0;
            return 1;
        }
    }
    return 0;
}

int TeamInfo::size() const       // ORIG T 0x0855cd26
{
    int n = 0;
    for (int i = 0; i <= 3; ++i)
        if (m_players[i].m_flag != 0)
            ++n;
    return n;
}

// ---------------------------------------------------------------------------
// 生命周期
// ---------------------------------------------------------------------------

// ORIG C1/C2 0x0855cd74：IMatch ctor → PVP_ROOM_INFO@+8 → 懒建 pWaitingRoomList_
// （new 0x1c 置零后 C1 0x08568fb0）→ 引用计数自增。
CLeagueMatch::CLeagueMatch()
{
    if (pWaitingRoomList_ == 0)
        pWaitingRoomList_ = new CLeagueMatchRoomList();
    ++nWaitingRoomListRefCount_;
}

// ORIG D1 0x0855ce4c：引用计数递减，归零且单例仍在时 operator delete（不析构、
// 不置空）；随后 PVP_ROOM_INFO / IMatch 析构由编译器生成。
CLeagueMatch::~CLeagueMatch()
{
    --nWaitingRoomListRefCount_;
    if (pWaitingRoomList_ != 0 && nWaitingRoomListRefCount_ == 0)
        ::operator delete(pWaitingRoomList_);
}

// ---------------------------------------------------------------------------
// IMatch 覆盖
// ---------------------------------------------------------------------------

float CLeagueMatch::GetGap(IMatch* other)     // ORIG T 0x0855e12a
{
    float gap = 0.0f;
    for (unsigned int i = 0; i < (unsigned int)nPlayerCountPerOneTeam_; ++i) {
        CUser* mine = m_teamA.m_players[i].m_user;
        CUser* theirs = other->GetUser(i);
        if (m_teamA.m_players[i].m_flag == 0)
            continue;
        if (mine == 0)
            continue;
        if (theirs == 0)
            continue;
        float a = mine->GetRating() * g_RankingIncr[i];
        float b = theirs->GetRating() * g_RankingIncr[i];
        gap += __builtin_fabsf(a - b);
    }
    return gap;
}

CUser* CLeagueMatch::GetUser(unsigned int idx)   // ORIG T 0x0855e5e4
{
    if (idx > 3)
        return 0;
    if (m_teamA.m_players[idx].m_flag == 0)
        return 0;
    return m_teamA.m_players[idx].m_user;
}

void CLeagueMatch::GetUserList(void* out)     // ORIG T 0x0855e642（拷贝第一队 0x70 字节，P83 rep movsl）
{
    *reinterpret_cast<TeamInfo*>(out) = m_teamA;
}

void CLeagueMatch::RequestMap(short& mapNo, PVP_BATTLE_MODE& mode)  // ORIG T 0x0855d096
{
    mapNo = 3;
    if ((int)mode != 6)
        mode = static_cast<PVP_BATTLE_MODE>(nBattleMode_);
}

int CLeagueMatch::GetExtraWinPoint(int extra)   // ORIG T 0x0855d0bc
{
    // 活动期窗口（0x4911b4f0, 0x492429ef]，期内返回额外分否则 0。
    // ORIG 两次调用 getCurSec（P48：O0 无 CSE，条件双臂各调一次）。
    if (GlobalData::s_systemTime_.getCurSec() > static_cast<int>(0x4911b4f0) &&
        GlobalData::s_systemTime_.getCurSec() <= static_cast<int>(0x492429ef))
        return extra;
    return 0;
}

int CLeagueMatch::OnCreateRoom(CUser* user)    // ORIG T 0x0855d104
{
    nextAuthValue_ += 1;
    m_authValue = nextAuthValue_;
    m_state = 0;
    m_teamB.reset();
    m_teamA.reset();
    m_teamA.add(user);
    return 0;
}

void CLeagueMatch::OnDestroyRoom(CUser* /*user*/)   // ORIG T 0x0855d168
{
    m_state = 0;
    pWaitingRoomList_->DelRoom(this);               // IRoomList vtable+0x04
}

void CLeagueMatch::OnJoinRoom(CUser* user)      // ORIG T 0x0855d198
{
    if (m_state != 3)
        m_teamA.add(user);
}

// ORIG T 0x0855d1c0
void CLeagueMatch::OnLeaveRoom(CUser* user, PvP_Room* room, int team, int flag)
{
    if (m_state == 4) {
        _SendMatchStatePacket(4, 2);
        m_state = 0;
        int managerTeam = room->get_manager_team();
        _ReturnTeam(room, managerTeam, user);
    } else if (m_state == 5) {
        int idx;
        float selfRating = 0.0f;
        float oppoRating = 0.0f;
        for (idx = 0; idx < 4; ++idx) {
            if (flag == 1) {
                if (m_teamA.m_players[idx].m_user == user) {
                    selfRating = *(float*)(reinterpret_cast<char*>(this) +
                                           idx * 0x1c + 0x20);
                    oppoRating = *(float*)(reinterpret_cast<char*>(this) +
                                           idx * 0x1c + 0x90);
                    break;
                }
            } else if (m_teamB.m_players[idx].m_user == user) {
                selfRating = *(float*)(reinterpret_cast<char*>(this) +
                                       idx * 0x1c + 0x90);
                oppoRating = *(float*)(reinterpret_cast<char*>(this) +
                                       idx * 0x1c + 0x20);
                break;
            }
        }
        if (idx <= 3) {
            CEloRating::calc(selfRating, oppoRating,
                             static_cast<ENUM_ELO_RATING_SYSTEM>(0),
                             PvPGlobalEnvironmentVariable::school_match_elo_k_);
            user->UpdateRating(selfRating);
        }
    } else {
        m_state = 0;
    }

    if (flag == 1)
        m_teamA.del(user);
    else
        m_teamB.del(user);

    pWaitingRoomList_->DelRoom(this);               // IRoomList vtable+0x04
}

// ORIG T 0x0855db58。基类虚槽声明为 void，ORIG 返回 bool 经 eax 回传【推断】。
void CLeagueMatch::OnRegisterMatch(CUser* user, PvP_Room* room)
{
    if (m_state == 1)
        return;

    if (!bMatchable_) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199), 0x13);
        return;
    }

    int err = _CheckMatchable(room);
    if (err != 0) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199),
                                 static_cast<unsigned char>(err & 0xff));
        return;
    }

    if (m_state != 2)
        _RegisterWaitingRoom(room);

    m_state = 1;
    long auth = m_authValue;
    int roomIdx = room->get_index();
    TimerStartMatching::registNextTimer(10, roomIdx, static_cast<int>(auth));
    _SendMatchStatePacket(0, 1);
}

// ORIG T 0x0855dc64。基类虚槽为 void，ORIG 返回 bool【推断】。
void CLeagueMatch::OnMatchRoom(PvP_Room* room, int authValue)
{
    if (m_authValue != authValue) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x295, 0);
        trace("OnMatchRoom timeout");
        return;
    }
    if (m_state != 1)
        return;

    IMatch* opponent = 0;
    if (!(pWaitingRoomList_ != 0 && pWaitingRoomList_->Match(this, &opponent)) ||
        opponent == 0) {
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return;
    }

    CUser* leader = opponent->GetUser(0);
    if (leader == 0) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2b0, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b0);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return;
    }

    PvP_Room* theirRoom = reinterpret_cast<PvP_Room*>(leader->GetPVPRoom());
    if (theirRoom == 0) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2b9, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b9);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return;
    }

    if (theirRoom->get_player_count() > (int)nPlayerCountPerOneTeam_) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2c2, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x2c2);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return;
    }

    bool badSize =
        ((int)nPlayerCountPerOneTeam_ < m_teamA.size()) ||
        ((int)nPlayerCountPerOneTeam_ < theirRoom->get_player_count());
    if (badSize) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2ca, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x2ca);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return;
    }

    _BackupRoomInfo(theirRoom);
    m_teamB.reset();
    theirRoom->get_user_list(&m_teamB);
    _SendMatchStatePacket(3, 2);
    m_state = 3;
    for (unsigned int seat = 0; seat <= 7; ++seat) {     // setbe 无符号 <=7
        CUser* member = reinterpret_cast<CUser*>(theirRoom->get_user_seat(static_cast<int>(seat)));
        if (member == 0)
            continue;
        int theirTeam = theirRoom->get_team(static_cast<int>(seat));
        _PullOutRoom(member, theirRoom, static_cast<int>(seat));
        int newSeat = _PutInRoom(member, room, false);
        if (theirTeam == 3)
            room->OnSetPVPSeatState(member, newSeat,
                                    static_cast<ENUM_SEAT_STATE>(3),
                                    static_cast<ENUM_WALKOUT_TYPE>(2));
    }
    pWaitingRoomList_->DelRoom(this);                   // IRoomList vtable+0x04
    m_state = 4;
    _SendMatchStatePacket(2, 2);
}

void CLeagueMatch::OnStartFight()           // ORIG T 0x0855e21c
{
    m_state = 5;
}

// ORIG T 0x0855e55a
void CLeagueMatch::OnEndFight(PvP_Room* room)
{
    int managerTeam = room->get_manager_team();
    m_state = 0;
    _ReturnTeam(room, managerTeam, 0);
    if (managerTeam != 1)
        m_teamA = m_teamB;   // ORIG rep movsl 0x1c dwords（P83 结构整体赋值）
    m_teamA.refresh();
    m_teamB.reset();
}

// ORIG T 0x0855e4c4。IMatch 虚槽 void；ORIG 返回 1【推断】。
void CLeagueMatch::OnWalkOut(PvP_Room* room, int seat)
{
    int team = room->get_team(seat);
    CUser* user = reinterpret_cast<CUser*>(room->get_user_seat(seat));
    OnLeaveRoom(user, room, team, seat);                // vtable+0x2c 四参形态
}

// ORIG T 0x0855e52a。ORIG 返回 bool（!ready && state==4 → 0）。
bool CLeagueMatch::OnReady(CUser* /*user*/, bool ready)
{
    if (!ready && m_state == 4)
        return false;
    return true;
}

// ORIG T 0x0855eb22（结算奖励全流程）
void CLeagueMatch::OnReward(PvP_Room* /*room*/, int winTeam, bool draw)
{
    // ---- 结果包（Packet_PvPResult 0x80 内部布局全部依 asm 偏移公式转写；
    //       UNLINKED_PRODUCER: Packet_PvPResult::Packet_PvPResult W 0x08568dc6
    //       未建模，此处手动展开 PacketHeader(this,0xfa5,0x80)） ----
    char pkt[0x80];
    MakePacketHeader10(pkt, 0x0fa5, 0x80);

    int   eloK2       = PvPGlobalEnvironmentVariable::school_match_elo_k2_;  // pkt+0x0a
    char  winType     = static_cast<char>(draw ? 1 : (winTeam == 1 ? 2 : 0));  // pkt+0x0e
    char  playCount   = static_cast<char>(nPlayerCountPerOneTeam_);            // pkt+0x0f
    // 字节标记区公共寻址（ORIG asm 统一公式）：
    //   flagAddr(i,j,t) = pkt + 0x1c + i + 3*j + 56*t
    // 族含义：i=0 → addBadge 输出旗标 / i=1、2 → 蓝、绿新获旗标；
    // 徽章计数字节回填亦落此区（GetDailyBadge 低字节）。
    unsigned int winFlagLocal = 0;
    (void)winFlagLocal;

    // 1) 初始：两支队伍每日徽章快照写 flagAddr(0,j,t)，同址旗标清零。
    for (int t = 0; t <= 1; ++t) {
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            short badge =
                *(unsigned short*)(reinterpret_cast<char*>(this) +
                                   j * 0x1c + t * 0x70 + 0x14);
            *(char*)(pkt + 0x1c + j * 3 + t * 56) = static_cast<char>(badge);
            *(char*)(pkt + 0x1d + j * 3 + t * 56) = 0;
            *(char*)(pkt + 0x1e + j * 3 + t * 56) = 0;
        }
    }

    // 2) 各分支记徽章（draw→type1 / A 队胜→type2 / 其余→type0）。
    int type = 0;
    if (draw) {
        type = 1;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            *(char*)(pkt + 0x1d + j * 3) = 0;   // 自队蓝章清零
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2), reinterpret_cast<unsigned char*>(
                         pkt + 0x1e + j * 3), false);
            *(char*)(pkt + 0x1c + 56 + j * 3) = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2), reinterpret_cast<unsigned char*>(
                         pkt + 0x1d + 56 + j * 3), false);
        }
    } else if (winTeam == 1) {
        type = 2;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(1), reinterpret_cast<unsigned char*>(
                         pkt + 0x1d + j * 3), false);
            *(char*)(pkt + 0x1e + j * 3) = 0;
            *(char*)(pkt + 0x1c + 56 + j * 3) = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2), reinterpret_cast<unsigned char*>(
                         pkt + 0x1d + 56 + j * 3), false);
        }
    } else {
        type = 0;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            *(char*)(pkt + 0x1d + j * 3) = 0;
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2), reinterpret_cast<unsigned char*>(
                         pkt + 0x1e + j * 3), false);
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(1), reinterpret_cast<unsigned char*>(
                         pkt + 0x1c + 56 + j * 3), false);
            *(char*)(pkt + 0x1d + 56 + j * 3) = 0;
        }
    }

    // 写包头数据字段（asm 0855efa6..0855efc1：elo_k2_@+0x0a、winType@+0x0e、
    // playCount@+0x0f —— 上面语义分配占位时已映射）。
    *(reinterpret_cast<int*>(pkt + 0x0a)) = eloK2;
    pkt[0x0e] = winType;
    pkt[0x0f] = playCount;

    // 3) 两队 schoolNo 表（intsBase=pkt+8，elem=(t*14+i+8)，per-team stride 56B）与
    //    school 排他检查（同校不算对手）及绿章提示。
    for (int t = 0; t <= 1; ++t) {
        TeamInfo& ti = (t == 0) ? m_teamA : m_teamB;
        for (int i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            *reinterpret_cast<unsigned int*>(pkt + 8 + (t * 14 + i + 8) * 4) =
                ti.m_players[i].m_schoolNo;
        }
        for (int i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            unsigned int mySchool =
                *reinterpret_cast<unsigned int*>(pkt + 8 + (t * 14 + i + 8) * 4);
            unsigned int* schools = reinterpret_cast<unsigned int*>(pkt + 8 + t * 56 + 32);
            if (!_getSameSchool(schools, i,
                                (unsigned int)nPlayerCountPerOneTeam_, mySchool)) {
                *(char*)(pkt + 0x1c + i * 3 + t * 56) = 0;
                if (*(pkt + 0x1d + i * 3 + t * 56) != 0 ||
                    *(pkt + 0x1e + i * 3 + t * 56) != 0) {
                    PlayerInfo& pi = ti.m_players[i];
                    if (pi.m_flag == 1 && pi.m_user != 0 &&
                        pi.m_user->getCurCharacR() != 0) {
                        char textBuf[0x100] = { '\0' };   // P4：ORIG rep stos 内联清零
                        const char* fmt = g_scriptStringManager_.findString(
                            4, "school_green_badge_msg", (bool*)0);
                        OS_API::snprintf(textBuf, 0xff, fmt, 1);
                        PacketGuard pg;
                        pg.put_header(0, 0xc);
                        pg.put_byte(0);
                        pg.put_short(0);
                        pg.put_byte(0);
                        pg.put_int((int)strlen(textBuf));
                        pg.put_str(textBuf, (int)strlen(textBuf));
                        pg.finalize(true);
                        pi.m_user->Send(pg);
                    }
                }
            } else if (*(pkt + 0x1d + i * 3 + t * 56) == 0 &&
                       *(pkt + 0x1e + i * 3 + t * 56) == 0) {
                addBadge(&ti.m_players[i], ENUM_BADGE_TYPE_0,
                         reinterpret_cast<unsigned char*>(pkt + 0x1c + i * 3 + t * 56),
                         false);
            } else {
                addBadge(&ti.m_players[i], ENUM_BADGE_TYPE_0,
                         reinterpret_cast<unsigned char*>(pkt + 0x1c + i * 3 + t * 56),
                         true);
            }
        }
        // 回填实时徽章计数（条件限定第二队 ASM 寻址 this+i*0x1c+0x98/+0x94）。
        for (int i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            PlayerInfo& pi = (t == 0) ? m_teamA.m_players[i] : m_teamB.m_players[i];
            if (pi.m_user == 0)
                continue;
            if (t == 1) {  // asm 仅在 team==1 分支检查 B 队旗标
                if (m_teamB.m_players[i].m_flag == 0 ||
                    m_teamB.m_players[i].m_user == 0)
                    continue;
            }
            for (int b = 0; b < 3; ++b) {
                short v = pi.m_user->GetDailyBadge(static_cast<ENUM_BADGE_TYPE>(b));
                *(char*)(pkt + 0x1c + b + i * 3 + t * 56) = static_cast<char>(v);
            }
        }
    }

    // 4) Elo 结算并回写个人分 + accId 表（elem=(t*14+i+12)）。
    float ratings[2] = { 0.0f, 0.0f };
    for (unsigned int i = 0; i < (unsigned int)nPlayerCountPerOneTeam_; ++i) {
        for (int t = 0; t <= 1; ++t) {
            TeamInfo& ti = (t == 0) ? m_teamA : m_teamB;
            ratings[t] = *(float*)(reinterpret_cast<char*>(&ti) + i * 0x1c + 4);
        }
        CEloRating::calc(ratings[0], ratings[1],
                         static_cast<ENUM_ELO_RATING_SYSTEM>(type),
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        for (int t = 0; t <= 1; ++t) {
            TeamInfo& ti = (t == 0) ? m_teamA : m_teamB;
            if (ti.m_players[i].m_flag != 0 && ti.m_players[i].m_user != 0)
                ti.m_players[i].m_user->UpdateRating(ratings[t]);
            *reinterpret_cast<unsigned int*>(pkt + 8 + (t * 14 + i + 12) * 4) =
                ti.m_players[i].m_accId;
        }
    }

    // 5) 上报：单人走消息队列流（0x136/-1 + 二进制），多人直发学校服务器代理。
    if (nPlayerCountPerOneTeam_ == 1) {
        Stream* st = GlobalData::s_stream_pool->Acquire("MatchingSystem.cpp", 0x4a8);
        CStreamGuard guard(st, true);
        (*&guard).operator<<(0x136);
        (*&guard).operator<<(-1);
        guard.operator->()->put_binary(pkt, 0x80);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    } else {
        GlobalData::s_school_proxy->SendPacket(pkt, 0x80);
    }
}

// ORIG T 0x0855e22c：向全体成员广播加载完成通知（0x81 包，携带 unique id）。
void CLeagueMatch::OnCompleteLoad(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(0, 0x81);
    pkt.put_short(user->get_unique_id() & 0xffff);
    pkt.finalize(true);
    for (int t = 0; t <= 1; ++t) {
        TeamInfo& ti = (t == 0) ? m_teamA : m_teamB;
        for (int i = 0; i < 4; ++i) {
            PlayerInfo const& pi = ti.m_players[i];
            if (pi.m_flag != 0 && pi.m_user != 0)
                pi.m_user->Send(pkt);
        }
    }
}

int CLeagueMatch::CheckJoinable(PvP_Room* room, CUser* user)   // ORIG T 0x0855cf8c
{
    if (room->get_player_count() >= (int)(nPlayerCountPerOneTeam_ * 2))
        return 4;
    if (m_teamA.size() + m_teamB.size() > (int)(nPlayerCountPerOneTeam_ * 2))
        return 4;

    switch (m_state) {
    case 0:
        return _CheckSchool(room, user) ? 0 : 4;
    case 3:
        return 0;
    default:
        return 4;
    }
}

int CLeagueMatch::CheckSeat(int seat)        // ORIG T 0x0855d058
{
    if (seat == 2) return 0;
    return 1;
}

bool CLeagueMatch::CheckMatchable(IMatch* other)   // ORIG T 0x0855fa14
{
    // ORIG 汇编对 other 直接按对象前部偏移读其第一队 PlayerInfo 槽
    // （this+slot*0x1c+0x24/0x28/0x2c），依赖 IMatch 派生类共享布局。
    int maxWinSelf = 0;
    int maxWinOther = 0;

    for (int i = 0; i < 4; ++i) {
        PlayerInfo const& pi = m_teamA.m_players[i];
        if (pi.m_flag == 0 || pi.m_user == 0)
            continue;
        int win = pi.m_user->getPVPResultR()->m_winPoint;
        if (maxWinSelf <= win)
            maxWinSelf = win;
    }
    for (int i = 0; i < 4; ++i) {
        PlayerInfo const& pi =
            *reinterpret_cast<PlayerInfo const*>(reinterpret_cast<char*>(other) +
                                                 0x1c + i * 0x1c);
        if (pi.m_flag == 0 || pi.m_user == 0)
            continue;
        int win = pi.m_user->getPVPResultR()->m_winPoint;
        if (maxWinOther <= win)
            maxWinOther = win;
    }

    if ((maxWinSelf >= 12 && maxWinSelf <= 14) && maxWinOther > 10)
        return false;
    if ((maxWinOther >= 12 && maxWinOther <= 14) && maxWinSelf > 10)
        return false;
    if (((maxWinSelf - maxWinOther) ^ ((maxWinSelf - maxWinOther) >> 31)) -
        ((maxWinSelf - maxWinOther) >> 31) > 4)
        return false;

    for (int a = 0; a < 4; ++a) {
        for (int b = 0; b < 4; ++b) {
            PlayerInfo const& pa = m_teamA.m_players[a];
            PlayerInfo const& pb =
                *reinterpret_cast<PlayerInfo const*>(reinterpret_cast<char*>(other) +
                                                     0x1c + b * 0x1c);
            if (pa.m_flag != 0 && pb.m_flag != 0 &&
                pa.m_schoolNo == pb.m_schoolNo)
                return false;
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// 非 IMatch 方法
// ---------------------------------------------------------------------------

void CLeagueMatch::SetMatchAble(bool able)      // ORIG T 0x0855d07e
{
    bMatchable_ = able;
}

void CLeagueMatch::SetPlayCountPerOneTeam(unsigned int n)   // ORIG T 0x0855d070
{
    nPlayerCountPerOneTeam_ = static_cast<unsigned char>(n);
}

// ORIG 虚槽布局：GetSchoolNo 为派生新增第 37 槽（ORIG vtable +0x90），
// GetIndentification 的覆盖体即经 vtable+0x90 虚调本函数。
int CLeagueMatch::GetSchoolNo()              // ORIG T 0x0855f9aa
{
    for (int i = 0; i <= 3; ++i) {
        if (m_teamA.m_players[i].m_flag != 0)
            return m_teamA.m_players[i].m_schoolNo;
    }
    return 0;
}

// ORIG W 0x08568834：call *(vptr+0x90) 并返回其值 ≡ GetSchoolNo()。
int CLeagueMatch::GetIndentification()
{
    return GetSchoolNo();
}

unsigned char CLeagueMatch::GetBattleMode()  // ORIG W 0x084ed387：movzbl 直返全局字节 nBattleMode_
{
    return nBattleMode_;
}

void CLeagueMatch::SetBattleMode(int mode)   // ORIG W 0x082743a0：写全局字节 nBattleMode_
{
    nBattleMode_ = static_cast<unsigned char>(mode);
}

unsigned char CLeagueMatch::GetPlayCountPerOneTeam()  // ORIG W 0x084ed37b：movzbl 直返全局字节
{
    return nPlayerCountPerOneTeam_;
}

PvP_Room* CLeagueMatch::_MakeRoom(CUser* user)   // ORIG T 0x0855d572
{
    PacketGuard pkt;
    pkt.put_header(1, 0x35);
    PvP_Room* pvp = G_CGameManager()->GetPvp();
    if (pvp == 0) {
        pkt.put_byte(0);
        pkt.put_byte(4);
        pkt.finalize(true);
        user->Send(pkt);
        return 0;
    }

    std::string const& passwd = m_roomInfo.passwd;
    std::string const& title = m_roomInfo.title;
    int err = pvp->create_room(user, m_roomInfo.byte4,
                               const_cast<char*>(title.c_str()), m_roomInfo.short6,
                               m_roomInfo.byte8 != 0, passwd.c_str(),
                               static_cast<int>(passwd.length()),
                               static_cast<PVP_BATTLE_MODE>(2));
    if (err >= 1) {
        pkt.put_byte(0);
        pkt.put_byte(err);
        pkt.finalize(true);
        user->Send(pkt);
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__, 0x1fe,
            "create_room fail: PVP슬롯반환 ID[%d]\n", pvp->get_index());
        G_CGameManager()->PutPvp(pvp);
        return 0;
    }

    pkt.clear();
    pkt.put_header(0, 2);
    pkt.put_byte(1);
    pkt.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&pkt), 1);
    pkt.finalize(true);
    user->Send(pkt);

    pkt.clear();
    pkt.put_header(0, 0x29);
    pkt.put_short(1);
    pvp->make_room_info(reinterpret_cast<char*>(&pkt));
    pkt.finalize(true);
    G_GameWorld()->send_all(pkt);
    G_GameWorld()->goto_pvp(user);
    return pvp;
}

void CLeagueMatch::_RegisterWaitingRoom(PvP_Room* room)   // ORIG T 0x0855d884
{
    bool foundMin = false;
    float minRating = 0.0f;
    for (int i = 0; i < 4; ++i) {
        PlayerInfo& pi = m_teamA.m_players[i];
        if (pi.m_flag == 0)
            continue;
        if (!foundMin) {
            minRating = pi.m_rating;
            foundMin = true;
        }
        if (minRating < pi.m_rating)   // asm fucompp：小于最小值则更新
            minRating = pi.m_rating;
    }
    minRating -= 1.0f;

    for (int seat = 0; seat <= 7; ++seat) {
        if (room->get_team(seat) != 3)
            continue;
        CUser* seatUser = reinterpret_cast<CUser*>(room->get_user_seat(seat));
        for (int i = 0; i < 4; ++i) {
            PlayerInfo& pi = m_teamA.m_players[i];
            if (pi.m_user == seatUser && seatUser != 0)
                pi.m_rating = minRating;
        }
    }

    std::sort<PlayerInfo*, bool (*)(PlayerInfo const&, PlayerInfo const&)>(
        m_teamA.m_players, m_teamA.m_players + 4, CompareRating);   // ORIG weak sort 实例
    pWaitingRoomList_->AddRoom(this);                               // IRoomList vtable+0x00
}

void CLeagueMatch::_BackupRoomInfo(PvP_Room* room)     // ORIG T 0x0855d554
{
    room->get_room_info(&m_roomInfo);
}

int CLeagueMatch::_CheckMatchable(PvP_Room* room) const   // ORIG T 0x0855da56
{
    int emptySeats = 0;
    for (int i = 0; i <= 7; ++i) {
        if (room->get_team(i) == 3) {
            ++emptySeats;
            continue;
        }
        CUser* user = reinterpret_cast<CUser*>(room->get_user_seat(i));
        if (user == 0)
            continue;
        int school = user->GetSchoolNo();
        if (school == 0) {
            LogManager::logFormat(
                1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__, 599,
                "(%d) found a different school no(%d)", 0, user->GetSchoolNo());
            return 0x16;
        }
    }
    return (m_teamA.size() - emptySeats == (int)nPlayerCountPerOneTeam_) ? 0 : 0x16;
}

void CLeagueMatch::_PullOutRoom(CUser* user, PvP_Room* room, int seat)  // ORIG T 0x0855d3d2
{
    // OnSetPVPSeatState 返回 1 时成员留在房内。
    if (!room->OnSetPVPSeatState(user, seat,
                                 static_cast<ENUM_SEAT_STATE>(0xfe),
                                 static_cast<ENUM_WALKOUT_TYPE>(2))) {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
}

int CLeagueMatch::_PutInRoom(CUser* user, PvP_Room* room, bool observer)  // ORIG T 0x0855d42a
{
    PacketGuard pkt;
    int seatNo = 0;
    int err = room->join_room(user, seatNo, observer);
    if (err > 0) {
        pkt.put_header(1, 0x36);
        pkt.put_byte(0);
        pkt.put_byte(err);
        pkt.finalize(true);
        user->Send(pkt);
    } else {
        room->make_seat_info(reinterpret_cast<char*>(&pkt), seatNo);
        G_GameWorld()->send_all(pkt);
        G_GameWorld()->goto_pvp(user);
    }
    return seatNo;
}

void CLeagueMatch::_ReturnTeam(PvP_Room* room, int team, CUser* except)  // ORIG T 0x0855e378
{
    PvP_Room* madeRoom = 0;
    for (unsigned int seat = 0; seat <= 7; ++seat) {
        CUser* user = reinterpret_cast<CUser*>(room->get_user_seat(static_cast<int>(seat)));
        if (user == 0)
            continue;
        int seatTeam = room->get_team(static_cast<int>(seat));
        if (team == seatTeam) {
            if (team == 2)
                room->OnSetPVPSeatState(user, static_cast<int>(seat),
                                        static_cast<ENUM_SEAT_STATE>(1),
                                        static_cast<ENUM_WALKOUT_TYPE>(2));
        } else if (except == 0 || user != except) {
            _PullOutRoom(user, room, static_cast<int>(seat));
            if (madeRoom == 0)
                madeRoom = _MakeRoom(user);
            else
                _PutInRoom(user, madeRoom, true);
        }
    }
    m_state = 0;
}

void CLeagueMatch::_SendMatchStatePacket(int state, int teams)   // ORIG T 0x0855e664
{
    PacketGuard pkt;
    pkt.put_header(0, 0xaf);
    pkt.put_byte(state);
    pkt.finalize(true);
    for (int t = 0; t < teams; ++t) {
        for (int i = 0; i <= 3; ++i) {
            TeamInfo const& ti = (t == 0) ? m_teamA : m_teamB;
            if (ti.m_players[i].m_flag != 0 && ti.m_players[i].m_user != 0)
                ti.m_players[i].m_user->Send(pkt);
        }
    }
}

int CLeagueMatch::_getSameSchool(unsigned int* schools, int selfIdx,
                                 int count, unsigned int schoolNo)  // ORIG T 0x0855e7a8
{
    for (int i = 0; i < count; ++i) {
        if (i == selfIdx)
            continue;
        if (schools[i] == schoolNo)
            return 1;
    }
    return 0;
}

int CLeagueMatch::_GetOppositeTeam(register int team)  // ORIG T 0x0855e49e（P85 寄存器物化）
{
    if (team != 1) {
        if (team != 2) return team;
        return 1;
    }
    return 2;
}

int CLeagueMatch::_CheckSchool(PvP_Room* room, CUser* user)   // ORIG T 0x0855cef8
{
    if (user->GetSchoolNo() == 0)
        return 0;
    int emptySeats = 0;
    for (int i = 0; i <= 7; ++i) {
        if (room->get_team(i) == 3)
            ++emptySeats;
    }
    return (m_teamA.size() - emptySeats >= (int)nPlayerCountPerOneTeam_) ? 0 : 1;
}

int CLeagueMatch::addBadge(PlayerInfo* info, ENUM_BADGE_TYPE badgeType,
                           unsigned char* msgFlag, bool extra)   // ORIG T 0x0855e7ee
{
    do {
        if (info->m_flag != 1)
            break;
        if (info->m_user == 0)
            break;
        if (info->m_user->getCurCharacR() == 0)
            break;

        if (badgeType == 0) {
            if (info->m_user->GetDailyBadge(ENUM_BADGE_TYPE_0) > 9)
                break;
        } else {
            int sum = info->m_user->GetDailyBadge(static_cast<ENUM_BADGE_TYPE>(1)) +
                      info->m_user->GetDailyBadge(static_cast<ENUM_BADGE_TYPE>(2));
            if (sum > 9)
                break;
        }

        info->m_user->AddDailyBadge(badgeType);
        *msgFlag = 1;
        int count = (*msgFlag) + (extra ? 1 : 0);

        char textBuf[0x100] = { '\0' };   // P4：ORIG rep stos 内联清零
        const char* key = (badgeType == 1) ? "school_red_badge_msg"
                        : (badgeType == 2) ? "school_blue_badge_msg"
                                           : "school_green_badge_msg";
        const char* fmt = g_scriptStringManager_.findString(4, key, (bool*)0);
        OS_API::snprintf(textBuf, 0xff, fmt, count);

        PacketGuard pkt;
        pkt.put_header(0, 0xc);
        pkt.put_byte(0);
        pkt.put_short(0);
        pkt.put_byte(0);
        pkt.put_int((int)strlen(textBuf));
        pkt.put_str(textBuf, (int)strlen(textBuf));
        pkt.finalize(true);
        info->m_user->Send(pkt);
        return 1;
    } while (0);
    return 0;
}
