_Mastered_skill* SkillSlot::get_temp_skill_slot()
{
    if (m_parent == 0)
    {
        return 0;
    }
    return m_tempSkillSlot;
}