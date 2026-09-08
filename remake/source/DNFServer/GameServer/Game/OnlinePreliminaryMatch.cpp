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

// ---------------------------------------------------------------------------
// TeamInfoSecond
// ---------------------------------------------------------------------------

/* COnlinePreliminary ctor 未触碰 teams 区、ORIG nm 无 TeamInfoSecond::C1/C2
 * ⇒ 无用户 ctor（POD 语义）；生命周期首次使用前经 OnCreateRoom/reset 显式
 * 清零。声明用户 ctor 会让 4.4 在本 TU ctor 生成数组元素构造循环（ORIG 无）。 */

/* ORIG reset T 0x855fdc0 */
void TeamInfoSecond::reset()
{
    /* ORIG 帧 0x28(@0855fdc3) / 循环变量槽 -0xc(@0855fdcf)：两个死槽在 i
     * 之前占用 -0x4/-0x8。4.4 死 pad 不占槽，改 double framePad 撑帧
     * （P190/技巧31 系，/tmp/p26_slot.cpp probeC 实证同形 0x28/i=-0xc）。 */
    double framePad;
    m_teamId = 0;
    for (int i = 0; i < 4; ++i)
        m_seats[i].reset();
    (void)&framePad;
}

/* ORIG add T 0x855fdfe */
int TeamInfoSecond::add(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        /* [b28c-L26 翻案·档②] ORIG @0855fe18 为 `movzbl 0xc(%eax,%edx,8),%eax;
         * xor $0x1,%eax; test %al,%al; je` 寄存器 xor 物化。旧判词（b28 早期
         * N96+P168a④）称"需 register 绑定、清绑后无源形"，实测四形（==1/!=1/
         * ^1!=0/裸 !field）恒折 cmp 或缺 xor——前提不成立：按 lane20A 定稿判例
         * （CSharedServerMessageManager::m_isSet uchar→bool ⇒ 裸 `!field` 自然
         * 复现 movzbl;xor$1;test;je，ndiff 3→0），把 TeamSeatInfo::m_flag
         * （OnlinePreliminary.h，非冻结）uchar→bool 后本件 **28=28、diff 仅剩
         * call/jmp 重定位伪差（AE）**；同 TU fresh 联判（P211 陷阱法）：
         * identical 42 不变 / ae 43→45 / near 8 不变 / diff 12→10 ⇒ 净 +2 AE、
         * 零回退（_GetLastUser 的 `^1` 段一并受益）。*/
        if (!m_seats[i].m_flag)
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

    // ORIG 0x0844dcf0：`mov 0x10(%ebp),%eax; cmp $1/je; cmp $2/je; test/jne`
    // 决策树（P17/P46/P208）⇒ 源形为 switch（if-else 链恒内存直比）。
    // case 按数值升序书写（P46：4.4 树根取 (lo+hi)/2=1，叶块按 case 序排放）
    // [b28d-solo-OnlinePreliminaryMatch] P305：ENUM_ELO_RATING_SYSTEM 仅定义
    // `_0=0`（SchoolMatchEnums.h），-fstrict-enums 判值域 {0} ⇒ 裸 switch(sys)
    // 静默删 case 1/2（实测 .o 仅剩 0.0f/0.5f 两叶，1.0f 块消失=真语义 bug，
    // ORIG 三叶全在）。改 `switch((int)sys)` 绕开值域裁剪（S18 规避法）。
    switch ((int)sys)
    {
    case 0:
        score = 0.0f;
        break;
    case 1:
        score = 0.5f;
        break;
    case 2:
        score = 1.0f;
        break;
    }

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
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE/P299 翻案] 旧 b22 N 族判词作废：
 * ORIG new(0x1c) 后 6 条 value-init 置零（+0x00/08/0c/10/14/18）并非不可翻，
 * 根因是 CLeagueMatchRoomList 的用户空 ctor 抑制 CWG434 前置零初始化
 * （ORIG nm 实证该 C1 为 W 0x2a 隐式合成）。修法已由 Main 代落（删
 * MatchRoomList.h 声明 + MatchRoomList.cpp 空体定义）⇒ 本件 67=67 AE。 */
COnlinePreliminary::COnlinePreliminary()
{
    if (pWaitingRoomList_ == 0)
        /* ORIG @0855ff42-0x66 value-init 有 6 条标量置零（4.1 对 new T() 先
         * 清零再调 C1@0855ff70）；4.4 视 user-provided ctor 类免置零直调，
         * probe(placement new+try) 反而引入 operator new(void*,ptr) call +
         * EH spill，保留直译，置零差记 N 类 */
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
        return seat;
    }
    room->make_seat_info((char*)(PacketGuard*)&pkt, seat);  // T 0x85db9e2
    G_GameWorld()->send_all(pkt);
    G_GameWorld()->goto_pvp(user);
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
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE] 本波修：①真语义 bug
 * create_room 第 5 参 true→m_roomInfo.byte8（ORIG 读 this[0x10]）；②序列
 * 还原 put_header(1,0x35) 移至 GetPvp 前（ORIG @08560fbc 序）；③删
 * name/pass/titleLen 布局局部（帧 0x9c→0x6c）；④成功/失败块序翻正。
 * 196=196 全等，详见函数体内注释。 */
