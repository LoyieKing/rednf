bool isUnlimitMail(SIG_MAILBOX_LIST const* info, int letterId)
{
    for (int i = 0; i < info->m_box.m_count; ++i)
    {
        if (info->m_box.m_postals[i].m_tail.m_occ.m_letterId == letterId)
            return info->m_box.m_postals[i].m_tail.m_occ.m_unlimit;
    }
    return false;
