int DisPatcher_SelectCharac::RequestDBTicket(CUser* user, MSG_CHARAC_SELECT& msg)
{
    // [b28c-L9] fresh 复验 ndiff 21→18；p167 25=25 全清。残差同 RequestDBCharac：
    // guard D1 共享尾块与 unwind 块的相对发射序（4.1 `jmp tail` / 4.4 fall-through）
    // ⇒ 跨版本 EH 块序 N，语义完成。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "PacketDispatcher_Impl_1.cpp", 0x17db), true);
    **guard << 0xb;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo((CACHE_SECTION)4);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_TICKET* sig = guard->GetInBuffer<SIG_LOAD_TICKET>();
    memset(sig, 0, sizeof(SIG_LOAD_TICKET));
    // ORIG 081c73a6/081c73ab（报告 RequestDBTicket.md）：`call get_charac_no;
    // mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`——无 vNo 局部、
    // 无视图指针，get_charac_no 右值经 %edx 直落 sig 首字（P0 双向对应律）。
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    // [C05-① 复查] ORIG 0x081c73dd/0x081c7400 两分支均为 `mov $0x1,%ebx; jmp <共享尾块>`
    // （尾块 0x081c7422 = guard D1Ev + `mov %ebx,%eax` + epilogue），且**全函数无 res 栈槽**
    // ⇒ 原码是两分支各自 `return 1;`（不引入具名 res 局部，P0）；4.1 用 %ebx 汇聚返回值。
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
        return 1;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
