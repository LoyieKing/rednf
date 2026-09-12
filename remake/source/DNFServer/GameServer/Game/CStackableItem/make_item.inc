void CStackableItem::make_item(Inven_Item& item) const
{
    switch (m_nItemType)
    {
    case 8:
        item.m_field1 = 4;
        break;
    case 1:
        item.m_field1 = 3;
        break;
    case 9:
    case 10:
        item.m_field1 = 7;
        break;
    case 0x14:
        item.m_field1 = 9;
        break;
    case 2:
        item.m_field1 = 10;
        break;
    default:
        item.m_field1 = 2;
        break;
    }
    item.m_fieldb = 0;
    int t = 0;
    if (getUsablePeriod() != 0)
    {
        // [R18scanPH 2026-09-13] 原为硬编码 ORIG 绝对地址 ((CSystemTime*)0x941f714)
        // ——该地址在 ORIG 是 GlobalData::s_systemTime_（0x941f714，全库 508 引用），
        // 但在本二进制布局下 s_systemTime_ 位于 0x917e54c，0x941f714 超出全部段
        // （RW LOAD 止于 0x929db84）⇒ 原写法读未映射内存（崩溃/脏读）。
        // 与 R17fixST 三件同口径：改用权威对象 GlobalData::s_systemTime_。
        t = GlobalData::s_systemTime_.getCurSec() +
            getUsablePeriod() * 0x15180;
    }
    else if (getExpirationDate() != 0)
    {
        t = getExpirationDate();
    }
    if (t != 0)
    {
        item.m_fieldb = (unsigned short)((t - 0x44a54a80) / 0x15180);
    }
    item.ResetItemAttr();
    item.m_fieldd = 0;
    ((itemGloballyUniqueIdentifier_t*)((char*)&item + 0x15))->reset();
    item.m_amp.reset();
    if (GetAttachType() == 7)
    {
        item.SetTradeLimitCount((unsigned char)getTradeLimitCount());
    }
}