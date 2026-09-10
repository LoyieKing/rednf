CSharedServerMessageManager::CSharedServerMessageManager(stSharedServerMessageInfo* pInfo)
{
    m_isSet = 0;
    m_adapter.SetServerMessageAddInfo(pInfo);
    m_event.clear();
