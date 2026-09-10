bool SkillSlot::checkComboSkillInsertQuickSlot(int skillIdx) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    if (((CUserCharacInfo*)m_parent)->get_charac_job() == 9 && skillIdx == 0)
    {
        return 0;
    }
    return 1;
