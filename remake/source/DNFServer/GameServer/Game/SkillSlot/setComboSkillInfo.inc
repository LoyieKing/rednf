void SkillSlot::setComboSkillInfo(ENUM_SKILL_TREE_KIND kind,
                                  ComboSkillMap& map)   // [Q6HeaderTypeAxis] 定义处
                                  // declarator 拼写决定 prett ⇒ ORIG
                                  // `…(ENUM_SKILL_TREE_KIND, ComboSkillMap&)`（同类型）
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
}