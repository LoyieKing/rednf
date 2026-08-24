#ifndef DNF_GAME_RANDOMVALUE_H_
#define DNF_GAME_RANDOMVALUE_H_

#include <vector>

// CRandomValue（ORIG 弱符号）：
//   _ZN12CRandomValueC1ERSt6vectorIiSaIiEEiii   (ctor, 0x085df7fe)
//   _ZN12CRandomValueclEv                        (operator(), 0x085df826, 非 const)
// 布局（ORIG 16 字节）：+0x00 std::vector<int>&（存引用对象地址）、
// +0x04 int、+0x08 int、+0x0c int（尝试次数）。
class CRandomValue
{
public:
    CRandomValue(std::vector<int>& list, int a, int b, int c);
    int operator()();

private:
    std::vector<int>& m_list;  // +0x00 排除值列表
    int m_b;                   // +0x04（ctor 第 4 参）
    int m_a;                   // +0x08（ctor 第 3 参，随机偏移基址）
    int m_tryCount;            // +0x0c（ctor 第 5 参，最大重试次数）
};

#endif  // DNF_GAME_RANDOMVALUE_H_
