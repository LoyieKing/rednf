int DisPatcher_AddFriend::process_packet_jpn(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stAddFriendParam& out = static_cast<stAddFriendParam&>(param);
    // [b23S3C] ORIG 槽集 {-0x10 req, -0x14 out, -0x1c guard, -0xc sig}；
    // kFile/kPretty 各用点均字符串立即数（无 static 局部指针槽）；
    // Acquire 结果直落 ctor 0x4(%esp)（无 stream 局部，P0 寄存器直传）；
    // 写流为 operator* + operator<< 两 call 链（+0x0c7/+0x0ef 各一次），
    // GetInBuffer 走 operator-> 链（+0x10c/+0x114，P103）——GmCmds_C
    // CDeleteGuild DONE 同族形。

    out.m_field4 = check_error(user, msg);
    if (out.m_field4 > 0)
    {
        return 0;
    }
    if (out.m_field4 < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp",
            "int DisPatcher_AddFriend::process_packet_jpn(CUser*, MSG_BASE&, ParamBase&)",
            0x20d8, "Dispatcher_Impl Check_Error No(%d)", out.m_field4);
        return 0x20d9;
    }

    MSG_ADD_FRIEND& req = *reinterpret_cast<MSG_ADD_FRIEND*>(&msg);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x20ed),
        true);
    (*guard)->operator<<(0x33e);                 // inter-server SIG 消息号
    (*guard)->operator<<(static_cast<int>(user->GetUID()));
    SIG_CHECK_ADD_FRIEND* sig =
        guard.operator->()->GetInBuffer<SIG_CHECK_ADD_FRIEND>();
    if (sig == 0)
    {
        out.m_field4 = 1;
        // [b28d-solo-DisPatcherCore] ORIG +0x12c `movl $0x1,0x4(%eax); mov $0x0,%ebx;
        // jmp <尾块>`（返回值物化到 %ebx 后跳共享尾析构块）⇒ if 块内是独立 return 0。
        return 0;
    }
    else
    {
        // ORIG +0x153 分支岛 mov$0/mov$1（非 setne 归一），直落 sig+0x04。
        sig->m_notGmFlag =
            GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) ? 0 : 1;
        strncpy(sig->m_characName,                               // sig+0x05..0x22
                reinterpret_cast<STFriendReqView*>(&req)->m_name, // msg+0x11
                0x1e);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);    // QUEUE_IDX 2=DB_Q
    }
    return 0;
