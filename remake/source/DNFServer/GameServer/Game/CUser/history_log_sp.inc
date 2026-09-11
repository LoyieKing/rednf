// 0x0866ac0e history_log_sp：before==-1 或当前值变化时记 SPAdd(tree 0)。
void CUser::history_log_sp(int before, int delta, eSPAddReason reason)
{
    // ORIG 0x866ac0e 三调用复制形（判据③直分支消费者）：==-1 真支调一次读+SPAdd；
    // 假支 cmp 直落 je；SPAdd 前**再调一次**重取（无 remain 槽）。免缓存三处展开。
    if (before == -1)
    {
        m_historyLog.SPAdd(0,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0),
            delta, reason);
    }
    else if ((((SkillSlot*)getCurCharacSkillR())
                  ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0)
              != before))
    {
        m_historyLog.SPAdd(0,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0),
            delta, reason);
    }
}