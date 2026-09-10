bool CUser::SendEffectItemInfo(unsigned int itemIdx)
{
    PacketGuard packet;
    packet.put_header(0, 0xae);
    std::vector<ContinuousItemInfo>* list = GetCurCharacEffectItemList();
    unsigned char count = static_cast<unsigned char>(list->size());
    packet.put_byte(static_cast<int>(count));
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        // b28d-solo-W8_CU_G：ORIG 0x086593de/0x086593f5 两次 operator[] 各存
        // 具名局部（-0x10 idx / -0xc expire，P0 槽往返实证）⇒ 恢复具名局部。
        unsigned int idx = (*list)[i].m_itemIdx;
        unsigned int expire = (*list)[i].m_expireTime;
        packet.put_int(idx);
        packet.put_int(static_cast<int>(expire - itemIdx));
    }
    packet.finalize(true);
    return Send(packet);