PvP_Room* COnlinePreliminary::_MakeRoom(CUser* user)
{
    PacketGuard pkt;
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08560fbc 先 put_header(1,0x35)，
     * 之后才 G_CGameManager()/GetPvp（原顺序反 ⇒ 序列错位）。 */
    pkt.put_header(1, 0x35);
    PvP_Room* pvp = G_CGameManager()->GetPvp();            // T 0x829507c
    if (pvp == 0)
    {
        pkt.put_byte(0);
        pkt.put_byte(4);
        pkt.finalize(true);
        user->Send(pkt);
        return (PvP_Room*)0;
    }

    /* [b28d-solo-OnlinePreliminaryMatch] ①删 name/pass/titleLen 布局对照局部：
     * ORIG 帧 0x6c，我们 0x9c 恰多 0x30（0x20+0x20+2 对齐）；ORIG 直接读
     * string 成员，无这些槽。②第 5 参 ORIG @08561050 `movzbl 0x10(%eax),%eax;
     * movzbl %al,%edi` = m_roomInfo.byte8（原写 true 字面量 = 真语义 bug）。
     * ③成功/失败块重排为 `if(err>0){失败}`：ORIG `jle 成功块(@0x1cc)`，
     * 失败路径 fall-through。 */
    int err = pvp->create_room(user,
                               (char)m_roomInfo.byte4,          // this[0xc]
                               const_cast<char*>(m_roomInfo.title.c_str()),
                               m_roomInfo.short6,               // this[0xe]
                               /* [b28d-solo-OnlinePreliminaryMatch] ORIG
                                * @08561050 两级 movzbl 无 test/setne（4.1 对
                                * uchar→bool 不归一化）；PvPTypes.h byte8
                                * uchar→bool 后同型直传复现。 */
                               m_roomInfo.byte8,                // this[0x10]
                               m_roomInfo.passwd.c_str(),       // this+0x14
                               (int)m_roomInfo.passwd.length(),
                               (PVP_BATTLE_MODE)2);
    if (err > 0)
    {
        // 失败路径（ORIG 0x85610ae 起，fall-through）
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

    // 成功路径（ORIG 0x856115e 起，jle 落点）
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

/* ORIG T 0x85612a4：vtable+0（IRoomList::AddRoom）虚调。@085612b3
 * mov 0x8(%ebp),%edx→0x4(%esp)(@+0x17)：实参为 this（与 CFairMatch 同族
 * 0x08563a32 一致），room 形参未用；具名 list 中转致 4.4 多 -0xc 槽【技巧
 * 36】→ 直调 */
/* N7 封顶（ndiff 全为寄存器换名 eax/ecx/edx）：4.4 间接 call 目标恒 %eax、
 * 4.1 用 ecx——源码不可控（负例表 N7）。NL6 2026-09-08 */
void COnlinePreliminary::_RegisterWaitingRoom(PvP_Room* /*room*/)
{
    pWaitingRoomList_->AddRoom(this);
}

// ---------------------------------------------------------------------------
// 校验族
// ---------------------------------------------------------------------------

/* ORIG T 0x08560082 前置的门禁函数 —— 见 _GetRating 家族段上方注释 */

/* ORIG T 0x085606e0（mangled 无 K → 非 const 成员） */
    // [b28d-solo-OnlinePreliminaryMatch] P248档① 翻案：调用点 CheckJoinable
    // @08560866 `call; xor $1,%eax; test %al,%al; je` ⇒ callee 返回 bool
    // （P292 表：bool 返回才出 xor 形）。旧 N96 判词作废，返回类型 int→bool。
bool COnlinePreliminary::_CheckSameTeam(PvP_Room* /*room*/, CUser* user)
{
    /* ORIG @085606f2 cmp $-1,%eax; sete %al; test %al,%al; je —— 裸 if 直判即产
     * sete+test（无独立 bool 槽，帧 sub $0x14 与 ORIG 全等）；具名 bool 反 spill 到
     * -0xa/-0x9（本件 b28 前判 N96 系误判，P168a① 裸形复测翻正）。 */
    if (user->GetCurCharacOnlinePreliminaryTeamId() == -1)
        return 0;
    /* ORIG @0856070e 第二次 call 结果直落 %ebx、@0856071b 第三 call→%eax、
     * @0856071d cmp %eax,%ebx：与 _CheckObserver 尾同族——lhs call 跨 rhs
     * call 自动落 callee-saved；具名绑定/中转折算反产 mov 链 */
    if (user->GetCurCharacOnlinePreliminaryTeamId() != GetTeamId())
        return 0;
    return 1;
}

/* ORIG T 0x08560736（mangled 无 K → 非 const 成员；第二参为 PvP_Room*，报告变量名 param_2 有误【推断修正】） */
    // [b28d-solo-OnlinePreliminaryMatch] P248档① 翻案：同 _CheckSameTeam，
    // CheckJoinable @0856088d 调用点 xor 形 ⇒ 返回类型 int→bool。
bool COnlinePreliminary::_CheckObserver(PvP_Room* room, CUser* /*user*/)
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
    /* ORIG @08560797 cmp %eax,%ebx; setge %al; test; je → mov $1 岛(@+0x71)、
     * ge 真走 fall mov $0(@+0x6a)：register bool 物化+正向 if（同
     * _CheckMatchable 尾法） */
    if (m_teams[0].size() - observers >= (int)(unsigned char)GetPlayCountPerOneTeam())
        return 0;
    return 1;
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
        /* ORIG @08561327 firstTeamId==0 分支内独立 call(@+0x6d) 赋 -0x14 后
         * jmp 增量块(@+0x75)；else @+0x7d 再次 call + cmp -0x14,%eax; setne
         * 物化(@+0x85【技巧22】)；log 分支 @+0x92 第三次 call */
        if (firstTeamId == 0)
        {
            firstTeamId = u->GetCurCharacOnlinePreliminaryTeamId();
            continue;
        }
        /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08561348 `cmp -0x14(%ebp),%eax;
         * setne %al; test %al,%al; je`（物化后直测、无槽）；具名 bool 局部在
         * 4.4 恒落 -0x9 槽（mov%al,-N;cmpb），故写裸比较表达式（P168a② 直测形）。
         * ORIG test %al,%al; je→增量块(@+0xcf)，log 体走 fall-through(@+0x8c)：
         * 正向 if 不取反。 */
        if (u->GetCurCharacOnlinePreliminaryTeamId() != firstTeamId)
        {
        /* ORIG @08561352 第三次 call 结果直落 0x14(%esp)(@+0x97)，无具名
         * 中转折算槽【技巧36/37】 */
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__,
            0x771, "(%d) found a different team_id(%d)",
            u->GetCurCharacOnlinePreliminaryTeamId());
        return 0x16;
        }
    }
    /* ORIG 尾 @085613c5 setne 物化 + 双常量岛(@0x16/jmp/mov$0)；
     * byte 化用 (unsigned char) 强转产 movzbl(@085613c0) */
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @085613c0 `movzbl %al,%eax;
     * cmp %eax,%ebx; setne %al; test %al,%al; je`（尾物化无槽）⇒ 裸比较。 */
    if (m_teams[0].size() - observers != (int)(unsigned char)GetPlayCountPerOneTeam())
        return 0x16;
    return 0;
}

