int Inter_NoticeDeleteGuildAgit::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_NoticeDeleteGuildAgitPktView* pPkt = reinterpret_cast<Inter_NoticeDeleteGuildAgitPktView*>(data);
    if (pPkt->m_16 != 0)   // msg+0x16 错误码
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xe8,
                                 pPkt->m_16);
        return 0;
    }

    user->SetGuildAgitFlag(false);

    if (pPkt->m_12 ==               // msg+0x12 == 本人
        user->get_charac_no(-1))
    {
        PacketGuard packet;                                   // asm local_28
        packet.put_header(1, 0xe8);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }

    PacketGuard notice;                                       // asm local_1c
    notice.put_header(0, 0xbe);
    notice.finalize(true);
    user->Send(notice);

    // b28c-l23 P0 清绑：ORIG 0x084dac90.. guildkey 跨 call 直落 %ebx；具名 proxy 槽
    // 会顶深 pPkt 至 -0x10（ORIG pPkt=-0xc）。绑定违 P0 已删，裸 register 保留。
    register unsigned int guildKey =
        static_cast<unsigned int>(user->get_charac_guildkey());
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        user->GetServerGroup())->ChangeGuildAgitFlagInProxy(guildKey, 0);
    return 0;
