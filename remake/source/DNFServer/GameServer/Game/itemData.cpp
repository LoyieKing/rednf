// ============================================================================
// itemData.cpp — itemData ctor/clear/析构（ORIG W 0x080f1134/0x080f1162/
// 0x080f32fa 逐指令转写）。
//
// ORIG 形状：每次 store 前重读 this（mov 0x8(%ebp),%eax）→ 必须写函数体
// 逐字段赋值（P29：初始化列表会缓存 this 为 ecx，指令形状不同）。
//   movb $0,0x0 / movb $1,0x1 / movb $1,0x2 / movl $0,0x4 / movl $0,0x8
// C1/C2 同址（无虚/无基类，gcc 生成同一体）；clear 为独立函数、指令同 C1。
// D1/D2（W 0x080f32fa）为空体（push/mov/pop/ret）——**隐式** trivial 析构，
// 不再用户声明：ORIG eventReward::ImportRewardScript 0x080ef3a2 对 6 处
// `itemData item = curItem;` 局部副本零 D1Ev 调用（用户空析构会逐点发 call）；
// 删声明后 D1/D2 仍由使用点发射同名 W 符号（P317），符号集不变。
// ============================================================================
#include "itemData.h"

itemData::itemData()
{
    m_upgrade = 0;
    m_seal = 1;
    m_roi = 1;
    m_itemIdx = 0;
    m_count = 0;
}

void itemData::clear()
{
    m_upgrade = 0;
    m_seal = 1;
    m_roi = 1;
    m_itemIdx = 0;
    m_count = 0;
}
