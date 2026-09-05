// ============================================================================
// df_game_r 还原 —— online_preliminary::COnlinePreliminary 全方法（唯一 owner）
// 权威头：OnlinePreliminary.h。报告依据
//   docs/class_func_reports/online_preliminary__COnlinePreliminary/*.md；
// 无独立报告的函数按门禁规则用 ORIG /home/wangyilei/下载/df_game_r objdump
// 忠实转写并标注【推断】。
// 对象布局实证：CMatchingSystem::GetMatchingSystem 内 new $0x70 @0x85dfcf1；
//   TeamInfoSecond[2] 位于 +0x1c/+0x44（CheckJoinable/_CheckMatchable 的
//   size() 调用点）；类尾 +0x6c int 计数器（OnConnectP2P ++ / OnMatchRoom 清零）；
//   CheckJoinable switch 跳转表 @0x8ca230c：state==3→0、state==0→校验链、
//   其余(1,2,4,5)→4。
// 本 TU 独占符号（含 README 漏列补充项）：
//   global class TeamInfoSecond 5 方法、PlayerInfoSecond::reset(0x8568850)、
//   CEloRating::calc(W 0x844dca1)、
//   PvPGlobalEnvironmentVariable::{school_match_elo_k_, school_match_elo_k2_}、
//   COnlinePreliminary::_GetTeam(ENUM_SEAT_STATE)（README 漏列，
//   OnLeaveRoom/OnReward 调用点实证）。
// ===========================================================================
#include <math.h>
#include <string.h>

#include "OnlinePreliminary.h"

#include "CDataManager.h"            // G_CDataManager / find_item / m_serverParameter / GetPvPTeamCount / GetPvPMode
#include "CUser.h"                   // Send/GetCharacExpandData/GetPVPRoom/SendCmdErrorPacket/make_basic_info...
#include "CUserCharacInfo.h"         // getCurCharacNo/getCurCharacInvenR/GetCurCharacOnlinePreliminaryTeam(Id)
#include "CEquipItem.h"              // CEquipItem::GetUsableEquipmentType
#include "CItem.h"                   // is_stackable/getUsableLevel/get_index/GetItemType(vtable+0xc)
#include "ServerParameterScript.h"   // CheckUnusableEquipmentInfo/CheckUnusableItem
#include "PvP_Room.h"               // join_room/create_room/get_team/gen_timer_key/.../m_relayMgr
#include "GameWorld.h"               // G_GameWorld/send_all/goto_pvp
#include "CGameManager.h"            // G_CGameManager/GetPvp/PutPvp
#include "InterfacePacketBuf.h"      // put_header/put_byte/finalize/clear
#include "PacketGuard.h"
#include "TimerDispatcher.h"         // TimerStartMatching::registNextTimer(long,int,int)
#include "TimerQueue.h"              // G_TimerQueue/InsertTimer/TIMER_MESSAGE
#include "LogManager.h"              // LogManager::logFormat
#include "GameTypes.h"               // cMyTrace
#include "RDARScriptStringManager.h" // g_scriptStringManager_/findString
#include "Inven_Item.h"

/* ORIG D 0x093abb60 / 0x093abb64 / 0x093abb68（.data 初值 1/2/5；
 * 消费点 DB_UpdatePvPResult::dispatch @0x0843097e，OrigDBPvPResult20） */
int PvPGlobalEnvironmentVariable::school_match_daily_point_ = 1;
int PvPGlobalEnvironmentVariable::school_match_daily_point_weekends_ = 2;
int PvPGlobalEnvironmentVariable::school_match_fight_count_for_gain_point_ = 5;
int PvPGlobalEnvironmentVariable::school_match_elo_k_ = 3;
int PvPGlobalEnvironmentVariable::school_match_elo_k2_ = 3;

// ---------------------------------------------------------------------------
// PlayerInfoSecond —— ORIG W 0x8568850
// ---------------------------------------------------------------------------

void PlayerInfoSecond::reset()
{
    m_user = 0;
    m_flag = 0;
}

// ---------------------------------------------------------------------------
// TeamInfoSecond
// ---------------------------------------------------------------------------

/* COnlinePreliminary ctor 未触碰 teams 区——保持 POD 语义【推断】；生命周期
 * 首次使用前经 OnCreateRoom/reset 显式清零。*/
TeamInfoSecond::TeamInfoSecond() {}

/* ORIG reset T 0x855fdc0 */
void TeamInfoSecond::reset()
{
    /* ORIG 帧 0x28(@0855fdc3) / 循环变量槽 -0xc(@0855fdcf)：两个死槽在 i
     * 之前占用 -0x4/-0x8（asm "m" 引用强制分配且不出机器指令），pad 撑帧
     * 【技巧31】 */
    char padA[28];
    int gapA, gapB;
    m_teamId = 0;
    for (int i = 0; i < 4; ++i)
        m_seats[i].reset();
    (void)padA; (void)gapA; (void)gapB;
}

/* ORIG add T 0x855fdfe */
int TeamInfoSecond::add(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        /* ORIG @0855fe18 xor $1 / test / je：显式 ^1 复现（4.4 !byte 恒折
         * test;jne） */
        if ((m_seats[i].m_flag ^ 1) != 0)
        {
            m_seats[i].m_flag = 1;
            m_seats[i].m_user = user;
            return 1;
        }
    }
    return 0;
}

/* ORIG del T 0x855fe54 */
int TeamInfoSecond::del(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        /* ORIG @0855fe70 test;je skip / @0855fe7f cmp arg;jne skip：&& 短路
         * 正向嵌套 */
        if (m_seats[i].m_flag != 0 && m_seats[i].m_user == user)
        {
            m_seats[i].m_flag = 0;
            return 1;
        }
    }
    return 0;
}

