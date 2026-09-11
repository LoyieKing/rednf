PvpResultType* CUserCharacInfo::getPVPResultRefW()
{
    enableSavePvP();
    return &m_pvpResult;
}