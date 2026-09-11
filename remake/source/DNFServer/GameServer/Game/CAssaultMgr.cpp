// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAssaultMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// [PoolA 2026-09-11 fresh 复核 · 清单 6 件（routed 11:55，本 TU 四数 109/293/1/5）]：
//   · CAssaulter::GetCharacName（ORIG 0x082e663c）/ ChangeHp（0x082e6694）：**AE**
//     15/16、68/69（ndiff 0，帧差 1 为 outgoing 预留 N115）——清单 NEAR 系陈旧口径。
//   · _MakeUserState(PacketGuard*, CUser*) 0x082e8d60：**DIFF** 34/33 ndiff1，唯一
//     残差 = `p->put_byte(user->GetUserState())` 实参装载角色：ORIG 0x082e8db7 段
//     `call GetUserState; mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0x4(%esp);
//     mov %eax,(%esp)`（值先中转 edx、this 后载）；4.4 恒 `mov 0xc(%ebp),%edx;
//     mov %eax,0x4(%esp); mov %edx,(%esp)`（this 先载、值留 eax）⇒ N7/P211 档①。
//     本波复核：ORIG 同 TU SendUserState 0x082e845c 同表达式**无** hop（ORIG 自身
//     分裂两形）⇒ callee 返回类型旋钮（GetUserState int→unsigned/enum）无自洽解，
//     维持封顶（改 CUser.h 会连带把 SendUserState 打反）。
//   · _MakeUserState(PacketGuard*) 0x082e8c5a：**DIFF** 73/72 ndiff1，同型残差
//     （0x082e8d03 段）⇒ 同上 N7 判词。
//   · CStaticDisposition::C1（ORIG 0x082e62aa）：**DIFF** 158/158 ndiff2，残差 =
//     `(a*4+b)` 与 `arr[(a*4+b)*2]` 的 add-vs-lea / scale 折叠（ORIG 0x082e636d
//     `mov -0x1c(%ebp),%eax; shl $0x2,%edx; lea (%edx,%eax,1),%eax` /
//     `mov <A>(,%eax,8),%eax` vs 4.4
//     `add -0x1c(%ebp),%eax` / `add %eax,%eax; mov <A>(,%eax,4),%eax`）⇒ N4/P30/N102
//     （八形探针在册），维持。
//   · StaticPool<CAssaultPlace,128>::CNode D1 0x082f13e8：**NEAR** 8/8 ndiff1，
//     仅帧 `sub $0x18`(ORIG) vs `sub $0x4`(我们)＝N115/P210②（见下方 1319 行判词；
//     E02 加用户空 dtor 实验已证伪）⇒ 维持。
//   ⇒ 6 件与既有判词一致；本轮零代码改动（仅登记）。
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

#include "CAssaultMgr/IsIncludeVirtualMoveAreas.inc"

// [W13F 2026-09-11] 复现 ORIG 文件作用域常量符号 `_ZL14MAX_PVP_PLAYER`
//   （`nm ORIG`：r, 4B, 0x8c1c344 = 0x00000008；本 TU 的 SetUserConnectP2P 取址使用）。
//   必须在**全局作用域**声明：放命名空间内会得到 `_ZN11pvp_assaultL14MAX_PVP_PLAYERE`。
static const int MAX_PVP_PLAYER = 0x8;

