// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG 0x0848fae2 (C1 W), nm-only.
// 布局（ORIG C1 0x0848fae2 反汇编实证）：movl $0,(%eax) + SIG_BEST_CLEAR_TIME_DIFFICULTY C1(+4)；
// sizeof 0x18（SIG_BEST_CLEAR_TIME::m_data[1000] 步进 0x18 交叉验证）。
#ifndef DNF_GAME_SIG_BEST_CLEAR_TIME_DATA_H_
#define DNF_GAME_SIG_BEST_CLEAR_TIME_DATA_H_

#include "BestClearTime.h"  // SIG_BEST_CLEAR_TIME_DIFFICULTY（20 字节，memset 0x14）

class SIG_BEST_CLEAR_TIME_DATA
{
public:
    SIG_BEST_CLEAR_TIME_DATA() : m_field0(0) {}  // W 0x0848fae2（m_diff 随后隐式构造）

    int m_field0;                     // +0x00
    SIG_BEST_CLEAR_TIME_DIFFICULTY m_diff;  // +0x04（0x14 字节）
};  // sizeof 0x18

#endif  // DNF_GAME_SIG_BEST_CLEAR_TIME_DATA_H_
