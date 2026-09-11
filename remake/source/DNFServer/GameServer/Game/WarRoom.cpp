// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WarRoom/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// df_game_r WarRoom（G2-3 PvP 链，战场房间，池化 40）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/WarRoom.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 跨类调用一律 include 权威头 + 真实类型调用（无 extern "C" asm 桥）。
// ---------------------------------------------------------------------------
// [b28d-solo-WarRoom 单文件泳道判词 · 2026-09-09]（证据均含 ORIG 地址/指令形）
//   identical_ae：SendWarRoomFailMsg 0x086c078e（GenTimerKey→%ebx/m_index→%esi，
//     旋钮=WarRoom.h GenTimerKey 返回类型同型化 int）、HostChangeWarRoom
//     0x086c08a6（lea 0x4(%edx),%ecx，旋钮=m_hostVotes 元素同型化 unsigned long）、
//     MakeStateInfo 0x086be07a / MakeSlotInfo 0x086bdea0 / MakeRoomInfo 0x086bdd58
//     （局部 char*+调用点 cast 复现值先物化 %edx；P222 极性翻正）、
//     ReviveUserByCoin 0x086bf324（地址归一后全等）。
//   near：SendGuildFP 0x086bc0b0（[SweepWarRoom] 本轮三循环岛形修复 271=271，
//     gap 0，DIFF→NEAR；残余标量槽序 N82）、EnterWarRoom（N82附 聚合优先低槽）。
//   [b28d-solo-WarRoom-3 · 2026-09-10] 判据①全 TU jcc/jmp 目标下标逐位比对 +
//   gap' 降序处理，本轮读数（orig→our，差=|gap|）：
//     OnEnterState   515→515（gap 0，原 528/-13）
//     ReviveUserByCoin 98→98（gap 0，P223 尾标签形复现 086bf3da nop 中继岛）
//     HandleItemDrop 776→777（gap -1，原 781/-5；P325 共享 case 块）
//     HandleGetItem  547→546（gap +1，原 551/-4；删 result 局部改早返回 +
//       count 具名化 + randomOption 内联短路）
//     HandleMonsterKill 445→442（gap 3，残余 4.4 lea/内存直加强度归约 N86/N1）
//     CalcGuildFP 165→168（gap -3，N82/N98：两数组首次使用序 + 6×movb vs
//       movl+movw 合并不可择）
//     ImportWarRoomWPTable 128→126（gap 2，P283 双入口岛 4.4 恒 jump-thread）
//     ClearReward 565→565 / SendGuildFP 271→271 / EnterWarRoom 279→279（gap 0，
//       残余版本槽基/寄存器 N82/N95/N126）
//   本轮修出的真 bug（判据⑩㊿/②/①）：
//     · OnEnterState 086bbb6a/086bbbcd 字段偏移：m_ec/m_f4 → m_f4/m_fc
//       （m_warRoomData+0xf4/+0xfc；原重建读错字段）；
//     · OnEnterState 086bbce7 `setle` 边界：`penalty < fp` → `fp <= penalty`
//       + else 直落（P194/P59 镜像；原写在 fp==penalty 时行为相反）；
//     · HandleGetItem 086bee7b..086bee9d：具名 bool randomOption 多占槽 ⇒
//       内联 `IsRandomOption() && !getCheckPickUpRandomOptionItem()`。
//   残余全部为跨版本结构性 N（已在函数头逐一记证），无未判定真缺口。
//   P0：全域无 register asm 绑定（唯一 register 为 P231 纯存储类提示）。
// ---------------------------------------------------------------------------
// [b28d-solo-WarRoom-4 · 2026-09-10] 四数 114/243/2/9 → **114/245/0/9**
//   （near 清零；ae +2）。本轮修为（判据⑩/⑪ + P231 + P223）：
//     · SendGuildFP 0x086bc0b0      NEAR → **AE（271=271）**：三标量（i1/minFp/count）
//       全头部声明并按 ORIG 槽深序（i1@-0x20/minFp@-0x1c/count@-0x18）落序；
//       `int minFp=…;/int count=0;` 的声明位置改独立赋值语句（保 ORIG 指令序）。
//     · EnterWarRoom 0x08201e7a      NEAR → **AE（279=279）**：声明序 packet, ret,
//       party, **seat**（seat 置队尾）+ 探针 /tmp/wr4/p.cpp f1..f5 五形对照；
//       `seat = 0;` 保留在 Join 前（086be6xx 形）。
//     · CalcGuildFP 0x086bbea6       168/-3 → **164/+1**：①两数组声明序交换
//       （local_40[6]={0} 先、local_4c[12] 后）= ORIG -0x48/-0x3c 槽序；②商
//       `local_20/local_1c` 复用 local_18（ORIG 086bc043/086bc075），消重复 idiv。
//     · ImportWarRoomWPTable 0x08902543  126/2 → **128/0**：①`register int again`
//       + `again != 0`（消 4.4 setne 物化，出 ORIG `test %ebx,%ebx`）；②do 体后与
//       else 各发 goto（LOOP_TAIL/ELSE_ISLAND 物理相邻）复现双落点。
//   p167 复验：本 TU 全部残余件 MISSING 仅 C1/C2、D1/D2 **同址别名镜像**（P93）
//   —— 脚本 /tmp/mirrorchk.sh 逐符号核 .o 的 U/D 符号，REALMISS=0 ⇒ 语义 CLEAR。
//   判据⑪ 槽直方图（objdump --disassemble 逐值对比）：
//     · OnEnterState：两侧 **11 槽逐值全等**（残余=E H 段序 N104 + 跳表 N4/N125）。
//     · HandleGetItem：整帧平移常数 **0xc**（我方 -0x40:46 ↔ ORIG -0x4c:46）⇒ N126。
//     · ClearReward：-0x50↓ 槽组平移 0xc（-0x54↔-0x50、-0xb4↔-0xa4/-0xa8…）⇒ N126+
//       N85（flds/fstps vs mov 直搬）、N76 store 序。
//     · HandleItemDrop / HandleMonsterKill / CalcGuildFP / WPTable：push Δ=0、
//       槽表近全等，残余 = 跳表 lowering(N4/N125)、算术强度归约(N86/N1，445/443)、
//       字节清零合并(N78/N202)、P223 单源尾岛。
//   结论：**本 TU 剩余真缺口 = 0**（9 件 DIFF 全为已记证 N 族；near 已清零）。
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#include "CGameManager.h"        // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CInventory.h"          // 权威 CInventory / Store / user_creature::CCreatureMgr / 共享枚举
#include "CDungeonEntranceLog.h"
#include "GlobalData.h"          // 权威 GlobalData::s_systemTime_ / s_event_manager
#include "CExpDoubleEvent.h"
#include "CBurningFatigueEvent.h"
#include "CServerEvent.h"
#include "DB_AccountCargoSync.h" // DB_InsertRandomOptionItemInform（静态 makeRequest）
#include "CUser.h"               // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CPrivateStoreMgr.h"    // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）
#include "CItem.h"
#include "CEquipItem.h"
#include "CDataManager.h"
#include "CDungeon.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CInventory.h"          // 权威 CInventory / Store / user_creature::CCreatureMgr / 共享枚举
#include "CDungeonEntranceLog.h"
#include "GlobalData.h"          // 权威 GlobalData::s_systemTime_ / s_event_manager
#include "CExpDoubleEvent.h"
#include "CBurningFatigueEvent.h"
#include "CServerEvent.h"
#include "DB_AccountCargoSync.h" // DB_InsertRandomOptionItemInform（静态 makeRequest）
#include "CGameManager.h"        // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CUser.h"               // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CPrivateStoreMgr.h"    // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）

