bool CNPCScript::isExistRole(ENUM_NPC_ROLE role) const
{
    return m_npcScript.m_roleMap.find(role) != m_npcScript.m_roleMap.end();
