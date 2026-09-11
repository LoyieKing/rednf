void SkillSlot::resetComboSkill(ENUM_SKILL_TREE_KIND kind)
{
    if (kind != -1 && kind != 0)
    {
        LogManager::logFormat(1, "skill_slot.cpp",
                              __PRETTY_FUNCTION__,
                              0xcab, "SkillSlot::resetComboSkill kind(%d) error",
                              kind);
    }
    else
    {
        m_comboSkill.resetTree(kind);
    }
}