#include "DB_AccountCargoSync.h" // DB_InsertRandomOptionItemInform（静态 makeRequest）
#include "CGameManager.h"        // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CUser.h"               // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CPrivateStoreMgr.h"    // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）
#include "CUser.h"           // get_state/SendCmdErrorPacket/CheckInParty/CheckInTrade/GetParty
#include "CGameManager.h"    // G_CGameManager/GetWarRoom/PutWarRoom/CheckOutParty
#include "CPrivateStoreMgr.h" // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt
#include "DisPatcher_MakeWarRoomTemp.h"  // 本批次实现（仅ORIG 符号落此 TU）

// ---- 本地枚举（与 CInventory.h 同 guard，避免同时包含时重定义） ----
#ifndef DNF_ENUM_INVEN_TYPE_DEFINED
#define DNF_ENUM_INVEN_TYPE_DEFINED
enum INVEN_TYPE
{
    INVEN_TYPE_INVEN = 0,
    INVEN_TYPE_EQUIP = 1,
    INVEN_TYPE_AVATAR = 2,
    INVEN_TYPE_CREATURE = 3,
    INVEN_TYPE_5 = 5,
    INVEN_TYPE_6 = 6
};
#endif

#ifndef DNF_ENUM_EITEMADDREASON_DEFINED
#define DNF_ENUM_EITEMADDREASON_DEFINED
enum eItemAddReason
{
    eItemAddReason_0 = 0,
    eItemAddReason_1 = 1,
    eItemAddReason_2 = 2,
    eItemAddReason_7 = 7,
    eItemAddReason_8 = 8
};
#endif

