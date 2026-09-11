void CUser::setBreakAwayRewards(const STBreakAwayRewards& rewards)
{
    int frame_pad; (void)frame_pad;   // ORIG 0x831d52b sub $0x18：operator= 调用点 2 outgoing 槽帧舍入，dead 局部无指令
    m_breakAway = rewards;
}