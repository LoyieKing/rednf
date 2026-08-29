#ifndef GAME_STCLEARTIMEAVERAGE_H_
#define GAME_STCLEARTIMEAVERAGE_H_

// df_game_r 排行系统清时平均结构（2026-08-29）。
// 布局依据 ORIG objdump（对照 df_game_r 0x0891b6a8 起）：
//   stClearTimeAverage：ctor C1/C2 0x0891b6a8（sub $0x18 后 call clear，P69 外联）；
//     clear 0x0891b6bc：5× movl $0 逐字段（+0/+4/+8/+0xc/+0x10）；无 D1/D2（trivial，P12）。
//   stClearTimeAverageMapKey：ctor C1/C2 0x08370d1c（sub $0x18 后 call clear）；
//     clear 0x08370d30：2× movl $0（+0/+4）；operator< 0x08370d48（W 头内联：
//     先比 m_a、相等再比 m_b 的分支链，P56 不直 return 表达式）。
// sizeof stClearTimeAverage = 0x14，stClearTimeAverageMapKey = 8。

struct stClearTimeAverage
{
    int m_v[5];   // +0x00..0x13

    stClearTimeAverage();   // ORIG C1/C2 0x0891b6a8：调用 clear
    void clear();           // ORIG 0x0891b6bc：5 int 清零
};

struct stClearTimeAverageMapKey
{
    int m_a;   // +0x00
    int m_b;   // +0x04

    stClearTimeAverageMapKey();   // ORIG C1/C2 0x08370d1c：调用 clear
    void clear();                 // ORIG 0x08370d30：两 int 清零
    bool operator<(const stClearTimeAverageMapKey& o) const;   // ORIG W 0x08370d48
};

#endif // GAME_STCLEARTIMEAVERAGE_H_
