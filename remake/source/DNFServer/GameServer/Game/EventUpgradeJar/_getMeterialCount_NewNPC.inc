// ORIG 0x080f4ccc。NewNPC 分支：按 m_npcScripts[idx].m_meterialCount 取值；越界返回 1000000。
int Event_Upgrade_Jar::_getMeterialCount_NewNPC(unsigned int idx)
{
    if (idx >= m_npcScripts.size())
        return 1000000;
    return m_npcScripts[idx].m_meterialCount;
}