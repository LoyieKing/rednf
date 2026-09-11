// ORIG 0x8541a3e：清空锁表 / 待发列表，抽干空闲号池，复位计时器。
void CItemLock::_reset()
{
    m_lockMap.clear();
    m_lockList.clear();
    while (!m_lockStack.empty())
        m_lockStack.pop();
    m_lockTimer.Reset();
}