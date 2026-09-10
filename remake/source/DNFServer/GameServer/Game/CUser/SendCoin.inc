void CUser::SendCoin()
{
    if (getCurCharacR() == 0)
        return;
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    // ORIG 0x865689e xor $0x1,%eax; 0x86568a3 jne cleanup：bool 返回取反 + 空 if（probe 实证 f1 形）
    if (!getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 1, packet))
    {
    }
    else
    {
        packet.finalize(true);
        Send(packet);
    }
