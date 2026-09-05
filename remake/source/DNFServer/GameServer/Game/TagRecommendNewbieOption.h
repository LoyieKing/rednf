// TagRecommendNewbieOption —— 新手推荐选项脚本结构（ORIG 顶层类型）。
// ORIG（mangled _ZN24tagRecommendNewbieOption...）：
//   C1 W 0x0891ad16（0x13）：仅调 clear()。
//   C2 W 0x0891ad16（同址）、D1/D2 W 0x0893908c（0x5，trivial 空体）。
//   clear W 0x0891ad2a（0x61）：+0x00 int=-1，+0x70..+0xd8 int[9][3] 全置 -1
//     （外层 i 0..8 / 内层 j 0..2，索引 3*i+j，movl $0xffffffff,(%ecx,%eax,4)）。
// importRecommendNewbieOptionScript（T 0x0890457c）实证：
//   +0x00 = [grow type] 职业索引（default -1；"ranger" 命中为 5）；
//   +0x04..+0x6c = 27 个职业键值（ranger/nenmaster/soulbringer/... 依 0x8e0fec5
//   字符串序，mov %eax,-0x194(+0x00)..-0x128(+0x6c) 连续写点）；
//   +0x70..+0xd8 = int[9][3]（clear 置 -1，import 不写）。
// sizeof 0xdc：vector<tagRecommendNewbieOption>::push_back finish 指针 lea 0xdc
// （0x891e61e）实证；import 帧内对象 -0x194..-0xbc 亦自洽。
#ifndef DNF_GAME_TAGRECOMMENDNEWBIEOPTION_H_
#define DNF_GAME_TAGRECOMMENDNEWBIEOPTION_H_

#include <vector>

struct tagRecommendNewbieOption
{
    tagRecommendNewbieOption() { clear(); }   // W 0x0891ad16（C1/C2）
    // 无用户析构（P12）：ORIG importRecommendNewbieOptionScript 0x0890457c..0x08905772
    // 全段无 0x0893908c(D1) 调用点（opt@-0x194 每轮仅 C1 8904911），隐式 trivial
    // 析构；D1/D2 W@0x0893908c(0x5) 由其它 TU 的 ODR 使用点发射。
    void clear()                              // W 0x0891ad2a
    {
        m_growType = -1;
        for (int i = 0; i <= 8; ++i)
            for (int j = 0; j <= 2; ++j)
                m_grow[i][j] = -1;
    }

    int m_growType;     // +0x00 [grow type] 职业索引（default -1）
    int m_jobValue[27]; // +0x04 27 个职业键值（ranger..battlemage_physical 序）
    int m_grow[9][3];   // +0x70 成长类型 0..8 × 子项 0..2（clear 置 -1）
};  // sizeof 0xdc

// tagRecommendNewbieOptionTable —— importRecommendNewbieOptionScript 形参类型
// （mangled R29tagRecommendNewbieOptionTable）。GCC 4.4 对 typedef 到模板实例会
// 展开 typedef 名，唯派生结构体才能产生 ORIG mangled。
// Table::clear（W 0x0891ad8c）与 import 函数归其它批次，此处仅声明。
struct tagRecommendNewbieOptionTable
    : public std::vector<std::vector<tagRecommendNewbieOption> >
{
    void clear();  // W 0x0891ad8c（定义归其它 agent）
};

#endif  // DNF_GAME_TAGRECOMMENDNEWBIEOPTION_H_