#ifndef DNF_ENUM_EMONEYADDREASON_DEFINED
#define DNF_ENUM_EMONEYADDREASON_DEFINED
enum eMoneyAddReason
{
    eMoneyAddReason_0 = 0,
    eMoneyAddReason_4 = 4,
    eMoneyAddReason_7 = 7,
    eMoneyAddReason_0x1a = 0x1a
};
#endif

#ifndef DNF_ENUM_EMONEYSUBREASON_DEFINED
#define DNF_ENUM_EMONEYSUBREASON_DEFINED
enum eMoneySubReason
{
    eMoneySubReason_0 = 0
};
#endif

#ifndef DNF_ENUM_EITEMDELREASON_DEFINED
#define DNF_ENUM_EITEMDELREASON_DEFINED
enum eItemDelReason
{
    eItemDelReason_0 = 0,
    eItemDelReason_3 = 3
};
#endif

#ifndef DNF_ENUM_EAVATARITEMADDREASON_DEFINED
#define DNF_ENUM_EAVATARITEMADDREASON_DEFINED
enum eAvatarItemAddReason
{
    eAvatarItemAddReason_0 = 0,
    eAvatarItemAddReason_0x14 = 0x14
};
#endif

// ---- 本地最小视图（权威声明见 CValueStatistic.cpp / CEventManager 头 /
//      HistoryLog.cpp / TimerQueue.h，因各自权威头与既有 include 冲突或尚未
//      建立，暂以本地声明 + 真实调用；方法定义在权威 TU，mangled 与 ORIG 一致）----
// 注：CEventManager/CEventBase 权威声明在 CGuildServerProxy.h（经 CExpDoubleEvent.h
//     引入），此处不再本地定义避免重定义冲突。
class HistoryLog
{
public:
    static void WriteUseCoin(_IO_FILE* file, const char* reason);  // _ZN10HistoryLog12WriteUseCoinEP8_IO_FILEPKc
};

extern CValueStatistic* GetInstanceValueStatistic();
TimerQueue* G_TimerQueue();  // 权威声明将入 TimerQueue.h（CleanPvPAsm 添加中）
char* NumberToString(unsigned int value, int radix);

// ============================================================================
// 依赖子对象（权威声明见 WarField.h，经 WarRoom.h→PvP_deps.h 引入；
//      构造/析构为真实符号）
// ============================================================================

#include "WarRoom/WarRoom.inc"


#include "WarRoom/dtor_WarRoom.inc"


