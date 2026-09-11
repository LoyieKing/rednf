// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CItemAmplifier/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// df_game_r CItemAmplifier 还原（2026-08-16）。
// 构造/checkInvestableItem/generateAmplifyItem 依 ORIG 反汇编；
// stAmplifyItemScript_t / upgrade_table_t 构造为数据表占位（后续批次细化）。

#include "CItemAmplifier.h"

#include <algorithm>
#include <string.h>

#include "CItemList.h"

// ===================== 数据表占位构造（ORIG 0x82aa510 / 0x89000ca） =====================

#include "CItemAmplifier/stInvestAmplifyOption_t.inc"


#include "CItemAmplifier/stAmplifyItemScript_t.inc"


#include "CItemAmplifier/upgrade_table_t.inc"


// ===================== CItemAmplifier =====================

#include "CItemAmplifier/CItemAmplifier.inc"


#include "CItemAmplifier/checkInvestableItem.inc"


#include "CItemAmplifier/generateAmplifyItem.inc"


#include "CItemAmplifier/_generateOption.inc"



// ===================== 净化材料查询（ORIG W 0x08234d20 / 0x08234cd4 / 0x08234cfa） =====================
// 两行净化材料在 stAmplifyItemScript_t 中按 int[2] 行布局（ORIG 表查找即
// 类内 int 数组寻址 arr[type]，gcc 拆 idx+0x24 + disp8 0xc/0x4 形）：
//   type 0 = m_purifyMatItem[0](+0x9c)/m_purifyMatCount[0](+0xa4)（[purify material]）
//   type 1 = m_purifyMatItem[1](+0xa0)/m_purifyMatCount[1](+0xa8)（[purify only material]）

#include "CItemAmplifier/getPurifyMaterialType.inc"


#include "CItemAmplifier/getPurifyMaterialItemIdx.inc"


#include "CItemAmplifier/getPurifyMaterialItemCount.inc"


// ===================== 强化投资（ORIG 0x08234d5e / 0x08234c3c / 0x08234b30） =====================

#include "CItemAmplifier/getInvestMaterialInfo.inc"


#include "CItemAmplifier/enableInvestAmplifyOption.inc"


#include "CItemAmplifier/investAmplifyOption.inc"


// ============================================================================
// upgrade_table_t::get_upgrade_info @ 0x8900436
//   按 rarity/growthGrade/upgrade/equipType 计算 upgrade_info 到 out。
//   返回 0=越界/失败, 1=成功。
// ============================================================================

#include "CItemAmplifier/get_upgrade_info.inc"


#include "CItemAmplifier/GetPenaltyType.inc"

