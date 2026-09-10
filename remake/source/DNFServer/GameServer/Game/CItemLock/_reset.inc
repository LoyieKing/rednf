void CItemLock::_reset()
{
    m_lockMap.clear();
    m_lockList.clear();
    while (!m_lockStack.empty())
        m_lockStack.pop();
    m_lockTimer.Reset();
