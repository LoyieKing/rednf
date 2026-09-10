int* SkillSlot::get_temp_remain_sp()
{
    if (m_parent == 0)
    {
        return 0;
    }
    return &m_tempRemainSP;
