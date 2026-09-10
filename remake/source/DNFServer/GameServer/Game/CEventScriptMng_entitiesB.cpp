// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CEventScriptMng_entitiesB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EventClassify 第二组实体实现（group 2，FixEventEntitiesB）。
// 覆盖 ctor/dtor/脚本导入路径方法（set_event_data / get_db_table_name）。
// 每个实体在 CEventEntity（sizeof 0x1c）之上追加绑定脚本指针 m_script1c(+0x1c)，
// sizeof = 0x20。get_db_table_name 为虚方法（ORIG 虚表 slot 8 / +0x20）。
#include "CEventScriptMng.h"

#include "PacketGuard.h"
#include "CUser.h"
#include "CUserCharacInfo.h"           // getCurCharacNo / getCurCharacUsedFatigue
#include "GameTypes.h"                 // cMyTrace
#include "LogManager.h"
// 事件发奖动作域（process_action_send_mail 定义 EventClassify_Action.cpp）
#include "EventClassify_Action.h"
#include "DB_UpdatePurchaseCashItemBonus.h"
#include "DB_InsertAccountFirstLoginDailyFlag.h"

// process_action_send_mail 形参用的是 EventClassify_Action.h 的命名空间内
// Action_SendMail；脚本子对象 m_mail 为全局 ::Action_SendMail（ORIG 符号口径，
// 见 InGameEventScripts.h）。两者逐字段同布局（sizeof 均 0x44）。用宏桥接：
// -O0 下 inline 函数会产生多余调用帧（ORIG 无）。同 CEventScriptMng_entities.cpp。
typedef char dnf_mail_layout_assert_b_[
    (sizeof(::Action_SendMail) == sizeof(Action_SendMail)) ? 1 : -1];
#define EVENT_MAIL_REF(mail) (*reinterpret_cast<Action_SendMail*>(&(mail)))

namespace EventClassify
{

// ============ CFatigueQuantity ============
// ORIG ctor 0x08111030 W / D1 0x08114aac / D0 0x08114adc / sizeof 0x20。
#include "CEventScriptMng_entitiesB/dtor_CFatigueQuantity.inc"
}

// ORIG 0x0810f95e：虚调度 set_event_record_type（vtable+0x18）写 m_recordType=key，
// 然后把绑定脚本存到 +0x1c。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：P211 档③——m_script1c
//   具型化（FatigueQuantityScript*）+ 赋值点 static_cast 异型化 ⇒ store 复现 ORIG
//   RHS-first（0x810f97d 三条逐条一致）；4.4.6 后端同时消 N7（vfn %ecx / this %eax
//   与 ORIG 同形）。4.4.7 下该件仍 NEAR（纯 N7 后端差：vfn %eax / this %edx）；
//   实测 TU identical 30→35 / ae 42 / near 5→0 / diff 1（N123 撞名假 DIFF）零回归。
//   路由已落 dnf_game_cxx.sh:370（b28d-S188）（ORIG 0x0810f95e sz 0x0000002a）
#include "CEventScriptMng_entitiesB/set_event_data.inc"
}

#include "CEventScriptMng_entitiesB/get_db_table_name.inc"
}

// ORIG W 0x08110edc：返回绑定脚本 +0x0c 的 [user type] 字节。
#include "CEventScriptMng_entitiesB/get_user_type.inc"
}

// ============ CExchangeRandomItemReward ============
// ORIG ctor 0x08110ff8 W / D1 0x08114b48 / D0 0x08114b78 / sizeof 0x20。
#include "CEventScriptMng_entitiesB/dtor_CExchangeRandomItemReward.inc"
}

// ORIG 0x0810eb06：同 CFatigueQuantity 模式。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：同
//   CFatigueQuantity::set_event_data——P211 档③ 具型化
//   （ExchangeRandomItemRewardScript*）⇒ store RHS-first；4.4.6 消 N7。4.4.7 下
//   仍 NEAR（纯 N7 后端差）。路由已落 dnf_game_cxx.sh:370（ORIG 0x0810eb06 sz 0x0000002a）
#include "CEventScriptMng_entitiesB/set_event_data_1.inc"
}

// ============ CAvatarDisjointRandomReward ============
// ORIG ctor 0x08111014 W / D1 0x08114afa / D0 0x08114b2a / sizeof 0x20。
#include "CEventScriptMng_entitiesB/dtor_CAvatarDisjointRandomReward.inc"
}

// ORIG 0x0810f37a：同 CFatigueQuantity 模式。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：同
//   CFatigueQuantity::set_event_data——P211 档③ 具型化
//   （AvatarDisjointRandomRewardScript*）⇒ store RHS-first；4.4.6 消 N7。4.4.7 下
//   仍 NEAR（纯 N7 后端差）。路由已落 dnf_game_cxx.sh:370（ORIG 0x0810f37a sz 0x0000002a）
#include "CEventScriptMng_entitiesB/set_event_data_2.inc"
}

// ============ CObjectBringUp ============
// ORIG ctor 0x08111068 W / D1 0x08114a10 / D0 0x08114a40 / sizeof 0x20。
#include "CEventScriptMng_entitiesB/dtor_CObjectBringUp.inc"
}

// ORIG 0x0810fcb2：同 CFatigueQuantity 模式。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：同
//   CFatigueQuantity::set_event_data——P211 档③ 具型化（ObjectBringUpScript*）⇒
//   store RHS-first；4.4.6 消 N7。4.4.7 下仍 NEAR（纯 N7 后端差）。路由已落
//   dnf_game_cxx.sh:370（ORIG 0x0810fcb2 sz 0x0000002a）
#include "CEventScriptMng_entitiesB/set_event_data_3.inc"
}

