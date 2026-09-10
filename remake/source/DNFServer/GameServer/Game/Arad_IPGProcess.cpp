// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Arad_IPGProcess/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— Arad_IPGProcess / Arad_MileageProcess 实现。
// 归属批次：LinkE2.JpnExImpl（ItemVendingMachine::ProcessIPGUserDataJpnEx 链
// 接收敛）。逐报告/逐指令转写，报告目录 docs/class_func_reports/
// {Arad_IPGProcess,Arad_MileageProcess}/。
//
// 布局实证（objdump 0x817a2ba/0x817a2ce）：
//   ctor 仅 *(this) = ItemVendingMachine*；D1/D2 空体；无 vtable（非虚）。
//   ORIG 各方法把 *(this)（= vm，其 +0x00 为内嵌 WongWork::CCeraShop m_ceraShop，
//   偏移 0 地址等价）按 CCeraShop* 传给 CCeraShop::_process* 家族
//   —— ORIG 原样类型双关，此处 reinterpret_cast 保持一致。
//
// Inven_Item 装载：ORIG (citem->vtable+0x08)(citem,&it) = CItem::make_item(it)
//   （CItem.h 虚表槽 2），随后 it.m_addInfo(+2)=itemIdx、it.m_addInfo2(+7)=stack。
// ============================================================================

#include "Arad_IPGProcess.h"
#include "Arad_MileageProcess.h"

#include "CDataManager.h"    // G_CDataManager / find_item / FindGoods / getItemLimitEditionMgr
#include "CUser.h"           // CUser / cUserHistoryLog（+0x79700）/ CeraShopPaymentType
#include "CInventory.h"      // AddAvatarItem / use_money / MakeItemPacket / Inven_Item
#include "CItem.h"           // isCerashopPackageItem / getUsablePeriod / getExpirationDate
#include "CItemList.h"       // CCeraShopGoods::GetSubGroupIndex / get_rand_int
#include <math.h>     // floor（GiveMileage）
#include <stdlib.h>   // atoi
#include "CMailBoxHelper.h"  // ReqDBSendNewAvatarMailCashShop
#include "RDARScriptStringManager.h"  // findString / g_scriptStringManager_
#include "LogManager.h"
#include "ItemVendingMachine.h"  // BillingRecord
#include "Arad_DataManager.h"    // ARAD::Singleton<Arad_DataManager> / Arad_Mileage_Data
#include "CUserCharacInfo.h"     // getCurCharacNo（经 CUser 继承可见）

#include <stdlib.h>   // atoi
#include <string.h>   // strlen

// ---------------------------------------------------------------------------
// Arad_IPGProcess
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CashShopModeAvatar —— ORIG T 0x0819c2b4（报告 CashShopModeAvatar.md 全文转写）。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] NEAR（助记符序列全等，仅 len 槽号差）：①gift 分支原已全等；
// ②删具名 inven/characNo 改 receiver 链式（P52/P0）后 slot=-0x20、msg=-0x1c 与 ORIG 一致；
// ③`int len` 头部声明得 -0x24，ORIG 为 -0x38（4.1 外层标量与形参 spill 区 -0x2c..-0x34
// 连续分配 vs 4.4 具名区 -0x1c..-0x24 分区分配）⇒ N82/N93 槽分配族封顶；
// size_t 化实测反劣（NEAR→DIFF）。
#include "Arad_IPGProcess/CashShopModeAvatar.inc"
}

// ORIG W 0x0817a2ba：push ebp; mov esp,ebp; mov 0xc(%ebp),%edx; mov %edx,(%eax); pop ebp; ret
#include "Arad_IPGProcess/Arad_IPGProcess.inc"
#include "Arad_IPGProcess/dtor_Arad_IPGProcess.inc"
#include "Arad_IPGProcess/CashShopModeCoin.inc"
}

// ---------------------------------------------------------------------------
// CashShopModeItem —— ORIG T 0x0819c4a0（报告 CashShopModeItem.md 全文转写）。
// ---------------------------------------------------------------------------
#include "Arad_IPGProcess/CashShopModeItem.inc"
}

