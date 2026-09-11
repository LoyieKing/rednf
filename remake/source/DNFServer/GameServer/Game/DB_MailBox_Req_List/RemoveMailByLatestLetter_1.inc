// ============================================================================
// RemoveMailByLatestLetter —— ORIG 0x0841cd03：postal 区自 idx 起左移一项
//   （0x61 字节），count-1、remain+1，恒返 1。
// ============================================================================
bool RemoveMailByLatestLetter(SIG_MAILBOX_LIST* list, int mailIdx)
{
    // [b28d-solo-DB_MailBox_Req_List] verdict=AE（check_tu_routed，87=87）。
    // 原 N78 判词（memmove 恒 call）已被 P312/N84
    // 路线推翻：ORIG 0x0841cd03 的 0x61 字节搬移 = **结构体整体赋值**的对齐剥离
    // rep movsl 展开（探针 /tmp/p2/c.cpp c1 与 ORIG 0x0841cd44..0x0841cdae 逐条同形）。
    // 改用 SIG_MAILBOX_LIST::m_box 整体赋值后该段与 count/remain 段全部复现
    // （探针 /tmp/p2/rm2.cpp 与 ORIG 逐条同形）；memmove 会退化为 call。
    if (list->m_box.m_count - 1 != mailIdx)
    {
        int i;
        for (i = mailIdx; i < list->m_box.m_count - 1; ++i)
        {
            list->m_box.m_postals[i] = list->m_box.m_postals[i + 1];
        }
    }
    list->m_box.m_count -= 1;
    list->m_box.m_remain += 1;
    return 1;
}