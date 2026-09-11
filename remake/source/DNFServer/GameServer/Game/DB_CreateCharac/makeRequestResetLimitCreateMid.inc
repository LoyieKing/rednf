// ===========================================================================
// makeRequestResetLimitCreateMid — 报告 @0x08401864
// channel_no==1 时经内部包请求重置中端限建表（==1 布局，匹配 ORIG sete 形式）。
// ===========================================================================
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
}