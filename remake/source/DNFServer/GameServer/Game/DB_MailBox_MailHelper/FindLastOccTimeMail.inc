int FindLastOccTimeMail(SIG_MAILBOX_LIST* list, int occTime)
{
    int maxVal = 0;
    for (int i = 0; i < list->m_box.m_count; ++i)
    {
        int v = list->m_box.m_postals[i].m_tail.m_occ.m_field55 - 0x13c680;
        if (v > occTime && maxVal < v)
            maxVal = v;
    }
    int lastIdx = -1;
    unsigned int maxId = 0;
    for (int i = 0; i < list->m_box.m_count; ++i)
    {
        int v = list->m_box.m_postals[i].m_tail.m_occ.m_field55 - 0x13c680;
        if (v == maxVal &&
            maxId < (unsigned int)list->m_box.m_postals[i].m_postalId)
        {
            maxId = (unsigned int)list->m_box.m_postals[i].m_postalId;
            lastIdx = i;
        }
    }
    return lastIdx;
