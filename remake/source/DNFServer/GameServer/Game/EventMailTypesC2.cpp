// ============================================================================
// EventMailTypesC2.cpp —— 事件邮件/升级条件剩余生命周期恢复（独占 TU；
// EventMailTypesC2 批次）。
//
// 指派符号（六函数在 ORIG 均为 W，nm 实证地址如下；无独立函数报告——
// 【报告缺失】情形：编辑前已完整阅读调用者/同类报告 ## 汇编+## 反编译 C 全文
//   docs/class_func_reports/InGameEventScriptLoader/importInGameEventScript.md
//   docs/class_func_reports/EventClassify__CLevelUpReward/{README,
//   check_recv_level,set_reward_item,send_reward_from_recv_level,do_action,
//   get_mail_data,get_db_table_name,is_recv_check_item,set_event_data}.md，
// 之后按 ORIG 地址 objdump 逐指令转写）：
//   1. Action_SendMail::Action_SendMail()               C1/C2 W 0x088e40ce
//   2. Action_SendMail::~Action_SendMail()              D1/D2 W 0x088e4160
//   3. Condition_LevelUp::Condition_LevelUp()           C1/C2 W 0x0898605a
//   4. Condition_LevelUp::~Condition_LevelUp()          D1/D2 W 0x08986070
//   5. Condition_LevelUp::Reward_Item::Reward_Item()    C1/C2 W 0x08986086
//   6. Condition_LevelUp::Reward_Item::~Reward_Item()   D1/D2 W 0x089860d2
//
// 明确不含（防重复定义）：
//   - Action_SendMail::reset / set_item、InGameEventScript C1/D1、
//     LevelupSupport(2nd)EventManger C1/D1、check_recv_level/set_reward_item/
//     importInGameEventScript —— 已由 EventLevelupA2.cpp 提供；
//   - 11 个 *Script 类 C1、ExchangeRandomItemRewardScript::reset、
//     ObjectBringUpRule C1/reset、import* 解析器 —— 已由 EventScriptsCompleteB2.cpp
//     提供。各 *Script 类析构在 ORIG 无符号（importInGameEventScript 失败路径仅
//     非虚直调基类 ~InGameEventScript 后 operator delete），故维持头文件声明、
//     本仓亦不提供定义（与 ORIG 符号集一致）。
// ============================================================================

#include "InGameEventScripts.h"   // ::Action_SendMail / Condition_LevelUp 权威布局

// ============ Action_SendMail C1/D1【报告缺失·objdump 转写】 ============
// C1 0x088e40ce：_ZNSt3mapIiSt4pairIiiESt4lessIiESaIS0_IKiS1_EEEC1Ev(this) →
//   _ZNSsC1Ev(this+0x30) → _ZNSsC1Ev(this+0x34) → _ZNSt6vectorIiSaIiEEC1Ev(this+0x38)。
//   忠实保留：+0x18..+0x2f 标量区（m_field18/m_field1c/m_itemIdx/m_addInfo/
//   m_gold/m_coin/pad）不初始化——由 reset()（0x080ec026）或各 import* 解析器写入。
// D1 0x088e4160：逆序析构 vector(+0x38) → string(+0x34) → string(+0x30) → map(+0x00)。
Action_SendMail::Action_SendMail()
    : m_jobMap(),
      m_field30(),
      m_field34(),
      m_vec38()
{
}

Action_SendMail::~Action_SendMail()
{
}

// ============ Condition_LevelUp C1/D1【报告缺失·objdump 转写】 ============
// C1 0x0898605a：仅 _ZNSt6vectorIN17Condition_LevelUp11Reward_ItemsaIS1_EEC1Ev(this+4)；
//   忠实保留：m_filter[4]（+0x00..+0x03）不初始化（解析器写入；+1 字节为 growType 过滤）。
// D1 0x08986070：仅析构该 vector。
Condition_LevelUp::Condition_LevelUp()
    : m_items()
{
}

Condition_LevelUp::~Condition_LevelUp()
{
}

// ============ Condition_LevelUp::Reward_Item C1/D1【报告缺失·objdump 转写】 ======
// C1 0x08986086：_ZNSsC1Ev(this+0xc) → _ZNSsC1Ev(this+0x10)；忠实保留：
//   m_level/m_itemIndex/m_itemCount（+0x00..+0x0b 三个 int）不初始化。
// D1 0x089860d2：逆序析构 string(+0x10) → string(+0xc)。
Condition_LevelUp::Reward_Item::Reward_Item()
    : m_title(),
      m_content()
{
}

Condition_LevelUp::Reward_Item::~Reward_Item()
{
}
