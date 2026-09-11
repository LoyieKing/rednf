int CAssaultMgr::OnTimeoutEndAssault(unsigned int id, int authenValue)
{
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(id, authenValue);
        place->TimeoutEndAssault();
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;                                            // ORIG T 0x082efaa8
}