// ORIG 0x0828186c (0x15a)：isStackablePackageItem 失败 → 0；外层 find/end 比较；
// 内层 std::find_if(begin, end, FindControllableEventProcessor(itemId))；
// == end → 0；否则 (*it).second->Process(user, itemId)（vtable+0xc 虚调用）。
// b28d-lane25：P167 差集实证 ORIG 走 deEv（operator*），源码全部改 (*it).second 形。
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
}