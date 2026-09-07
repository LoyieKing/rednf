#include <bitset>
#include "MissionTypes.h"


// ORIG 的 +0x10/+0x18 为 std::pair<int,int> 成员（ctor 默认构造为 0，
// clear() 只重置 +0x10 的 pair）；重建布局拆成独立 int，ctor 必须显式
// 清零 m_rateMin/m_rateMax（+0x18/+0x1c）以对齐 ORIG 的 pair 构造初值。
MissionScript::MissionScript()
{
    m_rateMin = 0;
    m_rateMax = 0;
    clear();
}


// ORIG 0x8a5aa4e（is_Victory_in_a_row.md）：连胜判定——mask=(1<<victory)-1，
// (bits & mask) == mask（低 victory 位全置位）。返回 bool：末尾直透
// operator== 的 al 结果，无 setne/movzbl 规范化（P9）。
// P112 封顶（b26 Lane27）：44 条指令/助记符全等；仅 3 个 bitset 局部槽位
// 非单调分配差（4.1 maskA@-0x14/maskB@-0xc/res@-0x10 vs 4.4 声明序
// -0xc/-0x10/-0x14），声明序换位会使 ctor 调用序反向，无合法源形，封顶。
bool is_Victory_in_a_row(std::bitset<32> bits, short victory)
{
    std::bitset<32> maskA((1 << victory) - 1);
    std::bitset<32> maskB((1 << victory) - 1);
    std::bitset<32> res = bits & maskB;
    return res == maskA;
}


// ORIG 0x85e68b6 W：4×movw 清零（kind/index/count/streak）
void MissionInfo::clear()
{
    m_kind = 0;
    m_index = 0;
    m_count = 0;
    m_streak = 0;
}

