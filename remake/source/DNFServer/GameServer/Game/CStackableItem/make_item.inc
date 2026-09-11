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
        t = ((CSystemTime*)0x941f714)->getCurSec() +
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