// stBloodWarBgmTable —— 血之战争 BGM 脚本结构（ORIG 顶层类型）。
// ORIG（mangled _ZN18stBloodWarBgmTable...）：
//   C1 W 0x0891a95c、C1ERKS_ W 0x0892d3b6、D1 W 0x0891c9fc、operator= W 0x0892d464、
//   clear W 0x0891a9a4（+0x00 int=-1 + string clear）。
// importBloodWarEtcScript（T 0x0890e3f8）[BGM] 段实证：+0x00 int（ScanInt id）、
//   +0x04 string（ScanStr 名）；sizeof 0x8。
#ifndef DNF_GAME_STBLOODWARBGMTABLE_H_
#define DNF_GAME_STBLOODWARBGMTABLE_H_

#include <string>

struct stBloodWarBgmTable
{
    stBloodWarBgmTable();                            // W 0x0891a95c
    stBloodWarBgmTable(stBloodWarBgmTable const&);   // W 0x0892d3b6
    ~stBloodWarBgmTable();                           // W 0x0891c9fc
    stBloodWarBgmTable& operator=(stBloodWarBgmTable const&);  // W 0x0892d464
    void clear();                                    // W 0x0891a9a4

    int m_id;          // +0x00
    std::string m_name;  // +0x04
};  // sizeof 0x8

// ---- importBloodWarEtcScript 依赖（同脚本族 ORIG 顶层类型，本头独占承载）----
// stBloodPoint：clear W 0x0891a9d8（+0x00=4、+0x04/+0x08/+0x0c=0，4×int）；
//   [BLOOD POINT] 段每条 {类型码 0..3, v1, v2, v3}。
struct stBloodPoint
{
    stBloodPoint();   // W 0x0891a9c4（= clear）
    void clear();     // W 0x0891a9d8

    int m_type;   // +0x00
    int m_v1;     // +0x04
    int m_v2;     // +0x08
    int m_v3;     // +0x0c
};  // sizeof 0x10

// stUltimateWarMonsterInfo：clear W 0x0891aa18（+0x00/+0x04/+0x08 int=0）；
//   [BLOOD INFO] 段每条 {v0, v1, v2}。
struct stUltimateWarMonsterInfo
{
    stUltimateWarMonsterInfo();   // W 0x0891aa04（= clear）
    void clear();                 // W 0x0891aa18

    int m_field0;   // +0x00
    int m_field4;   // +0x04
    int m_field8;   // +0x08
};  // sizeof 0xc

#endif  // DNF_GAME_STBLOODWARBGMTABLE_H_
