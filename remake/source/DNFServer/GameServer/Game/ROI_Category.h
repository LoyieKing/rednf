#ifndef DNF_GAME_ROI_CATEGORY_H_
#define DNF_GAME_ROI_CATEGORY_H_

// LinkHelper 核销批次（/tmp/undef_r21.txt：ROI_Category::_sort + 供 PCK_AUCTION 嵌入）。
// ORIG 证据：_ZN12ROI_CategoryC1Ev W @0x0822ac10（三 dword 清零），
//            _ZN12ROI_Category5_sortEv W @0x0822ac30（3 记录冒泡排序，objdump 转写）。
// 布局：3 个 ushort key @+0..+5、3 个 byte mark @+8..+a；sizeof=0xc（ctor 三 dword 覆盖）。
class ROI_Category
{
public:
    ROI_Category();
    void _sort();

    // ORIG _sort 0x0822ac3c 首比较为 `movzwl (%eax),%edx` 一步直落 + jle（有符号）；
    // short 字段 + 直接比较复现（探针 /tmp/l25probe/roi.cpp fB vs fA：
    // unsigned short+static_cast<short> 恒两段 `movzwl %eax; mov %eax,%edx`）。
    short m_keys[3];             // +0..+5
    unsigned char  m_pad6[2];    // +6..+7
    union {                      // +8..+b（sizeof 不变；ORIG ctor 第三 dword 清零视图）
        unsigned char  m_marks[3];   // +8..+a
        unsigned int   m_dw8;
    };
};

#endif  // DNF_GAME_ROI_CATEGORY_H_