#include "CEventScriptMng_entitiesB/get_db_table_name_1.inc"
}

// ORIG 0x08110386：按已用疲劳档位发放事件物品邮件。
// 遍历 m_ruleMails1c（[eventitem send] 规则-邮件对），内层阈值列表任一 t 满足
// 「(当前已用疲劳 - 增量) < t 且 当前已用疲劳 >= t」即发该对邮件；
// 发信成功读 m_mail.m_itemIdx 记日志后返回（ORIG 单次发放语义）。
// 全程不缓存脚本指针（与 ORIG 同）。
// [b28d-solo-CEventScriptMng_entitiesB] AE（compare_tu 口径，138=138）。修法：
// P0 去 usedBase/threshold/hit 具名局部（ORIG 无槽往返）⇒ 帧 0x50→0x40；
// CEventActionMng mng 移入内层循环作用域复现 ORIG 槽布局（mng -0x21/it -0x1c/
// vit -0x20）；第二条件改写 `*vit <= used` 保左→右求值序（*vit→%ebx 先于
// getCurCharacUsedFatigue→%eax）；外层 it->/ptEv 替 (*it)./deEv（P167）。
#include "CEventScriptMng_entitiesB/sendEventItemFromFatigue.inc"
}

// ============ CPurchaseCashItemBonus ============
// ORIG ctor 0x0811104c W / D1 0x08114a5e / D0 0x08114a8e / sizeof 0x20。
#include "CEventScriptMng_entitiesB/dtor_CPurchaseCashItemBonus.inc"
}

// ORIG 0x0810f996：同 CFatigueQuantity 模式。
// [b28d-solo-CEventScriptMng_entitiesB] AE（4.4.6 后端，17/17）：同
//   CFatigueQuantity::set_event_data——P211 档③ 具型化
//   （PurchaseCashItemBonusScript*）⇒ store RHS-first；4.4.6 消 N7。4.4.7 下仍
//   NEAR（纯 N7 后端差）。路由已落 dnf_game_cxx.sh:370（ORIG 0x0810f996 sz 0x0000002a）
#include "CEventScriptMng_entitiesB/set_event_data_4.inc"
}

// ORIG 0x0810f9c0：return &m_dbTableName（PurchaseCashItemBonusScript +0x08）。
#include "CEventScriptMng_entitiesB/get_db_table_name_2.inc"
}

// ORIG 0x0810f9ce：目标商品购买计数更新。
// user 为空走 cMyTrace 告警；否则取 acc_id，当 IPG 请求条目 m_itemIdx
// 命中脚本 m_set0c（[item idx start]/[item idx end] 区间导入的目标商品集合）
// 时向 DB 队列发 DB_UpdatePurchaseCashItemBonus（m_id, acc_id）请求，
// 并以 cMyTrace 记 "updateTargetGoodsCount(%d)"（acc_id）。
// [b28d-solo-CEventScriptMng_entitiesB] AE（compare_tu 口径，71=71，帧 0x48 无
// callee-saved）。修法：P293 具名迭代器固定 find→end 求值序（ORIG sret -0x38/-0x20；
// 4.4 单表达式 `find()!=end()` 恒先 end 后 find）；accId 声明 unsigned 消 %ebx
// 跨 GetUID 保活（P290 反向）；两处 cMyTrace 改全表达式临时链复现 ORIG 槽深（P221）。
#include "CEventScriptMng_entitiesB/updateTargetGoodsCount.inc"
}

// ORIG 0x0810fae0：档位规则校验（count=本次比较基准购买数）。
// count < start 不发；end 非 0 且 count > end 不发；(count-start)%step!=0 不发。
#include "CEventScriptMng_entitiesB/verifyGiveBonusReward.inc"
}

// ORIG 0x0810fb34：按购买数遍历 m_ruleMails24 规则-邮件对发奖励邮件。
// user 为空仅记 LogManager 告警返回 false；命中规则且发信成功读
// mail.m_itemIdx 记日志并将返回置 false（ORIG 返回值语义：true=本轮未发放）。
// [b28d-solo-CEventScriptMng_entitiesB] AE（compare_tu 口径，97=97；sent 槽 -0x9）。
// 修法：P198① result 循环外预清零（ORIG movl $0x0,-0x10）；删 rules 引用缓存，
// 每轮重发 `m_script1c->m_ruleMails24` 取址链（P157/P183）；it->/ptEv 替
// (*it)./deEv 并对 verifyGiveBonusReward/process_action_send_mail 逐次重发（P167）。
#include "CEventScriptMng_entitiesB/sendPurchaseBonusReward.inc"
}

// ============ CAccountFirstLoginDaily ============
// 派生自 CAccountFirstLogin（group1，FixEventEntitiesA）。
// ORIG ctor 0x0811110a W / D1 0x08114c80 / D0 0x08114cb0。
// 不新增成员，+0x1c 继承自 CAccountFirstLogin。
// ORIG T 0x810dfc6：直接委托基类（CAccountFirstLogin::set_event_data）。
#include "CEventScriptMng_entitiesB/set_event_data_5.inc"
}

#include "CEventScriptMng_entitiesB/dtor_CAccountFirstLoginDaily.inc"
}

// ORIG T 0x0810dfe8：发脚本邮件（AccountFirstLoginScript::m_mail @ +0xc）后
// 向 DB 插入账号每日首次登录标记（GetUID, acc_id, charac_no, flags）。
#include "CEventScriptMng_entitiesB/do_action.inc"
}

}  // namespace EventClassify
