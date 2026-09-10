CMap::CMap()
{
    // ORIG 0834dcda：容器构造后清 this+0(m_bossCount) 与 this+0x7c(m_assignedNpcCount)。
    // 写在 body 使其位于容器构造之后（匹配 ORIG movl $0,(%eax)/movl $0,0x7c(%eax) 顺序）。
    m_bossCount = 0;
    m_assignedNpcCount = 0;
