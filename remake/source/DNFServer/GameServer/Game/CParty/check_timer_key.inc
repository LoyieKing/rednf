bool CParty::check_timer_key(TIMER_MESSAGE msg, int param)
{
    // ORIG: lea -0xb(%eax),%edx; add $0x50,%edx; mov 0x8(%eax,%edx,4),%edx。
    // 表为真实成员 m_field148 @+0x148：gcc 将基址分解为 disp 8 + 0x50*4，
    // enum 基减(-0xb)与表基调整(+0x50)分处 fold 两阶段 ⇒ 两段不折叠。
    // ORIG 比较操作数序：tbl→%edx，再 mov param→%eax; cmp %eax,%edx。
    return m_field148[(int)msg - 0xb] == param;
}