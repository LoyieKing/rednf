bool CItemLock::_findItem(unsigned char lock)
{
    for (std::vector<stItemLockRef>::iterator it = m_lockList.begin();
         it != m_lockList.end(); ++it)
    {
        // ORIG 0x8542e12 段 `mov %eax,-0xc; mov -0xc,%eax; movzbl (%eax)`：
        // operator* 结果指针落槽回读（P0 有槽往返 ⇒ 原码具名引用）。
        stItemLockRef& ref = *it;
        if (ref.m_lock == lock)
            return true;
    }
    return false;
