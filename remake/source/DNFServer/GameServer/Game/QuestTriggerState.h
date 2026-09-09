#ifndef GAME_QUEST_TRIGGER_STATE_H_
#define GAME_QUEST_TRIGGER_STATE_H_

// stQuestTriggerState_t —— 任务触发状态（8 字节：3 个 9 位触发值打包于
// +0x00/+0x02/+0x04，高 7 位保留）。
//
// [§4.4 合并 2026-09-10] 唯一定义点。此前 Quest.h（struct 定义）与
// QuestTriggerState.cpp（class 镜像定义，方法声明序不同且无 setTrigger）各一份，
// 逐 token 不同 ⇒ ODR 违规；成员布局（4×short / 8 字节）逐字段一致，合并零 ABI 变化。
// 本头刻意零 include（消费方 Quest.h 体量大，QuestTriggerState.cpp 仅需本类型）。
//
// 权威符号（ORIG df_game_r，全部 W 弱符号）：
//   stQuestTriggerState_t::stQuestTriggerState_t()        W @0x0836ddac  (QuestTriggerState.cpp)
//   stQuestTriggerState_t::setTrigger(short,short,short)  W @0x0836dce0  (Quest.cpp)
//   stQuestTriggerState_t::getTrigger()                   W @0x0836dd68  (QuestTriggerState.cpp)
//   stQuestTriggerState_t::operator=(int const&)          W @0x086ad56c  (QuestTriggerState.cpp)
//   stQuestTriggerState_t::incTirggerState(char)          W @0x086ad602  (QuestTriggerState.cpp)
//   stQuestTriggerState_t::subTriggerState(char)          W @0x086ad6ea  (QuestTriggerState.cpp)

// [符号绑定判据 2026-09-10] ORIG 7 个符号全为 **W 弱绑定**（nm -S 实证：C1/C2
// @0x0836ddac、setTrigger @0x0836dce0、getTrigger @0x0836dd68、operator=
// @0x086ad56c、inc @0x086ad602、sub @0x086ad6ea，均 W），即原码为类内 inline 定义；
// 我方原为 T 强绑定（外联定义于 QuestTriggerState.cpp / Quest.cpp）。
// 在声明处标 weak 复现同一绑定：定义点不变（本 TU 仍发射实体，四数/符号集零变化），
// Quest.cpp 的 setTrigger 定义同样随之落 W。mangled/sizeof/布局零影响。
#define ST_QUEST_TRIGGER_STATE_WEAK __attribute__((weak))

struct stQuestTriggerState_t
{
    stQuestTriggerState_t() ST_QUEST_TRIGGER_STATE_WEAK;
    void setTrigger(short a, short b, short c) ST_QUEST_TRIGGER_STATE_WEAK;
    int getTrigger() ST_QUEST_TRIGGER_STATE_WEAK;
    stQuestTriggerState_t& operator=(int const& v) ST_QUEST_TRIGGER_STATE_WEAK;
    void incTirggerState(char v) ST_QUEST_TRIGGER_STATE_WEAK;
    void subTriggerState(char v) ST_QUEST_TRIGGER_STATE_WEAK;

    short m_trigger0;  // +0x00
    short m_trigger1;  // +0x02
    short m_trigger2;  // +0x04
    short m_trigger3;  // +0x06（保留；ORIG C1/operator= 2×movl 清 8 字节实证）
};

#undef ST_QUEST_TRIGGER_STATE_WEAK

#endif  // GAME_QUEST_TRIGGER_STATE_H_
