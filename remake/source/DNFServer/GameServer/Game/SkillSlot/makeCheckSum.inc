int SkillSlot::makeCheckSum(int key) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return -1;
    }
    int sum = 0;
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index == 0)
        {
            continue;
        }
        sum = getSkillChecksum(i, (unsigned char)buf[i].m_index,
                               buf[i].m_level) +
              sum;
    }
    return finishSkillChecksum(&sum, key);
