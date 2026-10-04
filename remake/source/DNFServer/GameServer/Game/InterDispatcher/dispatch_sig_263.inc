// [b28d-lane2] verdict=DIFF ndiff=196（ORIG 0x084e3c60）：槽序/帧差(N82/P212/P221)+混合残差；gap+54。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=184（ORIG 0x84e3c60）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_GuildJoinList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    char* const local_1c = data;

    PacketGuard packet;
    packet.put_header(1, 0x161);
    packet.put_byte(1);
    Inter_GuildJoinListHdrView* hdr = reinterpret_cast<Inter_GuildJoinListHdrView*>(local_1c);
    packet.put_int(hdr->m_count);

    for (int local_18 = 0; local_18 < hdr->m_count; ++local_18)
    {
        Inter_GuildJoinListEntryWinView* e =
            reinterpret_cast<Inter_GuildJoinListEntryWinView*>(local_1c + local_18 * 0x44);
        packet.put_int(e->m_4);
        const int local_14 = (int)strlen(e->m_name);
        packet.put_int(local_14);
        packet.put_str(e->m_name, local_14);
        packet.put_byte(e->m_26);
        packet.put_byte(e->m_27);
        packet.put_byte(e->m_29);
        packet.put_byte(e->m_28);
        const int local_10 = (int)strlen(e->m_note);
        packet.put_int(local_10);
        packet.put_str(e->m_note, local_10);
        packet.put_int(e->m_44);
    }

    packet.finalize(true);
    user->Send(packet);
    return 0;
}