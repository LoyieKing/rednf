void CExpertJobMgr::OnResponsePeer(CUser* requester, CUser* peerTarget,
                                   CUser* jobOwnerA, CUser* jobOwnerB,
                                   int cookie, ENUM_PEER_REQUEST_TYPE type,
                                   PacketGuard& packet)
{
    // ORIG 0x849e368：err 预清槽（movl $0x0,-0x10）+ 独立赋值（P197-③ 两句式）；
    // 0x849e39d setne %al 判定物化（4.4 恒折槽比零，P197-④/N-88）。
    unsigned int err = 0;
    err = IsAvailableRequestPeer(requester, peerTarget, type, packet);
    // b28d-31：P225 while+break 复现 4.1 槽比零 setne 物化（ORIG 0x0849e39d）。
    while (err != 0)
    {
        requester->SendCmdPeerErrorPacket((ENUM_CMDPACKET)0xb, type,
                                          (unsigned char)err, packet);
        return;
    }

    CExpertJob* enchanter = AcquireExpertJob((ENUM_EXPERT_JOB_TYPE)1);
    if (enchanter == 0)
    {
        requester->SendCmdPeerErrorPacket((ENUM_CMDPACKET)0xb, type, 0x16, packet);
        return;
    }

    // b28d-solo 试形（P277）：派生 cast 前置到调用点 ⇒ 复现 ORIG
    // 0x0849e43b 段 this-first 装载（`mov -0xc,%eax; mov 0x20,%edx; …`）。
    enchanter->OnStartMission(jobOwnerA, jobOwnerB);   // ORIG vptr+0 业务槽 0
    static_cast<CEnchanter*>(enchanter)->SetPeerRequestType(type);

    // b28d-solo 真语义修正（P171 照抄）：ORIG 0x0849e44d 首包前**无** clear，
    // 唯一 clear 在首包 Send 之后（0x0849e4ce）⇒ 删除多余的一次 clear()。
    packet.put_header(0, 8);
    packet.put_short((int)(requester->get_unique_id() & 0xffff));
    packet.put_byte(type);
    packet.put_int(cookie & 0xffffffff);
    packet.finalize(true);
    peerTarget->Send(packet);

    packet.clear();
    packet.put_header(1, 0xb);
    packet.put_byte(1);
    packet.put_short((int)(peerTarget->get_unique_id() & 0xffff));
    packet.put_byte(type);
    packet.finalize(true);
    requester->Send(packet);
