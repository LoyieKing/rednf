int Inter_GuildJoinList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    char* const local_1c = data;

    PacketGuard packet;
    packet.put_header(1, 0x161);
    packet.put_byte(1);
    packet.put_int(*reinterpret_cast<int*>(local_1c));

    for (int local_18 = 0; local_18 < *reinterpret_cast<int*>(local_1c); ++local_18)
    {
        packet.put_int(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_4);
        const int local_14 = (int)strlen(local_1c + local_18 * 0x44 + 0x08);
        packet.put_int(local_14);
        packet.put_str(local_1c + local_18 * 0x44 + 0x08, local_14);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_26);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_27);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_29);
        packet.put_byte(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_28);
        const int local_10 = (int)strlen(local_1c + local_18 * 0x44 + 0x2a);
        packet.put_int(local_10);
        packet.put_str(local_1c + local_18 * 0x44 + 0x2a, local_10);
        packet.put_int(((Inter_GuildJoinListEntryWinView*)(local_1c + local_18 * 0x44))->m_44);
    }

    packet.finalize(true);
    user->Send(packet);
    return 0;