/* ORIG size const T 0x855feaa */
int TeamInfoSecond::size() const
{
    int n = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (m_seats[i].m_flag != 0)
            ++n;
    }
    return n;
}

/* ORIG refresh T 0x855fcf6：最后一个在场成员刷新 teamId/rating（单人模式走
 * charac_expand(4)=COnlinePreliminaryMember::GetRating W 内联 0x8568f02，
 * 组队模式走 COnlinePreliminaryTeam::GetRating 0x8568eee）。*/
void TeamInfoSecond::refresh()
{
    CUser* last = 0;
    for (int i = 0; i < 4; ++i)
    {
        /* ORIG @0855fd33 mov last 后 @0855fd36 jmp 出循环 = break【Ghidra
         * goto LAB_0855fd47 实证】；双 test;je 直指 addl 块 = && 短路 */
        if (m_seats[i].m_flag != 0 && m_seats[i].m_user != 0)
        {
            last = m_seats[i].m_user;
            break;
        }
    }
    if (last == 0)
        return;

    m_teamId = last->GetCurCharacOnlinePreliminaryTeamId();  // T 0x8645de2
    if ((char)online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam() == 1)
        m_rating = reinterpret_cast<online_preliminary::COnlinePreliminaryMember*>(
                       last->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))
                       ->GetRating();
    else if (last->GetCurCharacOnlinePreliminaryTeam() != 0)
        m_rating = last->GetCurCharacOnlinePreliminaryTeam()->GetRating();
}

