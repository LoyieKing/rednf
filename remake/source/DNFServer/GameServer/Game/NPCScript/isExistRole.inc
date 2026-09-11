// ORIG 0x86d2736: m_roleMap.find(role) != end()（CNPCScript+0x50 = NpcScript+0x4c）。
bool CNPCScript::isExistRole(ENUM_NPC_ROLE role) const
{
    return m_npcScript.m_roleMap.find(role) != m_npcScript.m_roleMap.end();
}