DBMgr::DBMgr()
{
    m_dbHandle = new MySQL[0x11];
    if (m_dbHandle == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x301, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x301);
    }
    for (int i = 0; i < 0x11; i = i + 1)
    {
        m_dbHandle[i].init();
    }
    m_bActiveGlobalDB = false;
    m_userIdx = -1;
