// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventScriptMng_entitiesB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CEventScriptMng_entitiesB/dtor_CFatigueQuantity.inc"


#include "CEventScriptMng_entitiesB/set_event_data.inc"


#include "CEventScriptMng_entitiesB/get_db_table_name.inc"


#include "CEventScriptMng_entitiesB/get_user_type.inc"


#include "CEventScriptMng_entitiesB/dtor_CExchangeRandomItemReward.inc"


#include "CEventScriptMng_entitiesB/set_event_data_1.inc"


#include "CEventScriptMng_entitiesB/dtor_CAvatarDisjointRandomReward.inc"


#include "CEventScriptMng_entitiesB/set_event_data_2.inc"


#include "CEventScriptMng_entitiesB/dtor_CObjectBringUp.inc"


#include "CEventScriptMng_entitiesB/set_event_data_3.inc"


#include "CEventScriptMng_entitiesB/get_db_table_name_1.inc"


#include "CEventScriptMng_entitiesB/sendEventItemFromFatigue.inc"


#include "CEventScriptMng_entitiesB/dtor_CPurchaseCashItemBonus.inc"


#include "CEventScriptMng_entitiesB/set_event_data_4.inc"


#include "CEventScriptMng_entitiesB/get_db_table_name_2.inc"


#include "CEventScriptMng_entitiesB/updateTargetGoodsCount.inc"


#include "CEventScriptMng_entitiesB/verifyGiveBonusReward.inc"


#include "CEventScriptMng_entitiesB/sendPurchaseBonusReward.inc"


#include "CEventScriptMng_entitiesB/set_event_data_5.inc"


#include "CEventScriptMng_entitiesB/dtor_CAccountFirstLoginDaily.inc"


#include "CEventScriptMng_entitiesB/do_action.inc"


}  // namespace EventClassify
