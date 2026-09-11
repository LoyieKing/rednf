void SkillSlot::eraseDuplicationComboSKillTree(ENUM_SKILL_TREE_KIND kind)
{
    if (kind != 0 && kind != 1)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            __PRETTY_FUNCTION__,
            0xd0b, "SkillSlot::eraseDuplicationComboSKillTree kind(%d) error",
            kind);
    }
    else
    {
        int skillIdx = 0;
        for (int i = 0xc6; i < 0xcc; ++i)
        {
            skillIdx = getSkillSlotIndex(kind, i);
            if (skillIdx == 0)
            {
                continue;
            }
            m_comboSkill.eraseDuplicationComboSKill(kind, skillIdx);
        }
    }
}