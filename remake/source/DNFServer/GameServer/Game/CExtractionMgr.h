// expert_extraction::CExtractionMgr —— ORIG T 声明头（实现 ExpertExtraction.cpp；
// ctor/dtor 定义 GlobalData.cpp）。
// ORIG：C1 T 0x084a302e、get_extracter T 0x084a3166、D1 T 0x084a311a。
// 佐证：expert_extraction__CExtractionMgr/CExtractionMgr.md（ctor new(8) 后仅写
// +0x00/+0x04 两个指针槽）→ sizeof 0x8；D1 0x84a311a 以 (CExtraction**) 索引
// 循环析构 → 成员为数组 m_slots[3]。
#ifndef DNF_GAME_CEXTRACTIONMGR_H_
#define DNF_GAME_CEXTRACTIONMGR_H_
#include <cstddef>

namespace expert_extraction
{

class CExtraction;  // 完整定义见 CExtraction.h

class CExtractionMgr
{
public:
    CExtractionMgr();
    ~CExtractionMgr();
    CExtraction* get_extracter(unsigned int index);

private:
    CExtraction* m_slots[3];  // +0x00/+0x04/+0x08【推断】
};

}  // namespace expert_extraction

#endif  // DNF_GAME_CEXTRACTIONMGR_H_
