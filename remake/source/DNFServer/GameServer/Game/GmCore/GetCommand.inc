CCommand* CGameMasterMgr::GetCommand(int id)           // 0x084a8c92
{
    return m_cmdMap.get(id);
