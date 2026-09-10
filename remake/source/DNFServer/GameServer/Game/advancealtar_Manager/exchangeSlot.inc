unsigned int CharacAdvanceAltarManager::exchangeSlot(CMDPacketStruct::_SetSlotData& slotA,
                                            CMDPacketStruct::_SetSlotData& slotB)
{
    if (slotA.m_type != slotB.m_type)
    {
        return 2;
    }
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return 3;
    }
    std::map<unsigned char, _Slot>* slots =
        cur->getSlotMap(static_cast<AdvanceAltarSlotType::T>(slotA.m_type));
    if (slots == 0)
    {
        return 4;
    }

    // [b28d-solo-advancealtar_Manager] ORIG 形（0x08131f67..）：
    //   iterator foundA = slots->begin();  // begin 直构到 -0x34（无 end 初值）
    //   for (; foundA != slots->end(); foundA++)
    //       if (三条件合并) break;         // 无 foundA=it 记录（it 即 foundA）
    // 每用点重取 operator->（P48/P157，三处 ptEv 各自 call）。
    std::map<unsigned char, _Slot>::iterator foundA = slots->begin();
    for (; foundA != slots->end(); foundA++)
    {
        // [b28d-solo-advancealtar_Manager] ORIG 单段 `mov 0x2(%eax),%edx`：
        // m_int1i 为 _Slot 匿名 union 的 int 视角（同型比较，P235/P248 档③）。
        if (foundA->second.m_byte5 == static_cast<unsigned char>(slotA.m_field4) &&
            foundA->second.m_int1i == slotA.m_type &&
            foundA->second.m_int6 == slotA.m_field8)
        {
            break;
        }
    }
    std::map<unsigned char, _Slot>::iterator foundB = slots->begin();
    for (; foundB != slots->end(); foundB++)
    {
        if (foundB->second.m_byte5 == static_cast<unsigned char>(slotB.m_field4) &&
            foundB->second.m_int1i == slotB.m_type &&
            foundB->second.m_int6 == slotB.m_field8)
        {
            break;
        }
    }
    if (foundA == slots->end() || foundB == slots->end())
    {
        return 3;
    }
    if (foundA->second.m_byte0 || foundB->second.m_byte0)
    {
        return 3;                         // 任一处于锁定态不可交换
    }

    // [b28d-solo-advancealtar_Manager] ORIG +0x2f1/+0x305 直接写两侧
    // m_field8（匹配时 foundX->m_int6 == slotX.m_field8，语义等价），
    // 外层冗余守卫 `if (foundA!=end && foundB!=end)` 照抄（P171）。
    if (foundA != slots->end() && foundB != slots->end())
    {
        foundA->second.m_int6 = slotB.m_field8;
        foundB->second.m_int6 = slotA.m_field8;
    }
    cur->m_update = 1;
    return 0;
