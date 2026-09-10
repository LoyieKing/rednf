// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CAssaultMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ pvp_assault::CAssaultMgr 实现（批3：从 GameStubs.cpp 迁移）。
// 语义依据 ORIG：OnMoveArea 0x82ef610 / SendPacket 0x82efb54 /
// OnLeaveAssaultPlace 0x82ef176 / _GetAssaultPlace 0x82ed9fe。
// b28d-solo-CAssaultMgr（2026-09-09，泳道 CAssaultMgr.cpp 9 件）判词：
//   · LeaveAssaultPlace(Eibb)：**DIFF → IDENTICAL_AE**（leave 包作用域延长消
//     独立 EH cleanup + 收尾链去冗余外层 `if(user!=0)` 真语义修正）。
//   · CloseAssault：PacketGuard 改独立块（析构点对齐 082e9ec0）；残差 N22×1 +
//     N121 尾岛×2 + 重定位。
//   · CheckPoint：P167 归位（`(*it).m_*` 代 `it->m_*`，deEv 代 ptEv；missing 61→60）。
//   · MakeUserState ×2：N7 单条 %eax↔%edx 角色互换（P248 三档旋钮穷举后封顶）。
//   · CStaticDisposition::C1：N4/P30/N102 族（m_x 元素访问 + m_y 数组引用视图的
//     基址载入/add-vs-lea 残差；八形探针无更优）。
//   · AbuseUser / GetCharacName / ChangeHp：清单 NEAR → 路由口径 AE（复验不变）。
//   · p167：9/9 符号 CLEAR（CStaticDisposition 的 CDisposition C1/C2 同址别名
//     属 P93 良性；CheckPoint 已由 ptEv 归位 deEv）。
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
#include "Packet_Broadcast_Msg.h" // _BroadcastRunaway 公告包
#include "CServerProxyMgr.h"      // CMonitorServerProxy（_BroadcastRunaway）
#include "CMonitorServerProxy.h"  // _BroadcastRunaway 完整类型

// 全局依赖前置（3 参版定义见本文件尾；6 参版 owner=CVillageMonsterMgr.cpp）
#include "CAssaultMgr/IsIncludeVirtualMoveAreas.inc"

namespace pvp_assault
{

#include "CAssaultMgr/GetDirectionByTeam.inc"
}

CAssaultMgr* GetInstanceAssaultMgr()
{
    // ORIG _ZN11pvp_assault21GetInstanceAssaultMgrEv：guard 单例。
    static CAssaultMgr s;
    return &s;
}

#include "CAssaultMgr/_GetAssaultPlace.inc"
}

#include "CAssaultMgr/OnMoveArea.inc"
}

#include "CAssaultMgr/SendPacket.inc"
}

#include "CAssaultMgr/OnLeaveAssaultPlace.inc"
}

#include "CAssaultMgr/SetAssaultChannel.inc"
}

#include "CAssaultMgr/GetTeam.inc"
}

// ---- CAssaulter 访问器（ORIG 布局：+0x00 user / +0x04 team / +0x08 state）----

// b28d-solo-CAssaultMgr: P210 三分律②——外联定义置于 CAssaultPlace::~CAssaultPlace
// 之前，使 4.4 对其数组析构循环精确继承 ORIG 的 sub $0x4 帧（原头内 inline 为①，
// 保守撑帧 sub $0x14 ⇒ D1 NEAR 2）。
#include "CAssaultMgr/dtor_CAssaulter.inc"
}

#include "CAssaultMgr/GetUser.inc"
}

// ORIG W 0x82f055c：call IsAttachAble（0x82f054c）。
#include "CAssaultMgr/Empty.inc"
}

#include "CAssaultMgr/GetTeam_1.inc"
}

#include "CAssaultMgr/GetUserUniqueId.inc"
}

#include "CAssaultMgr/SetState.inc"
}

#include "CAssaultMgr/GetState.inc"
}

