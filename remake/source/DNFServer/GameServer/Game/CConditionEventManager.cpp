// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CConditionEventManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CConditionEventManager.h"
// ============================================================================
// b28d-52 判词表（本 TU）：
//   已修：_getRequiredParameter 0x083356a4（P236/P293 内联实参翻槽序）、
//     IsFreeGoldCard 0x0833579e（P0 单变量复用）、ProcessCheckStepUp 0x08335566
//     （P286 单 case switch）⇒ 均转 AE。
//   剩余 DIFF：
//   GetConditionRewardInfo 0x083353a0 —— N86（4.1 iterator 临时槽+拷贝两段；
//     4.4 copy-elision 直构 ⇒ 槽序 -0x28/-0x24 不可复现）
//   RequestConditionEventReward 0x08334850 —— 已修：删发明 helper insertFailSendMail
//     （ORIG nm 0 命中）改两处内联 + inv.m_addInfo(+0x02) 缓存 itemIdx + guard 内联短路
//     + GetItemType 双调/否定守卫块序 + slot 两段式(-1 初始化) + bool ok 内联 ⇒
//     帧 0x25c→0x24c 全对齐、p167 双向差集仅剩 C1/C2·D1/D2 同址别名（P93）。
//     [M4S3 round5c] 末尾 4 条（两处 slot 最高位判定）用 P211 类型轴
//     `(unsigned char)((unsigned int)slot >> 31)` 复现 4.1 的 `shr $0x1f;test %al;je`
//     ⇒ 等长同形（objdump 同口径 ORIG/our 663/663；报告头 614 为另一口径）、
//     ndiff 0、转 AE（详见函数头注释）。
// ============================================================================


#include <string.h>

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "CItemList.h"
#include "Inven_Item.h"
#include "CInventory.h"
#include "CCeraShop.h"
#include "CSystemTime.h"
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "PacketGuard.h"


#include "CConditionEventManager/CConditionEventManager.inc"


#include "CConditionEventManager/dtor_CConditionEventManager.inc"


#include "CConditionEventManager/ConditionRewardSameItemCombine.inc"


#include "CConditionEventManager/RequestConditionEventReward.inc"


#include "CConditionEventManager/GetCurEventIndex.inc"


#include "CConditionEventManager/GetMaxRewardStep.inc"


#include "CConditionEventManager/_getRequiredParameter.inc"


#include "CConditionEventManager/_processProperDungeonClearEvent.inc"


#include "CConditionEventManager/CheckErrorCardForStep.inc"


#include "CConditionEventManager/GetConditionRewardInfo.inc"


#include "CConditionEventManager/IsDoubleGoldCard.inc"


#include "CConditionEventManager/IsDoubleGoldCardProcess.inc"


#include "CConditionEventManager/IsFreeGoldCardProcess.inc"



#include "CConditionEventManager/IsFreeGoldCard.inc"


#include "CConditionEventManager/ProcessCheckStepUp.inc"

