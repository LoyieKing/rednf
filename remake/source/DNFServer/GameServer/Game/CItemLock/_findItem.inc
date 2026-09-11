// ORIG 0x8542e12：m_lockList 中存在指定 lock 号的引用 → 1，否则 0。
// [b28d-41] 返回类型 int→bool：ORIG VerifyItemLock 调用点 `call; xor $0x1,%eax;
// test %al,%al; je` 是 bool callee 裸形（P248 档①），int 返回恒产 sete 归一。
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
}