// ---------------------------------------------------------------------------
// CEloRating —— ORIG W 0x844dca1（无独立报告；objdump 忠实转写【推断】）
// ---------------------------------------------------------------------------
void CEloRating::calc(float& ratingSelf, float& ratingOther,
                      ENUM_ELO_RATING_SYSTEM sys, int k)
{
    float ra = ratingSelf;
    float rb = ratingOther;
    float ea = (float)(1.0 / (1.0 + pow(10.0, (double)((rb - ra) / 400.0f))));
    float eb = (float)(1.0 / (1.0 + pow(10.0, (double)((ra - rb) / 400.0f))));
    float score = 0.0f;
    float lose  = 0.0f;

    if ((int)sys == 1)
        score = 0.5f;
    else if ((int)sys == 2)
        score = 1.0f;

    lose = 1.0f - score;
    ratingSelf  = ra + (float)k * (score - ea);
    ratingOther = rb + (float)k * (lose - eb);
}
namespace online_preliminary
{

// ---------------------------------------------------------------------------
// 静态数据成员（ORIG nm）
// ---------------------------------------------------------------------------

/* ORIG B 0x0943e13c（delete 仅 _ZdlPv ⇒ IRoomList* 静态类型，见 OnlinePreliminary.h） */
IRoomList* COnlinePreliminary::pWaitingRoomList_ = 0;
/* ORIG B 0x0943e140 */
int COnlinePreliminary::nWaitingRoomListRefCount_ = 0;
/* ORIG B 0x0943e138 */
bool COnlinePreliminary::bMatchable_ = false;
/* ORIG D 0x0943e144 */
int COnlinePreliminary::nextAuthValue_ = 0;


// ---------------------------------------------------------------------------
// 生命周期
// ---------------------------------------------------------------------------

/* ORIG ctor T 0x0855fefe */
COnlinePreliminary::COnlinePreliminary()
{
    if (pWaitingRoomList_ == 0)
        pWaitingRoomList_ = new CLeagueMatchRoomList();   // ORIG new(0x1c)+置零+C1
    nWaitingRoomListRefCount_ = nWaitingRoomListRefCount_ + 1;
}

/* ORIG D1 0x0855ffd6（D0 = 本体 + operator delete(this)，由编译器生成） */
COnlinePreliminary::~COnlinePreliminary()
{
    nWaitingRoomListRefCount_ = nWaitingRoomListRefCount_ - 1;
    if (pWaitingRoomList_ != 0 && nWaitingRoomListRefCount_ == 0)
        delete pWaitingRoomList_;
}

// ---------------------------------------------------------------------------
// 每队人数 / 匹配开关 / 地图请求
// ---------------------------------------------------------------------------

/* ORIG T 0x0855fee8：G_CDataManager()->GetPvPTeamCount()（T 0x08568ce0）。
 * GetPvPTeamCount 声明在 CDataManager.h（producer 归 CDataManager 域批次）。*/
unsigned int COnlinePreliminary::GetPlayCountPerOneTeam()
{
    return G_CDataManager()->GetPvPTeamCount();
}

/* ORIG T 0x085608d4：纯 ret（ORIG 即空实现） */
void COnlinePreliminary::SetPlayCountPerOneTeam(unsigned int /*n*/)
{
}

/* ORIG T 0x085608da */
void COnlinePreliminary::SetMatchAble(bool able)
{
    bMatchable_ = able;
}

/* ORIG T 0x085608f2 */
void COnlinePreliminary::RequestMap(short& mapNo, PVP_BATTLE_MODE& mode)
{
    mapNo = 0;
    if ((int)mode != 6)
        mode = (PVP_BATTLE_MODE)G_CDataManager()->GetPvPMode();
}

// ---------------------------------------------------------------------------
// 房间信息 / 建房 / 入退房
// ---------------------------------------------------------------------------

/* ORIG T 0x08560f74 */
void COnlinePreliminary::_BackupRoomInfo(PvP_Room* room)
{
    room->get_room_info(&m_roomInfo);   // T 0x85d9296
}

/* ORIG T 0x08560e4a：返回座位号 */
int COnlinePreliminary::_PutInRoom(CUser* user, PvP_Room* room, bool observer)
{
    PacketGuard pkt;
    int seat = 0;
    int err = room->join_room(user, seat, observer);       // T 0x85d6cf2
    if (err >= 1)
    {
        pkt.put_header(1, 0x36);
        pkt.put_byte(0);
        pkt.put_byte(err);
        pkt.finalize(true);
        user->Send(pkt);
    }
    else
    {
        room->make_seat_info((char*)(PacketGuard*)&pkt, seat);  // T 0x85db9e2
        G_GameWorld()->send_all(pkt);
        G_GameWorld()->goto_pvp(user);
    }
    return seat;
}

/* ORIG T 0x08560df2 */
void COnlinePreliminary::_PullOutRoom(CUser* user, PvP_Room* room, int seat)
{
    if (!room->OnSetPVPSeatState(user, seat,
                                 ENUM_SEAT_STATE_0xfe,
                                 (ENUM_WALKOUT_TYPE)2))     // T 0x85dd070
    {
        room->reset();                                      // T 0x85d62c8
        G_CGameManager()->PutPvp(room);
    }
}

/* ORIG T 0x08560f92 */
PvP_Room* COnlinePreliminary::_MakeRoom(CUser* user)
{
    PacketGuard pkt;
    PvP_Room* pvp = G_CGameManager()->GetPvp();            // T 0x829507c
    if (pvp == 0)
    {
        pkt.put_header(1, 0x35);
        pkt.put_byte(0);
        pkt.put_byte(4);
        pkt.finalize(true);
        user->Send(pkt);
        return (PvP_Room*)0;
    }

    char name[0x20];
    char pass[0x20];
    short titleLen = 0;
    (void)name; (void)pass; (void)titleLen; /* 布局对照局部（ORIG 直接读 string）*/

    int err = pvp->create_room(user,
                               (char)m_roomInfo.byte4,          // this[0xc]
                               const_cast<char*>(m_roomInfo.title.c_str()),
                               m_roomInfo.short6,               // this[0xe]
                               true,                            // BlueMarble+0x41 → 1
                               m_roomInfo.passwd.c_str(),       // this+0x14
                               (int)m_roomInfo.passwd.length(),
                               (PVP_BATTLE_MODE)2);
    if (err < 1)
    {
        // 成功路径（ORIG jle 0x856115e）
        pkt.clear();                                            // 080cb8e6
        pkt.put_header(0, 2);
        pkt.put_byte(1);
        pkt.put_short(1);
        user->make_basic_info((char*)(PacketGuard*)&pkt, '\x01');  // T 0x865a44e
        pkt.finalize(true);
        user->Send(pkt);

        pkt.clear();
        pkt.put_header(0, 0x29);
        pkt.put_short(1);
        pvp->make_room_info((char*)(PacketGuard*)&pkt);            // T 0x85db7f6
        pkt.finalize(true);
        G_GameWorld()->send_all(pkt);
        G_GameWorld()->goto_pvp(user);
        return pvp;
    }

    // 失败路径（ORIG 0x85610ae 起）
    pkt.put_byte(0);
    pkt.put_byte(err);
    pkt.finalize(true);
    user->Send(pkt);
    LogManager::logFormat(1, "MatchingSystem.cpp",
                          __PRETTY_FUNCTION__,
                          0x73c,
                          "create_room fail: PVP슬롯반환 ID[%d]\n",
                          pvp->get_index());
    G_CGameManager()->PutPvp(pvp);
    return (PvP_Room*)0;
}

/* ORIG T 0x085612a4：经 pWaitingRoomList_ vtable+0（IRoomList::AddRoom）虚调用 */
void COnlinePreliminary::_RegisterWaitingRoom(PvP_Room* room)
{
    IRoomList* list = pWaitingRoomList_;
    list->AddRoom(reinterpret_cast<IMatch*>(room));
}

// ---------------------------------------------------------------------------
// 校验族
// ---------------------------------------------------------------------------

/* ORIG T 0x08560082 前置的门禁函数 —— 见 _GetRating 家族段上方注释 */

/* ORIG T 0x085606e0（mangled 无 K → 非 const 成员） */
int COnlinePreliminary::_CheckSameTeam(PvP_Room* /*room*/, CUser* user)
{
    /* ORIG @085606f2 cmp $-1; sete %al; test; je（bool 物化 @085606f5）【技巧22】；
     * @08560703 第二次 call 后 mov %eax,%ebx(@0856070e) */
    register bool isNone asm("al");
    isNone = (user->GetCurCharacOnlinePreliminaryTeamId() == -1);
    if (isNone)
        return 0;
    /* ORIG @0856070e 第二次 call 结果直落 %ebx（callee-saved），4.4 默认 spill
     * 栈槽 → register asm("ebx") 钉住（技巧28） */
    register int teamId asm("ebx") = user->GetCurCharacOnlinePreliminaryTeamId();
    register bool diff asm("al");
    /* ORIG cmp %eax,%ebx：dst=ebx(src=call 结果) → call 表达式置 lhs */
    diff = (GetTeamId() != teamId);
    if (diff)
        return 0;
    return 1;
}

/* ORIG T 0x08560736（mangled 无 K → 非 const 成员；第二参为 PvP_Room*，报告变量名 param_2 有误【推断修正】） */
int COnlinePreliminary::_CheckObserver(PvP_Room* room, CUser* /*user*/)
{
    int observers = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (room->get_team(i) == 3)
            ++observers;
    }
    /* ORIG(@0856078a 起 size→%ebx / sub mem,%ebx / cmp %eax,%ebx; setge 物化)
     * 属 N22/N18a 类：4.4 恒将跨 call 差值 spill 或经 %eax 往返，probe(
     * register asm("ebx")+setge 技巧22/操作数对调) 均不可翻，保留直译 */
    return (m_teams[0].size() - observers < (int)(unsigned char)GetPlayCountPerOneTeam())
               ? 1 : 0;
}

