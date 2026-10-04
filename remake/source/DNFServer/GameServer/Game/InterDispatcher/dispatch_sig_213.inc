int Inter_GuildBoardDelete::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildBoardDeletePktView* pPkt = reinterpret_cast<Inter_GuildBoardDeletePktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    // ORIG +0x53：`mov %eax,%edx`+`mov -0xc(%ebp),%eax`+`mov 0x13(%eax),%eax` 两步 spill
    // ＝unsigned 字段与 get_charac_no(int) 失配；显式 `(int)` 会把失配折平 ⇒ 去掉转换。
    if (user->get_charac_no(-1) != pPkt->m_characNo)        // recv charId @ msg+0x13
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x157);
    if (pPkt->m_result == 0)
        packet.put_byte(1);
    else
    {
        packet.put_byte(0);
        packet.put_short(
            static_cast<int>(static_cast<unsigned short>(pPkt->m_result)));
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}