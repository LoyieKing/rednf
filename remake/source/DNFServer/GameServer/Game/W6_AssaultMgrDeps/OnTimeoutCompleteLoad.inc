int CAssaultMgr::OnTimeoutCompleteLoad(unsigned int id, int authenValue)
{
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(id, authenValue);
        place->TimeoutCompleteLoad();
    }
    catch (ENUM_ERROR)
    {
    }
    return 1;                                            // ORIG T 0x082ef818
}