#ifndef DNF_GAME_STSUMMONAPC_H_
#define DNF_GAME_STSUMMONAPC_H_

// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// ORIG W 符号：C1 0x089c3238 / clear 0x089c324c。
// clear 返回类型 nm 不可见 → void【推断】（clear 常规签名）。

struct STSummonApc
{
    STSummonApc();      // W 0x089c3238
    void clear();       // W 0x089c324c【推断 void】
};
// sizeof 未证实，不加布局断言。

#endif  // DNF_GAME_STSUMMONAPC_H_
