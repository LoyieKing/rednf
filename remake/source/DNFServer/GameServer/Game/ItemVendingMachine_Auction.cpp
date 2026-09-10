// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ItemVendingMachine_Auction/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— ItemVendingMachine::BuyAuctionItem（ORIG 0x08175c5c，
// _ZN18ItemVendingMachine14BuyAuctionItemEP5CUserRNS_15AuctionNeedInfoEibi，
// range [0x08175c5c, 0x08176347]，源文件 localjapan/AppendVendingMachine.cpp）。
//
// 依据 docs/class_func_reports/ItemVendingMachine/BuyAuctionItem.md + df_game_r
// objdump 逐指令核对（报告汇编段存在错位解码，常量与分支以 objdump 原文为准）。
// 声明见 ItemVendingMachine.h；Taiwan 计费依赖见 TaiwanCash.h/.cpp。
// C++98 / -m32 / O0 基线。
//
// ORIG 流程概要：
//   1) 手续费基数：refValue==0 → premium?2:5；否则 rate=price/refValue：
//      <0.5 拒绝（return false）；<1.5 → premium?2:5；
//      1.5..1.8 分三档（[1.5,1.6) / [1.6,1.7) / [1.7,1.8)）→ 50；≥1.8 → 80。
//      ORIG 0x08175cd0..0x08175dc4 共 8 次 fucompp，常量序 0.5/1.5/1.5/1.6/1.6/1.7/1.7/1.8。
//   2) fee = (int)(基数 * price/100.0)，fldcw RC=0b11 向零截断后 fistpl；
//      写回 info.m_fee(+0xc)。
//   3) StreamPool::Acquire("localjapan/AppendVendingMachine.cpp",0x7b) +
//      CStreamGuard(stream,true).put_binary(&info,0x28)（审计流序列化）。
//   4) 第一次 purchase(uid, 99999, price-fee, acc, charac, trans1, 0x3c)：
//      失败 → BillingFail + LogManager::logFormat(行号 0xa5) → false。
//   5) 第二次 purchase(uid, 99999, fee, acc, charac, trans2, 0x3e)：
//      失败 → 同上行号 0xe8 → false；成功 → true。
//   每段成功均 cUserHistoryLog::BillingSuccess(type=1, trans.m_tranNo, 金额)。
//
// [b28d-SweepVending 2026-09-09] 本轮判词（gap 49→8，四数不回退 identical1/ae5/near1/
//   diff2，p167 CLEAR 仅 C1/C2·D1/D2 同址别名伪差 P93）：
//   真 bug 修复：①分档链漏第 4 层 —— 旧码 6 次比较，rate∈[1.7,1.8) 误发 0x50（应 0x32，
//     ORIG 0x08175d8d..0x08175dc4 第 8 次比较 1.8 实证）；②uidBuf 缺 `= {0}` 21 字节
//     预清零（ORIG 0x08175e5f..0x08175ebd 对齐感知展开）；③snprintf("%ld") 传完整
//     long long m_itemUid（ORIG 0x08175f13..0x08175f3a 压低+高双 dword，旧码 cast 成
//     long 少压一条）；④恢复具名局部集合（P0）：nRef(-0x44)/bPremium(-0x3d)/
//     nFeeNormal(-0x3c)/nFeePremium(-0x38)/feeBase(-0x34)/price(-0x30)/rate(-0x20)/
//     nPrice(-0x2c)/mgr(-0x28，含 ORIG 0x08175f3f 的 `movl $0` 前置清零两段形）；
//     ⑤fee 结果写回 feeBase 槽（ORIG `fistpl -0x34` 无独立 fee 局部）。
//   残余 gap=8 = N85 x87 比较 lowering（4.1 fucompp/fnstsw/test/sete vs 4.4 fucomip/
//     fstp/seta，8 次比较各 -1）。`-march=i486 -mtune=generic` 探针实测 461/462 且
//     x87 形与 ORIG 同（20 处 fucompp/fnstsw/sahf），但该旗标是 TU 级路由项，需 Main 裁决。
//   另残余（均跨版本、非语义）：m_itemUid 装载 4.4 多 2 条 mov（edx/eax 中转→ebx/esi）、
//     虚调用目标寄存器 4.4.7 用 %eax vs ORIG %edx（N7）、0x32/0x50 叶块物理序相反（N93附）。
// ============================================================================

#include "ItemVendingMachine.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "StreamPool.h"
#include "CUser.h"
#include "CInventory.h"  // OS_API::snprintf（ORIG 0x858c81c）
#include "CDataManager.h"  // G_CDataManager/FindGoods/getItemList（GetFinalProbability）
#include "CItemList.h"     // CCeraShopGoods 完整定义（_GetItemType/_GetInvenSlotInfo 等）
#include <cstdlib>  // atoi（ORIG atoi@plt）
#include <cstdlib>  // atoi（ORIG atoi@plt）
#include "CDataManager.h"  // G_CDataManager/FindGoods/getItemList（GetFinalProbability）
#include "CItemList.h"     // CCeraShopGoods 完整定义（_GetItemType/_GetInvenSlotInfo 等）
#include <cstdlib>  // atoi（ORIG atoi@plt）
#include <cstdlib>  // atoi（ORIG atoi@plt）