// ---- CAssaultPlace 内部前置函数 ----

#include "CAssaultMgr/_GetTeamCount.inc"
}

#include "CAssaultMgr/_CheckPlayable.inc"
}

#include "CAssaultMgr/CheckPlayable.inc"
}

#include "CAssaultMgr/GetState_1.inc"
}

#include "CAssaultMgr/IsArea.inc"
}

#include "CAssaultMgr/_GetUserIndex.inc"
}

#include "CAssaultMgr/ExistUser.inc"
}

#include "CAssaultMgr/SendPacket_1.inc"
}

#include "CAssaultMgr/SendPacket_2.inc"
}

#include "CAssaultMgr/MakeViewAssaultIconOnePlayer.inc"
}

#include "CAssaultMgr/LeaveAssaultPlace.inc"
}

// ============================================================================
// CAssaulter 方法（ORIG 权威；LeaveAssaultPlace 链依赖）
// ============================================================================

// ORIG T 0x82e6688：return *(int*)(this+0x10)。
#include "CAssaultMgr/GetHp.inc"
}

// ORIG T 0x82e6812：卸下 creature（TurnStomach false）+ 清 user/team 槽位。
#include "CAssaultMgr/DetachUser.inc"
}

// ORIG T 0x82e6868：放弃惩罚 —— 备份耐力 → 记 GiveupPvP 日志 → 设 ghost 惩罚时间。
// 收敛口径（batch-14 EmoC11）：o=142 n=143（原 -18）。已修语义两处：第一段
// 条件真值 `!pw && vill!=7`→(7)/else(3)（旧写 `pw||vill==7`→7 为分支反了）+
// 两段各自独立 IsPowerWarEventOn call、两分支独立 GiveupPvP（P48/P133）。
// N96 清绑：两处 bool 绑定删除后，裸合并短路条件在 4.4 直产 ORIG 的
// $1/$0 物化+test 汇聚岛（探针 /tmp/x1.cpp 同族）；全展开 m_user->
// 直读（P183）。残 1 条 = ORIG push esi/ebx 寄存器接力 vs 4.4 栈槽 + .o
// 全局地址/槽号显示（AE 后核销）。
#include "CAssaultMgr/GiveupPanalty.inc"
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
#include "CAssaultMgr/_IsGiveupPanalty.inc"
}

// ORIG T 0x82eab1a：_IsBroadcastRunaway —— PowerWar 且村庄==7 直接 false；
//   否则 getCurCharacR()!=0 决定返回值（0x82eab50 `mov $0` 在 t 为 true 分支）。
#include "CAssaultMgr/_IsBroadcastRunaway.inc"
}

// ORIG T 0x82eab7a：_BroadcastRunaway(CUser*, int) —— 经 monitor server
// 广播逃跑公告（ServerParameter 的 notice 模板 + 双方角色名）。
#include "CAssaultMgr/_BroadcastRunaway.inc"
}

// ORIG T 0x82e9046：_MakeLeaveUser —— 包 0x87 + 玩家 unique_id。
#include "CAssaultMgr/_MakeLeaveUser.inc"
}

// ORIG T 0x82e88d6：_MakeCloseAssault —— 包 0x89 + tick。
#include "CAssaultMgr/_MakeCloseAssault.inc"
}

// ORIG T 0x82e8d60：_MakeUserState(PacketGuard*, CUser*) —— 包 0x03 + state。
#include "CAssaultMgr/_MakeUserState.inc"
}

// ORIG T 0x82e83c4：SendUserState(CUser*) —— 组 0x03 状态包并全服广播
// （报告 pvp_assault__CAssaultPlace/SendUserState.md；ORIG 调用点仅压 CUser*，
//   头文件声明为静态形态）。
#include "CAssaultMgr/SendUserState.inc"
}

// ORIG T 0x82e7b2a：_SendCloseAssault(CUser*, int) —— tick 下限 0x1e，发关闭包。
#include "CAssaultMgr/_SendCloseAssault.inc"
}

