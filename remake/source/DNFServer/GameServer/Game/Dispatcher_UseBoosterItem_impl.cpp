#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_UseBoosterItem_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "Dispatcher_UseBoosterItem_impl/sendBroadCastItems.inc"


#include "Dispatcher_UseBoosterItem_impl/_onBoosterItemUseResult.inc"


#include "Dispatcher_UseBoosterItem_impl/_onBoosterItemUseResult_OnlyOne.inc"


#include "Dispatcher_UseBoosterItem_impl/dispatch_sig.inc"


#include "Dispatcher_UseBoosterItem_impl/process.inc"
#include "Dispatcher_UseBoosterItem_impl/read.inc"
