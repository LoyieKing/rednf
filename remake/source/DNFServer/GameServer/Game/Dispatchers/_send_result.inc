int Dispatcher_Compound_ExtreamItem::_send_result()
{
    PacketGuard guard;                                           // ORIG local_1c
    InterfacePacketBuf& pkt = *(InterfacePacketBuf*)&guard;
    pkt.put_header(1, 0x146);
    pkt.put_byte(1);
    for (int i = 0; i < 2; ++i)                                  // ORIG local_10 < 2
    {
        pkt.put_byte(0);
        pkt.put_short((int)(unsigned short)m_slots[i]);          // +0x08/+0x0a
        pkt.put_int(1);
    }
    pkt.put_byte(0);
    pkt.put_short((int)(unsigned short)m_field4);                // +0x04
    pkt.put_int(1);
    pkt.put_short((int)m_newSlot);                               // +0x06（short 符号扩展）
    pkt.put_int(m_resultItem.m_addInfo);                         // +0x92（m_resultItem+0x02）
    pkt.put_int(m_resultItem.m_addInfo2);                        // +0x97（m_resultItem+0x07）
    pkt.put_byte((int)m_resultItem.GetItemAttr() & 0xff);        // m_resultItem（+0x90）
    pkt.put_short((int)(unsigned short)m_resultItem.m_fieldb);   // +0x9b（endurance）
    pkt.put_byte((int)(unsigned char)m_resultItem.m_field0);     // m_resultItem+0x00
    if (!G_GameWorld()->IsEnchantRevisionChannel())
    {
        pkt.put_int(m_resultItem.m_fieldd);                      // +0x9d（m_resultItem+0x0d）
    }
    else
    {
        pkt.put_int(0);
    }
    pkt.put_byte((int)m_resultItem.m_amp.getAbilityType() & 0xff);       // +0xa1（m_amp）
    pkt.put_short((int)m_resultItem.m_amp.getAbilityValue() & 0xffff);   // getAbilityValue
    pkt.put_packet(m_resultItem);
    pkt.finalize(true);
    m_user->Send(guard);                                         // m_user（+0xd0）
    return 0;                                                    // ORIG 尾部未设 eax，按调用方口径返 0
