int EventProcessManager::Process(EVENT_TYPE type, CUser& user, int itemId)
{
    if (!isStackablePackageItem(itemId))
        return 0;
    std::map<EVENT_TYPE, std::map<int, EventProcessor*> >::iterator group =
        m_impl->processors.find(type);
    if (m_impl->processors.end() == group)
        return 0;
    std::map<int, EventProcessor*>& inner = (*group).second;
    std::map<int, EventProcessor*>::iterator it =
        std::find_if(inner.begin(), inner.end(),
                     FindControllableEventProcessor(itemId));
    if (inner.end() == it)
        return 0;
    return (*it).second->Process(user, itemId);
