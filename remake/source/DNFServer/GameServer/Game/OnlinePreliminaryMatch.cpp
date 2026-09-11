// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlinePreliminaryMatch/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "OnlinePreliminaryMatch/reset.inc"


/* ORIG add T 0x855fdfe */
#include "OnlinePreliminaryMatch/add.inc"


/* ORIG del T 0x855fe54 */
#include "OnlinePreliminaryMatch/del.inc"


/* ORIG size const T 0x855feaa */
#include "OnlinePreliminaryMatch/size.inc"


/* ORIG refresh T 0x855fcf6：最后一个在场成员刷新 teamId/rating（单人模式走
 * charac_expand(4)=COnlinePreliminaryMember::GetRating W 内联 0x8568f02，
 * 组队模式走 COnlinePreliminaryTeam::GetRating 0x8568eee）。*/
#include "OnlinePreliminaryMatch/refresh.inc"


#include "OnlinePreliminaryMatch/calc.inc"

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
#include "OnlinePreliminaryMatch/COnlinePreliminary.inc"


/* ORIG D1 0x0855ffd6（D0 = 本体 + operator delete(this)，由编译器生成） */
#include "OnlinePreliminaryMatch/dtor_COnlinePreliminary.inc"


// ---------------------------------------------------------------------------
// 每队人数 / 匹配开关 / 地图请求
// ---------------------------------------------------------------------------

/* ORIG T 0x0855fee8：G_CDataManager()->GetPvPTeamCount()（T 0x08568ce0）。
 * GetPvPTeamCount 声明在 CDataManager.h（producer 归 CDataManager 域批次）。*/
#include "OnlinePreliminaryMatch/GetPlayCountPerOneTeam.inc"


/* ORIG T 0x085608d4：纯 ret（ORIG 即空实现） */
#include "OnlinePreliminaryMatch/SetPlayCountPerOneTeam.inc"


/* ORIG T 0x085608da */
#include "OnlinePreliminaryMatch/SetMatchAble.inc"


/* ORIG T 0x085608f2 */
#include "OnlinePreliminaryMatch/RequestMap.inc"


// ---------------------------------------------------------------------------
// 房间信息 / 建房 / 入退房
// ---------------------------------------------------------------------------

/* ORIG T 0x08560f74 */
#include "OnlinePreliminaryMatch/_BackupRoomInfo.inc"


/* ORIG T 0x08560e4a：返回座位号 */
#include "OnlinePreliminaryMatch/_PutInRoom.inc"


/* ORIG T 0x08560df2 */
#include "OnlinePreliminaryMatch/_PullOutRoom.inc"


/* ORIG T 0x08560f92 */
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE] 本波修：①真语义 bug
 * create_room 第 5 参 true→m_roomInfo.byte8（ORIG 读 this[0x10]）；②序列
 * 还原 put_header(1,0x35) 移至 GetPvp 前（ORIG @08560fbc 序）；③删
 * name/pass/titleLen 布局局部（帧 0x9c→0x6c）；④成功/失败块序翻正。
 * 196=196 全等，详见函数体内注释。 */
#include "OnlinePreliminaryMatch/_MakeRoom.inc"


/* ORIG T 0x85612a4：vtable+0（IRoomList::AddRoom）虚调。@085612b3
 * mov 0x8(%ebp),%edx→0x4(%esp)(@+0x17)：实参为 this（与 CFairMatch 同族
 * 0x08563a32 一致），room 形参未用；具名 list 中转致 4.4 多 -0xc 槽【技巧
 * 36】→ 直调 */
/* N7 封顶（ndiff 全为寄存器换名 eax/ecx/edx）：4.4 间接 call 目标恒 %eax、
 * 4.1 用 ecx——源码不可控（负例表 N7）。NL6 2026-09-08 */
#include "OnlinePreliminaryMatch/_RegisterWaitingRoom.inc"


// ---------------------------------------------------------------------------
// 校验族
// ---------------------------------------------------------------------------

/* ORIG T 0x08560082 前置的门禁函数 —— 见 _GetRating 家族段上方注释 */

/* ORIG T 0x085606e0（mangled 无 K → 非 const 成员） */
#include "OnlinePreliminaryMatch/_CheckSameTeam.inc"


/* ORIG T 0x08560736（mangled 无 K → 非 const 成员；第二参为 PvP_Room*，报告变量名 param_2 有误【推断修正】） */
#include "OnlinePreliminaryMatch/_CheckObserver.inc"


/* ORIG T 0x085612c6 const；返回 0 可匹配 / 0x16 失配 */
#include "OnlinePreliminaryMatch/_CheckMatchable.inc"


