// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCreature/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <string.h>

#include <map>
#include <algorithm>   // 与 CCreatureMgr.cpp 同前缀，保持共享头解析环境一致

#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "GlobalData.h"
#include "CGameManager.h"
#include "CUser.h"
#include "CCreatureScriptMgr.h"  // GetInstanceCreatureScriptMgr / FindCreatureScript
#include "CPacketRespondent.h"
#include "CCreatureScript.h"     // CCreatureScript 访问器（GetMaxLevel 等）
#include "TimerQueue.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"
#include "GameTypes.h"           // cMyTrace / ENUM_ITEMSPACE


namespace user_creature
{

#include "CCreature/IsSaveStomach.inc"


#include "CCreature/Resume.inc"


// CCreature ctor/dtor/GetOverSkillTime 定义于 CCreatureMgr.cpp（去重保留）。

#include "CCreature/Die.inc"


#include "CCreature/CheckDiedCreature.inc"


// ============================================================================
// Save/Load 落库链（TranslateCCreatureMgrTU 批次；逐函数对照
// docs/class_func_reports/user_creature__CCreature/ 的 ## 汇编 + ## 反编译 C）
// ============================================================================

#include "CCreature/SaveCreatureItem.inc"


#include "CCreature/LoadCreatureItem.inc"


// ============================================================================
// 以下为待头声明落地后的追加实现（TranslateCCreatureTU；已按各 .md 转写完毕）
// ============================================================================

#include "CCreature/RenameCreature.inc"


#include "CCreature/IsEvolutionCreature.inc"


#include "CCreature/IsEvolutionCreatureMulty.inc"


#include "CCreature/IsEventEvolutionCreature.inc"


#include "CCreature/GetLearnOverSkillLevel.inc"


#include "CCreature/GetOverSkillRecoveryTime.inc"


#include "CCreature/GetSkillRecoveryTime.inc"


#include "CCreature/EvoluteMulty.inc"


#include "CCreature/ItemEvolute.inc"


#include "CCreature/EventEvolute.inc"


}  // namespace user_creature
// ORIG _ZN13user_creature12CDBMsgSender20EventEvoluteCreatureEiii W 0x833fddc
//（objdump 逐条转写）：stream 空→0；buf memset 0x74（ORIG 无判空，如实保留）；
// m_hdr|=0x248；compat.m_itemIdx(+0x08)=itemIdx；m_addInfo2(+0x30)=uid；
// exp 写 +0x4c；find_item(itemIdx)（ORIG 不判空）→ m_days(+0x54)=
// getUsablePeriod()、m_expireTick(+0x58)=getExpirationDate()；返 1。

#include "CCreature/EventEvoluteCreature.inc"

 
 // ORIG _ZN13user_creature12CDBMsgSender4InitE19ENUM_INTERNALPACKETP5CUser W 0x833f7a8

#include "CCreature/Init.inc"

