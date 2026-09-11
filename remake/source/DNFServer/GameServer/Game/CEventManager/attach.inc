// b28d-40: BOOST_FOREACH 复现 + 正向 `if (it != end)` 包裹（P220）；ORIG 0x081163fa，AE。
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
}