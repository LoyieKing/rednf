void CUser::AddTicket(int ticketIdx, bool notify)
{
    // [b28d-solo-CUserOnlyOrig FL5 2026-09-10] ORIG 0x08678290 入口形
    //   `movzbl -0x2c(%ebp),%eax; xor $0x1,%eax; test %al,%al; je <主体>`
    //   = 4.4.6 对**裸 `if (!bool形参)`** 的唯一产物（探针 /tmp/fl5/p1.cpp f4 vs
    //   f1 `if (b)`→cmpb / f2 具名 bool→cmpb / f5 uchar^=1→xorb+cmpb）；
    //   故原码是「否定早判 + push_back 落 fall-through、主体在 je 目标」⇒ 反转两块。
    if (!notify)
    {
        m_list8cf80.push_back(ticketIdx);
    }
    else
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

        // [FL5 2026-09-10 P259] ORIG 0x08678358..0x08678387 直证：Acquire 结果
        // `mov %eax,0x4(%esp)` 直入 CStreamGuard 构造实参，**无** store/load 对
        // ⇒ 删具名 Stream* 局部，表达式内联（同时释放 -0xc 槽给迭代器临时）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x703d), true);
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
