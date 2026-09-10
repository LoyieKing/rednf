int DisPatcher_SelectCharac::RequestDBKillMonsterInfo(CUser* user,

                                                      MSG_CHARAC_SELECT& msg)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x1843), true);
    **guard << 0x282;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_KILL_MONSTER);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_KILL_MONSTER_INFO* sig =
        guard->GetInBuffer<SIG_LOAD_KILL_MONSTER_INFO>();
    memset(sig, 0, sizeof(SIG_LOAD_KILL_MONSTER_INFO));
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
}
