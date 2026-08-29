// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG 0x088cf8ca (D1 W) + 提取自 CharacterAIA2.cpp:237 定义段（未改 cpp）。
#ifndef DNF_GAME_STAICHARACTERCUSTOMSKILLLEVEL_H_
#define DNF_GAME_STAICHARACTERCUSTOMSKILLLEVEL_H_

// 步长 0x10（W 0x088ccb62 区）；ScanInt 解析 4 个 int 字段。
// ORIG D1 0x088cf8ca 为 trivial 弱符号（隐式析构），头内不显式声明。
struct STAICharacterCustomSkillLevel
{
    int m_a;  // ScanInt(&ok)
    int m_b;  // ScanInt(NULL)
    int m_c;  // ScanInt(NULL)
    int m_d;  // ScanInt(NULL)
};  // 0x10
typedef char assert_staichar_customskilllevel_size[(sizeof(STAICharacterCustomSkillLevel) == 0x10) ? 1 : -1];

#endif  // DNF_GAME_STAICHARACTERCUSTOMSKILLLEVEL_H_
