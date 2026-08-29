#ifndef DNF_GAME_TSEARCHBYCATEGORY__H_
#define DNF_GAME_TSEARCHBYCATEGORY__H_

// LinkHelper 核销批次：ORIG _ZN18TSearchByCategory_C1Ev W @0x0822e720
// （ctor 仅在 this+0xb 构造 ROI_Category，objdump 转写）。
// 隐式 ctor 由 m_roi 成员声明自动产生，mangled 与 ORIG 一致。
#include "ROI_Category.h"

class TSearchByCategory_
{
public:
    char m_pad0[0xb];        // +0..+0xa
    ROI_Category m_roi;      // +0xb..+0x16（ctor 在 this+0xb 调 ROI_Category ctor）
};

#endif  // DNF_GAME_TSEARCHBYCATEGORY__H_
