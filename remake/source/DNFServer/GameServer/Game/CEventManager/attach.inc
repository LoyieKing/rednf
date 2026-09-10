void CDelivery::attach(int type, CEventBase* event)
{
    if (!checkAddress(type))
        return;
    std::map<int, std::list<CEventBase*> >::iterator it = m_eventMap.find(type);
    if (it != m_eventMap.end())
    {
        BOOST_FOREACH(CEventBase*& e, it->second)
        {
            if (e == event)
                return;
        }
    }
    m_eventMap[type].push_back(event);
