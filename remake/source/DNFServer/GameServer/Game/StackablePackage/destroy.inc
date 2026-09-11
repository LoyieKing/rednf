// ORIG 0x08281a9a (0x104)：双层迭代 delete；无 clear()（map 由 Impl D1 清空）。
// b28d-lane25：P167 差集实证 ORIG 走 `_Rb_tree_iterator<...>::deEv`（operator*）
// 而非 ptEv（operator->）⇒ 源码用 (*it).second 形。
void EventProcessManager::destroy()
{
    for (std::map<EVENT_TYPE, std::map<int, EventProcessor*> >::iterator group =
             m_impl->processors.begin(); group != m_impl->processors.end(); ++group)
    {
        std::map<int, EventProcessor*>& inner = (*group).second;
        for (std::map<int, EventProcessor*>::iterator it = inner.begin();
             it != inner.end(); ++it)
        {
            EventProcessor* p = (*it).second;
            if (p != 0)
            {
                delete p;
                p = 0;
            }
        }
    }
}