/* ORIG T 0x085607b2（switch 表 @0x8ca230c 实证） */
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE/P248档①] 旧 N96 判词作废：
 * _CheckSameTeam/_CheckObserver 返回类型 int→bool（OnlinePreliminary.h，
 * R2 已广播）+ 裸 `if(!call) return 4;` ⇒ 复现 ORIG @08560866/@0856088d
 * `call; xor $1,%eax; test %al,%al; je`，帧 0x14 与 85=85 全等（旧 uchar
 * 栈化三段 + 帧 0x24 一并消）。 */
int COnlinePreliminary::CheckJoinable(PvP_Room* room, CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 1;

    /* ORIG @08560862/@085608?? movzbl %al,%eax（(unsigned char) 强转形，
     * `& 0xff` 出 and $0xff）+ add %eax,%eax 两段，size 累加走 add %eax,%ebx */
    if (room->get_player_count() >= (int)(unsigned char)GetPlayCountPerOneTeam() * 2)
        return 4;

    /* ORIG @085608xx sum(两 size call)→%ebx 先、再第 2 次 pc call：
     * lhs=sum 正向 `>`（cmp %eax,%ebx; setg） */
    if (m_teams[0].size() + m_teams[1].size() >
        (int)(unsigned char)GetPlayCountPerOneTeam() * 2)
        return 4;

    switch (m_field04)
    {
    case 0:
        /* [b28d-solo-OnlinePreliminaryMatch] P248档①：_CheckSameTeam/
         * _CheckObserver 返回类型 int→bool 后，裸 `if(!call) return 4;`
         * 直产 ORIG @08560866/@0856088d 的 `call; xor $1,%eax; test %al,%al;
         * je`（旧 uchar 栈化三段 mov%al,-N;xorb$1,-N;cmpb 与帧 +0x10 一并消）。 */
        if (!_CheckSameTeam(room, user))
            return 4;
        if (!_CheckObserver(room, user))
            return 4;
        return 0;
    case 3:                     /* 叶岛序（P46：叶排放=case 书写序，跳表
                                 * @8ca230c：3→0 岛、1/4/5→共享 4 岛、
                                 * 2/default→共享 4 岛） */
        return 0;
    case 1:                     /* 1/4/5 标签叠放：4.1 同体 case 合并共享岛
                                 * （跳表 1/4/5→同一块），4.4 需显式叠放 */
    case 4:
    case 5:
        return 4;
    default:
        return 4;
    }
}

/* ORIG T 0x085608bc：cmpl $2 / jne else(@+0x10)；then 体 mov $0 fall-through
 * (@085608c5)，else 体 mov $1 (@085608cc) → !=2 判定块在前【技巧17】 */
int COnlinePreliminary::CheckSeat(int a)
{
    /* ORIG @085608c3 cmpl $2 / jne +0x10(mov $1)、fall-through mov $0、jmp +0x15(leave)
     * ——守卫早返 `if(a==2)return 0;`+尾 `return 1;` 精确复现（三目被折叠成 setne，goto 形
     * 多一 nop 中继；P133 独立守卫形）。 */
    if (a == 2)
        return 0;
    return 1;
}

/* ORIG T 0x085623ec；N7 封顶（虚调用寄存器换名，见 _RegisterWaitingRoom 注） */
bool COnlinePreliminary::CheckMatchable(IMatch* other)
{
    /* ORIG 槽序 selfUser@-0x14 / otherUser@-0x10 / 共享 i@-0xc(@08562434
     * 声明+双循环 entry init 三写同 i 槽)；declaration order = far→near */
    CUser* selfUser = 0;
    CUser* otherUser = 0;
    int i;                     /* ORIG entry 仅 for-init 单写 i(@+0x15) */
    for (i = 0; i <= 3; ++i)
    {
        otherUser = other->GetUser(i);
        if (otherUser != 0)
            break;
    }
    if (otherUser == 0)
        return false;
    for (i = 0; i <= 3; ++i)
    {
        selfUser = GetUser(i);
        if (selfUser != 0)
            break;
    }
    if (selfUser == 0)
        return false;

    // ORIG 0x085624c3 有符号 setge 比较（count >= limit → false）；
    // call 结果免 played 槽直 cmp
    if ((int)_GetEachOtherPlayCount(selfUser, otherUser) >=
        G_CDataManager()->m_serverParameter.GetDailyMatchCount())
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

/* ORIG T 0x08560082：f 槽(-0xc)无 entry 初始化；else 路径 @085600e8
 * `mov $0,%eax` 单物化；尾 merge `mov %eax,-0xc; flds -0xc`(@085600ed) */
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE] 旧 DIFF/N85③ 判词作废。真因
 * 是**具名 float 局部 + return f** 使 4.4 另开 -0x1c 返回临时槽（帧 0x38）；
 * 改为**各分支直接 return**（无具名局部）后返回槽与 -0xc 合一，逐条复现
 * ORIG：各臂 `fstps -0xc; mov -0xc,%eax; jmp merge` + 尾
 * `mov %eax,-0xc; flds -0xc`，帧 0x28（探针 /tmp/s190/p_gr.cpp fB 全等；
 * fA/fC/fD 具名形恒多 -0x1c）。语义等价（返回值即各分支表达式）。 */
