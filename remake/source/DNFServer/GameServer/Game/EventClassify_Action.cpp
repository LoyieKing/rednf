#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventClassify_Action/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— EventClassify::CEventActionMng 发奖/发信实现。
// 逐函数依据 docs/class_func_reports/EventClassify__CEventActionMng/*.md
// （process_action_send_mail 0x0810cf32 / process_coin_send 0x0810d4ae /
//   process_avatar_send 0x0810d70c / process_gold_mail_send 0x0810d83c /
//   process_mail_send 0x0810da1c）反汇编转写；类布局见 EventClassify_Action.h。
// 邮件发件人/标题缓冲按 ORIG 以 `= {0}` 清零（gcc 4.4 -O0 展开为内联循环，
// 与 ORIG rep stos 一致），再 findString(表4) + strncpy(0x1d/0xff) 填充。

#include "EventClassify_Action.h"

#include <cstring>

#include "CAvatarItemMgr.h"          // WongWork::CAvatarItemMgr::GetRemainDate
#include "CGenUniqueNo.h"            // WongWork::CGenUniqueNo::genIPGNo
#include "CInventory.h"
#include "CItem.h"                   // make_item / IsAvatarItem / getUsablePeriod
#include "CMailBoxHelper.h"          // WongWork::CMailBoxHelper::ReqDBSendNewSystemMail
#include "CDataManager.h"            // G_CDataManager / find_item
#include "CUser.h"                   // Send/SendCoin/SaveMoneyCoin/m_historyLog
#include "CStackableItem.h"         // getBoosterItem / stStackableBoosterItemInfo_t
#include "GlobalData.h"              // s_systemTime_
#include "CSystemTime.h"             // getCurSec
#include "Inven_Item.h"
#include "PacketGuard.h"
#include "RDARScriptStringManager.h" // g_scriptStringManager_ / findString
#include "SyncScriptDeps.h"          // toTString

namespace EventClassify
{

#include "EventClassify_Action/process_action_send_mail.inc"


#include "EventClassify_Action/process_coin_send.inc"


#include "EventClassify_Action/process_avatar_send.inc"


#include "EventClassify_Action/process_gold_mail_send.inc"


#include "EventClassify_Action/process_mail_send.inc"


#include "EventClassify_Action/getResultItemFromLottery.inc"


}  // namespace EventClassify
