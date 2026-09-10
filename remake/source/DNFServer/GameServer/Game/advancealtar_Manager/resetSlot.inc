int CharacAdvanceAltarManager::resetSlot(advancealtar::AdvanceAltarData& cur,
                                         const CMDPacketStruct::_SetSlotData& data)
{
    std::map<unsigned char, _Slot>* slots =
        cur.getSlotMap(static_cast<AdvanceAltarSlotType::T>(data.m_type));
    if (slots == 0)
    {
        return 3;
    }
    std::map<unsigned char, _Slot>::iterator it = slots->find(data.m_field4);
    if (it == slots->end())
    {
        return 2;
    }
    // [b28d-D33] ORIG 0x08132c39 `xor $1,%eax; test %al,%al; je` ⇒ 裸 !field 形
    // （P168a 档②；值域 {0,1}，与 !=1 语义等价）。
    if (!it->second.m_byte0)   // P222 反向包裹：ORIG body-fall、mov$2 尾块
    {
        it->second.m_int6 = 0;
        cur.m_update = 1;
        return 0;
    }
    return 2;                          // 锁定槽
