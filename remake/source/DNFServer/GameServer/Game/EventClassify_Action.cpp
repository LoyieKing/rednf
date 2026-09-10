// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EventClassify_Action/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ============================================================================
// process_action_send_mail（ORIG 0x0810cf32）
// 返回：3=user 为空；0x11=物品表未命中；0=成功（MakeItemPacket 失败亦回 0）。
// ============================================================================
// [b28d-solo-EventClassify_Action] ORIG 0x0810cf32，381=381 AE（p167 41/41 CLEAR）。
//   本件修法：①mail.m_itemIdx 先落具名 int itemIdx（ORIG -0x18）、find_item(itemIdx)；
//   ②IsAvatarItem 两处改正极性 `if (IsAvatarItem()) {avatar} else {insert}`（ORIG
//     `test %al,%al; je <else>`）；③PacketGuard 内删 sendOk 预清局部，改
//     `if (!MakeItemPacket(...)) return 0;` 内联（ORIG `call; xor $1; test; je`）。
#include "EventClassify_Action/process_action_send_mail.inc"
}

// ============================================================================
// process_coin_send（ORIG 0x0810d4ae）
// m_field1c!=0：事件币直加（GetEventCoin+add_info → SetEventCoin → 日志 →
//               SaveMoneyCoin/SendCoin）；否则置 endurance=1 走 DB 邮件。
// [b28d-solo-EventClassify_Action] ORIG 0x0810d4ae，169=169 AE（p167 19/19 CLEAR）。
//   旧判词记的「characNo eax→edx 中转」系旧 .o 假象：fresh 编译后尾段
//   `call getCurCharacNo; mov 0x10(%ebp),%edx; mov 0x28(%edx),%edx` 与 ORIG 全等。
// ============================================================================
#include "EventClassify_Action/process_coin_send.inc"
}

// ============================================================================
// process_avatar_send（ORIG 0x0810d70c）
// genIPGNo(type7) 取 IPG 号 → AddAvatarItem(idx, usablePeriod,0,0,-1,ipg,2,0,0)
// 成功后按槽内物品与当前时间回填剩余日期到 item +0x07 并清 upgrade 位。
// ============================================================================
// [b28d-solo-EventClassify_Action] ORIG 0x0810d70c，79=79 AE。
//   本件修法：①ipg/usablePeriod 去具名局部改内联实参（P0/P200）⇒ 复现 ORIG
//     esi(ipg)/ebx(usablePeriod)/edi(item.m_addInfo) callee-saved 独占；
//   ②GetRemainDate 的 addInfo2 实参改匿名嵌套 sret
//     `GetRemainDate(GetInvenSlot(2,slot).m_addInfo2, now)`（P278）⇒ ipgBuf@-0x75、
//     sret@-0x5d 槽序与帧 0x9c 与 ORIG 全等（具名 slotItem 时两者互换、NEAR）。
#include "EventClassify_Action/process_avatar_send.inc"
}

// ============================================================================
// process_gold_mail_send（ORIG 0x0810d83c）
// 全新 Inven_Item（idx 置 -1）携带 mail.m_gold(+0x28) 走 DB 金币邮件。
// ============================================================================
#include "EventClassify_Action/process_gold_mail_send.inc"
}

// ============================================================================
// process_mail_send（ORIG 0x0810da1c）
// citem 为空回 0x11；avatar 物品不入邮件（仅清局部缓冲），其余走 DB 邮件。
// [b28d-solo-EventClassify_Action] ORIG 0x0810da1c，173=173 AE。
//   分支极性 `if (IsAvatarItem()) {avatar 死缓冲} else {DB 邮件}` 与
//   title[256]={0}/sender[30]={0} 预清、findString(4,toTString)=strncpy、
//   ReqDBSendNewSystemMail 九参装载逐条对齐（旧 N82 判词系旧 .o 假象，已撤销）。
// ============================================================================
#include "EventClassify_Action/process_mail_send.inc"
}

// ============================================================================
// getResultItemFromLottery（ORIG 0x0810dc5a）
// 按 lotterycard/booster 两类物品展开为 <idx,count> 列表：
//   GetItemType()==6  → find_item + get_lotto_item → push(m_addInfo,m_addInfo2)
//   GetItemType()==16 → getBoosterItem(list) → 逐条 push(m_itemIdx,m_addInfo)
// 其它类型返回 0x17；find 失败或空结果返回 0x11。
// [b28d-solo-EventClassify_Action] ORIG 0x0810dc5a，176 vs 170（差 6 条=唯一残差），
//   N 封顶。残差=case 16 循环内 `if (it->m_itemIdx == 0) return 0x11;` 的 0/1 物化：
//   ORIG(4.1) 发「`call ptEv; mov (%eax),%eax; test; je L1; call ptEv(丢弃);
//   mov $0; jmp L2; L1: mov $1; L2: test %al,%al; je`」= 分支+常量且条件表达式二次
//   求值；4.4 恒折 `test; sete %al; test; je`（一次求值）。已试 `==0` / `!` /
//   具名 bool / while 四形，sete 恒不变 ⇒ setcc-fold 跨版本负例（P197④ 族）。
//   可修部分已落：CStackableItem* stackable 具名局部（ORIG -0xc）复现
//   `mov -0x10(%ebp),%eax; mov %eax,-0xc(%ebp)` 两条。
// ============================================================================
#include "EventClassify_Action/getResultItemFromLottery.inc"
}

}  // namespace EventClassify
