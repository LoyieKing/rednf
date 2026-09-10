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
