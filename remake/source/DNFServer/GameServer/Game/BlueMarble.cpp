#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BlueMarble/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r BlueMarble（G2-3 PvP 链，大富翁房间，池化 300）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarble.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 依赖子对象（BlueMarbleScriptManager/BlueMarbleUserInfo/CMTRand/...）为
// PvP_deps 桩；本 TU 以 asm-label extern 发跨类调用。只写本文件。
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "BlueMarble.h"
#include "CDataManager.h"
#include "CInventory.h"          // 权威 CInventory（原本地最小声明与 CInventory.h:418 重定义冲突）
#include "CEnvironment.h"
#include "LogManager.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "PacketGuard.h"   // 权威 PacketGuard（0xc）
#include "GameWorld.h"            // G_GameWorld / GameWorld::gotoBlueMarble/outFromBlueMarble（§9）
#include "CPrivateStoreMgr.h"     // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore（§9）
#include "RDARScriptStringManager.h"  // g_scriptStringManager_/findString（§9）
#include "CGameManager.h"         // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail + G_CGameManager（§9）
#include "PacketTypes.h"          // 权威 MSG_MAILBOX_SEND（+0x11/+0x39/+0x3d 具名字段）

// 2026-09-05 裸偏移收尾：原本地 shadow struct BM_ScriptFields（把 m_pScript 当
// 未建模字节区直读 +0x30..+0x4c）删除 —— 该区间即权威
// BlueMarbleScriptManager::m_info（+0x30，BlueMarbleInfoScript），逐槽改用
// m_pScript->m_info.<字段>（证据：getRandomItem 0x080d7690 `mov 0x4c(%eax)`、
// setDiceNumber 0x080d6f88 `mov 0x48/0x44(%eax)`、checkUserConditionEnterBlueMarble
// 0x080d8d02 `mov 0x30/0x34/0x38/0x3c(%eax)`，均 4 字节读）；private 槽位经
// BlueMarbleInfoScript 的 `friend class BlueMarble` 授权（ORIG 无 getter 符号）。

// ---- CGameManager / G_CGameManager：经 #include "CGameManager.h"（§9） ----

// ============================================================================
// BlueMarble 实现
// ============================================================================

#include "BlueMarble/BlueMarble.inc"


#include "BlueMarble/dtor_BlueMarble.inc"


#include "BlueMarble/getMemoryPoolIndex.inc"


#include "BlueMarble/setMemoryPoolIndex.inc"


#include "BlueMarble/getState.inc"


#include "BlueMarble/changeState.inc"


#include "BlueMarble/getWinner.inc"


#include "BlueMarble/getWaitUserCount.inc"


#include "BlueMarble/getUserSlot.inc"


#include "BlueMarble/getUserState.inc"


#include "BlueMarble/setUserState.inc"


#include "BlueMarble/setUserGrade.inc"


#include "BlueMarble/getUserDeadCount.inc"


#include "BlueMarble/setUserDeadCount.inc"


#include "BlueMarble/getMaxBoardSize.inc"


#include "BlueMarble/setMaxBoardSize.inc"


#include "BlueMarble/getDiceNumber.inc"


#include "BlueMarble/setDiceNumber.inc"


#include "BlueMarble/getBlueMarbleMap.inc"


#include "BlueMarble/setBlueMarbleMap.inc"


#include "BlueMarble/getMovePosition.inc"


#include "BlueMarble/setMovePosition.inc"


#include "BlueMarble/getVisitZone.inc"


#include "BlueMarble/setVisitZone.inc"


#include "BlueMarble/getDungeonIndex.inc"


#include "BlueMarble/setDungeonIndex.inc"


#include "BlueMarble/getItemIndex.inc"


#include "BlueMarble/setItemIndex.inc"


#include "BlueMarble/getExpBuff.inc"


#include "BlueMarble/setExpBuff.inc"


#include "BlueMarble/getGoldBuff.inc"


#include "BlueMarble/setGoldBuff.inc"


#include "BlueMarble/getDropBuff.inc"


#include "BlueMarble/setDropBuff.inc"


#include "BlueMarble/getBuff.inc"


#include "BlueMarble/getRandomItem.inc"


#include "BlueMarble/getRandomDungeon.inc"


#include "BlueMarble/checkJoin.inc"


#include "BlueMarble/join.inc"


#include "BlueMarble/leaveUser.inc"


#include "BlueMarble/setScript.inc"


#include "BlueMarble/setUserInfo.inc"


#include "BlueMarble/resetBlueMarble.inc"


#include "BlueMarble/resetBuffInfo.inc"


#include "BlueMarble/resetIndexInfo.inc"


#include "BlueMarble/checkUserConditionEnterBlueMarble.inc"


#include "BlueMarble/checkBlueMarbleUserConfirm.inc"


#include "BlueMarble/enterBlueMarble.inc"


#include "BlueMarble/checkStartBlueMarble.inc"


#include "BlueMarble/startBlueMarble.inc"


#include "BlueMarble/checkThrowDice.inc"


#include "BlueMarble/throwDice.inc"


#include "BlueMarble/userThrowDice.inc"


#include "BlueMarble/movePosition.inc"


#include "BlueMarble/checkMoveFirstZone.inc"


#include "BlueMarble/tileType.inc"


#include "BlueMarble/intoBoardZone.inc"


#include "BlueMarble/checkLastZoneDungeonClear.inc"


#include "BlueMarble/enterDungeon.inc"


#include "BlueMarble/exitDungeon.inc"


#include "BlueMarble/clearDungeon.inc"


#include "BlueMarble/giveupDungeon.inc"


#include "BlueMarble/giveBonusItem.inc"


#include "BlueMarble/resultReward.inc"


#include "BlueMarble/finishBlueMarble.inc"


#include "BlueMarble/setResultItem.inc"


#include "BlueMarble/insertItem.inc"


#include "BlueMarble/setLeaveUserBlueMarbleState.inc"


#include "BlueMarble/setLeaveUserSlot.inc"


#include "BlueMarble/sendBlueMarbleDiceInfo.inc"


#include "BlueMarble/sendBlueMarbleRoomInfo.inc"


#include "BlueMarble/sendBoardZoneActionInfo.inc"


#include "BlueMarble/sendFinishBlueMarble.inc"


#include "BlueMarble/sendStartBlueMarble.inc"


#include "BlueMarble/sendTileGiveUpInfo.inc"


#include "BlueMarble/sendUpdateItemInfo.inc"


#include "BlueMarble/sendToBlueMarble.inc"

