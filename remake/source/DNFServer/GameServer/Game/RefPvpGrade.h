#ifndef DNF_GAME_RefPvpGrade_H_
#define DNF_GAME_RefPvpGrade_H_

#include <vector>

// PvP 等级参照表（权威布局，按 ORIG 反汇编逐字段确认）。
// 布局：
//   +0x000 int m_lastGrade                         (SetLastGrade @ 0x85d4768 写 this+0;
//                                                    GetLastGrade  @ 0x85d478e 读 this+0)
//   +0x004 RefPvpGradeData m_data[0x26]            (SetPvpGrade @ 0x85d4798 写 this+4+grade*8 /
//                                                    this+8+grade*8；8 字节/项，38 项 → 0x130 字节)
//   +0x134 int m_pvpExpLoss1                       (set @ 0x85d48a0 写 this+0x134 = param+0x10)
//   +0x138 int m_pvpExpLoss2                       (set @ 0x85d48a0 写 this+0x138 = param+0x14)
//   +0x13c int m_rankRateGrade                     (set @ 0x85d48a0 写 this+0x13c = param+0x18)
//   +0x140 std::vector<int> m_rankRate             (ctor @ 0x85d46e6 构造; set @ 0x85d48a0
//                                                    operator= param+0x1c; dtor @ 0x85d470a 析构)
// sizeof = 0x140 + 0x0c (vector<int>) = 0x14c。
// ORIG 符号（T）：
//   _ZN11RefPvpGradeC1Ev                           (0x085d46e6)
//   _ZN11RefPvpGradeD1Ev                           (0x085d470a)
//   _ZN11RefPvpGrade5ClearEv                       (0x085d4722)
//   _ZN11RefPvpGrade12SetLastGradeEi               (0x085d4768)
//   _ZNK11RefPvpGrade12GetLastGradeEv              (0x085d478e)
//   _ZN11RefPvpGrade11SetPvpGradeEiRKNS_15RefPvpGradeDataE (0x085d4798)
//   _ZNK11RefPvpGrade11GetPvpGradeEi               (0x085d47ce)
//   _ZNK11RefPvpGrade11GetPvpPointEi               (0x085d4832)
//   _ZNK11RefPvpGrade19GetPvpNextRankPointEi       (0x085d4860)
//   _ZNK11RefPvpGrade19GetPvpCurrRankPointEi       (0x085d4880)
//   _ZN11RefPvpGrade3setERK15stPvpRefTable_t       (0x085d48a0)

// PvP 参照表（importPvPReferenceTable @ 0x0890857b 填充）。
struct stPvPGradeExp_t
{
    int a; // +0x00
    int b; // +0x04
};

struct stPvpRefTable_t
{
    int m_count;                              // +0x00  "[pvp experience max grade]"
    std::vector<stPvPGradeExp_t> m_grades;    // +0x04  grades vector
    int m_pvpExpLoss1;                        // +0x10  "[pvp experience loss]" v1
    int m_pvpExpLoss2;                        // +0x14  "[pvp experience loss]" v2
    int m_rankRateGrade;                      // +0x18  "[pvp rank rate grade]"
    std::vector<int> m_rankRate;              // +0x1c  "[pvp rank rate]"
    std::vector<int> m_recordOrder;           // +0x28  "[pvp record order]"
};

class RefPvpGrade
{
public:
    struct RefPvpGradeData
    {
        int curr; // +0x00
        int next; // +0x04
    };

    RefPvpGrade();
    ~RefPvpGrade();

    void Clear();
    int SetLastGrade(int grade);
    int GetLastGrade() const;
    int SetPvpGrade(int grade, const RefPvpGradeData& data);
    int GetPvpGrade(int idx) const;
    int GetPvpPoint(int idx) const;
    int GetPvpNextRankPoint(int idx) const;
    int GetPvpCurrRankPoint(int idx) const;
    void set(const stPvpRefTable_t& refTable);

private:
    int m_lastGrade;                        // +0x000
    RefPvpGradeData m_data[0x26];           // +0x004（38 项 × 8 = 0x130 字节）
    int m_pvpExpLoss1;                      // +0x134
    int m_pvpExpLoss2;                      // +0x138
    int m_rankRateGrade;                    // +0x13c
    std::vector<int> m_rankRate;            // +0x140
};

#endif // DNF_GAME_RefPvpGrade_H_
