int SkillSlot::skill_move(unsigned char slot1, unsigned char slot2, int job)
{
    if (m_parent == 0)
    {
        return -1;
    }
    if (!checkMoveComboSkillSlot(slot1, slot2))
    {
        return 0x13;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return -1;
    }
    if (buf[slot1].m_index == 0 && buf[slot2].m_index == 0)
    {
        return 0x15;
    }
    CSkill* skill1 = G_CDataManager()->find_skill(job, buf[slot1].m_index);
    CSkill* skill2 = G_CDataManager()->find_skill(job, buf[slot2].m_index);
    if (skill1 == 0 && skill2 == 0)
    {
        return 1;
    }
    int group1 = get_skillslot_group(slot1);
    while (group1 == 6)
    {
        return 1;
    }
    int group2 = get_skillslot_group(slot2);
    while (group2 == 6)
    {
        return 1;
    }
    if (group1 == 4 || group2 == 4)
    {
        return 0x13;
    }
    bool canMove = false;
    if (buf[slot1].m_index == 0 && buf[slot2].m_index != 0)
    {
        if (skill2 == 0)
        {
            canMove = false;
        }
        else if (group1 == -1)
        {
            if (skill2->is_active_skill())
            {
                canMove = true;
            }
        }
        else
        {
            if (skill2->get_group() == group1)
            {
                canMove = true;
            }
        }
    }
    else if (buf[slot2].m_index == 0 && buf[slot1].m_index != 0)
    {
        if (skill1 == 0)
        {
            canMove = false;
        }
        else if (group2 == -1)
        {
            if (skill1->is_active_skill())
            {
                canMove = true;
            }
        }
        else
        {
            if (skill1->get_group() == group2)
            {
                canMove = true;
            }
        }
    }
    else if (skill1 == 0 || skill2 == 0)
    {
        canMove = false;
    }
    else if (group1 == -1 && group2 == -1)
    {
        canMove = true;
    }
    else
    {
        if (group1 == -1 && skill1->get_group() == group2)
        {
            if (skill2->is_active_skill())
            {
                canMove = true;
            }
        }
        else
        {
            if (skill2->get_group() == group1 && group2 == -1)
            {
                if (skill1->is_active_skill())
                {
                    canMove = true;
                }
            }
            else
            {
                if (skill2->get_group() == group1 &&
                    skill1->get_group() == group2)
                {
                    canMove = true;
                }
            }
        }
    }
    if (!canMove)
    {
        return 0x13;
    }
    _Mastered_skill temp;
    temp = buf[slot1];
    buf[slot1] = buf[slot2];
    buf[slot2] = temp;
    return 0;