/* ORIG T 0x085612c6 const；返回 0 可匹配 / 0x16 失配 */
int COnlinePreliminary::_CheckMatchable(PvP_Room* room) const
{
    int observers = 0;
    int firstTeamId = 0;
    for (int i = 0; i <= 7; ++i)
    {
        if (room->get_team(i) == 3)
        {
            ++observers;
            continue;
        }
        CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(i));
        if (u == 0)
            continue;
        int tid = u->GetCurCharacOnlinePreliminaryTeamId();
        if (firstTeamId == 0)
            firstTeamId = tid;
        else if (tid != firstTeamId)
        {
            LogManager::logFormat(
                1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__,
                0x771, "(%d) found a different team_id(%d)", tid);
            return 0x16;
        }
    }
    return (m_teams[0].size() - observers == (int)(GetPlayCountPerOneTeam() & 0xff))
               ? 0 : 0x16;
}

/* ORIG T 0x085607b2（switch 表 @0x8ca230c 实证） */
int COnlinePreliminary::CheckJoinable(PvP_Room* room, CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 1;

    if (room->get_player_count() >= (int)((GetPlayCountPerOneTeam() & 0xff) * 2))
        return 4;

    if ((int)((GetPlayCountPerOneTeam() & 0xff) * 2) <
        m_teams[0].size() + m_teams[1].size())
        return 4;

    switch (m_field04)
    {
    case 0:
        if (_CheckSameTeam(room, user))
        {
            if (!_CheckObserver(room, user))
                return 0;
        }
        return 4;
    case 3:
        return 0;
    case 1:                     // 与 4、5 及 default 同落 ret 4（表实证）
    case 4:
    case 5:
    default:
        return 4;
    }
}

/* ORIG T 0x085608bc：cmpl $2 / jne else(@+0x10)；then 体 mov $0 fall-through
 * (@085608c5)，else 体 mov $1 (@085608cc) → !=2 判定块在前【技巧17】 */
int COnlinePreliminary::CheckSeat(int a)
{
    /* ORIG 用 jne 反向跳(@085608c3)，if/else 被 4.4 恒翻 je → goto 钉块序
     * 【技巧23】 */
    if (a != 2)
        goto RET1;
    return 0;
RET1:
    return 1;
}

/* ORIG T 0x085623ec */
bool COnlinePreliminary::CheckMatchable(IMatch* other)
{
    CUser* otherUser = 0;
    CUser* selfUser = 0;
    for (int i = 0; i <= 3; ++i)
    {
        otherUser = other->GetUser(i);
        if (otherUser != 0)
            break;
    }
    if (otherUser == 0)
        return false;
    for (int i = 0; i <= 3; ++i)
    {
        selfUser = GetUser(i);
        if (selfUser != 0)
            break;
    }
    if (selfUser == 0)
        return false;

    // ORIG 0x085624c3 有符号 setge 比较（count >= limit → false）。
    unsigned int played = _GetEachOtherPlayCount(selfUser, otherUser);
    if ((int)played >= G_CDataManager()->m_serverParameter.GetDailyMatchCount())
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// rating 记录家族
// ---------------------------------------------------------------------------

float COnlinePreliminary::_GetRating(ENUM_SCHOOL_MATCH_TEAM team)
{
    return m_teams[(int)team].m_rating;
}

/* ORIG T 0x08560082 */
int COnlinePreliminary::_GetRating(CUser* user)
{
    float f = 0.0f;
    if ((char)GetPlayCountPerOneTeam() == 1)
        f = reinterpret_cast<COnlinePreliminaryMember*>(
                user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->GetRating();
    else if (user->GetCurCharacOnlinePreliminaryTeam() != 0)
        f = user->GetCurCharacOnlinePreliminaryTeam()->GetRating();
    int bits;
    memcpy(&bits, &f, sizeof(bits));
    return bits;
}

/* ORIG T 0x08560368：首个在场成员同步个人 rating */
void COnlinePreliminary::_SetRating(ENUM_SCHOOL_MATCH_TEAM team, float v)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_teams[(int)team].m_seats[i].m_flag != 0 &&
            m_teams[(int)team].m_seats[i].m_user != 0)
        {
            _SetRating(m_teams[(int)team].m_seats[i].m_user, v);
            break;
        }
    }
}

/* ORIG T 0x085600f6 */
void COnlinePreliminary::_SetRating(CUser* user, float v)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
        reinterpret_cast<COnlinePreliminaryMember*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->SetRating(v);
    else if (user->GetCurCharacOnlinePreliminaryTeam() != 0)
        user->GetCurCharacOnlinePreliminaryTeam()->SetRating(v);
}

