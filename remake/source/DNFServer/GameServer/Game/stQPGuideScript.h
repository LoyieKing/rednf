// stQPGuideScript —— 快速组队 QP 指南脚本结构（ORIG 顶层类型）。
// ORIG（mangled _ZN15stQPGuideScript17stRecommendQPData...）：
//   stRecommendQPData：C1 W 0x0891ba22、C1ERKS_ W 0x0892f730、D1 W 0x0891ba6a、
//   operator= W 0x0892f7ea、clear W 0x0891ba80（+0x00/+0x04 int=-1，+0x08
//   vector<int> clear + reserve(4)）。
// importQPGuideScript（T 0x0890ff93）实证：+0x00 vector<vector<stRecommendQPData>>
//   （size()==0xb 校验，at(10) 读取），+0x0c map<int,string>（[qp tooltip mapping]）。
#ifndef DNF_GAME_STQPGUIDESCRIPT_H_
#define DNF_GAME_STQPGUIDESCRIPT_H_

#include <map>
#include <string>
#include <vector>

struct stQPGuideScript
{
    struct stRecommendQPData
    {
        stRecommendQPData();                          // W 0x0891ba22
        stRecommendQPData(stRecommendQPData const&);  // W 0x0892f730
        ~stRecommendQPData();                         // W 0x0891ba6a
        stRecommendQPData& operator=(stRecommendQPData const&);  // W 0x0892f7ea
        void clear();                                 // W 0x0891ba80

        int m_job;                    // +0x00（[qp recommendation] 首串 getCharacterJob）
        int m_grow;                   // +0x04（次串 getGrowTypeIndex）
        std::vector<int> m_items;     // +0x08（后续 ScanInt 列表）
    };  // sizeof 0x14

    // +0x00：11 槽（job 0..10）推荐数据（import 校验 size()==0xb）
    std::vector<std::vector<stRecommendQPData> > m_vec0;
    // +0x0c：[qp tooltip mapping] 键值对
    std::map<int, std::string> m_tooltipMapC;
};  // sizeof 0x24

#endif  // DNF_GAME_STQPGUIDESCRIPT_H_
