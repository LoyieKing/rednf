// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 AvatarRechargeC2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ============================================================================
// 1. AvatarRechargeServer::CalcDurabilityDetail —— ORIG T 0x081902e8
//
// 头像耐久掷点递减：取当前角色背包 0 号区槽位物品；空槽或耐久计数为 0 返 false。
// 否则取减免率：Gm 开关(+0x58)==1 用 GM 值(+0x54 低 16 位)，否则查徽章耐久表
// （CAvatarItemMgr::GetEmblemReduceRate，键=item m_addInfo2 即 avatar uid）；
// 率 >0 且 get_rand_int(1000) < 率 时耐久 -1、经 UpdateEndurance 上报 DB 保存流，
// 并把槽位号 push 进 rates。除空槽分支外恒返 true。
// ============================================================================
// [b28d-D43] N82/N89 大函数（90/92）（ORIG 0x081902e8）。
// [C05③ 2026-09-10 F3c] 三守卫岛族归零：96→**90 = ORIG 90（gap 0）**，TU 四数
//   48/1/2 → **49/1/1**（该件 DIFF→AE）。正解 = 正向嵌套 if（P223 定案①），
//   合并单条 `||` goto 实测净零（物化抵消）。
#include "AvatarRechargeC2/CalcDurabilityDetail.inc"
}

// ============================================================================
// 2. advancealtar::AdvanceAltarShopParameter::getFieldType —— ORIG T 0x088a27a2
//
// 字段名 → FieldType::T 编号；逐个 string::compare 命中即返回，未命中返回 -1。
// 名称顺序与编号严格按报告（level=0 … etc2=24）。
// ============================================================================
// [b28d-D43][W6] 判据⑫ 命中：ORIG 有具名 int ret（-0xc，首条 `movl $0xffffffff`
// + 命中点 `movl $N,-0xc` + 尾部 `mov -0xc(%ebp),%eax`）⇒ if/else-if 链 +
// 单出口 return（我方旧形 25 个 early-return 缺该槽，帧 0x10 vs ORIG 0x28）。
// ORIG 0x088a27a2。
#include "AvatarRechargeC2/getFieldType.inc"
}

// ============================================================================
// 3. advancealtar::AdvanceAltarShopParameter::getdataType —— ORIG T 0x088a2b8e
//
// 字段类型 → 数据形态：无符号比较 ft<8 且位集 0xc8（bit3/6/7 = name/explain/
// type explain，字符串型字段）命中返 0（字符串），其余返 1（整数）。
// ============================================================================
// [b28d-D43][W6] N6 位折叠攻形：内联 `1u<<ft` 被 4.4 折 `(0xc8>>t)&1`；
// 改裸 register（P231，非绑定，不占槽）承载移位基值，4.4 保 `shl %cl` 形（ORIG 0x088a2b8e）。
#include "AvatarRechargeC2/getdataType.inc"
}