// ============================================================================
// LeaveAssaultPlace(int,bool,bool) —— ORIG 0x082ead6a 逐段转写。
// 结构：索引/用户校验 → GiveupPanalty 判定（m_state 区间分支）→ 逃跑广播 →
// leave 包 + PowerWar 计数 + close 包 → _DelUser/用户状态包 → PowerWar 特判
// （村庄==7 时 guild 点数/连击广播）→ 双方 SetAssaultState(6)+EndAssault。
// 收敛口径（2026-09-05 EmoC11）：o=453 n=455。已消：G_GameWorld 具名中间槽
// （P52/P134 内联实参 → %eax 直通 ×2）、Empty() 正向判定（P22/P59 去 xor 物化）。
// P167 调用点差集 ONLY-ORIG=0（ONLY-OURS 仅 1×_Unwind_Resume：4.4 对第二个
// PacketGuard 块独立 EH 岛 vs 4.1 merge——N 表跨版本「EH 双份 dtor epilogue vs
// merge」族）。其余残余：帧 0x80 vs 0x70 + 全槽平移（N82/P112）、尾多 1 条
// `jmp <epilogue>` 岛边（跨版本 nop;jmp 岛/jump-threading 族）、`add $0x70` vs
// `sub $0xffffff80` 文本形差。语义完成、byte 层 N 封顶。
// b28d-solo-CAssaultMgr（2026-09-09）: **DIFF → IDENTICAL_AE**（TU diff 6→5、
// ae 225→226）。两处结构修正使 ORIG 的 PacketGuard/EH 布局与 p167 同时归位：
// ①leave 包（-0x24）作用域由独立块延到 082eb06f——ORIG 的 D1 在收尾链 +0x310
//   之前、ghost 块 +0x2e8 之后 ⇒ 消掉我方独立 EH cleanup（EXTRA _Unwind_Resume）；
// ②收尾链去掉冗余外层 `if(user!=0)`（裸作用域仅维持缩进）——ORIG +0x310 短路链
//   自含 user 项，user==0 时仍落到 +0x3d0 的 m_field100 检查（旧形真语义差异）。
// 修正后 clean 口径 orig 453 = ours 453、p167 calls 63=63 CLEAR。
// 本轮两处**语义/结构修正**（原形为真语义反向 + 缺条件）：
// ①82eb07a 收尾链 = `user != 0 && (!IsPowerWarEventOn() || vill != 7)` 嵌套
//   `m_state <= 3 && index∈{0,1}`（旧德摩根形缺 user 项且把 m_state 并入外层链）；
// ②82eb1b0 段 `call _CheckPlayable; xor$1; test; je <epilogue>` 是 4.1 对
//   **`if (!_CheckPlayable()) { 剩余全部 }`** 的形（旧写 `if (!_CheckPlayable())
//   return;` 早返 + 后续平铺，条件方向与块边界均与 ORIG 不符，且 4.4 产 jne 反跳）。
// ndiff 126→103（语义修正后）。
// ============================================================================
#include "CAssaultMgr/LeaveAssaultPlace_1.inc"
}

// ============================================================
// 批 A（OnlyOrig_Assault 恢复落盘；跳过 GetDistance(Point2d,Point2d) 与
// 6 参 IsIncludeVirtualMoveAreas——唯一 owner 归 CVillageMonsterMgr.cpp）
// ============================================================

