// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GiveGrowCreatureEvent/<Func>.inc（不参与编译、内部不含 #include）。
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

// ORIG W 0x081b763e _Z9ss_strcpyILj30EEiRAT__cPKc：头内联模板版（同
// OnlyOrigBatch12.cpp 口径），调用点以数组引用选择本重载。
template <size_t N>
#include "GiveGrowCreatureEvent/ss_strcpy.inc"

#include "GiveGrowCreatureEvent/GiveGrowCreatureEvent.inc"
#include "GiveGrowCreatureEvent/dtor_GiveGrowCreatureEvent.inc"

#include "GiveGrowCreatureEvent/StartEvent.inc"


#include "GiveGrowCreatureEvent/EndEvent.inc"


#include "GiveGrowCreatureEvent/StartEvent_1.inc"


#include "GiveGrowCreatureEvent/StartAction.inc"


#include "GiveGrowCreatureEvent/SendGrowthCreatureEventInfo.inc"


#include "GiveGrowCreatureEvent/CheckEventInfo.inc"


#include "GiveGrowCreatureEvent/processEvent.inc"


#include "GiveGrowCreatureEvent/isGrowCreatureMaxLevel.inc"


#include "GiveGrowCreatureEvent/changeCharacName.inc"


#include "GiveGrowCreatureEvent/IsEventing.inc"


#include "GiveGrowCreatureEvent/makeRequest.inc"


#include "GiveGrowCreatureEvent/makeRequest_1.inc"