/* ORIG T 0x08560be2：winOrLose=1 以 team0 为自身结算，否则以 team1 */
void COnlinePreliminary::_UpdateRating(int winOrLose, ENUM_ELO_RATING_SYSTEM sys)
{
    float selfRating;
    float otherRating;
    ENUM_SCHOOL_MATCH_TEAM selfTeam;

    if (winOrLose == 1)
    {
        selfRating = _GetRating((ENUM_SCHOOL_MATCH_TEAM)0);
        otherRating = _GetRating((ENUM_SCHOOL_MATCH_TEAM)1);
        CEloRating::calc(selfRating, otherRating, sys,
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        selfTeam = SCHOOL_MATCH_TEAM_0;
    }
    else
    {
        selfRating = _GetRating((ENUM_SCHOOL_MATCH_TEAM)1);
        otherRating = _GetRating((ENUM_SCHOOL_MATCH_TEAM)0);
        CEloRating::calc(selfRating, otherRating, sys,
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        selfTeam = SCHOOL_MATCH_TEAM_1;
    }
    float out = selfRating;
    _SetRating(selfTeam, out);
}

void COnlinePreliminary::_IncreaseWinCount(CUser* user)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
        reinterpret_cast<COnlinePreliminaryMember*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->IncreaseWinCount();
    else if (user->GetCurCharacOnlinePreliminaryTeam() != 0)
        user->GetCurCharacOnlinePreliminaryTeam()->IncreaseWinCount();
}

/* ORIG T 0x085603fa：队内第一个在场成员计数 */
void COnlinePreliminary::_IncreaseWinCount(ENUM_SCHOOL_MATCH_TEAM team)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_teams[(int)team].m_seats[i].m_flag != 0 &&
            m_teams[(int)team].m_seats[i].m_user != 0)
        {
            _IncreaseWinCount(m_teams[(int)team].m_seats[i].m_user);
            break;
        }
    }
}

/* ORIG T 0x085601b8 */
void COnlinePreliminary::_IncreaseLoseCount(CUser* user)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
        reinterpret_cast<COnlinePreliminaryMember*>(
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->IncreaseLoseCount();
    else if (user->GetCurCharacOnlinePreliminaryTeam() != 0)
        user->GetCurCharacOnlinePreliminaryTeam()->IncreaseLoseCount();
}

/* ORIG T 0x08560484 */
void COnlinePreliminary::_IncreaseLoseCount(ENUM_SCHOOL_MATCH_TEAM team)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (m_teams[(int)team].m_seats[i].m_flag != 0 &&
            m_teams[(int)team].m_seats[i].m_user != 0)
        {
            _IncreaseLoseCount(m_teams[(int)team].m_seats[i].m_user);
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// 双方交手次数家族
// ---------------------------------------------------------------------------

/* ORIG T 0x08560212 */
unsigned int COnlinePreliminary::_GetEachOtherPlayCount(CUser* a, CUser* b)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
    {
        // 在 a 的“对过谁”表（charac_expand(5)）中记/查 b 的 (serverGroup,characNo)
        COnlinePreliminaryMemberMatchList* list =
            reinterpret_cast<COnlinePreliminaryMemberMatchList*>(
                a->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)5));
        return list->GetPeerPlayCount(b->GetServerGroup(),
                                      b->getCurCharacNo());
    }
    if (a->GetCurCharacOnlinePreliminaryTeam() == 0)
        return 0;
    return a->GetCurCharacOnlinePreliminaryTeam()->GetEachOtherTeamPlayCount(
        b->GetCurCharacOnlinePreliminaryTeamId());
}

/* ORIG T 0x0856050e：取两队在场的最后一名成员查询 */
unsigned int COnlinePreliminary::_GetEachOtherPlayCount()
{
    CUser* lastA = 0;
    CUser* lastB = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_teams[0].m_seats[i].m_flag != 0 &&
            m_teams[0].m_seats[i].m_user != 0)
            lastA = m_teams[0].m_seats[i].m_user;
    }
    for (int i = 0; i <= 3; ++i)
    {
        if (m_teams[1].m_seats[i].m_flag != 0 &&
            m_teams[1].m_seats[i].m_user != 0)
            lastB = m_teams[1].m_seats[i].m_user;
    }
    if (lastA == 0 || lastB == 0)
        return 0;
    return _GetEachOtherPlayCount(lastA, lastB);
}

/* ORIG T 0x085602ae */
void COnlinePreliminary::_IncreaseEachOtherPlayCount(CUser* a, CUser* b)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
    {
        reinterpret_cast<COnlinePreliminaryMemberMatchList*>(
            a->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)5))
            ->IncreasePeerPlayCount(b->GetServerGroup(),
                                    b->getCurCharacNo());
    }
    else if (a->GetCurCharacOnlinePreliminaryTeam() != 0)
    {
        a->GetCurCharacOnlinePreliminaryTeam()->IncreaseEachOtherTeamPlayCount(
            b->GetCurCharacOnlinePreliminaryTeamId());
    }
}

/* ORIG T 0x085605ee：双向累计 */
void COnlinePreliminary::_IncreaseEachOtherPlayCount()
{
    CUser* lastA = 0;
    CUser* lastB = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_teams[0].m_seats[i].m_flag != 0 &&
            m_teams[0].m_seats[i].m_user != 0)
            lastA = m_teams[0].m_seats[i].m_user;
    }
    for (int i = 0; i <= 3; ++i)
    {
        if (m_teams[1].m_seats[i].m_flag != 0 &&
            m_teams[1].m_seats[i].m_user != 0)
            lastB = m_teams[1].m_seats[i].m_user;
    }
    if (lastA == 0 || lastB == 0)
        return;
    _IncreaseEachOtherPlayCount(lastA, lastB);
    _IncreaseEachOtherPlayCount(lastB, lastA);
}

// ---------------------------------------------------------------------------
// 装备合法性
// ---------------------------------------------------------------------------