// [SweepCAssaultMgr 真型修正] ORIG 本 TU 的 `_Z41__static_initialization_and_
// destruction_0ii`（GI 0x82f027c 调 0x82f023c）**只**含 ios_base::Init +
// __cxa_atexit，**无任何 Point2d::C1Eii 调用** ⇒ ORIG 的这张表是 POD 静态初始化
//（.rodata 0x8c1cce0，无动态 ctor）。旧形 `Point2d[8]`（用户 ctor）使 4.4 发 9
// 次 Point2d::C1Eii 动态初始化（我方 static_init 65 insn vs ORIG 41）。ORIG 的
// 访问形 `mov TABLE(,%eax,8)`（第一处，idx*2 折 scale 8）与 `add %eax,%eax;
// add $0x1,%eax; mov TABLE(,%eax,4)`（第二处，显式 idx*2+1）同为 int 数组证据。
static const int ASSAULT_RECT_PLAYER_POINT[16] = {
    0x000, 0x0f, 0x038, 0x05, 0x054, 0x00, 0x01c, 0x0a,
    0x104, 0x0f, 0x0cc, 0x05, 0x0b0, 0x00, 0x0e8, 0x0a
};                                   // .rodata 0x8c1cce0 实测内容
static const int ASSAULT_RECT_ICON_POINT[2] = { 0x82, 5 };   // .rodata 0x8c1cd20

int Point2d::operator-(const Point2d& o) const
{
    // ORIG 0x82f0486：|dx| + |dy|，abs 为无分支 `sar $0x1f; xor; sub` 三件套
    // ⇒ __builtin_abs 复现（实测 4.4.7 -O0 同族展开；if(x<0)x=-x 在 4.4 恒产
    // cmpl/jns/negl 分支形，多 3 条）。ORIG 全寄存器接力零槽 vs 4.4 store/load
    // 往返属 N82 帧槽族，收敛到指令序同构即止。
    return __builtin_abs(m_x - o.m_x) + __builtin_abs(m_y - o.m_y);
}

#include "CAssaultMgr/Set.inc"
}

// ORIG W 0x82f060a（size 0xa）：mov 0x0(%eax),%eax; ret —— 外联定义（P119）
#include "CAssaultMgr/Get.inc"
}

#include "CAssaultMgr/CandidatePoint.inc"
#include "CAssaultMgr/CExtraMatrix.inc"

#include "CAssaultMgr/AddPoint.inc"
}

#include "CAssaultMgr/MovePoint.inc"
}

#include "CAssaultMgr/CheckPoint.inc"
}

#include "CAssaultMgr/MoveUser.inc"
}

#include "CAssaultMgr/CCandidateMatrix.inc"
#include "CAssaultMgr/dtor_CCandidateMatrix.inc"

#include "CAssaultMgr/At.inc"
}

#include "CAssaultMgr/_AddCandidatePoint.inc"
}

#include "CAssaultMgr/MakeCandidatePoint.inc"
}

#include "CAssaultMgr/RemoveCandidatePoint.inc"
}

#include "CAssaultMgr/ArrangeCandidatePoint.inc"
}

// ============================================================
// 全局依赖（本 TU 唯一 owner 部分）
// ============================================================

} // namespace pvp_assault

// ORIG T _Z24GetMaxCandidateMatirxRowv 0x82e5c9e（全局函数，非 pvp_assault 成员）。
#include "CAssaultMgr/GetMaxCandidateMatirxRow.inc"
}

// ORIG T _ZN7MapArea7includeEii 0x82f0298：矩形包含判定。
#include "CAssaultMgr/include.inc"
}

// ORIG T _Z25IsIncludeVirtualMoveAreasPSt6vectorI7MapAreaSaIS0_EEii
// 0x82e5ed1：全部区域均包含 (x,y) 才返回 true。
#include "CAssaultMgr/IsIncludeVirtualMoveAreas_1.inc"
}

namespace pvp_assault
{

// ============================================================
// 批 B：CAssaulter（ORIG 逐指令转写）
#include "CAssaultMgr/AttachUser.inc"
}

#include "CAssaultMgr/IsAttachAble.inc"
}

#include "CAssaultMgr/_PrepareAssault.inc"
}

#include "CAssaultMgr/IsPowerWarWinner.inc"
}

#include "CAssaultMgr/SetPowerWarWinner.inc"
}

