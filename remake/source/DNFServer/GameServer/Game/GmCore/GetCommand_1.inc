CCommand* CGameMasterMgr::GetCommand(std::string& name)  // 0x084a8cb0
{
    return m_cmdMap.get(name);
}