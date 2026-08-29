#ifndef GAME_MYRANDOM_H_
#define GAME_MYRANDOM_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 实现依据 docs/class_func_reports 与 df_game_r ORIG 反汇编：
//   _ZN8MyRandomC1Ei (0x8aafe18, 0xd)   this->m_seed = seed
//   _ZN8MyRandomclEi (0x8aafe26, 0x14)  sar $0x1f + idivl n → 返回 m_seed % n（带符号余数）
// 用于 std::random_shuffle<..., MyRandom> 的随机数生成器（返回 [0,n)）。

class MyRandom
{
public:
    // ORIG 0x8aafe18 _ZN8MyRandomC1Ei (size 0x0000000d)
    MyRandom(int seed);
    // ORIG 0x8aafe26 _ZN8MyRandomclEi (size 0x00000014)  非 const 成员（mangled 无 K）
    int operator()(int n);
private:
    int m_seed;
};

#endif  // GAME_MYRANDOM_H_
