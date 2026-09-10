int CAssaultMgr::check_party_teleport_user(char slotno, CUser* srcUser,
                                           CUser* destUser, bool notifyErr)
{
    // [b23S3C] P0/P48 槽映射：ORIG errCode→-0x10、party→-0xc（第二块 party
    // 清零复用同槽，0x9d 直覆 errCode，无独立 errDest 局部）；每块 state 判定
    // && 短路双臂各自重取 GetPartyTelePort()+get_state()（4 call/块），bool
    // 物化岛由 && 形复现。4.1 实参 spill（slotno→-0x1c/notify→-0x20，尾段
    // movsbl/cmpb 槽读）为跨版本形态（具名复制局部实测产双槽+截断对，更差）。
    int errCode = 0;
    CParty* party = 0;

    if (srcUser->CheckInParty() != 0)
    {
        party = static_cast<CParty*>(srcUser->GetParty());
    }
    if (party != 0)
    {
        if (static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                    ->get_teleport_state() >= 0
            && static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                       ->get_teleport_state() <= 0x3)
        {
            errCode = 0x9c;
        }
    }
    party = 0;
    if (destUser->CheckInParty() != 0)
    {
        party = static_cast<CParty*>(destUser->GetParty());
    }
    if (party != 0)
    {
        if (static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                    ->get_teleport_state() >= 0
            && static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                       ->get_teleport_state() <= 0x3)
        {
            errCode = 0x9d;
        }
    }
    if (notifyErr && errCode != 0)
    {
        srcUser->SendCmdPeerErrorPacket(
            static_cast<ENUM_CMDPACKET>(0xa),
            static_cast<ENUM_PEER_REQUEST_TYPE>(static_cast<int>(slotno)),
            static_cast<unsigned char>(errCode));
    }
    return errCode;
