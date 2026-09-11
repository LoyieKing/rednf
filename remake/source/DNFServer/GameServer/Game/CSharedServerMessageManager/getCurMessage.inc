stSERVER_MESSAGE* CSharedServerMessageManager::getCurMessage(int nIndex)
{
    std::vector<stSERVER_MESSAGE>::iterator it = m_event.m_vecMsg.begin();
    while (it != m_event.m_vecMsg.end())
    {
        stSERVER_MESSAGE* p = &(*it);
        if (p->m_index == nIndex)
            return p;
        it++;
    }
    return 0;
}