// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// TSearchByItemId_ 为 ORIG 符号原样名（Ghidra 风格尾下划线），仅 ctor W 0x0822e694。
// 布局（ORIG ctor 0x822e694：ROI_Category ctor(this+0x9)）：
//   +0x00..0x08 未观测标量区；+0x09 ROI_Category（sizeof 0xc）→ sizeof 0x15。
#ifndef DNF_GAME_TSEARCHBYITEMID__H_
#define DNF_GAME_TSEARCHBYITEMID__H_

#include "ROI_Category.h"

#pragma pack(push, 1)
class TSearchByItemId_
{
public:
    TSearchByItemId_();   // W 0x0822e694（定义 ValueCtors.cpp）
    char m_pad0[0x9];     // +0x00..0x08
    ROI_Category m_roi;   // +0x09
};  // sizeof 0x15（packed：ROI_Category@+0x9 奇数偏移，随 ORIG）
#pragma pack(pop)

#endif  // DNF_GAME_TSEARCHBYITEMID__H_