float COnlinePreliminary::_GetRating(CUser* user)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
        return reinterpret_cast<COnlinePreliminaryMember*>(
                   user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)4))->GetRating();
    if (user->GetCurCharacOnlinePreliminaryTeam() != 0)
        return user->GetCurCharacOnlinePreliminaryTeam()->GetRating();
    return 0.0f;
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
    // ORIG 0x08560be2：两 float 顶部声明 =0.0f（入口预清 -0xc/-0x10），
    // 双臂 _GetRating 序 (自,对) 互换 + CEloRating::calc(引用双参) +
    // _SetRating(队,自评)。原按值/selfTeam 三局部结构不收敛。
    float selfRating = 0.0f;
    float otherRating = 0.0f;
    if (winOrLose == 1)
    {
        selfRating = (float)_GetRating((ENUM_SCHOOL_MATCH_TEAM)0);
        otherRating = (float)_GetRating((ENUM_SCHOOL_MATCH_TEAM)1);
        CEloRating::calc(selfRating, otherRating, sys,
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        _SetRating((ENUM_SCHOOL_MATCH_TEAM)0, selfRating);
    }
    else
    {
        selfRating = (float)_GetRating((ENUM_SCHOOL_MATCH_TEAM)1);
        otherRating = (float)_GetRating((ENUM_SCHOOL_MATCH_TEAM)0);
        CEloRating::calc(selfRating, otherRating, sys,
                         PvPGlobalEnvironmentVariable::school_match_elo_k_);
        _SetRating((ENUM_SCHOOL_MATCH_TEAM)1, selfRating);
    }
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
    /* [b28d-solo-OnlinePreliminaryMatch 判词·NEAR/N82附] 帧/槽/指令 51=51
     * 全等；残 4 条 = getCurCharacNo→%esi、GetServerGroup→%ebx（ORIG 反之，
     * 即 %ebx↔%esi 汇聚对调），callee-saved 角色分配源形不可控（N82附）。 */
unsigned int COnlinePreliminary::_GetEachOtherPlayCount(CUser* a, CUser* b)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
    {
        // 在 a 的“对过谁”表（charac_expand(5)）中记/查 b 的 (serverGroup,characNo)
        /* ORIG @0856022e gcn(b)→%ebx、@0856023b gsg(b)→%esi 跨 expand call 保全
         * （push esi;push ebx;sub $0x10）——P52 单语句全内联实参形复现（具名 no/grp
         * 反落栈槽退化 sub $0x24，本件 b28 N96 系误判，P0 内联复测翻正）。 */
        return reinterpret_cast<COnlinePreliminaryMemberMatchList*>(
                   a->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)5))
                   ->GetPeerPlayCount(b->GetServerGroup(), b->getCurCharacNo());
    }
    /* ORIG @08560272 test;setne %al;test;je→mov $0 岛(@+0x8f)：!=0 正向 */
    if (a->GetCurCharacOnlinePreliminaryTeam() != 0)
        return a->GetCurCharacOnlinePreliminaryTeam()->GetEachOtherTeamPlayCount(
            b->GetCurCharacOnlinePreliminaryTeamId());
    return 0;
}

/* ORIG T 0x0856050e：取两队在场的最后一名成员查询 */
unsigned int COnlinePreliminary::_GetEachOtherPlayCount()
{
    /* ORIG @085605f4 槽序 lastA=-0x14/lastB=-0x10/i=-0xc；两循环共享同一
     * 函数级 i（@+0x22 与 @+0x74 两次 i=0 初始化同一槽）*/
    CUser* lastA = 0;
    CUser* lastB = 0;
    int i = 0;                                     /* @08560522 声明+for 双写 */
    for (i = 0; i <= 3; ++i)
    {
        if (m_teams[0].m_seats[i].m_flag != 0 &&
            m_teams[0].m_seats[i].m_user != 0)
            lastA = m_teams[0].m_seats[i].m_user;
    }
    for (i = 0; i <= 3; ++i)
    {
        if (m_teams[1].m_seats[i].m_flag != 0 &&
            m_teams[1].m_seats[i].m_user != 0)
            lastB = m_teams[1].m_seats[i].m_user;
    }
    /* ORIG @085605bf/0x5c5 两独立 je→mov $0 岛(@+0x8f)：|| + goto 合并双直
     * 跳，消除 4.4 单 goto 的跳板块【技巧23 变体】 */
    if (lastA == 0 || lastB == 0)
        goto RET0;
    return _GetEachOtherPlayCount(lastA, lastB);
RET0:
    return 0;
}

/* ORIG T 0x085602ae */
    /* [b28d-solo-OnlinePreliminaryMatch 判词·NEAR/N82附] 49=49 全等；残 4
     * 条 = %ebx↔%esi 汇聚对调（同 _GetEachOtherPlayCount(CUser*,CUser*)）。 */
void COnlinePreliminary::_IncreaseEachOtherPlayCount(CUser* a, CUser* b)
{
    if ((char)GetPlayCountPerOneTeam() == 1)
    {
        /* ORIG @085602ca gcn(b)→%ebx、@085602d7 gsg(b)→%esi 跨 expand call 保全
         * ——P52 单语句全内联实参形复现（具名 no/grp 反退化帧，b28 N96 系误判）。 */
        reinterpret_cast<COnlinePreliminaryMemberMatchList*>(
            a->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)5))
            ->IncreasePeerPlayCount(b->GetServerGroup(), b->getCurCharacNo());
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
    /* 同 _GetEachOtherPlayCount()：共享函数级 i【ORIG 085605ee @+0x14/@+0x1b
     * 双写同槽】 */
    CUser* lastA = 0;
    CUser* lastB = 0;
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        if (m_teams[0].m_seats[i].m_flag != 0 &&
            m_teams[0].m_seats[i].m_user != 0)
            lastA = m_teams[0].m_seats[i].m_user;
    }
    for (i = 0; i <= 3; ++i)
    {
        if (m_teams[1].m_seats[i].m_flag != 0 &&
            m_teams[1].m_seats[i].m_user != 0)
            lastB = m_teams[1].m_seats[i].m_user;
    }
    /* ORIG @0856069f/0x6a5 双 je 直指 epilogue 无空块岛：正向 && 嵌套
     * 【技巧11】 */
    if (lastA != 0 && lastB != 0)
    {
        _IncreaseEachOtherPlayCount(lastA, lastB);
        _IncreaseEachOtherPlayCount(lastB, lastA);
    }
}

