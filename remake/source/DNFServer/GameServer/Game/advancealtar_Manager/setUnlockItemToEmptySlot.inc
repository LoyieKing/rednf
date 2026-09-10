void CharacAdvanceAltarManager::setUnlockItemToEmptySlot(AdvanceAltarShopType::T shopType,
                                                         int itemIdx)
{
    // [b28d-solo-advancealtar_Manager] ORIG +0x06 `test/je; cmp $1/je; jmp` 决策树
    // （case0 块 +0x17、case1 块 +0x20 直落）⇒ switch(slotType 具名槽 -0x14)。
    AdvanceAltarSlotType::T slotType;
    switch (shopType)
    {
    case AdvanceAltarShopType::T_0:
        slotType = AdvanceAltarSlotType::SLOT_N1;
        break;
    case AdvanceAltarShopType::T_1:
        slotType = AdvanceAltarSlotType::SLOT_N2;
        break;
    default:
        return;
    }
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return;
    }
    std::map<unsigned char, _Slot>* slots = cur->getSlotMap(slotType);
    if (slots == 0)
    {
        return;
    }
    // ORIG 每用点重取 operator->（+0x7b/+0x86/+0xa8 三次，P48 无引用缓存）；
    // 合并短路条件产 setne 物化岛（P133/P258）。
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); it++)
    {
        if (it->second.m_byte0 != 1 && it->second.m_int6 == 0)
        {
            it->second.m_int6 = itemIdx;
            cur->m_update = 1;
            return;
        }
    }
