void charac_expand::CDataMgr::ResetDaily()
{
    for (int i = 0; i < 0x14; ++i)
    {
        m_pad[i]->ResetDaily();
    }