// ---------------------------------------------------------------------------
// 装备合法性
// ---------------------------------------------------------------------------

/* ORIG T 0x08560920 */
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE] 旧 NEAR/N82附 判词作废。本波修：
 * ①删 equip 具名局部（ORIG 全函数仅一个 CItem* 槽 -0xc，equip 每次重读）；
 * ②尾改 `if(!CheckUnusableItem(...)) return false; return true;`（复现 ORIG
 * xor$1/test/je 物化）；③CheckUnusableEquipmentInfo 两实参 callee-saved 角色
 * （ORIG 先求值 getUsableLevel→%esi、后 GetUsableEquipmentType→%ebx；4.4 恒
 * 反）用**第二实参语句表达式** `({ found->getUsableLevel(); })` 翻正（P226
 * 实参向；探针 /tmp/s190/p_ces.cpp v4 逐条复现 ORIG，v0/v1/v2/v3 均反）。
 * 语义等价（值即调用结果，无具名变量）。 */
bool COnlinePreliminary::CheckUnusableEquipment(const Inven_Item* item)
{
    if (item == 0)
        return false;
    if (item->m_addInfo == 0)
        return true;

    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（find_item /
    // CheckUnusableEquipmentInfo / CheckUnusableItem 共 3 次）；equipType 也
    // 不在比较与实参间缓存（GetUsableEquipmentType 共 2 次）。
    CItem* found = G_CDataManager()->find_item(item->m_addInfo);
    if (found == 0)
        return false;
    if (found->is_stackable())
        return false;

    /* [b28d-solo-OnlinePreliminaryMatch] ORIG 全函数只有一个 CItem* 槽 -0xc
     * （equip 每次从同一槽重读，无独立 -0x10 槽）⇒ 原码无 equip 具名局部，
     * 删除后两处调用均按 ORIG 从 -0xc 重读（帧 0x20/槽号对齐）。 */
    if (static_cast<CEquipItem*>(found)->GetUsableEquipmentType() == 0)  // W 0x832e036
        return true;
    if (!G_CDataManager()->m_serverParameter.CheckUnusableEquipmentInfo(
            /* [b28d-solo-OnlinePreliminaryMatch] ORIG 寄存器角色：先求值
             * getUsableLevel→%esi、后求值 GetUsableEquipmentType→%ebx；
             * 4.4 裸形恒反 ⇒ 第二实参包语句表达式（P226 实参向）。 */
            (ENUM_UNUSABLE_EQUIPMENT_TYPE)static_cast<CEquipItem*>(found)->GetUsableEquipmentType(),
            ({ found->getUsableLevel(); })))
        return false;
    /* ORIG 经虚表槽 +0xc（GetItemType）取类型后查 CheckUnusableItem；
     * [b28d-solo-OnlinePreliminaryMatch] ORIG @08560a23 尾为 `call; xor $1,%eax;
     * test %al,%al; je <mov $1 岛>; mov $0,%eax; jmp` ⇒ 源形是
     * `if(!call) return false; return true;`（直接 `return call;` 无物化）。 */
    if (!G_CDataManager()->m_serverParameter.CheckUnusableItem(
                 (ENUM_EQUIPMENTTYPE)found->GetItemType(),
                                                  (unsigned long)found->get_index()))
        return false;
    return true;
}

