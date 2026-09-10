int DisPatcher_SelectCharac::RequestDBDungeonClear(CUser* user, MSG_CHARAC_SELECT& msg)

{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x17f8), true);
    **guard << 12;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_DUNGEON_CLEAR);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_DUNGEON_CLEAR* sig = guard->GetInBuffer<SIG_LOAD_DUNGEON_CLEAR>();
    memset(sig, 0, sizeof(SIG_LOAD_DUNGEON_CLEAR));
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_accId = user->get_acc_id();
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
