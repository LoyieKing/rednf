bool Dispatcher_PurifyItem::processPurifyType(int purifyType, Inven_Item& item)
{
    if (purifyType == 0)
    {
        return item.m_amp.identify();   // ORIG 0x082194cb：call 后落 %eax 直返
    }
    if (purifyType == 1)
    {
        item.m_amp.reset();
        return true;
    }
    return false;
