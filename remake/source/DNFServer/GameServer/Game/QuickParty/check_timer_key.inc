// ---- check_timer_key @0x082696b0：槽值与给定 key 相等判断 ----
// [b28d-L1] N7/N18b 封顶：ORIG 0x082696b0 为 `mov 0xc(%eax,%edx,4),%edx; mov 0x10(%ebp),%eax; cmp %eax,%edx`（两寄存器）；4.4 恒折 `mov …,%eax; cmp 0x10(%ebp),%eax`（内存直比，11 vs 12 条）。unsigned/signed、key 置左/置右、&0xff 掩码四形实测同折。
// [b28d-solo-QuickParty] verdict=AE（12=12）：修法=P248 档②字段侧——m_timerKeyCnt
// int[2]→unsigned int[2]（QuickParty.h，同宽零布局影响）+ `elem == (unsigned int)key`；
// 探针 /tmp/b28d_solo/probe_ck.cpp g0/g4 逐条全等（ORIG 两寄存器 12 条）。旧 N7/N18b 判词作废。
bool CQuickParty::check_timer_key(TIMER_MESSAGE msg, int key)
{
    // b28c-lane3：ORIG 0x082696c2 元素→%edx、0x082696c6 key→%eax、
    // `cmp %eax,%edx`（12 条）；无 cast 时 4.4 折成 `cmp key_mem,%eax`（11 条，
    // N18b）。两侧同 cast 到 unsigned int 后 4.4 走「两寄存器操作数」形。
    return m_timerKeyCnt[static_cast<int>(msg) - 0x9d] == (unsigned int)key;
}