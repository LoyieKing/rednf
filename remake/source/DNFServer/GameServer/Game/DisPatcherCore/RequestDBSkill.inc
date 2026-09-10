int DisPatcher_SelectCharac::RequestDBSkill(CUser* user, MSG_CHARAC_SELECT& msg)
{
    // [b28c-L9] fresh 复验 ndiff 21→18；p167 25=25 全清。残差 = guard D1 共享尾
    // 块序（同 RequestDBCharac），跨版本 EH 布局 N，语义完成。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x17a1), true);
    **guard << 9;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_SKILL);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_SKILL* sig = guard->GetInBuffer<SIG_LOAD_SKILL>();
    memset(sig, 0, sizeof(SIG_LOAD_SKILL));
    // ORIG 081c7032/081c7037（报告 RequestDBSkill.md）：`call get_charac_no;
    // mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`——值经 %edx 中转、
    // sig 槽 reload 后直落首字（SigTypes_C.h m_characNo 权威字段，无局部视图）。
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    // [C05-① 命中形，同 RequestDBTicket] ORIG +0x161/+0x1a6 两分支均 `mov $0x1,%ebx;
    // jmp <共享尾块 0x081c7082：guard D1Ev + `mov %ebx,%eax` + epilogue>`，全函数无 res
    // 栈槽 ⇒ 原码两分支各自 `return 1;`（无 res 具名局部，P0）。实测 RequestDBTicket
    // 同法 120=120（TU ae +1 / diff −1）。
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
        return 1;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;   // guard dtor 后 `mov %ebx,%eax`（技39 EH-ebx）
