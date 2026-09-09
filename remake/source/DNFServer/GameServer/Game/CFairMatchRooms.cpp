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
//   +0x0b8 m_leaderSeat[0]  队0 队长座位（OnMatchRoom 存 GetManagerSeat()；
//                           OnAskRematch 存队伍重赛决定，双用【推断】）
//   +0x0bc m_leaderSeat[1]  队1 队长座位（OnMatchRoom 尾 get_user_seat(房主)；
//                           双用同上）
//   +0x0c0 m_fieldC0        IMatch* next match（GetNextMatch/SetNextMatch 槽）
//
// 等待表交互均经 pWaitingRoomList_（IRoomList）虚槽：AddRoom/DelRoom/Match。
//
// ============================================================================
// b28d-solo-CFairMatchRooms（本波）判词汇总（逐件 fresh 复测，p167 全 CLEAR）
//   UpdateScore        DIFF→NEAR ndiff 133→32：P296 嵌套正向 if 消 m_flag 的
//                      xor $1 物化（ORIG `movzbl; test %al,%al; je` 直测）；
//                      P171 补 killCnt 死写入（ORIG `mov %eax,-0x44`）；
//                      P0 补 missionList 具名局部（ORIG `mov %eax,-0x1c`）；
//                      P3 cond 实参 `(unsigned char)` 收窄复现 `cmp $0x1,%al`。
//                      残差 = 深区临时槽深 0x10 + 帧 0x7c vs 0x6c（N115/N82）。
//   GiveTrainingSeal   DIFF ndiff 226→3：删 bool ok 改双短路（ORIG jle/jbe +
//                      mov$1/mov$0 汇聚）；bonusRemain bool→char（S1/P321-T3，
//                      复现 4.1 `cmpb $0x0,-0x11`）；P222 正向包裹翻块序（ORIG
//                      bonusRemain==0 块在尾）；P0 内联 getCurCharacInvenW（去
//                      inven 槽）；vector 声明序 updatedSlots 前置（ORIG 构造序
//                      -0x30→-0x3c）。残 3 = N121 epilogue 尾岛。
//   OnAskRematch       DIFF ndiff 244→36：删具名 bool reject 改 P226 语句表达式
//                      （复现 ORIG 无槽 `mov $1/mov $0` 岛 + test/je，槽整体回正
//                      -0x24/-0x20/-0x1c）；if 链→switch（P251/P286，复现形参
//                      载入 + `test;js;cmp$1;jle;cmp$2;je` 决策链）。残 = 伪差 +
//                      1 处 `add $0x20` vs `lea 0x20`（N107 族）。
//   OnRematchTimeout   DIFF ndiff 248→~90：P0 内联 CheckRematch 为最右 vararg
//                      （去 rematchable 槽）+ P3 `(unsigned char)` 收窄复现
//                      `movzbl %al,%edi`；P0 内联 room->m_relayMgr（去 relay 引用
//                      槽，帧 0x7c→0x6c 对齐）；team0Decision 具名槽→switch
//                      （P251/P261，复现 `test;je;test;js;cmp$2;jg`）。残 = 槽号
//                      分区 + `CheckRematch==1` 的 xor/sete（跨版本）+ N104 EH 段。
//   _ReturnTeam        NEAR 4（N）：CSwitchLog varargs 的 %esi/%edi 分配互换
//                      （ORIG 第1值→esi、4.4→edi），P75/N82附 跨版本不可择。
//   CheckJoinable      NEAR 4（N）：两处 `pc>=X`/`total>X` 被 4.4 折成反向比较
//                      （cmp %ebx,%eax;setle vs ORIG cmp %eax,%ebx;setge），
//                      两向源形实测同形（N196）。
//   OnWalkOut          NEAR 5（N）：五实参装载时刻 + 间接 call 目标 %ecx vs %eax
//                      （N7/N82附）；绑定形实测净负。
//   _PullOutRoomTeam   DIFF 2（N）：ORIG body 尾 `jmp <紧邻>` 空岛被 4.4
//                      jump-thread（N121）。
//   _CheckMovableRoom  DIFF 5（N18a）：`*out = (PvP_Room*)GetPVPRoom()` 的
//                      LHS-first 不可造；void** 视图形引 push %ebx（+3 条）实测
//                      净负回退。
//   OnMatchRoom        DIFF 187（N78/N82/N93）：ORIG nameBuf 落非 4 对齐槽
//                      -0x193 + 4.1 memset 对齐剥离慢路（N78）；matched/常量槽
//                      分区与帧 0x1ac vs 0x1bc（N82）；语义闭合（p167 50=50）。
// ============================================================================
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
bool CFairMatch::_CheckSameTeam(PvP_Room* /*room*/, CUser* /*user*/)
{
    return 1;
}