/* ORIG T 0x08560a42：装备栏 0..22 槽全检 */
bool COnlinePreliminary::CheckUnusableEquipmentInUser(CUser* user)
{
    for (int slot = 0; slot <= 0x16; ++slot)
    {
        const Inven_Item* item =
            user->getCurCharacInvenR()->GetInvenRef(0, slot);   // T 0x84fc1de
        /* ORIG @08560a7f xor $1,%eax / test / je 续循环；msg 免槽直用
         * (@08560ab4 mov %eax,0x4(%esp)【技巧37】) */
        if (!CheckUnusableEquipment(item))
        {
            user->SendNotiPacketMessage(g_scriptStringManager_.findString(
                4, "game_server_msg_145", (bool*)0), (ENUM_MESSAGE_TYPE)0);
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
    m_teams[0].m_rating = _GetRating(user);
    return 0;                                     /* ORIG mov $0x0,%eax（0x8560b83） */
}

/* ORIG T 0x08560b8a；N7 封顶（虚调用寄存器换名，见 _RegisterWaitingRoom 注） */
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

/* ORIG T 0x08560cd2：第 4 参为离场者队伍组值（OnWalkOut 以 get_team(seat) 传入）；
 * N7 封顶（虚调用寄存器换名，见 _RegisterWaitingRoom 注） */
// [b28 判词 N7] 残差=间接 call 目标寄存器 %ecx↔%eax（4.4 恒 %eax）+伴随寄存对调，
// 主体 83 行逐条同形，NEAR 封顶。
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
    // [b28 判词 N96+P168a④] X2 寄存器 xor/钉值物化形需 register 绑定；绑定 P0 全域禁用后无源形，N 封顶。

int COnlinePreliminary::OnRegisterMatch(CUser* user, PvP_Room* room)
{
    if (m_field04 == 1)
        return 1;

    if (!bMatchable_)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc7, 0x13);
        return 0;
    }

    /* ORIG @0856142d 声明初始化 `int err = 0;` 独立于 call 赋值(@+0x68)，
     * @+0x6b cmpl;setne %al;test 物化【技巧22】 */
    int err = 0;
    err = _CheckMatchable(room);
    /* ORIG @08561443 cmpl $0,-0xc; setne %al; test; je（bool 四段物化，无独立 bool 槽）
     * ——P225 while+break 形精确复现（具名 bool 会另开 -0x9 槽破坏帧）。 */
    while (err != 0)
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
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE] 旧 NEAR/N95 判词作废。历史修：
 * ①5 处 cMyTrace 改全表达式临时链 + __PRETTY_FUNCTION__（P221/P139，复现
 * ORIG -0x6c/-0x5c/-0x4c/-0x3c/-0x2c 深→浅槽序）；②oppUser/oppRoom/opponent
 * 全头部声明 =0（P212）；③team 提至循环外（P212）；④`& 0xff`→
 * `(unsigned char)`（消 and）；⑤size 比较方向改 `size > pc`（复现 jg）；
 * ⑥gen_timer_key 内联实参（P24，消 key 槽往返）。
 * 本波收口（全帧槽号 -4 根因，302=302 全等）：⑦循环内 user 不再另起
 * `CUser* u` 局部，直接复用 oppUser（ORIG @085618b1 `mov %eax,-0x1c(%ebp)`
 * 实证槽复用；另起局部多占 -0x10 使全帧槽深 4——N95 判词的真因，非槽基不可
 * 上移）；⑧真语义 bug：第一处 get_player_count 的接收者 ORIG 为**形参 room**
 * （@085616e5 `mov 0xc(%ebp),%eax`，Ghidra C `get_player_count(param_1)` 同
 * 证），原写 oppRoom 查错对象，已改 room。 */
int COnlinePreliminary::OnMatchRoom(PvP_Room* room, int authNo)
{
    if (m_authValue != authNo)
    {
        /* [b28d-solo-OnlinePreliminaryMatch·P221] ORIG 5 个 cMyTrace 槽
         * -0x6c/-0x5c/-0x4c/-0x3c/-0x2c（深→浅），4.4 对具名对象恒反向
         * （-0x38/-0x48/...）；改全表达式临时链复现 ORIG 槽深（P221 实证）。
         * 第 1 参用 __PRETTY_FUNCTION__（P139：ORIG 符号
         * _ZZ...OnMatchRoom...E19__PRETTY_FUNCTION__，手写串不生成该局部符号）。 */
        cMyTrace(__PRETTY_FUNCTION__, 0x7a6, 0)("OnMatchRoom timeout");
        return 0;
    }
    if (m_field04 != 1)
        return 0;

    /* [b28d-solo-OnlinePreliminaryMatch·P212 全头部声明] ORIG @08561555-0x79
     * 三条初始化 `movl $0,-0x1c/-0x18/-0x70`（oppUser/oppRoom/opponent 均在
     * 函数头声明 =0），使用点仅赋值 ⇒ 局部提至函数头。 */
    CUser* oppUser = 0;
    PvP_Room* oppRoom = 0;
    IMatch* opponent = 0;
    pWaitingRoomList_->Match(this, &opponent);            /* vtable+8 虚调 */
    if (opponent == 0)
    {
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    oppUser = opponent->GetUser(0);                       /* vt+0x10 */
    if (oppUser == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7c1, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x7c1);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    oppRoom = (PvP_Room*)oppUser->GetPVPRoom();                      /* T 0x865518e */
    if (oppRoom == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7ca, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x7ca);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG `movzbl %al,%eax` 形 ⇒
     * `(unsigned char)` 强转（`& 0xff` 恒产 and $0xff）。
     * [b28d-solo-OnlinePreliminaryMatch 真语义 bug 修] ORIG @085616e5
     * `mov 0xc(%ebp),%eax` = 形参 room（非 local_1c/oppRoom；Ghidra C 同证
     * `get_player_count(param_1)`）——原写 oppRoom 查错对象。 */
    if (room->get_player_count() > (int)(unsigned char)GetPlayCountPerOneTeam())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7d2, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x7d2);
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG `movzbl %al,%eax` 形 ⇒
     * `(unsigned char)` 强转（`& 0xff` 恒产 and $0xff）。 */
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08561790 `cmp %eax,%ebx;
     * jg`（ebx=size 在左）⇒ 比较方向必须写 size > pc（写成 pc < size 产 jl）。 */
    if (m_teams[0].size() > (int)(unsigned char)GetPlayCountPerOneTeam() ||
        oppRoom->get_player_count() > (int)(unsigned char)GetPlayCountPerOneTeam())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x7da, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x7da);
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

    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @0856188c-0x93 循环前双初始化
     * `movl $0,-0x14`（team）+ `movl $0,-0x10`（s）⇒ team 是循环外声明
     * （P212），循环内仅赋值。 */
    int team = 0;
    for (unsigned int s = 0; s <= 7; ++s)
    {
        /* [b28d-solo-OnlinePreliminaryMatch] ORIG @085618b1 `mov %eax,
         * -0x1c(%ebp)`：循环内 user 复用 oppUser 槽（ORIG 全函数仅 5 个浅槽
         * -0xc/-0x10/-0x14/-0x18/-0x1c 承载 6 个局部）⇒ 原码就是复用 oppUser
         * 变量本身（oppUser 在取到 oppRoom 后已死）。另起 `CUser* u` 会多占
         * -0x10 槽使全帧槽号整体深 4（帧同为 0x90 但槽基 -4）。 */
        oppUser = reinterpret_cast<CUser*>(oppRoom->get_user_seat((int)s));
        if (oppUser == 0)
            continue;
        team = oppRoom->get_team((int)s);
        _PullOutRoom(oppUser, oppRoom, (int)s);
        /* [b28d-solo-OnlinePreliminaryMatch·P218/P290] ORIG @0856191d-0x41
         * 压参序为「先压常量 2/3 再装载 newSeat」；unsigned 局部使 4.4 提前
         * 装载 %eax。声明改 int 复现 ORIG 装载时刻。 */
        int newSeat = _PutInRoom(oppUser, room, false);
        if (team == 3)
            room->OnSetPVPSeatState(oppUser, newSeat,
                                    (ENUM_SEAT_STATE)3, (ENUM_WALKOUT_TYPE)2);
    }

    pWaitingRoomList_->DelRoom(this);
    m_field04 = 4;
    _SendMatchStatePacket(2, 2);
    m_p2pReadyCount = 0;

    /* [b28d-solo-OnlinePreliminaryMatch·P24] ORIG @085619b6-0x510 无 key 具名
     * 槽：gen_timer_key 结果直落 %esi 跨 get_index/G_TimerQueue call 保活 ⇒
     * 实参内联（具名局部会多 -0x14 槽往返 2 条）。 */
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)5, room->get_index(),
                                (TIMER_MESSAGE)0x3b, 0xb4,
                                (int)room->gen_timer_key((TIMER_MESSAGE)0x3b), 0);
    return 1;
}

