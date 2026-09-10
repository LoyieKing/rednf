int DisPatcher_SelectCharac::RequestBlackCount(CUser* user, MSG_CHARAC_SELECT& msg)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x1862), true);
    **guard << 0xb3;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_BLACK_COUNT);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_REQ_BLACK_COUNT* sig = guard->GetInBuffer<SIG_REQ_BLACK_COUNT>();
    memset(sig, 0, sizeof(SIG_REQ_BLACK_COUNT));
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
        return 1;
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 1;
    }
