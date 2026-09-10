int Dispatcher_Compound_ExtreamItem::_push_newitem()
{
    // ORIG 0x08225b54：inven=getCurCharacInvenW(m_user) → insertItemIntoInventory(
    // m_resultItem, 0x30, 1, 1) → m_newSlot=返回值；返回 m_newSlot > -1
    CInventory* inven = m_user->getCurCharacInvenW();
    m_newSlot = (short)inven->insertItemIntoInventory(m_resultItem,
                                                      (eItemAddReason)0x30, true, true);
    return m_newSlot > -1 ? 1 : 0;
