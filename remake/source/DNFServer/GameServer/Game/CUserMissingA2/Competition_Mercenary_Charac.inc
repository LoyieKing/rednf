int CUser::Competition_Mercenary_Charac(unsigned int slot, char modeA, char modeB)
{
    // ORIG 0x0868e6c0：透传 CompetitionCharac 的 0/1（Ghidra 因 leave/ret 误判 void；
    // 调用点 Dispatcher_Mercenary_Competition::process 读 %al 写 result+4 实证）。
    return m_mercenaryMgr.CompetitionCharac(this, slot, modeA, modeB);
