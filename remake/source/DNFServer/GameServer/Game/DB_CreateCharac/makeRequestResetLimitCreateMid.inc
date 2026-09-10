void DB_CreateCharac::makeRequestResetLimitCreateMid()
{
    if (G_CEnvironment()->get_channel_no() == 1)
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xee3),
            true);
        ARAD::DISPATCHER::make_internal_stream_jpn(guard,
                                                   ARAD::ENUM_INTERNALPACKET_JPN(0x37),
                                                   -1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
