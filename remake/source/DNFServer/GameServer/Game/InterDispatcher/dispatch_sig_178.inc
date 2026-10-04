// [b28d-lane2] verdict=DIFF ndiff=156（ORIG 0x084da726）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=157（ORIG 0x84da726）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_NoticeServerMessageInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticeServerMessageInfoPktView* pPkt = reinterpret_cast<Inter_NoticeServerMessageInfoPktView*>(data);
    // [K3 2026-09-11] P0：ORIG 全程只用具名 pPkt 槽（-0x20），无第二个 data 槽（旧稿
    //   local_24 令各点多一次 `mov -0x24(%ebp),%eax`）⇒ 内联。
    CSharedServerMessageManager* local_20 =
        G_CGameManager()->GetSharedServerMessageManager();
    if (local_20 != 0)
    {
        // P183：ORIG 无 sourceChannel 具名 byte 槽（`movzbl %al,%ebx` 直取）⇒ 内联表达式。
        if (pPkt->m_channelNo != G_CEnvironment()->get_channel_no())
        {
            local_20->UpdateServerMessageByBroadcast(
                (int)pPkt->m_c,
                const_cast<char*>(pPkt->m_title),
                const_cast<char*>(pPkt->m_body),
                pPkt->m_5b);
        }
        cMyTrace(__PRETTY_FUNCTION__,
            0x42be, 0)("SERVER_MESSAGE : Recv ServerMessage from monitor server(source_channel:%d, kind:%d, index:%d, msg_len:%d, msg:%s, name_len:%d, name:%s, time:%d",
              (unsigned int)pPkt->m_channelNo,
              (unsigned int)pPkt->m_b,
              (unsigned int)pPkt->m_c,
              pPkt->m_titleLen,
              pPkt->m_title,
              pPkt->m_bodyLen,
              pPkt->m_body,
              pPkt->m_5b);
    }

    PacketGuard packet;
    packet.put_header(0, 0xc0);
    packet.put_byte(1);
    packet.put_byte(1);
    packet.put_byte(
        (unsigned int)pPkt->m_c);
    packet.put_int(pPkt->m_titleLen);
    packet.put_str(pPkt->m_title, pPkt->m_titleLen);
    packet.put_int(pPkt->m_bodyLen);
    packet.put_str(pPkt->m_body, pPkt->m_bodyLen);
    packet.finalize(true);
    G_GameWorld()->send_all_with_state(packet, 3);
    return 0;
}