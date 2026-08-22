#ifndef DNF_GAME_ST_EXPERT_JOB_SCRIPT_H_
#define DNF_GAME_ST_EXPERT_JOB_SCRIPT_H_

// df_game_r STExpertJobScript —— 唯一声明点（CDataManagerScripts.cpp 实现；
// CUser::GetCurExpertJobLevel 真实调用）。
// ORIG 符号（nm -S --defined-only，均为 W）：
//   _ZN17STExpertJobScript18isBoundaryExpValueEi  0x0849f420
//   _ZN17STExpertJobScript8GetLevelEj             0x08693ba0
// 仅建模 CUser 使用的两个查询方法；ORIG 类体更大（ctor 0x0896e312 等），
// 其余方法暂未建模。布局：std::vector<ExpertJobExp> @ +0x00（元素尺寸 12，
// 匹配 ORIG _ZNSt6vectorI12ExpertJobExpSaIS0_EEixEj 的 i*12 乘法）。
// [推断]：类最小声明，字段/方法按上述两个 ORIG 访问器反汇编推导。

#include <vector>

// 专家职业经验表项（0x0c 字节；ORIG operator[] i*12）。
struct ExpertJobExp
{
    int m_exp;    // +0x00
    int m_field4; // +0x04
    int m_field8; // +0x08
};

class STExpertJobScript
{
public:
    bool isBoundaryExpValue(int exp);
    int GetLevel(unsigned int exp);

    std::vector<ExpertJobExp> m_expVec;  // +0x00
};

#endif  // DNF_GAME_ST_EXPERT_JOB_SCRIPT_H_
