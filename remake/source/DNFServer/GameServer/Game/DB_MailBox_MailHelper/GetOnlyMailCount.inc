// ============================================================================
// GetOnlyMailCount —— ORIG 0x0841cc6a：postal 区扫 +0x5c（letter_id），
// 统计为 0 的条目数，返计数。
// ============================================================================
// [sweep] AE：ORIG 0x0841cc6a；改走 m_box.m_postals[i].m_tail.m_occ.m_letterId
// 后循环体装载序/寻址与 ORIG 逐条同形（25=25，p167 不适用=0 call）。
int GetOnlyMailCount(SIG_MAILBOX_LIST* list)
{
    int count = 0;
    int i;
    for (i = 0; i < list->m_box.m_count; ++i)
    {
        if (list->m_box.m_postals[i].m_tail.m_occ.m_letterId == 0)
            ++count;
    }
    return count;
}