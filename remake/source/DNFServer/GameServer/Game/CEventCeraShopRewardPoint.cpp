// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventCeraShopRewardPoint/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Cera 商城奖励点数事件 CEventCeraShopRewardPoint 实现。
// 报告：docs/class_func_reports/CEventCeraShopRewardPoint/（C1 0x0816459a /
// D1 0x081645ca / D0 0x081645fa / StartEvent 0x08164618 /
// StartEvent(Word_Param) 0x08164684 / EndEvent 0x081646b8 /
// SendReward 0x0816471e）。
#include "CEventCeraShopRewardPoint.h"

#include <cstring>              // strncpy / strlen
#include "GameTypes.h"          // cMyTrace
#include "RDARScriptStringManager.h"   // g_scriptStringManager_ / findString
#include "CMailBoxHelper.h"     // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
                                //   （含 Inven_Item / ENUM_SERVER_GROUP）
#include "CUser.h"              // GetServerGroup / get_charac_no

#include "CEventCeraShopRewardPoint/CEventCeraShopRewardPoint.inc"


#include "CEventCeraShopRewardPoint/dtor_CEventCeraShopRewardPoint.inc"


#include "CEventCeraShopRewardPoint/StartEvent.inc"


#include "CEventCeraShopRewardPoint/StartEvent_1.inc"


#include "CEventCeraShopRewardPoint/EndEvent.inc"


#include "CEventCeraShopRewardPoint/SendReward.inc"