// CheckUnusableEquipmentInUser ORIG T 0x08562cfe 报告同名 md —— 恒返 1（bool 返回：
// CheckJoinable +0x12/OnCreateRoom +0x11 均以 xor $1 取反，P22 实证）
bool CFairMatch::CheckUnusableEquipmentInUser(CUser* /*user*/)  // static（声明侧 CFairMatch.h）
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
    return m_fieldC0;
}
void CFairMatch::SetNextMatch(IMatch* next)
{
    // [B24D判词 N] 语义正确：m_fieldC0=(int)next；diff=mov 0xc(%ebp),%edx 与
    // mov 0x8(%ebp),%eax 装载序互换（GCC4.1 vs 4.4），7/7 纯 lowering，语义等价。
    m_fieldC0 = next;
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
bool CFairMatch::_CheckObserver(PvP_Room* room, CUser* /*user*/)
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
// b28d-solo-CFairMatchRooms 判词：N196——两处 `pc>=X`/`total>X` 被 4.4 折成反向
// 比较（cmp %ebx,%eax; setle/setl vs ORIG cmp %eax,%ebx; setge/setg，同语义不同
// compare 方向）；两向源码写法（`pc>=X` / `X<=pc`）实测同形 ⇒ 源形不可择。
// 余 1 条跳表重定位伪差。call 差集 8=8 CLEAR。fresh ndiff=4（85=85）。
int CFairMatch::CheckJoinable(PvP_Room* room, CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 1;
    // ORIG +0x2e：pc 直落 callee-saved %ebx 跨 GetPlayCountPerOneTeam 调用。
    // [b28d-PoolMisc2 P231 反例复验] 删 register ⇒ 值落栈槽 -0x10/-0xc、帧
    // 0x14→0x24、ndiff 4→9（更差）⇒ 本件 register 是**正例**，维持 register。
    register const int pc = room->get_player_count();
    // b28d-196/211：ORIG `cmp %eax,%ebx; setge %al; test %al,%al; je`（ebx=pc
    // 为左操作数）⇒ 试 P225 X3 形（while+单 return 复现 4.1 的 setcc 四段物化）。
    while (pc >= static_cast<int>(
               static_cast<unsigned char>(GetPlayCountPerOneTeam())) * 2) {
        return 4;
    }
    register const int total = m_teams[0].size() + m_teams[1].size();
    while (total > static_cast<int>(
               static_cast<unsigned char>(GetPlayCountPerOneTeam())) * 2) {
        return 4;
    }
    // ORIG +0x8d..0x99 跳表（cmp $5/ja/jmp *table）⇒ switch（P17）；
    // 叶块排布序 = case 书写序（P46）：0 → 3 → 1/4/5 与 default。
    switch (m_field04) {
    case 0:
        if (!_CheckSameTeam(room, user))
            return 4;
        // b28d-168a①：ORIG +0xd6 段 `call _CheckObserver; xor $0x1,%eax; test %al,%al;
        // je <mov $0>` ⇒ 源码为否定早返 `if (!f()) return 4; return 0;`（正极性写法
        // 少 xor 物化）。
        if (!_CheckObserver(room, user))
            return 4;
        return 0;
    case 3:
        return 0;
    case 1:
    case 4:
    case 5:
        return 4;
    case 2:
    default:
        // ORIG 跳表 case2/越界共用 +0xfe 块（P46 叶块排布）。
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
    // ORIG +0x99..0xb4：差值留 %ebx 跨 call（P52），(bool)(a > b) 复现 setg 物化
    // （t23/f3 实测；!=0 特例会被 4.4 折叠，> 不会）。
    if ((bool)(static_cast<int>(m_teams[0].size()) - observerCnt >
               static_cast<int>(static_cast<unsigned char>(
                   const_cast<CFairMatch*>(this)->GetPlayCountPerOneTeam()))))
    {
        return 0x16;
    }
    return 0;
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
    if (counts[1] != static_cast<int>(static_cast<unsigned char>(
            const_cast<CFairMatch*>(this)->GetPlayCountPerOneTeam())))
        return false;
    if (counts[2] != static_cast<int>(static_cast<unsigned char>(
            const_cast<CFairMatch*>(this)->GetPlayCountPerOneTeam())))
        return false;
    return true;
}

// CheckMatchable ORIG T 0x08565c54 报告 CheckMatchable.md
// b28d-solo-CFairMatchRooms 判词：AE——myUser/otherUser 提函数头 + 循环变量 i 提头
// 共用同一槽（P212），逐条全等（.o 仅重定位标签差）。call 差集 0=0。
bool CFairMatch::CheckMatchable(IMatch* other)
{
    // b28d-solo-CFairMatchRooms：ORIG 槽序 myUser@-0x14 / otherUser@-0x10 /
    // i@-0xc（4.1 先声明更深）⇒ 两个 CUser* 均声明在函数头，myUser 在前。
    CUser* myUser = 0;
    CUser* otherUser = 0;
    // b28d-solo-CFairMatchRooms：ORIG 两个循环共用同一 i 槽（-0xc）⇒ i 提到
    // 函数头声明一次（4.4 对两个独立 for-init i 各占一槽，整体槽深 +4）。
    int i;
    for (i = 0; i < 4; ++i) {
        otherUser = other->GetUser(static_cast<unsigned>(i));
        if (otherUser != 0) break;
    }
    if (otherUser == 0) return false;

    for (i = 0; i < 4; ++i) {
        myUser = GetUser(static_cast<unsigned>(i));
        if (myUser != 0) break;
    }
    // b28d-solo-CFairMatchRooms：ORIG 尾 `cmpl $0,-0x10(%ebp); jne <mov$1>;
    // mov $0,%eax; jmp; <mov$1>` ⇒ P56/P47 分支形（`return myUser != 0;`
    // 生 setne 物化，与 ORIG 的 mov$0/mov$1 双岛不符）。
    if (myUser == 0) return false;
    return true;
}

// GetGap ORIG T 0x856491c 报告 GetGap.md —— 双 call 内联差：4.4 实测
// `__builtin_fabsf(f() - g())` 直出 `call; fstps -0xc; call; fsubrs -0xc; fabs`。
float CFairMatch::GetGap(IMatch* other)
{
    return __builtin_fabsf(
        _GetRating(static_cast<ENUM_SCHOOL_MATCH_TEAM>(0)) -
        static_cast<CFairMatch*>(other)->_GetRating(
            static_cast<ENUM_SCHOOL_MATCH_TEAM>(0)));
}

// idx∈[0,4)：ORIG 反汇编 0x856515c/0x856517d 实证槽基址 = this + 0x24 + idx*0x10
// （movzbl 0x18+(idx+1)*16 读 m_flag、+(idx+1)*16+0x14 读 m_user），即
// m_teams[0].m_players[idx]（m_teams[0] @ +0x1c，m_players @ +0x08）。
CUser* CFairMatch::GetUser(unsigned int idx)
{
    if (idx > 3u) return 0;
    if (!m_teams[0].m_players[idx].m_flag) return 0;
    return m_teams[0].m_players[idx].m_user;
}

// GetPlayerCount ORIG T 0x8566d3e 报告 GetPlayerCount.md
int CFairMatch::GetPlayerCount()
{
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        if (m_teams[0].m_players[i].m_flag != 0) ++count;
        if (m_teams[1].m_players[i].m_flag != 0) ++count;
    }
    return count;
}

// _FindTeam ORIG T 0x856663a8 报告 _FindTeam.md
int CFairMatch::_FindTeam(CUser* user)
{
    for (int t = 0; t < 2; ++t) {
        for (int i = 0; i < 4; ++i) {
            if (m_teams[t].m_players[i].m_flag != 0 &&
                m_teams[t].m_players[i].m_user != 0 &&
                m_teams[t].m_players[i].m_user == user)
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
    m_fieldB0 = 0;
    m_fieldB4 = 0;
    m_leaderSeat[0] = 0;
    m_leaderSeat[1] = 0;
    m_fieldC0 = 0;
    return 0;
}

// OnDestroyRoom ORIG T 0x8562dc6 报告 OnDestroyRoom.md
// [b28 BM2 判词·N7] AE-ndiff=6（fresh .o 归一）：pWaitingRoomList_->DelRoom 虚槽
// 调用 ORIG 4.1 以 %ecx 承载 fnptr、%edx/%eax 装载实参，4.4 恒 %eax 承载 call 目标
// （N7「间接 call 目标恒 %eax」，b26 NL28 族）。助记符/条数全等，纯寄存器文本差；
// 禁绑定形（P0/N96）。
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
        for (int i = 0; i <= 3; ++i) {
            // ORIG +0x68..0x8b：先取 m_user 落槽（无论 flag），后独立重算地址读
            // m_flag（P48 双独立访问），再判 u!=0。
            CUser* u = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && u != 0)
                u->Send(pkt);
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
// [b28 BM2 判词·N7] 同 OnDestroyRoom：AddRoom 虚槽 %ecx vs %eax 承载（N7 族），
// 其余逐条同。
void CFairMatch::_RegisterWaitingRoom(PvP_Room* /*room*/)
{
    // ORIG 0x8563a38 实证无 null 检查直接解引用（pWaitingRoomList_ 恒非 0）
    pWaitingRoomList_->AddRoom(this);
}

// _ChangeTeamLeader ORIG T 0x8563272 报告 _ChangeTeamLeader.md
// 离队者恰为队长座位 → 该队首名在座玩家接任并 0x13d 包通知全队
// b28d-solo-CFairMatchRooms 判词：AE——m_leaderSeat[2] 数组化（CFairMatch.h，偏移
// 0xb8/0xbc 与 sizeof 不变）+ 去 PlayerInfoThird 引用缓存，逐条全等。
void CFairMatch::_ChangeTeamLeader(PvP_Room* room,
                                   ENUM_SCHOOL_MATCH_TEAM teamIdx,
                                   int oldLeaderSeat)
{
    // b28d-solo-CFairMatchRooms：Ghidra C 实证 ORIG 每次访问均重算
    // `*(int*)(this + (teamIdx+0x2c)*4 + 8)`（数组索引形，无引用局部槽；
    // 三目引用形会生 cmpl/jne/add 分支 + -0x14 引用槽）。循环内亦直接
    // 逐字段重算地址（P157/P183 去引用缓存）。
    CUser* newLeader = 0;
    if (m_leaderSeat[teamIdx] == oldLeaderSeat) {
        for (int i = 0; i < 4; ++i) {
            if (m_teams[teamIdx].m_players[i].m_flag != 0) {
                newLeader = m_teams[teamIdx].m_players[i].m_user;
                break;
            }
        }
    }
    if (newLeader != 0) {
        m_leaderSeat[teamIdx] = room->get_user_seat(newLeader);
        PacketGuard pkt;
        pkt.put_header(0, 0x13d);
        pkt.put_short(m_leaderSeat[teamIdx]);
        pkt.finalize(true);
        m_teams[teamIdx].SendToTeam(pkt);
    }
}

// OnLeaveRoom ORIG T 0x8562fbc 报告 OnLeaveRoom.md
// [b28 BM2 判词·N7] AE-ndiff=6（fresh .o 归一，全二进制口径 16 含标签伪差）：
// 唯一差 = _ReturnTeam 类虚槽调用 call *%ecx(ORIG) vs call *%eax(4.4) 及其
// 实参装载寄存器对调（N7 族，b26 NL28）。其余 181  insn 逐条同。
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
        // ORIG +0x44..：(this+other*0x48)+0x10/disp0xc = m_field00（P1 组件视图等价具名字段）
        if (m_teams[other].m_field00) {
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
    // ORIG 无判空直调（0x08563cc6 同族形态，P148 忠实模拟）。
    pWaitingRoomList_->DelRoom(this);
    _SendMatchStatePacket(5, 1);
}

// OnUnregisterMatch ORIG T 0x85663cc6（README 行：0x08563cc6）报告同名 md
// [b28 P222-y10 DIFF→NEAR] 否定合并前置形已翻正块序（ndiff 20→6）；残余 6 行 =
// DelRoom 虚槽 call *%ecx vs *%eax = N7 封顶。
// ORIG 返回 int：条件不满足 → 0；执行 → 1。
// [b28 BM2 判词·N7] AE-ndiff=8（fresh .o 归一）：DelRoom/Remove 虚槽 call *%ecx
// vs call *%eax + 实参 %edx/%eax→%ecx/%edx 对调（N7 族）。其余逐条同。
int CFairMatch::OnUnregisterMatch(CUser* /*user*/, PvP_Room* /*room*/)
{
    // [b28 P59 变体·y10 探针 /tmp/l2_p5.cpp] 否定合并 `!=1 && !=2` return 0 前置形
    // 令 4.4 产 ORIG 0x08563cc6 布局：cmp$1;je body / cmp$2;je body（双 je 同目标）
    // / fall-through `mov $0;jmp` false 岛 / body 尾 `mov $1`。旧正极性共享体形产
    // `jne end` 反向+岛换位（DIFF 20 中真差 4 行）。goto 形带 nop 岛（y3）不用。
    // 语义不变：仅 f04∈{1,2} 时注销并返 1，否则 0。
    if (m_field04 != 1 && m_field04 != 2)
        return 0;
    m_field04 = 0;
    // ORIG +0x2d..0x48 无判空直调 vtable[+0x04]（忠实模拟 ORIG 未定义行为，P148）。
    // 残余 call *%ecx vs call *%eax = N7（4.4 间接 call 目标恒 %eax）。
    pWaitingRoomList_->DelRoom(this);
    _SendMatchStatePacket(5, 1);
    return 1;
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
    int err = 0;
    if ((err = _CheckMatchable(room)) != 0) {
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
// [b28d-PoolSmall·检查点22 收尾] 唯一残差 `*out = GetPVPRoom()` 的 store 装载序
// （ORIG LHS-first `mov 0x14(%ebp),%edx; mov %eax,(%edx)` vs 4.4 RHS-first+edx
// 中转）由 **P211 档① callee 侧同型旋钮**解决：CUser.h `GetPVPRoom()` 返回
// void*→PvP_Room*（Main 检查点22 落地，mangled 不含返回类型、CUser.cpp:1128
// 本体同步）⇒ 目标与 RHS 同型，4.4 直出 LHS-first 直存。
// 【负例记录】绕冻结头曾用两种临时形，现均**不必要且更差**，勿再引入：
//  ①`*(void**)out = member->GetPVPRoom()` 同型视图——可翻 LHS-first 但引
//    `push %ebx` 缓存 out（+5 行，净负）；
//  ②`*out = static_cast<PvP_Room*>(...)` 冗余 cast——callee 同型后该 cast 不再
//    产生异型中转，实测与本形 TU 三数完全相同（36/88/3/7），即零收益且与 ORIG
//    源码形不符 ⇒ 已删（§4 真形优先）。
// call 差集 5=5 CLEAR。
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
        if ((member = other->GetUser(static_cast<unsigned>(i))) != 0) break;
    }
    // b28d-solo-CFairMatchRooms：P222 正向早返 —— ORIG `cmpl $0,-0x10(%ebp);
    // jne <then>`（member==0 的 trace 0xc5d 块直落、成功路径在尾部）。
    if (member == 0) {
        // P221 全表达式临时链（翻互斥块 trace 槽方向，ORIG c5d@-0x30）。
        cMyTrace(__PRETTY_FUNCTION__, 0xc5d, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0xc5d);
        return 0;
    }
    // ORIG `mov 0x14(%ebp),%edx; mov %eax,(%edx); mov 0x14(%ebp),%eax;
    // mov (%eax),%eax; jne` ⇒ 两句式（store 后重读）；目标与 RHS 同型 ⇒
    // P211 档① LHS-first 直存。
    *out = member->GetPVPRoom();
    // ORIG `test %eax,%eax; jne <mov $1 尾块>` ⇒ 失败块直落、return 1 在尾
    // （P191/P222 正极性尾块）。
    if (*out == 0) {
        // P221 全表达式临时链（ORIG c64@-0x20）。
        cMyTrace(__PRETTY_FUNCTION__, 0xc64, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0xc64);
        return 0;
    }
    return 1;
}

// _PullOutRoom ORIG T 0x856633ce?（README 0x085633ce）报告同名 md
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 全表达式临时链
// （`CSwitchLog(..)(...)`）翻正变参求值序（ORIG 在 ctor 前求 varargs 并跨 ctor
// 保活 edi/esi/ebx）；call 差集 10=10。fresh ndiff=0（66=66）。
void CFairMatch::_PullOutRoom(CUser* user, PvP_Room* room, int seat)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链 —— ORIG 在 ctor 前右起求
    // varargs 并跨 ctor 保活（edi/esi/ebx + -0x2c 槽）；具名 log 两语句形令 4.4
    // 先 ctor 后求值（帧 0x3c vs 0x4c）。
    CSwitchLog(
        "void fair_pvp::CFairMatch::_PullOutRoom(CUser*, PvP_Room*, int)",
        0xb5a, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                     user->get_acc_name(), user->getCurCharacName(),
                     room->get_index(), room->get_pvp_battle_mode(), seat);
    // b28d-solo-CFairMatchRooms：P292 档①——ORIG 尾部 `xor $1,%eax; test %al,%al;
    // je`（bool callee 裸否定形，无 setne/槽物化）⇒ 源码直写 if(!call())，
    // 删具名 int ok 局部（该局部在 ORIG 无槽往返）。
    if (!room->OnSetPVPSeatState(
            user, seat,
            static_cast<ENUM_SEAT_STATE>(0xfe),
            static_cast<ENUM_WALKOUT_TYPE>(2))) {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
}

// _PutInRoom ORIG T 0x856334bc?（README 0x085634bc）报告同名 md
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 链式 + P162（put_byte(result) 不
// 收窄，消 movzbl）+ P89（去 guardAsBuf 局部，内联 reinterpret_cast）+ 成功分支内
// 独立 return seat（复现 ORIG 两处 ebx 物化）；call 差集 21=21。
// fresh ndiff=0（121=121）。
int CFairMatch::_PutInRoom(CUser* user, PvP_Room* room, bool observer)
{
    PacketGuard pkt;
    int seat = 0;
    const int result = room->join_room(user, seat, observer);
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链（_PullOutRoom 实证同族）。
    CSwitchLog(
        "int fair_pvp::CFairMatch::_PutInRoom(CUser*, PvP_Room*, bool)",
        0xb6a, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                     user->get_acc_name(), user->getCurCharacName(),
                     room->get_index(), room->get_pvp_battle_mode(), result);
    if (result >= 1) {
        pkt.put_header(1, 0x36);
        pkt.put_byte(0);
        // b28d-solo-CFairMatchRooms：P162 —— ORIG 直接传 int（无 movzbl），
        // 源码不做 (unsigned char) 收窄。
        pkt.put_byte(result);
        pkt.finalize(true);
        user->Send(pkt);
        // ORIG @+0x81 `mov -0x3c(%ebp),%ebx; jmp` ⇒ 成功分支内独立 return
        // （两处 return seat 各自物化 ebx）。
        return seat;
    } else {
        // ORIG: make_seat_info(room,(char*)&pkt,seat) —— PacketGuard 对象
        // 地址被传入（无具名指针局部，P0/P89），函数内部经其
        // InterfacePacketBuf 基类写包。
        room->make_seat_info(reinterpret_cast<char*>(&pkt), seat);
        G_GameWorld()->send_all(pkt);
        G_GameWorld()->goto_pvp(user);
    }
    // b28d-167：p167 差集 EXTRA InterfacePacketBuf::finalize×1 ⇒ ORIG 无尾部
    // finalize（两条路径各自 finalize 后返回），删除。
    return seat;
}

// _MakeRoom ORIG T 0x8563692 报告 _MakeRoom.md
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 链式 + P222 反向布局（ORIG
// `jle <失败尾块>` + result>=1 成功路径直落；改写 if(result>=1){成功}else{失败}）
// + P162；call 差集 45=45。fresh ndiff=0（230=230）。
PvP_Room* CFairMatch::_MakeRoom(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(1, 0x35);

    PvP_Room* room = G_CGameManager()->GetPvp();
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
        m_roomInfo.byte8,
        m_roomInfo.passwd.c_str(),
        static_cast<int>(m_roomInfo.passwd.length()),
        static_cast<PVP_BATTLE_MODE>(2));
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
    CSwitchLog("PvP_Room* fair_pvp::CFairMatch::_MakeRoom(CUser*)",
               0xba0, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                            user->get_acc_name(), user->getCurCharacName(),
                            room->get_index(), room->get_pvp_battle_mode(), result);

    // b28d-solo-CFairMatchRooms：P222 反向布局 —— ORIG `jle <失败尾块>` +
    // result>=1 成功路径直落（logFormat 0xba9 + PutPvp + return 0），失败块在尾。
    if (result >= 1) {
        pkt.put_byte(0);
        // P162：ORIG 直接传 int（无 movzbl）。
        pkt.put_byte(result);
        pkt.finalize(true);
        user->Send(pkt);
        LogManager::logFormat(
            1, "MatchingSystem.cpp",
            __PRETTY_FUNCTION__, 0xba9,
            "create_room fail: PVP슬롯반환 ID[%d]\n", room->get_index());
        G_CGameManager()->PutPvp(room);
        return 0;
    } else {
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
}

// _MoveRoom ORIG T 0x8563d32 报告 _MoveRoom.md
// 遍历 from 房 8 座逐个换到 to；观战队（team==3）换入后重设观战态
void CFairMatch::_MoveRoom(PvP_Room* to, PvP_Room* from, bool observerFlag)
{
    // ORIG 0x08563d32 +0x0e..+0x23 初始化序 = 声明序槽：
    // team→-0x18、mover→-0x14、newSeat→-0x10、seat(unsigned)→-0xc（setbe 实证）。
    int teamOf = 0;
    CUser* mover = 0;
    int newSeat = 0;
    for (unsigned seat = 0; seat < 8; ++seat) {
        mover = reinterpret_cast<CUser*>(from->get_user_seat(seat));
        if (mover == 0) continue;
        teamOf = from->get_team(seat);

        PacketGuard pkt;
        pkt.put_header(0, 0x29);
        pkt.put_short(1);
        to->make_room_info(reinterpret_cast<char*>(&pkt));  // ORIG +0xa3 0xc(%ebp)=to
        pkt.finalize(true);
        from->send_to_pvp(pkt);

        _PullOutRoom(mover, from, seat);
        _SetEnterCharacNo(mover->getCurCharacNo());
        newSeat = _PutInRoom(mover, to, observerFlag);
        if (teamOf == 3) {
            to->OnSetPVPSeatState(mover, newSeat,
                                  static_cast<ENUM_SEAT_STATE>(3),
                                  static_cast<ENUM_WALKOUT_TYPE>(2));
        }
    }
}

// OnMatchRoom ORIG T 0x8564150 报告 OnMatchRoom.md。ORIG 失败路径 → 0；成功 → 1。
// b28d-solo-CFairMatchRooms 判词：N82——帧 0x1ac vs 0x1bc + cMyTrace/CSwitchLog 槽位
// 平移；已修语义：删 !matched 分支多余 trace 块（p167 50=50 CLEAR）、nameBuf[256]={0}
// 单缓冲复用两次、去 pWaitingRoomList_ null 守卫（Ghidra C 无守卫）。
// fresh ndiff=181（500/446，超大函数按 §6.2 三件套：call 差集 50=50 CLEAR、
// 语义闭合）。
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
    m_leaderSeat[0] = room->GetManagerSeat();

    IMatch* other = 0;
    // b28d-solo-CFairMatchRooms：Ghidra C 实证 ORIG 直接解引用
    // pWaitingRoomList_（无 null 守卫）——`cVar2 = (**(*pWaitingRoomList_+8))(...)`
    // 无 test/je 前置。照抄原码行为（P171）。
    bool matched = pWaitingRoomList_->Match(this, &other);
    if (!matched) {   // 等待表 vtable[+0x08]=Match
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        // b28d-solo-CFairMatchRooms：ORIG 只有两处 trace 块（0xcb0/0xcb1 与
        // 0xcbb/0xcbc）；Ghidra C 实证 !matched 分支直接 return 0（无
        // CSwitchLog/GetUserName）⇒ 删除本分支误加的 trace（p167 EXTRA
        // 2×CSwitchLogC1/op()/GetUserName）。
        return 0;
    }

    PvP_Room* movable = 0;
    if (_CheckMovableRoom(other, room, &movable) == 0) {
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        {
            // b28d-solo-CFairMatchRooms：Ghidra C 实证——单一 nameBuf[256]={0}
            // （4.1 全量清零展开 rep stos，~15 条）+ 复用同一缓冲两次 GetUserName，
            // CSwitchLog 对象紧跟各自 GetUserName 之后构造。
            char nameBuf[256] = {0};
            GetUserName(this, nameBuf);
            CSwitchLog l0("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcb0, 0, 0);
            l0("pvp@log 1 call match(%x), %s ",
               reinterpret_cast<void*>(this), nameBuf);
            GetUserName(other, nameBuf);
            CSwitchLog l1("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcb1, 0, 0);
            l1("pvp@log 1 opposite match(%x), %s ", other, nameBuf);
        }
        return 0;
    }

    if (room->get_index() == movable->get_index()) {
        // 撞进同一间：只作战斗开始兜底
        m_field04 = 2;
        _SendMatchStatePacket(1, 1);
        {
            // b28d-solo-CFairMatchRooms：同 0xcb0 块（Ghidra C 实证同一 nameBuf
            // 清零一次并复用两次）。
            char nameBuf[256] = {0};
            GetUserName(this, nameBuf);
            CSwitchLog l0("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcbb, 0, 0);
            l0("pvp@log 2 call match(%x), %s ",
               reinterpret_cast<void*>(this), nameBuf);
            GetUserName(other, nameBuf);
            CSwitchLog l1("virtual bool fair_pvp::CFairMatch::OnMatchRoom(PvP_Room*, int)",
                          0xcbc, 0, 0);
            l1("pvp@log 2 opposite match(%x), %s ", other, nameBuf);
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
        // b28d-solo-CFairMatchRooms：ORIG `uVar10 == (uVar7 & 0xff)`（Ghidra C
        // 实证）⇒ GetPlayCountPerOneTeam() 结果按 uchar 截断后再比。
        if (static_cast<CFairMatch*>(it)->GetPlayerCount()
            == static_cast<int>(static_cast<unsigned char>(
                   GetPlayCountPerOneTeam()))) {
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
    m_leaderSeat[1] = room->get_user_seat(managerUser);
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
        pkt.put_short(m_leaderSeat[t]);
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
// b28d-solo-CFairMatchRooms 判词：N7——P227 链式 + P196（seat unsigned 复现
// setbe）后仅剩 CSwitchLog varargs 的 esi↔edi 分配互换（ORIG 第1值→esi/第2→edi，
// 4.4 反之）+ 对应压栈序（4 行 NEAR，113=113）。跨版本 callee-saved 分配不可择
// （P75/N82附）⇒ 封顶。call 差集 11=11 CLEAR。
PvP_Room* CFairMatch::_ReturnTeam(PvP_Room* room, int teamNo, CUser* excluded)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链（_PullOutRoom 实证同族）。
    CSwitchLog(
        "PvP_Room* fair_pvp::CFairMatch::_ReturnTeam(PvP_Room*, int, CUser*)",
        0xd95, 0, 0)("pvp@log room(%d), %d, %d",
                     room->get_index(), room->get_pvp_battle_mode(),
                     room->get_waiter_count());
    CUser* member = 0;
    PvP_Room* newRoom = 0;
    // b28d-solo-CFairMatchRooms：P196 —— ORIG 循环尾测为 setbe（无符号），
    // 源码 seat 须为 unsigned（int 形 4.4 恒 setle）。
    for (unsigned int seat = 0; seat <= 7; ++seat) {
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
// b28d-solo-CFairMatchRooms 判词：N121——P227 链式 + P225/P295（while+break 复现
// `cmp $2; sete; test; je`）后仅缺 ORIG body 尾 `jmp <紧邻下一指令>`（1 行）：
// 4.4 恒 jump-thread 掉该空岛（N121 epilogue-threading 尾岛不可消）⇒ 封顶。
// fresh ndiff=1（157/156）。call 差集 10=10 CLEAR。
// [b28d-PoolSmall 复验] 再补两形实测均不闭合：①`break`→`goto <体尾空标签>`
// （标签置于 while 之后）⇒ 4.4 仍把 goto 融为 fall-through，`jmp` 不发射
// （l8_cmp 严格口径 ndiff 2，其中 1 行为 rodata 标签伪差）；②UpdateScore 侧
// P212「全头部声明 + 块内仅赋值」把 killCnt 提出块外 ⇒ 深槽号零改善、
// 本函数 ndiff 6→8 反劣（已回滚）。⇒ P223 定案③「4.4 岛数 = 条件数或 0」
// 成立，N121 封顶维持。
int CFairMatch::_PullOutRoomTeam(PvP_Room* room, ENUM_SCHOOL_MATCH_TEAM teamIdx)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链（_PullOutRoom 实证同族）。
    CSwitchLog(
        "bool fair_pvp::CFairMatch::_PullOutRoomTeam(PvP_Room*, ENUM_SCHOOL_MATCH_TEAM)",
        0xfa7, 0, 0)("pvp@log room(%d),%d,%d",
                     room->get_index(), room->get_pvp_battle_mode(),
                     static_cast<int>(teamIdx));
    int lastTeam = 0;
    for (int i = 0; i < 4; ++i) {
        // b28d-solo-CFairMatchRooms：P157/P183 去引用缓存（ORIG 每处字段访问
        // 重算完整地址，无引用槽）。
        if (m_teams[teamIdx].m_players[i].m_flag != 0 &&
            m_teams[teamIdx].m_players[i].m_user != 0) {
            const int seat =
                room->get_user_seat(m_teams[teamIdx].m_players[i].m_user);
            if (seat != -1) {
                lastTeam = room->get_team(seat);
                _PullOutRoom(m_teams[teamIdx].m_players[i].m_user, room, seat);
            }
        }
    }
    if (lastTeam == 1) {
        for (int seat = 0; seat <= 7; ++seat) {
            CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(seat));
            // b28d-solo-CFairMatchRooms：P225/P295 —— ORIG `cmp $2; sete %al;
            // test %al,%al; je`（bool 物化）；`&&` 短路形被 4.4 折成 jne 直跳，
            // 改 while+break 复现物化。
            if (u != 0) {
                while (room->get_team(seat) == 2) {
                    room->OnSetPVPSeatState(u, seat,
                                            static_cast<ENUM_SEAT_STATE>(1),
                                            static_cast<ENUM_WALKOUT_TYPE>(2));
                    break;
                }
            }
        }
    }
    return 1;
}

// OnWalkOut ORIG T 0x8564fb2 报告 OnWalkOut.md。ORIG 返回 1。
// 转发到本类虚槽 +0x2c（OnLeaveRoom(user,room,team,seat)）
// b28d-75（N82附/P75 封顶，语义等价）：vtable[+0x2c] 虚调用目标寄存器两侧同为
// %ecx（b28d 复核，旧 N7 判词已不适用）；唯一真差 = 五实参装载时刻：
// ORIG 4.1 先载第4参 team(-0x10)→%eax，再 seat→0x10；4.4 先载第2参
// userSeat(-0xc)→%eax。b28d 复测 (int)team 冗余 cast 形无变化（P41 在此不触发）
// ⇒ 跨版本实参装载调度，禁绑定形（P0）⇒ N82附 封顶。
// b28d-solo-CFairMatchRooms 判词：N82附——五实参物化序差：ORIG 先物化 team
// (-0x10)→%eax 再压 seat/team/room/userSeat；4.4 先物化 userSeat(-0xc)→%eax、
// team/room 走 %edx。unsigned team / `*&team` 两形实测均引 push %ebx（fnptr 落
// %ebx 而非 ORIG %ecx）⇒ 净负回退。槽位集全等、call 差集 2=2 CLEAR。封顶。
// fresh ndiff=5（33=33 NEAR）。
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
    if (m_fieldB0 >= 3) return 0;  // ORIG +0x0f cmp $0x2 立即数实证（P97）
    if (!_CheckRematchable(room)) return 0;
    // ORIG +0x44..0x63：(this+t*0x48)+0x10 / disp 0xc = 全局 0x1c+0x48t
    // ⇒ m_teams[t].m_field00（“可重赛”标记；bool 取反 xor $1 实证，P22）。
    for (int t = 0; t < 2; ++t) {
        if (!m_teams[t].m_field00) return 0;
    }
    return 1;
}

// OnEndFight ORIG T 0x8565048 报告 OnEndFight.md
// b28d-solo-CFairMatchRooms 判词：IDENT——P211/P274（mgrTeam 局部 unsigned 与
// _ReturnTeam int 形参异型，翻实参装载序）+「声明即初始化」形（`PvP_Room*
// releasedRoom = _ReturnTeam(...)` 令 mgrTeam 深槽 -0x20/返回值浅槽 -0x1c，
// 探针 /tmp/fm_p4.cpp c/d）+ P83 结构赋值；call 差集 9=9。fresh ndiff=0（71=71）。
void CFairMatch::OnEndFight(PvP_Room* room)
{
    // b28d-solo-CFairMatchRooms：P211/P274 —— 局部 unsigned 与 _ReturnTeam 的
    // int 形参异型，令 4.4 先求值第 3 参再写第 4 参常量（ORIG 4.1 装载序）。
    const unsigned int mgrTeam = room->get_manager_team();
    m_field04 = 0;
    // b28d-solo-CFairMatchRooms：ORIG（Ghidra C）为 if/else 双分支布局（timer
    // 在 then、_ReturnTeam 在 else），且 gen_timer_key 结果直接进 %esi 跨
    // get_index/G_TimerQueue 调用保活（无 key 具名槽）。
    // ORIG `cmp $0x1,%al; setne %al; test; je <timer 块>` ⇒ 否定比较 +
    // _ReturnTeam 为 then 直落、timer 为 else 尾块（P222 反向布局）。
    if (static_cast<unsigned char>(GetPlayCountPerOneTeam()) != 1) {
        // b28d-solo-CFairMatchRooms：ORIG 在 -0x1c 保存 _ReturnTeam 返回值（后续
        // 未读的死写入，P171 照抄）；「声明即初始化」形令 4.4 给 mgrTeam 深槽
        // -0x20、返回值浅槽 -0x1c（探针 /tmp/fm_p4.cpp c/d 形）。
        PvP_Room* releasedRoom = _ReturnTeam(room, mgrTeam, 0);
        (void)releasedRoom;
        if (mgrTeam != 1) {
            // b28d-solo-CFairMatchRooms：P83/N84——ORIG 是结构整体赋值
            // `rep movsl` 0x12 dwords（TeamInfoThird 0x48 字节）。
            m_teams[0] = m_teams[1];
        }
        m_teams[0].refresh();
        m_teams[1].reset();
    } else {
        // 单人模式：0x3e 定时器 0ms 兜底唤醒战房清算链
        G_TimerQueue()->InsertTimerInMilisecond(
            static_cast<TimerEntry::OBJ_TYPE>(1), room->get_index(),
            static_cast<TIMER_MESSAGE>(0x3e),
            static_cast<long long>(0),
            static_cast<int>(
                room->gen_timer_key(static_cast<TIMER_MESSAGE>(0x3e))),
            0);
    }
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
// b28d-solo-CFairMatchRooms 判词：DIFF→NEAR（ndiff 133→32，172→174 指令）。
// 已收：P296 嵌套正向 if（`if (m_flag) { if (user) { body } }`）复现 ORIG
// `movzbl; test %al,%al; je` 直测（取反 continue 形 4.4 恒 xor $1 物化）；
// P171 补 killCnt 死写入数组（ORIG 0x8565904 `mov %eax,-0x44`，P232 未用数组
// 仍占槽）；P0 补 missionList 具名局部（ORIG 0x85658ca `mov %eax,-0x1c`，内联形
// 把 GetCharacExpandData(8) 排到 killCnt 之后）；P3 cond 实参
// `(unsigned char)` 收窄复现 `cmp $0x1,%al`；P227 链式、P191 双初始化、P133 两
// 独立守卫、P157/P183 去引用缓存、score/missions 内联。真语义已修（job/grow 取
// 第 2 参 info）。残差 = 深区临时槽（score7/grow）深 0x10 + 帧 0x7c vs 0x6c
// （N115 4.4 outgoing 预留 + N82 槽分区）；call 差集 17=17 CLEAR。
void CFairMatch::UpdateScore(PvP_Room* room, PlayerInfoThird& info,
                             ENUM_SCHOOL_MATCH_TEAM teamIdx,
                             int win, int lose, int result)
{
    (void)teamIdx;
    // P191 配套：ORIG 双初始化形 `movl $0,-0x28` ×2（声明 + for-init 各一条）。
    int i = 0;
    for (i = 0; i < 4; ++i) {
        // b28d-solo-CFairMatchRooms：ORIG 先读 m_user 存槽（@+0x22），再两条独立
        // 守卫（m_flag 直读字段 / user 读槽；P133/P157 去引用缓存）；P296 嵌套
        // 正向 if 复现 ORIG `movzbl; test %al,%al; je` 直测（取反 continue 形
        // 4.4 恒 xor $1 物化，N122）。
        CUser* user = m_teams[teamIdx].m_players[i].m_user;
        if (m_teams[teamIdx].m_players[i].m_flag) {
            if (user != 0) {
                const int seat = room->get_user_seat(user);
                // b28d-solo-CFairMatchRooms：ORIG（Ghidra C）为两条独立早返
                // （`if (seat < 0) return; if (7 < seat) return;`），合并 || 会产
                // 不同控制流（P133）。
                if (seat < 0)
                    return;
                if (seat > 7)
                    return;
                // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
                CSwitchLog(
                    "void fair_pvp::CFairMatch::UpdateScore(PvP_Room*, fair_pvp::PlayerInfoThird&, ENUM_SCHOOL_MATCH_TEAM, int, int, int)",
                    0xee5, 0, 0)("pvp@log %s,%s,room(%d),%d,%d,%d",
                                 user->get_acc_name(), user->getCurCharacName(),
                                 room->get_index(), room->get_pvp_battle_mode(),
                                 win, lose);
                // b28d-solo-CFairMatchRooms：ORIG（Ghidra C）job=param_2+5、
                // grow=param_2+6 低 nibble —— param_2 即本函数第 2 参 info。
                // 原实现取 m_teams[teamIdx].m_players[0] 属真语义偏差（契约应为形参）。
                reinterpret_cast<CFairPvPScore*>(
                    user->GetCharacExpandData(
                        static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->UpdateScore(static_cast<int>(info.m_kind.m_job),
                                  info.m_kind.m_grow & 0xf,
                                  static_cast<int>(
                                      static_cast<unsigned char>(
                                          GetPlayCountPerOneTeam())),
                                  win, lose, result,
                                  room->m_relayMgr.GetKillCount(seat));

                // ORIG 0x085658ca `mov %eax,-0x1c(%ebp)`：GetCharacExpandData(8)
                // 结果先存具名局部（P0 槽往返 ⇒ 原码有具名局部），再在
                // Update_GameOver_event 处使用（内联形会把该 call 排到 killCnt 之后）。
                CMissionList_Charac* missionList =
                    reinterpret_cast<CMissionList_Charac*>(
                        user->GetCharacExpandData(
                            static_cast<ENUM_CHARAC_EXPAND_TYPE>(8)));
                // ORIG `cmp $0x1,%al`（单字节比较）⇒ 调用点收窄（P3）。
                {
                    MissionClearCondition_Parameter cond(
                        static_cast<PVP_BATTLE_MODE>(
                            static_cast<unsigned char>(GetPlayCountPerOneTeam())
                                == 1 ? 2 : 3));
                    // ORIG 0x8565904 `mov %eax,-0x44(%ebp)`：二次取 GetKillCount
                    // 结果写入死局部（P171 死写入照抄；P232 未使用数组仍占槽）。
                    int killCnt[1];
                    killCnt[0] = room->m_relayMgr.GetKillCount(seat);
                    (void)killCnt;
                    missionList->Update_GameOver_event(*user, cond);
                }
                GiveTrainingSeal(user, win != 0);
            }
        }
    }
}

// GiveTrainingSeal ORIG T 0x85652c8 报告 GiveTrainingSeal.md
// b28d-solo-CFairMatchRooms 判词：DIFF（ndiff 226→3，274→276 指令；全部槽位
// 与 ORIG 对齐）。已收：删具名 bool ok 改双短路（ORIG 0x0856564e..0x08565675
// `jle`/`jbe` + mov$1/mov$0 汇聚）；bonusRemain bool→char（S1 一次性旋钮，
// P321-T3 消费端 `cmpb $0x0,-0x11; je` 无 setcc ⇒ bool/char 直证不足，char 复现
// 4.1 内存直比；bool 恒 xor 物化 N122）；P222 正向包裹翻块序（ORIG bonusRemain==0
// 块在尾部、win 链 fall-through）；P0 内联 `getCurCharacInvenW()`（ORIG 无 inven
// 槽，结果直留 %eax 跨实参装载）；vector 声明序 updatedSlots 前置（ORIG 构造序
// -0x30→-0x3c、push_back this=-0x3c）；P183 去 sps/score 缓存、P3 `cmp $1,%al`、
// dailyLimit 表达式内联、P135 push_back(O&&)。残 3 = N121 epilogue 尾岛
// （我方多 `jmp+nop`）。p167 49=49 CLEAR。
void CFairMatch::GiveTrainingSeal(CUser* user, bool win)
{
    if (user->getCurCharacR() == 0)
        return;

    // S1 一次性旋钮（P321-T3 直证不足：ORIG 消费端 `cmpb $0x0,-0x11; je`
    // 无 setcc，bool/char 均可）——试 char 复现 4.1 内存直比（bool 恒 xor 物化）。
    char bonusRemain = 1;
    // b28d-solo-CFairMatchRooms：P183 去缓存铁律。ORIG（p167 实证）每处 SPS 成员
    // 访问都重发 G_CDataManager()（13 次）、每处 score 访问都重发
    // GetCharacExpandData(7)（10 次）；我方缓存 sps 引用/score 指针只发 1/1 次。
    // 且 win 三目必须展开为两条独立路径（ORIG 各路径各一条 call：
    // SetLastTrainingSealBonusCount ORIG 6 条 vs 我方 4 条）。
    // b28d-solo-CFairMatchRooms：ORIG `movzwl 0x49e0(%eax),%eax; movzwl %ax,%ebx`
    // 值留 callee-saved %ebx 跨 GetCharacExpandData/GetDailyPlayCount 保活、无栈槽
    // ⇒ dailyLimit 以表达式内联进 remain（P0：无槽往返即无具名局部）。
    const int remain = static_cast<int>(FM_SPS().m_schoolMatchDailyPlayLimit -
        reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->GetDailyPlayCount());
    if (remain <= 0) {
        reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->SetRemainBonusCount(0);
        bonusRemain = 0;
    } else {
        reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->SetRemainBonusCount(remain);
    }

    int sealTotal = 0;
    // b28d-solo-CFairMatchRooms：P3 —— ORIG `cmp $0x1,%al`（单字节比较）⇒
    // 调用点收窄。
    if (static_cast<unsigned char>(GetPlayCountPerOneTeam()) == 1) {
        if (win)
            sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[0]);
        else
            sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[1]);
        // ORIG 块序：`cmpb $0,-0x11; je <尾部>`（bonusRemain==0 块在尾）、win
        // 链 fall-through ⇒ P222 正向包裹。
        if (bonusRemain) {
            if (win) {
                sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[2]);
                reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->SetLastTrainingSealBonusCount(
                        static_cast<int>(FM_SPS().m_schoolMatchSeal[2]));
            } else {
                sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[3]);
                reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->SetLastTrainingSealBonusCount(
                        static_cast<int>(FM_SPS().m_schoolMatchSeal[3]));
            }
        } else {
            reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                ->SetLastTrainingSealBonusCount(0);
        }
    } else {
        if (win)
            sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[4]);
        else
            sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[5]);
        if (bonusRemain) {
            if (win) {
                sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[6]);
                reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->SetLastTrainingSealBonusCount(
                        static_cast<int>(FM_SPS().m_schoolMatchSeal[6]));
            } else {
                sealTotal += static_cast<int>(FM_SPS().m_schoolMatchSeal[7]);
                reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                    static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                    ->SetLastTrainingSealBonusCount(
                        static_cast<int>(FM_SPS().m_schoolMatchSeal[7]));
            }
        } else {
            reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
                ->SetLastTrainingSealBonusCount(0);
        }
    }
    reinterpret_cast<CFairPvPScore*>(user->GetCharacExpandData(
        static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))
        ->SetLastTrainingSealCount(sealTotal);
    if (sealTotal <= 0)
        return;

    // ORIG 声明序实证：updatedSlots 先构造（lea -0x30 @0x085655bd）、giveList
    // 后（lea -0x3c @0x085655c8），且 push_back this=-0x3c ⇒ giveList 后声明。
    std::vector<int> updatedSlots;
    std::vector<std::pair<int,int> > giveList;
    // b28d-solo-CFairMatchRooms：P135——ORIG 选 push_back(O&&) 右值重载，
    // 具名左值 entry 会选 push_back(RKS1_)（p167 EXTRA 实证）。
    giveList.push_back(std::pair<int,int>(static_cast<int>(0xcf0), sealTotal));
    // P0：ORIG 无 inven 具名槽（`call getCurCharacInvenW` 结果直留 %eax 跨
    // 实参装载至 insert_event_items）⇒ 表达式内联。
    const int insertResult = user->getCurCharacInvenW()->insert_event_items(
        giveList, updatedSlots,
        static_cast<eMoneyAddReason>(3), static_cast<eItemAddReason>(8),
        true, "game_server_msg_171", "game_server_msg_172");
    // ORIG 0x0856564e..0x08565675：`jle`/`jbe` 双短路 + `mov $1/mov $0` 汇聚
    // （无 bool 具名局部物化）⇒ 直接短路 if。
    // [b28d-PoolMisc2 复验（属主 PoolSmall，读数以交接）] 本件尾岛两形实测均劣：
    // ①尾部加显式 `return;` ⇒ ndiff 1→1 零效（2d3d/2d40 双 jmp 到同目标不变）；
    // ②`A && B` 拆嵌套正向 `if(A){if(B){}}` ⇒ ndiff 1→**6**（0x08565667..0x0856566e
    // 的 `jbe/mov $1/jmp/mov $0` 汇聚岛被折成单条 `seta %al`，反坏前轮短路形）。
    // ⇒ 维持短路 `&&` 原形；残 1 条 epilogue 双岛 jmp 按 P223 定案③ + N121 封顶。
    if (insertResult >= 1 && updatedSlots.size() >= 3u) {
        user->SendUpdateItem(CUser::eSendTarget_1,
                             static_cast<ENUM_ITEMSPACE>(0),
                             updatedSlots[0]);
    }
}

// OnRematchTimeout ORIG T 0x85665f92 报告 OnRematchTimeout.md
// b28d-solo-CFairMatchRooms 判词：DIFF（ndiff 248→~90，295→292 指令；帧
// 0x7c→0x6c 与 ORIG 对齐）。已收：P0 内联 CheckRematch 为最右 vararg（ORIG 无
// rematchable 槽，`call; movzbl %al,%edi` 直留 callee-saved）+ P3 `(unsigned char)`
// 收窄复现 movzbl；P0 内联 `room->m_relayMgr.OnReady`（去 relay 引用槽，帧回正）；
// team0Decision 具名槽→`switch (m_teams[0].m_field04)`（P251/P261，复现 ORIG
// `test %eax,%eax; je; test; js; cmp $2; jg` 无槽决策链）。残 = 槽号分区（pkt/kept/
// mgrTeam 深 4~0xc，N82）+ `CheckRematch(room)==1` 的 xor vs cmp/sete（跨版本）+
// N104 EH 尾块相对序；call 差集 46=46 CLEAR。
void CFairMatch::OnRematchTimeout(PvP_Room* room)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
    // P0：ORIG 无 rematchable 槽（`call CheckRematch; movzbl %al,%edi` 直接留
    // callee-saved 供 vararg）⇒ 表达式内联为最右实参。
    CSwitchLog(
        "virtual void fair_pvp::CFairMatch::OnRematchTimeout(PvP_Room*)",
        0xfd7, 0, 0)("pvp@log room(%d),%d,%d",
                     room->get_index(), room->get_pvp_battle_mode(),
                     static_cast<unsigned char>(CheckRematch(room)));

    if (CheckRematch(room) == 1) {
        m_teams[0].refresh();
        m_teams[1].refresh();
        PacketGuard pkt;
        room->battle_reset();
        for (int seat = 0; seat <= 7; ++seat) {
            if (room->get_user_seat(seat) != 0) {
                // P0：ORIG 无 relay 引用槽（`lea 0x620(%eax),%edx` 直传 this）
                // ⇒ 表达式内联。
                room->m_relayMgr.OnReady(seat, true);
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
    // （队决定位，OnAskRematch 写入；FM_TeamByte0 队头字节仅 CheckRematch 用）。
    // ORIG 0x085661f5 段 `test %eax,%eax; je; test; js; cmp $2; jg` = 无槽
    // switch 决策树（P251/P261；具名 team0Decision 恒产 cmpl mem 槽形）。
    switch (m_teams[0].m_field04) {
    case 0:
        _PullOutRoomTeam(room, static_cast<ENUM_SCHOOL_MATCH_TEAM>(0));
        if (m_teams[1].size() != 0) {
            PacketGuard pkt;
            pkt.put_header(0, 0x13b);
            pkt.finalize(true);
            m_teams[1].SendToTeam(pkt);
            // b28d-solo-CFairMatchRooms：P83/N84——ORIG 0x08566131 为结构整体
            // 赋值 rep movsl（0x12 dwords）。
            m_teams[0] = m_teams[1];
            m_teams[0].refresh();
            m_teams[1].reset();
            m_fieldB0 = 0;
        }
        break;
    case 1:
    case 2: {
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
            // b28d-solo-CFairMatchRooms：P83/N84——ORIG 0x08566265 为结构整体
            // 赋值 rep movsl（0x12 dwords）。
            m_teams[0] = m_teams[1];
        }
        m_teams[0].refresh();
        m_teams[1].reset();
        m_fieldB0 = 0;
        break;
    }
    }
}

// OnAskRematch ORIG T 0x856664a0 报告 OnAskRematch.md
// param：0=拒绝 1=接受 2=全队同意提交；队决定写入 m_field04(team)
// b28d-solo-CFairMatchRooms 判词：DIFF（ndiff 244→36，322→321 指令；room/myTeam/
// other/trace/pkt 槽全部回正 -0x24/-0x20/-0x1c/-0x34/-0x40）。已收：删具名 bool
// reject 改 P226 语句表达式 `if (({ !(A && B && C); }))`（ORIG 0x08566795/0x0856679c
// 无槽 `mov $1/mov $0` 岛 + test/je；具名 bool 曾多占 -0x19 槽把全帧推深 4）；if
// 链→`switch (decision)`（P251/P286，复现 ORIG `mov 0x10(%ebp),%eax; test; js;
// cmp $1; jle; cmp $2; je; jmp` 形参载入 + 寄存器比较链）；P227 链式。残 = 伪差 +
// 1 处 `m_teams[myTeam].m_field04=decision` 的 `add $0x20` vs `lea 0x20`（N107 族）；
// call 差集 41=41 CLEAR。
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
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
    CSwitchLog(
        "virtual void fair_pvp::CFairMatch::OnAskRematch(CUser*, int)",
        0x105c, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                      user->get_acc_name(), user->getCurCharacName(),
                      room->get_index(), room->get_pvp_battle_mode(), decision);

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
    // ORIG 0x085667a5 段：`mov 0x10(%ebp),%eax; test; js; cmp $1; jle;
    // cmp $2; je; jmp` = 形参载入 + 寄存器比较决策链 ⇒ switch（P251/P286；
    // if 链在 4.4 恒折内存直比 cmpl $imm,0x10(%ebp)）。
    switch (decision) {
    case 0:                                   // 拒绝 / 单人接受
    case 1:
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
        break;
    case 2:                                   // 全队满编 → 提交重赛
        *reinterpret_cast<unsigned char*>(&m_teams[myTeam]) = 1;   // 队头字节置可重赛标记
        // ORIG 0x08566795/0x0856679c：reject 为**无槽**寄存器物化（mov $1/mov $0
        // 岛 + test/je），非具名 bool 局部 ⇒ P226 语句表达式复现（禁具名变量）。
        if (({ !(m_teams[other].m_field04 == 2 &&
                 m_teams[other].size() != 0 &&
                 _CheckRematchable(room) == 1); })) {
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
        break;
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
// b28d-solo-CFairMatchRooms 判词：IDENT——P227 链式 + P0 内联
// `_ReturnTeam(room, _GetOppositeTeam(teamOf), 0)`（去 oppo 具名槽使全帧槽号回正）
// + P221 链式 trace；call 差集 29=29。fresh ndiff=0（267=267）。
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
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链。
    CSwitchLog(
        "virtual void fair_pvp::CFairMatch::OnEndPvPResult(PvP_Room*, CUser*)",
        0x10b8, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                      user->get_acc_name(), user->getCurCharacName(),
                      room->get_index(), room->get_pvp_battle_mode(),
                      room->get_waiter_count());
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
        // b28d-solo-CFairMatchRooms：P157/P183 去引用缓存（ORIG 每处字段访问
        // 重算地址）。
        if (m_teams[myTeam].m_players[i].m_flag != 0 &&
            m_teams[myTeam].m_players[i].m_user != 0)
            m_teams[myTeam].m_players[i].m_user->set_state(ch_state6);
    }
    if (m_teams[myTeam].m_field04 == 1) {
        // 已请求返回：把对方队伍整体搬回自建房并广播离场包
        // P0：ORIG 把 _GetOppositeTeam(teamOf) 内联为 _ReturnTeam 第 2 实参
        // （无 oppo 具名槽）。
        PvP_Room* kept = _ReturnTeam(room, _GetOppositeTeam(teamOf), 0);
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
        for (int i = 0; i <= 3; ++i) {
            // ORIG 循环体：先独立取 m_user 落槽，再重算地址读 m_flag（P48）。
            CUser* u = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && u != 0)
                u->Send(pkt);
        }
    }
}

// OnConnectP2PTimeout ORIG T 0x8564d3a 报告 OnConnectP2PTimeout.md
// [b28 判词·N] AE-ndiff=2：唯一真差 = _ReturnTeam 实参装载中 mgrTeam 槽 reload 与
// `movl $0x0,0xc(%esp)` 的先后交换——4.1 值先载、4.4 常量先写（P75/N82附 装载时刻
// 跨版本调度）。探针 /tmp/l2_p3.cpp w1-w7（具名/拆分/语句表达式/cast 全形）不可翻；
// 禁 register 绑定（P0/N96）。其余逐条同。
// b28d-solo-CFairMatchRooms 判词：IDENT——P211/P274：局部 unsigned mgrTeam 与
// _ReturnTeam 的 int 形参异型，令 4.4 先求值第 3 参再写第 4 参常量（探针
// /tmp/fm_p3.cpp w31）；call 差集 5=5。fresh ndiff=0（52=52）。
void CFairMatch::OnConnectP2PTimeout(PvP_Room* room)
{
    if (m_field04 == 4) {
        _SendMatchStatePacket(4, 2);
        // b28d-solo-CFairMatchRooms：局部 unsigned + _ReturnTeam 形参 int 异型
        // （P211/P274 旋钮）令 4.4 先求值第 3 参再写第 4 参常量 —— 复现 ORIG 4.1 的
        // `mov -0x1c(%ebp),%eax; movl $0x0,0xc(%esp); mov %eax,0x8(%esp)` 装载序
        // （探针 /tmp/fm_p3.cpp w31）。
        unsigned int mgrTeam = room->get_manager_team();
        _ReturnTeam(room, mgrTeam, static_cast<CUser*>(0));
        if (mgrTeam != 1) {
            m_teams[0] = m_teams[1];
        }
        m_teams[0].refresh();
        m_teams[1].reset();
    }
}

// OnConnectP2P ORIG T 0x8564a8e 报告 OnConnectP2P.md
// 判例（ADeepY4[4]）：NEAR 196=196（原 DIFF 204）。已收：连接探测循环改 ORIG 形
//   `for(i=0;(i<8)&&(i<port);++i){if(addr[i]=='\0'){_SendMatchStatePacket(4,2);OnEndFight(room);return;}}`（4.4 收敛 ORIG 底部
//   跳转循环布局）、mgrTeam 局部内联直参、GetPlayCountPerOneTeam()&0xff→(unsigned char) 复现
//   movzbl、m_fieldAC==比较复现 sete；双层队员循环去 PlayerInfoThird& 引用、hoist m_user 读序
//   （ORIG 先读 m_user 存槽再判 m_flag，Send 用槽）。残余=+0x40 兜底虚槽调用 ORIG 用 %edx
//   承载 fn、我们用 %eax（寄存器文本差 _NEAR_）+ 双层索引 ORIG 用 lea 链、我们用更紧凑寻址
//   （计数已对称）。
// [b28 判词·N] AE-ndiff=6：残余全为 +0x40 兜底虚槽 call *%edx(ORIG) vs call *%eax
// (4.4) = N7（间接 call 目标恒 %eax，b26 NL28 8 件实证族）。其余逐条同。
void CFairMatch::OnConnectP2P(CUser* user, PvP_Room* room,
                              char* addr, int port)
{
    (void)user;
    if (m_field04 != 4) {
        _SendMatchStatePacket(4, 2);
        m_field04 = 0;
        _ReturnTeam(room, room->get_manager_team(), 0);
        return;
    }

    int i;
    for (i = 0; (i < 8) && (i < port); ++i) {
        if (addr[i] == '\0') {
            // 未全部连上：发失败态并直接走 +0x40 兜底虚槽（OnEndFight）
            _SendMatchStatePacket(4, 2);
            OnEndFight(room);
            return;
        }
    }

    ++m_fieldAC;
    if (m_fieldAC == static_cast<int>((unsigned char)GetPlayCountPerOneTeam() * 2)) {
        PacketGuard pkt;
        pkt.put_header(0, 0x117);
        pkt.finalize(true);
        for (int t = 0; t < 2; ++t) {
            for (int p = 0; p < 4; ++p) {
                CUser* u = m_teams[t].m_players[p].m_user;
                if (m_teams[t].m_players[p].m_flag != 0 && u != 0)
                    u->Send(pkt);
            }
        }
        pkt.clear();
        room->battle_reset();
        for (int seat = 0; seat <= 7; ++seat) {
            if (room->get_user_seat(seat) != 0) {
                room->m_relayMgr.OnReady(seat, true);
            }
        }
        room->start_pvp();
        room->make_state_info(reinterpret_cast<char*>(&pkt));
        G_GameWorld()->send_all(pkt);
    }
}


// ORIG 反汇编实证：teamNo==1 分支 GetTeamRating 用 this+0x64（m_teams[1]），
// 队员循环读 this+0x24+i*0x10（m_teams[0]）；else 分支相反（0x1c / 0x6c）。
// BLOCKED 局部依赖：CEloRating::calc 本体与 PvPGlobalEnvironmentVariable::
// school_match_elo_k_ 定义尚未由任何 TU 提供（仅 ORIG 符号）。
// b28d-solo-CFairMatchRooms 判词：AE——去 PlayerInfoThird 引用缓存 + memberRating/
// oppositeRating 声明序翻转，逐条全等；仅剩 elo_k 全局重定位伪差。
// call 差集 6=6 CLEAR。
void CFairMatch::_UpdateRating(int teamNo, ENUM_ELO_RATING_SYSTEM system)
{
    // b28d-solo-CFairMatchRooms：ORIG 槽序 memberRating@-0x18（浅）/
    // oppositeRating@-0x1c（深）⇒ 4.4 按首次使用序，声明序须 memberRating 在前。
    float memberRating = 0.0f;
    float oppositeRating = 0.0f;
    CUser* member = 0;
    if (teamNo == 1) {
        oppositeRating = static_cast<float>(m_teams[1].GetTeamRating());
        for (int i = 0; i < 4; ++i) {
            // b28d-solo-CFairMatchRooms：P157/P183 去引用缓存——ORIG 每处字段
            // 访问都重算完整地址（mov i; mov this; add $1; shl $4; add; add $0x14），
            // 无 -0x14 指针槽。
            if (m_teams[0].m_players[i].m_flag != 0 &&
                m_teams[0].m_players[i].m_user != 0) {
                member = m_teams[0].m_players[i].m_user;
                memberRating = m_teams[0].m_players[i].m_rating;
                CEloRating::calc(memberRating, oppositeRating, system,
                                 PvPGlobalEnvironmentVariable::school_match_elo_k_);
                member->UpdateRating(memberRating);
            }
        }
    } else {
        oppositeRating = static_cast<float>(m_teams[0].GetTeamRating());
        for (int i = 0; i < 4; ++i) {
            if (m_teams[1].m_players[i].m_flag != 0 &&
                m_teams[1].m_players[i].m_user != 0) {
                member = m_teams[1].m_players[i].m_user;
                memberRating = m_teams[1].m_players[i].m_rating;
                CEloRating::calc(memberRating, oppositeRating, system,
                                 PvPGlobalEnvironmentVariable::school_match_elo_k_);
                member->UpdateRating(memberRating);
            }
        }
    }
}
