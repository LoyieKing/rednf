int* SkillSlot::get_temp_remain_sfp()
{
    if (m_parent == 0)
    {
        return 0;
    }
    return &m_tempRemainSFP;
}