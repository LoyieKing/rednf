#ifndef DNF_GAME_STAICHARACTERSKILLDATA_H_
#define DNF_GAME_STAICHARACTERSKILLDATA_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + CharacterAIA2.cpp@225（源码定义复制）
// ORIG W dtor _ZN22STAICharacterSkillDataD2Ev 0x088ced32。

// vector 元素 POD；元素步长 0x8 由 vector<T>::push_back construct 步长 objdump 实证
//（W 0x088cc940 区）；字段语义为【推断】，按解析写入顺序命名。
struct STAICharacterSkillData  // 步长 0x8（W 0x088cc940 区）
{
    int m_a;  // ScanInt(&ok)
    int m_b;  // ScanInt(NULL)
};  // 0x8

typedef char stai_character_skill_data_size_check[(sizeof(STAICharacterSkillData) == 0x8) ? 1 : -1];

#endif  // DNF_GAME_STAICHARACTERSKILLDATA_H_
