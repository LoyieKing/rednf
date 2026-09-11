// ORIG W 0x0815de02：restrictValue 写 +0；flag 写 m_extra[index]（0x119c 段）；
// 计数自增。index 无界（ORIG 同样直接下标）。
void SigLoadRestrictPolicy::set(int restrictValue, int index, char flag)
{
    m_restrictValue = restrictValue;
    // [b27L8] ORIG 0x0815de02 物化序 index→this→flag，4.4 恒 index→flag→this
    // （N18a 族寄存器物化序差；指针算术形丢 scaled-index 更劣，探针已证）。
    m_extra[index] = flag;
    ++m_count;
}