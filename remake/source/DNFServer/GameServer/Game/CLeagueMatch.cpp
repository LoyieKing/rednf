// ============================================================================
// df_game_r 还原 —— CLeagueMatch 实现（学院联赛匹配）
// 依据 docs/class_func_reports/CLeagueMatch/ 全部报告逐函数转写；TeamInfo /
// PlayerInfo 依据 docs/class_func_reports/TeamInfo/ 报告与 ORIG objdump。
// Packet_PvPResult 的 ORIG ctor（W 0x08568dc6，PacketHeader(this,0xfa5,0x80)）
// 由 SigTypes 域提供（当前仅有 m_pad[0x80] 形态）——此处按 objdump 手动展开
// 头部字段并注明 UNLINKED_PRODUCER。
//
// [b28d-solo-CLeagueMatch·S180 总判词] 本 TU 19 件（清单 /tmp/b28d_tu/
// CLeagueMatch.tsv）逐件复验；TU 口径 check_tu_routed.sh 恒 COMPILE OK。
//   · _getSameSchool：AE（§6.1 循环对齐 nop 已在 TU 口径剥离）。
//   · 其余 17 件 DIFF/NEAR 的唯一/主要残差 = **N-STRIDE**：stride 0x1c 的
//     `i*4` ORIG(4.1.2) 发 `lea 0x0(,%eax,4),%edx`，4.4.7 恒 `shl $0x2,%eax`，
//     后续 `lea(,%x,8); sub` 两步全等 ⇒ 仅第一步指令选择差（部分件连带 1 条
//     `mov %edx,%eax` 中转）。穷举探针全败：/tmp/b28d_cm/probe_stride.cpp 11 形
//     （下标/指针算术/数组引用/unsigned/long long/char* 视图/二维数组）、
//     probe2.cpp 索引类型 7 形（long/unsigned long/short/ushort/char/uchar/enum）、
//     4.4.6 后端（cc1plus446bin）、旗标矩阵 8 组（-mtune=i386/i486/i586、
//     -march=i386/i586、-Os、-fno-strength-reduce、-fno-tree-loop-optimize）
//     —— 4.4 恒 shl。语义等价，按 N 类封顶（建议入库 N125）。
//   · 寻址序可翻项已翻：P235 匿名 union 数组视角 m_teams[2]（CLeagueMatch.h，
//     等宽 0xe0、偏移/sizeof 不变）使 OnCompleteLoad/_SendMatchStatePacket 的
//     `t*0x70+i*0x1c` 复现 ORIG「偏移先算 + lea this」序（ndiff 42→26 ×2）；
//     _RegisterWaitingRoom 的 sort 双实参改 m_teams[0]/m_teams[1] 复现 ORIG
//     双独立算址（this+0x1c / this+0x8c，52→48）。OnReward 两形取优后**保留**
//     (&m_teamA)[t]（union 视角该件反劣 1390→1398，S166 实测）。
//   · 大件 p167 调用点差集复验 CLEAR：OnReward 45/45、OnMatchRoom 29/29、
//     CheckMatchable 4/4、_RegisterWaitingRoom 3/3、OnLeaveRoom 7/7、
//     _PutInRoom 15/15（0 missing / 0 extra）⇒ §6.2 语义完成、byte 层 N 封顶。
//   · 其余 N 归因：OnEndFight=N-P75（第三实参常量压栈时刻）、OnWalkOut=N7+
//     P75（虚调用目标 %ecx↔%edx + 实参装载序）、OnMatchRoom=N95（槽基差 8）+
//     P226 物化差、_PutInRoom=N104（EH 尾块相对序）、OnReward=N82（帧 0x1fc vs
//     0x24c 槽复用策略）。
//   · P0：本 TU 无 `register asm` 绑定（清绑数 0）。
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

// [b28c-21] P210 定义顺序：ORIG TeamInfo::reset `sub $0x28` 保守帧；4.4 对
// 已见定义的 PlayerInfo::reset 精确继承 $0x14 ⇒ 本定义前置于 callee 之前。
// [b28d-solo-CLeagueMatch] DIFF 19/20·N-STRIDE（S180 定稿）：stride 0x1c 的 i*4
// ORIG 用 lea(,%eax,4)、4.4 恒 shl $2（探针 /tmp/b28d_cm/probe_stride.cpp 11 形 +
// probe2.cpp 7 索引类型 + 4.4.6 后端 + 8 旗标全败）⇒ 4.4 多 1 条 mov %edx,%eax
// 中转；语义等价，结构性豁免。
void TeamInfo::reset()      // ORIG T 0x0855cb16
{
    for (int i = 0; i <= 3; ++i)
        m_players[i].reset();
}

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

// [b28d-lane6] AE（已修）：m_flag bool 化（P248 档②）+ `==` 正向包裹（P222）后
// 第一/二段与 ORIG 0x0855c95a 全等；x87 比较段经 fresh 重编译后亦归一（34=34）。
bool CompareRating(PlayerInfo const& a, PlayerInfo const& b)  // ORIG T 0x0855c952
{
    // b28d-lane6：m_flag bool 化（P248 档②）复现第一段 ORIG 0x0855c95a 双重 movzbl
    // + cmpl/jle；第二段改 `==` 正向包裹复现 `jne` 落 false 尾块（P222/P47）；
    // rating 段用 if/return true 保留 ORIG 的 seta/test/je 物化（P56）。
    if (a.m_flag > b.m_flag) return true;
    if (a.m_flag == b.m_flag)
    {
        if (a.m_rating > b.m_rating) return true;
    }
    return false;
}

// [b28d-solo-CLeagueMatch] DIFF 100/100·N-STRIDE：同 TeamInfo 族 stride 差异
// （ORIG lea 链 vs 4.4 shl 链，逐访问点 4 条对 4 条），语义等价，结构性豁免。
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