/* ORIG T 0x08560920 */
bool COnlinePreliminary::CheckUnusableEquipment(const Inven_Item* item)
{
    if (item == 0)
        return false;
    if (item->m_addInfo == 0)
        return true;

    CDataManager& dm = *G_CDataManager();
    CItem* found = dm.find_item(item->m_addInfo);
    if (found == 0)
        return false;
    if (found->is_stackable())
        return false;

    CEquipItem* equip = static_cast<CEquipItem*>(found);
    int equipType = equip->GetUsableEquipmentType();      // W 0x832e036
    if (equipType == 0)
        return true;
    if (!dm.m_serverParameter.CheckUnusableEquipmentInfo((ENUM_UNUSABLE_EQUIPMENT_TYPE)equipType,
                                                         found->getUsableLevel()))
        return false;
    // ORIG 经虚表槽 +0xc（GetItemType）取类型后查 CheckUnusableItem
    return dm.m_serverParameter.CheckUnusableItem(
                 (ENUM_EQUIPMENTTYPE)found->GetItemType(),
                                                  (unsigned long)found->get_index());
}

/* ORIG T 0x08560a42：装备栏 0..22 槽全检 */
int COnlinePreliminary::CheckUnusableEquipmentInUser(CUser* user)
{
    for (int slot = 0; slot <= 0x16; ++slot)
    {
        const Inven_Item* item =
            user->getCurCharacInvenR()->GetInvenRef(0, slot);   // T 0x84fc1de
        if (!CheckUnusableEquipment(item))
        {
            const char* msg = g_scriptStringManager_.findString(
                4, "game_server_msg_145", (bool*)0);
            user->SendNotiPacketMessage(msg, (ENUM_MESSAGE_TYPE)0);
            return 0;
        }
    }
    return 1;
}
// ---------------------------------------------------------------------------
// IMatch 虚槽家族
// ---------------------------------------------------------------------------

/* ORIG T 0x08560ae4 */
int COnlinePreliminary::OnCreateRoom(CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 0x13;                              /* ORIG mov $0x13,%eax（0x8560afc） */

    m_authValue = ++nextAuthValue_;               /* 全局自增令牌 0x943e144 */
    m_field04 = 0;
    m_teams[1].reset();                           /* T 0x855fdc0 */
    m_teams[0].reset();
    m_teams[0].add(user);                         /* T 0x855fdfe */
    m_teams[0].m_teamId = user->GetCurCharacOnlinePreliminaryTeamId();
    int ratingBits = _GetRating(user);
    memcpy(&m_teams[0].m_rating, &ratingBits, 4);
    return 0;                                     /* ORIG mov $0x0,%eax（0x8560b83） */
}

/* ORIG T 0x08560b8a */
void COnlinePreliminary::OnDestroyRoom(CUser* /*user*/)
{
    m_field04 = 0;
    pWaitingRoomList_->DelRoom(this);             /* vtable+4 虚调实证 */
}

/* ORIG T 0x08560bba */
void COnlinePreliminary::OnJoinRoom(CUser* user)
{
    if (m_field04 != 3)
        m_teams[0].add(user);
}

/* ORIG T 0x08560cd2：第 4 参为离场者队伍组值（OnWalkOut 以 get_team(seat) 传入）*/
void COnlinePreliminary::OnLeaveRoom(CUser* user, PvP_Room* room,
                                     int teamGroup, int /*b*/)
{
    if (m_field04 == 4)
    {
        _SendMatchStatePacket(4, 2);
        m_field04 = 0;
        _ReturnTeam(room, room->get_manager_team(), user);
    }
    else if (m_field04 == 5)
    {
        if (room->GetTeamCount(teamGroup) == 1)
        {
            _UpdateRating(teamGroup, (ENUM_ELO_RATING_SYSTEM)0);
            _IncreaseLoseCount(_GetTeam((ENUM_SEAT_STATE)teamGroup));
        }
    }
    else
    {
        m_field04 = 0;
    }

    if (teamGroup == 1)
        m_teams[0].del(user);
    else
        m_teams[1].del(user);

    pWaitingRoomList_->DelRoom(this);
}

/* ORIG T 0x085613de。ORIG 返回 int：m_field04==1 → 1；失败路径 → 0；成功 → 1。 */
int COnlinePreliminary::OnRegisterMatch(CUser* user, PvP_Room* room)
{
    if (m_field04 == 1)
        return 1;

    if (!bMatchable_)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc7, 0x13);
        return 0;
    }

    int err = _CheckMatchable(room);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc7, (unsigned char)err);
        return 0;
    }

    if (m_field04 != 2)
        _RegisterWaitingRoom(room);

    m_field04 = 1;
    TimerStartMatching::registNextTimer(10, room->get_index(), m_authValue);
    _SendMatchStatePacket(0, 1);
    return 1;
}