/* ORIG T 0x08561a52 */
void COnlinePreliminary::OnStartFight()
{
    m_field04 = 5;
    _IncreaseEachOtherPlayCount();
}

/* ORIG T 0x08562022 */
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE] 旧 DIFF/N75(N=2) 判词作废。
 * 唯一残差曾是 ORIG @08562040-0x29「`mov -0xc(%ebp),%eax`（managerTeam）
 * 先于 `movl $0x0,0xc(%esp)`（第3参常量）」而 4.4 恒 R2L 先压常量。
 * 翻正旋钮=**实参来源局部改 unsigned int**（P248 档③ 变体）：实参表达式带
 * 隐式 int 转换后 4.4 判其需预求值寄存器，先发 `mov -0xc(%ebp),%eax` 再压
 * 常量，逐条复现 ORIG（探针 /tmp/s190/p_arg4.cpp v17：int 局部恒常量先、
 * unsigned 局部则值先；v12 enum 同效）。值域 0..2，`!=1` 为等值比较，
 * 语义/槽位/帧零变化。 */
void COnlinePreliminary::OnEndFight(PvP_Room* room)
{
    unsigned int managerTeam = room->get_manager_team();  /* T 0x85d9242 */
    m_field04 = 0;
    _ReturnTeam(room, managerTeam, (CUser*)0);
    /* ORIG @08562067 十对字 block-move（%eax=LHS %edx=RHS 双 this 装载、
     * 末对复用 %edx）= 结构体整体赋值【GetUserList 同法】 */
    if (managerTeam != 1)
        m_teams[0] = m_teams[1];
    m_teams[0].refresh();                                 /* T 0x855fcf6 */
    m_teams[1].reset();
}

/* ORIG T 0x08561f8c（vtable+0x2c＝OnLeaveRoom 级联）。ORIG 返回 1。 */
/* [b28d-solo-OnlinePreliminaryMatch 判词·NEAR] 本波修真语义 bug：OnLeaveRoom
 * 实参序 (u,room,a,team)→(u,room,team,a)（ORIG @08561fc6-0x5d 压参序实证，
 * 原第 3 参 teamGroup 收到座位号）。
 * [b28d-solo-OnlinePreliminaryMatch] 另修实参装载序：ORIG @08561fc6 先
 * `mov -0x10(%ebp),%eax`（team）再 `mov 0x10(%ebp),%edx`（a）；4.4 恒 R2L
 * 逐参 load+store。旋钮=team 局部改 unsigned int（P248 档③ 变体，探针
 * /tmp/s190/p_arg4.cpp v17 实证）：实参隐式 int 转换使 4.4 预求值 team 入
 * %eax 再压 a，逐条复现 ORIG 装载序。
 * 残 N7 虚调目标 %ecx↔%edx（4.4.6 恒 %edx，ORIG 4.1 上下文用 %ecx）。 */
int COnlinePreliminary::OnWalkOut(PvP_Room* room, int a)
{
    unsigned int team = room->get_team(a);
    CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(a));
    /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08561fc6-0x5d 压参序实证：
     * 0xc(%esp)=team(-0x10 槽)、0x10(%esp)=a(0x10(%ebp)) ⇒ 第 3 参是
     * get_team(a) 的 team 值、第 4 参是座位号 a。原写 (a, team) 使
     * OnLeaveRoom 的 teamGroup 收到座位号 = 真语义 bug；按 ORIG 交换。 */
    OnLeaveRoom(u, room, team, a);
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
            /* ORIG 内层尾 call 后无跳板直落 addl：双 je 直指循环增量块
             * （正向 && 【技巧11】，同 _IncreaseEachOtherPlayCount()） */
            if (m_teams[t].m_seats[i].m_flag != 0 && u != 0)
                u->Send(pkt);
        }
    }
}

/* ORIG T 0x08561dee */
void COnlinePreliminary::OnConnectP2PTimeout(PvP_Room* room)
{
    // b28：ORIG 单 leave 无中继（jne→leave；body 尾 call 直落）。4.4 对
    // guard-return 形在 body 尾产 `jmp leave; nop` 中继（P63）；改正极性
    // if 共享统一出口消岛（DIFF15→NEAR）。
    // [b28 判词 N7] 残差=call *%edx↔%eax 换名 + 实参链寄存器对调，封顶。
    if (m_field04 == 4)
    {
        _SendMatchStatePacket(4, 2);
        OnEndFight(room);                                /* vtable+0x40 虚调实证 */
    }
}

