unsigned int CUser::UseCeraPoint(unsigned int amount)
{
    // ORIG 0x08692b25：`cmp 0xc(%ebp),%eax; jb 8692b40` —— fall-through=减法块在前，
    // 置零块为 jb 跳入的 else 岛（技巧17：源码首分支=fall 块）
    if (m_ceraPoint >= amount)
    {
        m_ceraPoint = m_ceraPoint - amount;
    }
    else
    {
        m_ceraPoint = 0;
    }
    return m_ceraPoint;
