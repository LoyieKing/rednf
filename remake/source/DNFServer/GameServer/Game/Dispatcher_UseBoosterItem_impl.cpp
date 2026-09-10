// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Dispatcher_UseBoosterItem_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "Dispatchers.h"

// ============================================================================
// Dispatcher_UseBoosterItem 还原（ORIG）
//   dispatch_sig                 0x08208d40 T（762 insn）
//   _onBoosterItemUseResult      0x082099bc T
//   _onBoosterItemUseResult_OnlyOne 0x08209a5e T
//   sendBroadCastItems           0x08209b0e T
// 实现以 docs/class_func_reports/Dispatcher_UseBoosterItem/*.md 为准。
// 本类仅 dispatch_sig/process/read 在 Dispatchers.h 声明；_onBoosterItemUseResult*
// 与 sendBroadCastItems 未在头声明（原 Dispatchers.cpp 桩已删），按 Main 指示以此
// 文件内的文件级静态辅助函数复现（行为与 ORIG 一致；不新增导出符号）。
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "PacketBuf.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "Inven_Item.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"   // CEventManager / CEventBase（GetRepeatEvent / IsEventing）
#include "CEnvironment.h"       // GlobalInstance<Arad_GiftItem_Set>
#include "CPrivateStoreMgr.h"   // private_store::GetInstancePrivateStoreMgr / IsBusyPrivateStore
#include "CItemList.h"           // Arad_GiftItem_Set::level_check
#include "CCeraShop.h"           // WongWork::CCeraShop::ProcessSpecialItem / SaveFeaturedIdx

#include <vector>
#include <map>
#include <utility>
#include <string>

// GrowthEquipEvent 权威头（ORIG 0x080f74b2 族）：IsEventing W 0x080f7820 /
// SelectGrowthWeapon T 0x080f73be，实现见 GrowthEquipEvent.cpp。
#include "GrowthEquipEvent.h"

// ORIG 自由函数 LineFunc(int, const char*, int, unsigned int) @0x085908d7 T；
// 权威声明见 CompoundEmblemDeps.h，定义由 CompoundEmblemDeps.cpp 提供。
extern int LineFunc(int line, const char* func, int errNo, unsigned int mid);

#include "GameTypes.h"           // cMyTrace（ORIG 0x0854f718 C1 / 0x0854f788 operator()）

// ============================================================================
// 本类三个辅助方法（Dispatchers.h 已声明；行为忠实 ORIG）。
// ============================================================================

// sendBroadCastItems(CUser*, Inven_Item const&, int) —— ORIG 0x08209b0e
#include "Dispatcher_UseBoosterItem_impl/sendBroadCastItems.inc"
}

// _onBoosterItemUseResult(CUser*, vector<Inven_Item>&, int) —— ORIG 0x082099bc
#include "Dispatcher_UseBoosterItem_impl/_onBoosterItemUseResult.inc"
}

// _onBoosterItemUseResult_OnlyOne(CUser*, vector<Inven_Item>&, int) —— ORIG 0x08209a5e
// b28d-60：P277 receiver cast 前置——原写法 `static_cast<GrowthEquipEvent*>(evt2)->f(args)`
// 恒 this-first（receiver 先入 %eax、实参经 %edx）；把 cast 移到赋值点（具名派生指针）
// 复现 ORIG 的「实参先装 %eax、receiver 最后压栈」序。
#include "Dispatcher_UseBoosterItem_impl/_onBoosterItemUseResult_OnlyOne.inc"
}

// ============================================================================
// dispatch_sig —— ORIG 0x08208d40
// ============================================================================
#include "Dispatcher_UseBoosterItem_impl/dispatch_sig.inc"
}

#include "Dispatcher_UseBoosterItem_impl/process.inc"
#include "Dispatcher_UseBoosterItem_impl/read.inc"