// ---------------------------------------------------------------------------
// CashShopModeCreature —— ORIG T 0x0819c670（报告 CashShopModeCreature.md 转写）。
// 期限槽实证：esp0x50=08110c60 getUsablePeriod、esp0x54=080f1306 getExpirationDate。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] AE：删两组具名 usablePeriod/expirationDate 局部改实参内联
// （P0，ORIG %ebx 跨 call 保活、无槽），右起求值序自然复现（expirationDate 先 call）；
// 帧 0xc4、localItem -0x49、spill -0x6c/-0x70 与 ORIG 全等。
#include "Arad_IPGProcess/CashShopModeCreature.inc"
}

// ---------------------------------------------------------------------------
// CashShopModeVendingItem —— ORIG T 0x0819c862（报告 CashShopModeVendingItem.md）。
// 非特殊商品：insertItemIntoInventory 直插背包（失败转邮件补发）；
// 特殊商品：ProcessSpecialItem 通道。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] AE + 两处真语义 bug：①IsSpecialItem 第 1 实参按 ORIG
// 0x0819c8dd 从 localItem.m_addInfo(-0x58) 读（原写形参 itemIdx）；②日志条件极性反置
// （ORIG 0x0819cacc `test %al,%al; je end` ⇒ result!=0 才打日志，原写 !result）；
// ③删具名 ceraShop/result（P0）后槽位全对齐：localItem -0x5a / item -0x14 /
// outType -0x18 / outValue -0x1c / special -0x1d / msg -0xc / slot -0x10，
// strlen 结果 %ebx 跨 getCurCharacNo 保活。
#include "Arad_IPGProcess/CashShopModeVendingItem.inc"
}

// ---------------------------------------------------------------------------
// Arad_MileageProcess
// ---------------------------------------------------------------------------
// ORIG W 0x0817a2ce：同 Arad_IPGProcess ctor 形态。
#include "Arad_IPGProcess/Arad_MileageProcess.inc"
#include "Arad_IPGProcess/dtor_Arad_MileageProcess.inc"
#include "Arad_IPGProcess/BuySuccess.inc"
}

// ---------------------------------------------------------------------------
// GiveMileage —— ORIG T 0x0819dc9c（报告 GiveMileage.md 全文转写）。
// 汇编算术：fild rate; fild amount; flds [1000.0f @0x8ba7330]; fdivrp; fmulp;
//   fstpl; call floor; fistp（fldcw 0xc 截断模式）→ (int)floor(...)。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] IDENTICAL：三条钥匙——①`int rate=0` 前置清零 +
// `int found;` 两句式（-0x14/-0x10 槽序按声明序 rate→found 复现）；②Get 链全程右值
// 直传（P0，无具名 mgr 局部）；③ROUND 结果具名 `int mileage` 落 -0xc，x87 左操作数
// 写 `(float)amount * ((float)rate / 1000.0f)` 复现 fild amount→fild rate 序（P243）。
// 51=51 逐条全等（fresh .o 核验）。
#include "Arad_IPGProcess/GiveMileage.inc"
}

// ---------------------------------------------------------------------------
// BuyFail —— ORIG T 0x0819e50e（报告 BuyFail.md 全文转写）。
// ---------------------------------------------------------------------------
#include "Arad_IPGProcess/BuyFail.inc"
}

// ---------------------------------------------------------------------------
// BuyMileageItem —— ORIG T 0x0819dd4e（报告 BuyMileageItem.md 全文转写）。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] NEAR（318=318，助记符序列全等）+ 三处真语义 bug：
// ①BillingRecord 第 7 实参按 ORIG 0x0819e18d 是 `(int)stack`（原写 0）；
// ②`CItem* item = 0;` 先于 localItem ctor（ORIG 0x0819dd9d 先于 0x0819ddaa）；
// ③第一次 GetSubGroupIndex 判定内联（ORIG `cmp $1; setne; test; je`）、case 0 receiver
// 链式（P52）、characNo 实参内联（P0）、case 3 两 getter 内联（%ebx 跨 call 保活）。
// 残差：len 槽 ORIG -0x98（4.1 与形参 spill 区 -0x8c..-0x94 连续分配）vs 4.4 具名区
// -0x30，连带 localItem(-0x7d→-0x81)/items/outCodeA/outCodeB 四处平移 4 ⇒ N82/N93 封顶。
#include "Arad_IPGProcess/BuyMileageItem.inc"
}
