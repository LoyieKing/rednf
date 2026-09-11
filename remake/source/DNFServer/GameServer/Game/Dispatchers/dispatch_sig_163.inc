int Dispatcher_UseCreatureEvolutionItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08228a76
    // ORIG cmp $0x3,get_state; setne → state!=3 恒返 0
    if (user->get_state() != 3)
    {
        return 0;
    }

    unsigned char evolveSlot = 0;   // local_11（-0xd）
    short itemCount = 0;            // local_14（-0x10）
    int itemIdx = 0;                // local_18（-0x14）

    if (!packet.get_byte(evolveSlot))
    {
        return LineFunc(0xe609,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(itemCount))
    {
        return LineFunc(0xe60a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(itemIdx))
    {
        return LineFunc(0xe60b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    int err = process(user, evolveSlot, itemCount, itemIdx);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x144, (unsigned char)err);
    }
    return 0;
}