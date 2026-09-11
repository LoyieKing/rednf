bool CTitleBook::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("TitleBook.cpp", 0x9c), true);
    **guard << 0x285;
    **guard << (int)pUser->GetUID();
    stTitleBook_Save* pData = guard->GetInBuffer<stTitleBook_Save>();
    if (pData == 0)
        return 0;
    {
        std::memset(pData, 0, 0x6b3e);
        pData->m_characNo = pUser->get_charac_no(-1);
        std::memcpy(pData->m_data, m_book.m_category0, 0x10ae);
        std::memcpy(pData->m_data + 0x10ae, m_book.m_category1,
                    0x2fa8);
        std::memcpy(pData->m_data + 0x4056, m_book.m_category2,
                    0x1c98);
        std::memcpy(pData->m_data + 0x5cee, m_book.m_category3,
                    0xe4c);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
}