#include "CAssaultMgr/SetUserConnectP2P.inc"
}

#include "CAssaultMgr/Reset.inc"
}

#include "CAssaultMgr/DieUser.inc"
}

#include "CAssaultMgr/ChangeHp.inc"
}

#include "CAssaultMgr/GetCharacName.inc"
}


// ============================================================
// 批 A 续：Disposition / Factory / CAssaultPlace 访问器（恢复落盘）
// ============================================================

#include "CAssaultMgr/_MoveMatrix.inc"
}

#include "CAssaultMgr/GetFlagPoint.inc"
}

#include "CAssaultMgr/_CheckMatrix.inc"
}

#include "CAssaultMgr/SetVirtualMap.inc"
}

#include "CAssaultMgr/MoveUser_1.inc"
}

// [b23S3C] A：真修 counts[j*4]→counts[j]（原越界读）、P48 内层每次重读 counts[j]、
// icon 字段拆两句对齐 this 重载 store；残余 = N86(4.1 临时+拷贝 vs 4.4 直构形)
// + N87(ARP 索引 add/lea vs 折叠位移) + CDispositionC1vsC2 基构造（跨版本），
// fresh .o 复验 our=154/orig=158，调用点差集仅 C1/C2 同名差，语义等价。
// [round1-L29 2026-09-10 复核] 判据⑪ 槽位直方图逐值全等 ⇒ 非槽问题。唯一残差 = 索引两步寻址
//   `j*4+k`：ORIG `mov j,%edx; mov k,%eax; shl $2,%edx; lea (%edx,%eax,1),%eax` vs 4.4
//   `mov j,%eax; shl $2,%eax; add k_mem,%eax`（add-vs-lea，P30/N4/N102；八形探针全败）⇒ N。
//   ORIG 0x082e6386。
#include "CAssaultMgr/CStaticDisposition.inc"
}

#include "CAssaultMgr/dtor_CStaticDisposition.inc"

#include "CAssaultMgr/GetFlagPoint_1.inc"
}

#include "CAssaultMgr/SetVirtualMap_1.inc"
}

#include "CAssaultMgr/CDynamicDisposition.inc"
}

#include "CAssaultMgr/CAssaultPlaceFactory.inc"
#include "CAssaultMgr/dtor_CAssaultPlaceFactory.inc"

#include "CAssaultMgr/AcquireAssaultPlace.inc"
}

#include "CAssaultMgr/FreeAssaultPlace.inc"
}

CAssaultPlaceFactory* GetInstanceAssaultPlaceFactory()       // ORIG 0x82f0fa9
{
    static CAssaultPlaceFactory instance;                    // guard 0x943bcc0/0x943bce0
    return &instance;
}

#include "CAssaultMgr/GetAuthenValue.inc"
}

#include "CAssaultMgr/GetAssaultPlaceId.inc"
}

#include "CAssaultMgr/SetAssaultPlaceId.inc"
}

#include "CAssaultMgr/GetAssaulterCount.inc"
}

#include "CAssaultMgr/GetAnotherTeamCharacName.inc"
}

#include "CAssaultMgr/AbuseUser.inc"
}

// ============================================================================
// CAssaultPlace 构造 / 析构（ORIG W C1/C2 0x82f0614 size 0x6c、D1/D2 0x82f13b6
// size 0x31）。ORIG ctor：CAssaulter[8] 循环默认构造 → Point2d::C1 于 this+0x110
// （m_posBase）→ PartyInfo[2] 循环构造；其余字段留待 Reset()/Acquire 初始化。
// ORIG dtor：仅反向析构 assaulters[8]（Point2d/PartyInfo 析构平凡）。
// [SweepCAssaultMgr 迁移] C1 定义已迁至 W6_AssaultPlaceDepsA.cpp（Main 批准）。
//   依据（探针 /tmp/p_sp1..p_sp8，4.4.7 与 4.1.2 双编译器）：T 的默认 ctor 体在
//   StaticPool 实例化 TU 内**可见** ⇒ 编译器判 CNode ctor nothrow ⇒ 省掉
//   `new CNode[N]` 的异常回滚段；仅**声明**（定义在他 TU）⇒ 保守布 cleanup。
//   ORIG StaticPool<pvp_assault::CAssaultPlace,128>::C1 @0x82f1462（128 insn）
//   含完整 cleanup（0x82f14f9..0x82f153d：CNodeD1 循环 + _ZdaPv + _Unwind_Resume）；
//   本 ctor 定义在场时我方仅 103 insn（gap 25）。迁移后逐条对齐（P284）。
// ============================================================================

