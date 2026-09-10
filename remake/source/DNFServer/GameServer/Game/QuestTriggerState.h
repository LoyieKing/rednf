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
//
// [位域建模 2026-09-10，E04] 字段由 `short` 改「16 位容器 9 位位域 + 同址 int 视图」，
// 依据 ORIG 指令形（本 TU 四函数与 Quest.cpp 的 setTrigger 一致）：
//   读：`movzwl (%eax),%eax; andw $0x1ff,%ax; movzwl %ax,%eax`（16 位 and 后零扩展）；
//   写：`movzwl 旧容器; andw $0xfe00,%ax; or 新值; mov %ax,容器`（保留高 7 位）；
//   operator= 解包：`and $0x3fe00,%eax; sar $0x9` / `and $0x7fc0000,%eax; sar $0x12`
//   ⇒ 原码是 (v & (0x1ff<<9)) >> 9 形；subTriggerState v==0 对 +0x00 的 4 字节自减
//   （`mov (%eax),%eax; lea -1(%eax),%edx; mov %edx,(%eax)`）⇒ 需 m_low32 视图。
// 双编译器探针 /tmp/qts/b.cpp（4.4.7）与 ORIG 逐条一致；4.1.2 同形（/tmp/qts/p2_41.s m8）。
// sizeof 8 / 偏移 0,2,4,6 / 对齐 2 与 short 版逐值不变；7 个符号 mangled 全不变。
// operator= 返回类型引用→void：返回类型不入 mangled（`aSERKi`），ORIG 0x086ad56c 尾部
// `pop %ebp; ret` 无 eax 装载（void 形）；全部调用点为语句式（UserQuest.cpp `state = m_trigger[i];`）。
#define ST_QUEST_TRIGGER_STATE_WEAK __attribute__((weak))

struct stQuestTriggerState_t
{
    stQuestTriggerState_t() ST_QUEST_TRIGGER_STATE_WEAK;
    void setTrigger(short a, short b, short c) ST_QUEST_TRIGGER_STATE_WEAK;
    int getTrigger() ST_QUEST_TRIGGER_STATE_WEAK;
    void operator=(int const& v) ST_QUEST_TRIGGER_STATE_WEAK;
    void incTirggerState(char v) ST_QUEST_TRIGGER_STATE_WEAK;
    void subTriggerState(char v) ST_QUEST_TRIGGER_STATE_WEAK;

    union
    {
        struct
        {
            unsigned short m_trigger0 : 9;   // +0x00 bit0..8
            unsigned short m_reserved0 : 7;  // +0x00 bit9..15（ORIG `and $0xfe00` 保留位）
            unsigned short m_trigger1 : 9;   // +0x02 bit0..8
            unsigned short m_reserved1 : 7;  // +0x02 bit9..15
        };
        int m_low32;                         // +0x00 的 4 字节视图（subTriggerState v==0）
    };
    unsigned short m_trigger2 : 9;           // +0x04 bit0..8
    unsigned short m_reserved2 : 7;          // +0x04 bit9..15
    short m_trigger3;                        // +0x06（保留；ORIG C1/operator= 2×movl 清 8 字节）
};

static_assert(sizeof(stQuestTriggerState_t) == 8,
              "stQuestTriggerState_t ABI layout drift");

#undef ST_QUEST_TRIGGER_STATE_WEAK

#endif  // GAME_QUEST_TRIGGER_STATE_H_
