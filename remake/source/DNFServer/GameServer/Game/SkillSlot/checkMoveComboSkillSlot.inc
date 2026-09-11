bool SkillSlot::checkMoveComboSkillSlot(unsigned char slot1, unsigned char slot2)
{
    if (m_parent == 0)
    {
        return false;
    }
    if (((CUserCharacInfo*)m_parent)->get_charac_job() == 9 &&
        (slot1 < 6 || slot2 < 6))
    {
        return false;
    }
    return true;
}