/* ORIG T 0x085614ea：第 2 参为匹配授权号，与 m_authValue 校验。ORIG 失败路径 → 0；成功 → 1。 */
int COnlinePreliminary::OnMatchRoom(PvP_Room* room, int authNo)
{
    if (m_authValue != authNo)
    {
        cMyTrace trace("online_preliminary::COnlinePreliminary::OnMatchRoom(PvP_Room*, int)",
                       0x7a6, 0);
        trace("OnMatchRoom timeout");
        return 0;
    }
    if (m_field04 != 1)
        return 0;

    IMatch* opponent = 0;
    pWaitingRoomList_->Match(this, &opponent);            /* vtable+8 虚调 */
    if (opponent == 0)
    {
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    CUser* oppUser = opponent->GetUser(0);                /* vt+0x10 */
    if (oppUser == 0)
    {
        cMyTrace trace(".../OnMatchRoom(PvP_Room*, int)", 0x7c1, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x7c1);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    PvP_Room* oppRoom = (PvP_Room*)oppUser->GetPVPRoom();            /* T 0x865518e */
    if (oppRoom == 0)
    {
        cMyTrace trace(".../OnMatchRoom(PvP_Room*, int)", 0x7ca, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x7ca);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }
    if (oppRoom->get_player_count() > (int)(GetPlayCountPerOneTeam() & 0xff))
    {
        cMyTrace trace(".../OnMatchRoom(PvP_Room*, int)", 0x7d2, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x7d2);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }
    if ((int)(GetPlayCountPerOneTeam() & 0xff) < m_teams[0].size() ||
        oppRoom->get_player_count() > (int)(GetPlayCountPerOneTeam() & 0xff))
    {
        cMyTrace trace(".../OnMatchRoom(PvP_Room*, int)", 0x7da, 0);
        trace("error file(%s) line(%d)", "MatchingSystem.cpp", 0x7da);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    _BackupRoomInfo(oppRoom);
    m_teams[1].reset();
    /* ORIG 经对方房间 get_user_list（其 match->GetUserList）填充 team1 视图：
     * 对方同为 COnlinePreliminary 时即 memcpy 其 teams[0]，布局天然兼容。*/
    oppRoom->get_user_list(&m_teams[1]);

    _SendMatchStatePacket(3, 2);
    m_field04 = 3;

    for (unsigned int s = 0; s <= 7; ++s)
    {
        CUser* u = reinterpret_cast<CUser*>(room->get_user_seat((int)s));
        if (u == 0)
            continue;
        int team = room->get_team((int)s);
        _PullOutRoom(u, room, (int)s);
        unsigned int newSeat =
            (unsigned int)_PutInRoom(u, room, false);
        if (team == 3)
            room->OnSetPVPSeatState(u, (int)newSeat,
                                    (ENUM_SEAT_STATE)3, (ENUM_WALKOUT_TYPE)2);
    }

    pWaitingRoomList_->DelRoom(this);
    m_field04 = 4;
    _SendMatchStatePacket(2, 2);
    m_p2pReadyCount = 0;

    unsigned int key = room->gen_timer_key((TIMER_MESSAGE)0x3b);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)5, room->get_index(),
                                (TIMER_MESSAGE)0x3b, 0xb4, (int)key, 0);
    return 1;
}

/* ORIG T 0x08561a52 */
void COnlinePreliminary::OnStartFight()
{
    m_field04 = 5;
    _IncreaseEachOtherPlayCount();
}

/* ORIG T 0x08562022 */
void COnlinePreliminary::OnEndFight(PvP_Room* room)
{
    int managerTeam = room->get_manager_team();           /* T 0x85d9242 */
    m_field04 = 0;
    _ReturnTeam(room, managerTeam, (CUser*)0);
    if (managerTeam != 1)
        memcpy(&m_teams[0], &m_teams[1], sizeof(TeamInfoSecond));  /* ORIG 十双字块拷贝 */
    m_teams[0].refresh();                                 /* T 0x855fcf6 */
    m_teams[1].reset();
}

/* ORIG T 0x08561f8c（vtable+0x2c＝OnLeaveRoom 级联）。ORIG 返回 1。 */
int COnlinePreliminary::OnWalkOut(PvP_Room* room, int a)
{
    int team = room->get_team(a);
    CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(a));
    OnLeaveRoom(u, room, a, team);
    return 1;
}

/* ORIG T 0x08561ff2 */
bool COnlinePreliminary::OnReady(CUser* /*user*/, bool ready)
{
    if (!ready && m_field04 == 4)
        return false;
    return true;
}

/* ORIG T 0x08562250：param(b)=胜方标记，param(a)=失败方队伍标记【推断命名】 */
void COnlinePreliminary::OnReward(PvP_Room* /*room*/, int loseTeamMark,
                                  bool isDrawWin)
{
    if (isDrawWin)
    {
        _UpdateRating(1, (ENUM_ELO_RATING_SYSTEM)1);
        _UpdateRating(2, (ENUM_ELO_RATING_SYSTEM)1);
        _IncreaseWinCount(_GetTeam((ENUM_SEAT_STATE)1));
        _IncreaseWinCount(_GetTeam((ENUM_SEAT_STATE)2));
    }
    else if (loseTeamMark == 1)
    {
        _UpdateRating(1, (ENUM_ELO_RATING_SYSTEM)2);
        _UpdateRating(2, (ENUM_ELO_RATING_SYSTEM)0);
        _IncreaseWinCount(_GetTeam((ENUM_SEAT_STATE)1));
        _IncreaseLoseCount(_GetTeam((ENUM_SEAT_STATE)2));
    }
    else
    {
        _UpdateRating(2, (ENUM_ELO_RATING_SYSTEM)2);
        _UpdateRating(1, (ENUM_ELO_RATING_SYSTEM)0);
        _IncreaseLoseCount(_GetTeam((ENUM_SEAT_STATE)1));
        _IncreaseWinCount(_GetTeam((ENUM_SEAT_STATE)2));
    }
}

/* ORIG T 0x08561a70：包(0,0x116){uid short} 双队广播 */
void COnlinePreliminary::OnCompleteLoad(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(0, 0x116);
    pkt.put_short((int)(user->get_unique_id() & 0xffff));
    pkt.finalize(true);
    for (int t = 0; t <= 1; ++t)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* u = m_teams[t].m_seats[i].m_user;
            if (m_teams[t].m_seats[i].m_flag == 0 || u == 0)
                continue;
            u->Send(pkt);
        }
    }
}

