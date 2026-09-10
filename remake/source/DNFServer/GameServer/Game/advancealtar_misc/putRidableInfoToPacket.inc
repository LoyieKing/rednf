void putRidableInfoToPacket(PacketGuard& packet, const AdvanceAltarData& data)
{
    // b28d-35: DIFF→N82（350/349 差 1 条）。ORIG @0x08141781 首条载 0xc(%ebp)，
    //   我方装载序差 1 条（b28d 已 288→1，P183 去缓存后残余为实参装载序）。
    // ORIG 每处直接 `mov 0x8(%ebp)` 传参（无 buf 引用槽）⇒ 不用具名别名。
    // 段 2..6 容器指针 = ORIG 二段初始化槽（`movl $0,-0x10/-0xc` + 赋值，
    // 0x0814184e/0x0814185b 等）；段 1/7 直取（0x081417a3/0x08141bb7）。
    packet.put_binary(reinterpret_cast<char*>(const_cast<AdvanceAltarData*>(&data)), 0x21);   // [objdump 0x8141798]

    // +0x24 stages（put_binary 8 字节，0x081417fe）
    packet.put_int((int)data.m_stages.size());                           // [0x081417ac/0x081417bd]
    for (std::map<int, _Stage>::const_iterator it = data.m_stages.begin();
         it != data.m_stages.end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Stage*>(&it->second)), sizeof(_Stage));

    // +0x3c / +0x54 / +0x6c 三张 buyItem 表（各 0xc，0x081418b3/0x0814195e/
    // 0x08141a09），局部引用形：ORIG 每轮 `lea 0xXX(%eax)` 重取容器地址。
    std::map<int, _BuyItem> const* pBuy = 0;
    pBuy = &data.m_buyItems1;
    packet.put_int((int)pBuy->size());                                   // [0x08141864]
    for (std::map<int, _BuyItem>::const_iterator it = pBuy->begin();
         it != pBuy->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_BuyItem*>(&it->second)), sizeof(_BuyItem));

    pBuy = &data.m_buyItems2;
    packet.put_int((int)pBuy->size());                                   // [0x0814190f]
    for (std::map<int, _BuyItem>::const_iterator it = pBuy->begin();
         it != pBuy->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_BuyItem*>(&it->second)), sizeof(_BuyItem));

    pBuy = &data.m_buyItems3;
    packet.put_int((int)pBuy->size());                                   // [0x081419ba]
    for (std::map<int, _BuyItem>::const_iterator it = pBuy->begin();
         it != pBuy->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_BuyItem*>(&it->second)), sizeof(_BuyItem));

    // +0x84 / +0x9c slot 表（key uchar，value 偏移 +1，len 10，
    // 0x08141abd/0x08141b6a）
    std::map<unsigned char, _Slot> const* pSlot = 0;
    pSlot = &data.m_slots1;
    packet.put_int((int)pSlot->size());                                  // [0x08141875]
    for (std::map<unsigned char, _Slot>::const_iterator it = pSlot->begin();
         it != pSlot->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Slot*>(&it->second)), sizeof(_Slot));

    pSlot = &data.m_slots2;
    packet.put_int((int)pSlot->size());                                  // [0x08141920]
    for (std::map<unsigned char, _Slot>::const_iterator it = pSlot->begin();
         it != pSlot->end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Slot*>(&it->second)), sizeof(_Slot));

    // +0xb4 rewards（5 字节，0x08141c17）
    packet.put_int((int)data.m_rewards.size());                          // [0x08141bc2]
    for (std::map<int, _Reward>::const_iterator it = data.m_rewards.begin();
         it != data.m_rewards.end(); it++)
        packet.put_binary(reinterpret_cast<char*>(
            const_cast<_Reward*>(&it->second)), sizeof(_Reward));
