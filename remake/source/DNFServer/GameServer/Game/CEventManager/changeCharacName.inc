void CEventManager::changeCharacName(CUser* user, int key, char* name, int flag)
{
    for (int i = 0; i <= 0xa5; ++i)
    {
        if (m_events[i] != 0)
        {
            if (m_events[i]->IsEventing(0))
                m_events[i]->changeCharacName(user, key, name, flag);
        }
    }
