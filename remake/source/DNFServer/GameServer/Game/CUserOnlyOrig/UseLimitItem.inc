unsigned char CUser::UseLimitItem(int itemIdx, bool sendNoti)
{
    unsigned char ok = 1;
    for (std::vector<LimitItemUsage>::iterator it = m_limitItemUsage.begin();
         it != m_limitItemUsage.end(); it++)   // ORIG 命中 operator++(int)（P28）
    {
        // [L6 判据①/P222 极性重核] ORIG 0x08688ae6 段 `mov 0x15(%eax),%eax;
        // cmp 0xc(%ebp),%eax; sete %al; test %al,%al; je` ⇒ 原码是**正极性**守卫
        // `if (field == itemIdx) { 体 }`（体在 je 之后直落）；旧 `if (field != itemIdx)
        // continue;` 使 4.4 反产 `setne %al; test; je`。体各路径均 return，改写等价。
        if (it->m_field15 == itemIdx)
        {
        if (it->isUsable() != 1)
            ok = 0;
        if (sendNoti)
            SendLimitItemUsageNotiInfo(itemIdx,
                                       // [b28d-solo-CUserOnlyOrig P167] ORIG 0x08688bxx
                                       // 命中 `__normal_iterator<LimitItemUsage*>::operator->()`
                                       // ×3、无 operator* ⇒ 字段一律经 `it->` 具名成员
                                       // （旧 `&*it + 0x11` 生 operator* 差集）。
                                       // [FL5 2026-09-10 P162/类型轴] ORIG 0x08688b8d
                                       // `mov 0x11(%eax),%eax; mov %eax,0x8(%esp)` = **4 字节
                                       // 整字直传**（int 观），旧 `(signed char)m_field11b[0]`
                                       // 产 movzbl+movsbl（1 字节读）⇒ 改 union 的 int 观
                                       // `m_field11`（sizeof/偏移/符号集不变）。
                                       it->m_field11,
                                       it->m_field19);
        if (it->useIt())
        {
            long long t = *reinterpret_cast<long long*>(&it->m_fieldc);
            DB_UpdateLimitItemUsage::makeRequest(get_charac_no(-1),
                (signed char)it->m_field10,
                itemIdx, it->m_field19, t);
            return 1;
        }
        return 0;
        }
    }
    return ok;
