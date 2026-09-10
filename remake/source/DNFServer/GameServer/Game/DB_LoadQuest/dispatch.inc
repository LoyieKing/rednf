bool DB_LoadQuest::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return 0;
    }

    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp",
                                                              0x29f9), true);
    **guard << userIdx;
    **guard << slot;
    SIG_LOAD_QUEST* sig = guard->GetInBuffer<SIG_LOAD_QUEST>();
    memset(sig, 0, sizeof(SIG_LOAD_QUEST));
    sig->m_field04 = 0;
    sig->m_characNo = stream->GetOutBuffer<SIG_LOAD_QUEST>()->m_characNo;
    if (!GetUserQuest(sig))
    {
        sig->m_field04 = 1;
    }
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    return 1;
