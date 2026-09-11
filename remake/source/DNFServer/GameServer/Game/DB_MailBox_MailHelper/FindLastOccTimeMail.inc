// ============================================================================
// FindLastOccTimeMail —— ORIG 0x0841cf5a：第一遍取 occ_time(+0x55)-0x13c680
//   大于入参的最大值；第二遍找等于该值且 postal_id(+0x00) 最大的 postal 下标。
// ============================================================================
// [sweep] AE：ORIG 0x0841cf5a；p167 CLEAR（0 call）。
//   真 bug 修复：第二循环的 maxId 取自 postal **+0x00**（ORIG 0x0841cff9
//   `mov (%edx,%eax,1),%eax`，Ghidra C 亦为 `*(uint *)(param_1 + local_c*0x61)`），
//   原码误读 +0x5c（m_occ.m_letterId）⇒ 选出的 lastIdx 与 ORIG 不同（删错邮件）。
//   同时改走权威头 m_box.m_postals[i].m_postalId / m_tail.m_occ.m_field55
//   （SigTypes_D.h 具名视图，67=67 逐条同形）。
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
}