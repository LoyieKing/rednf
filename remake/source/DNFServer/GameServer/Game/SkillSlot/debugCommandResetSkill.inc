void SkillSlot::debugCommandResetSkill()
{
    memset(m_skillSlot, 0, 0x198);
    memset(m_skillSlot2, 0, 0x198);
    _Mastered_skill temp[204];
    addSkillOnCreateCharacter(
        temp, ((CUserCharacInfo*)m_parent)->get_charac_job());
    for (int i = 0; i < 204; ++i)
    {
        if (temp[i].m_index)
        {
            m_skillSlot[i].m_index = temp[i].m_index;
            m_skillSlot[i].m_level = temp[i].m_level;
            m_skillSlot2[i].m_index = temp[i].m_index;
            m_skillSlot2[i].m_level = temp[i].m_level;
        }
    }
    if (((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType() != 0)
    {
        m_parent->set_grow_type(
            ((CUserCharacInfo*)m_parent)->getCurCharFirstGrowType(),
            ((CUserCharacInfo*)m_parent)->getCurCharSecondGrowType(), 0,
            (eChangeGrowTypeReason)2);
    }
