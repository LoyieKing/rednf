void CharacAdvanceAltarManager::setDefaultSlot()
{
    // [b28d-solo-advancealtar_Manager] ORIG：while 形（前置 jmp + 尾 cmpl/setle/test/
    // jne，P25/P51）；`== end` 正极性 if/else（je 到 m_int6=0 块）；每次重读
    // m_currentAdvanceAltarData（无 cur/slots 引用槽，P183）；键数组直接下标
    // （kUnitSlotNoLock[k]，无 static 指针局部）；字段序 byte0/int1/int6/byte5；
    // make_pair 首参取 slot.m_byte5 地址（ORIG lea +0x5）。
    if (m_currentAdvanceAltarData == 0)
    {
        return;
    }
    int k = 0;
    while (k <= 5)
    {
        std::map<unsigned char, _Slot>::iterator it =
            m_currentAdvanceAltarData->m_slots1.find(static_cast<unsigned char>(k));
        if (it == m_currentAdvanceAltarData->m_slots1.end())
        {
            _Slot slot;                                  // ORIG _Slot ctor memset
            slot.m_byte0 = kUnitSlotNoLock[k];
            slot.m_int1 = static_cast<SlotInt1_t>(1);    // 槽类型标记（SLOT_N1）
            slot.m_int6 = 0;
            slot.m_byte5 = static_cast<unsigned char>(k);
            m_currentAdvanceAltarData->m_slots1.insert(
                std::make_pair(slot.m_byte5, slot));
        }
        else
        {
            it->second.m_int6 = 0;
        }
        k++;
    }
    int j = 0;
    while (j <= 5)
    {
        std::map<unsigned char, _Slot>::iterator it =
            m_currentAdvanceAltarData->m_slots2.find(static_cast<unsigned char>(j));
        if (it == m_currentAdvanceAltarData->m_slots2.end())
        {
            _Slot slot;
            slot.m_byte0 = kSkillSlotNoLock[j];
            slot.m_int1 = static_cast<SlotInt1_t>(2);    // 槽类型标记（SLOT_N2）
            slot.m_int6 = 0;
            slot.m_byte5 = static_cast<unsigned char>(j);
            m_currentAdvanceAltarData->m_slots2.insert(
                std::make_pair(slot.m_byte5, slot));
        }
        else
        {
            it->second.m_int6 = 0;
        }
        j++;
    }
