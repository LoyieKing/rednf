void SkillSlot::setComboSkillInfo(ENUM_SKILL_TREE_KIND kind,
                                  std::map<short, std::vector<int> >& map)
{
    if (kind != 0 && kind != 1)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            __PRETTY_FUNCTION__,
            0xc92, "SkillSlot::setComboSkillInfo kind(%d) error", kind);
    }
    else
    {
        m_comboSkill.setComboSkill(kind, map);
    }
