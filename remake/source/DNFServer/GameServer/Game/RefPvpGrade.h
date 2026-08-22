#ifndef DNF_GAME_RefPvpGrade_H_
#define DNF_GAME_RefPvpGrade_H_

// 迁移自 GameStubs.cpp（batch-6）。PvP 等级参照表访问器。
// 布局按 ORIG getters/setter 反汇编推导：
//   +0x00 int m_lastGrade(GetLastGrade @ 0x85d478e 读 this+0)
//   +0x04 RefPvpGradeData m_data[0x26]（SetPvpGrade @ 0x85d4798
//        写 this+4+grade*8 / this+8+grade*8；8 字节/项，38 项 → 0x130 字节）
//   +0x12c = m_data[37].curr（idx 越界时的默认 curr）
//   +0x130 = m_data[37].next（idx 越界时的默认 next）
// ORIG 符号（T）：
//   _ZNK11RefPvpGrade12GetLastGradeEv        (0x085d478e)
//   _ZNK11RefPvpGrade19GetPvpCurrRankPointEi (0x085d4880)
//   _ZNK11RefPvpGrade19GetPvpNextRankPointEi (0x085d4860)
//   _ZNK11RefPvpGrade11GetPvpGradeEi         (0x085d47ce)
//   _ZNK11RefPvpGrade11GetPvpPointEi         (0x085d4832)

class RefPvpGrade
{
public:
    struct RefPvpGradeData
    {
        int curr; // +0x00
        int next; // +0x04
    };

    int GetLastGrade() const;
    int GetPvpCurrRankPoint(int idx) const;
    int GetPvpNextRankPoint(int idx) const;
    int GetPvpGrade(int idx) const;
    int GetPvpPoint(int idx) const;

private:
    int m_lastGrade;                                  // +0x00
    RefPvpGradeData m_data[0x26];                     // +0x04（38 项 × 8 = 0x130 字节）
};

#endif // DNF_GAME_RefPvpGrade_H_