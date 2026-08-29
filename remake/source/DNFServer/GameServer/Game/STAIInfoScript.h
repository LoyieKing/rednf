// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG +
// docs/class_func_reports/STAIInfoScript/{STAIInfoScript,operator=}.md
// STAIInfoScript：ORIG C1 W @0x08366f48 / C2（copy T @0x08a38bc6）/ D1 W @0x083671a4 /
// clear W @0x083673d4 / destroy W @0x0836753a / operator= T @0x08a38e34。
// 多态（ORIG _ZTV/_ZTI/_ZTS 存在）；sizeof 0x224 由 CharacterAIA2.cpp 断言实证。
// has_def=1：完整复制定义自 CharacterAIA2.cpp:300。ctor/copy/赋值体仍由
// CharacterAIA2.cpp 提供（本头不改任何 cpp）。
#ifndef DNF_GAME_STAIINFOSCRIPT_H_
#define DNF_GAME_STAIINFOSCRIPT_H_

#include <string>
#include <vector>

class STAIInfoScript
{
public:
    STAIInfoScript();                          // ORIG W @0x08366f48（体：CharacterAIA2.cpp）
    STAIInfoScript(STAIInfoScript const&);     // ORIG T @0x08a38bc6
    virtual ~STAIInfoScript();                 // 发 D1/D0/vtable（ORIG D1 @0x083671a4）
    STAIInfoScript& operator=(STAIInfoScript const&);  // ORIG T @0x08a38e34
    void clear();                              // ORIG W @0x083673d4
    void destroy();                            // ORIG W @0x0836753a

    int m_thinkTerm;                    // +0x04
    int m_destinationChangeTerm;        // +0x08
    std::string m_dir;                  // +0x0c
    std::vector<std::string*> m_patterns[5];  // +0x10 easy/medium/hard/ultimate/hero
    int m_targetingBonus[22];           // +0x4c
    std::vector<int> m_targetingBonusStr[22]; // +0xa4
    int m_keepDistance;                 // +0x1ac
    std::vector<int> m_keepRange[5];    // +0x1b0/+0x1bc/+0x1c8/+0x1d4/+0x1e0
    int m_intelligence;                 // +0x1ec
    int m_field1f0;                     // +0x1f0
    int m_field1f4;                     // +0x1f4
    int m_vision;                       // +0x1f8
    int m_backVision;                   // +0x1fc
    bool m_noChangeTarget;              // +0x200
    char m_pad201[3];
    int  m_pod204[8];                   // +0x204..+0x220（外层 clear 清零）
};  // sizeof == 0x224
typedef char assert_stai_info_size_h[(sizeof(STAIInfoScript) == 0x224) ? 1 : -1];

#endif  // DNF_GAME_STAIINFOSCRIPT_H_
