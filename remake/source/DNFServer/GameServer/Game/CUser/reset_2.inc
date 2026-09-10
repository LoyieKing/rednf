void charac_expand::CDataMgr::reset()
{
    for (int i = 0; i <= 0x13; ++i)
    {
        m_pad[i]->reset();
    }
