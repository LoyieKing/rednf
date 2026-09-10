int CDataManager::find_npc(unsigned int idx) const
{
    return (int)(unsigned long)m_npcScriptList->find(idx);
