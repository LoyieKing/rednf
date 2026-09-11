// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemUpgrade/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::CItemUpgrade 实现。
// [2026-09-10 round5 复核（A25TailBatch3）] 本 TU 三件全部维持 N（p167 三件皆 CALLSITE CLEAR）：
//   · _CheckCondition 325/323：仅 2 处索引寻址形差 —— ORIG 08546603/0x08546635
//     `mov %eax,%edx; …; add $0x104,%edx; mov 0xc(%eax,%edx,4)`（两步）vs 4.4
//     `lea 0x104(%eax),%edx`（一条折叠）⇒ P30/N4 结构性伪影（P311 二维指针视角已实测反劣
//     323→329）；另 float 局部槽 -0x74/-0x70（ORIG）vs -0x14/-0x10 + Inven_Item 临时槽
//     ∓8 = N95/N126 帧族（判据⑪ MIXED）。
//     [C05② 复查 2026-09-10] 本件 cmp mem 仅 2 处（discountGroup `cmp -0x28(%ebp),%eax`、
//     predict `cmp -0xc(%ebp),%eax`），objdump 实测**均已是 mem 直比**，与 ORIG
//     `cmp -0x20(%ebp),%eax; setl` / `cmp -0xc(%ebp),%eax; jae` 同形 ⇒ **非**「两个具名
//     局部比较」命中形，未触发 int/unsigned 旋钮。边界留痕（Main 2026-09-10 定稿）：
//     `cmp $imm` 常量比较子型不适用（A01）；`test` 零比较子型可试（A12）；比较对象为
//     形参 / 跨调用返回值 / 该值被 setcc 物化 —— 均不适用（A07）。
//   · ProcUpgrade(MSG_ENCHANT) 437/437：p167 CLEAR 41/41；残余 = 帧 0x1ac/0x19c 与槽号平移
//     （N82/N95）+ 位域读 2 处多 `movzbl %al,%eax`（P168a③）+ cMyTrace 实参寄存器角色
//     `%ebx`↔`%eax`（N7）+ 数据地址伪差（N132）。
//   · _ProcPenalty 359/359：p167 CLEAR 54/54；残余 = 3 处 `lea` 与前邻指令的次序互换
//     （N7/N18a 装载序）+ 字符串地址伪差（N124/N132）。
// [C05 三条细化留痕 2026-09-10] 本 TU 三件均**未命中**：
//   ① C05③ 试前闸门（我方岛数 − ORIG 岛数 ≥ 2 才试合并）—— 三件各早退出口均为共享
//      epilogue、jcc 目标下标逐位一致、无 `jmp+nop` 岛残差 ⇒ 不满足闸门；
//   ② C05① 多层尾 fail 块 —— ORIG 无「两检查共享同一尾块」结构（失败出口一律
//      `mov $imm,%eax; jmp <共享 epilogue>`，与我方同形，故无需双层嵌套）；
//   ③ C07（字段读升格 callee-saved ⇒ 写具名 int 局部）—— 本 TU 无该族残件；其负例
//      边界（消费者为被调方按值实参时写具名局部反而落槽 +2，A14 实证）亦不适用。
//   ④ [C05④ 分诊 2026-09-10，按 Main「四步判定顺序」] `ProcUpgrade` 的两处 OUR-only
//      `movzbl`（我方 `movzbl -0x7a(%ebp),%eax; movzbl %al,%eax` 双段 vs ORIG 单段
//      `movzbl -0xba(%ebp),%eax`）逐条判：
//      ① 无消费者死比较？**否** —— ORIG 该段 `and $0x1,%eax; test %al,%al; je` 有真消费者；
//      ② 槽值/表达式物化 ⇒ 试 P225 原形？**不适用** —— 偏差方向是**我方多一条归一化**，
//        非「ORIG 有 setne 我方缺」（P225 治的是后者，D04 实证）；
//      ③ callee 返回类型与 ORIG 不一致？**否** —— 该 `movzbl` 与任何 call 返回无关，
//        两侧均从 1 字节槽读（ORIG -0xba / 我方 -0x7a 同族），属 4.4 前端 uchar 读冗余
//        归一化（P168a③）；
//      ⇒ 落第 4 档：**N-88 / P197④ 封顶**（勿动控制流、勿动 callee），与 A07 两例同族。
//        补充取证：ProcUpgrade 函数范围内**无**可见的多余 `(unsigned char)` cast
//        （内置 grep 已核），故该冗余归一化源自 4.4 前端对 1 字节槽值的使用点零扩展，
//        非源形可择；C05④ 四步中「body 终止性」（P225 命中条件）对本偏差亦不适用
//        （偏差方向为**我方多归一化**而非缺 setne）。
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CItemAmplifier.h"
#include "CStackableItem.h"
#include "CEquipItem.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager / CEventBase::GetRepeatEvent
#include "CEnvironment.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "DB_ReqItemUpgradeLog.h"
#include "PvPTypes.h"            // CMTRand（+0x4e8 randInt）
#include "GameTypes.h"           // cMyTrace
#include "DisJoint.h"            // DisJoint::GetResult/getPredictResultItemTypeCount
#include "CCubeStatistic.h"      // CCubeStatistic::collectCubeStatistics
#include "STStackableScript.h"   // STEnchantSystemMapKey/MapData
#include "PacketTypes.h"         // MSG_ENCHANT 完整类型
#include "CReduceUpgradeItemPay.h"   // CReduceUpgradeItemPay（GetReduceRate 权威声明）

#include <cstring>
#include <cstdio>
#include <ctime>     // ctor 的 time(0)（CMTRand 种子）

// 类声明见 CDataManager.h（WongWork::CItemUpgrade）。
namespace
{
// [b28d-solo-CItemUpgrade] ORIG getUpgradeItemRepairCostRate 0x08549170/0x0854918d
// 为 `add $0x9c,%edx; mov 0x4(%eax,%edx,4)`：基址是表指针本身、数组成员位于
// 视图 +0x04（P311 成员数组视图）。表本体 +0x04 落在 vector 内部指针上，无法
// 直接声明成员，故以只读视图类型表达该区布局（仅 reinterpret_cast 视图，
// 不新增符号、不改任何 sizeof/偏移）。
struct UpgradeRateTableView
{
    char m_pad04[4];                  // +0x00..+0x03
    float m_rate[(0x404 - 4) / 4];    // +0x04 起 4 字节步长（同 m_bodyView.m_f32 区）
};

}  // namespace

namespace WongWork
{
#include "CItemUpgrade/CItemUpgrade.inc"


#include "CItemUpgrade/dtor_CItemUpgrade.inc"


#include "CItemUpgrade/getUpgradeItemRepairCostRate.inc"


#include "CItemUpgrade/make3rdChroniclePacket.inc"


#include "CItemUpgrade/enableAmplifyRandomUpgrade.inc"


#include "CItemUpgrade/ProcAmplifyRandomUpgrade.inc"


#include "CItemUpgrade/_GetUpgradeInfo.inc"


#include "CItemUpgrade/_CheckCondition.inc"


#include "CItemUpgrade/_NoticeUpgrade.inc"


#include "CItemUpgrade/isDoEnchant.inc"


#include "CItemUpgrade/ProcUpgrade.inc"


#include "CItemUpgrade/_CheckCondition_1.inc"


#include "CItemUpgrade/ProcUpgrade_3rdChronicle.inc"


#include "CItemUpgrade/_ProcPenalty.inc"

}  // namespace WongWork
