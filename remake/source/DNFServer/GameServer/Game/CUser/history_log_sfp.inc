void CUser::history_log_sfp(int before, int delta, eSPAddReason reason)
{
    // ORIG 同构三调用形（tree 2 / SFPAdd，0x0866acd0）。
    if (before == -1)
    {
        m_historyLog.SFPAdd(2,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2),
            delta, reason);
    }
    else if ((((SkillSlot*)getCurCharacSkillR())
                  ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2)
              != before))
    {
        m_historyLog.SFPAdd(2,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2),
            delta, reason);
    }
