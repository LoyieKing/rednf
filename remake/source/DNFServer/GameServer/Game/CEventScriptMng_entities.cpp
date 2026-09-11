#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CEventScriptMng_entities/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ EventClassify 事件实体第一组实现。
// 建模并实现以下 CEventEntity 派生类（ORIG 均为 0x20 字节：
//   基类 CEventEntity 0x1c + InGameEventScript* m_script1c(+0x1c)）。
//   - CLevelUpReward      ← 事件记录类型 0x194
//   - CUsedFatigueGiveItem← 事件记录类型 0x65
//   - CAttendance         ← 事件记录类型 0x56
//   - CPcRoomPlayTime     ← 事件记录类型 0x5c
//   - CAccountFirstLogin  ← 事件记录类型 0x191/0x192
//   - CAddItemCountDB     ← 事件记录类型 0x193
//
// 脚本绑定：m_script1c 指向的具体脚本类型由记录类型唯一确定（ORIG nm W 符号：
// 六个派生类 ctor 均为隐式生成，仅调基类 C1 + 设 vptr；对象由 create_event_entity
// value-init：operator new(0x20) → 整块清零 → C1 thunk）。除 CAccountFirstLogin
// （entitiesB 的派生类需显式调用基类 ctor，故保留声明并补 m_script1c=0 对齐工厂
// 清零终态）外均不声明 ctor，由编译器同样产生清零循环 + 弱 C1。
//
// 脚本访问一律经 InGameEventScripts.h 权威布局的具名字段（禁止裸偏移）：
//   0x194→LevelUpRewardScript        0x65→UsedFatigueGiveItemScript
//   0x56→AttendanceScript            0x5c→PcBangPlayTimeScript
//   0x191/0x192→AccountFirstLoginScript    0x193→AddItemCountDBScript

#include "CEventScriptMng.h"

#include "PacketGuard.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "CEnvironment.h"
#include "LogManager.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager（GetRepeatEvent）

#include <cstring>
#include <vector>

#include "InGameEventScripts.h"         // 具体事件脚本类型（权威布局）
// 依赖类由事件系统提供（FixEventScriptDeps）：
#include "EventClassify_Action.h"       // EventClassify::Action_SendMail / CEventActionMng
#include "DB_InsertAccountFirstLoginFlag.h"
#include "GameTypes.h"                  // cMyTrace

#include "CEventScriptMng_entities/isSetBit_Uint.inc"
// ORIG T 0x0810904b（声明 CInventory.h/SkillSlot.h）：
char* NumberToString(unsigned int value, int radix);

namespace EventClassify
{

// process_action_send_mail 形参用的是 EventClassify_Action.h 的命名空间内
// Action_SendMail；脚本子对象 m_mail 为全局 ::Action_SendMail（ORIG 符号口径，
// 见 InGameEventScripts.h）。两者逐字段同布局（sizeof 均 0x44）。用宏桥接：
// -O0 下 inline 函数会产生多余调用帧（ORIG 无）。
typedef char dnf_mail_layout_assert_[
    (sizeof(::Action_SendMail) == sizeof(Action_SendMail)) ? 1 : -1];
#define EVENT_MAIL_REF(mail) (*reinterpret_cast<Action_SendMail*>(&(mail)))

#include "CEventScriptMng_entities/do_action.inc"

 
#include "CEventScriptMng_entities/set_event_data.inc"


#include "CEventScriptMng_entities/get_db_table_name.inc"


#include "CEventScriptMng_entities/is_recv_check_item.inc"


#include "CEventScriptMng_entities/get_mail_data.inc"


#include "CEventScriptMng_entities/send_reward_from_recv_level.inc"

#include "CEventScriptMng_entities/is_eventing.inc"


#include "CEventScriptMng_entities/verifyApplyUser.inc"


#include "CEventScriptMng_entities/get_db_table_name_1.inc"


#include "CEventScriptMng_entities/set_event_data_1.inc"


#include "CEventScriptMng_entities/getIsCharacEvent.inc"


#include "CEventScriptMng_entities/sendEventItemFromFatigue.inc"


// ============ CAttendance（0x56 → AttendanceScript）============

#include "CEventScriptMng_entities/is_eventing_1.inc"


#include "CEventScriptMng_entities/set_event_data_2.inc"


#include "CEventScriptMng_entities/send_init_data.inc"


#include "CEventScriptMng_entities/get_db_table_name_2.inc"


#include "CEventScriptMng_entities/get_mail_data_1.inc"


#include "CEventScriptMng_entities/get_fatigue_condition.inc"


#include "CEventScriptMng_entities/get_final_day_condition.inc"


#include "CEventScriptMng_entities/send_attend_reward_item.inc"


#include "CEventScriptMng_entities/send_attend_final_rward_item.inc"


// ============ CPcRoomPlayTime（0x5c → PcBangPlayTimeScript）============

#include "CEventScriptMng_entities/is_eventing_2.inc"


#include "CEventScriptMng_entities/set_event_data_3.inc"


#include "CEventScriptMng_entities/get_db_table_name_3.inc"


#include "CEventScriptMng_entities/verifyUserPcRoomPlayEvent.inc"


// ============ CAccountFirstLogin（0x191/0x192 → AccountFirstLoginScript）============

// [b28d 单文件泳道] ctor 定义删除：ORIG 的 C1 为隐式 W（0x08110f88，仅
// call CEventEntity::C1 + 写 vptr），用户提供 ctor 会令 4.4 对 `new T()` 不做
// value-init 前置清零（CWG434/N77）——删后由编译器隐式合成，工厂 12 个 case
// 的 value-init 清零循环与 ORIG 逐条对齐。

#include "CEventScriptMng_entities/set_event_data_4.inc"


#include "CEventScriptMng_entities/get_db_table_name_4.inc"


#include "CEventScriptMng_entities/get_mail_data_2.inc"
{
    // ORIG 0x810decc：return &m_mail。
    return reinterpret_cast<char*>(
        &static_cast<AccountFirstLoginScript*>(m_script1c)->m_mail);
}

#include "CEventScriptMng_entities/do_action_1.inc"
{
    // ORIG 0x810df04：发首登奖励邮件并请求 DB 落首登标记（仅判 user 空指针）。
    // 服务器组：PvP 统合频道取 GetServerGroup 低字节，否则取 CEnvironment +0x378
    // 低字节（共享头暂无命名字段，用组件视图复现 ORIG 单指令 disp 加载）。
    if (user == 0)
        return;
    CEventActionMng mng;
    mng.process_action_send_mail(
        user,
        EVENT_MAIL_REF(static_cast<AccountFirstLoginScript*>(m_script1c)->m_mail),
        true);

    struct CEnvironment378View
    {
        char         m_pad378[0x378];
        unsigned int m_serverGroup378;   // +0x378
    };
    int serverGroup = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverGroup = (unsigned char)user->GetServerGroup();
    else
        serverGroup = (unsigned char)
            ((CEnvironment378View*)G_CEnvironment())->m_serverGroup378;

    DB_InsertAccountFirstLoginFlag::makeRequest(
        (int)user->GetUID(), user->get_acc_id(), serverGroup,
        (unsigned int)user->getCurCharacNo(), flags);
}

// ============ CAddItemCountDB（0x193 → AddItemCountDBScript）============

#include "CEventScriptMng_entities/set_event_data_5.inc"


#include "CEventScriptMng_entities/get_db_table_name_5.inc"


#include "CEventScriptMng_entities/is_event_item.inc"


#include "CEventScriptMng_entities/is_event_quest.inc"


#include "CEventScriptMng_entities/create_event_entity.inc"

}  // namespace EventClassify