#include "CAssaultMgr/dtor_CAssaultPlace.inc"
}

// ORIG T 0x82e6d3a：m_pos.x/y 非零才向所在区域广播。
#include "CAssaultMgr/_SendPacketToArea.inc"
}

// ORIG T 0x82e6dce：同门槛；带用户列表的 4 参 send_to_area 版。
//   求值序（报告汇编 +0x25/+0x2e）：m_y 先入 esi、m_x 后入 ebx（右→左），
//   与 send_to_area(int vill, int area, ...) 形参序一致。
#include "CAssaultMgr/_SendPacketToArea_1.inc"
}

// ORIG T 0x82e8afc：包 0x86 图标列表 = put_header(0,0x86) + put_short(1) +
// MakeViewAssaultIconOnePlayer + finalize(true)。
#include "CAssaultMgr/_MakeViewAssaultIconPlayer.inc"
}

// ORIG T 0x82e79f6：_MakeUserState 后 G_GameWorld 全服广播。
#include "CAssaultMgr/_SendUserState.inc"
}

// ORIG T 0x82e8c5a：包 0x03 全场用户状态；先记录写位置，尾部回填人数。
// [round1-L29 2026-09-10 复核] 同 2 参版：1 处 N7 寄存器角色互换 + 尾部 epilogue 1 条
//   （ORIG 0x082e8c5a）。判据① CLEAR（16 vs 16 分支，目标无错位）。
#include "CAssaultMgr/_MakeUserState_1.inc"
}

// ORIG T 0x82e6e2c：移除指定槽位用户并联动村庄/断线检测子系统。
#include "CAssaultMgr/_DelUser.inc"
}

// ORIG T 0x82ea188 size 0x191：超时关场。trace 打印全场角色名；
// state 5/6 时两队下发 SetAssaultState(7)、置 state=7 并 CloseAssault()。
#include "CAssaultMgr/TimeoutCloseAssault.inc"
}

// ORIG T 0x82e6ab8（报告 Reset.md）：全场复位；归还池前调用。
#include "CAssaultMgr/Reset_1.inc"
}

// ORIG T 0x82edb6a：map 摘除 + place->Reset() + 归还 StaticPool。
#include "CAssaultMgr/FreeAssaultPlace_1.inc"
}

