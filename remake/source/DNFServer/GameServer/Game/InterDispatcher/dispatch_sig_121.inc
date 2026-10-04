int Inter_DBCheckDoubleGuildAddress::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_DBCheckDoubleGuildAddressPktView* pPkt = reinterpret_cast<Inter_DBCheckDoubleGuildAddressPktView*>(data);

    if (pPkt->m_result == '\x02')
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xa0, 0x6f);
    }
    // ORIG：同 DBCheckDoubleGuildName 形态（else 支内 PacketGuard，汇合后统一 return）。
    else
    {
        user->setGuildCreateFlag(GUILD_ADDRESS_DOUBLE_CHECK);
        PacketGuard packet;
        packet.put_header(1, 0xa0);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}