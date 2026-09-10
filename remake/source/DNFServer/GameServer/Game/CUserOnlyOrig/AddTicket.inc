void CUser::AddTicket(int ticketIdx, bool notify)
{
    if (notify)
    {
        // [b28d-solo-CUserOnlyOrig P167/P96/P28] ORIG 命中非 const
        // `_List_iterator<int>::operator*() const / operator++(int) / operator!=(const&) const`；
        // 旧 const_iterator + 前置 ++ 产 `_List_const_iterator` 转换构造差集。
        for (std::list<int>::iterator it = m_list8cf80.begin();
             it != m_list8cf80.end(); it++)
        {
            if (*it == ticketIdx)
                return;
        }
        m_list8cf80.push_back(ticketIdx);

        // [A06 2026-09-10 P259 试行记录·已回退] 本件 ORIG 133 条含该 store/load 对
        // （内联后我方 133→131、delta 26→32 反劣）⇒ ORIG 该处确有具名槽，维持具名形。
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x703d);
        CStreamGuard guard(s, true);
        // [b28d-solo-CUserOnlyOrig P167/P132] ORIG 0x08678xxx 命中
        // `CStreamGuard::operator*()` ×2 + `operator->()` ×1 ⇒ 原码是
        // `(*guard) << x` / `guard->put_binary(...)` 形（直接 guard<< 省略 de() 调用）。
        guard.operator*()->operator<<(0x28);
        guard.operator*()->operator<<((int)GetUID());
        struct { unsigned int characNo; int ticket; } payload;
        memset(&payload, 0, 8);
        payload.characNo = getCurCharacNo();
        payload.ticket = ticketIdx;
        guard->put_binary(&payload, 8);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    else
    {
        m_list8cf80.push_back(ticketIdx);
    }
