int SkillSlot::get_skillslot_no(_Mastered_skill* buf, int skillIdx, int group,
                                bool checkCombo) const
{
    if (m_parent == 0)
    {
        return -1;
    }
    if (buf == 0)
    {
        return -1;
    }
    if (checkCombo)
    {
        for (int i = 0; i < 6; ++i)
        {
            if ((unsigned char)buf[i].m_index == skillIdx)
            {
                return i;
            }
        }
    }
    int start = 0;
    int end = 0;
    switch (group)
    {
    case 0:
        start = 6;
        end = 0x36;
        break;
    case 1:
        start = 0x36;
        end = 0x66;
        break;
    case 2:
        start = 0x66;
        end = 0x96;
        break;
    case 3:
        start = 0x96;
        end = 0xc6;
        break;
    default:
        break;
    }
    for (int i = start; i < end; ++i)
    {
        if ((unsigned char)buf[i].m_index == skillIdx)
        {
            return i;
        }
    }
    if (checkCombo)
    {
        for (int i = 0xc6; i < 0xcc; ++i)
        {
            if ((unsigned char)buf[i].m_index == skillIdx)
            {
                return i;
            }
        }
    }
    return -1;
}