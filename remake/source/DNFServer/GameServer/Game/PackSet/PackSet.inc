PackSet::PackSet()      // C1/C2 @0x08ac47f4：ctor vectors 与 clear()
    : m_indexes(), m_addeds()
{
    // 对齐 ORIG：构造两个空 vector 后调 clear() 重置全部字段
    clear();
