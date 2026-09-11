// ============================================================================
// DB_LoadQuest::dispatch @0x0840e236
// 基类 dispatch 通过 → BigStreamPool::Acquire("DBThread.cpp",0x29f9) →
// guard<<userIdx<<slot → GetInBuffer<SIG_LOAD_QUEST>（memset 0x7608、
// +0x4 清零）→ Stream::GetOutBuffer 首字（charac_no）拷入 →
// GetUserQuest 失败置 +0x4=1 → MsgQueueMgr::put(QUEUE_IDX 1)。返 1/0。
// ============================================================================
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
}