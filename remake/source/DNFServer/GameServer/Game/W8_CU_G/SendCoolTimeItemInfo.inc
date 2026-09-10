bool CUser::SendCoolTimeItemInfo(unsigned int nowMs)
{
    PacketGuard packet;
    packet.put_header(0, 0xac);
    std::vector<ContinuousItemInfo>* list = GetCurCharacCoolTimeItemList();
    // b28d-solo-W8_CU_G：ORIG 0x0865923a begin() 先于 size()/put_byte（P0 槽序
    // 与求值序判据：begin 的 sret 直接落 it 槽 -0x30，无默认 ctor）。
    std::vector<ContinuousItemInfo>::iterator it = list->begin();
    unsigned char count = static_cast<unsigned char>(list->size());
    packet.put_byte(static_cast<int>(count));
    for (; it != list->end(); it++)   // b28d-21：P167 差集 ORIG 为 ppEi（后置，
                                    // P28 增量侧推论），改 it++。
    {
        // b28d-solo-W8_CU_G：ORIG 0x0865927d/0x0865928d 两次 operator-> 结果
        // 各存具名局部（-0x10 idx / -0xc expire，P0 槽往返实证）。
        unsigned int idx = it->m_itemIdx;
        unsigned int expire = it->m_expireTime;
        packet.put_int(idx);
        packet.put_int(static_cast<int>(expire - nowMs));
    }
    packet.finalize(true);
    return Send(packet);
