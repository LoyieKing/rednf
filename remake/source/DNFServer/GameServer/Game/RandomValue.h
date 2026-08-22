#ifndef DNF_GAME_RANDOMVALUE_H_
#define DNF_GAME_RANDOMVALUE_H_

#include <vector>

// 迁移自 GameStubs.cpp CRandomValue 桩（batch-5）。
// ORIG 弱符号：
//   _ZN12CRandomValueC1ERSt6vectorIiSaIiEEiii   (ctor, 0x085df7fe)
//   _ZN12CRandomValueclEv                        (operator(), 0x085df826, 非 const)
class CRandomValue
{
public:
    CRandomValue(std::vector<int>& list, int a, int b, int c);
    int operator()();
};

#endif  // DNF_GAME_RANDOMVALUE_H_