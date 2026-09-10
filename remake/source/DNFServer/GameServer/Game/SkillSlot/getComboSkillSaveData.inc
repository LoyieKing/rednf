void SkillSlot::getComboSkillSaveData(SIG_COMBO_SKILL* data) const
{
    if (m_parent == 0)
    {
        return;
    }
    if (data == 0)
    {
        return;
    }
    data->m_characNo = m_parent->get_charac_no(-1);
    data->m_job = ((CUserCharacInfo*)m_parent)->get_charac_job();
    if (!m_comboSkill.saveData(data))
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            __PRETTY_FUNCTION__,
            0xc84, "SkillSlot::getComboSkillSaveData error");
    }
