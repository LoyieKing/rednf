// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_CompoundEmblem_impl/<Func>.inc（不参与编译、内部不含 #include）。
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

#include <cstring>          // strlen
#include <utility>          // std::pair/make_pair
#include <vector>

#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "CUserCharacInfo.h"
#include "PacketGuard.h"            // CUser::Send(PacketGuard&)
#include "CGameManager.h"           // WongWork::CMailBoxHelper
#include "RDARScriptStringManager.h"
#include "CompoundEmblemDeps.h"     // getCompoundEmblemStatistic / getAvatarEmblemTypeIdxForCompountEmblem / LineFunc
#include "Packet_Emblem_Create_Statistic.h"  // Packet_Emblem_Create_Statistic（C1 W 0x08110bdc）
#include "CStatisticServerProxy.h"
#include "GlobalData.h"             // GlobalData::s_statistic_proxy_mgr
#include "CServerProxyMgr.h"        // CServerProxyMgr<T>::GetServerProxy

// _selectJewelTypeIndex 与 SendCreateEmblemInfo 为 ORIG 私有成员
// （_ZNK25Dispatcher_CompoundEmblem21_selectJewelTypeIndexEjPKt T 0x08220a60、
//  _ZN25Dispatcher_CompoundEmblem20SendCreateEmblemInfoERKSt6vectorISt4pairIiiESaIS2_EE
//  T 0x08221dde），声明于 Dispatchers.h，本 TU 实现。
// ============================================================================

#include "Dispatcher_CompoundEmblem_impl/_selectJewelTypeIndex.inc"

#include "Dispatcher_CompoundEmblem_impl/SendCreateEmblemInfo.inc"
#include "Dispatcher_CompoundEmblem_impl/dispatch_sig.inc"

int Dispatcher_CompoundEmblem::read(PacketBuf& packet, MSG_BASE& msg) { (void)packet; (void)msg; return 0; }