// [b28d-solo-CLeagueMatch] DIFF 91/91·N-STRIDE：xor $1 守卫形已按 P292/P248 修复
// （`if (!m_flag)` 正向包裹），残差全为 stride 0x1c 的 shl-vs-lea，语义等价。
int TeamInfo::add(CUser* user)   // ORIG T 0x0855cb54
{
    // [b28d-solo-CLeagueMatch] P292/P248 档②：ORIG 0x0855cb83 `movzbl; xor $1; test; je`
    // = bool 字段 + 裸 `if (!m_flag)` 正向包裹（P296）；continue 形反产 xor+jne 岛。
    for (int i = 0; i <= 3; ++i) {
        if (!m_players[i].m_flag) {
            m_players[i].m_flag = 1;
            m_players[i].m_user = user;
            m_players[i].m_accId = user->get_acc_id();
            m_players[i].m_rating = user->GetRating();
            m_players[i].m_schoolNo = user->GetSchoolNo();
            user->GetDailyBadge(m_players[i].m_badges);
            return 1;
        }
    }
    return 0;
}

// [b28d-solo-CLeagueMatch] DIFF 39/39·N-STRIDE：残差全为 stride 0x1c 的
// shl-vs-lea（3 访问点），语义等价，结构性豁免。
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

// [b28d-solo-CLeagueMatch] DIFF 23/23·N-STRIDE：stride 0x1c 的 i*4 ORIG 用
// lea(,%eax,4)、4.4 恒 shl $2（S180 探针 11 形 + 7 索引类型 + 4.4.6 + 8 旗标
// 全败），语义等价。
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
// [b22判词·N族] ORIG new(0x1c) 后、C1 call 前有 6 条 value-init 标量置零
// （+0x00/08/0c/10/14/18），GCC4.1 语义；4.4 视 user-provided ctor 类免置零直调。
// placement-new+try 实测反引入 operator new(void*,ptr) call + EH spill，C++98
// 无法复现该置零序列，语义等价（残差 6 insns），按 N 族结案，勿再开轮。
// [b28d-solo-CLeagueMatch] AE（TU 口径）：67=67 全等。原 N77 残差（new
// PVP_ROOM_INFO 的 6 条 value-init 清零 +0x00/08/0c/10/14/18）随 PvPTypes.h
// PVP_ROOM_INFO 保持「无用户 ctor」而恢复（P299：用户 ctor 抑制 value-init）；
// EH 尾块序亦已归一。本件无需再改。
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

// [b28d-solo-CLeagueMatch] DIFF 72/72·N-STRIDE：三守卫已按 P296 改嵌套正向 if，
// 与 ORIG 0x0855e1a3/a9/af 同落 +0xcb（消 3 个 jmp+nop 岛）；残差全为 stride 的
// shl-vs-lea，语义等价。
float CLeagueMatch::GetGap(IMatch* other)     // ORIG T 0x0855e12a
{
    float gap = 0.0f;
    for (unsigned int i = 0; i < (unsigned int)nPlayerCountPerOneTeam_; ++i) {
        CUser* mine = m_teamA.m_players[i].m_user;
        CUser* theirs = other->GetUser(i);
        // [b28d-solo-CLeagueMatch] P296：ORIG 三守卫各 `je` 直跳循环尾（无 continue
        // 的 jmp+nop 中继岛，0x0855e1a3/a9/af 同落 +0xcb）⇒ 嵌套正向 if 复现。
        if (m_teamA.m_players[i].m_flag != 0) {
            if (mine != 0) {
                if (theirs != 0) {
                    float a = mine->GetRating() * g_RankingIncr[i];
                    float b = theirs->GetRating() * g_RankingIncr[i];
                    gap += __builtin_fabsf(a - b);
                }
            }
        }
    }
    return gap;
}

