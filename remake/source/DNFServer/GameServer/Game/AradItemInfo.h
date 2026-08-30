#ifndef DNF_GAME_ARADITEMINFO_H_
#define DNF_GAME_ARADITEMINFO_H_

#include <string>
#include <vector>

#include "AradRabbitBox.h"

// ---- AradItemInfo（ORIG C1/D1 W 0x08515618/0x08514430，全 W → 头内联）----
// 布局（C1 逐 call 实证）：int@+0x00 + char@+0x04 + pad[3] +
// string@+0x08 + string@+0x0c + string@+0x10 + vector<int>@+0x14 +
// vector<CRabbitBox>@+0x20，sizeof 0x2c。
// C1 只构造 5 个非平凡成员（3 string + 2 vector，无 vptr 写入）；D1 按逆序
// 析构 5 成员。typeinfo 符号存在但 C1/D1 无 vptr 读写、nm 无 D0 → ORIG 析构
// 非虚（头此前声明 virtual 为 HdrCreate1 推断，本批按汇编事实修正）。
// +0x04 按 CStackableItem::set_item（ORIG 0x850d81e，mov %dl,0x14c 1 字节写）与
// CStackableItem::check_change_job_type（ORIG 0x850eba2，movzbl 0x14c 1 字节读）
// 实证为 char（此前的 int 建模为推断，本批修正；C1/D1 均不触标量故不影响）。
class AradItemInfo
{
public:
    AradItemInfo()
        : m_str8(), m_strC(), m_str10(), m_vec14(), m_vec20()
    {
    }

    ~AradItemInfo() {}

    int m_field0;                         // +0x00（C1 未初始化）
    unsigned char m_field4;               // +0x04（C1 未初始化；set_item 1 字节写）
    char m_pad5[3];                       // +0x05..+0x07
    std::string m_str8;                   // +0x08
    std::string m_strC;                   // +0x0c
    std::string m_str10;                  // +0x10
    std::vector<int> m_vec14;             // +0x14
    std::vector<CRabbitBox> m_vec20;      // +0x20
};   // sizeof 0x2c


#endif // DNF_GAME_ARADITEMINFO_H_