// ORIG T 0x82e9ad6 size 0x52d：关场主流程 —— 全员按 PowerWar/普通分支
// 发关闭包并回补体力，广播状态与图标， ghost 记录，定时两队重载，
// 清场后归还池。
// b28d-solo-CAssaultMgr: N 封顶（§6.2 三件套，S174 复验）— P167 calls 60 vs 60
// **CLEAR**；gap -15→-3（372 vs 375）、ndiff 593→35。本轮修复：
// ①4 个 bool 局部（powerWarVill/gainChaosKill/powerWar/reloadParty）内联为
//   短路条件（ORIG 无 bool 槽、我方 cmpb×4 全消）；②ghost 分支改正极性
//   `if (IsCurCharacGhost())`（ORIG 082e9bd6 正极性，消 xor$1）；
// ③P191 双初始化：`int i=0` 函数头声明、两循环复用（对齐 ORIG 082e9ade 双
//   初始化 + 两处 `movl $0,-0x28`）；④now/chaos/level/sps 内联（ORIG 无槽往返，
//   P0）；⑤stamina 内联（ORIG `movzbl %al,%edx` 直传）。
// 残差仅 N18b 比较操作数序（ORIG `cmp %edx,%eax`）、N121 尾 jmp 岛 ×2、
// N104 EH 块相对序（ORIG cleanup 居中 vs 我方居末），无语义缺口。
// b28d-solo-CAssaultMgr（2026-09-09）: N 封顶（§6.2 三件套；清单 DIFF 235）。
// 本轮修正：`PacketGuard packet` 由函数级作用域改**独立块**——ORIG 082e9ec0
// 在 _SendPacketToArea 后立即 `jmp` 跳过 landing pad 直落 D1 ⇒ 原码是独立块
//（旧形析构落函数尾，多尾岛 ×2 + EH 块位置整体偏移；归一化 ratio 0.449→0.572）。
// 修正后 clean 口径 orig 374 / ours 379（+5 = N22 比较段 1 条 + N121 尾
// `jmp+nop` 岛 ×2）；p167 调用点差集 CLEAR（ORIG 60 = OUR 60）；帧 `sub $0x40`
// 双方一致。残差族：N22/N18b（`call GetTeam; mov this,%edx; mov 0x124(%edx),%edx;
// cmp %edx,%eax` vs 4.4 `mov %eax,%edx; ...; cmp %eax,%edx`，字段/返回异型旋钮
// 需改冻结头，S2 归 Main）+ N121（循环尾与 FreeAssaultPlace 后 `jmp+nop` 岛，
// 三形实测不消）+ N123/重定位（`mov 0x0,%eax` 系 .o 未解析 R_386_32，P264）。
// 无语义缺口。
// [round1-L29 2026-09-10 复核] difflib 唯一差异 = 比较两操作数装载序（ORIG 0x082e9ad6：
//   `mov 0x8(%ebp),%edx; mov 0x124(%edx),%edx; cmp %edx,%eax` = call 结果留 %eax；
//   我方 `mov %eax,%edx; mov this,%eax; mov 0x124(%eax),%eax; cmp %eax,%edx`）⇒ N22/N18b 封顶。
//   p167：calls 60=60；MISSING/EXTRA 仅 PacketGuard C1/C2·D1/D2 同址别名（P93 良性）。
// [round5-A29 破 N · 类型旋钮净账留痕，2026-09-10] 上述 N22/N18b 判词**已作废**：按
//   P254/P248档③ 对 `CAssaultPlace::m_state`(+0x10c) 与 `m_winTeam`(+0x124) 做 **enum 同型化**
//   （本 TU 侧先落 7 处显式 cast：`m_state = (ENUM_ASSAULT_STATE){0,6,7}` ×6 +
//   `m_winTeam = (ENUM_ASSAULT_TEAM)0`）⇒ 4.4 恢复 ORIG 的两段装载
//   `mov 0x8(%ebp),%edx; mov 0x124(%edx),%edx; cmp %edx,%eax`，**本件 DIFF→AE 归零**。
//   **净账（Main「类型旋钮先算两侧 identical 增量」红线）**：CAssaultMgr TU
//   `identical 109→109（不降） / ae 292→293（+1） / near 1→1 / diff 6→5（−1）` ⇒
//   **净 identical ≥ 0 且总非 identical 下降**，合规保留。字段侧为 enum（非返回类型）
//   ⇒ 定义侧 store 无归一化插入（`(ENUM)7` 直存），符合「两侧同核」要求。
#include "CAssaultMgr/CloseAssault.inc"
}

} // namespace pvp_assault

// ORIG T 0x0863771a size 0x42：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE 1, partyIdx, TIMER_MESSAGE 0x2a,
//                               sec, key, 0)
// —— CloseAssault 后按队安排 CompleteLoadAfterAssault 超时重载。
#include "CAssaultMgr/registNextTimer.inc"
}
