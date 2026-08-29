#ifndef GAME_RANDOMMESSAGEBALLOONINFO_H_
#define GAME_RANDOMMESSAGEBALLOONINFO_H_

#include <string>
#include <vector>

// ---- randomMessageBalloonInfo（ORIG C1/clear T 0x08a00c08/0x08a00bd2 → cpp
//      定义；D1 W 0x08581d50 → 编译器隐式析构，cpp 强制实例化）----
// 布局（clear/C1 实证）：std::string@+0x00 + int@+0x04 + std::vector<std::string>
// @+0x08，sizeof 0x14。无 vtable。
// C1：string 默认构造 → vector 默认构造 → clear()；
// clear：+0x04 置 0 + vector::clear + vector::reserve(0xa)；
// D1：vector D1 → string D1（隐式，ORIG 无 D2，勿写用户析构）。
// 本头独占（仅 randomMessageBalloonInfo.cpp include），不进入共享头。
// 报告: docs/class_func_reports/randomMessageBalloonInfo
class randomMessageBalloonInfo
{
public:
    // ORIG 0x08a00c08 _ZN24randomMessageBalloonInfoC1Ev
    randomMessageBalloonInfo();
    // ORIG 0x08a00bd2 _ZN24randomMessageBalloonInfo5clearEv
    void clear();

    std::string m_randomMessageAnimation;      // +0x00
    int m_randomMessageProbability;            // +0x04
    std::vector<std::string> m_randomMessageList;  // +0x08
};   // sizeof 0x14

#endif  // GAME_RANDOMMESSAGEBALLOONINFO_H_
