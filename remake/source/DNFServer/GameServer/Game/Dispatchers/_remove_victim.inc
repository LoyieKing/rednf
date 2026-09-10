int Dispatcher_Compound_ExtreamItem::_remove_victim()
{
    // ORIG 0x08225a88：先删 m_slots[0..1]（0x2a 原因），任一失败返 0；
    // 再删 m_field4，成功返 1
    CInventory* inven = m_user->getCurCharacInvenW();
    // ORIG 0x08225afa：unsigned 计数 + cmpl $1/setbe（<= 1 直写）。
    for (unsigned int i = 0; i <= 1; ++i)
    {
        // ORIG 0x08225ae8：bool callee 取反 xor$1;test;je（技巧1）。
        if (!inven->delete_item((INVEN_TYPE)1, m_slots[i], 1,
                                (eItemDelReason)0x2a, true))
        {
            return 0;
        }
    }
    // ORIG 0x08225b3e-5b4c：xor1;test;je→mov$1 岛 / fall mov$0 = if(!b){return 0;} return 1;
    if (!inven->delete_item((INVEN_TYPE)1, m_field4, 1, (eItemDelReason)0x2a, true))
    {
        return 0;
    }
    return 1;
