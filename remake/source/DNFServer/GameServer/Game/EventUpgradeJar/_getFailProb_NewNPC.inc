// ORIG 0x080f4c8e。NewNPC 分支：按 m_npcScripts[idx].m_failRate 取值；越界返回 100000。
unsigned int Event_Upgrade_Jar::_getFailProb_NewNPC(unsigned int idx) const
{
    if (idx >= m_npcScripts.size())
        return 100000;
    return m_npcScripts[idx].m_failRate;
}