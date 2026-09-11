// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_InvestItemAmplifyOption_impl/<Func>.inc（不参与编译、内部不含 #include）。
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

#include "CDataManager.h"      // G_CDataManager(), WongWork::CItemUpgrade, m_itemUpgrade (+0x20)
#include "CInventory.h"        // getCurCharacInvenR/W, INVEN_TYPE, GetInvenSlot/delete_item/update_item
#include "CItemAmplifier.h"    // CItemAmplifier, stInvestAmplifyOption_t, ENUM_ITEM_AMPLIFY_INVEST_TYPE, stAmplifyOption_t
#include "CItem.h"             // CItem, IsEnableWorld(ENUM_WORLD_TYPE) + IsEnableWorld(const CItem*)
#include "GlobalData.h"        // GlobalData::s_itemAmplifier_
#include "GameWorld.h"         // G_GameWorld(), IsPVPChannel(), IsPvPSkilTreeChannel()
#include "CUserCharacInfo.h"   // get_charac_level(), getCurCharacInvenR/W()（CUser is-a CUserCharacInfo）
#include "Inven_Item.h"        // Inven_Item（0x3d, pack1）
#include "CUser.h"             // CUser::get_state()/CheckItemLock/Send/SendCmdErrorPacket, m_historyLog

// ORIG 自由函数 LineFunc(int, const char*, int, unsigned int) @0x085908d7 T；
// 定义由 CompoundEmblemDeps.cpp 提供（见 CompoundEmblemDeps.h）。
extern int LineFunc(int, const char*, int, unsigned int);

#include "Dispatcher_InvestItemAmplifyOption_impl/dispatch_sig.inc"


#include "Dispatcher_InvestItemAmplifyOption_impl/process.inc"

#include "Dispatcher_InvestItemAmplifyOption_impl/read.inc"