// [b28d-solo-CLeagueMatch] DIFF 29/29·N-STRIDE：bool 化 + 裸 `if(!m_flag)` 的
// xor 段已与 ORIG 0x0855e5e9 一致；残差为 stride 0x1c 的 shl-vs-lea，语义等价。
CUser* CLeagueMatch::GetUser(unsigned int idx)   // ORIG T 0x0855e5e4
{
    if (idx > 3)
        return 0;
    // b28d-lane6：m_flag bool 化 + 裸 `if(!field)` ⇒ ORIG 0x0855e5e9 的
    // `movzbl 0x18; xor $1; test %al,%al; je`（P248 档②，探针 /tmp/l6p/p6.cpp f2）。
    if (!m_teamA.m_players[idx].m_flag)
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

// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %ecx↔%eax 互换（ORIG
// mov (%eax),%ecx+call *%ecx vs 4.4 mov (%eax),%eax+call *%eax，16 条逐条
// 长度/助记符全等），临时/引用/限定名不可翻，语义同。
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
// [b28d-solo-CLeagueMatch] DIFF 142/142·真 bug 已修 + N-STRIDE：ORIG 0x0855d245/
// d273 以**第 3 参** team 判 m_teamA/m_teamB（原码误用第 4 参 flag，调用点
// OnWalkOut/PvP_Room 均传 (user,room,team,seat)）；本波再修两处 P0：
// ①`_ReturnTeam(room, room->get_manager_team(), user)` 内联实参（ORIG
// 0x0855d1fc 返回值 %eax 直压 0x8(%esp)，无 managerTeam 具名槽）；
// ②`int idx = 0;` + `for (idx = 0;…)` 双初始化（ORIG 0x0855d232/d239 两次
// movl $0,-0xc）且删 selfRating/oppoRating 的 0.0f 初始化（ORIG 无该清零，
// P171）；gap -5→0，残差 6 处 stride shl-vs-lea（N-STRIDE；p167 7/7 CLEAR），
// 语义等价。
void CLeagueMatch::OnLeaveRoom(CUser* user, PvP_Room* room, int team, int /*flag*/)
{
    if (m_state == 4) {
        _SendMatchStatePacket(4, 2);
        m_state = 0;
        // [b28d-solo-CLeagueMatch] P0：ORIG 0x0855d1fc..0x0855d219 的
        // get_manager_team() 返回值在 %eax 直压 0x8(%esp)（无具名局部槽往返）
        // ⇒ 内联实参，删 managerTeam 局部。
        _ReturnTeam(room, room->get_manager_team(), user);
    } else if (m_state == 5) {
        // [b28d-solo-CLeagueMatch] P191：ORIG 0x0855d232/d239 两次 `movl $0x0,-0xc`
        // = 声明初始化 + for 初始化双形；P171：ORIG 无 selfRating/oppoRating 的
        // float 0.0f 清零（仅 idx 清零），未命中时由 idx<=3 保护 ⇒ 删初始化式。
        int idx = 0;
        float selfRating;
        float oppoRating;
        for (idx = 0; idx < 4; ++idx) {
            // [b28d-solo-CLeagueMatch] 真 bug 修正：ORIG 0x0855d245/d273 以**第 3 参**
            // `cmpl $0x1,0x14(%ebp)` 判队（m_teamA +0x24 / m_teamB +0x94），原码误用
            // 第 4 参 flag（0x18）——调用点 OnWalkOut/PvP_Room 均传 (user,room,team,seat)。
            if (team == 1) {
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

    if (team == 1)
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

    // ORIG movl $0x0,-0xc 预清（P198③ 声明=0+独立赋值两句式）。
    int err = 0;
    err = _CheckMatchable(room);
    // P225：ORIG `cmpl $0,slot; setne %al; test %al; je <主流程>` bool 四段物化
    // → while+return 形精确复现（if 直判被 4.4 折成 cmp;je 两段）。
    while (err != 0) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(199),
                                 static_cast<unsigned char>(err & 0xff));
        return 0;
    }

    if (m_state != 2)
        _RegisterWaitingRoom(room);

    m_state = 1;
    // N96 清绑：ORIG mov 0x18(%eax),%ebx 跨 get_index call 缓存 m_authValue（P87）；
    // 成员表达式直传（P0 无具名局部）。
    TimerStartMatching::registNextTimer(10, room->get_index(), m_authValue);
    _SendMatchStatePacket(0, 1);
    return 1;
}

// ORIG T 0x0855dc64。ORIG 返回 int：失败路径 → 0；成功 → 1。
// [b28d-solo-CLeagueMatch] DIFF 278/278·真语义 bug 已修 + N95（S180 复验，
// p167 29/29 CLEAR）：①0x2c2 守卫
// 原用 theirRoom->get_player_count()，ORIG 0x0855de5e `mov 0xc(%ebp),%eax` +
// Ghidra C `get_player_count(param_1)` 实证为**形参 room** ⇒ 改回 room（真 bug）；
// ②leader/theirRoom 按 ORIG 0x0855dca0/dca7 函数头预清 0（P198③，gap 2→0）；
// ③五处 cMyTrace 改全表达式临时链（P221）翻正槽方向（ORIG -0x6c→-0x2c 由深到浅，
// 原具名形 -0x38→-0x78 反向）；残差为全帧槽基差 8（N95：4.4 槽基恒 ebp-8 起、
// 4.1 可顶到 ebp）与 setg/test 物化差；p167 CLEAR（29/29），语义闭合。
int CLeagueMatch::OnMatchRoom(PvP_Room* room, int authValue)
{
    if (m_authValue != authValue) {
        // [b28d-solo-CLeagueMatch] P221：ORIG 五处 trace 槽 -0x6c/-0x5c/-0x4c/
        // -0x3c/-0x2c（由深到浅），具名声明形 4.4 反向（-0x38→-0x78）⇒ 改
        // 「构造即调用」全表达式临时链翻槽方向。
        cMyTrace(__PRETTY_FUNCTION__, 0x295, 0)("OnMatchRoom timeout");
        return 0;
    }
    if (m_state != 1)
        return 0;

    // [b28d-solo-CLeagueMatch] P198③：ORIG 0x0855dca0/dca7/dcb4 三处预清
    // （Ghidra C：leader=0; theirRoom=0; opponent=0 在 Match 调用前）
    // ⇒ 函数头声明并置 0（双槽置 0 形），后文赋值；补 gap 2。
    CUser* leader = 0;
    PvP_Room* theirRoom = 0;
    IMatch* opponent = 0;
    pWaitingRoomList_->Match(this, &opponent);   // ORIG 丢弃返回值，仅判 opponent==0
    if (opponent == 0) {
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    leader = opponent->GetUser(0);
    if (leader == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2b0, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b0);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    theirRoom = reinterpret_cast<PvP_Room*>(leader->GetPVPRoom());
    if (theirRoom == 0) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2b9, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2b9);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    // [b28d-solo-CLeagueMatch] 真语义 bug 修正（Ghidra C `get_player_count(param_1)`
    // + ORIG 0x0855de5e `mov 0xc(%ebp),%eax` 实证）：此守卫检查的是**形参 room**
    // 的人数，非 theirRoom（theirRoom 仅用于下方 badSize）。
    if (room->get_player_count() > (int)nPlayerCountPerOneTeam_) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2c2, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2c2);
        m_state = 2;
        _SendMatchStatePacket(1, 1);
        return 0;
    }

    bool badSize =
        ((int)nPlayerCountPerOneTeam_ < m_teamA.size()) ||
        ((int)nPlayerCountPerOneTeam_ < theirRoom->get_player_count());
    if (badSize) {
        cMyTrace(__PRETTY_FUNCTION__, 0x2ca, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0x2ca);
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
// [b28d-solo-CLeagueMatch] DIFF 45/45·N-P75（S180 复验）：ORIG 0x0855e594 第二
// 实参先入 %eax 再压常量 0，4.4 恒先压常量；cast/NULL/具名局部 3 形复测不可翻
// （S180 追加逗号求值序形 ((void)managerTeam,(CUser*)0) 亦不变）⇒ 4 形全败，
// 语义等价。
void CLeagueMatch::OnEndFight(PvP_Room* room)
{
    int managerTeam = room->get_manager_team();
    m_state = 0;
    // b28d-lane6 [N-P75]：ORIG 0x0855e594 先 `mov -0x1c(%ebp),%eax`（第二实参装载）
    // 再 `movl $0x0,0xc(%esp)`（第三实参常量压栈）；4.4 恒先压常量。cast/NULL/
    // 具名局部 3 形实测均不可翻（P75 跨版本实参装载时刻）。
    _ReturnTeam(room, managerTeam, 0);
    if (managerTeam != 1)
        m_teamA = m_teamB;   // ORIG rep movsl 0x1c dwords（P83 结构整体赋值）
    m_teamA.refresh();
    m_teamB.reset();
}

// ORIG T 0x0855e4c4。ORIG 返回 1。
// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %ecx↔%eax 互换 + 实参装载序
// 跨版本差（ORIG 先槽 -0x10 后形参 0x10，4.4 反序——N80 族不可翻），33 条
// 长度/助记符全等。
// [b28d-solo-CLeagueMatch] NEAR 33/33·N7（S180 复验）：虚调用目标
// %ecx(ORIG 0x0855e4dc)↔%edx(4.4.7) + 实参装载序（P75）；33 条助记符逐条全等，
// 语义等价。
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
// §6.2 巨件核销：P167 45/45 [0缺0多]（修前 42/45）；gap 79→35；残差归因 N86/N82
// （t*0x70/j*3 寻址代数分解跨版本差：ORIG shl$4+lea+sub 折乘链 vs 4.4
//   shl$3+add，纯分解形态、语义同；全帧槽号平移）。
// 真修三处：
//   1) 结果包改栈对象 Packet_PvPResult res 走真 ctor（W 0x08568dc6≡
//      PacketHeader(0xfa5,0x80)，重建侧符号已在）；原手写展开的
//      reversed1/m_connNo 清零属发明语义（ORIG ctor 不清），删。
//   2) 实时徽章回填条件修正：ORIG 0x0855f539 固定寻址 this+i*0x1c+0x98/0x94
//      = teamB 的 m_flag/m_user（与 t 无关），原实现错挂 t 分支。
//   3) GetDailyBadge 由循环改 3 条独立调用（ORIG $0x0/$0x1/$0x2 立即数直传
//      三处 call，P0），读取 user 按数组化寻址 (&m_teamA)[t].m_players[i].m_user
//      复现 ORIG 乘法寻址链（无具名缓存槽）。
// [b28d-solo-CLeagueMatch] DIFF 996/962·N82（S180 复验）：帧 0x1fc vs 0x24c，
// 4.4 槽复用策略差（§6.2 三件套口径：p167 调用点差集 45/45 CLEAR [0 缺 0 多]、
// gap 34 归因槽复用与 t*0x70/j*3 寻址代数分解；S180 试 P235 union 视角
// m_teams[t] 该件反劣（1390→1398）⇒ 两形取优保留 (&m_teamA)[t]；剩余全为
// 槽号平移与 shl-vs-lea；语义完成、byte 层 N 封顶）。
void CLeagueMatch::OnReward(PvP_Room* /*room*/, int winTeam, bool draw)
{
    // ---- 结果包（ORIG 0x0855eb37 call W Packet_PvPResult::C1Ev 0x08568dc6，
    //       重建侧已有该 W 符号（PacketHeader(0xfa5,0x80)），改栈对象真 ctor；
    //       原手写字段展开含 reversed1/m_connNo 额外清零，属发明语义已删——
    //       ORIG ctor 仅设包头，不清该 2 字节） ----
    Packet_PvPResult res;

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
                res.m_team[t].m_dailyBadge[j][i] =
                    static_cast<char>(ti.m_players[j].m_badges[i]);
                res.m_team[t].m_newFlag[j][i] = 0;
            }
        }
    }

    // 2) 各分支记徽章（draw→type1 / A 队胜→type2 / 其余→type0）。
    int type = 0;
    if (draw) {
        type = 1;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            res.m_team[0].m_newFlag[j][1] = 0;   // 自队蓝章清零
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[0].m_newFlag[j][2], false);
            res.m_team[1].m_newFlag[j][1] = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[1].m_newFlag[j][2], false);
        }
    } else if (winTeam == 1) {
        type = 2;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(1),
                     &res.m_team[0].m_newFlag[j][1], false);
            res.m_team[0].m_newFlag[j][2] = 0;
            res.m_team[1].m_newFlag[j][1] = 0;
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[1].m_newFlag[j][2], false);
        }
    } else {
        type = 0;
        for (int j = 0; j < (int)nPlayerCountPerOneTeam_; ++j) {
            res.m_team[0].m_newFlag[j][1] = 0;
            addBadge(&m_teamA.m_players[j], static_cast<ENUM_BADGE_TYPE>(2),
                     &res.m_team[0].m_newFlag[j][2], false);
            addBadge(&m_teamB.m_players[j], static_cast<ENUM_BADGE_TYPE>(1),
                     &res.m_team[1].m_newFlag[j][1], false);
            res.m_team[1].m_newFlag[j][2] = 0;
        }
    }

    // 写包头数据字段（asm：elo_k2_@+0x0a 4B、winType@+0x0e 1B、playCount@+0x0f 1B）。
    res.m_eloK = eloK2;
    res.m_winType = winType;
    res.m_playCount = playCount;

    // 3) 两队 schoolNo 表（intsBase=pkt+8，elem=(t*14+i+8)，per-team stride 56B）与
    //    school 排他检查（同校不算对手）及绿章提示。
    for (int t = 0; t <= 1; ++t) {
        TeamInfo& ti = (t == 0) ? m_teamA : m_teamB;
        for (int i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            res.m_team[t].m_schoolNo[i] = ti.m_players[i].m_schoolNo;
        }
        for (int i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            unsigned int mySchool = res.m_team[t].m_schoolNo[i];
            unsigned int* schools = res.m_team[t].m_schoolNo;
            if (!_getSameSchool(schools, i,
                                (unsigned int)nPlayerCountPerOneTeam_, mySchool)) {
                res.m_team[t].m_newFlag[i][0] = 0;
                if (res.m_team[t].m_newFlag[i][1] != 0 ||
                    res.m_team[t].m_newFlag[i][2] != 0) {
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
            } else if (res.m_team[t].m_newFlag[i][1] == 0 &&
                       res.m_team[t].m_newFlag[i][2] == 0) {
                addBadge(&ti.m_players[i], ENUM_BADGE_TYPE_0,
                         &res.m_team[t].m_newFlag[i][0],
                         false);
            } else {
                addBadge(&ti.m_players[i], ENUM_BADGE_TYPE_0,
                         &res.m_team[t].m_newFlag[i][0],
                         true);
            }
        }
        // 回填实时徽章计数。ORIG 0x0855f539..0x0855f6da：条件寻址固定
        // this+i*0x1c+0x98(m_flag)/0x94(m_user) = **teamB** 槽，与 t 无关；
        // 读取 user 为 this+i*0x1c+t*0x70+0x24（含 t）；GetDailyBadge 为 3 条
        // 独立调用（$0x0/$0x1/$0x2 立即数直传，非循环，P0）。
        for (int i = 0; i < (int)nPlayerCountPerOneTeam_; ++i) {
            if (m_teamB.m_players[i].m_flag == 0 ||
                m_teamB.m_players[i].m_user == 0)
                continue;
            res.m_team[t].m_dailyBadge[i][0] =
                static_cast<char>((&m_teamA)[t].m_players[i].m_user->GetDailyBadge(
                    static_cast<ENUM_BADGE_TYPE>(0)));
            res.m_team[t].m_dailyBadge[i][1] =
                static_cast<char>((&m_teamA)[t].m_players[i].m_user->GetDailyBadge(
                    static_cast<ENUM_BADGE_TYPE>(1)));
            res.m_team[t].m_dailyBadge[i][2] =
                static_cast<char>((&m_teamA)[t].m_players[i].m_user->GetDailyBadge(
                    static_cast<ENUM_BADGE_TYPE>(2)));
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
            res.m_team[t].m_accId[i] = ti.m_players[i].m_accId;
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
        guard.operator->()->put_binary(&res, 0x80);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    } else {
        GlobalData::s_school_proxy->SendPacket(reinterpret_cast<char*>(&res), 0x80);
    }
}

// ORIG T 0x0855e22c：向全体成员广播加载完成通知（0x81 包，携带 unique id）。
// [b28d-solo-CLeagueMatch] DIFF 97/97·N-STRIDE（S180 更新）：删 TeamInfo&/
// PlayerInfo& 引用槽 + member 具名局部（ORIG -0x10）后条数 97=97；S180 再以
// P235 匿名 union 视角 m_teams[t] 复现 ORIG「偏移先算 + lea this + add $0x24」
// 寻址序（0x0855e2a2 段，改前 4.4 先算 this+0x1c 基址），残差仅剩 i*4 的
// shl-vs-lea（N-STRIDE），语义等价。
void CLeagueMatch::OnCompleteLoad(CUser* user)
{
    PacketGuard pkt;
    pkt.put_header(0, 0x81);
    pkt.put_short(user->get_unique_id() & 0xffff);
    pkt.finalize(true);
    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 无 TeamInfo&/PlayerInfo& 引用缓存槽
    // （Ghidra 仅 t/i/user 三局部）；以 (&m_teamA)[t] 数组化寻址 + user 具名局部
    // （ORIG -0x10）复现，并消 4.4 的引用槽。
    for (int t = 0; t <= 1; ++t) {
        for (int i = 0; i < 4; ++i) {
            CUser* member = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && member != 0)
                member->Send(pkt);
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
        // [b28c-21 代落 C28L25 spec] ORIG `call _CheckSchool; xor $1,%eax;
        // test %al,%al; je` = bool callee 裸形（P168a①）；int callee 恒 sete。
        if (!_CheckSchool(room, user))
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

// [b28d-solo-CLeagueMatch] DIFF 202/202·真语义 bug 已修 + N-STRIDE：①段位窗口边界
// 取反真 bug（ORIG 0x0855fbc0..fbf3 `jg` 跳过 return 0 ⇒ return false 条件是
// `self∈[12,14] && other<=10`，原实现写成 `other>10`）——改 `<=10` 后 jg/jle
// 差异同步消失；②i 复用同一槽 + `int i=0` 双初始化 + 具名 diff（P191/P0）；
// 残差 12 处为 stride 0x1c 的 shl-vs-lea（N-STRIDE：4.1 `lea 4;lea 8;sub` vs
// 4.4 `shl 2;lea 8;sub`，S180 探针 /tmp/b28d_cm/probe_stride.cpp 11 形 +
// probe2.cpp 7 索引类型 + 4.4.6 后端 + 8 旗标全败）；p167 CLEAR（4/4）。
bool CLeagueMatch::CheckMatchable(IMatch* other)   // ORIG T 0x0855fa14
{
    // ORIG 汇编对 other 直接按对象前部偏移读其第一队 PlayerInfo 槽
    // （this+slot*0x1c+0x24/0x28/0x2c），依赖 IMatch 派生类共享布局。
    // ORIG 无引用缓存：每次成员访问都重算 base+i*0x1c（每处 7 条寻址指令），
    // 且胜点比较是 MAX 宏形态 —— getPVPResultR()->m_pvpGrade 被求值两次；
    // 差值判定用 abs()（单临时槽 sar/xor/sub 展开），不得手写三遍表达式。
    int maxWinSelf = 0;
    int maxWinOther = 0;
    // [b28d-solo-CLeagueMatch] P0/P191：ORIG 0x0855fa1b/fa22/fa29/fa30 四清零
    // = maxWinSelf/maxWinOther + i 双初始化（`int i = 0;` + `for (i = 0;…)`）；
    // 三个循环共用同一 i 槽 -0x14（ORIG 0x0855faf8/fc1d 复用），非各自 for
    // 内声明（4.4 会各占独立槽，实测多 -0x20/-0x18 两槽）。
    int i = 0;

    for (i = 0; i < 4; ++i) {
        if (m_teamA.m_players[i].m_flag != 0 && m_teamA.m_players[i].m_user != 0)
            maxWinSelf =
                (m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade < maxWinSelf)
                    ? maxWinSelf
                    : m_teamA.m_players[i].m_user->getPVPResultR()->m_pvpGrade;
    }
    for (i = 0; i < 4; ++i) {
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

    // [b28d-solo-CLeagueMatch] 真语义 bug 修正（Ghidra C 对照 0x0855fbc0..fbf3）：
    // ORIG 第一组 `cmpl $0xb,self; jle 第二组 / cmpl $0xe,self; jg 第二组 /
    // cmpl $0xa,other; jg 第二组` ⇒ **跳过** return 0 的条件是 other > 10，
    // 即 return false 当 `self∈[12,14] && other<=10`（Ghidra `(10 < local_1c)`
    // 为跳转条件）；原实现写成 `other > 10` 恰把边界取反（真语义 bug）。
    if ((maxWinSelf >= 12 && maxWinSelf <= 14) && maxWinOther <= 10)
        return false;
    if ((maxWinOther >= 12 && maxWinOther <= 14) && maxWinSelf <= 10)
        return false;
    // [b28d-solo-CLeagueMatch] P0：ORIG 0x0855fc07 把 abs 结果写具名槽 -0x10
    // （mov %edx,-0x10(%ebp); sub %eax,-0x10(%ebp)）⇒ 具名 diff 局部。
    int diff = abs(maxWinSelf - maxWinOther);
    if (diff > 4)
        return false;

    for (i = 0; i < 4; ++i) {
        for (int b = 0; b < 4; ++b) {
            if (m_teamA.m_players[i].m_flag != 0 &&
                reinterpret_cast<CLeagueMatch*>(other)->m_teamA.m_players[b].m_flag != 0 &&
                m_teamA.m_players[i].m_schoolNo ==
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
// [b28d-solo-CLeagueMatch] DIFF 32/32·N-STRIDE：残差全为 stride 0x1c 的
// shl-vs-lea（2 访问点），语义等价，结构性豁免。
int CLeagueMatch::GetSchoolNo()              // ORIG T 0x0855f9aa
{
    for (int i = 0; i <= 3; ++i) {
        if (m_teamA.m_players[i].m_flag != 0)
            return m_teamA.m_players[i].m_schoolNo;
    }
    return 0;
}

// ORIG W 0x08568834：call *(vptr+0x90) 并返回其值 ≡ GetSchoolNo()。
// 【N7 封顶】b26 lane19：虚调用 callee 寄存器 %edx↔%eax 互换（12 条逐条全等
// 仅寄存器名差），N7 判据命中。
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

// [b28d-solo-CLeagueMatch] AE（TU 口径）：196=196 全等。修法=删 passwd/title
// 引用缓存局部（P183/P0，消 2 个指针槽 -0x24/-0x20）+ S27 PvPTypes.h
// PVP_ROOM_INFO::byte8 uchar→bool（消 uchar→bool 的 test/setne 2 条）。
// p167 调用点差集 CLEAR（39/39）。
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

    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 0x0855d5xx 每处重算
    // `mov 0x8(%ebp),%eax; add $0x14/$0x8,%eax`（无 passwd/title 引用缓存槽）⇒ 删引用
    // 局部改直接成员访问，消 2 个指针槽（-0x24/-0x20）。
    int err = pvp->create_room(user, m_roomInfo.byte4,
                               const_cast<char*>(m_roomInfo.title.c_str()), m_roomInfo.short6,
                               m_roomInfo.byte8 != 0, m_roomInfo.passwd.c_str(),
                               static_cast<int>(m_roomInfo.passwd.length()),
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

// [b28d-solo-CLeagueMatch] DIFF 132/132·N-STRIDE+N85（S180 更新）：foundMin 改
// int（ORIG -0x20 `cmpl $0`）+ 两循环改嵌套正向 if（P296）后条数 132=132；
// S180 再把 sort 双实参改 P235 视角 m_teams[0]/m_teams[1]，复现 ORIG
// 0x0855da16 的 `add $0x8c` / `add $0x1c` 双独立算址；残差仅剩 stride
// shl-vs-lea 与 x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 seta 形），
// 语义等价。
void CLeagueMatch::_RegisterWaitingRoom(PvP_Room* room)   // ORIG T 0x0855d884
{
    // [b28d-solo-CLeagueMatch] ORIG foundMin 为 int（-0x20 `cmpl $0x0,-0x20`）非 bool；
    // 两循环均嵌套正向 if（P296）：m_flag 正向 `test;je`（0x0855d8c7）、
    // get_team==3 正向（0x0855d98a），消 continue 的 jmp+nop 中继岛。
    int foundMin = 0;
    float minRating = 0.0f;
    for (int i = 0; i < 4; ++i) {
        if (m_teamA.m_players[i].m_flag != 0) {
            if (foundMin == 0) {
                minRating = m_teamA.m_players[i].m_rating;
                foundMin = 1;
            }
            if (m_teamA.m_players[i].m_rating < minRating)   // asm fucompp：小于最小值则更新
                minRating = m_teamA.m_players[i].m_rating;
        }
    }
    minRating -= 1.0f;

    for (int seat = 0; seat <= 7; ++seat) {
        if (room->get_team(seat) == 3) {
            CUser* seatUser = reinterpret_cast<CUser*>(room->get_user_seat(seat));
            for (int i = 0; i < 4; ++i) {
                if (m_teamA.m_players[i].m_user == seatUser && seatUser != 0)
                    m_teamA.m_players[i].m_rating = minRating;
            }
        }
    }

    std::sort<PlayerInfo*, bool (*)(PlayerInfo const&, PlayerInfo const&)>(
        m_teams[0].m_players, m_teams[1].m_players, CompareRating);  // ORIG weak sort 实例
        // [b28d-solo-CLeagueMatch] P235：ORIG 0x0855da16 两实参各自独立算址
        // （this+0x1c / this+0x8c）；写 m_teamA.m_players+4 会让 4.4 先算基址再
        // lea 0x70 ⇒ 用等宽数组视角 m_teams[0]/m_teams[1] 复现 ORIG 双独立地址。
    pWaitingRoomList_->AddRoom(this);                               // IRoomList vtable+0x00
}

void CLeagueMatch::_BackupRoomInfo(PvP_Room* room)     // ORIG T 0x0855d554
{
    room->get_room_info(&m_roomInfo);
}

// [b28d-solo-CLeagueMatch] AE（TU 口径）：69=69 全等。修法=P225 `while(cond){return X;}`
// 复现 ORIG 尾 `cmp %eax,%edx; setne %al; test %al,%al; je` 的 bool 四段物化
// （三目/if 直跳形恒被 4.4 折成 `jne`，探针 /tmp/b28d_cmatch 两形实测：三目
// ndiff 8、bool 局部占槽反劣 13、while+break 归零）；槽序按 P0 对齐
// （emptySeats/X/i/user，X=logArg0 复现 -0x14 具名局部，删 school 局部改两次
// GetSchoolNo P48）。
int CLeagueMatch::_CheckMatchable(PvP_Room* room) const   // ORIG T 0x0855da56
{
    int emptySeats = 0;
    // [b28d-solo-CLeagueMatch] P0：ORIG -0x14 具名局部（恒 0）作 logFormat 首个 %d
    // 实参（0x0855dad6 `mov -0x14(%ebp),%eax; mov %eax,0x14(%esp)`）；删除 school
    // 局部改两次 GetSchoolNo（P48 O0 无 CSE），槽序 emptySeats/X/i/user 与 ORIG 全等。
    int logArg0 = 0;
    for (int i = 0; i <= 7; ++i) {
        if (room->get_team(i) == 3) {
            ++emptySeats;
            continue;
        }
        CUser* user = reinterpret_cast<CUser*>(room->get_user_seat(i));
        if (user == 0)
            continue;
        if (user->GetSchoolNo() == 0) {
            LogManager::logFormat(
                1, "MatchingSystem.cpp",
                __PRETTY_FUNCTION__, 599,
                "(%d) found a different school no(%d)", logArg0, user->GetSchoolNo());
            return 0x16;
        }
    }
    while (m_teamA.size() - emptySeats != (int)nPlayerCountPerOneTeam_) {
        return 0x16;
    }
    return 0;
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

// [b28d-solo-CLeagueMatch] DIFF 85/83·N104（S180 复验，p167 15/15 CLEAR）：
// EH 尾块相对序（ORIG 正常出口块在函数末、unwind 居中；块内容逐条全等，
// §6.2 口径语义闭合）。
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

// [b28d-solo-CLeagueMatch] AE（TU 口径）：79=79 全等。修法=P223② 显式提前
// `continue`（同层多 continue 复现 ORIG 0x0855e477/e47a/e47d 三个独立
// nop+jmp 中继岛；else-if 合并形被 4.4 jump-thread 少 2 岛，gap 2→0）。
void CLeagueMatch::_ReturnTeam(PvP_Room* room, int team, CUser* except)  // ORIG T 0x0855e378
{
    // [b28d-solo-CLeagueMatch] P0：ORIG 槽序 user(-0x18)/madeRoom(-0x14)/seat(-0x10)
    // ⇒ user 声明于函数头并置 0（有槽往返=具名局部），循环内赋值。
    CUser* user = 0;
    PvP_Room* madeRoom = 0;
    for (unsigned int seat = 0; seat <= 7; ++seat) {
        user = reinterpret_cast<CUser*>(room->get_user_seat(static_cast<int>(seat)));
        if (user == 0)
            continue;
        int seatTeam = room->get_team(static_cast<int>(seat));
        if (team == seatTeam) {
            // [b28d-solo-CLeagueMatch] P223②：ORIG 0x0855e47a/e47d 两个独立
            // nop+jmp 中继岛 = 同层多 `continue`（提前 continue 形），
            // else-if 合并形被 4.4 jump-thread 少 2 岛。
            if (team != 2)
                continue;
            room->OnSetPVPSeatState(user, static_cast<int>(seat),
                                    static_cast<ENUM_SEAT_STATE>(1),
                                    static_cast<ENUM_WALKOUT_TYPE>(2));
            continue;
        }
        if (except != 0 && user == except)
            continue;
        _PullOutRoom(user, room, static_cast<int>(seat));
        if (madeRoom == 0)
            madeRoom = _MakeRoom(user);
        else
            _PutInRoom(user, madeRoom, true);
    }
    m_state = 0;
}

// [b28d-solo-CLeagueMatch] DIFF 95/95·N-STRIDE（S180 更新）：删 TeamInfo& 引用
// 缓存 + user 具名局部（ORIG t/i/user 三槽）后条数 95=95；S180 再以 P235 匿名
// union 视角 m_teams[t] 复现 ORIG「偏移先算 + lea this」序，残差仅剩 i*4 的
// shl-vs-lea（N-STRIDE），语义等价。
void CLeagueMatch::_SendMatchStatePacket(int state, int teams)   // ORIG T 0x0855e664
{
    PacketGuard pkt;
    pkt.put_header(0, 0xaf);
    pkt.put_byte(state);
    pkt.finalize(true);
    // [b28d-solo-CLeagueMatch] P183/P0：ORIG 无 TeamInfo& 引用槽（t/i/user 三局部，
    // user 具名缓存 -0xc，寻址 this+i*0x1c+t*0x70+0x24）⇒ 数组化访问复现。
    for (int t = 0; t < teams; ++t) {
        for (int i = 0; i <= 3; ++i) {
            CUser* member = m_teams[t].m_players[i].m_user;
            if (m_teams[t].m_players[i].m_flag != 0 && member != 0)
                member->Send(pkt);
        }
    }
}

// [b28d-solo-CLeagueMatch] AE：TU 口径 identical/AE（25=25 全等；game_func_report
// 单函数口径多 1 条循环对齐 nop，§6.1/N112 已在 TU 口径剥离）。
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

bool CLeagueMatch::_CheckSchool(PvP_Room* room, CUser* user)  // ORIG T 0x0855cef8
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

// [b28d-solo-CLeagueMatch] AE（TU 口径）：207=207 全等。修法=P305 的 S18 规避形
// `switch((int)badgeType)`：既避免 -fstrict-enums 删越界 case（ENUM_BADGE_TYPE
// 仅定义 0，曾删 case 2 ⇒ p167 MISSING 各 1），又复现 ORIG 0x0855e90f/914/919
// 的「形参载入 %eax + 寄存器比较链」（P251/P286；if 链恒折 `cmpl $K,mem` 每段
// 重读内存，探针 /tmp/b28d_cmatch/probe4.cpp sw3 vs if3 实证）；p167 25/25 CLEAR。
int CLeagueMatch::addBadge(PlayerInfo* info, ENUM_BADGE_TYPE badgeType,
                           unsigned char* msgFlag, bool extra)   // ORIG T 0x0855e7ee
{
    // [b23S3C] 直返形（ORIG 各 guard return-0 body fall-through）；flag 判定
    // 为 `== 1`（ORIG xor$1：flag∉{0,1} 值域下 ==1 才过，`!flag` 语义差）；
    // 红蓝合计去 sum 具名局部（ORIG 无槽：movswl/cwtl/lea 直比 cmp $9,%eax）；
    // count 双句式 `int count=0; if(extra)++count`（ORIG `movl $0; cmpb; addl$1`
    // 分支岛非三目物化）；三分支各复制完整 findString+snprintf call 序列
    // （ORIG 三独立岛，非选串指针汇聚）；值表达式 (int)*msgFlag+count 右→左
    // 求值先于 findString（ORIG ebx 先算）。
    // N96 清绑：字段表达式直读（P0）；xor$1 单段形复测。
    if ((info->m_flag ^ 1) != 0)
        return 0;
    if (info->m_user == 0)
        return 0;
    if (info->m_user->getCurCharacR() == 0)
        return 0;

    if (badgeType == 0) {
        // ORIG `mov 0x10(%ebp),%edx` 传形参变量本身（非 $0 立即数）。
        if (info->m_user->GetDailyBadge(badgeType) > 9)
            return 0;
    } else {
        if (info->m_user->GetDailyBadge(static_cast<ENUM_BADGE_TYPE>(1)) +
            info->m_user->GetDailyBadge(static_cast<ENUM_BADGE_TYPE>(2)) > 9)
            return 0;
    }

    info->m_user->AddDailyBadge(badgeType);
    *msgFlag = 1;
    // ORIG：count 初 0，仅 extra 加 1（不读 *msgFlag）。
    int count = 0;
    if (extra)
        ++count;

    char textBuf[0x100] = { '\0' };   // P4：ORIG rep stos 内联清零
    // [b28d-solo-CLeagueMatch] 真语义缺口修正：ENUM_BADGE_TYPE 仅定义值 0，
    // `switch(badgeType)` 在 4.4 被 -fstrict-enums 判值域 {0} 而**删除 case 2**
    // （.o 仅 2×findString/snprintf，ORIG 3×；P167 差集 MISSING 各 1）。
    // 改 `switch((int)badgeType)`（P305 的 S18 规避形）：值域未知 ⇒ 三分支
    // 全保留，且 4.4 产 ORIG 0x0855e90f/914/919 的「形参载入 %eax + 寄存器
    // 比较链」`mov 0x10(%ebp),%eax; cmp $1;je / cmp $2;je / test;je / jmp`
    // （P251/P286；if 链恒折 `cmpl $K,mem` 每段重读内存，探针 /tmp/b28d_cmatch/
    // probe4.cpp sw3 vs if3 实证）。
    switch (static_cast<int>(badgeType)) {
    case 1:
        OS_API::snprintf(textBuf, 0xff,
                         (const char*)g_scriptStringManager_.findString(
                             4, "school_red_badge_msg", (bool*)0),
                         (int)*msgFlag + count);
        break;
    case 2:
        OS_API::snprintf(textBuf, 0xff,
                         (const char*)g_scriptStringManager_.findString(
                             4, "school_blue_badge_msg", (bool*)0),
                         (int)*msgFlag + count);
        break;
    case 0:
        OS_API::snprintf(textBuf, 0xff,
                         (const char*)g_scriptStringManager_.findString(
                             4, "school_green_badge_msg", (bool*)0),
                         (int)*msgFlag + count);
        break;
    }

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
}
