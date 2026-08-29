#ifndef GAME_STBONUSPOINTVALUES_H_
#define GAME_STBONUSPOINTVALUES_H_

// df_game_r stBonusPointValues（排行系统清时奖励点区间元素，2026-08-29）。
// 布局依据 ORIG objdump：
//   ctor C1/C2 0x08370d8c：sub $0x18 后 call clear（clear 外联，P69）；
//   clear 0x08370da0：movl $0,(%eax) 置 m_a，movl $0,0x4(%eax) 置 m_b；
//   无 D1/D2 符号（trivial 析构，P12）。
// sizeof 8：{ int m_a; int m_b; }，与 CDataManager.h stDungeonRankSystem_t
// m_vec18（vector<stBonusPointValues>，+0x18）声明一致。

struct stBonusPointValues
{
    int m_a;   // +0x00
    int m_b;   // +0x04

    stBonusPointValues();   // ORIG C1/C2 0x08370d8c：调用 clear
    void clear();           // ORIG 0x08370da0：两 int 清零
};

#endif // GAME_STBONUSPOINTVALUES_H_
