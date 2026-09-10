// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 Dispatcher_CompoundEmblem_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ORIG _ZNK25Dispatcher_CompoundEmblem21_selectJewelTypeIndexEjPKt（0x8220a60）
// 纹章位图→槽位 加权随机选取珍宝类型索引。
#include "Dispatcher_CompoundEmblem_impl/_selectJewelTypeIndex.inc"
}

// 布局（ORIG 汇编实证）：Packet_Emblem_Create_Statistic(0x17a4, 0x36)
// + count@slots[0](+0xa) + grades@slots[i+1](+0xe) + addInfo@slots[i+6](+0x22)，
// SendPacket(pkt, 0x36)。迭代器 while 循环（ORIG __gnu_cxx::operator!= 判续）。
#include "Dispatcher_CompoundEmblem_impl/SendCreateEmblemInfo.inc"
}
#include "Dispatcher_CompoundEmblem_impl/dispatch_sig.inc"
}

#include "Dispatcher_CompoundEmblem_impl/process.inc"
#include "Dispatcher_CompoundEmblem_impl/read.inc"
