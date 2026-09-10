int CAssaultMgr::OnTimeoutCloseAssault(unsigned int id, int authenValue)
{
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(id, authenValue);
        place->TimeoutCloseAssault();
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;                                            // ORIG T 0x082efafe
