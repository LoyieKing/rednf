bool CInGameAdvertisementManager::LoadADExposeInfoAtApp()
{
    stInGameADExposeCondition buf[200];
    std::memset(buf, 0xff, 0x67b60);
    if (queryADExposeInfo(buf))
    {
        for (int i = 0; i <= 199; ++i)
        {
            if (buf[i].m_sentinel == -1)
            {
                break;
            }
            InsertADExposeInfo(buf[i]);
        }
        return true;
    }
    return false;
