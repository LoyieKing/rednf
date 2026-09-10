bool RemoveMailByMailIndex(SIG_MAILBOX_LIST* list, int mailIdx)
{
    int i = 0;
    int foundIdx = 0;
    bool found = false;
    for (i = 0; i < list->m_box.m_count; ++i)
    {
        if (mailIdx == list->m_box.m_postals[i].m_postalId)
        {
            found = true;
            foundIdx = i;
            break;
        }
    }
    if (!found)
        return 0;
    for (i = foundIdx; i < list->m_box.m_count - 1; ++i)
    {
        list->m_box.m_postals[i] = list->m_box.m_postals[i + 1];
    }
    list->m_box.m_count -= 1;
    list->m_box.m_remain += 1;
    return 1;
