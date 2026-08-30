#include <bitset>
#include "MissionTypes.h"


// ORIG _ZN13MissionScriptC1Ev W 0x08a5baf0：成员默认构造后调 clear()。
MissionScript::MissionScript()
{
    clear();
}


// ORIG 0x8a5aa4e（is_Victory_in_a_row.md）：连胜判定——mask=(1<<victory)-1，
// (bits & mask) == mask（低 victory 位全置位）。返回 bool：末尾直透
// operator== 的 al 结果，无 setne/movzbl 规范化（P9）。
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

