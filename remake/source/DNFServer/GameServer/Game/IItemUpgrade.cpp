// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 CItemUpgrade_Separate.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN8WongWork12IItemUpgradeC2Ev
//   ORIG TU     ：IItemUpgrade.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "CServerProxyMgr.h"         // CServerProxyMgr<T>::GetServerProxy 模板定义
#include "CItemUpgrade_Separate.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CItem.h"
#include "PacketTypes.h"     // MSG_ITEM_UPGRADE 完整类型（具名字段访问）
#include "Inven_Item.h"
#include "CEquipItem.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CInventory.h"
#include "GameWorld.h"
#include "PvPTypes.h"                 // CMTRand 完整类型（0x9c8）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_.findString
#include <cstdio>
#include <cstring>
#include <string>

namespace WongWork
{
#include "IItemUpgrade/IItemUpgrade.inc"
}  // namespace WongWork
