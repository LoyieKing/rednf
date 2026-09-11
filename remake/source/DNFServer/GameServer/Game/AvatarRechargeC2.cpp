#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AvatarRechargeC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ AvatarRechargeC2 —— 独占恢复批次（AvatarRechargeC2）。
// 恢复以下 ORIG 真实符号（df_game_r nm T 实证）：
//   _ZN20AvatarRechargeServer20CalcDurabilityDetailEP5CUseriRSt6vectorIiSaIiEE   @0x081902e8
//   _ZN12advancealtar25AdvanceAltarShopParameter12getFieldTypeESs                @0x088a27a2
//   _ZN12advancealtar25AdvanceAltarShopParameter11getdataTypeENS_9FieldType1TE   @0x088a2b8e
// 依据 docs/class_func_reports/AvatarRechargeServer/CalcDurabilityDetail.md 与
// docs/class_func_reports/advancealtar__AdvanceAltarShopParameter/
// {getFieldType,getdataType}.md 的 ## 汇编 + ## 反编译 C 全文转写。
//
// 注：AvatarRoulette::swap 已由 StrikerWorldDropB2.cpp 提供（ORIG T 0x08911e4e），
// 本 TU 不重复定义。

#include <string>
#include <vector>

#include "AvatarRechargeServer.h"  // AvatarRechargeServer 布局（m_enduranceReduceRate +0x54 /
                                   // m_bUseEnduranceReduceRate +0x58）
#include "CAvatarItemMgr.h"        // WongWork::CAvatarItemMgr::GetEmblemReduceRate/UpdateEndurance
#include "CDataManager.h"          // advancealtar::AdvanceAltarShopParameter / FieldType::T
#include "CInventory.h"            // CInventory::GetInvenRef / GetAvatarItemMgrW
#include "CUser.h"                 // CUserCharacInfo::getCurCharacInvenW
#include "GameRand.h"              // get_rand_int
#include "Inven_Item.h"

#include "AvatarRechargeC2/CalcDurabilityDetail.inc"


#include "AvatarRechargeC2/getFieldType.inc"


#include "AvatarRechargeC2/getdataType.inc"