/* ORIG T 0x08561dee */
void COnlinePreliminary::OnConnectP2PTimeout(PvP_Room* room)
{
    if (m_field04 != 4)
        return;
    _SendMatchStatePacket(4, 2);
    OnEndFight(room);                                     /* vtable+0x40 虚调实证 */
}

/* ORIG T 0x08561b80 */
void COnlinePreliminary::OnConnectP2P(CUser* user, PvP_Room* room,
                                      char* addr, int len)
{
    if (m_field04 != 4)
        return;

    for (int i = 0; i <= 7 && i < len; ++i)
    {
        if (addr[i] == 0)
        {
            _SendMatchStatePacket(4, 2);
            OnEndFight(room);
            return;
        }
    }

    ++m_p2pReadyCount;                                    /* this+0x6c */
    if ((int)m_p2pReadyCount ==
        (int)(GetPlayCountPerOneTeam() & 0xff) * 2)
    {
        PacketGuard ready;
        ready.put_header(0, 0x117);
        ready.finalize(true);
        for (int t = 0; t <= 1; ++t)
        {
            for (int i = 0; i <= 3; ++i)
            {
                CUser* u = m_teams[t].m_seats[i].m_user;
                if (m_teams[t].m_seats[i].m_flag == 0 || u == 0)
                    continue;
                u->Send(ready);
            }
        }
    }

    PacketGuard st;
    st.clear();
    room->battle_reset();                                 /* T 0x85d6430 */
    for (int s = 0; s <= 7; ++s)
    {
        if (room->get_user_seat(s) != 0)
            room->m_relayMgr.OnReady(s, true);            /* +0x620 子对象 */
    }
    room->start_pvp();                                    /* T 0x85d8c3a */
    char buf[0x80];
    room->make_state_info(buf);                           /* T 0x85dbd3a */
    G_GameWorld()->send_all(st);
}

/* ORIG T 0x08561e36：纯返 1 */
int COnlinePreliminary::OnGiveup(CUser* /*user*/)
{
    return 1;
}

// ---------------------------------------------------------------------------
// IMatch 视图 / 辅助查询
// ---------------------------------------------------------------------------

/* ORIG T 0x08561a0e：fabs(self.team0.rating − other.team0.rating)，other 按
 * 本类静态寻址（非虚）实证。 */
float COnlinePreliminary::GetGap(IMatch* other)
{
    // ORIG 0x08561a0e：经 _GetRating(0) 取双方 rating（fsubrs + fabs）。
    const float mine = _GetRating(static_cast<ENUM_SCHOOL_MATCH_TEAM>(0));
    const float theirs = static_cast<COnlinePreliminary*>(other)->_GetRating(
        static_cast<ENUM_SCHOOL_MATCH_TEAM>(0));
    return fabsf(mine - theirs);
}

/* ORIG T 0x085620c8：仅 team0 前四席视图，占用且 state==1 才有效 */
CUser* COnlinePreliminary::GetUser(unsigned int idx)
{
    if (idx >= 4)
        return 0;
    /* ORIG @085620e6 xor $1 / test / je body(@+0x2c)：显式 ^1 */
    if ((m_teams[0].m_seats[idx].m_flag ^ 1) != 0)
        return 0;
    return m_teams[0].m_seats[idx].m_user;
}

/* ORIG T 0x08562104（docs/class_func_reports/online_preliminary__COnlinePreliminary/
 * GetUserList.md）：将 this+0x1c..+0x40 即 m_teams[0]（TeamInfoSecond，0x28 字节）
 * 按 10 个 dword 展开逐字复制到出参。 */
void COnlinePreliminary::GetUserList(void* out)
{
    const unsigned int* src = reinterpret_cast<const unsigned int*>(&m_teams[0]);
    unsigned int* dst = static_cast<unsigned int*>(out);
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
    dst[4] = src[4];
    dst[5] = src[5];
    dst[6] = src[6];
    dst[7] = src[7];
    dst[8] = src[8];
    dst[9] = src[9];
}

/* ORIG T 0x085623e0：teams[0].teamId */
int COnlinePreliminary::GetTeamId()
{
    return m_teams[0].m_teamId;
}

/* ORIG T 0x8561f66 */
int COnlinePreliminary::_GetOppositeTeam(int team)
{
    /* ORIG @08561f69 比较链 cmp $1/%eax; je; cmp $2; je; jmp def（4.1 switch
     * 下探形【技巧34】） */
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

/* ORIG T 0x08568866（README 漏列补充）：seat-state 1/2 → 队伍 0/1 */
ENUM_SCHOOL_MATCH_TEAM COnlinePreliminary::_GetTeam(ENUM_SEAT_STATE state)
{
    return (ENUM_SCHOOL_MATCH_TEAM)((int)state - 1);
}

/* ORIG T 0x0856214a：包(0,0xaf){byte a} 广播前 b 个队伍的在场成员 */
void COnlinePreliminary::_SendMatchStatePacket(int a, int b)
{
    PacketGuard pkt;
    pkt.put_header(0, 0xaf);
    pkt.put_byte(a);
    pkt.finalize(true);
    for (int t = 0; t < b; ++t)
    {
        for (int i = 0; i <= 3; ++i)
        {
            CUser* u = m_teams[t].m_seats[i].m_user;
            if (m_teams[t].m_seats[i].m_flag == 0 || u == 0)
                continue;
            u->Send(pkt);
        }
    }
}

} // namespace online_preliminary
