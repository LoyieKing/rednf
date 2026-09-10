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
