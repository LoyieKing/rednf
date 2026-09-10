void Arad_MileageProcess::BuySuccess(CUser* user, int a2, int a3, int a4, int a5,
                                     int a6, int a7, std::vector<Inven_Item>& items)
{
    user->UseMileage(a5);

    // b28c-28 判据（同 BuyFail）：ORIG 在 PacketGuard ctor 前发 `movl $0xb,-0x10` /
    // `movb $0x0,-0x9` 且 put_byte 从槽 `movsbl -0x9` 读 ⇒ 非 const 局部（const 被折叠成立即数）。
    int kResultCode = 0xb;
    char kGiftFlag = 0;
    PacketGuard packet;
    packet.put_header(1, 0x43);
    packet.put_byte(1);
    packet.put_byte((int)kGiftFlag);
    packet.put_int(kResultCode);
    packet.put_int(a2);
    packet.put_int(a3);
    packet.put_int(a6);
    packet.put_int(a4);
    packet.put_short((int)items.size());
    // P13：ORIG end() 在循环初始化区求值一次（end 临时 -0x14 + 转换构造 -0x2c）。
    for (std::vector<Inven_Item>::const_iterator it = items.begin(),
             endIt = items.end(); it != endIt; ++it)
    {
        packet.put_int(it->m_addInfo);    // ORIG put_int(it+2)
        packet.put_int(it->m_addInfo2);   // ORIG put_int(it+7)
    }
    if (a6 == a7)
    {
        user->SendCashData(false);
        PacketGuard itemPacket;
        itemPacket.put_header(0, 0xe);
        itemPacket.put_byte(0);
        itemPacket.put_short(2);
        // ORIG 0x0819e421/0x0819e44b：getCurCharacInvenR() 两次独立调用（P48，无缓存）。
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 0, itemPacket);
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 2, itemPacket);
        itemPacket.finalize(true);
        user->Send(itemPacket);
    }
    packet.finalize(true);
    user->Send(packet);