// ORIG T 0x08561b80
/* [b28d-solo-OnlinePreliminaryMatch 判词·NEAR/N7] 语义与 ORIG 全等。
 * 本波修：①`(int)(GetPlayCountPerOneTeam() & 0xff)` → `(unsigned char)`
 * 强转（消 and $0xff，复现 ORIG movzbl）；②ready 发送循环
 * `if(flag==0||u==0)continue` → 正向 `if(flag!=0&&u!=0)Send`（消 xor$1 物化
 * + 双 jmp/nop 岛，复现 ORIG 双 je 直指增量块）；③两 PacketGuard 槽方向：
 * ORIG ready=-0x28/st=-0x34，4.4 对「if 块内对象 + 函数体对象」恒反（N93）；
 * 把 st 包进**尾随复合块**（scope_probe /tmp/s190/p_guard.cpp a2：同为块级
 * 对象后 4.4 按声明序给出 ready 浅/st 深，逐条复现 ORIG）——ctor/dtor 时序
 * 与语句序零变化（块即函数末段）。
 * 残 N7 虚调目标 %edx↔%eax。 */
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
        (int)(unsigned char)GetPlayCountPerOneTeam() * 2)
    {
        PacketGuard ready;
        ready.put_header(0, 0x117);
        ready.finalize(true);
        for (int t = 0; t <= 1; ++t)
        {
            for (int i = 0; i <= 3; ++i)
            {
                CUser* u = m_teams[t].m_seats[i].m_user;
                /* [b28d-solo-OnlinePreliminaryMatch] ORIG @08561cb0-0x151：
                 * `test %al,%al; je skip` + `cmpl $0,-0x10; je skip`（双 je
                 * 直指增量块、无 setne/无岛）⇒ 正向 && 单语句（P296/技巧11）；
                 * 旧 `==0 || ==0 continue` 产 xor$1 物化 + 双 jmp/nop 岛。 */
                if (m_teams[t].m_seats[i].m_flag != 0 && u != 0)
                    u->Send(ready);
            }
        }
    }

    {
        PacketGuard st;
        st.clear();
        room->battle_reset();                             /* T 0x85d6430 */
        for (int s = 0; s <= 7; ++s)
        {
            if (room->get_user_seat(s) != 0)
                room->m_relayMgr.OnReady(s, true);        /* +0x620 子对象 */
        }
        room->start_pvp();                                /* T 0x85d8c3a */
        // ORIG：make_state_info 写入 st 自身缓冲（Ghidra local_38 复用，ORIG `(char*)local_38`），
        // send_all(st) 发送同一包。
        room->make_state_info(reinterpret_cast<char*>(&st));  /* T 0x85dbd3a */
        G_GameWorld()->send_all(st);
    }
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
    /* ORIG @08561a14 float 局部 0.0f 两段物化到 -0x8【技巧43】；具名指针
     * 中转 other→-0x4(@08561a1c)【技巧13】；首 call 结果 fstps -0x14、次
     * call 结果免 spill 直接 fsubrs(@08561a4b) */
    float theirs = 0.0f;   // ORIG -0x8 死局部（写 0 后未被读）
    COnlinePreliminary* o = static_cast<COnlinePreliminary*>(other);
    return fabsf(_GetRating(static_cast<ENUM_SCHOOL_MATCH_TEAM>(0))
                 - o->_GetRating(static_cast<ENUM_SCHOOL_MATCH_TEAM>(0)));
}

/* ORIG T 0x085620c8：仅 team0 前四席视图，占用且 state==1 才有效 */
    // [b28 判词 N96+P168a④] X2 寄存器 xor/钉值物化形需 register 绑定；绑定 P0 全域禁用后无源形，N 封顶。
CUser* COnlinePreliminary::GetUser(unsigned int idx)
{
    if (idx >= 4)
        return 0;
    /* ORIG @08560e6：无独立 bool 槽（无 sub 帧），field==1 判据。4.4 对
     * `if(m_flag==1)` 产 `cmp $1,%al; jne`，ORIG 为 `xor $1,%eax; test %al,%al; je`——
     * [b28b 判词 N96+P168a④] xor $1 物化（非 cmp）需 register 绑定，P0 禁用后
     * 4 形裸测（==1 / !=1 / ^1!=0 / (uchar)v^=1）恒折 cmp；帧/槽已对齐（无 sub），仅余 3 条。 */
    if ((m_teams[0].m_seats[idx].m_flag ^ 1) != 0)
        return 0;
    return m_teams[0].m_seats[idx].m_user;
}

/* ORIG T 0x08562104（docs/class_func_reports/online_preliminary__COnlinePreliminary/
 * GetUserList.md）：将 this+0x1c..+0x40 即 m_teams[0]（TeamInfoSecond，0x28 字节）
 * 按 10 个 dword 展开逐字复制到出参。 */
void COnlinePreliminary::GetUserList(void* out)
{
    /* ORIG @08562107 一次性装载 out→%eax / this→%edx，随后 10 对字
     * mov disp(%edx),%ecx / mov %ecx,disp(%eax)（末对复用 %edx）——4.1 O0
     * 结构体整体赋值的 block-move 形；用 0x28 POD 视图结构赋值复现 */
    struct TeamView28 { unsigned int w[10]; };  // 10 dword = m_teams[0] 全貌
    /* pad+成员 形式使 0x1c 折进每条 disp（ORIG @+0x09 mov 0x1c(%edx),%ecx
     * 无独立 add） */
    struct TeamGap { char g[0x1c]; TeamView28 v; };
    *static_cast<TeamView28*>(out) =
        reinterpret_cast<const TeamGap*>(this)->v;
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
            /* ORIG 内层尾 call 后无跳板直落 addl：双 je 直指循环增量块
             * （正向 && 【技巧11】，同 _IncreaseEachOtherPlayCount()） */
            if (m_teams[t].m_seats[i].m_flag != 0 && u != 0)
                u->Send(pkt);
        }
    }
}

} // namespace online_preliminary
