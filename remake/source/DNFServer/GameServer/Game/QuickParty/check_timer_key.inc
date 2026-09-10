bool CQuickParty::check_timer_key(TIMER_MESSAGE msg, int key)
{
    // b28c-lane3：ORIG 0x082696c2 元素→%edx、0x082696c6 key→%eax、
    // `cmp %eax,%edx`（12 条）；无 cast 时 4.4 折成 `cmp key_mem,%eax`（11 条，
    // N18b）。两侧同 cast 到 unsigned int 后 4.4 走「两寄存器操作数」形。
    return m_timerKeyCnt[static_cast<int>(msg) - 0x9d] == (unsigned int)key;
