bool CItemDictionary::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemDictionary.cpp", 0x18), true);
    **guard << 0x2b2;
    **guard << (int)user->GetUID();
    SIG_ITEM_DICTIONARY_FLAG* sig =
        guard->GetInBuffer<SIG_ITEM_DICTIONARY_FLAG>();
    std::memset(sig, 0, 0x138c);
    sig->m_characNo = user->get_charac_no(-1);
    std::memcpy(sig->m_data, m_data, kBinaryLength);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
