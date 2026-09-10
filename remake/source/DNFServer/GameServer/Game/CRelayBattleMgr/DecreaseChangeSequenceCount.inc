void CRelayBattleMgr::DecreaseChangeSequenceCount(int team)
{
    if (team <= 0)
        return;
    if (team > 2)
        return;
    --m_changeSeqCount[team];
    if (m_changeSeqCount[team] < 0)
    {
        // [b28c-L18] 同上 P227 全表达式临时链（清 register asm("ebx") 绑定）。
        cMyTrace(__PRETTY_FUNCTION__, 0x13ea, 0)(
            "ERROR ChangeSequenceCount underflow (%d)", m_changeSeqCount[team]);
        m_changeSeqCount[team] = 0;
    }
