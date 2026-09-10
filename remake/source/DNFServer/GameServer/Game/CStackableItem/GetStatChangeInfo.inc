StatChangeInfo CStackableItem::GetStatChangeInfo()
{
    // 直接 reinterpret 拷贝：不调用 StatChangeInfo ctor，生成 ORIG 的 8 字节整拷。
    return *reinterpret_cast<StatChangeInfo*>(&m_stat2dc);
