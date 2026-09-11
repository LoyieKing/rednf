bool SkillSlot::backupSkill(ENUM_SKILL_TREE_KIND kind)
{
    int backupSize = 0x198;
    if (kind == -1 || kind == 0)
    {
        memcpy(m_tempSkillSlot, m_skillSlot, backupSize);
        m_tempRemainSP = m_remainSP1;
        m_tempRemainSFP = m_remainSFP1;
        return 1;
    }
    if (kind == 1)
    {
        memcpy(m_tempSkillSlot, m_skillSlot2, backupSize);
        m_tempRemainSP = m_remainSP2;
        m_tempRemainSFP = m_remainSFP2;
        return 1;
    }
    return 0;
}