namespace pvp_assault
{

#include "CAssaultMgr/GetDirectionByTeam.inc"


CAssaultMgr* GetInstanceAssaultMgr()
{
    // ORIG _ZN11pvp_assault21GetInstanceAssaultMgrEv：guard 单例。
    // [W13F 2026-09-11] 局部 static 名实证：ORIG guard = `_ZGVZN11pvp_assault21
    //   GetInstanceAssaultMgrEvE8instance` @0x94001a0（8B）/ 对象
    //   `_ZZN11pvp_assault21GetInstanceAssaultMgrEvE8instance` @0x9400b520（0x20B）；
    //   我方原名 `s` ⇒ 两条 ORIG-only 符号缺口 ⇒ 改名复现（纯符号名，零 codegen 影响）。
    //   遗留：ORIG 两符号为 W/V（原厂头内 inline 定义所致），我方为 T/b —— 见 W13F 报告待核。
    static CAssaultMgr instance;
    return &instance;
}

#include "CAssaultMgr/_GetAssaultPlace.inc"


#include "CAssaultMgr/OnMoveArea.inc"


#include "CAssaultMgr/SendPacket.inc"


#include "CAssaultMgr/OnLeaveAssaultPlace.inc"


#include "CAssaultMgr/SetAssaultChannel.inc"


#include "CAssaultMgr/GetTeam.inc"


// ---- CAssaulter 访问器（ORIG 布局：+0x00 user / +0x04 team / +0x08 state）----

#include "CAssaultMgr/dtor_CAssaulter.inc"


#include "CAssaultMgr/GetUser.inc"


#include "CAssaultMgr/Empty.inc"


#include "CAssaultMgr/GetTeam_1.inc"


#include "CAssaultMgr/GetUserUniqueId.inc"


#include "CAssaultMgr/SetState.inc"


#include "CAssaultMgr/GetState.inc"


// ---- CAssaultPlace 内部前置函数 ----

#include "CAssaultMgr/_GetTeamCount.inc"


#include "CAssaultMgr/_CheckPlayable.inc"


#include "CAssaultMgr/CheckPlayable.inc"


#include "CAssaultMgr/GetState_1.inc"


#include "CAssaultMgr/IsArea.inc"


#include "CAssaultMgr/_GetUserIndex.inc"


#include "CAssaultMgr/ExistUser.inc"


#include "CAssaultMgr/SendPacket_1.inc"


#include "CAssaultMgr/SendPacket_2.inc"


#include "CAssaultMgr/MakeViewAssaultIconOnePlayer.inc"


#include "CAssaultMgr/LeaveAssaultPlace.inc"


// ============================================================================
// CAssaulter 方法（ORIG 权威；LeaveAssaultPlace 链依赖）
// ============================================================================

#include "CAssaultMgr/GetHp.inc"


#include "CAssaultMgr/DetachUser.inc"


#include "CAssaultMgr/GiveupPanalty.inc"


// ============================================================================
// CAssaultPlace LeaveAssaultPlace 链私有 helper（ORIG 权威；自包含部分已还原，
// 其余依赖未还原子系统处标注，不造桩）。
// ============================================================================

#include "CAssaultMgr/_IsGiveupPanalty.inc"


#include "CAssaultMgr/_IsBroadcastRunaway.inc"


#include "CAssaultMgr/_BroadcastRunaway.inc"


#include "CAssaultMgr/_MakeLeaveUser.inc"


#include "CAssaultMgr/_MakeCloseAssault.inc"


#include "CAssaultMgr/_MakeUserState.inc"


#include "CAssaultMgr/SendUserState.inc"


#include "CAssaultMgr/_SendCloseAssault.inc"


#include "CAssaultMgr/LeaveAssaultPlace_1.inc"


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


#include "CAssaultMgr/Get.inc"


#include "CAssaultMgr/CandidatePoint.inc"
#include "CAssaultMgr/CExtraMatrix.inc"

#include "CAssaultMgr/AddPoint.inc"


#include "CAssaultMgr/MovePoint.inc"


#include "CAssaultMgr/CheckPoint.inc"


#include "CAssaultMgr/MoveUser.inc"


#include "CAssaultMgr/CCandidateMatrix.inc"
#include "CAssaultMgr/dtor_CCandidateMatrix.inc"

#include "CAssaultMgr/At.inc"


#include "CAssaultMgr/_AddCandidatePoint.inc"


#include "CAssaultMgr/MakeCandidatePoint.inc"


#include "CAssaultMgr/RemoveCandidatePoint.inc"


#include "CAssaultMgr/ArrangeCandidatePoint.inc"


// ============================================================
// 全局依赖（本 TU 唯一 owner 部分）
// ============================================================

} // namespace pvp_assault

#include "CAssaultMgr/GetMaxCandidateMatirxRow.inc"


#include "CAssaultMgr/include.inc"


#include "CAssaultMgr/IsIncludeVirtualMoveAreas_1.inc"
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

#include "CAssaultMgr/AttachUser.inc"


#include "CAssaultMgr/IsAttachAble.inc"


#include "CAssaultMgr/_PrepareAssault.inc"


#include "CAssaultMgr/IsPowerWarWinner.inc"


#include "CAssaultMgr/SetPowerWarWinner.inc"


#include "CAssaultMgr/SetUserConnectP2P.inc"


#include "CAssaultMgr/Reset.inc"


#include "CAssaultMgr/DieUser.inc"


#include "CAssaultMgr/ChangeHp.inc"


#include "CAssaultMgr/GetCharacName.inc"



// ============================================================
// 批 A 续：Disposition / Factory / CAssaultPlace 访问器（恢复落盘）
// ============================================================

#include "CAssaultMgr/_MoveMatrix.inc"


#include "CAssaultMgr/GetFlagPoint.inc"


#include "CAssaultMgr/_CheckMatrix.inc"


#include "CAssaultMgr/SetVirtualMap.inc"


#include "CAssaultMgr/MoveUser_1.inc"


#include "CAssaultMgr/CStaticDisposition.inc"


#include "CAssaultMgr/dtor_CStaticDisposition.inc"

#include "CAssaultMgr/GetFlagPoint_1.inc"


#include "CAssaultMgr/SetVirtualMap_1.inc"


#include "CAssaultMgr/CDynamicDisposition.inc"


#include "CAssaultMgr/CAssaultPlaceFactory.inc"
#include "CAssaultMgr/dtor_CAssaultPlaceFactory.inc"

#include "CAssaultMgr/AcquireAssaultPlace.inc"


#include "CAssaultMgr/FreeAssaultPlace.inc"


CAssaultPlaceFactory* GetInstanceAssaultPlaceFactory()       // ORIG 0x82f0fa9
{
    static CAssaultPlaceFactory instance;                    // guard 0x943bcc0/0x943bce0
    return &instance;
}

#include "CAssaultMgr/GetAuthenValue.inc"


#include "CAssaultMgr/GetAssaultPlaceId.inc"


#include "CAssaultMgr/SetAssaultPlaceId.inc"


#include "CAssaultMgr/GetAssaulterCount.inc"


#include "CAssaultMgr/GetAnotherTeamCharacName.inc"


#include "CAssaultMgr/AbuseUser.inc"


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


#include "CAssaultMgr/_SendPacketToArea.inc"


#include "CAssaultMgr/_SendPacketToArea_1.inc"


#include "CAssaultMgr/_MakeViewAssaultIconPlayer.inc"


#include "CAssaultMgr/_SendUserState.inc"


#include "CAssaultMgr/_MakeUserState_1.inc"


#include "CAssaultMgr/_DelUser.inc"


#include "CAssaultMgr/TimeoutCloseAssault.inc"


#include "CAssaultMgr/Reset_1.inc"


#include "CAssaultMgr/FreeAssaultPlace_1.inc"


#include "CAssaultMgr/CloseAssault.inc"


} // namespace pvp_assault

#include "CAssaultMgr/registNextTimer.inc"

