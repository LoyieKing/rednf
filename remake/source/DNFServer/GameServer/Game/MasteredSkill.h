#ifndef DNF_GAME_MASTERED_SKILL_H_
#define DNF_GAME_MASTERED_SKILL_H_

// ============================================================================
// _Mastered_skill —— 已学技能（2 字节：索引 + 等级）
// 自 SkillSlot.h 抽出（SigTypes_C.h 的 TAG_CHARAC_ETC_DATA / SIG_LOAD_SKILL
// 数组成员构造需完整类型，且 SkillSlot.h 依赖 CUser.h 存在包含环）。
// _Mastered_skill() 定义于 SkillSlot.cpp（ORIG T 0x0848f34c 同址 C1/C2）。
// ============================================================================

struct _Mastered_skill
{
    _Mastered_skill();
    unsigned char m_index;   // +0x00
    char m_level;            // +0x01
};

#endif  // DNF_GAME_MASTERED_SKILL_H_
