int CAssaultMgr::OnTimeoutConnectP2P(unsigned int id, int authenValue)
{
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(id, authenValue);
        place->TimeoutConnectP2P();
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;                                            // ORIG T 0x082efa52
