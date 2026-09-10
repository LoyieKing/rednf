void charac_expand::CDataMgr::ResetDailyMidnight()
{
    for (int i = 0; i < 0x14; ++i)
    {
        m_pad[i]->ResetDailyMidnight();
    }
