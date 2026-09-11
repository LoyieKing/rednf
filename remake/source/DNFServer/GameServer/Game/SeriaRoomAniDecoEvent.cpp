// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SeriaRoomAniDecoEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <boost/foreach.hpp>       // ORIG：questClearLog 用 BOOST_FOREACH
#include <boost/lexical_cast.hpp>  // ORIG：buyItemLog/questClearLog 用 lexical_cast
#include "GlobalData.h"
#include "W8LT_EventTail.h"
#include "GameTypes.h"            // cMyTrace
#include "GlobalData.h"
#include "CSystemTime.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "MoreTypes.h"            // SIG_GIVE_GROWTH_CREATURE_EVENT
#include "CUser.h"
#include "../../../Library/Include/Core/SecureStdio.h"   // ss_strcpy                // GrowthCreatureEvent / getGrowthCreatureEventdata
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CDataManager.h"
#include "MySQL.h"            // MySQL::{set_query,exec,getAffectedRowCount}
#include "CEnvironment.h"     // DB_HANDLE_9 / E_SERVER_GROUP_NONE
#include "CItem.h"
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameWorld.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"

// ============================ SeriaRoomAniDecoEvent ============================

SeriaRoomAniDecoEvent::SeriaRoomAniDecoEvent() {}       // C1/C2 0x0827b490
SeriaRoomAniDecoEvent::~SeriaRoomAniDecoEvent() {}      // D1/D2 0x0827b4ac

#include "SeriaRoomAniDecoEvent/IsEventing.inc"

#include "SeriaRoomAniDecoEvent/StartEvent.inc"

#include "SeriaRoomAniDecoEvent/EndEvent.inc"

#include "SeriaRoomAniDecoEvent/StartEvent_1.inc"

#include "SeriaRoomAniDecoEvent/StartAction.inc"

#include "SeriaRoomAniDecoEvent/dailyresetData.inc"

#include "SeriaRoomAniDecoEvent/SendAniDecoItem.inc"

#include "SeriaRoomAniDecoEvent/SendEventInfo.inc"

#include "SeriaRoomAniDecoEvent/CheckEventInfo.inc"

#include "SeriaRoomAniDecoEvent/processEvent.inc"

#include "SeriaRoomAniDecoEvent/makeRequest.inc"

#include "SeriaRoomAniDecoEvent/dispatch.inc"

// [L32 T3] ORIG `correctQuestIds` D 0x09370168 size 0xc：TU 级 file-scope 全局
//   （非 const ⇒ .data；值 {0x1567,0x1568,0x1569}），由 OneADayItemShopEvent::isQuestId
//   0x081ba1e2 线性查表消费。原先误做成函数内 `static const int`（局部符号 + .rodata）
//   ⇒ 提为 TU 级定义。位置在首个强符号（SeriaRoomAniDecoEvent::C2Ev）之后，GI 键名不变。
extern int correctQuestIds[3];   // [SplitC batch5] 定义已迁出至 OneADayItemShopEvent.cpp

// ============================ OneADayItemShopEvent ============================
OneADayItemShopEvent::OneADayItemShopEvent() {}         // C1/C2 0x081b9fb4
OneADayItemShopEvent::~OneADayItemShopEvent() {}        // D1/D2 0x081b9fd0

#include "SeriaRoomAniDecoEvent/StartEvent_2.inc"

#include "SeriaRoomAniDecoEvent/EndEvent_1.inc"

#include "SeriaRoomAniDecoEvent/StartEvent_3.inc"

#include "SeriaRoomAniDecoEvent/StartAction_1.inc"

#include "SeriaRoomAniDecoEvent/deliveryStart.inc"

#include "SeriaRoomAniDecoEvent/deliveryEnd.inc"

#include "SeriaRoomAniDecoEvent/delivery.inc"

#include "SeriaRoomAniDecoEvent/isQuestId.inc"

#include "SeriaRoomAniDecoEvent/questClearLog.inc"

#include "SeriaRoomAniDecoEvent/buyItemLog.inc"

#include "SeriaRoomAniDecoEvent/makeRequest_1.inc"
