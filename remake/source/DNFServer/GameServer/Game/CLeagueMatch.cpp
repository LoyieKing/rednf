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
#include "Packet_PvPResult.h"           // 结果包 0x80 载荷具名布局（shared/packet/include）
#include "RDARScriptStringManager.h"
#include "TimerDispatcher.h"            // TimerStartMatching
#include "CInventory.h"                 // OS_API::snprintf

namespace {

// GetGap 排位增量表：ORIG 各 TU 本地 _ZL13g_RankingIncr（16 字节，
// df_game_r nm 显示 15 份内容一致）＝ {1.0f, 1.0f, 1.0f, 1.0f}。
const float g_RankingIncr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

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
        if (m_players[i].m_flag != 0) {
            m_players[i].m_accId = m_players[i].m_user->get_acc_id();
            m_players[i].m_rating = m_players[i].m_user->GetRating();
            m_players[i].m_schoolNo = m_players[i].m_user->GetSchoolNo();
            m_players[i].m_user->GetDailyBadge(m_players[i].m_badges);
        }
    }
}

int TeamInfo::add(CUser* user)   // ORIG T 0x0855cb54
{
    for (int i = 0; i <= 3; ++i) {
        if (m_players[i].m_flag != 0)
            continue;
        m_players[i].m_flag = 1;
        m_players[i].m_user = user;
        m_players[i].m_accId = user->get_acc_id();
        m_players[i].m_rating = user->GetRating();
        m_players[i].m_schoolNo = user->GetSchoolNo();
        user->GetDailyBadge(m_players[i].m_badges);
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
    /* ORIG @0855cd42 乘法链 lea(,%eax,4)/lea(,%edx,8)/sub %edx（PlayerInfo
     * stride 0x1c）；4.4 恒把 i*4 折成 shl $2（probe：显式 unsigned long
     * 两步 stride 同产 shl），指令选择成本模型差，N 类不可翻，保持具名
     * 字段下标访问【§4.3】 */
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
                    selfRating = m_teamA.m_players[idx].m_rating;
                    oppoRating = m_teamB.m_players[idx].m_rating;
                    break;
                }
            } else if (m_teamB.m_players[idx].m_user == user) {
                selfRating = m_teamB.m_players[idx].m_rating;
                oppoRating = m_teamA.m_players[idx].m_rating;
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

// ORIG T 0x0855db58。ORIG 返回 int：m_state==1 → 1；失败路径 → 0；成功 → 1。
int CLeagueMatch::OnRegisterMatch(CUser* user, PvP_Room* room)
{
    if (m_state == 1)
        return 1;

    if (!bMatchable_) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199), 0x13);
        return 0;
    }

    int err = _CheckMatchable(room);
    if (err != 0) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199),
                                 static_cast<unsigned char>(err & 0xff));
        return 0;
    }

    if (m_state != 2)
        _RegisterWaitingRoom(room);

    m_state = 1;
    long auth = m_authValue;
    int roomIdx = room->get_index();
    TimerStartMatching::registNextTimer(10, roomIdx, static_cast<int>(auth));
    _SendMatchStatePacket(0, 1);
    return 1;
}

// ORIG T 0x0855dc64。ORIG 返回 int：失败路径 → 0；成功 → 1。
int CLeagueMatch::OnMatchRoom(PvP_Room* room, int authValue)
{
    if (m_authValue != authValue) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x295, 0);
        trace("OnMatchRoom timeout");
        return 0;
    }
    if (m_state != 1)
        return 0;

    IMatch* opponent = 0;
    if (!(pWaitingRoomList_ != 0 && pWaitingRoomList_->Match(this, &opponent)) ||
        opponent == 0) {
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    CUser* leader = opponent->GetUser(0);
    if (leader == 0) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2b0, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b0);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    PvP_Room* theirRoom = reinterpret_cast<PvP_Room*>(leader->GetPVPRoom());
    if (theirRoom == 0) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2b9, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b9);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    if (theirRoom->get_player_count() > (int)nPlayerCountPerOneTeam_) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2c2, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x2c2);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    bool badSize =
        ((int)nPlayerCountPerOneTeam_ < m_teamA.size()) ||
        ((int)nPlayerCountPerOneTeam_ < theirRoom->get_player_count());
    if (badSize) {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2ca, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x2ca);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
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
    return 1;
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

