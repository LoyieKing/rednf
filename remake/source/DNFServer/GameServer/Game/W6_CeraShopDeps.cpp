#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_CeraShopDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CCeraShop.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "SpecialItemHandler.h"
#include "CStreamGuard.h"
#include "CCeraShopBonusItemEvent.h"   // 权威声明（P104 ODR 收敛，2026-08-29 EventManagerWave）
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "PacketTypes.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "CItemLimitEdition.h"
#include "CEquipItem.h"
#include "CUser.h"
#include "DB_UpdateGiftCount.h"     // DB_UpdateGiftCount / DB_UpdateBreakAwayAccruedCera 权威声明
#include "CInventory.h"
#include "CMailBoxHelper.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"   // CEventManager / CEventBase 权威声明
#include "CEventMgr.h"
#include "CGenUniqueNo.h"
#include "LogManager.h"
#include "CCirculationStatistic.h"
#include "RDARScriptStringManager.h"
#include "TaiwanCash.h"          // WongWork::IPG::CIPGHelper 权威声明
#include "CSystemTime.h"
#include "CEnvironment.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include "BreakAwayPreventSystem.h"   // break_away_prevent::CBreakAwayPreventSystem 权威声明


#include "W6_CeraShopDeps/updateUsedCera.inc"
                                                                 unsigned int cera)
{
    DB_UpdateBreakAwayAccruedCera::makeRequest(accId, cera);
}

// DB_UpdateGiftCount/DB_UpdateBreakAwayAccruedCera 权威声明（含 makeRequest/dispatch
// 与生命周期实现）见 DB_UpdateGiftCount.h（本批次新建 TU，已登记 CMake note）。

// WongWork::CWebEvent 权威声明现位于 GlobalData.h（ReqSaveEventItem ORIG T 0x086c1910）。

namespace WongWork
{

namespace
{
// [AnonLane2 2026-09-11 匿名轴] 旧 `kBuyEntryStride = 0x1cf` 已删：全 TU（含
// 部件 .inc）零引用死常量，ORIG 全库零命中；-fkeep-static-consts 默认开 ⇒ 删定义。
}  // namespace

// SIPGData 为 char 镜像（TaiwanCash.h）；按偏移读形。P49：-O0 下 inline helper
// 会产真实 call 符号（仅NEW 发明符号违规），统一用宏直发 ORIG 的内联寻址形。
// AtoiAt/ReadIntAt 语义字段化由后续 IPG 批次收口【推断】。
#define AtoiAt(data, off) atoi(reinterpret_cast<const char*>(data) + (off))
// [b28d-S278 P319] 整型加法+回 cast：复现 4.1 `add $K,%eax; mov %eax,%reg`
// 两步形（指针算术 4.4 恒折单条 `lea K(%eax),%reg`；探针 /tmp/lg_probe4.cpp fA/fB）。
#define BufAt(data, off) ((char*)((unsigned int)(data) + (off)))

#include "W6_CeraShopDeps/CallSpecialItemHandler.inc"


#include "W6_CeraShopDeps/LogError.inc"


#include "W6_CeraShopDeps/_ProcessIPGError.inc"


#include "W6_CeraShopDeps/_processCoinGift.inc"


#include "W6_CeraShopDeps/_processCoinGift_1.inc"


#include "W6_CeraShopDeps/_processItemGift.inc"


#include "W6_CeraShopDeps/_processAvatarGift.inc"


#include "W6_CeraShopDeps/_processCreatureGift.inc"


#include "W6_CeraShopDeps/ProcessBuyRequest.inc"


#include "W6_CeraShopDeps/ProcessIPGUserData.inc"


}  // namespace WongWork

#include "W6_CeraShopDeps/isCerashopPackageItem.inc"