/* ORIG T 0x085607b2（switch 表 @0x8ca230c 实证） */
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE/P248档①] 旧 N96 判词作废：
 * _CheckSameTeam/_CheckObserver 返回类型 int→bool（OnlinePreliminary.h，
 * R2 已广播）+ 裸 `if(!call) return 4;` ⇒ 复现 ORIG @08560866/@0856088d
 * `call; xor $1,%eax; test %al,%al; je`，帧 0x14 与 85=85 全等（旧 uchar
 * 栈化三段 + 帧 0x24 一并消）。 */
#include "OnlinePreliminaryMatch/CheckJoinable.inc"


/* ORIG T 0x085608bc：cmpl $2 / jne else(@+0x10)；then 体 mov $0 fall-through
 * (@085608c5)，else 体 mov $1 (@085608cc) → !=2 判定块在前【技巧17】 */
#include "OnlinePreliminaryMatch/CheckSeat.inc"


/* ORIG T 0x085623ec；N7 封顶（虚调用寄存器换名，见 _RegisterWaitingRoom 注） */
#include "OnlinePreliminaryMatch/CheckMatchable.inc"


// ---------------------------------------------------------------------------
// rating 记录家族
// ---------------------------------------------------------------------------

#include "OnlinePreliminaryMatch/_GetRating.inc"


/* ORIG T 0x08560082：f 槽(-0xc)无 entry 初始化；else 路径 @085600e8
 * `mov $0,%eax` 单物化；尾 merge `mov %eax,-0xc; flds -0xc`(@085600ed) */
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE] 旧 DIFF/N85③ 判词作废。真因
 * 是**具名 float 局部 + return f** 使 4.4 另开 -0x1c 返回临时槽（帧 0x38）；
 * 改为**各分支直接 return**（无具名局部）后返回槽与 -0xc 合一，逐条复现
 * ORIG：各臂 `fstps -0xc; mov -0xc,%eax; jmp merge` + 尾
 * `mov %eax,-0xc; flds -0xc`，帧 0x28（探针 /tmp/s190/p_gr.cpp fB 全等；
 * fA/fC/fD 具名形恒多 -0x1c）。语义等价（返回值即各分支表达式）。 */
#include "OnlinePreliminaryMatch/_GetRating_1.inc"


/* ORIG T 0x08560368：首个在场成员同步个人 rating */
#include "OnlinePreliminaryMatch/_SetRating.inc"


/* ORIG T 0x085600f6 */
#include "OnlinePreliminaryMatch/_SetRating_1.inc"


/* ORIG T 0x08560be2：winOrLose=1 以 team0 为自身结算，否则以 team1 */
#include "OnlinePreliminaryMatch/_UpdateRating.inc"


#include "OnlinePreliminaryMatch/_IncreaseWinCount.inc"


/* ORIG T 0x085603fa：队内第一个在场成员计数 */
#include "OnlinePreliminaryMatch/_IncreaseWinCount_1.inc"


/* ORIG T 0x085601b8 */
#include "OnlinePreliminaryMatch/_IncreaseLoseCount.inc"


/* ORIG T 0x08560484 */
#include "OnlinePreliminaryMatch/_IncreaseLoseCount_1.inc"


// ---------------------------------------------------------------------------
// 双方交手次数家族
// ---------------------------------------------------------------------------

/* ORIG T 0x08560212 */
    /* [b28d-solo-OnlinePreliminaryMatch 判词·NEAR/N82附] 帧/槽/指令 51=51
     * 全等；残 4 条 = getCurCharacNo→%esi、GetServerGroup→%ebx（ORIG 反之，
     * 即 %ebx↔%esi 汇聚对调），callee-saved 角色分配源形不可控（N82附）。 */
#include "OnlinePreliminaryMatch/_GetEachOtherPlayCount.inc"


/* ORIG T 0x0856050e：取两队在场的最后一名成员查询 */
#include "OnlinePreliminaryMatch/_GetEachOtherPlayCount_1.inc"


/* ORIG T 0x085602ae */
    /* [b28d-solo-OnlinePreliminaryMatch 判词·NEAR/N82附] 49=49 全等；残 4
     * 条 = %ebx↔%esi 汇聚对调（同 _GetEachOtherPlayCount(CUser*,CUser*)）。 */
#include "OnlinePreliminaryMatch/_IncreaseEachOtherPlayCount.inc"


/* ORIG T 0x085605ee：双向累计 */
#include "OnlinePreliminaryMatch/_IncreaseEachOtherPlayCount_1.inc"


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
#include "OnlinePreliminaryMatch/CheckUnusableEquipment.inc"


/* ORIG T 0x08560a42：装备栏 0..22 槽全检 */
#include "OnlinePreliminaryMatch/CheckUnusableEquipmentInUser.inc"

// ---------------------------------------------------------------------------
// IMatch 虚槽家族
// ---------------------------------------------------------------------------