// ============================================================================
// 基础访问器（ORIG weak：GetIndex 0x822ee8a / GetState 0x822ee96 /
//   SetState 0x82a5a56 / GenTimerKey 0x86c0b24 / CheckTimerKey 0x863bd92）
// ============================================================================
// [SweepWarRoom ㉓ 符号绑定] ORIG 全 W（头内 inline 弱副本），我方 T ⇒ 定义加
//   __attribute__((weak))（不改头，mangled/语义不变）。
__attribute__((weak)) int WarRoom::GetIndex()
{
    return m_index;
}

__attribute__((weak)) int WarRoom::GetState()
{
    return m_state;
}

__attribute__((weak)) bool WarRoom::CheckTimerKey(TIMER_MESSAGE msg, int key)
{
    return m_timerKey[msg - 0x3f] == key;
}

#include "WarRoom/CheckState.inc"


#include "WarRoom/ChangeState.inc"


#include "WarRoom/GetLiveCount.inc"


#include "WarRoom/GetRemainSeat.inc"


#include "WarRoom/GetUserSlot.inc"


#include "WarRoom/GetWaiterCount.inc"


#include "WarRoom/SetIndex.inc"


#include "WarRoom/CurSpawnStepReset.inc"


#include "WarRoom/Create.inc"


#include "WarRoom/AddBanList.inc"


#include "WarRoom/IsBanUser.inc"


#include "WarRoom/UpdateBanList.inc"


#include "WarRoom/BattleReset.inc"


#include "WarRoom/Reset.inc"


#include "WarRoom/InvalidUserCheck.inc"


#include "WarRoom/IsJoinable.inc"


#include "WarRoom/WalkOutUserBySlot.inc"


#include "WarRoom/WalkOutUser.inc"


#include "WarRoom/Destroy.inc"


#include "WarRoom/Open.inc"


#include "WarRoom/Close.inc"


#include "WarRoom/ShutDown.inc"


#include "WarRoom/Prepare.inc"


#include "WarRoom/Start.inc"


#include "WarRoom/ReviveAll.inc"


#include "WarRoom/RemoveKC.inc"


#include "WarRoom/CancelAllTimer.inc"


#include "WarRoom/SkipLastRankResultTimer.inc"


#include "WarRoom/SendWalkOut.inc"

#include "WarRoom/SendWarRoomFailMsg.inc"

#include "WarRoom/WarRoomFail.inc"


#include "WarRoom/HostChangeWarRoom.inc"

#include "WarRoom/CheckHostChange.inc"


#include "WarRoom/HandleTimerTimeBomb.inc"


#include "WarRoom/HandleTimerResultCount.inc"


#include "WarRoom/SendStatePacket.inc"


#include "WarRoom/MakeStateInfo.inc"


#include "WarRoom/MakeRoomInfo.inc"

#include "WarRoom/MakeSlotInfo.inc"


#include "WarRoom/SendToRoom.inc"


#include "WarRoom/OnLeaveState.inc"


#include "WarRoom/OnEnterState.inc"


#include "WarRoom/CheckFatuigue.inc"


// [b28d-solo-WarRoom-5] ORIG 086bbed0/086bbfaa 的 6 字节清零是 `movl $0`+`movw $0`
//   （宽写），且 local_4c[i+6] 的访问是**单条** `-0x42(%ebp,%eax,1)`（无 `add $6`）
//   ⇒ 该 12 字节区是「2×6 字节块」双视角（P235/P246/P311 成员数组视图）：
//   数组视角 m_b 供 i 索引，标量视角 m_z 供宽清零。sizeof/偏移与旧
//   `char local_4c[12]` 逐位相同（静态断言锁）。
namespace
{
struct WrBlock6
{
    union
    {
        char m_b[6];
        struct { int m_l; short m_s; } __attribute__((packed)) m_z;
    };
} __attribute__((packed));
}
static_assert(sizeof(WrBlock6) == 6, "WrBlock6 must stay 6 bytes");

#include "WarRoom/CalcGuildFP.inc"


#include "WarRoom/SendGuildFP.inc"


