CSession::CSession()
{
    // [b28-L11 复验] 先按 ORIG 直书 dead temp 实测 4.4 EH 代价（b22 判词称 +15）。
    {
        CNetwork<95000, 450000> deadTmp;
    }
    m_triggerEventType = 0;
    m_channelNo = 0;
    // ORIG movb $0x1,0xc(%eax)（仅写低字节；throw() 后 dead-temp 直书零 EH 代价）
    ((NetFlagView*)this)->m_flag0c = 1;
}