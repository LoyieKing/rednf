void CExpertJobMgr::OnRequestPeer(CUser* requester, CUser* target,
                                  ENUM_PEER_REQUEST_TYPE type,
                                  PacketGuard& packet)
{
    unsigned int err = 0;
    err = IsAvailableRequestPeer(requester, target, type, packet);
    // b28：清 register asm（N96/P0 全域禁）——直测短路条件。
    // b28d-31：P225 while+break 复现 4.1 槽比零 setne 物化
    //（ORIG 0x0849e26b `cmpl $0,-0xc(%ebp); setne %al; test %al,%al; je`）。
    while (err != 0)
    {
        requester->SendCmdPeerErrorPacket((ENUM_CMDPACKET)10, type,
                                          (unsigned char)err, packet);
        return;
    }
    packet.clear();
    packet.put_header(0, 7);
    packet.put_short((int)(requester->get_unique_id() & 0xffff));
    packet.put_byte(type);
    packet.put_int((int)(target->get_unique_id() & 0xffffffff));
    packet.finalize(true);
    target->Send(packet);
