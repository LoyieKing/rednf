int Event_Upgrade_Jar::_getMeterialCount_NewNPC(unsigned int idx)
{
    if (idx >= m_npcScripts.size())
        return 1000000;
    return m_npcScripts[idx].m_meterialCount;
