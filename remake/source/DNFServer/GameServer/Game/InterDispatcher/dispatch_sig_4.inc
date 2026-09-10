int Inter_DeleteCharac::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0x812;
    if (!user->unlockDeleteCharac())                   // xor $1；!=1 → 拒绝
        return 0x817;

    // [b28d-lane2] ORIG 084bf9bb 段：data spill 在两道守卫之后（`mov 0x10(%ebp),%eax;
    // mov %eax,-0xc(%ebp)`），声明位置与 ORIG 一致。
    Inter_DeleteCharacPktView* pPkt = reinterpret_cast<Inter_DeleteCharacPktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 6);

    if (pPkt->m_8 == 1)
    {   // 正在删除保护期
        packet.put_byte(0);
        packet.put_byte(2);
        packet.finalize(true);
        user->Send(packet);
        return 0x825;
    }
    else if (pPkt->m_8 == 2)
    {
        packet.put_byte(0);
        packet.put_byte(0x24);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }
    else if (pPkt->m_8 == 3)
    {
        packet.put_byte(0);
        packet.put_byte(0xd5);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    Packet_Monitor_Charac_Delete delMsg;               // ORIG C1 0x084e9318 W
    delMsg.m_accId = (unsigned int)user->get_acc_id();
    delMsg.m_charNo = pPkt->m_4;
    delMsg.m_guildKey = pPkt->m_guildKey;

    // [b28d-lane2] ORIG 084bfa2e 段：GetServerProxy 结果直落 (%esp)（无具名 proxy 槽，
    // P183/P0）——旧具名 monitor/guild 各占 4B 槽并多发 2×2 条 mov（-0x10/-0xc 双槽）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup())->SendTcpPacket((char*)&delMsg, 0x16);
    if (delMsg.m_guildKey != 0)
    {
        GlobalData::s_guild_proxy_mgr->GetServerProxy(
            user->GetServerGroup())->SendTcpPacket((char*)&delMsg, 0x16);
    }

    user->DeleteCharac(pPkt->m_4);
    packet.put_byte(1);
    // 块序：ORIG 084bfa67 `test %al; je <else 块>` + then（setFlag+put_byte(1)）直落
    //   ⇒ 正极性 `if (flag != 0) {then} else {else}`（P59/P214① 反写）。
    // [lane2 2026-09-10] 原 `if (flag == 0) {…} else {…}` 产 4.4 反向块序
    //   （jne <then> + else 直落）；极性反写后 jcc 目标下标与 ORIG 逐位一致。
    if (pPkt->m_eventCharacFlag != 0)
    {
        user->setUserEventCharacterFlag(0);
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
    }
    packet.finalize(true);
    user->Send(packet);

    packet.clear();                                    // ORIG InterfacePacketBuf::clear
    packet.put_header(0, 2);
    packet.put_byte(2);
    user->make_basic_info((char*)&packet, 2);
    packet.finalize(true);
    user->Send(packet);
    user->UpdateCharacView();
    return 0;
