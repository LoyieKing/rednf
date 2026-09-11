// ---------------------------------------------------------------------------
// DB_UpdateAvatarJewelSlot::makeRequest —— ORIG 0x0843081c 逐指令转写
// （报告 docs/class_func_reports/DB_UpdateAvatarJewelSlot/makeRequest.md）：
// StreamPool::Acquire("DBThread.cpp",0x7731) → CStreamGuard(stream,true)
//   → guard << 0x139 → guard << -1
//   → sig = guard.GetInBuffer<SIG_UPDATE_AVATAR_JEWEL_SOCKET>()
//   → sig->m_characNo/m_avatarKey = param_1/2；memcpy(&sig->m_emblem, emblem, 0x1e)
//   → MsgQueueMgr::put((QUEUE_IDX)2, guard)（析构时入队）。
void DB_UpdateAvatarJewelSlot::makeRequest(
    unsigned int characNo, unsigned int avatarKey,
    const stAvatarEmblemInfo_t* emblem)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7731), true);
    guard.operator*()->operator<<((int)0x139);
    guard.operator*()->operator<<((int)-1);
    SIG_UPDATE_AVATAR_JEWEL_SOCKET* sig =

        guard->GetInBuffer<SIG_UPDATE_AVATAR_JEWEL_SOCKET>();
    sig->m_characNo = characNo;
    sig->m_avatarKey = avatarKey;
    memcpy(sig->m_emblem, emblem, 0x1e);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}