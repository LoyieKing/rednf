// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + CharacterAIA2.cpp
// STAICharacterSkillCorrection：ORIG 弱内联 ctor W @0x088cc30e、dtor W @0x088cf1b4。
// has_def=1：完整复制定义自 CharacterAIA2.cpp:245（{m_skill, m_rate} 0x8 字节，ctor 置 {0, 1.0f}）。
#ifndef DNF_GAME_STAICHARACTERSKILLCORRECTION_H_
#define DNF_GAME_STAICHARACTERSKILLCORRECTION_H_

struct STAICharacterSkillCorrection
{
    int   m_skill;   // +0x00
    float m_rate;    // +0x04
    STAICharacterSkillCorrection() : m_skill(0), m_rate(1.0f) {}   // ORIG W @0x088cc30e
};  // sizeof = 0x8【推断：源码现布局，未对照 ORIG 尺寸证据，故不加 static_assert】

#endif  // DNF_GAME_STAICHARACTERSKILLCORRECTION_H_