// [§6.2 核销 · b25] WarRoom::ClearReward（ORIG 0x086bc5fe，0x915B）：
//   P167 调用点差集 58/58 归零（0 MISSING / 0 EXTRA，新鲜 .o R_386_PC32 逐
//   符号 Counter 与 ORIG call 多重集全等）；ORIG insns 565 vs 新鲜 .o 566
//   （gap +1）。b25 回扫真修：补 avatarExp/creatureExp 两段 baseExp×rate
//   完整乘算赋值（ORIG 0x086bc398/0x086bc461 fildll+fmuls+fistpll），原重建
//   缺失该两段、仅剩 max(·,1) 比较 —— 奖励值语义真 bug 已修。残余
//   byte 层属 N82 帧槽平移 + N76/N18a store 装载步序 + GetRepeatEvent 链寄存器
//   物化；语义与 ORIG 一致（逐段注释见函数体）⇒ 判「语义完成、byte 层 N 封顶」。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18a/N76/N82）→ 试=槽集合比对：
//   ORIG 38 槽 / OURS 40 槽，逐槽配对后 maxRef[2] ORIG@-0x84/-0x88 vs OURS@-0x94/-0x98、
//   double 暂存组 ORIG@-0xa4/-0xa8/-0xaa vs OURS@-0xb4/-0xb8/-0xba（我方整体深 0x10）；
//   ORIG `lea -0x64(%ebp),%eax`（数组元素直 lea）vs 4.4 `lea -0x74; add $0x4`（下标式
//   lea+add）与 float 组/结构体槽互置 ⇒ 4.4/4.1 槽分配器差，无源码可择项 ⇒ 维持 N82。
__attribute__((target("arch=i486")))
#include "WarRoom/ClearReward.inc"


#include "WarRoom/Join.inc"


#include "WarRoom/SetCharacterLive.inc"


#include "WarRoom/SetSlotState.inc"


#include "WarRoom/HandleSetSeatState.inc"


#include "WarRoom/HandleDieCharacer.inc"


__attribute__((target("arch=i486")))
#include "WarRoom/HandleMonsterKill.inc"


#include "WarRoom/HandleWpPerMonster.inc"


#include "WarRoom/HandleGetItem.inc"


#include "WarRoom/HandleItemDrop.inc"


#include "WarRoom/ReviveUserByCoin.inc"

// ============================================================================
// 仅ORIG 符号（本批 OrigWarRoom 转写，报告已完整阅读：
// ============================================================================
// 仅ORIG 符号（本批 OrigWarRoom 转写，报告已完整阅读：
//   docs/class_func_reports/_lt_global_gt_/EnterWarRoom.md
//   docs/class_func_reports/_lt_global_gt_/ImportWarRoomWPTable.md
//   docs/class_func_reports/DisPatcher_MakeWarRoomTemp/dispatch_sig.md）
// ============================================================================

// ---- stBloodWarGrowTable（ImportWarRoomWPTable 形参类型，mangled P19stBloodWarGrowTable；
//      派生自 vector<vector<pair<int,int>>> —— GCC 4.4 对 typedef 到模板实例会展开
//      typedef 名，唯派生结构体才能产生 ORIG 的 P19stBloodWarGrowTable）----
struct stBloodWarGrowTable
    : public std::vector<std::vector<std::pair<int, int> > >
{
};

#include "WarRoom/EnterWarRoom.inc"


#include "WarRoom/ImportWarRoomWPTable.inc"


#include "WarRoom/dispatch_sig.inc"


__attribute__((weak)) int DisPatcher_MakeWarRoomTemp::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;    // ORIG 0x082335f8：恒定返回 0
}

__attribute__((weak)) int DisPatcher_MakeWarRoomTemp::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                                              ParamBase& /*param*/)
{
    return 0;    // ORIG 0x08233602：恒定返回 0
}

__attribute__((weak)) DisPatcher_MakeWarRoomTemp::~DisPatcher_MakeWarRoomTemp()
{
}
