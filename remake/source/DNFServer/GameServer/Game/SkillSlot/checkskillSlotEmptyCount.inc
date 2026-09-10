int SkillSlot::checkskillSlotEmptyCount(int& out)
{
    if (m_parent == 0)
    {
        return 0;
    }
    int total = 0;
    int charLevel = ((CUserCharacInfo*)m_parent)->get_charac_level();
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return 0;
    }
    int emptyCount = 0;
    for (int i = 0; i < 6; ++i)
    {
        if (buf[i].m_index == 0)
        {
            emptyCount = emptyCount + 1;
        }
    }
    if (charLevel < 0x10)
    {
        emptyCount = emptyCount - 3;
        out = 1;
    }
    else if (charLevel < 0x1f)
    {
        emptyCount = emptyCount - 1;
        out = 2;
    }
    else
    {
        out = 3;
    }
    total = emptyCount;
    if (0x2d >= charLevel)
    {
        return total;
    }
    emptyCount = 0;
    for (int i = 0xc6; i < 0xcc; ++i)
    {
        if (buf[i].m_index == 0)
        {
            emptyCount = emptyCount + 1;
        }
    }
    if (charLevel < 0x3d)
    {
        emptyCount = emptyCount - 3;
        out = 4;
    }
    else
    {
        emptyCount = emptyCount - 2;
        out = 5;
    }
    if (0 < emptyCount)
    {
        total = total + emptyCount;
    }
    return total;
