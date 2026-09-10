void CDelivery::notify(int type, DeliveryMsg& msg)
{
    if (!checkAddress(type))
        return;
    std::map<int, std::list<CEventBase*> >::iterator it = m_eventMap.find(type);
    if (it != m_eventMap.end())
    {
        BOOST_FOREACH(CEventBase*& e, it->second)
        {
            if (e != 0)
                e->delivery(msg);
        }
    }