/* ORIG T 0x08560ae4 */
#include "OnlinePreliminaryMatch/OnCreateRoom.inc"


/* ORIG T 0x08560b8a；N7 封顶（虚调用寄存器换名，见 _RegisterWaitingRoom 注） */
#include "OnlinePreliminaryMatch/OnDestroyRoom.inc"


/* ORIG T 0x08560bba */
#include "OnlinePreliminaryMatch/OnJoinRoom.inc"


/* ORIG T 0x08560cd2：第 4 参为离场者队伍组值（OnWalkOut 以 get_team(seat) 传入）；
 * N7 封顶（虚调用寄存器换名，见 _RegisterWaitingRoom 注） */
#include "OnlinePreliminaryMatch/OnLeaveRoom.inc"


/* ORIG T 0x085613de。ORIG 返回 int：m_field04==1 → 1；失败路径 → 0；成功 → 1。 */
    // [b28 判词 N96+P168a④] X2 寄存器 xor/钉值物化形需 register 绑定；绑定 P0 全域禁用后无源形，N 封顶。

#include "OnlinePreliminaryMatch/OnRegisterMatch.inc"


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
#include "OnlinePreliminaryMatch/OnMatchRoom.inc"


/* ORIG T 0x08561a52 */
#include "OnlinePreliminaryMatch/OnStartFight.inc"


/* ORIG T 0x08562022 */
/* [b28d-solo-OnlinePreliminaryMatch 判词·AE] 旧 DIFF/N75(N=2) 判词作废。
 * 唯一残差曾是 ORIG @08562040-0x29「`mov -0xc(%ebp),%eax`（managerTeam）
 * 先于 `movl $0x0,0xc(%esp)`（第3参常量）」而 4.4 恒 R2L 先压常量。
 * 翻正旋钮=**实参来源局部改 unsigned int**（P248 档③ 变体）：实参表达式带
 * 隐式 int 转换后 4.4 判其需预求值寄存器，先发 `mov -0xc(%ebp),%eax` 再压
 * 常量，逐条复现 ORIG（探针 /tmp/s190/p_arg4.cpp v17：int 局部恒常量先、
 * unsigned 局部则值先；v12 enum 同效）。值域 0..2，`!=1` 为等值比较，
 * 语义/槽位/帧零变化。 */
#include "OnlinePreliminaryMatch/OnEndFight.inc"


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
#include "OnlinePreliminaryMatch/OnWalkOut.inc"


/* ORIG T 0x08561ff2 */
#include "OnlinePreliminaryMatch/OnReady.inc"


/* ORIG T 0x08562250：param(b)=胜方标记，param(a)=失败方队伍标记【推断命名】 */
#include "OnlinePreliminaryMatch/OnReward.inc"


/* ORIG T 0x08561a70：包(0,0x116){uid short} 双队广播 */
#include "OnlinePreliminaryMatch/OnCompleteLoad.inc"


/* ORIG T 0x08561dee */
#include "OnlinePreliminaryMatch/OnConnectP2PTimeout.inc"


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
#include "OnlinePreliminaryMatch/OnConnectP2P.inc"


/* ORIG T 0x08561e36：纯返 1 */
#include "OnlinePreliminaryMatch/OnGiveup.inc"


// ---------------------------------------------------------------------------
// IMatch 视图 / 辅助查询
// ---------------------------------------------------------------------------

/* ORIG T 0x08561a0e：fabs(self.team0.rating − other.team0.rating)，other 按
 * 本类静态寻址（非虚）实证。 */
#include "OnlinePreliminaryMatch/GetGap.inc"


/* ORIG T 0x085620c8：仅 team0 前四席视图，占用且 state==1 才有效 */
#include "OnlinePreliminaryMatch/GetUser.inc"


/* ORIG T 0x08562104（docs/class_func_reports/online_preliminary__COnlinePreliminary/
 * GetUserList.md）：将 this+0x1c..+0x40 即 m_teams[0]（TeamInfoSecond，0x28 字节）
 * 按 10 个 dword 展开逐字复制到出参。 */
#include "OnlinePreliminaryMatch/GetUserList.inc"
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
#include "OnlinePreliminaryMatch/GetTeamId.inc"


/* ORIG T 0x8561f66 */
#include "OnlinePreliminaryMatch/_GetOppositeTeam.inc"


/* ORIG T 0x08568866（README 漏列补充）：seat-state 1/2 → 队伍 0/1 */
#include "OnlinePreliminaryMatch/_GetTeam.inc"


/* ORIG T 0x0856214a：包(0,0xaf){byte a} 广播前 b 个队伍的在场成员 */
#include "OnlinePreliminaryMatch/_SendMatchStatePacket.inc"


} // namespace online_preliminary