// ORIG T 0x0855e4c4。ORIG 返回 1。
int CLeagueMatch::OnWalkOut(PvP_Room* room, int seat)
{
    int team = room->get_team(seat);
    CUser* user = reinterpret_cast<CUser*>(room->get_user_seat(seat));
    OnLeaveRoom(user, room, team, seat);                // vtable+0x2c 四参形态
    return 1;
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
    // ---- 结果包（ORIG 在 ebp-0xd8 调 Packet_PvPResult ctor W 0x08568dc6，
    //       该 weak ctor 未建符号 → 此处按 PacketHeader ctor 0x08110b70 的
    //       逐字段宽度（movw/movw/movw/movl）具名展开，再写载荷字段） ----
    char pkt[0x80];
    Packet_PvPResult* res = reinterpret_cast<Packet_PvPResult*>(pkt);
    res->packetId = 0x0fa5;
    res->packetSize = 0x80;
    res->reversed1 = 0;
    res->m_connNo = 0;

    int   eloK2       = PvPGlobalEnvironmentVariable::school_match_elo_k2_;  // m_eloK
    char  winType     = static_cast<char>(draw ? 1 : (winTeam == 1 ? 2 : 0));  // m_winType
    char  playCount   = static_cast<char>(nPlayerCountPerOneTeam_);            // m_playCount
    // ORIG asm 统一寻址公式：team 块基 = pkt + 0x10 + 56*t，块内
    //   徽章值 @ 0x00 + i + 3*j（`movzwl 0x10(%ebx,%eax,2)` 读 PlayerInfo
    //   m_badges[i] → `mov %bl,(%eax)` 单字节写）
    //   新获旗标 @ 0x0c + i + 3*j（`movb $0x0,0xc(%eax)`）
    // 即 Packet_PvPResult::Team::m_dailyBadge[j][i] / m_newFlag[j][i]。
    unsigned int winFlagLocal = 0;
    (void)winFlagLocal;

    // 1) 初始：两支队伍每日徽章快照写 m_dailyBadge[j][i]，同队旗标区清零。
    for (int t = 0; t <= 1; ++t) {
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            TeamInfo& ti = (t == 0) ? m_teamA : m_teamB;
            for (int i = 0; i <= 2; ++i) {
                res->m_team[t].m_dailyBadge[j][i] =
                    static_cast<char>(ti.m_players[j].m_badges[i]);
                res->m_team[t].m_newFlag[j][i] = 0;
            }
        }
    }

    // 2) 各分支记徽章（draw→type1 / A 队胜→type2 / 其余→type0）。
    int type = 0;
    if (draw) {
        type = 1;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            res->m_team[0].m_newFlag[j][1] = 0;   // 自队蓝章清零
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res->m_team[0].m_newFlag[j][2], false);
            res->m_team[1].m_newFlag[j][1] = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res->m_team[1].m_newFlag[j][2], false);
        }
    } else if (winTeam == 1) {
        type = 2;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(1),
                     &res->m_team[0].m_newFlag[j][1], false);
            res->m_team[0].m_newFlag[j][2] = 0;
            res->m_team[1].m_newFlag[j][1] = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res->m_team[1].m_newFlag[j][2], false);
        }
    } else {
        type = 0;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            res->m_team[0].m_newFlag[j][1] = 0;
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res->m_team[0].m_newFlag[j][2], false);
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(1),
                     &res->m_team[1].m_newFlag[j][1], false);
            res->m_team[1].m_newFlag[j][2] = 0;
        }
    }

    // 写包头数据字段（asm：elo_k2_@+0x0a 4B、winType@+0x0e 1B、playCount@+0x0f 1B）。
    res->m_eloK = eloK2;
    res->m_winType = winType;
    res->m_playCount = playCount;

    // 3) 两队 schoolNo 表（intsBase=pkt+8，elem=(t*14+i+8)，per-team stride 56B）与
    //    school 排他检查（同校不算对手）及绿章提示。
    for (int t = 0; t <= 1; ++t) {
        TeamInfo& ti = (t == 0) ? m_teamA : m_teamB;
        for (int i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            res->m_team[t].m_schoolNo[i] = ti.m_players[i].m_schoolNo;
        }
        for (int i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            unsigned int mySchool = res->m_team[t].m_schoolNo[i];
            unsigned int* schools = res->m_team[t].m_schoolNo;
            if (!_getSameSchool(schools, i,
                                (unsigned int)nPlayerCountPerOneTeam_, mySchool)) {
                res->m_team[t].m_newFlag[i][0] = 0;
                if (res->m_team[t].m_newFlag[i][1] != 0 ||
                    res->m_team[t].m_newFlag[i][2] != 0) {
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
            } else if (res->m_team[t].m_newFlag[i][1] == 0 &&
                       res->m_team[t].m_newFlag[i][2] == 0) {
                addBadge(&ti.m_players[i], ENUM_BADGE_TYPE_0,
                         &res->m_team[t].m_newFlag[i][0],
                         false);
            } else {
                addBadge(&ti.m_players[i], ENUM_BADGE_TYPE_0,
                         &res->m_team[t].m_newFlag[i][0],
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
                res->m_team[t].m_dailyBadge[i][b] = static_cast<char>(v);
            }
        }
    }

    // 4) Elo 结算并回写个人分 + accId 表（elem=(t*14+i+12)）。
    float ratings[2] = { 0.0f, 0.0f };
    for (unsigned int i = 0; i < (unsigned int)nPlayerCountPerOneTeam_; ++i) {
        for (int t = 0; t <= 1; ++t) {
            TeamInfo& ti = (t == 0) ? m_teamA : m_teamB;
            ratings[t] = ti.m_players[i].m_rating;
        }
        CEloRating::calc(ratings[0], ratings[1],
                         static_cast<ENUM_ELO_RATING_SYSTEM>(type),
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        for (int t = 0; t <= 1; ++t) {
            TeamInfo& ti = (t == 0) ? m_teamA : m_teamB;
            if (ti.m_players[i].m_flag != 0 && ti.m_players[i].m_user != 0)
                ti.m_players[i].m_user->UpdateRating(ratings[t]);
            res->m_team[t].m_accId[i] = ti.m_players[i].m_accId;
        }
    }

    // 5) 上报：单人走消息队列流（0x136/-1 + 二进制），多人直发学校服务器代理。
    if (nPlayerCountPerOneTeam_ == 1) {
        Stream* st = GlobalData::s_stream_pool->Acquire("MatchingSystem.cpp", 0x4a8);
        CStreamGuard guard(st, true);
        // ORIG 每次写经显式 operator*() 调用（同 TimerDispatcher 代理链形）；
        // 直 deref/shim 会丢 2 处 operator*() call（P167 缺失）。
        guard.operator*()->operator<<((int)0x136);
        guard.operator*()->operator<<((int)-1);
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
    case 0: {
        // X1: ORIG `call _CheckSchool; xor $1,%eax; test %al; je`（int 返回不自救不行，
        // 绑 %eax 具名 uchar + ^=1 复现单段 xor——技巧 22 精化）
        register unsigned char sc asm("eax") =
            (unsigned char)_CheckSchool(room, user);
        sc ^= 1;
        if (sc)
            return 4;
        return 0;
    }
    case 3:
        return 0;
    case 1:
    case 4:
    case 5:
        return 4;
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
    // ORIG 无引用缓存：每次成员访问都重算 base+i*0x1c（每处 7 条寻址指令），
    // 且胜点比较是 MAX 宏形态 —— getPVPResultR()->m_pvpGrade 被求值两次；
    // 差值判定用 abs()（单临时槽 sar/xor/sub 展开），不得手写三遍表达式。
    int maxWinSelf = 0;
    int maxWinOther = 0;

    for (int i = 0; i < 4; ++i) {
        if (m_teamA.m_players[i].m_flag != 0 && m_teamA.m_players[i].m_user != 0)
            maxWinSelf =
                (m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade < maxWinSelf)
                    ? maxWinSelf
                    : m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade;
    }
    for (int i = 0; i < 4; ++i) {
        if (reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[i].m_flag != 0 &&
            reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[i].m_user != 0)
            maxWinOther = (reinterpret_cast<CLeagueMatch*>(other)
                               ->m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade <
                           maxWinOther)
                              ? maxWinOther
                              : reinterpret_cast<CLeagueMatch*>(other)
                                    ->m_teamA.m_players[i]
                                    .m_user->getPVPResultR()
                                    ->m_pvpGrade;
    }

    if ((maxWinSelf >= 12 && maxWinSelf <= 14) && maxWinOther > 10)
        return false;
    if ((maxWinOther >= 12 && maxWinOther <= 14) && maxWinSelf > 10)
        return false;
    if (abs(maxWinSelf - maxWinOther) > 4)
        return false;

    for (int a = 0; a < 4; ++a) {
        for (int b = 0; b < 4; ++b) {
            if (m_teamA.m_players[a].m_flag != 0 &&
                reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[b].m_flag != 0 &&
                m_teamA.m_players[a].m_schoolNo ==
                    reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[b].m_schoolNo)
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

int CLeagueMatch::_GetOppositeTeam(int team)  // ORIG T 0x0855e49e
{
    /* ORIG @0855e4a1 比较链 cmp $1,%eax;je/cmp $2;je/jmp def = 4.1 switch
     * 下探形【技巧34，OnlinePreliminary 同法 IDENTICAL 已验】。ORIG default
     * 块尾多一条 `mov 0xc(%ebp),%eax`(@0855e4be) 重载：4.4 沿 cfg 传播 eax
     * 免载，probe(register 副本/asm("eax")+clobber)均不可逼出，N18a 类 */
    switch (team)
    {
    case 1:
        return 2;
    case 2:
        return 1;
    default:
        break;
    }
    return team;
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
    if ((int)(m_teamA.size() - emptySeats) >= (int)nPlayerCountPerOneTeam_)
        return 0;
    return 1;
}

int CLeagueMatch::addBadge(PlayerInfo* info, ENUM_BADGE_TYPE badgeType,
                           unsigned char* msgFlag, bool extra)   // ORIG T 0x0855e7ee
{
    do {
        if (!info->m_flag)    // ORIG movzbl 0xc + xor $1（flag==0 → 返回 0）
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
        // ORIG：count 初 0，仅 extra 加 1（不读 *msgFlag）。
        int count = extra ? 1 : 0;

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
