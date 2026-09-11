void SkillSlot::loadComboSkill(SIG_COMBO_SKILL* data)
{
    if (data == 0)
    {
        return;
    }
    if (!m_comboSkill.loadData(data))
    {
        LogManager::logFormat(
            1, "skill_slot.cpp", __PRETTY_FUNCTION__,
            0xc75, "SkillSlot::loadComboSkill error");
    }
}