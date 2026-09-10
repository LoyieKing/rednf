unsigned int Event_Upgrade_Jar::_getFailProb_NewNPC(unsigned int idx) const
{
    if (idx >= m_npcScripts.size())
        return 100000;
    return m_npcScripts[idx].m_failRate;
