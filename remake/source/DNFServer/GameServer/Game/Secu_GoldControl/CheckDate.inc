void Secu_GoldControl::CheckDate()
{
    if (m_dateFlag != GlobalData::g_GoldControlDateFlag)
    {
        m_dateFlag = GlobalData::g_GoldControlDateFlag;
        // Copy current gold info (+0x40) to backup (+0x7c)
        m_cur.toCopy(m_backup);
        // Reset total gold info (+0x04)
        m_total.reset();
        // Reset current gold info (+0x40)
        m_cur.reset();
        // Copy m_fieldb8 to m_fieldbc, reset m_fieldb8
        m_fieldbc = m_fieldb8;
        m_fieldb8 = 0;
    }
}