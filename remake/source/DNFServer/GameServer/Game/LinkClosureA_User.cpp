// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkClosureA_User/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [G5 2026-09-10 · L12 round2 组5 复核] + [2026-09-11 L2Free4to15.S4 复推] 5 件：
//   p167 全部 CLEAR 或仅 C1/C2·D1/D2 同址别名；enc_kr_scan 两 TU 0 处；串内容 0 处
//   （本 TU literals 全命中 ORIG）。逐件本轮结果：
//   · GetCurCharacInventoryRef（TT 非 const 版）：叶块书写序改 0,2,3,**1**,7 ——
//     非 const（0x08680f2e）与 const（0x08680c8a）的 ORIG 物理块序**不同**，
//     `movl $2/$3,0x4(%esp)` 两行互换 ⇒ 改后 ndiff 2→**0**，NEAR→AE（四数 65）。
//   · CalculateExpertJobMaterials（NEAR）：交换 ci/mi 声明序 ndiff 2→**14**（劣）⇒
//     二度回滚；残余 lea -0x10/-0x14 互换 = 后置 ++ 两处 sret 临时槽向（N86）封顶。
//   · compute_quest_basic_reward_exp：三处 P0 修正（单表达式连乘除 / 删 addRate 具名 /
//     尾部 `exp += …`）⇒ insns 153→**150**=ORIG、gap 0、ndiff 20。残余 = 4.1 寄存器
//     保活（`mov %eax,%ecx` 中转）+ growthView/sret 槽互换 + x87 临时槽深 0xc（均 N）。
//   · compute_quest_basic_reward_gold：P331 声明序（levelDelta/penalty/gold 裸声明提前
//     到 itemIdx 之前）⇒ 槽全对齐、ndiff 17→**6**、gap −1→0。残余 = 循环条件 4.4 恒
//     xor 物化（N122/N292）+ 循环头 jmp/nop 与尾块布局（4.1 独有）；试 `it != end`
//     得 ndiff 5 但 p167 由别名变 MISSING(eq)/EXTRA(ne) ⇒ 回滚（符号集口径优先）。
//   · getUserQuestRewardItem：p167 CALLSITE CLEAR；残余仅 ORIG 的 1 条 `jmp`（跳过
//     4.1 对齐 nop，P223 族）⇒ 4.1 布局产物封顶，AE 前仅差此行。

#include "CUser.h"
#include "Quest.h"
#include "CDataManager.h"      // G_CDataManager / m_questParameter(+0x8cc8)
#include "CInventory.h"
#include "CExpandEquipslot.h"
#include "CTitleBook.h"
#include "CUserCharacInfo.h"

#include "LinkClosureA_User/GetCurCharacInventoryRef.inc"


#include "LinkClosureA_User/GetCurCharacInventoryRef_1.inc"


#include "LinkClosureA_User/UpdateRating.inc"


#include "LinkClosureA_User/getUserQuestRewardItem.inc"


#include "LinkClosureA_User/getAddExpRateFromPremium.inc"


#include "LinkClosureA_User/compute_level_penalty.inc"


#include "LinkClosureA_User/compute_quest_basic_reward_exp.inc"


#include "LinkClosureA_User/compute_quest_basic_reward_gold.inc"


#include "LinkClosureA_User/quest_basic_reward.inc"


#include "LinkClosureA_User/CalculateExpertJobMaterials.inc"


// [B01 铁律1 / Main 批准] 原 `_forceLinkClosureAWeakSymbols`（static + used）为**发明符号**
// （ORIG `_ZL29_forceLinkClosureA…` 不存在），其目的（P147 强制发射 7 个弱模板实例）已由
// 其它 TU 自然发射覆盖（本机 nm 普查 841 个 .o）：`_ZNK…normal_iteratorIPi…4baseEv` 116 个、
// `…IPKi…C1ERKS2_` 82 个、`…C1IPiE`(enable_if 转换构造) 16 个、`pair 迭代器 ppEv` 16 个、
// `vector<int>::begin/end` 116/116、`vector<pair<int,int>>::begin` 98 个 ⇒ 删定义不丢符号。