namespace
{

// .rodata 费率常量（df_game_r 实测字节）
const double kRateDeny   = 0.5;    // 0x08b871a8
const double kRate150    = 1.5;    // 0x08b871b0
const double kRate160    = 1.6;    // 0x08b871b8
const double kRate170    = 1.7;    // 0x08b871c0
const double kRate180    = 1.8;    // 0x08b871c8
const double kFeeDivisor = 100.0;  // 0x08b871d0

}  // namespace

#include "ItemVendingMachine_Auction/BuyAuctionItem.inc"
}

// ORIG 布局断言：AuctionNeedInfo 0x28（put_binary 长度；trans1.m_tranNo 落在
// +0x14、trans2.m_tranNo 落在 +0x20，与 BillingSuccess/Fail 读参偏移一致）。
typedef char (*AuctionNeedInfoSizeAssert)[sizeof(ItemVendingMachine::AuctionNeedInfo) == 0x28 ? 1 : -1];
AuctionNeedInfoSizeAssert g_auctionNeedInfoSizeAssert = 0;

// ============================================================================
// W8_LongTail 批次 5 符号（ItemVendingMachine.h 声明；ORIG objdump 逐指令核对，
// 报告 docs/class_func_reports/ItemVendingMachine/{_GetItemType,_GetInvenSlotInfo,
// _CheckEnableInventoryCondition,LoadPurcahseBonusScript,GetFinalProbability}.md）
// ============================================================================

// ---- _GetItemType —— ORIG 0x08176348 ----
// 函数局部 static Inven_Item temp（guard 惰性构造）；GetRelatedItem 取 CItem*
// 后经 vtable+0x08（ORIG vtable 槽 2 = make_item）拷入 temp，返回 temp.m_field1
// （+0x01，ITEM_TYPE）。temp 每次调用先 reset（0x80cb7d8）。
#include "ItemVendingMachine_Auction/_GetItemType.inc"
}

// ---- _GetInvenSlotInfo —— ORIG 0x081763be ----
// 稀疏 switch 决策树（P46：case 数值升序 0/2/3/7；叶块排放 = 书写序）。
// groupIdx: 0→AVATAR；2→EQUIP+_GetItemType；3→CREATURE+_GetItemType；
// 7→EQUIP+itemType=2；其余保持调用方初值，恒 return true。
// [b28c-L19] AE：store 序 LHS-first 靠 _GetItemType 返回类型改 Inven_Item::ITEM_TYPE
// （P211 同型化，ItemVendingMachine.h:125 同步；mangled 不含返回类型）。
#include "ItemVendingMachine_Auction/_GetInvenSlotInfo.inc"
}

// ---- _CheckEnableInventoryCondition —— ORIG 0x081768ba ----
// getCurCharacInvenR()->GetRemainCapacity(invenType,itemType) > 0，或物品
// get_index ∈ {200,0xcb,0xcc,0xcd,5,6,7} → true。ORIG 形态：条件真路径
// 统一 je/jg 到 mov $0，全假 fallthrough mov $1，再 test/je 双常反转
// （对应 if (!(C)) return false; return true; 逐字同构）。
#include "ItemVendingMachine_Auction/_CheckEnableInventoryCondition.inc"
}

// ---- LoadPurcahseBonusScript —— ORIG 0x08179eae ----
// 委托 m_bonus.LoadScript()；ORIG 形态为 call 后 xor $1、test/je 双常返回
// （if (!LoadScript()) return false; return true;）。
#include "ItemVendingMachine_Auction/LoadPurcahseBonusScript.inc"
}

// ---- GetFinalProbability —— ORIG 0x0854c470 ----
// FindGoods(goodsIdx) 视作 CCeraShopGoods* 取 GetBonusWeight()（float）；
// G_CDataManager()+0x0c 的 CItemList*（ORIG 内联读，非 getItemList W call）的
// GetVendingBonusRate()（W 0x0854c596，读 +0x4c）。fistpl 复用 rate 槽
// （ORIG fistpl -0x1c）。栈槽序依 ORIG：weight(-0x24)/rate(-0x1c)/goods(-0xc)，
// 4 个未用 dummy 撑出中间空隙（P31 帧差对齐）。+0x0c 读用 P1 组件视图折叠。
struct VendingItemListView { char m_pad[0xc]; CItemList* m_list; };  // CDataManager+0x0c 视图
// [b26-NL28] GetFinalProbability：删 4 个无 ORIG 证据的编造局部后帧 sub $0x38=ORIG；
//   残差=rate/weight 槽位平移（4.1 把 FPU cw 临时夹在具名槽之间，4.4 恒置槽区尾部，
//   N82 族源形不可消除），封顶。
// [b28d-26] NEAR 11 保持，N82 封顶（实测三形）：ORIG 0x0854c470 具名槽
//   goods(-0xc)/rate(-0x1c)/weight(-0x24) 且 cw 临时夹在 -0x1e/-0x20；4.4 恒
//   goods(-0xc)/rate(-0x10)/weight(-0x14)、cw 落 -0x1a/-0x1c（分配方向相反）。
//   试形①char padD[4]+padX[0xc] 声明穿插 → 仅帧 0x38→0x48、具名槽不动；
//   试形②double framePad;(void)&framePad（P190）→ 同样只撑帧不动槽基 ⇒ 封 N82。
#include "ItemVendingMachine_Auction/GetFinalProbability.inc"
}

