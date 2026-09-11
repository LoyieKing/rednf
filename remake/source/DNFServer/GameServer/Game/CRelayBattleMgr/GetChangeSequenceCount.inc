int CRelayBattleMgr::GetChangeSequenceCount(int team)
{
    // ORIG 0x085de474：`cmpl $0; jle; cmpl $2; jle` 两 guard 共享尾部 return 0
    // （Ghidra 合并短路 `(team<1)||(team>2)`）。独立双 return 使 4.4 生两套
    // mov$0;jmp 块（+2 指令）。合并短路由 4.4 复现 ORIG jle 链。
    if (team <= 0 || team > 2)
        return 0;
    return m_changeSeqCount[team];
}